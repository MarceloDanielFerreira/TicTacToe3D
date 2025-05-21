#ifndef GEOMETRYMAKER_H
#define GEOMETRYMAKER_H

#include <cmath>
#include <vector>
#include <cassert>

#include "cvec.h"

//--------------------------------------------------------------------------------
// Helpers for creating some special geometries such as plane, cubes, and spheres
//--------------------------------------------------------------------------------

// A generic vertex structure containing position, normal, and texture information
// Used by make* functions to pass vertex information to the caller
struct GenericVertex {
    Cvec3f pos;
    Cvec3f normal;
    Cvec2f tex;
    Cvec3f tangent, binormal;

    GenericVertex(
        float x, float y, float z,
        float nx, float ny, float nz,
        float tu, float tv,
        float tx, float ty, float tz,
        float bx, float by, float bz)
        : pos(x, y, z), normal(nx, ny, nz), tex(tu, tv), tangent(tx, ty, tz), binormal(bx, by, bz)
    {
    }
};

inline void getPlaneVbIbLen(int& vbLen, int& ibLen) {
    vbLen = 4;
    ibLen = 6;
}

template<typename VtxOutIter, typename IdxOutIter>
void makePlane(float size, VtxOutIter vtxIter, IdxOutIter idxIter) {
    float h = size / 2.0;
    *vtxIter++ = GenericVertex(-h, 0, -h, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0, -1);
    *vtxIter++ = GenericVertex(-h, 0, h, 0, 1, 0, 0, 1, 1, 0, 0, 0, 0, -1);
    *vtxIter++ = GenericVertex(h, 0, h, 0, 1, 0, 1, 1, 1, 0, 0, 0, 0, -1);
    *vtxIter++ = GenericVertex(h, 0, -h, 0, 1, 0, 1, 0, 1, 0, 0, 0, 0, -1);

    *idxIter++ = 0;
    *idxIter++ = 1;
    *idxIter++ = 2;
    *idxIter++ = 0;
    *idxIter++ = 2;
    *idxIter++ = 3;
}

inline void getCubeVbIbLen(int& vbLen, int& ibLen) {
    vbLen = 24;
    ibLen = 36;
}

template<typename VtxOutIter, typename IdxOutIter>
void makeCube(float size, VtxOutIter vtxIter, IdxOutIter idxIter) {
    float h = size / 2.0;
#define DEFV(x, y, z, nx, ny, nz, tu, tv) { \
    *vtxIter++ = GenericVertex(x h, y h, z h, \
                             nx, ny, nz, tu, tv, \
                             tan[0], tan[1], tan[2], \
                             bin[0], bin[1], bin[2]); \
}
    Cvec3f tan(0, 1, 0), bin(0, 0, 1);
    DEFV(+, -, -, 1, 0, 0, 0, 0); // facing +X
    DEFV(+, +, -, 1, 0, 0, 1, 0);
    DEFV(+, +, +, 1, 0, 0, 1, 1);
    DEFV(+, -, +, 1, 0, 0, 0, 1);

    tan = Cvec3f(0, 0, 1);
    bin = Cvec3f(0, 1, 0);
    DEFV(-, -, -, -1, 0, 0, 0, 0); // facing -X
    DEFV(-, -, +, -1, 0, 0, 1, 0);
    DEFV(-, +, +, -1, 0, 0, 1, 1);
    DEFV(-, +, -, -1, 0, 0, 0, 1);

    tan = Cvec3f(0, 0, 1);
    bin = Cvec3f(1, 0, 0);
    DEFV(-, +, -, 0, 1, 0, 0, 0); // facing +Y
    DEFV(-, +, +, 0, 1, 0, 1, 0);
    DEFV(+, +, +, 0, 1, 0, 1, 1);
    DEFV(+, +, -, 0, 1, 0, 0, 1);

    tan = Cvec3f(1, 0, 0);
    bin = Cvec3f(0, 0, 1);
    DEFV(-, -, -, 0, -1, 0, 0, 0); // facing -Y
    DEFV(+, -, -, 0, -1, 0, 1, 0);
    DEFV(+, -, +, 0, -1, 0, 1, 1);
    DEFV(-, -, +, 0, -1, 0, 0, 1);

    tan = Cvec3f(1, 0, 0);
    bin = Cvec3f(0, 1, 0);
    DEFV(-, -, +, 0, 0, 1, 0, 0); // facing +Z
    DEFV(+, -, +, 0, 0, 1, 1, 0);
    DEFV(+, +, +, 0, 0, 1, 1, 1);
    DEFV(-, +, +, 0, 0, 1, 0, 1);

    tan = Cvec3f(0, 1, 0);
    bin = Cvec3f(1, 0, 0);
    DEFV(-, -, -, 0, 0, -1, 0, 0); // facing -Z
    DEFV(-, +, -, 0, 0, -1, 1, 0);
    DEFV(+, +, -, 0, 0, -1, 1, 1);
    DEFV(+, -, -, 0, 0, -1, 0, 1);
