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

#include "mathlib.h"

double VectorLength (const TVector3 &v) {
	return sqrt (v.x * v.x + v.y * v.y + v.z * v.z);
}

double VectorLength (double x, double y, double z) {
	return sqrt (x * x + y * y + z * z);
}

TVector3 ProjectToPlane (TVector3 nml, TVector3 v){
    TVector3 nmlComp;
    double dotProd;

    dotProd = DotProduct (nml, v);
    nmlComp = ScaleVector (dotProd, nml);

    return SubtractVectors (v, nmlComp);
} 

double NormVector (TVector3 *v) {
	double denom = (v->x * v->x + v->y * v->y + v->z * v->z);
	if (denom <= 0.0) return 0.0;
	denom = sqrt (denom);
	*v = ScaleVector (1.0 / denom, *v);
	return denom;
}

double NormVectorN (TVector3 &v) {
	double denom = (v.x * v.x + v.y * v.y + v.z * v.z);
	if (denom <= 0.0) return 0.0;
	denom = sqrt (denom);
	v = ScaleVector (1.0 / denom, v);
	return denom;
}

bool IntersectPlanes (TPlane s1, TPlane s2, TPlane s3, TVector3 *p){
    double A[3][4];
    double x[3];
    double retval;

    A[0][0] =  s1.nml.x;
    A[0][1] =  s1.nml.y;
    A[0][2] =  s1.nml.z;
    A[0][3] = -s1.d;

    A[1][0] =  s2.nml.x;
    A[1][1] =  s2.nml.y;
    A[1][2] =  s2.nml.z;
    A[1][3] = -s2.d;

    A[2][0] =  s3.nml.x;
    A[2][1] =  s3.nml.y;
    A[2][2] =  s3.nml.z;
    A[2][3] = -s3.d;

    retval = Gauss ((double*) A, 3, x);

    if  (retval != 0) {
	return false;
    } else {
	p->x = x[0];
	p->y = x[1];
	p->z = x[2];
	return true;
    }
}

void MultiplyRotationMatrices (TMatrix mat, TMatrix inv, double angle, char axis) {
  if(mat) {
    TMatrix r;
    MakeRotationMatrix( r, angle, axis );
    MultiplyMatrices( mat, mat, r );
  }
  if(inv) {
    TMatrix ir;
    MakeRotationMatrix( ir, -angle, axis );
    MultiplyMatrices( inv, ir, inv );
  }
}

void MultiplyTranslationMatrices(TMatrix mat, TMatrix inv, double x, double y, double z) {
  if(mat) {
    for (int i=0; i<4; i++ ) {
      mat[3][i] += mat[0][i] * x
                 + mat[1][i] * y
                 + mat[2][i] * z;
    }
  }
  if(inv) {
    inv[3][0] -= inv[3][3] * x;
    inv[3][1] -= inv[3][3] * y;
    inv[3][2] -= inv[3][3] * z;
  }
}

void MultiplyScalingMatrices(TMatrix mat, TMatrix inv, double x, double y, double z) {
  if(mat) {
    for (int i=0; i<4; i++) {
      mat[0][i] *= x;
      mat[1][i] *= y;
      mat[2][i] *= z;
    }
  }
  if(inv) {
    double ix = 1.0 / x, iy = 1.0 / y, iz = 1.0 / z;
    for (int j=0; j<4; j++) {
      inv[j][0] *= ix;
      inv[j][1] *= iy;
      inv[j][2] *= iz;
    }
  }
}

void TransposeMatrix (TMatrix mat, TMatrix trans) {
    if( trans != mat )
    {
        for (int i=0 ; i< 4 ; i++)
            for (int j=0 ; j< 4 ; j++)
                trans[j][i]= mat[i][j];
    }
    else
    {
        double m01 = trans[0][1]; double m10 = trans[1][0];
        double m02 = trans[0][2]; double m20 = trans[2][0];
        double m03 = trans[0][3]; double m30 = trans[3][0];
        double m12 = trans[1][2]; double m21 = trans[2][1];
        double m13 = trans[1][3]; double m31 = trans[3][1];
        double m23 = trans[2][3]; double m32 = trans[3][2];
        trans[1][0] = m01; trans[0][1] = m10;
        trans[2][0] = m02; trans[0][2] = m20;
        trans[3][0] = m03; trans[0][3] = m30;
        trans[2][1] = m12; trans[1][2] = m21;
        trans[3][1] = m13; trans[1][3] = m31;
        trans[3][2] = m23; trans[2][3] = m32;
    }
}

