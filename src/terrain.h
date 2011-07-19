#ifndef TERRAIN_H
#define TERRAIN_H

#include "vec3f.h"
#include <string>

/* Code inspired from http://www.videotutorialsrock.com/opengl_tutorial/terrain/home.php */

class Terrain {
public:
    Terrain(int w2, int l2);
    ~Terrain();

    int width();
    int length();

    //Sets the height at (x, z) to y
    void setHeight(int x, int z, float y);
    //Returns the height at (x, z)
    float getHeight(int x, int z);

    //Computes the normals, if they haven't been computed yet
    void computeNormals();
    //Returns the normal at (x, z)
    Vec3f getNormal(int x, int z);

    //Draws the terrain on the scene
    void draw();

    static Terrain *loadTerrain(const std::string &filename, float height);
private:
    //Computes the offset in the array from x and z given
    int offset(int x, int z);

    int w; //Width
    int l; //Length
    float* hs; //Heights
    Vec3f* normals;
    bool computedNormals; //Whether normals is up-to-date
};

#include "terrain-inl.h"

#endif // TERRAIN_H
