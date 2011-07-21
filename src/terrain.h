#ifndef TERRAIN_H
#define TERRAIN_H

#include "vec3f.h"
#include <string>

/* Code inspired from http://www.videotutorialsrock.com/opengl_tutorial/terrain/home.php */

class Terrain {
public:
    Terrain(int w2, int l2);
    ~Terrain();

    /* Returns raw width */
    int rawWidth() const;
    int rawLength() const;
    /*Returns coordinates width */
    float width() const;
    float length() const;

    //Sets the height at (x, z) to y
    void setHeight(int x, int z, float y);
    //Returns the height at (x, z)
    float getHeight(int x, int z);

    float heightAt(float x, float z);

    //Computes the normals, if they haven't been computed yet
    void computeNormals();
    //Returns the normal at (x, z)
    Vec3f getNormal(int x, int z);

    //Draws the terrain on the scene
    void draw();

    /**
      * Creates a terrain from a heightmap, with height range @p height,
      * and width @p width. Length is calculated automatically as the image
      * is scaled uniformly
      */
    static Terrain *loadTerrain(const std::string &filename, float height, float width);
private:
    //Computes the offset in the array from x and z given
    int offset(int x, int z);

    int w; //Width
    int l; //Length
    float* hs; //Heights
    Vec3f* normals;
    bool computedNormals; //Whether normals is up-to-date
    float scale, scaledWidth, scaledLength;
};

#include "terrain-inl.h"

#endif // TERRAIN_H
