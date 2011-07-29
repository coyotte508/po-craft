/* File for "Terrain" lesson of the OpenGL tutorial on
 * www.videotutorialsrock.com
 */

#ifndef VEC3F_H_INCLUDED
#define VEC3F_H_INCLUDED

#include <ostream>

class Vec3f {
private:
    float v[3];
public:
    Vec3f(float x=0, float y=0, float z=0);

    float &operator[](int index);
    float operator[](int index) const;

    Vec3f operator*(float scale) const;
    Vec3f operator*(const Vec3f &other) const;
    Vec3f operator/(float scale) const;
    Vec3f operator+(const Vec3f &other) const;
    Vec3f operator-(const Vec3f &other) const;
    Vec3f operator-() const;

    const Vec3f &operator*=(float scale);
    const Vec3f &operator/=(float scale);
    const Vec3f &operator+=(const Vec3f &other);
    const Vec3f &operator-=(const Vec3f &other);

    float magnitude() const;
    float magnitudeSquared() const;
    Vec3f normalize() const;
    float dot(const Vec3f &other) const;
    Vec3f cross(const Vec3f &other) const;
};

Vec3f operator*(float scale, const Vec3f &v);
std::ostream &operator<<(std::ostream &output, const Vec3f &v);

#include "vec3f-inl.h"

#endif
