#ifndef VEC3FINL_H
#define VEC3FINL_H

/* File for "Terrain" lesson of the OpenGL tutorial on
 * www.videotutorialsrock.com
 */

#include <cmath>
#include "vec3f.h"

inline Vec3f::Vec3f(float x, float y, float z) {
    v[0] = x;
    v[1] = y;
    v[2] = z;
}

inline float &Vec3f::operator[](int index) {
    return v[index];
}

inline float Vec3f::operator[](int index) const {
    return v[index];
}

inline Vec3f Vec3f::operator*(float scale) const {
    return Vec3f(v[0] * scale, v[1] * scale, v[2] * scale);
}

inline Vec3f Vec3f::operator*(const Vec3f &other) const {
    return Vec3f(v[0] * other[0], v[1] * other[1], v[2] * other[2]);
}

inline Vec3f Vec3f::operator/(float scale) const {
    return Vec3f(v[0] / scale, v[1] / scale, v[2] / scale);
}

inline Vec3f Vec3f::operator+(const Vec3f &other) const {
    return Vec3f(v[0] + other.v[0], v[1] + other.v[1], v[2] + other.v[2]);
}

inline Vec3f Vec3f::operator-(const Vec3f &other) const {
    return Vec3f(v[0] - other.v[0], v[1] - other.v[1], v[2] - other.v[2]);
}

inline Vec3f Vec3f::operator-() const {
    return Vec3f(-v[0], -v[1], -v[2]);
}

inline const Vec3f &Vec3f::operator*=(float scale) {
    v[0] *= scale;
    v[1] *= scale;
    v[2] *= scale;
    return *this;
}

inline const Vec3f &Vec3f::operator/=(float scale) {
    v[0] /= scale;
    v[1] /= scale;
    v[2] /= scale;
    return *this;
}

inline const Vec3f &Vec3f::operator+=(const Vec3f &other) {
    v[0] += other.v[0];
    v[1] += other.v[1];
    v[2] += other.v[2];
    return *this;
}

inline const Vec3f &Vec3f::operator-=(const Vec3f &other) {
    v[0] -= other.v[0];
    v[1] -= other.v[1];
    v[2] -= other.v[2];
    return *this;
}

inline float Vec3f::magnitude() const {
    return sqrt(magnitudeSquared());
}

inline float Vec3f::magnitudeSquared() const {
    return v[0] * v[0] + v[1] * v[1] + v[2] * v[2];
}

inline Vec3f Vec3f::normalize() const {
    float m = magnitude();
    return Vec3f(v[0] / m, v[1] / m, v[2] / m);
}

inline float Vec3f::dot(const Vec3f &other) const {
    return v[0] * other.v[0] + v[1] * other.v[1] + v[2] * other.v[2];
}

inline Vec3f Vec3f::cross(const Vec3f &other) const {
    return Vec3f(v[1] * other.v[2] - v[2] * other.v[1],
                 v[2] * other.v[0] - v[0] * other.v[2],
                 v[0] * other.v[1] - v[1] * other.v[0]);
}

inline Vec3f operator*(float scale, const Vec3f &v) {
    return v * scale;
}

#endif // VEC3FINL_H
