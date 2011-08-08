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
    float getHeight(int x, int z) const;

    float heightAt(float x, float z) const;
    /**
     * Gives the height if a ball with the given radius
     * had its center at the given x/z points.
     *
     * pInt is set to the collision point with the terrain,
     * if needed, otherwise just (x, height, z).
     *
     * Complexity: radius^2
    */
    float heightAt(float x, float z, float radius, Vec3f *pInt=NULL) const;
    /**
     * Gives the normal calculated dynamically at this precise point.
     * The normal isn't averaged in the least, contrary to light normals
     * and their fallout ratio.
     *
     * Used for collision & all.
     */
    Vec3f normalAt(float x, float z) const;

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
    int offset(int x, int z) const;

    int w; //Width
    int l; //Length
    float* hs; //Heights
    Vec3f* normals;
    bool computedNormals; //Whether normals is up-to-date
    float scale, scaledWidth, scaledLength;
};

#include "terrain-inl.h"

#endif // TERRAIN_H
