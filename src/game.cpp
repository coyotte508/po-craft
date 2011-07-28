#ifdef __APPLE__
#include <OpenGL/OpenGL.h>
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

#include "game.h"
#include "mathconst.h"
#include "terrain.h"
#include "ball.h"
#include "dirs.h"

Game::Game() {
    xDir = zDir = 0;
    cameraZoomDirection = cameraRotateDirectionX = cameraRotateDirectionY = Center;
    terrain = NULL;
    cameraMode = BehindCharacter;
}

Game::~Game() {
    delete terrain, terrain = NULL;
}

void Game::loadTerrain(const std::string &filename, float height, float width) {
    terrain = Terrain::loadTerrain(filename, height, width);
    ball.setTerrain(terrain);
}

void Game::draw() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    if (!terrain) {
        return;
    }

    if (cameraMode == OnTerrain) {
        camera.project(*terrain);
    } else if (cameraMode == OnCharacter){
        camera.project(ball);
    } else if (cameraMode == BehindCharacter) {
        camera.projectBehind(ball);
    }

    GLfloat ambientColor[] = {0.4f, 0.4f, 0.4f, 1.0f};
    glLightModelfv(GL_LIGHT_MODEL_AMBIENT, ambientColor);

    GLfloat lightColor0[] = {0.6f, 0.6f, 0.6f, 1.0f};
    GLfloat lightPos0[] = {-0.5f, 0.8f, 0.1f, 0.0f};
    glLightfv(GL_LIGHT0, GL_DIFFUSE, lightColor0);
    glLightfv(GL_LIGHT0, GL_POSITION, lightPos0);

    terrain->draw();

    if (cameraMode != OnCharacter) {
        ball.draw();
    }
}

void Game::setBallDirection(int horDirection, int vertDirection)
{
    xDir = horDirection;
    zDir = vertDirection;
}

void Game::setCameraRotate(int x, int y)
{
    cameraRotateDirectionX = x;
    cameraRotateDirectionY = y;
}

void Game::setCameraZoom(int directionForward) {
    cameraZoomDirection = directionForward;
}

void Game::alternateCameraMode()
{
    cameraMode += 1;

    if (cameraMode == LastCameraMode) {
        cameraMode = 0;
    }
}

void Game::update(int time)
{
    camera.rotate(time * .08f * cameraRotateDirectionX, time * .08f * cameraRotateDirectionY);
    camera.zoom(time * .03f * cameraZoomDirection);

    if (xDir || zDir) {
        float div = 1.0f;

        if (xDir && zDir) {
            div = SQRT2INV;
        }

        div *= time * 0.007f;

        float sina = sin(camera.angleY*PI/180);
        float cosa = cos(camera.angleY*PI/180);
        ball.advance( (xDir * cosa - zDir * sina)*div, (zDir * cosa + xDir * sina)*div );
    }
}