void MakeRotationMatrix (TMatrix mat, double angle, char axis){
    double sinv, cosv;
    sinv = sin (ANGLES_TO_RADIANS (angle));
    cosv = cos (ANGLES_TO_RADIANS (angle));

    MakeIdentityMatrix (mat);

    switch (axis) {
    case 'x':
        mat[1][1] = cosv;
        mat[2][1] = -sinv;
        mat[1][2] = sinv;
        mat[2][2] = cosv;
        break;

    case 'y':
        mat[0][0] = cosv;
        mat[2][0] = sinv;
        mat[0][2] = -sinv;
        mat[2][2] = cosv;
        break;

    case 'z': 
        mat[0][0] = cosv;
        mat[1][0] = -sinv;
        mat[0][1] = sinv;
        mat[1][1] = cosv;
        break;
    }
} 

void MakeTranslationMatrix (TMatrix mat, double x, double y, double z){
    MakeIdentityMatrix (mat);
    mat[3][0] = x;
    mat[3][1] = y;
    mat[3][2] = z;
} 

void MakeScalingMatrix (TMatrix mat, double x, double y, double z){
    MakeIdentityMatrix (mat);
    mat[0][0] = x;
    mat[1][1] = y;
    mat[2][2] = z;
} 

void MakeBasisMat (TMatrix mat, TVector3 w1, TVector3 w2, TVector3 w3) {
    MakeIdentityMatrix (mat);
    mat[0][0] = w1.x;
    mat[0][1] = w1.y;
    mat[0][2] = w1.z;
    mat[1][0] = w2.x;
    mat[1][1] = w2.y;
    mat[1][2] = w2.z;
    mat[2][0] = w3.x;
    mat[2][1] = w3.y;
    mat[2][2] = w3.z;
} 

void MakeBasismatrix_Inv (TMatrix mat, TMatrix invMat, 
		TVector3 w1, TVector3 w2, TVector3 w3){
    MakeIdentityMatrix (mat);
    mat[0][0] = w1.x;
    mat[0][1] = w1.y;
    mat[0][2] = w1.z;
    mat[1][0] = w2.x;
    mat[1][1] = w2.y;
    mat[1][2] = w2.z;
    mat[2][0] = w3.x;
    mat[2][1] = w3.y;
    mat[2][2] = w3.z;

    MakeIdentityMatrix (invMat);
    invMat[0][0] = w1.x;
    invMat[1][0] = w1.y;
    invMat[2][0] = w1.z;
    invMat[0][1] = w2.x;
    invMat[1][1] = w2.y;
    invMat[2][1] = w2.z;
    invMat[0][2] = w3.x;
    invMat[1][2] = w3.y;
    invMat[2][2] = w3.z;
} 

