#ifdef __APPLE__
#include <OpenGL/OpenGL.h>
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif
#include <cfloat>

#include "ball.h"
#include "terrain.h"
#include "vec3f.h"

Ball::Ball(float radius) : pos(radius, radius, radius)
{
    this->radius = radius;
    this->terrain = NULL;
}

void Ball::setTerrain(Terrain *terrain)
{
    this->terrain = terrain;

    advance(0, 0);
}

void Ball::checkPos()
{
    if (x() < radius) {
        x() = radius + FLT_EPSILON;
    }

    if (z() < radius) {
        z()  = radius + FLT_EPSILON;
    }

    if (terrain) {
        if (x() > terrain->width()-radius) {
            x() = terrain->width()-radius - FLT_EPSILON;
        }
        if (z() > terrain->length()-radius) {
            z() = terrain->length()-radius - FLT_EPSILON;
        }
    }
}

void Ball::draw()
{
    glColor3f(0.3f, 0.3f, 0.9f);
    glPushMatrix();
    glTranslatef(x(), y(), z());
    glutSolidSphere(radius, 15, 15);
    glPopMatrix();
}

void Ball::advance(float xdiff, float zdiff)
{
    x() += xdiff;
    z() += zdiff;

    checkPos();

    if (terrain) {
        y() = terrain->heightAt(x(), z(), radius) + radius;
    } else {
        y() = radius;
    }
}
