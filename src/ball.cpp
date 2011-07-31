#include <cfloat>

#include "engine.h"
#include "ball.h"
#include "terrain.h"
#include "vec3f.h"
#include "mathconst.h"

Ball::Ball(float radius) : pos(radius, radius, radius), angleInit(false)
{
    this->radius = radius;
    this->terrain = NULL;

    maxVelocity = 0.06;
    maxVelocityS = maxVelocity*maxVelocity;
    friction = 0.2;
    weight = .00005;

    std::fill(rotationMatrix, rotationMatrix+16, 0);
    rotationMatrix[0] = rotationMatrix[5] = rotationMatrix[10] = rotationMatrix[15] = 1;
}

void Ball::setTerrain(Terrain *terrain)
{
    this->terrain = terrain;

    advance(0);
    angleInit = true;
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
    glMultMatrixf(rotationMatrix);
    /* Red */
    glColor3f(0.9f, 0.3f, 0.3f);
    Engine::drawHemisphere(radius);
    /* White */
    glColor3f(1.f, 1.f, 1.f);
    Engine::drawHemisphere(-radius);
    glPopMatrix();
}

void Ball::setAcceleration(float xacc, float zacc) {
    accX() = xacc;
    accZ() = zacc;
}

void Ball::advance(int time)
{
    Vec3f oldpos = pos;
    float v = velocity.magnitude();

    /* Friction resistance */
    if (v > FLT_EPSILON) {
        float resX = - weight * friction * velX()/v;
        float resZ = - weight * friction * velZ()/v;

        bool sigx = velX() > 0;
        bool sigz = velZ() > 0;

        velX() += resX*time;
        velZ() += resZ*time;

        /* If the friction caused the item to change direction,
          instead it stops */
        if (sigx != velX() > 0) {
            velX() = 0;
        }

        if (sigz != velZ() > 0) {
            velZ() = 0;
        }

        v = velocity.magnitude();
    }

    /* Only use own acceleration if we're not too fast */
    if (v < maxVelocity) {
        velX() += accX()*time;
        velZ() += accZ()*time;

        if (velocity.magnitudeSquared() > maxVelocityS) {
            velocity = velocity * (maxVelocity/velocity.magnitude());
        }
    }

    x() += velX()*time;
    z() += velZ()*time;

    checkPos();

    if (terrain) {
        y() = terrain->heightAt(x(), z(), radius) + radius;
    } else {
        y() = radius;
    }

    if (angleInit) {
        /* Rotates the ball according to the difference in positions */
        Vec3f dynamic = Vec3f(x()-oldpos[0], 0, z()-oldpos[2]).cross(Vec3f(0, 1, 0));
        if (dynamic.magnitudeSquared() < FLT_EPSILON) {
            return;
        }
        dynamic = dynamic.normalize();
        float ampl = oldpos.dist(pos)/radius;

        glMatrixMode(GL_MODELVIEW);
        glPushMatrix();
        glLoadIdentity();
        glRotatef(ampl*RTOD, -dynamic[0], -dynamic[1], -dynamic[2]);
        glMultMatrixf(rotationMatrix);
        glGetFloatv(GL_MODELVIEW_MATRIX, rotationMatrix);
        glPopMatrix();
    }
}
