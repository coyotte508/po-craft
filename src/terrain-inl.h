#ifndef TERRAININL_H
#define TERRAININL_H

#include "terrain.h"

//Returns the normal at (x, z)
inline Vec3f Terrain::getNormal(int x, int z) {
    if (!computedNormals) {
        computeNormals();
    }
    return normals[offset(x, z)];
}

//Returns the height at (x, z)
inline float Terrain::getHeight(int x, int z) const {
    return hs[offset(x, z)];
}

//Sets height at (x, z) to y
inline void Terrain::setHeight(int x, int z, float y) {
    hs[offset(x, z)] = y;
    computedNormals = false;
}

//Returns the width of the terrain
inline int Terrain::rawWidth() const {
    return w;
}

//Returns the length of the terrain
inline int Terrain::rawLength() const {
    return l;
}

//Returns the width of the terrain
inline float Terrain::width() const {
    return scaledWidth;
}

//Returns the length of the terrain
inline float Terrain::length() const {
    return scaledLength ;
}

//Computes the offset in the array from x and z given
inline int Terrain::offset(int x, int z) const {
    return z*rawWidth() + x;
}

#endif // TERRAININL_H
