/* --------------------------------------------------------------------
EXTREME TUXRACER

Copyright (C) 1999-2001 Jasmin F. Patry (Tuxracer)
Copyright (C) 2004-2005 Volker Stroebel (Planetpenguin Racer)
Copyright (C) 2010 Extreme Tuxracer Team

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.
---------------------------------------------------------------------*/

#ifndef MATHLIB_H
#define MATHLIB_H

#include "bh.h"

const TVector3 NullVec = {0.0, 0.0, 0.0};
const TVector3 GravVec = {0.0, -1.0, 0.0};

// --------------------------------------------------------------------
//			vector and matrix
// --------------------------------------------------------------------

inline TVector3 MakeVector (scalar_t x, scalar_t y, scalar_t z){
    TVector3 result;
    result.x = x;
    result.y = y;
    result.z = z;
    return result;
}

inline TVector2 MakeVector2 (scalar_t x, scalar_t y){
    TVector2 result;
    result.x = x;
    result.y = y;
    return result;
}

inline TVector3 MakeVector3 (scalar_t x, scalar_t y, scalar_t z) {
        return MakeVector (x, y, z);
}

inline TVector4 MakeVector4 (scalar_t x, scalar_t y, scalar_t z, scalar_t w) {
    TVector4 result;
    result.x = x;
    result.y = y;
    result.z = z;
    result.w = w;
    return result;
}

inline TColor MakeColor (scalar_t r, scalar_t g, scalar_t b, scalar_t a) {
    TColor result;
    result.r = r;
    result.g = g;
    result.b = b;
    result.a = a;
    return result;
}

inline TColor3 MakeColor3 (scalar_t r, scalar_t g, scalar_t b) {
    TColor3 result;
    result.r = r;
    result.g = g;
    result.b = b;
    return result;
}

inline TIndex2 MakeIndex2 (int i, int j) {
    TIndex2 result;
    result.i = i;
    result.j = j;
    return result;
}

inline TIndex3 MakeIndex3 (int i, int j, int k) {
    TIndex3 result;
    result.i = i;
    result.j = j;
    result.k = k;
    return result;
}

inline TVector3 ScaleVector (scalar_t s, TVector3 v){
    TVector3 rval;
    rval.x = s * v.x;
    rval.y = s * v.y;
    rval.z = s * v.z;
    return rval;
}

inline TVector3 AddVectors (TVector3 v1, TVector3 v2){
    TVector3 result;
    result.x = v1.x + v2.x;
    result.y = v1.y + v2.y;
    result.z = v1.z + v2.z;
    return result;
}

inline TVector3 SubtractVectors (TVector3 v1, TVector3 v2){
    TVector3 result;
    result.x = v1.x - v2.x;
    result.y = v1.y - v2.y;
    result.z = v1.z - v2.z;
    return result;
}

inline scalar_t DotProduct (TVector3 v1, TVector3 v2){
    return v1.x * v2.x + v1.y * v2.y + v1.z * v2.z;
}

inline TVector3 CrossProduct(TVector3 u, TVector3 v){
    TVector3 ret;
    ret.x = u.y * v.z - u.z * v.y;
    ret.y = u.z * v.x - u.x * v.z;
    ret.z = u.x * v.y - u.y * v.x;
    return ret;
}

inline scalar_t VectorSquaredLength (const TVector3 &v) {
        return (v.x * v.x + v.y * v.y + v.z * v.z);
}

inline scalar_t VectorSquaredLength (scalar_t x, scalar_t y, scalar_t z) {
        return (x * x + y * y + z * z);
}

double		VectorLength (const TVector3 &v);
double		VectorLength (double x, double y, double z);

double		NormVector (TVector3 *v);
double		NormVectorN (TVector3 &v);	// new version with reference

// --------------------------------------------------------------------
//			        transform
// --------------------------------------------------------------------