void RotateAboutVectorMatrix (TMatrix mat, TVector3 u, double angle) {
    TMatrix rx, irx, ry, iry;
    double a, b, c, d, bd, cd;

    a = u.x;
    b = u.y;
    c = u.z;

    d = sqrt (b*b + c*c);

    if  (d < EPS) {
        angle = (a < 0 ? -angle : angle);
        MakeRotationMatrix (mat, angle, 'x');
        return;
    }

    MakeRotationMatrix (mat, angle, 'z');
    bd = b/d;
    cd = c/d;

    //MakeIdentityMatrix (ry);
    ry[0][0] =   d; ry[0][1] = 0.0; ry[0][2] =   a; ry[0][3] = 0.0;
    ry[1][0] = 0.0; ry[1][1] = 1.0; ry[1][2] = 0.0; ry[1][3] = 0.0;
    ry[2][0] =  -a; ry[2][1] = 0.0; ry[2][2] =   d; ry[2][3] = 0.0;
    ry[3][0] = 0.0; ry[3][1] = 0.0; ry[3][2] = 0.0; ry[3][3] = 1.0;
    MultiplyMatrices (mat, mat, ry);

    //MakeIdentityMatrix (rx);
    rx[0][0] = 1.0; rx[0][1] = 0.0; rx[0][2] = 0.0; rx[0][3] = 0.0;
    rx[1][0] = 0.0; rx[1][1] =  cd; rx[1][2] =  bd; rx[1][3] = 0.0;
    rx[2][0] = 0.0; rx[2][1] = -bd; rx[2][2] =  cd; rx[2][3] = 0.0;
    rx[3][0] = 0.0; rx[3][1] = 0.0; rx[3][2] = 0.0; rx[3][3] = 1.0;
    MultiplyMatrices (mat, mat, rx);

    //MakeIdentityMatrix (iry);
    iry[0][0] =   d; iry[0][1] = 0.0; iry[0][2] =  -a; iry[0][3] = 0.0;
    iry[1][0] = 0.0; iry[1][1] = 1.0; iry[1][2] = 0.0; iry[1][3] = 0.0;
    iry[2][0] =   a; iry[2][1] = 0.0; iry[2][2] =   d; iry[2][3] = 0.0;
    iry[3][0] = 0.0; iry[3][1] = 0.0; iry[3][2] = 0.0; iry[3][3] = 1.0;
    MultiplyMatrices (mat, iry, mat);

    //MakeIdentityMatrix (irx);
    irx[0][0] = 1.0; irx[0][1] = 0.0; irx[0][2] = 0.0; irx[0][3] = 0.0;
    irx[1][0] = 0.0; irx[1][1] =  cd; irx[1][2] = -bd; irx[1][3] = 0.0;
    irx[2][0] = 0.0; irx[2][1] =  bd; irx[2][2] =  cd; irx[2][3] = 0.0;
    irx[3][0] = 0.0; irx[3][1] = 0.0; irx[3][2] = 0.0; irx[3][3] = 1.0;
    MultiplyMatrices (mat, irx, mat);
}

void MakeMatrixFromQuaternion (TMatrix mat, TQuaternion q){
    mat[0][0] = 1.0 - 2.0 *  (q.y * q.y + q.z * q.z);
    mat[1][0] =       2.0 *  (q.x * q.y - q.w * q.z);
    mat[2][0] =       2.0 *  (q.x * q.z + q.w * q.y);

    mat[0][1] =       2.0 *  (q.x * q.y + q.w * q.z);
    mat[1][1] = 1.0 - 2.0 *  (q.x * q.x + q.z * q.z);
    mat[2][1] =       2.0 *  (q.y * q.z - q.w * q.x);

    mat[0][2] =       2.0 *  (q.x * q.z - q.w * q.y);
    mat[1][2] =       2.0 *  (q.y * q.z + q.w * q.x);
    mat[2][2] = 1.0 - 2.0 *  (q.x * q.x + q.y * q.y);

    mat[3][0] = mat[3][1] = mat[3][2] = 0.0;
    mat[0][3] = mat[1][3] = mat[2][3] = 0.0;
    mat[3][3] = 1.0;
}

TQuaternion MakeQuaternionFromMatrix (TMatrix m){
    TQuaternion res;
    double  tr, s, q[4];
    int    i, j, k;

    static int nxt[3] = {1, 2, 0};

    tr = m[0][0] + m[1][1] + m[2][2];

    if (tr > 0.0) {
	s = sqrt (tr + 1.0);
	res.w = 0.5 * s;
	s = 0.5 / s;
	res.x = (m[1][2] - m[2][1]) * s;
	res.y = (m[2][0] - m[0][2]) * s;
	res.z = (m[0][1] - m[1][0]) * s;
    } else {                
	i = 0;
	if (m[1][1] > m[0][0]) i = 1;
	if (m[2][2] > m[i][i]) i = 2;
	j = nxt[i];
	k = nxt[j];

	s = sqrt (m[i][i] - m[j][j] - m[k][k] + 1.0);
                       
	q[i] = s * 0.5;
                             
	if (s != 0.0) s = 0.5 / s;

	q[3] = (m[j][k] - m[k][j]) * s;
	q[j] = (m[i][j] + m[j][i]) * s;
	q[k] = (m[i][k] + m[k][i]) * s;

	res.x = q[0];
	res.y = q[1];
	res.z = q[2];
	res.w = q[3];
    }

    return res;
}

