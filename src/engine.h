#ifndef ENGINE_H
#define ENGINE_H

#include "gl.h"

class Engine {
public:
    static GLuint loadTexture(const char *filepath);
    /* Draws a sphere. Radius is the radius of the sphere,
      del is the slice parameters (9.0f = 20 slices, as the
      whole is 180.f) */
    static void drawSphere(float radius = 0.75f, float del=9.0f);
    /* Useful way to make 2-color spheres: draw the hemisphere with
      a positive, then negative radius */
    static void drawHemisphere(float radius = 0.75f, float del=9.0f);
};

#endif // ENGINE_H
