#ifndef BALL_H
#define BALL_H

#include "vec3f.h"
#include "macro.h"

class Terrain;

class Ball {
public:
    Ball(float radius = 0.4f);

    void setTerrain(Terrain *terrain);
    void draw();

    /* Reposition the ball inside the terrain if its out */
    void checkPos();
    /* advances the ball */
    void advance(int time);
    void setAcceleration(float xacc, float zacc);

    float radius;
    Vec3f pos;
    Vec3f velocity;
    Vec3f acceleration;

    SETVEC3FCOORDS(pos, x, y, z)
    SETVEC3FCOORDS(velocity, velX, velY, velZ)
    SETVEC3FCOORDS(acceleration, accX, accY, accZ)
private:
    Terrain *terrain;
    bool angleInit;
    /* Too lazy to calculate rotations manually,
      so using opengl's rotatef utility and storing
      a large matrix */
    float rotationMatrix[16];
    /* Regular velocity squared after which it's not possible to accelerate */
    float maxVelocity, maxVelocityS;
    /* Friction of the field, between 0 and 1 */
    float friction;
    /* Weight of the ball, for the friction */
    float weight;
};

#endif // BALL_H
