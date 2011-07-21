#include "camera.h"
#include "terrain.h"

Camera::Camera() {
    angleX = angleY = 0;
}

void Camera::project(const Terrain &terrain) {
    glTranslatef(0, 0, - terrain.width());
    glRotatef(30, 1, 0, 0);
    glRotatef(angleX, 1, 0, 0);
    glRotatef(angleY, 0, 1, 0);
    glTranslatef(-terrain.width() / 2, 0, -terrain.width() / 2);
}

void Camera::rotate(float a1, float a2) {
    angleX += a1;
    angleY += a2;
    if (angleX > 360) {
        angleX -= 360;
    } else if (angleX < 0) {
        angleX += 360;
    }
    if (angleY > 360) {
        angleY -= 360;
    } else if (angleY < 0) {
        angleY += 360;
    }
}
