#ifndef GAME_H
#define GAME_H

#include <string>
#include "ball.h"

class Terrain;

/**
 * Main class for the Game. The whole lifecycle of the openGL window.
 */
class Game {
public:
    Game();
    ~Game();

    void loadTerrain(const std::string &filename, float height, float width);
    void update(int time);
    void draw();

    void setCameraRotate (int direction);
private:
    float cameraAngle;
    int cameraRotateDirection;
    Ball ball;
    Terrain *terrain;
};

#endif // GAME_H
