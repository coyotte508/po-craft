#include <QImage>
#include <cmath>
#include "engine.h"
#include "mathconst.h"

GLuint Engine::loadTexture(const char *filepath) {
    QImage image(filepath);

    if (image.isNull()) {
        return 0;
    }

    image = image.convertToFormat(QImage::Format_RGB888);

    GLuint textureId;
    glGenTextures(1, &textureId); //Make room for our texture

    glBindTexture(GL_TEXTURE_2D, textureId); //Tell OpenGL which texture to edit
    //Map the image to the texture
    glTexImage2D(GL_TEXTURE_2D,                //Always GL_TEXTURE_2D
                 0,                            //0 for now
                 GL_RGB,                       //Format OpenGL uses for image
                 image.width(), image.height(),  //Width and height
                 0,                            //The border of the image
                 GL_RGB, //GL_RGB, because pixels are stored in RGB format
                 GL_UNSIGNED_BYTE, //GL_UNSIGNED_BYTE, because pixels are stored
                 //as unsigned numbers
                 image.bits());               //The actual pixel data

    return textureId;
}

inline void vert(float r, float theta, float phi)
{
    float x, y, z, nx, ny, nz;
    float mult = r < 0 ? -1 : 1;

    nx = mult * sin(DTOR * theta) * cos(DTOR * phi);
    ny = mult * sin(DTOR * phi);
    nz = mult * cos(DTOR * theta) * cos(DTOR * phi);
    glNormal3f(nx, ny, nz);

    x = r * sin(DTOR * theta) * cos(DTOR * phi);
    y = r * sin(DTOR * phi);
    z = r * cos(DTOR * theta) * cos(DTOR * phi);
    glVertex4f(x, y, z, 1.0);
}

void Engine::drawHemisphere(float radius, float del) {
    float phi, phi2, theta;

    for (phi = -90.0f; phi < 90.0f; phi += del) {
        glBegin(GL_TRIANGLE_STRIP);

        phi2 = phi + del;

        for (theta = -90.0f; theta <= 90.0f; theta += del) {
            vert(radius, theta, phi);
            vert(radius, theta, phi2);
        }
        glEnd();
    }
}

void Engine::drawSphere(float radius, float del) {
    drawHemisphere(radius, del);
    drawHemisphere(-radius, del);
}
