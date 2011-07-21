#include "camera.h"
#include "terrain.h"

Camera::Camera() {
    angle = 0;
}

void Camera::project(const Terrain &terrain) {
    glTranslatef(0, 0, - terrain.width());
    glRotatef(30, 1, 0, 0);
    glRotatef(angle, 0, 1, 0);
    glTranslatef(-terrain.width() / 2, 0, -terrain.width() / 2);
}

void Camera::rotate(float angleDiff) {
    angle += angleDiff;
}
