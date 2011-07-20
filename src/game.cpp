#include <SFML/Window.hpp>

#include "game.h"
#include "mathconst.h"
#include "terrain.h"
#include "ball.h"
#include "dirs.h"

Game::Game() {
    cameraAngle = 0.f;
    xDir = zDir = 0;
    cameraRotateDirection = Center;
    terrain = NULL;
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

    glTranslatef(0, 0, -1.0f * terrain->width());
    glRotatef(30, 1, 0, 0);
    glRotatef(cameraAngle, 0, 1, 0);
    glTranslatef(-terrain->width() / 2, 0, -terrain->width() / 2);

    GLfloat ambientColor[] = {0.4f, 0.4f, 0.4f, 1.0f};
    glLightModelfv(GL_LIGHT_MODEL_AMBIENT, ambientColor);

    GLfloat lightColor0[] = {0.6f, 0.6f, 0.6f, 1.0f};
    GLfloat lightPos0[] = {-0.5f, 0.8f, 0.1f, 0.0f};
    glLightfv(GL_LIGHT0, GL_DIFFUSE, lightColor0);
    glLightfv(GL_LIGHT0, GL_POSITION, lightPos0);

    terrain->draw();
    ball.draw();
}

void Game::setBallDirection(int horDirection, int vertDirection)
{
    xDir = horDirection;
    zDir = vertDirection;
}

void Game::setCameraRotate(int direction)
{
    cameraRotateDirection = direction;
}

void Game::update(int time)
{
    cameraAngle += time * .08f * cameraRotateDirection;

    if (xDir || zDir) {
        float div = 1.0f;

        if (xDir && zDir) {
            div = SQRT2INV;
        }

        div *= time * 0.01f;

        float sina = sin(cameraAngle*PI/180);
        float cosa = cos(cameraAngle*PI/180);
        ball.advance( (xDir * cosa - zDir * sina)*div, (zDir * cosa + xDir * sina)*div );
    }
}
