#ifndef DEBUGVAL_H
#define DEBUGVAL_H

#include <vec3f.h>

struct DebugVal {
    static Vec3f debug, debug2, pInt;
    static float dis, p, dis2, insideS, radius2;
    static float iniHeight, endHeight, intHeight;
    static bool active;
};

#endif // DEBUGVAL_H
