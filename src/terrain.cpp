#ifdef __APPLE__
#include <OpenGL/OpenGL.h>
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

#include <algorithm>
#include <string>
#include <QImage>
#include "terrain.h"

Terrain::Terrain(int w2, int l2)
{
    w = w2;
    l = l2;

    hs = new float[l*w];
    normals = new Vec3f[l*w];

    std::fill(hs, hs+l*w, 0.f);
    std::fill(normals, normals+l*w, Vec3f(0.f, 1.f, 0.f));

    computedNormals = false;
}

Terrain::~Terrain()
{
    delete[] hs;
    delete[] normals;
}

//Computes the normals, if they haven't been computed yet
void Terrain::computeNormals() {
    if (computedNormals) {
        return;
    }

    //Compute the rough version of the normals
    Vec3f* normals2 = new Vec3f[l*w];

    for(int z = 0; z < l; z++) {
        for(int x = 0; x < w; x++) {
            Vec3f sum(0.0f, 0.0f, 0.0f);

            Vec3f out;
            if (z > 0) {
                out = Vec3f(0.0f, getHeight(x, z-1) - getHeight(x, z), -1.0f);
            }
            Vec3f in;
            if (z < l - 1) {
                in = Vec3f(0.0f, getHeight(x, z+1) - getHeight(x, z), 1.0f);
            }
            Vec3f left;
            if (x > 0) {
                left = Vec3f(-1.0f, getHeight(x-1, z) - getHeight(x, z), 0.0f);
            }
            Vec3f right;
            if (x < w - 1) {
                right = Vec3f(1.0f, getHeight(x+1, z) - getHeight(x, z), 0.0f);
            }

            if (x > 0 && z > 0) {
                sum += out.cross(left).normalize();
            }
            if (x > 0 && z < l - 1) {
                sum += left.cross(in).normalize();
            }
            if (x < w - 1 && z < l - 1) {
                sum += in.cross(right).normalize();
            }
            if (x < w - 1 && z > 0) {
                sum += right.cross(out).normalize();
            }

            normals2[offset(x, z)] = sum;
        }
    }

    //Smooth out the normals
    const float FALLOUT_RATIO = 0.5f;
    for(int z = 0; z < l; z++) {
        for(int x = 0; x < w; x++) {
            Vec3f sum = normals2[offset(x, z)];

            if (x > 0) {
                sum += normals2[offset(x-1, z)] * FALLOUT_RATIO;
            }
            if (x < w - 1) {
                sum += normals2[offset(x+1, z)] * FALLOUT_RATIO;
            }
            if (z > 0) {
                sum += normals2[offset(x, z-1)] * FALLOUT_RATIO;
            }
            if (z < l - 1) {
                sum += normals2[offset(x, z+1)] * FALLOUT_RATIO;
            }

            if (sum.magnitude() == 0) {
                sum = Vec3f(0.0f, 1.0f, 0.0f);
            }
            normals[offset(x, z)] = sum;
        }
    }

    delete[] normals2;

    computedNormals = true;
}

Terrain *Terrain::loadTerrain(const std::string &filename, float height)
{
    QImage image(filename.c_str());

    if (image.isNull()) {
        return NULL;
    }

    Terrain* t = new Terrain(image.width(), image.height());
    for(int y = 0; y < image.height(); y++) {
        for(int x = 0; x < image.width(); x++) {
            unsigned char color = image.bits()[4 * (y * image.width() + x)];
            float h = height * ((color / 255.0f) - 0.5f);
            t->setHeight(x, y, h);
        }
    }

    t->computeNormals();
    return t;
}

void Terrain::draw()
{
    glPushMatrix();
    float scale = 5.0f / std::max(width() - 1, length() - 1);
    glScalef(scale, scale, scale);
    glTranslatef(-(float)(width() - 1) / 2,
                 0.0f,
                 -(float)(length() - 1) / 2);

    glColor3f(0.3f, 0.9f, 0.0f);
    for(int z = 0; z < length() - 1; z++) {
        //Makes OpenGL draw a triangle at every three consecutive vertices
        glBegin(GL_TRIANGLE_STRIP);
        for(int x = 0; x < width(); x++) {
            Vec3f normal = getNormal(x, z);
            glNormal3f(normal[0], normal[1], normal[2]);
            glVertex3f(x, getHeight(x, z), z);
            normal = getNormal(x, z + 1);
            glNormal3f(normal[0], normal[1], normal[2]);
            glVertex3f(x, getHeight(x, z + 1), z + 1);
        }
        glEnd();
    }
    glPopMatrix();
}
