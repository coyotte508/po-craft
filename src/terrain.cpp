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
    planeOffsets = new float[l*w];

    std::fill(hs, hs+l*w, 0.f);
    std::fill(normals, normals+l*w, Vec3f(0.f, 1.f, 0.f));

    computedNormals = false;
}

Terrain::~Terrain()
{
    delete[] hs;
    delete[] normals;
    delete[] planeOffsets;
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
            planeOffsets[offset(x,z)] = -Vec3f(x, getHeight(x, z), z).dot(sum);
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

    if (fracX + fracZ <= 1) {
        normal = -Vec3f(1, getHeight(leftX + 1, outZ)-getHeight(leftX, outZ), 0).
                cross(Vec3f(0, getHeight(leftX, outZ+1)-getHeight(leftX, outZ), 1));
        plane = -normal.dot(Vec3f(leftX, getHeight(leftX, outZ), outZ));
    } else {
        normal = Vec3f(1, getHeight(leftX+1, outZ+1) - getHeight(leftX, outZ+1), 0).
                cross(Vec3f(0, getHeight(leftX+1, outZ+1)-getHeight(leftX+1, outZ), 1));
        plane = -normal.dot(Vec3f(leftX+1, getHeight(leftX+1, outZ+1), outZ+1));
    }

    return (-normal[0]*x-normal[2]*z-plane)/normal[1]*scale;
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
            float h = getHeight(i, j);
            if (h <= minHeight) {
                continue;
            }
            if (i+1 < rawWidth() && j+1 < rawLength()
                    && i < xlim && j < zlim) {
                /* Lets do a plane check */
                Vec3f v1(1, getHeight(x+1,z)-getHeight(x, z), 0);
                Vec3f v2(0, getHeight(x,z+1)-getHeight(x, z), 1);
                Vec3f normal = v1.cross(v2).normalize();
                float p = -Vec3f(x, getHeight(x, z), z).dot(normal);
                if (normal[1] < 0) {
                    p=-p;
                    normal=-normal;
                }
                float dis = normal.dot(spherePos)+p;
                if (dis < 0) {
                    dis = -dis;
                }
                if (dis < radius) {
                    Vec3f pInt = spherePos-dis*normal;
                    float minH = pInt[1]+ sqrt(radius2-(spherePos[0]-pInt[0])*(spherePos[0]-pInt[0])
                                     -(spherePos[2]-pInt[2])*(spherePos[2]-pInt[2]))-radius;
                    if (minH > minHeight) {
                        DebugVal::debug2 = spherePos-pInt;
                        DebugVal::insideS = radius2-(spherePos[0]-pInt[0])*(spherePos[0]-pInt[0])
                                -(spherePos[2]-pInt[2])*(spherePos[2]-pInt[2]);
                        DebugVal::debug = normal;
                        DebugVal::dis = dis;
                        DebugVal::dis2 = radius;
                        DebugVal::radius2 = radius2;
                        DebugVal::p = p;
                        DebugVal::intHeight = minHeight;
                        DebugVal::endHeight = minH;
                        DebugVal::pInt = pInt;

                        minHeight=minH;
                        spherePos[1] = minHeight+radius;
                        DebugVal::active = true;
                    }
                }
            }

            float hordis = (i-x)*(i-x) +(j-z)*(j-z);
            if (hordis >= radius2) {
                continue;
            }
            if (hordis+(minHeight+radius-h)*(minHeight+radius-h)<radius2) {
                minHeight = sqrt(radius2-hordis)-radius+h;
                spherePos[1] = minHeight+radius;
            }
        }
    }

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
    glPopMatrix();
}
