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
    void advance(float xdiff, float zdiff);

    float radius;
    Vec3f pos;
    Vec3f angle;

    SETVEC3FCOORDS(pos, x, y, z)
private:
    Terrain *terrain;
    bool angleInit;
    /* Too lazy to calculate rotations manually,
      so using opengl's rotatef utility and storing
      a large matrix */
    float rotationMatrix[16];
};

#endif // BALL_H