TQuaternion MakeRotationQuaternion (TVector3 s, TVector3 t){
    TQuaternion res;
    TVector3 u;
    double cos2phi, sin2phi;
    double cosphi, sinphi;

    u = CrossProduct (s, t);
    sin2phi = NormVector (&u);

    if  (sin2phi < EPS) {
	res = MakeQuaternion (0., 0., 0., 1.);
    } else {
	cos2phi = DotProduct (s, t);

	sinphi = sqrt ( (1 - cos2phi) / 2.0);
	cosphi = sqrt ( (1 + cos2phi) / 2.0);

	res.x = sinphi * u.x;
	res.y = sinphi * u.y;
	res.z = sinphi * u.z;
	res.w = cosphi;
    }

    return res;
}

TQuaternion InterpolateQuaternions (TQuaternion q, 
		TQuaternion r, double t){
    TQuaternion res;
    double cosphi;
    double sinphi;
    double phi;
    double scale0, scale1;

    cosphi = q.x * r.x + q.y * r.y + q.z * r.z + q.w * r.w;

    if  (cosphi < 0.0) {
	cosphi = -cosphi;
	r.x = -r.x;
	r.y = -r.y;
	r.z = -r.z;
	r.w = -r.w;
    }

    if  (1.0 - cosphi > EPS) {
	phi = acos (cosphi);
	sinphi = sin (phi);
	scale0 = sin (phi *  (1.0 - t)) / sinphi;
	scale1 = sin (phi * t) / sinphi;
    } else {
	scale0 = 1.0 - t;
	scale1 = t;
    }

    res.x = scale0 * q.x + scale1 * r.x; 
    res.y = scale0 * q.y + scale1 * r.y; 
    res.z = scale0 * q.z + scale1 * r.z; 
    res.w = scale0 * q.w + scale1 * r.w; 

    return res;
}

TVector3 RotateVector (TQuaternion q, TVector3 v){
    TVector3 res;
    TQuaternion res_q;
    TQuaternion qs;
    TQuaternion p;

    p.x = v.x;
    p.y = v.y;
    p.z = v.z;
    p.w = 1.0;

    qs.x = -q.x;
    qs.y = -q.y;
    qs.z = -q.z;
    qs.w =  q.w;

    res_q = MultiplyQuaternions (q, MultiplyQuaternions (p, qs));
    res.x = res_q.x;
    res.y = res_q.y;
    res.z = res_q.z;

    return res;
}

// --------------------------------------------------------------------
//				 Gauss
// --------------------------------------------------------------------

unsigned short order (double *matrix, int n, int pivot);
void elim (double *matrix, int n, int pivot);
void backsb (double *matrix, int n, double *soln);

int Gauss(double *matrix, int n, double *soln){
    int pivot=0;
    unsigned short error=0;

    while ((pivot<(n-1)) && (!error)) {
		if(!(error = order(matrix,n,pivot))) {
		    elim(matrix,n,pivot);
		    pivot++;
		}
    }
    if (error) {
		return 1;
    } else {
		backsb(matrix, n, soln);
    }
    return 0;
}

unsigned short order (double *matrix, int n, int pivot){
    int row, rmax, k;
    double temp;
    unsigned short error=0;

    rmax = pivot;

    for (row=pivot+1; row<n; row++) {
		if (fabs(*(matrix+row*(n+1)+pivot)) > fabs(*(matrix+rmax*(n+1)+pivot)))
		    rmax = row;
	}

    if (fabs(*(matrix+rmax*(n+1)+pivot)) < EPS)
		error = 1;
    else if (rmax != pivot) {
		for (k=0; k<(n+1); k++)
		{
		    temp = *(matrix+rmax*(n+1)+k);
		    *(matrix+rmax*(n+1)+k) = *(matrix+pivot*(n+1)+k);
		    *(matrix+pivot*(n+1)+k) = temp;
		}
    }
    return error;
}

