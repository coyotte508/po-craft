#ifndef CAMERA_H
#define CAMERA_H

#include "mathconst.h"
#include <cmath>

class Terrain;

class Camera {
public:
    Camera();

    /* Puts itself at the position of the object */
    template<class T> void project(const T &xyzObj);
    /* Puts itself at the position of the object */
    template<class T> void projectBehind(const T &xyzObj);
    /* Puts itself over the terrain */
    void project(const Terrain &t);
    /* Rotates the camera angle */
    void rotate(float angleXDiff, float angleYDiff);

    float angleX, angleY;
};

#ifdef __APPLE__
#include <OpenGL/OpenGL.h>
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

template<class T> void Camera::project(const T &xyz) {
    glRotatef(angleX, 1, 0, 0);
    glRotatef(angleY, 0, 1, 0);
    glTranslatef(-xyz.x(), -xyz.y(), -xyz.z());
}

template<class T> void Camera::projectBehind(const T &xyz) {
    /* Genius calculus at work here */
    glRotatef(angleX, 1, 0, 0);
    glTranslatef(0, 0, -7.f*cos(angleX*PI/180));
    glRotatef(angleY, 0, 1, 0);
    glTranslatef(0, 0, 7.f*cos(angleX*PI/180));
    glRotatef(-angleX, 1, 0, 0);
    glTranslatef(0,0,-7.f);
    glRotatef(angleX, 1, 0, 0);
    glTranslatef(-xyz.x(), -xyz.y(), -xyz.z());
}

#endif // CAMERA_H
