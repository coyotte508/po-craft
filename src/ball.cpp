#include <cfloat>

#include "engine.h"
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
    glPushMatrix();
    glTranslatef(x(), y(), z());

    glColor3f(0.9f, 0.3f, 0.3f);
    Engine::drawHemisphere(radius);
    glColor3f(1.f, 1.f, 1.f);
    Engine::drawHemisphere(-radius);
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
