#include <SFML/Window.hpp>
#ifdef __APPLE__
#include <OpenGL/OpenGL.h>
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif
#include <cstdlib>
#include <ctime>
#include <sstream>
#include <vector>
#include "text3d.h"
#include "game.h"
#include "dirs.h"

typedef void (*TimerFunc)(int elapsed);

struct TimeContainer {
    clock_t timeStarted;
    clock_t timeExpected;
    TimerFunc function;
};

//The number of milliseconds between calls to update
const int TIMER_MS = 25;
sf::Window App;
Game game;
std::vector<TimeContainer> timeFuncs;
bool needDraw = false;

//The width of the terrain in units, after scaling
const float TERRAIN_WIDTH = 50.0f;

void cleanup() {
    t3dCleanup();
}

void handleSpecialKeypress() {
    game.setCameraRotate(Left * App.GetInput().IsKeyDown(sf::Key::Left) + Right * App.GetInput().IsKeyDown(sf::Key::Right));

    game.setBallDirection(Left * App.GetInput().IsKeyDown(sf::Key::Q) + Right * App.GetInput().IsKeyDown(sf::Key::D),
                          Left * App.GetInput().IsKeyDown(sf::Key::Z) + Right * App.GetInput().IsKeyDown(sf::Key::S));
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
    gluPerspective(45.0, (float)w / (float)h, 1.f, 300.f);
}

template<class T>
void drawVal (const std::string &desc, T val) {
    std::ostringstream oss;
    oss << desc << val;
    std::string str = oss.str();

    glDisable(GL_TEXTURE_2D);
    glDisable(GL_LIGHTING);
    glColor3f(1.0f, 1.0f, 0.0f);
    glPushMatrix();
    glTranslatef(0.0f, 1.7f, -5.0f);
    glScalef(0.2f, 0.2f, 0.2f);
    t3dDraw2D(str, 0, 0);
    glPopMatrix();
    glEnable(GL_LIGHTING);
}

void drawScene() {
    game.draw();
    App.Display();
}

void timerFunc(int ms, TimerFunc func) {
    TimeContainer cont;
    cont.timeStarted = clock();
    cont.timeExpected = cont.timeStarted + (ms*CLOCKS_PER_SEC/1000);
    cont.function = func;

    for (int i = timeFuncs.size(); i >= 0; i--) {
        if (i == 0 || cont.timeExpected > timeFuncs[i-1].timeExpected) {
            timeFuncs.insert(timeFuncs.begin() + i, cont);
            break;
        }
    }
}

void dealWithTimers() {
    while (!timeFuncs.empty() && timeFuncs.front().timeExpected < clock()) {
        TimeContainer &ref =  timeFuncs.front();
        ref.function((ref.timeExpected - ref.timeStarted)*1000/CLOCKS_PER_SEC);
        timeFuncs.erase(timeFuncs.begin(), timeFuncs.begin()+1);
    }
}

void pauseTimers() {
    if (timeFuncs.empty()) {
        sf::Sleep(0.001f);
    } else {
        TimeContainer &ref = timeFuncs.front();
        sf::Sleep(float(ref.timeExpected - clock())/CLOCKS_PER_SEC);
    }
}

void update(int time) {
    game.update(time);
    postRedisplay();
    timerFunc(TIMER_MS, update);
}

int main(int argc, char** argv) {
    (void) argc;
    (void) argv;

    srand((unsigned int)time(0)); //Seed the random number generator

    App.Create(sf::VideoMode(800, 600, 32), "Penguins Craft");
    App.UseVerticalSync(false);
    handleResize(800, 600);

    game.loadTerrain("db/maps/heightmap.png", 30.f, TERRAIN_WIDTH);

    initRendering();
    timerFunc(TIMER_MS, update);

    while (App.IsOpened()) {
        sf::Event Event;
        while (App.GetEvent(Event))
        {
            if (Event.Type == sf::Event::Closed || ((Event.Type == sf::Event::KeyPressed) && (Event.Key.Code == sf::Key::Escape))) {
                App.Close();
            }
            if (Event.Type == sf::Event::Resized) {
                handleResize(Event.Size.Width, Event.Size.Height);
            }
            if (Event.Type == sf::Event::KeyPressed || Event.Type == sf::Event::KeyReleased) {
                handleSpecialKeypress();
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