void elim (double *matrix, int n, int pivot){
    int row, col;
    double factor;

    for (row = pivot+1; row < n; row++) {
		factor = (*(matrix+row*(n+1)+pivot))/(*(matrix+pivot*(n+1)+pivot));
		*(matrix+row*(n+1)+pivot)=0.0;
		for (col=pivot+1l; col<n+1; col++) {
		    *(matrix+row*(n+1)+col) = *(matrix+row*(n+1)+col) - 
			(*(matrix+pivot*(n+1)+col))*factor;
		}
    }
}


void backsb (double *matrix, int n, double *soln){
    int row, col;

    for (row = n-1; row >=0; row--){
		for (col = n-1; col >= row+1; col--) {
		    *(matrix+row*(n+1)+(n)) = *(matrix+row*(n+1)+n) - 
			(*(soln+col))*(*(matrix+row*(n+1)+col));
		}
		*(soln+row) = (*(matrix+row*(n+1)+n))/(*(matrix+row*(n+1)+row));
    }
}

// ***************************************************************************
// ***************************************************************************

#define MAG_SQD2(vec) ((vec).x * (vec).x + \
       (vec).y * (vec).y + (vec).z * (vec).z )

bool IntersectPolygon (TPolygon p, TVector3 *v) {
    TRay ray; 
    TVector3 nml, edge_nml, edge_vec;
    TVector3 pt;
    double d, s, nuDotProd, wec;
    double edge_len, t, distsq;
    int i;

    nml = MakeNormal (p, v);
    ray.pt = MakeVector (0., 0., 0.);
    ray.vec = nml;

    nuDotProd = DotProduct (nml, ray.vec);
    if  (fabs(nuDotProd) < EPS)
        return false;

    d = - (nml.x * v[p.vertices[0]].x + 
           nml.y * v[p.vertices[0]].y + 
           nml.z * v[p.vertices[0]].z);

    if  (fabs (d) > 1) return false;

    for  (i=0; i < p.num_vertices; i++) {
		TVector3 *v0, *v1;

		v0 = &v[p.vertices[i]];
		v1 = &v[p.vertices[ (i+1) % p.num_vertices ]]; 

		edge_vec = SubtractVectors (*v1, *v0);
		edge_len = NormVector (&edge_vec);

		t = - DotProduct (*((TVector3 *) v0), edge_vec);

		if  (t < 0) {
			distsq = MAG_SQD2 (*v0);
		} else if  (t > edge_len) {
			distsq = MAG_SQD2 (*v1);
		} else {
			*v0 = AddVectors (*v0, ScaleVector (t, edge_vec));
			distsq = MAG_SQD2 (*v0);
		}

		if  (distsq <= 1) return true;
    }

    s = - (d + DotProduct (nml, MakeVector (ray.pt.x, ray.pt.y, ray.pt.z))) / nuDotProd;
    pt = AddVectors (ray.pt, ScaleVector (s, ray.vec));

    for  (i=0; i < p.num_vertices; i++) {
        edge_nml = CrossProduct (nml, 
            SubtractVectors (v[p.vertices[ (i+1) % p.num_vertices ]], v[p.vertices[i]]));

        wec = DotProduct (SubtractVectors (pt, v[p.vertices[i]]), edge_nml);
        if (wec < 0) return false;
    } 
    return true;
} 

bool IntersectPolyhedron (TPolyhedron p) {
    bool hit = false;
    int i;
    for (i=0; i<p.num_polygons; i++) {
        hit = IntersectPolygon (p.polygons[i], p.vertices);
        if  (hit == true) break;
    } 
    return hit;
} 

TVector3 MakeNormal (TPolygon p, TVector3 *v) {
    TVector3 normal, v1, v2;

    v1 = SubtractVectors (v[p.vertices[1]], v[p.vertices[0]]);
    v2 = SubtractVectors (v[p.vertices[p.num_vertices-1]], v[p.vertices[0]]);
    normal = CrossProduct (v1, v2);

    NormVector (&normal);
    return normal;
} 


TPolyhedron CopyPolyhedron (TPolyhedron ph) {
    TPolyhedron newph = ph;
    newph.vertices = (TVector3 *) malloc (sizeof(TVector3) * ph.num_vertices);
    for (int i=0; i<ph.num_vertices; i++) newph.vertices[i] = ph.vertices[i];
    return newph;
} 

