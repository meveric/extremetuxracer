/* --------------------------------------------------------------------
EXTREME TUXRACER

Copyright (C) 1999-2001 Jasmin F. Patry (Tuxracer)
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

#include "ogl.h"
#include "spx.h"

typedef struct {
    char name[40];
    GLenum value;
    GLenum type;
} gl_value_t;

gl_value_t gl_values[] = {
    { "maximum lights", GL_MAX_LIGHTS, GL_SHORT },
    { "modelview stack depth", GL_MAX_MODELVIEW_STACK_DEPTH, GL_SHORT },
    { "projection stack depth", GL_MAX_PROJECTION_STACK_DEPTH, GL_SHORT },
    { "max texture size", GL_MAX_TEXTURE_SIZE, GL_SHORT },
#if !defined(HAVE_GL_GLES1)
    { "double buffering", GL_DOUBLEBUFFER, GL_UNSIGNED_BYTE },
#endif
    { "red bits", GL_RED_BITS, GL_SHORT },
    { "green bits", GL_GREEN_BITS, GL_SHORT },
    { "blue bits", GL_BLUE_BITS, GL_SHORT },
    { "alpha bits", GL_ALPHA_BITS, GL_SHORT },
    { "depth bits", GL_DEPTH_BITS, GL_SHORT },
    { "stencil bits", GL_STENCIL_BITS, GL_SHORT } };

void check_gl_error(const char *title) {
    GLenum error;
    error = glGetError();
    if (error != GL_NO_ERROR) {
	char str[256];
	if (NULL != title) {
		snprintf (str, sizeof(str), "%d [%s]", error, title);
	} else {
		snprintf (str, sizeof(str), "%d", error);
	}
	Message ("OpenGL Error: ", str);
    }
}

void init_glfloat_array (int num, GLfloat arr[], ...) {
    int i;
    va_list args;
    va_start (args, arr);
    for (i=0; i<num; i++) arr[i] = va_arg(args, double);
    va_end (args);
}

PFNGLLOCKARRAYSEXTPROC glLockArraysEXT_p = NULL;
PFNGLUNLOCKARRAYSEXTPROC glUnlockArraysEXT_p = NULL;

typedef void (*(*get_gl_proc_fptr_t)(const GLubyte *))(); 

void InitOpenglExtensions () {
	get_gl_proc_fptr_t get_gl_proc = NULL;

	#if defined(HAVE_GL_GLES1)
	get_gl_proc = (get_gl_proc_fptr_t) eglGetProcAddress;
	#elif defined (OS_WIN32_NATIVE)
	//get_gl_proc = (get_gl_proc_fptr_t) wglGetProcAddress;
	#elif defined (HAVE_SDL)
	get_gl_proc = (get_gl_proc_fptr_t) SDL_GL_GetProcAddress;
	#endif

	if (get_gl_proc) {
		glLockArraysEXT_p = (PFNGLLOCKARRAYSEXTPROC) 
		    (*get_gl_proc)((GLubyte*) "glLockArraysEXT");
		glUnlockArraysEXT_p = (PFNGLUNLOCKARRAYSEXTPROC) 
		    (*get_gl_proc)((GLubyte*) "glUnlockArraysEXT");
	
		if (glLockArraysEXT_p != NULL && glUnlockArraysEXT_p != NULL) {
		
		} else {
		    Message ("GL_EXT_compiled_vertex_array extension NOT supported", "");
	    	    glLockArraysEXT_p = NULL;
		    glUnlockArraysEXT_p = NULL;
		}
	} else {
		Message ("No function available for obtaining GL proc addresses", "");
	}
}

void PrintGLInfo (){
    char *extensions;
    char *p, *oldp;
    int i;
    GLint int_val;
    GLfloat float_val;
    GLboolean boolean_val;
    string ss;

    Message ("Gl vendor: ", (char*)glGetString (GL_VENDOR));
    Message ("Gl renderer: ", (char*)glGetString (GL_RENDERER));
    Message ("Gl version: ", (char*)glGetString (GL_VERSION));
    extensions = NewStr ((char*) glGetString (GL_EXTENSIONS));
    Message ("", "");

    Message ("Gl extensions:", "");
    Message ("", "");
	
    oldp = extensions;
    while ((p=strchr(oldp,' '))) {
		*p='\0';
		Message (oldp,"");
		oldp = p+1;
    }
    if (*oldp) Message (oldp,"");

    free (extensions);
    Message ("", "");

    for (i=0; i<(int)(sizeof(gl_values)/sizeof(gl_values[0])); i++) {
		switch (gl_values[i].type) {
		    case GL_SHORT:
			glGetIntegerv (gl_values[i].value, &int_val);
			ss = Int_StrN (int_val);
			Message (gl_values[i].name, ss.c_str());
		    break;

		    case GL_FLOAT:
			glGetFloatv (gl_values[i].value, &float_val);
			ss = Float_StrN (float_val, 2);
			Message (gl_values[i].name, ss.c_str());
		    break;

		    case GL_UNSIGNED_BYTE:
			glGetBooleanv (gl_values[i].value, &boolean_val);
			ss = Int_StrN (boolean_val);
			Message (gl_values[i].name, ss.c_str());
		    break;

		    default:
			Message ("","");
		    break;
		}
    }
}

void set_material (TColor diffuse_colour, TColor specular_colour, double specular_exp) {
	GLfloat mat_amb_diff[4];
	GLfloat mat_specular[4];

	mat_amb_diff[0] = diffuse_colour.r;
	mat_amb_diff[1] = diffuse_colour.g;
	mat_amb_diff[2] = diffuse_colour.b;
	mat_amb_diff[3] = diffuse_colour.a; 
	glMaterialfv (GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, mat_amb_diff);

	mat_specular[0] = specular_colour.r;
	mat_specular[1] = specular_colour.g;
	mat_specular[2] = specular_colour.b;
	mat_specular[3] = specular_colour.a;
	glMaterialfv (GL_FRONT_AND_BACK, GL_SPECULAR, mat_specular);

	glMaterialf (GL_FRONT_AND_BACK, GL_SHININESS, specular_exp);

	glColor4f (diffuse_colour.r, diffuse_colour.g, diffuse_colour.b, 
	     diffuse_colour.a);
} 

void ClearRenderContext () {
	glDepthMask (GL_TRUE);
	glClearColor (colBackgr.r, colBackgr.g, colBackgr.b, colBackgr.a);
	glClearStencil (0);
	glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
}

void ClearRenderContext (TColor col) {
	glDepthMask (GL_TRUE);
	glClearColor (col.r, col.g, col.b, col.a);
	glClearStencil (0);
	glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
}

void SetupGuiDisplay (){
    double offset = 0.0;

    glMatrixMode (GL_PROJECTION);
    glLoadIdentity ();
    glOrtho (0, param.x_resolution, 0, param.y_resolution, -1.0, 1.0);
    glMatrixMode (GL_MODELVIEW);
    glLoadIdentity ();
    glTranslatef (offset, offset, -1.0);
    glColor4f (1.0, 1.0, 1.0, 1.0);
}

void Reshape (int w, int h) {
    double far_clip_dist;
    glViewport (0, 0, (GLint) w, (GLint) h );
    glMatrixMode (GL_PROJECTION );
    glLoadIdentity ();
    far_clip_dist = param.forward_clip_distance + FAR_CLIP_FUDGE_AMOUNT;
    gluPerspective (param.fov, (double)w/h, NEAR_CLIP_DIST, far_clip_dist );
    glMatrixMode (GL_MODELVIEW );
} 

void DrawStdSphere (int num_divisions) {
    double theta, phi, d_theta, d_phi, eps, twopi;
    double x, y, z;
    int div = num_divisions;
    eps = 1e-15;
    twopi = M_PI * 2.0;
    d_theta = d_phi = M_PI / div;

    int max_vertices = 2+2*2*(num_divisions+2);
    GLfloat* vtx = (GLfloat*) alloca(3*max_vertices*sizeof(GLfloat));
    GLfloat* nrm = (GLfloat*) alloca(3*max_vertices*sizeof(GLfloat));

    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_NORMAL_ARRAY);

    for  (phi = 0.0; phi + eps < M_PI; phi += d_phi) {
	double cos_theta, sin_theta;
	double sin_phi, cos_phi;
	double sin_phi_d_phi, cos_phi_d_phi;
	sin_phi = sin (phi);
	cos_phi = cos (phi);
	sin_phi_d_phi = sin (phi + d_phi);
	cos_phi_d_phi = cos (phi + d_phi);

	GLint drawMode;
	GLfloat *pvtx = vtx;
	GLfloat *pnrm = nrm;
	GLsizei num_vertices = 0;

	if  (phi <= eps) {
		drawMode = GL_TRIANGLE_FAN;
		*pnrm++ = 0.0; *pnrm++ = 0.0; *pnrm++ = 1.0;
		*pvtx++ = 0.0; *pvtx++ = 0.0; *pvtx++ = 1.0;
		num_vertices++;

		for  (theta = 0.0; theta + eps < twopi; theta += d_theta) {
			sin_theta = sin (theta);
			cos_theta = cos (theta);
			x = cos_theta * sin_phi_d_phi;
			y = sin_theta * sin_phi_d_phi;
			z = cos_phi_d_phi;
			*pnrm++ = x; *pnrm++ = y; *pnrm++ = z;
			*pvtx++ = x; *pvtx++ = y; *pvtx++ = z;
			num_vertices++;
		}

		x = sin_phi_d_phi;
		y = 0.0;
		z = cos_phi_d_phi;
		*pnrm++ = x; *pnrm++ = y; *pnrm++ = z;
		*pvtx++ = x; *pvtx++ = y; *pvtx++ = z;
		num_vertices++;
	} else if  (phi + d_phi + eps >= M_PI) {
		drawMode = GL_TRIANGLE_FAN;
		*pnrm++ = 0.0; *pnrm++ = 0.0; *pnrm++ = -1.0;
		*pvtx++ = 0.0; *pvtx++ = 0.0; *pvtx++ = -1.0;
		num_vertices++;

                for  (theta = twopi; theta - eps > 0; theta -= d_theta) {
			sin_theta = sin (theta);
			cos_theta = cos (theta);
			x = cos_theta * sin_phi;
			y = sin_theta * sin_phi;
			z = cos_phi;
			*pnrm++ = x; *pnrm++ = y; *pnrm++ = z;
			*pvtx++ = x; *pvtx++ = y; *pvtx++ = z;
			num_vertices++;
		} 

                x = sin_phi;
                y = 0.0;
                z = cos_phi;
		*pnrm++ = x; *pnrm++ = y; *pnrm++ = z;
		*pvtx++ = x; *pvtx++ = y; *pvtx++ = z;
		num_vertices++;
        } else {
		drawMode = GL_TRIANGLE_STRIP;
		for  (theta = 0.0; theta + eps < twopi; theta += d_theta) {
			sin_theta = sin (theta);
			cos_theta = cos (theta);
			x = cos_theta * sin_phi;
			y = sin_theta * sin_phi;
			z = cos_phi;
			*pnrm++ = x; *pnrm++ = y; *pnrm++ = z;
			*pvtx++ = x; *pvtx++ = y; *pvtx++ = z;
			num_vertices++;
			x = cos_theta * sin_phi_d_phi;
			y = sin_theta * sin_phi_d_phi;
			z = cos_phi_d_phi;
			*pnrm++ = x; *pnrm++ = y; *pnrm++ = z;
			*pvtx++ = x; *pvtx++ = y; *pvtx++ = z;
			num_vertices++;
                } 

		x = sin_phi;
		y = 0.0;
		z = cos_phi;
		*pnrm++ = x; *pnrm++ = y; *pnrm++ = z;
		*pvtx++ = x; *pvtx++ = y; *pvtx++ = z;
		num_vertices++;

		x = sin_phi_d_phi;
		y = 0.0;
		z = cos_phi_d_phi;
		*pnrm++ = x; *pnrm++ = y; *pnrm++ = z;
		*pvtx++ = x; *pvtx++ = y; *pvtx++ = z;
		num_vertices++;
        } 

	glNormalPointer(GL_FLOAT, 0, nrm);
	glVertexPointer(3, GL_FLOAT, 0, vtx);
	glDrawArrays(drawMode, 0, num_vertices);
    }

    glDisableClientState(GL_NORMAL_ARRAY);
    glDisableClientState(GL_VERTEX_ARRAY);
} 

// ====================================================================
//					GL options
// ====================================================================

void set_gl_options (TRenderMode mode) 
{
    switch (mode) {
    case GUI:
        glEnable (GL_TEXTURE_2D);
        glDisable (GL_DEPTH_TEST);
        glDisable (GL_CULL_FACE);
	glDisable (GL_LIGHTING);
	glDisable (GL_NORMALIZE);
	glDisable (GL_ALPHA_TEST);
        glEnable (GL_BLEND);
	glDisable (GL_STENCIL_TEST);
	glDisable (GL_COLOR_MATERIAL);
	glDepthMask (GL_TRUE);
	glShadeModel (GL_SMOOTH);
	glDepthFunc (GL_LESS);
	glDisable (GL_FOG);
        break;
    
    case GAUGE_BARS:
        glEnable (GL_TEXTURE_2D);
	glDisable (GL_DEPTH_TEST);
	glDisable (GL_CULL_FACE);
	glDisable (GL_LIGHTING);
	glDisable (GL_NORMALIZE);
	glDisable (GL_ALPHA_TEST);
        glEnable (GL_BLEND);
	glDisable (GL_STENCIL_TEST);
	glDisable (GL_COLOR_MATERIAL);
	glDepthMask (GL_TRUE);
	glShadeModel (GL_SMOOTH);
	glDepthFunc (GL_LESS);
        break;

    case TEXFONT:
        glEnable (GL_TEXTURE_2D);
        glDisable (GL_DEPTH_TEST);
        glDisable (GL_CULL_FACE);
	glDisable (GL_LIGHTING);
	glDisable (GL_NORMALIZE);
	glDisable (GL_ALPHA_TEST);
        glEnable (GL_BLEND);
	glDisable (GL_STENCIL_TEST);
	glDisable (GL_COLOR_MATERIAL);
	glDepthMask (GL_TRUE);
	glShadeModel (GL_SMOOTH);
	glDepthFunc (GL_LESS);
        break;
    
    case COURSE:
	glEnable (GL_TEXTURE_2D);
	glEnable (GL_DEPTH_TEST);
	glEnable (GL_CULL_FACE);
	glEnable (GL_LIGHTING);
	glDisable (GL_NORMALIZE);
	glDisable (GL_ALPHA_TEST);
	glEnable (GL_BLEND);
	glDisable (GL_STENCIL_TEST);
	glEnable (GL_COLOR_MATERIAL);
	glDepthMask (GL_TRUE);
	glShadeModel (GL_SMOOTH);
	glDepthFunc (GL_LEQUAL);
	break;

    case TREES:
	glEnable (GL_TEXTURE_2D);
	glEnable (GL_DEPTH_TEST);
        glDisable (GL_CULL_FACE);
	glEnable (GL_LIGHTING);
	glDisable (GL_NORMALIZE);
        glEnable (GL_ALPHA_TEST);
	glEnable (GL_BLEND);
	glDisable (GL_STENCIL_TEST);
	glDisable (GL_COLOR_MATERIAL);
	glDepthMask (GL_TRUE);
	glShadeModel (GL_SMOOTH);
	glDepthFunc (GL_LESS);

        glAlphaFunc (GL_GEQUAL, 0.5);
        break;
        
    case PARTICLES:
        glEnable (GL_TEXTURE_2D);
	glEnable (GL_DEPTH_TEST);
        glDisable (GL_CULL_FACE);
	glDisable (GL_LIGHTING);
	glDisable (GL_NORMALIZE);
	glEnable (GL_ALPHA_TEST);
        glEnable (GL_BLEND);
	glDisable (GL_STENCIL_TEST);
	glDisable (GL_COLOR_MATERIAL);
	glDepthMask (GL_TRUE);
	glShadeModel (GL_SMOOTH);
	glDepthFunc (GL_LESS);

        glAlphaFunc (GL_GEQUAL, 0.5);
        break;
    
    case SKY:
	glEnable (GL_TEXTURE_2D);
	glDisable (GL_DEPTH_TEST);
	glDisable (GL_CULL_FACE); 
	glDisable (GL_LIGHTING);
	glDisable (GL_NORMALIZE);
	glDisable (GL_ALPHA_TEST);
	glEnable (GL_BLEND);
	glDisable (GL_STENCIL_TEST);
	glDisable (GL_COLOR_MATERIAL);
	glDepthMask (GL_FALSE);
	glShadeModel (GL_SMOOTH);
	glDepthFunc (GL_LESS);
	break;
 	
    case FOG_PLANE:
	glDisable (GL_TEXTURE_2D);
	glEnable (GL_DEPTH_TEST);
	glDisable (GL_CULL_FACE); 
	glDisable (GL_LIGHTING);
	glDisable (GL_NORMALIZE);
	glDisable (GL_ALPHA_TEST);
	glEnable (GL_BLEND);
	glDisable (GL_STENCIL_TEST);
	glDisable (GL_COLOR_MATERIAL);
	glDepthMask (GL_TRUE);
	glShadeModel (GL_SMOOTH);
	glDepthFunc (GL_LESS);
	break;

    case TUX:
	glDisable (GL_TEXTURE_2D);
	glEnable (GL_DEPTH_TEST);
	glEnable (GL_CULL_FACE);
    	glEnable (GL_LIGHTING);
	glEnable (GL_NORMALIZE);
	glDisable (GL_ALPHA_TEST);
	glEnable (GL_BLEND);
	glDisable (GL_STENCIL_TEST);
	glDisable (GL_COLOR_MATERIAL);
	glDepthMask (GL_TRUE);
	glShadeModel (GL_SMOOTH);
	glDepthFunc (GL_LESS);
    	break;

    case TUX_SHADOW:
#ifdef USE_STENCIL_BUFFER
	glDisable (GL_TEXTURE_2D);
	glEnable (GL_DEPTH_TEST);
	glDisable (GL_CULL_FACE);
	glDisable (GL_LIGHTING);
	glDisable (GL_NORMALIZE);
	glDisable (GL_ALPHA_TEST);
	glEnable (GL_BLEND);
	glEnable (GL_STENCIL_TEST);
	glDisable (GL_COLOR_MATERIAL);
	glDepthMask (GL_FALSE);
	glShadeModel (GL_SMOOTH);
	glDepthFunc (GL_LESS);

	glStencilFunc (GL_EQUAL, 0, ~0);
	glStencilOp (GL_KEEP, GL_KEEP, GL_INCR);
#else
	glDisable (GL_TEXTURE_2D);
	glEnable (GL_DEPTH_TEST);
	glEnable (GL_CULL_FACE);
	glDisable (GL_LIGHTING);
	glDisable (GL_NORMALIZE);
	glDisable (GL_ALPHA_TEST);
	glEnable (GL_BLEND);
	glDisable (GL_STENCIL_TEST);
	glDisable (GL_COLOR_MATERIAL);
	glDepthMask (GL_TRUE);
	glShadeModel (GL_SMOOTH);
	glDepthFunc (GL_LESS);
#endif
	break;

    case TRACK_MARKS:
	glEnable (GL_TEXTURE_2D);
	glEnable (GL_DEPTH_TEST);
	glDisable (GL_CULL_FACE);
	glEnable (GL_LIGHTING);
	glDisable (GL_NORMALIZE);
	glDisable (GL_ALPHA_TEST);
	glEnable (GL_BLEND);
	glDisable (GL_STENCIL_TEST);
	glDisable (GL_COLOR_MATERIAL);
	glDepthMask (GL_FALSE);
	glShadeModel (GL_SMOOTH);
	glDepthFunc (GL_LEQUAL);
	break;
    
    default:
	Message ("not a valid render mode", "");
	break;
    } 
} 

/* defined but not used
    case TEXT:
        glDisable (GL_TEXTURE_2D);
        glDisable (GL_DEPTH_TEST);
        glDisable (GL_CULL_FACE);
	glDisable (GL_LIGHTING);
	glDisable (GL_NORMALIZE);
	glDisable (GL_ALPHA_TEST);
        glEnable (GL_BLEND);
	glDisable (GL_STENCIL_TEST);
	glDisable (GL_COLOR_MATERIAL);
	glDepthMask (GL_TRUE);
	glShadeModel (GL_SMOOTH);
	glDepthFunc (GL_LESS);
        break;

    case SPLASH_SCREEN:
        glDisable (GL_TEXTURE_2D);
        glDisable (GL_DEPTH_TEST);
        glDisable (GL_CULL_FACE);
	glDisable (GL_LIGHTING);
	glDisable (GL_NORMALIZE);
	glDisable (GL_ALPHA_TEST);
        glEnable (GL_BLEND);
	glDisable (GL_STENCIL_TEST);
	glDisable (GL_COLOR_MATERIAL);
	glDepthMask (GL_TRUE);
	glShadeModel (GL_SMOOTH);
	glDepthFunc (GL_LESS);
        break;

    case PARTICLE_SHADOWS:
        glDisable (GL_TEXTURE_2D);
	glEnable (GL_DEPTH_TEST);
        glDisable (GL_CULL_FACE);
	glDisable (GL_LIGHTING);
	glDisable (GL_NORMALIZE);
	glDisable (GL_ALPHA_TEST);
        glEnable (GL_BLEND);
	glDisable (GL_STENCIL_TEST);
	glDisable (GL_COLOR_MATERIAL);
	glDepthMask (GL_TRUE);
	glShadeModel (GL_SMOOTH);
	glDepthFunc (GL_LESS);
        break;

    case OVERLAYS:
	glEnable (GL_TEXTURE_2D);
	glDisable (GL_DEPTH_TEST);
	glDisable (GL_CULL_FACE);
	glDisable (GL_LIGHTING);
	glDisable (GL_NORMALIZE);
	glEnable (GL_ALPHA_TEST);
	glEnable (GL_BLEND);
	glDisable (GL_STENCIL_TEST);
	glDisable (GL_COLOR_MATERIAL);
	glDepthMask (GL_TRUE);
	glShadeModel (GL_SMOOTH);
	glDepthFunc (GL_LESS);
    	glAlphaFunc (GL_GEQUAL, 0.5);
    break;
*/