#undef DEFV

    for (int v = 0; v < 24; v += 4) {
        *idxIter++ = v;
        *idxIter++ = v + 1;
        *idxIter++ = v + 2;
        *idxIter++ = v;
        *idxIter++ = v + 2;
        *idxIter++ = v + 3;
    }
}

inline void getSphereVbIbLen(int slices, int stacks, int& vbLen, int& ibLen) {
    assert(slices > 1);
    assert(stacks >= 2);
    vbLen = (slices + 1) * (stacks + 1);
    ibLen = slices * stacks * 6;
}

template<typename VtxOutIter, typename IdxOutIter>
void makeSphere(float radius, int slices, int stacks, VtxOutIter vtxIter, IdxOutIter idxIter) {
    using namespace std;
    assert(slices > 1);
    assert(stacks >= 2);

    const double radPerSlice = 2 * CS175_PI / slices;
    const double radPerStack = CS175_PI / stacks;

    vector<double> longSin(slices + 1), longCos(slices + 1);
    vector<double> latSin(stacks + 1), latCos(stacks + 1);
    for (int i = 0; i < slices + 1; ++i) {
        longSin[i] = sin(radPerSlice * i);
        longCos[i] = cos(radPerSlice * i);
    }
    for (int i = 0; i < stacks + 1; ++i) {
        latSin[i] = sin(radPerStack * i);
        latCos[i] = cos(radPerStack * i);
    }

    for (int i = 0; i < slices + 1; ++i) {
        for (int j = 0; j < stacks + 1; ++j) {
            float x = longCos[i] * latSin[j];
            float y = longSin[i] * latSin[j];
            float z = latCos[j];

            Cvec3f n(x, y, z);
            Cvec3f t(-longSin[i], longCos[i], 0);
            Cvec3f b = cross(n, t);

            *vtxIter++ = GenericVertex(
                x * radius, y * radius, z * radius,
                x, y, z,
                1.0 / slices * i, 1.0 / stacks * j,
                t[0], t[1], t[2],
                b[0], b[1], b[2]);

            if (i < slices && j < stacks) {
                *idxIter++ = (stacks + 1) * i + j;
                *idxIter++ = (stacks + 1) * i + j + 1;
                *idxIter++ = (stacks + 1) * (i + 1) + j + 1;

                *idxIter++ = (stacks + 1) * i + j;
                *idxIter++ = (stacks + 1) * (i + 1) + j + 1;
                *idxIter++ = (stacks + 1) * (i + 1) + j;
            }
        }
    }
}

inline void getCylinderVbIbLen(int slices, int stacks, int& vbLen, int& ibLen) {
    assert(slices > 2);
    assert(stacks >= 1);
    vbLen = (slices + 1) * (stacks + 1) + 2 * (slices + 1) + 2; // +2 for center points
    ibLen = slices * stacks * 6 + slices * 6; // 6 indices per slice for caps
}

template<typename VtxOutIter, typename IdxOutIter>
void makeCylinder(float radius, float height, int slices, int stacks, VtxOutIter vtxIter, IdxOutIter idxIter) {
    using namespace std;
    assert(slices > 2);
    assert(stacks >= 1);

    const double radPerSlice = 2 * CS175_PI / slices;
    const double heightPerStack = height / stacks;

    vector<double> sliceSin(slices + 1), sliceCos(slices + 1);
    for (int i = 0; i < slices + 1; ++i) {
        sliceSin[i] = sin(radPerSlice * i);
        sliceCos[i] = cos(radPerSlice * i);
    }

    // Superficie lateral
    for (int i = 0; i < slices + 1; ++i) {
        for (int j = 0; j < stacks + 1; ++j) {
            float x = sliceCos[i];
            float z = sliceSin[i];
            float y = heightPerStack * j - height / 2.0f;

            Cvec3f n(x, 0, z);
            normalize(n);

            Cvec3f t(-sliceSin[i], 0, sliceCos[i]);
            Cvec3f b(0, 1, 0);

            *vtxIter++ = GenericVertex(
                x * radius, y, z * radius,
                n[0], n[1], n[2],
                static_cast<float>(i) / slices, static_cast<float>(j) / stacks,
                t[0], t[1], t[2],
                b[0], b[1], b[2]);

            if (i < slices && j < stacks) {
                *idxIter++ = (stacks + 1) * i + j;
                *idxIter++ = (stacks + 1) * i + j + 1;
                *idxIter++ = (stacks + 1) * (i + 1) + j + 1;

                *idxIter++ = (stacks + 1) * i + j;
                *idxIter++ = (stacks + 1) * (i + 1) + j + 1;
                *idxIter++ = (stacks + 1) * (i + 1) + j;
            }
        }
    }

    // Tapas del cilindro
    int baseIndex = (slices + 1) * (stacks + 1);

    // Tapa inferior
    for (int i = 0; i < slices + 1; ++i) {
        float x = sliceCos[i];
        float z = sliceSin[i];
        float y = -height / 2.0f;

        *vtxIter++ = GenericVertex(
            x * radius, y, z * radius,
            0, -1, 0,
            static_cast<float>(i) / slices, 0,
            1, 0, 0,
            0, 0, 1);
    }

    // Centro tapa inferior
    *vtxIter++ = GenericVertex(
        0, -height / 2.0f, 0,
        0, -1, 0,
        0.5, 0.5,
        1, 0, 0,
        0, 0, 1);

    // Tapa superior
    for (int i = 0; i < slices + 1; ++i) {
        float x = sliceCos[i];
        float z = sliceSin[i];
        float y = height / 2.0f;

        *vtxIter++ = GenericVertex(
            x * radius, y, z * radius,
            0, 1, 0,
            static_cast<float>(i) / slices, 1,
            1, 0, 0,
            0, 0, 1);
    }

    // Centro tapa superior
    *vtxIter++ = GenericVertex(
        0, height / 2.0f, 0,
        0, 1, 0,
        0.5, 0.5,
        1, 0, 0,
        0, 0, 1);

    // Índices para las tapas
    int centerBottom = baseIndex + slices + 1;
    int centerTop = centerBottom + slices + 2;

    for (int i = 0; i < slices; ++i) {
        // Tapa inferior
        *idxIter++ = baseIndex + i;
        *idxIter++ = baseIndex + i + 1;
        *idxIter++ = centerBottom;

        // Tapa superior
        *idxIter++ = baseIndex + slices + 1 + i + 1;
        *idxIter++ = baseIndex + slices + 1 + i;
        *idxIter++ = centerTop;
    }
}

