#include <cfloat>
#ifdef __APPLE__
#include <OpenGL/OpenGL.h>
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

#include "ball.h"
#include "terrain.h"

Ball::Ball(float radius)
{
    this->radius = radius;
    this->x = this->z = this->y = radius;
    this->terrain = NULL;
}

void Ball::setTerrain(Terrain *terrain)
{
    this->terrain = terrain;
}

void Ball::checkPos()
{
    if (x < radius) {
        x = radius + FLT_EPSILON;
    }

    if (z < radius) {
        z  = radius + FLT_EPSILON;
    }

    if (terrain) {
        if (x > terrain->width()-radius) {
            x = terrain->width()-radius - FLT_EPSILON;
        }
        if (z > terrain->length()-radius) {
            z = terrain->length()-radius - FLT_EPSILON;
        }
    }
}

void Ball::draw()
{
    checkPos();

    if (terrain) {
        y = terrain->heightAt(x, z) + radius;
    } else {
        y = radius;
    }

    glColor3f(0.3f, 0.3f, 0.9f);
    glPushMatrix();
    glTranslatef(x, y, z);
    glutSolidSphere(radius, 15, 15);
    glPopMatrix();
}

void Ball::advance(float xdiff, float zdiff)
{
    x += xdiff;
    z += zdiff;

    checkPos();
}
