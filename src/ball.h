#ifndef BALL_H
#define BALL_H

class Terrain;

class Ball {
public:
    Ball(float radius = 1.0f);

    void setTerrain(Terrain *terrain);
    void draw();

    /* Reposition the ball inside the terrain if its out */
    void checkPos();
    /* advances the ball */
    void advance(float xdiff, float zdiff);

    float radius;
    float x, z, y;
private:
    Terrain *terrain;
};

#endif // BALL_H
