/*
 * Scilab ( http://www.scilab.org/ ) - This file is part of Scilab
 * Copyright (C) 2012 - Pedro Arthur dos S. Souza
 * Copyright (C) 2012 - Caio Lucas dos S. Souza
 *
 * This file must be used under the terms of the CeCILL.
 * This source file is licensed as described in the file COPYING, which
 * you should have received as part of this distribution.  The terms
 * are also available at
 * http://www.cecill.info/licences/Licence_CeCILL_V2-en.txt
 *
 */

extern "C"
{
#include <stdio.h>
#include <math.h>

#include "getGraphicObjectProperty.h"
#include "graphicObjectProperties.h"

    double pickSurface(char * uid, double x, double y,  double z, double dx, double dy, double dz, double mx, double my, double mz, double mw);
}

#define EPS 1e-8

class Vec3
{
public:
    double x, y, z;

    Vec3(): x(0), y(0), z(0) {}
    Vec3(double _x, double _y, double _z): x(_x), y(_y), z(_z) {}
    
    Vec3 operator - (Vec3 v) 
    {
        return Vec3( x - v.x, y - v.y, z - v.z );
    }

    Vec3 operator + (Vec3 v) 
    {
        return Vec3( x + v.x, y + v.y, z + v.z );
    }

    Vec3 operator * (double s) 
    {
        return Vec3( x*s, y*s, z*s );
    }

    Vec3 operator / (double s) 
    {
        return Vec3( x/s, y/s, z/s );
    }

    double dot(Vec3 v)
    {
        return x*v.x + y*v.y + z*v.z;
    }

    Vec3 cross(Vec3 v)
    {
        return Vec3(y*v.z - z*v.y, z*v.x - x*v.z, x*v.y - y*v.x);
    }

    Vec3& normalize()
    {
        double d = sqrt(this->dot(*this));
        if (d < EPS)
        {
            x = y = z = 0;
        }
        else
        {
            x/=d;
            y/=d;
            z/=d;
        }
        return *this;
    }

    Vec3 getNormalized()
    {
        Vec3 n = Vec3(x, y, z);
        return n.normalize();
    }

    void print()
    {
        printf("\nv = %.8f, %.8f, %.8f", x, y, z);
    }
};

typedef Vec3 Vector3d;
int test_tri(Vec3 V1, Vec3 V2, Vec3 V3, Vec3 Dir, Vec3 P0, Vec3 &ret);

/*
 * Given a ray (point(x, y,z) + direction(dx, dy, dz))
 * check if the ray intersects any triangle from the given surface.
 * returns the projected Z from the intersection point (p) (p.x*mx + p.y*my + p.z*mz + mw;)
 * or 2.0 if there isn't intersection (projected z vary between -1.0 - 1.0).
 */

double pickSurface(char * uid, double x, double y,  double z, double dx, double dy, double dz, double mx, double my, double mz, double mw)
{
    double* X = NULL;
    double* Y = NULL;
    double* Z = NULL;

    int numX = 0;
    int* piNumX = &numX;
    int numY = 0;
    int* piNumY = &numY;

    getGraphicObjectProperty(uid, __GO_DATA_MODEL_NUM_X__, jni_int, (void**) &piNumX);
    getGraphicObjectProperty(uid, __GO_DATA_MODEL_NUM_Y__, jni_int, (void**) &piNumY);

    if (numX == 0 || numY == 0)
    {
        return 2.0;
    }

    getGraphicObjectProperty(uid, __GO_DATA_MODEL_X__, jni_double_vector, (void**) &X);
    getGraphicObjectProperty(uid, __GO_DATA_MODEL_Y__, jni_double_vector, (void**) &Y);
    getGraphicObjectProperty(uid, __GO_DATA_MODEL_Z__, jni_double_vector, (void**) &Z);

    double lastZ = 2.0;

    /* for each quad in the mesh separate it in 2 triangles
     * and use test_tri function to test intersection from
     * mouse click ray
     * A point (x, y, z)  at (n,m) is given by
     * (X[n], Y[m], Z[n][m]) where X, Y are vectors and Z a matrix.
     */
    for (int i = 0; i < (numX-1); ++i)
    {
        for (int j = 0; j < (numY-1); ++j)
        {
            Vector3d P0 = Vector3d(X[i],   Y[j],   Z[i + j*numX]);
            Vector3d P1 = Vector3d(X[i+1], Y[j],   Z[(i+1) + j*numX]);
            Vector3d P2 = Vector3d(X[i+1], Y[j+1], Z[(i+1) + (j+1)*numX]);
            Vector3d P3 = Vector3d(X[i],   Y[j+1], Z[i + (j+1)*numX]);
            Vector3d ret;

            /*test first triangle*/
            if (test_tri(P0, P1, P2, Vec3(dx, dy, dz), Vec3(x, y, z), ret) == 1)
            {
                /* ray intersects the triangle, then we project only the Z cordinate
                 * and store the nearest projected Z.
                 */
                double curZ = ret.x*mx + ret.y*my + ret.z*mz + mw;
                lastZ = lastZ < curZ ? lastZ : curZ;
            }
            /*test second triangle*/
            if (test_tri(P0, P2, P3, Vec3(dx, dy, dz), Vec3(x, y, z), ret) == 1)
            {
                double curZ = ret.x*mx + ret.y*my + ret.z*mz + mw;
                lastZ = lastZ < curZ ? lastZ : curZ;
            }

        }
    }


    return lastZ;

}

/*
 * Test if the ray intersects the given triangle (V1, V2, V3)
 * Fast, minimum storage ray/triangle intersection
 * algorithm propose by Tomas Möller and Ben Trumbore.
 * Calculate barycentric cordinates (u, v) and test if
 * 0 <= u <= 1 && 0 <= v <= 1 && (u + v) <= 1, then the
 * intersection point is inside the triangle. 
 */
int test_tri(Vec3 V1, Vec3 V2, Vec3 V3, Vec3 Dir, Vec3 P0, Vec3 &ret)
{
    Vec3 Edge1, Edge2, tVec, pVec, qVec;
    double det, inv_det, t, u, v;

    Edge1 = V2 - V1;
    Edge2 = V3 - V1;

    pVec = Dir.cross(Edge2);
    det = Edge1.dot(pVec);

    if (det > -EPS && det < EPS)
        return 0;

    inv_det = 1/det;

    tVec = P0 - V1;
    u = tVec.dot(pVec) * inv_det;

    if (u < 0.0 || u > 1.0)
        return 0;

    qVec = tVec.cross(Edge1);
    v = Dir.dot(qVec) * inv_det;

    if (v < 0.0 || (u+v) > 1.0)
        return 0;

    t = Edge2.dot(qVec) * inv_det;
    ret = P0 + Dir * t;

    return 1;
}