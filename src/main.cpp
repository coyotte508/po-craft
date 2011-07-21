#include <SDL/SDL.h>
#ifdef main
#undef main
#endif
#include <QImage>
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
Uint8 *keystate = SDL_GetKeyState(NULL);
bool needDraw = false;
char xvel(0), yvel(0);
//The width of the terrain in units, after scaling
const float TERRAIN_WIDTH = 50.0f;

void cleanup() {
    t3dCleanup();
}

void handleSpecialKeypress() {
    game.setCameraRotate(Left * keystate[SDLK_LEFT] + Right * keystate[SDLK_RIGHT]);
    game.setBallDirection(Left * keystate[SDLK_a] + Right * keystate[SDLK_d],
                          Left * keystate[SDLK_w] + Right * keystate[SDLK_s]);
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
    gluPerspective(45.0, (float)w / (float)h, 0.5f, 300.f);
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
    SDL_GL_SwapBuffers();
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
    SDL_WM_SetCaption("Penguins Craft", "Penguins Craft");
    {
        QImage image("db/icon.png");
        image = image.convertToFormat(QImage::Format_ARGB32);
        SDL_Surface *icon = SDL_CreateRGBSurfaceFrom(image.bits(), image.width(), image.height(),
                                                     image.depth(), image.bytesPerLine(), 0x00FF0000,
                                                     0x0000FF00, 0x000000FF, 0xFF000000);
        SDL_WM_SetIcon(icon, NULL);
    }

    SDL_SetVideoMode(800, 600, 32, SDL_HWSURFACE|SDL_OPENGL|SDL_GL_DOUBLEBUFFER);

    handleResize(800, 600);

    game.loadTerrain("db/maps/heightmap.png", 30.f, TERRAIN_WIDTH);

    initRendering();
    timerFunc(TIMER_MS, update);

    drawScene();

    bool running = true;
    while (running) {
        SDL_Event event;
        while (SDL_PollEvent(&event))
        {
            if (event.type == SDL_QUIT) {
                running = false;
                continue;
            }
            if (event.type == SDL_KEYDOWN) {
                if (event.key.keysym.sym == SDLK_r) {
                    game.alternateCameraMode();
                    continue;
                }
                if (event.key.keysym.sym == SDLK_ESCAPE) {
                    running = false;
                    continue;
                }
            }
            if (event.type == SDL_VIDEORESIZE) {
                handleResize(event.resize.w, event.resize.h);
                continue;
            }
            if (event.type == SDL_KEYDOWN || event.type == SDL_KEYUP) {
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