void FreePolyhedron (TPolyhedron ph) {
    free(ph.vertices);
} 

void TransPolyhedron (TMatrix mat, TPolyhedron ph) {
    for (int i=0; i<ph.num_vertices; i++)
        ph.vertices[i] = TransformPoint (mat, ph.vertices[i]);
} 

// --------------------------------------------------------------------
//					ode solver
// --------------------------------------------------------------------

const double ode23_time_step_mat[] = { 0., 1./2., 3./4., 1. };
const double ode23_coeff_mat[][4] = {
    {0.0, 1./2.,   0.0,  2./9.},
    {0.0,   0.0, 3./4.,  1./3.},
    {0.0,   0.0,   0.0,  4./9.},
    {0.0,   0.0,   0.0,    0.0} 
};

const double ode23_error_mat[] = {-5./72., 1./12., 1./9., -1./8. };
const double ode23_time_step_exp = 1./3.;

TOdeData* ode23_NewOdeData() {
    TOdeData23 *data;
    data = (TOdeData23*)malloc(sizeof(TOdeData23));
    return (TOdeData*) data;
}

int ode23_NumEstimates() {return 4; }

void ode23_InitOdeData (TOdeData *p, double init_val, double h) {
    TOdeData23 *data = (TOdeData23*)p;
    data->init_val = init_val;
    data->h = h;
}

double ode23_NextTime(TOdeData *p, int step) {
    TOdeData23 *data = (TOdeData23*)p;
    return ode23_time_step_mat[step] * data->h;
}

double ode23_NextValue (TOdeData *p, int step) {
    TOdeData23 *data = (TOdeData23*)p;
    double val = data->init_val;
    int i;

    for  (i=0; i<step; i++) val += ode23_coeff_mat[i][step] * data->k[i];
    return val;
}

void ode23_UpdateEstimate(TOdeData *p, int step, double val) {
    TOdeData23 *data = (TOdeData23*)p;
    data->k[step] = data->h * val;
}

double ode23_FinalEstimate(TOdeData *p) {
    TOdeData23 *data = (TOdeData23*)p;
    double val = data->init_val;
    int i;

    for (i=0; i<3; i++) val += ode23_coeff_mat[i][3] * data->k[i];
    return val;
}

double ode23_EstimateError(TOdeData *p){
    TOdeData23 *data = (TOdeData23*)p;
    double err=0.;
    int i;

    for  (i=0; i<4; i++) err += ode23_error_mat[i] * data->k[i];
    return fabs(err);
}

double ode23_TimestepExponent(){
    return ode23_time_step_exp;
}

TOdeSolver NewOdeSolver23(){
    TOdeSolver s; 
    s.NewOdeData = ode23_NewOdeData;
    s.NumEstimates = ode23_NumEstimates;
    s.InitOdeData = ode23_InitOdeData;
    s.NextTime = ode23_NextTime;
    s.NextValue = ode23_NextValue;
    s.UpdateEstimate = ode23_UpdateEstimate;
    s.FinalEstimate = ode23_FinalEstimate;
    s.EstimateError = ode23_EstimateError;
    s.TimestepExponent = ode23_TimestepExponent;
    return s;
}

double LinearInterp (const double x[], const double y[], double val, int n){
    int i;
    double m, b;

    if (val < x[0]) i = 0;
    else if (val >= x[n-1]) i = n-2;
    else for (i=0; i<n-1; i++) if (val < x[i+1]) break;

    m = (y[i+1] - y[i]) / (x[i+1] - x[i]);
    b = y[i] - m * x[i];

    return m * val + b;
}

double XRandom (float min, float max) {
	return (double)rand () / RAND_MAX * (max - min) + min; 
}

double FRandom () {
	return (double)rand () / RAND_MAX; 
}

int IRandom (int min, int max) {
	return (int)XRandom ((double)min, (double)max + 0.999999);
}

int ITrunc (int val, int base) {
	return (int)(val / base);
}

int IFrac (int val, int base) {
	return val - ITrunc (val, base) * base;
}
