#ifdef __APPLE__
#include <OpenGL/OpenGL.h>
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

#include <cstdlib>
#include <ctime>
#include <sstream>
#include "text3d.h"
#include "terrain.h"
#include "ball.h"

//The number of milliseconds between calls to update
const int TIMER_MS = 25;
Terrain *terrain = NULL;
Ball ball;
float _angle = 0;
bool isRightKeyPressed = false, isLeftKeyPressed = false;

enum Dir {
    Left = -1,
    Center = 0,
    Right = 1
};

Dir angleDir;

//The width of the terrain in units, after scaling
const float TERRAIN_WIDTH = 50.0f;

void cleanup() {
    t3dCleanup();
    delete terrain, terrain = NULL;
}

void handleKeypress(unsigned char key, int, int) {
    switch (key) {
        case 27: //Escape key
            cleanup();
            exit(0);
    }
}

void handleSpecialKeypress(int key, int, int) {
    switch (key) {
        case GLUT_KEY_LEFT:
            isLeftKeyPressed = true;
            if (isRightKeyPressed) {
                angleDir = Center;
            }
            else {
                angleDir = Left;
            }
            break;
        case GLUT_KEY_RIGHT:
            isRightKeyPressed = true;
            if (isLeftKeyPressed) {
                angleDir = Center;
            }
            else {
                angleDir = Right;
            }
            break;
    }
}

void handleSpecialKeyReleased(int key, int, int) {
    switch (key) {
        case GLUT_KEY_LEFT:
            isLeftKeyPressed = false;
            if (isRightKeyPressed) {
                angleDir = Right;
            }
            else {
                angleDir = Center;
            }
            break;
        case GLUT_KEY_RIGHT:
            isRightKeyPressed = false;
            if (isLeftKeyPressed) {
                angleDir = Left;
            }
            else {
                angleDir = Center;
            }
            break;
    }
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
    gluPerspective(45.0, (float)w / (float)h, 1.0, 200.0);
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
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    glTranslatef(0, 0, -1.0f * TERRAIN_WIDTH);
    glRotatef(30, 1, 0, 0);
    glRotatef(_angle, 0, 1, 0);
    glTranslatef(-TERRAIN_WIDTH / 2, 0, -TERRAIN_WIDTH / 2);

    GLfloat ambientColor[] = {0.4f, 0.4f, 0.4f, 1.0f};
    glLightModelfv(GL_LIGHT_MODEL_AMBIENT, ambientColor);

    GLfloat lightColor0[] = {0.6f, 0.6f, 0.6f, 1.0f};
    GLfloat lightPos0[] = {-0.5f, 0.8f, 0.1f, 0.0f};
    glLightfv(GL_LIGHT0, GL_DIFFUSE, lightColor0);
    glLightfv(GL_LIGHT0, GL_POSITION, lightPos0);

    //Draw the terrain
    terrain->draw();
    //And the ball!
    ball.draw();

    glutSwapBuffers();
}

void update(int) {
    glutPostRedisplay();
    glutTimerFunc(TIMER_MS, update, 0);

    _angle += angleDir * 50.f * (float)TIMER_MS / 1000;
}

int main(int argc, char** argv) {
    srand((unsigned int)time(0)); //Seed the random number generator

    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(600, 600);

    terrain = Terrain::loadTerrain("db/maps/heightmap.png", 30.f, TERRAIN_WIDTH);
    ball.setTerrain(terrain);

    glutCreateWindow("Penguins Craft");
    initRendering();

    glutDisplayFunc(drawScene);
    glutKeyboardFunc(handleKeypress);
    glutReshapeFunc(handleResize);
    glutTimerFunc(TIMER_MS, update, 0);
    glutSpecialFunc(handleSpecialKeypress);
    glutSpecialUpFunc(handleSpecialKeyReleased);

    glutMainLoop();
    return 0;
}
