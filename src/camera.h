#ifndef CAMERA_H
#define CAMERA_H

class Terrain;

class Camera {
public:
    Camera();

    /* Puts itself at the position of the object */
    template<class T> void project(T xyzObj);
    /* Puts itself over the terrain */
    void project(const Terrain &t);
    /* Rotates the camera angle */
    void rotate(float angleDiff);

    float angle;
};

#ifdef __APPLE__
#include <OpenGL/OpenGL.h>
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

template<class T> void Camera::project(T xyz) {
    glTranslatef(-xyz.x(), -xyz.y(), -xyz.z());
    glRotatef(angle, 0, 1, 0);
}

#endif // CAMERA_H
