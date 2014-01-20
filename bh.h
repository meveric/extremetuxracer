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

#ifndef BH_H
#define BH_H

// --------------------------------------------------------------------
//		compiler flags
// --------------------------------------------------------------------

#define HAVE_SDL 
#define HAVE_SDL_MIXER
#define HAVE_SDL_IMAGE
#define HAVE_SDL_JOYSTICK
#define TIME_WITH_SYS_TIME	
#define HAVE_GETCWD 
#define HAVE_GETTIMEOFDAY 
#define HAVE_GL_GLEXT_H 
#define HAVE_GL_GLX_H 
//#define HAVE_GL_GLES1
#define HAVE_SYS_TIME_H 
#define USE_STENCIL_BUFFER

// --------------------------------------------------------------------
//			includes
// --------------------------------------------------------------------

#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <time.h>
#include <ctype.h>
#include <string.h>
#include <limits.h>
#include <stdarg.h>
#include <float.h>
#include <sys/stat.h>

#include <map>
#include <list>
#include <string>
#include <iostream>

#if defined ( OS_LINUX )
	#include <unistd.h>
	#include <sys/types.h>
	#include <pwd.h>
	#include <dirent.h>
	#include <sys/time.h>
	#define SEP "/"
#elif defined ( OS_WIN32_MINGW )
	#define SEP "/"
	#include <dirent.h>
#elif defined ( OS_WIN32_MSC )
	#include <io.h>
	#include <direct.h>
	#include <windows.h>
	#pragma warning (disable:4244)
	#pragma warning (disable:4305)
	#pragma warning (disable:4761)
	#define SEP "\\"
#elif defined ( OS_WIN32_NATIVE )
	#include <io.h>
	#include <direct.h>
	#include <windows.h>
	#define SEP "\\"
	#define snprintf    _snprintf
	#define mkdir(x,y)  _mkdir(x)
#elif defined ( OS_MAC )
	#include <unistd.h>
	#include <sys/types.h>
	#include <pwd.h>
	#include <dirent.h>
	#include <sys/time.h>
	#define SEP "/"
#endif

#if defined ( OS_MAC )
	#include <OpenGL/gl.h>
	#include <OpenGL/glu.h>
#elif defined ( OS_WIN32_MINGW )
	#include <GL/gl.h>
	#include <GL/glu.h>
	#include <GL/glext.h>
#elif defined ( HAVE_GL_GLES1 )
	#include <EGL/egl.h>
	#include <GLES/gl.h>
	#include "SDL/SDL_syswm.h"
#else
	#include <GL/gl.h>
	#include <GL/glu.h>
#endif

#if defined ( OS_MAC )
	#include "SDL/SDL.h"
	#include "SDL/SDL_joystick.h"
	#include "SDL_image/SDL_image.h"
	#include "SDL_mixer/SDL_mixer.h"
#else
	#include "SDL/SDL.h"
	#include "SDL/SDL_joystick.h"
	#include "SDL/SDL_image.h"
	#include "SDL/SDL_mixer.h"
#endif

// --------------------------------------------------------------------
//			defines
// --------------------------------------------------------------------

#define PROG_NAME "ETR"
#define PACKAGE "etr"
#define VERSION "0.6"
#define WINDOW_TITLE "Extreme Tux Racer " VERSION
#define PROG_DIR "/usr/local/games/etr-bh"

// --------------------------------------------------------------------
//			GLES -> GL conversion
// --------------------------------------------------------------------

#if defined ( HAVE_GL_GLES1 )
	#define GL_CLAMP        GL_CLAMP_TO_EDGE
	#define glOrtho         glOrthof
	#define glFrustum       glFrustumf

	inline void gluPerspective( double fovy, double aspect, double zNear, double zFar )
	{
		GLfloat ymax = zNear * tan( fovy * M_PI / 360.0 );
		GLfloat ymin = -ymax;
		GLfloat xmin = ymin * aspect;
		GLfloat xmax = ymax * aspect;
		glFrustumf( xmin, xmax, ymin, ymax, zNear, zFar );
	}
#endif

using namespace std;

#include "etr_types.h"
#include "mathlib.h"
#include "common.h"
#include "game_config.h"
#include "winsys.h"
#include "physics.h"

extern TGameData g_game;

#endif