inline TVector3 TransformVector(TMatrix mat, TVector3 v) {
    TVector3 r;
    r.x = v.x * mat[0][0] + v.y * mat[1][0] + v.z * mat[2][0];
    r.y = v.x * mat[0][1] + v.y * mat[1][1] + v.z * mat[2][1];
    r.z = v.x * mat[0][2] + v.y * mat[1][2] + v.z * mat[2][2];
    return r;
}

inline TVector3 TransformNormal(TVector3 n, TMatrix mat) {
    TVector3 r;
    r.x = n.x * mat[0][0] + n.y * mat[0][1] + n.z * mat[0][2];
    r.y = n.x * mat[1][0] + n.y * mat[1][1] + n.z * mat[1][2];
    r.z = n.x * mat[2][0] + n.y * mat[2][1] + n.z * mat[2][2];
    return r;
}

inline TVector3 TransformPoint(TMatrix mat, TVector3 p) {
    TVector3 r;
    r.x = p.x * mat[0][0] + p.y * mat[1][0] + p.z * mat[2][0];
    r.y = p.x * mat[0][1] + p.y * mat[1][1] + p.z * mat[2][1];
    r.z = p.x * mat[0][2] + p.y * mat[1][2] + p.z * mat[2][2];
    r.x += mat[3][0];
    r.y += mat[3][1];
    r.z += mat[3][2];
    return r;
}

// --------------------------------------------------------------------
//			         plane
// --------------------------------------------------------------------

inline TPlane MakePlane (scalar_t nx, scalar_t ny, scalar_t nz, scalar_t d) {
    TPlane tmp;
    tmp.nml.x = nx;
    tmp.nml.y = ny;
    tmp.nml.z = nz;
    tmp.d = d;
    return tmp;
}

inline scalar_t DistanceToPlane (TPlane plane, TVector3 pt) {
    return
        plane.nml.x * pt.x +
        plane.nml.y * pt.y +
        plane.nml.z * pt.z +
        plane.d;
}

TVector3	ProjectToPlane (TVector3 nml, TVector3 v);
bool		IntersectPlanes (TPlane s1, TPlane s2, TPlane s3, TVector3 *p);

// --------------------------------------------------------------------
//			         matrix
// --------------------------------------------------------------------

inline void MultiplyMatrices (TMatrix ret, TMatrix mat1, TMatrix mat2){
    int i,j;
    TMatrix r;

    for (i= 0 ; i< 4 ; i++)
        for (j= 0 ; j< 4 ; j++)
            r[j][i]= mat1[0][i] * mat2[j][0] +
                mat1[1][i] * mat2[j][1] +
                mat1[2][i] * mat2[j][2] +
                mat1[3][i] * mat2[j][3];

    for (i= 0 ; i< 4 ; i++)
        for (j= 0 ; j< 4 ; j++)
            ret[i][j] = r[i][j];
}

inline void MakeIdentityMatrix (TMatrix h)
{
    int i,j;

    for (i= 0 ; i< 4 ; i++)
        for (j= 0 ; j< 4 ; j++)
            h[i][j] = ( i == j );
}

void MakeRotationMatrix (TMatrix mat, double angle, char axis);
void MakeTranslationMatrix (TMatrix mat, double x, double y, double z);
void MakeScalingMatrix (TMatrix mat, double x, double y, double z);

void MultiplyRotationMatrices (TMatrix mat, TMatrix inv, double angle, char axis);
void MultiplyTranslationMatrices(TMatrix mat, TMatrix inv, double x, double y, double z);
void MultiplyScalingMatrices(TMatrix mat, TMatrix inv, double x, double y, double z);

void TransposeMatrix (TMatrix mat, TMatrix trans);
void MakeBasisMat (TMatrix mat,	TVector3 w1, TVector3 w2, TVector3 w3);
void MakeBasismatrix_Inv (TMatrix mat, TMatrix invMat, TVector3 w1, TVector3 w2, TVector3 w3);
void RotateAboutVectorMatrix (TMatrix mat, TVector3 u, double angle);

// --------------------------------------------------------------------
//			       quaternion
// --------------------------------------------------------------------

