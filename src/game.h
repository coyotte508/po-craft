#ifndef GAME_H
#define GAME_H

#include <string>
#include "ball.h"
#include "camera.h"

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

    void setCameraRotate (int directionX, int directionY);
    void setCameraZoom (int directionForward);
    void setBallDirection (int horDirection, int vertDirection);
    void alternateCameraMode();
private:
    int cameraRotateDirectionX, cameraRotateDirectionY, cameraZoomDirection;
    Ball ball;
    int xDir, zDir;
    Terrain *terrain;
    Camera camera;

    enum CameraMode {
        OnCharacter,
        OnTerrain,
        BehindCharacter,
        LastCameraMode
    };

    int cameraMode;
};

#endif // GAME_H
