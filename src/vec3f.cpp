/* See vec3f-inl.h */
#include "vec3f.h"
#include <iostream>

using namespace std;

ostream &operator<<(ostream &output, const Vec3f &v) {
    output << '(' << v[0] << ", " << v[1] << ", " << v[2] << ')';
    return output;
}