inline TQuaternion MakeQuaternion (scalar_t x, scalar_t y, scalar_t z, scalar_t w) {
    TQuaternion q;
    q.x = x;
    q.y = y;
    q.z = z;
    q.w = w;
    return q;
}

inline TQuaternion MultiplyQuaternions (TQuaternion q, TQuaternion r) {
    TQuaternion res;
    res.x = q.y * r.z - q.z * r.y + r.w * q.x + q.w * r.x;
    res.y = q.z * r.x - q.x * r.z + r.w * q.y + q.w * r.y;
    res.z = q.x * r.y - q.y * r.x + r.w * q.z + q.w * r.z;
    res.w = q.w * r.w - q.x * r.x - q.y * r.y - q.z * r.z;
    return res;
}

inline TQuaternion AddQuaternions (TQuaternion q, TQuaternion r) {
    TQuaternion res;
    res.x = q.x + r.x;
    res.y = q.y + r.y;
    res.z = q.z + r.z;
    res.w = q.w + r.w;
    return res;
}

inline TQuaternion ConjugateQuaternion (TQuaternion q) {
    TQuaternion res;
    res.x = -1 * q.x;
    res.y = -1 * q.y;
    res.z = -1 * q.z;
    res.w = q.w;
    return res;
}

inline TQuaternion ScaleQuaternion (scalar_t s, TQuaternion q) {
    TQuaternion res;
    res.x = s * q.x;
    res.y = s * q.y;
    res.z = s * q.z;
    res.w = s * q.w;
    return res;
}

void        MakeMatrixFromQuaternion (TMatrix mat, TQuaternion q);
TQuaternion MakeQuaternionFromMatrix (TMatrix mat);
TQuaternion MakeRotationQuaternion (TVector3 s, TVector3 t);
TQuaternion InterpolateQuaternions (TQuaternion q, TQuaternion r, double t);
TVector3    RotateVector (TQuaternion q, TVector3 v);

bool		IntersectPolygon (TPolygon p, TVector3 *v);
bool		IntersectPolyhedron (TPolyhedron p);
TVector3	MakeNormal (TPolygon p, TVector3 *v);
TPolyhedron	CopyPolyhedron (TPolyhedron ph);
void		FreePolyhedron (TPolyhedron ph) ;
void		TransPolyhedron (TMatrix mat, TPolyhedron ph);

// --------------------------------------------------------------------
//				ode solver
// --------------------------------------------------------------------

typedef char		TOdeData;
typedef TOdeData*	(*PNewOdeData) ();
typedef int 		(*PNumEstimates) ();
typedef void 		(*PInitOdeData) (TOdeData *, double init_val, double h);
typedef double 		(*PNextTime) (TOdeData *, int step);
typedef double 		(*PNextValue) (TOdeData *, int step);
typedef void 		(*PUpdateEstimate) (TOdeData *, int step, double val);
typedef double 		(*PFinalEstimate) (TOdeData *);
typedef double 		(*PEstimateError) (TOdeData *);
typedef double 		(*PTimestepExponent) ();

typedef struct {
    PNewOdeData			NewOdeData;
    PNumEstimates		NumEstimates;
    PInitOdeData		InitOdeData;
    PNextTime			NextTime;
    PNextValue			NextValue;
    PUpdateEstimate		UpdateEstimate;
    PFinalEstimate		FinalEstimate;
    PEstimateError		EstimateError;
    PTimestepExponent	TimestepExponent;
} TOdeSolver;

typedef struct {
    double k[4];
    double init_val;
    double h;
} TOdeData23;

TOdeSolver NewOdeSolver23 ();

// --------------------------------------------------------------------
//			special
// --------------------------------------------------------------------

int Gauss (double *matrix, int n, double *soln);
double LinearInterp (const double x[], const double y[], double val, int n);

double	XRandom (float min, float max);
double	FRandom ();
int		IRandom (int min, int max);
int		ITrunc (int val, int base);
int		IFrac (int val, int base);
#endif
