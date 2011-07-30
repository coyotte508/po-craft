#ifdef __APPLE__
#include <OpenGL/OpenGL.h>
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

#include <algorithm>
#include <string>
#include <QImage>
#include "mathconst.h"
#include "terrain.h"
#include "debug-val.h"

Terrain::Terrain(int w2, int l2)
{
    w = w2;
    l = l2;
    scale = 1.0f;
    scaledWidth = w2-1;
    scaledLength = l2-1;

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

float Terrain::heightAt(float x, float z)
{
    x/=scale;
    z/=scale;
    //Make (x, z) lie within the bounds of the terrain
    if (x < 0) {
        x = 0;
    } else if (x > rawWidth() - 1) {
        x = rawWidth() - 1;
    }

    if (z < 0) {
        z = 0;
    } else if (z > rawLength() - 1) {
        z = rawLength() - 1;
    }

    //Compute the grid cell in which (x, z) lies and how close we are to the
    //left and outward edges
    int leftX = (int)x;
    if (leftX == rawWidth() - 1) {
        leftX--;
    }
    float fracX = x - leftX;

    int outZ = (int)z;
    if (outZ == rawWidth() - 1) {
        outZ--;
    }
    float fracZ = z - outZ;

    Vec3f normal;
    float plane;

    /* First we check if the point is in the first triangle
      or the second triangle. This is done by fracX + fracZ
      being more or less than 1.

      Then we get the plane equation corresponding to the triangle,
      and use it to get the y coordinate of the point knowing x and z */
    if (fracX + fracZ <= 1) {
        float mh = getHeight(leftX, outZ);

        normal = -Vec3f(1, getHeight(leftX + 1, outZ)-mh, 0).
                cross(Vec3f(0, getHeight(leftX, outZ+1)-mh, 1));
        plane = -normal.dot(Vec3f(leftX, mh, outZ));
    } else {
        float mh = getHeight(leftX+1, outZ+1);

        normal = Vec3f(1, mh - getHeight(leftX, outZ+1), 0).
                cross(Vec3f(0, mh-getHeight(leftX+1, outZ), 1));
        plane = -normal.dot(Vec3f(leftX+1, mh, outZ+1));
    }

    return (-normal[0]*x-normal[2]*z-plane)/normal[1]*scale;
}

inline void newHeight(Vec3f &spherePos, float &minHeight, int x, int z, float h0, float h10, float h01, float radius) {
    Vec3f normal = -Vec3f(1, h10-h0, 0).cross(Vec3f(0, h01-h0, 1)).normalize();

    float dis = (spherePos-Vec3f(x,h0,z)).dot(normal);
    float absDis = dis < 0 ? - dis : dis;

    if (absDis > radius) {
        return;
    }

    /* Now let's get the point of intersection the closest to the center of the sphere */
    Vec3f pInt = spherePos - dis * normal;

    /* Let's check if it's in the triangle */
    if (pInt[0] < x || pInt[2] < z || pInt[0]-x + pInt[2]-z > 1) {
        if (pInt[0] < x) {
            if (pInt[2] < z) {
                /* It intersects with the (x,z) summit */
                pInt[0] = x;
                pInt[2] = z;
            } else if (pInt[2] > z + 1) {
                pInt[0] = x;
                pInt[2] = z+1;
            } else {
                /* Intersects with the (x,z) (x,z+1) verticle */
                pInt[0] = x;
            }
        } else if (pInt[2] < z) {
            /* We already know pInt[0] > x, so only two possibilities */
            if (pInt[0] > x + 1) {
                pInt[0] = x + 1;
                pInt[2] = z;
            } else {
                pInt[2] = z;
            }
        } else {
            /* Trouble: the third, annoying verticle is the one to be intersected */
            float dotproduct = (1 * (pInt[0]-x) - 1 * (pInt[2]-(z+1)));

            if (dotproduct < 0) {
                pInt[0] = x;
                pInt[2] = z+1;
            } else if (dotproduct > 2) {
                pInt[0] = x+1;
                pInt[2] = z;
            } else {
                pInt[0] = x + dotproduct/2;
                pInt[2] = z - 1 - dotproduct/2;
            }
        }
        float p = -normal.dot(Vec3f(x,h0,z));
        pInt[1] = (-normal[0]*pInt[0]-normal[2]*pInt[2]-p)/normal[1];
    }

    float hordis = (pInt[0]-spherePos[0])*(pInt[0]-spherePos[0])+(pInt[2]-spherePos[2])*(pInt[2]-spherePos[2]);
    if (hordis < radius*radius) {
        float h = sqrt(radius*radius-hordis)-radius+pInt[1];
        if (h > minHeight) {
            minHeight = h;
            spherePos[1] = minHeight+radius;
        }
    }
}

inline void newHeight2(Vec3f &spherePos, float &minHeight, int x1, int z1, float h11, float h10, float h01, float radius) {
    Vec3f normal = -Vec3f(1, h11-h01, 0).cross(Vec3f(0, h11-h10, 1)).normalize();

    float dis = (spherePos-Vec3f(x1,h11,z1)).dot(normal);
    float absDis = dis < 0 ? - dis : dis;

    if (absDis > radius) {
        return;
    }

    /* Now let's get the point of intersection the closest to the center of the sphere */
    Vec3f pInt = spherePos - dis * normal;

    /* Let's check if it's in the triangle */
    if (pInt[0] > x1 || pInt[2] > z1 || pInt[0]-x1 + pInt[2]-z1 < -1) {
        if (pInt[0] > x1) {
            if (pInt[2] > z1) {
                /* It intersects with the (x1,z1) summit */
                pInt[0] = x1;
                pInt[2] = z1;
            } else if (pInt[2] < z1-1) {
                pInt[0] = x1;
                pInt[2] = z1-1;
            } else {
                /* Intersects with the (x1,z1-1) (x1,z1) verticle */
                pInt[0] = x1;
            }
        } else if (pInt[2] > z1) {
            /* We already know pInt[0] < x1, so only two possibilities */
            if (pInt[0] < x1-1) {
                pInt[0] = x1-1;
                pInt[2] = z1;
            } else {
                pInt[2] = z1;
            }
        } else {
            ///NOT DONE
            /* Trouble: the third, annoying verticle is the one to be intersected */
            float dotproduct = (1 * (pInt[0]-(x1-1)) - 1 * (pInt[2]-(z1)));

            if (dotproduct < 0) {
                pInt[0] = x1-1;
                pInt[2] = z1;
            } else if (dotproduct > 2) {
                pInt[0] = x1;
                pInt[2] = z1-1;
            } else {
                pInt[0] = x1 -1 + dotproduct/2;
                pInt[2] = z1 - dotproduct/2;
            }
        }
        float p = -normal.dot(Vec3f(x1,h11,z1));
        pInt[1] = (-normal[0]*pInt[0]-normal[2]*pInt[2]-p)/normal[1];
    }

    float hordis = (pInt[0]-spherePos[0])*(pInt[0]-spherePos[0])+(pInt[2]-spherePos[2])*(pInt[2]-spherePos[2]);
    if (hordis < radius*radius) {
        float h = sqrt(radius*radius-hordis)-radius+pInt[1];
        if (h > minHeight) {
            minHeight = h;
            spherePos[1] = minHeight+radius;
        }
    }
}

/* Can be optimized */
float Terrain::heightAt(float x, float z, float radius)
{
    DebugVal::active = false;
    float minHeight = heightAt(x, z)/scale;

    x/=scale;
    z/=scale;
    radius/=scale;
    float radius2 = radius*radius;

    DebugVal::iniHeight = minHeight;

    Vec3f spherePos(x, minHeight+radius,z);

    int xlim = x+radius+1.f;
    int zlim = z+radius+1.f;
    /* Complexity: radius^2 */
    for (int i = std::max(int(x-radius), 0); i <= xlim && i < rawWidth(); i++) {
        for (int j = std::max(int(z-radius), 0); j <= zlim && j < rawLength(); j++) {
            if (i+1 < rawWidth() && j+1 < rawLength()
                    && i < xlim && j < zlim) {
                newHeight(spherePos, minHeight, i, j, getHeight(i, j), getHeight(i+1, j), getHeight(i, j+1), radius);
                newHeight2(spherePos, minHeight, i+1, j+1, getHeight(i+1, j+1), getHeight(i+1, j), getHeight(i, j+1), radius);
            }
        }
    }

    DebugVal::active = true;
    DebugVal::iniHeight = minHeight;

    return minHeight*scale;
}

Terrain *Terrain::loadTerrain(const std::string &filename, float height, float side)
{
    QImage image(filename.c_str());

    if (image.isNull() || image.width() <= 1 || image.height() <= 1) {
        return NULL;
    }

    Terrain* t = new Terrain(image.width(), image.height());

    t->scale = side/(t->rawWidth()-1);
    t->scaledWidth = (t->rawWidth()-1)*t->scale;
    t->scaledLength = (t->rawLength()-1)*t->scale;
    height /= t->scale;

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
    glDisable(GL_TEXTURE_2D);
    glColor3f(0.3f, 0.9f, 0.0f);
    glScalef(scale, scale, scale);
    for(int z = 0; z < rawLength() - 1; z++) {
        glBegin(GL_TRIANGLE_STRIP);
        for(int x = 0; x < rawWidth(); x++) {
            Vec3f normal = getNormal(x, z);
            glNormal3f(normal[0], normal[1], normal[2]);
            glVertex3f(x, getHeight(x, z), z);
            normal = getNormal(x, z + 1);
            glNormal3f(normal[0], normal[1], normal[2]);
            glVertex3f(x, getHeight(x, z + 1), z + 1);
        }
        glEnd();
    }
#if 0
    /* Wireframe mode */
    glDisable(GL_LIGHTING);
    glColor3f(1.f, 1.f, 1.f);
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    for(int z = 0; z < rawLength() - 1; z++) {
        glBegin(GL_TRIANGLE_STRIP);
        for(int x = 0; x < rawWidth(); x++) {
            glVertex3f(x, getHeight(x, z)+.1, z);
            glVertex3f(x, getHeight(x, z + 1)+.1, z + 1);
        }
        glEnd();
    }
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    glEnable(GL_LIGHTING);
#endif
    glPopMatrix();
}
