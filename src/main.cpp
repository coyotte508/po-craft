#include <SDL/SDL.h>
#include <SFML/Graphics.hpp>
#ifdef main
#undef main
#endif
#include <QSettings>
#ifdef __APPLE__
#include <OpenGL/OpenGL.h>
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif
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

typedef void (*TimerFunc)(int elapsed);

struct TimeContainer {
    Uint32 timeStarted;
    Uint32 timeExpected;
    TimerFunc function;
};

//The number of milliseconds between calls to update
const int TIMER_MS = 25;
Game game;
std::vector<TimeContainer> timeFuncs;
bool needDraw = false;
sf::RenderWindow App;
//The width of the terrain in units, after scaling
const float TERRAIN_WIDTH = 100.0f;
Controller controller;
Menu menu(App, controller);

void cleanup() {
    t3dCleanup();
}

void handleSpecialKeypress() {
    game.setCameraRotate(Left * App.GetInput().IsKeyDown(sf::Key::Up)
                            + Right * App.GetInput().IsKeyDown(sf::Key::Down),
                         Left * App.GetInput().IsKeyDown(sf::Key::Left)
                            + Right * App.GetInput().IsKeyDown(sf::Key::Right));
    game.setBallDirection(Left * App.GetInput().IsKeyDown(controller.getKey(Controller::MoveCharLeft))
                            + Right * App.GetInput().IsKeyDown(controller.getKey(Controller::MoveCharRight)),
                         Left * App.GetInput().IsKeyDown(controller.getKey(Controller::MoveCharUp))
                            + Right * App.GetInput().IsKeyDown(controller.getKey(Controller::MoveCharDown)));
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
    glShadeModel(GL_SMOOTH);

    t3dInit();
}

void handleResize(int w, int h) {
    glViewport(0, 0, w, h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(45.0, (float)w / (float)h, 0.1f, 300.f);
}

void drawScene() {
    game.draw();
    if (menu.running()) {
        menu.draw();
    }

    App.Display();
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
        Uint32 ticks = SDL_GetTicks();
        if (ticks < ref.timeExpected) {
            SDL_Delay(ref.timeExpected-SDL_GetTicks());
        }
    }
}

void update(int time) {
    game.update(time);
    postRedisplay();
    timerFunc(TIMER_MS, update);
}

int main(int argc, char** argv) {
    srand((unsigned int)time(0)); //Seed the random number generator

    glutInit(&argc, argv);

    SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER);

    App.Create(sf::VideoMode(800, 600, 32), "Penguins Craft");

    handleResize(800, 600);

    game.loadTerrain("db/maps/heightmap.png", 30.f, TERRAIN_WIDTH);

    initRendering();
    timerFunc(TIMER_MS, update);

    drawScene();

    bool running = true;
    while (running) {
        sf::Event Event;
        while (App.GetEvent(Event)) {
            if (Event.Type == Event.Closed) {
                running = false;
                break;
            }
            if (Event.Type == Event.KeyPressed) {
                if (menu.running()) {
                    menu.handleKeyPress(Event.Key);
                    continue;
                }
                if (Event.Key.Code == sf::Key::Escape) {
                    running = false;
                    break;
                }
                if (Event.Key.Code == controller.getKey(Controller::AlternateCamera)) {
                    game.alternateCameraMode();
                    continue;
                }
                if (Event.Key.Code == sf::Key::F2) {
                    menu.start();
                    continue;
                }
            }
            if (Event.Type == Event.Resized) {
                handleResize(Event.Size.Width, Event.Size.Height);
                continue;
            }
            if (Event.Type == Event.KeyPressed || Event.Type == Event.KeyReleased) {
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