inline void getConeVbIbLen(int slices, int stacks, int& vbLen, int& ibLen) {
    assert(slices > 2);
    assert(stacks >= 1);
    vbLen = (slices + 1) * (stacks + 1) + (slices + 1) + 1; // +1 for center point
    ibLen = slices * stacks * 6 + slices * 3; // 3 indices per slice for base
}

template<typename VtxOutIter, typename IdxOutIter>
void makeCone(float radius, float height, int slices, int stacks, VtxOutIter vtxIter, IdxOutIter idxIter) {
    using namespace std;
    assert(slices > 2);
    assert(stacks >= 1);

    const double radPerSlice = 2 * CS175_PI / slices;
    const double heightPerStack = height / stacks;
    const double radiusPerStack = radius / stacks;

    vector<double> sliceSin(slices + 1), sliceCos(slices + 1);
    for (int i = 0; i < slices + 1; ++i) {
        sliceSin[i] = sin(radPerSlice * i);
        sliceCos[i] = cos(radPerSlice * i);
    }

    // Superficie lateral
    for (int i = 0; i < slices + 1; ++i) {
        for (int j = 0; j < stacks + 1; ++j) {
            float currentRadius = radius - radiusPerStack * j;
            float x = sliceCos[i] * currentRadius / radius;
            float z = sliceSin[i] * currentRadius / radius;
            float y = heightPerStack * j - height / 2.0f;

            Cvec3f n(sliceCos[i], radius / height, sliceSin[i]);
            normalize(n);

            Cvec3f t(-sliceSin[i], 0, sliceCos[i]);
            Cvec3f b = cross(n, t);

            *vtxIter++ = GenericVertex(
                x * radius, y, z * radius,
                n[0], n[1], n[2],
                static_cast<float>(i) / slices, static_cast<float>(j) / stacks,
                t[0], t[1], t[2],
                b[0], b[1], b[2]);

            if (i < slices && j < stacks) {
                *idxIter++ = (stacks + 1) * i + j;
                *idxIter++ = (stacks + 1) * i + j + 1;
                *idxIter++ = (stacks + 1) * (i + 1) + j + 1;

                *idxIter++ = (stacks + 1) * i + j;
                *idxIter++ = (stacks + 1) * (i + 1) + j + 1;
                *idxIter++ = (stacks + 1) * (i + 1) + j;
            }
        }
    }

    // Base del cono
    int baseIndex = (slices + 1) * (stacks + 1);

    // Vértices de la base
    for (int i = 0; i < slices + 1; ++i) {
        float x = sliceCos[i];
        float z = sliceSin[i];
        float y = -height / 2.0f;

        *vtxIter++ = GenericVertex(
            x * radius, y, z * radius,
            0, -1, 0,
            static_cast<float>(i) / slices, 0,
            1, 0, 0,
            0, 0, 1);
    }

    // Centro de la base
    *vtxIter++ = GenericVertex(
        0, -height / 2.0f, 0,
        0, -1, 0,
        0.5, 0.5,
        1, 0, 0,
        0, 0, 1);

    // Índices para la base
    int centerIndex = baseIndex + slices + 1;
    for (int i = 0; i < slices; ++i) {
        *idxIter++ = baseIndex + i;
        *idxIter++ = baseIndex + i + 1;
        *idxIter++ = centerIndex;
    }
}

#endif