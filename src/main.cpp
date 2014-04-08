#include <SFML/Graphics.hpp>
#include <QSettings>
#include "glu.h"

#include <cstdlib>
#include <cstdio>
#include <sstream>
#include <ctime>
#include <vector>
#include "text3d.h"
#include "game.h"
#include "dirs.h"
#include "menu.h"
#include "controller.h"
#include "debug-val.h"

typedef void (*TimerFunc)(int elapsed);

struct TimeContainer {
    quint32 timeStarted;
    quint32 timeExpected;
    TimerFunc function;
};

//The number of milliseconds between calls to update
const int TIMER_MS = 25;

std::vector<TimeContainer> timeFuncs;
bool needDraw = false;

//The width of the terrain in units, after scaling
const float TERRAIN_WIDTH = 100.0f;

namespace {
    struct globalObjects {
        globalObjects() : menu(window, controller) {
        }

        Controller controller;
        sf::RenderWindow window;
        Menu menu;
        Game game;
    };
    globalObjects *gb = nullptr;
}

void cleanup() {
    t3dCleanup();
    delete gb, gb = nullptr;
}

quint32 SDL_GetTicks()
{
    static sf::Clock clock;
    return clock.getElapsedTime().asMilliseconds();
}

void SDL_Delay(int ms)
{
    sf::sleep(sf::milliseconds(ms));
}

void handleSpecialKeypress() {
    gb->game.setCameraRotate(Left * sf::Keyboard::isKeyPressed(sf::Keyboard::Up)
                            + Right * sf::Keyboard::isKeyPressed(sf::Keyboard::Down),
                         Left * sf::Keyboard::isKeyPressed(sf::Keyboard::Left)
                            + Right * sf::Keyboard::isKeyPressed(sf::Keyboard::Right));
    gb->game.setCameraZoom(Left * sf::Keyboard::isKeyPressed(gb->controller.getKey(Controller::CameraZoomOut))
                            + Right * sf::Keyboard::isKeyPressed(gb->controller.getKey(Controller::CameraZoomIn)));
    gb->game.setBallDirection(Left * sf::Keyboard::isKeyPressed(gb->controller.getKey(Controller::MoveCharLeft))
                            + Right * sf::Keyboard::isKeyPressed(gb->controller.getKey(Controller::MoveCharRight)),
                         Left * sf::Keyboard::isKeyPressed(gb->controller.getKey(Controller::MoveCharUp))
                            + Right * sf::Keyboard::isKeyPressed(gb->controller.getKey(Controller::MoveCharDown)));
}

void postRedisplay() {
    needDraw = true;
}

void initRendering() {
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glEnable(GL_NORMALIZE);
    glEnable(GL_COLOR_MATERIAL);
    glClearColor(0, 191.f/255, 1.f, 1.f);
    glShadeModel(GL_SMOOTH);

    t3dInit();
}

void handleResize(int w, int h) {
    glViewport(0, 0, w, h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(45.0, (float)w / (float)h, 0.1f, 300.f);
}

template<class T>
void drawVal (const std::string &desc, T val) {
    if (!DebugVal::active) {
        return;
    }
    std::ostringstream oss;
//    oss << desc << val << ", distance: " << DebugVal::dis << ", radius: " << DebugVal::dis2 << ", p: " << DebugVal::p
//        << ", diff: " << DebugVal::debug2 << "\nInside: " << DebugVal::insideS << ", Radius2: " << DebugVal::radius2
//        << ", iniheight: " << DebugVal::iniHeight << ", intheight: " << DebugVal::intHeight <<
//           ", endheight: " << DebugVal::endHeight << ",pint: " << DebugVal::pInt;
    oss << "Heights: " << DebugVal::iniHeight << ", " << DebugVal::intHeight << ", " << DebugVal::endHeight << ", Normal: " << DebugVal::debug;
    std::string str = oss.str();

    sf::String s(str);
    gb->window.draw(sf::Text(s, sf::Font(), 12));
//    glDisable(GL_TEXTURE_2D);
//    glDisable(GL_LIGHTING);
//    glColor3f(1.0f, 1.0f, 0.0f);
//    glPushMatrix();
//    glTranslatef(0.0f, 1.7f, -5.0f);
//    glScalef(0.2f, 0.2f, 0.2f);
//    t3dDraw2D(str, 0, 0);
//    glPopMatrix();
//    glEnable(GL_LIGHTING);
}

void drawScene() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    gb->game.draw();
    gb->window.pushGLStates();
    if (gb->menu.running()) {
        gb->menu.draw();
    }
    gb->window.popGLStates();
    gb->window.display();
}

void timerFunc(int ms, TimerFunc func) {
    TimeContainer cont;
    cont.timeStarted = SDL_GetTicks();
    cont.timeExpected = cont.timeStarted + ms;
    cont.function = func;

    for (int i = timeFuncs.size(); i >= 0; i--) {
        if (i == 0 || cont.timeExpected > timeFuncs[i-1].timeExpected) {
            timeFuncs.insert(timeFuncs.begin() + i, cont);
            break;
        }
    }
}

void dealWithTimers() {
    while (!timeFuncs.empty() && timeFuncs.front().timeExpected < SDL_GetTicks()) {
        TimeContainer &ref =  timeFuncs.front();
        ref.function(ref.timeExpected - ref.timeStarted);
        timeFuncs.erase(timeFuncs.begin(), timeFuncs.begin()+1);
    }
}

void pauseTimers() {
    if (timeFuncs.empty()) {
        SDL_Delay(1);
    } else {
        TimeContainer &ref = timeFuncs.front();
        quint32 ticks = SDL_GetTicks();
        if (ticks < ref.timeExpected) {
            SDL_Delay(ref.timeExpected-SDL_GetTicks());
        }
    }
}

void update(int time) {
    gb->game.update(time);
    postRedisplay();
    timerFunc(TIMER_MS, update);
}

int main(int argc, char** argv) {
    (void) argc;
    (void) argv;

    srand((unsigned int)time(0)); //Seed the random number generator

    gb = new globalObjects();

    gb->window.create(sf::VideoMode(800, 600, 32), "Penguins Craft");
    sf::Image icon;
    icon.loadFromFile("db/icon.png");
    gb->window.setIcon(icon.getSize().x, icon.getSize().y, icon.getPixelsPtr());

    handleResize(800, 600);

    gb->game.loadTerrain("db/maps/heightmap.png", 15.f, TERRAIN_WIDTH);

    initRendering();
    timerFunc(TIMER_MS, update);

    drawScene();

    bool running = true;
    while (running) {
        sf::Event event;
        while (gb->window.pollEvent(event)) {
            if (event.type == event.Closed) {
                running = false;
                break;
            }
            if (event.type == event.KeyPressed) {
                if (gb->menu.running()) {
                    gb->menu.handleKeyPress(event.key);
                    continue;
                }
                if (event.key.code == gb->controller.getKey(Controller::AlternateCamera)) {
                    gb->game.alternateCameraMode();
                    continue;
                }
                if (event.key.code == sf::Keyboard::F2 || event.key.code == sf::Keyboard::Escape) {
                    gb->menu.start();
                    continue;
                }
            }
            if (event.type == event.Resized) {
                handleResize(event.size.width, event.size.height);
                continue;
            }
            if (event.type == event.KeyPressed || event.type == event.KeyReleased) {
                handleSpecialKeypress();
                continue;
            }
        }
        dealWithTimers();
        if (needDraw) {
            drawScene();
            needDraw = false;
        }
        pauseTimers();
    }

    cleanup();

    return EXIT_SUCCESS;
}
