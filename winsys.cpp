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

#include "winsys.h"
#include "ogl.h"
#include "audio.h"
#include "game_ctrl.h"
#include "font.h"
#include "score.h"
#include "textures.h"

#define USE_JOYSTICK true

#define COLOURDEPTH_RED_SIZE   5
#define COLOURDEPTH_GREEN_SIZE 6
#define COLOURDEPTH_BLUE_SIZE  5
#define COLOURDEPTH_DEPTH_SIZE 16

#if defined(HAVE_GL_GLES1)
EGLDisplay g_eglDisplay = 0;
EGLConfig  g_eglConfig  = 0;
EGLContext g_eglContext = 0;
EGLSurface g_eglSurface = 0;
Display*   g_x11Display = NULL;

static const EGLint g_configAttribs[] = {
  EGL_RED_SIZE,              COLOURDEPTH_RED_SIZE,
  EGL_GREEN_SIZE,            COLOURDEPTH_GREEN_SIZE,
  EGL_BLUE_SIZE,             COLOURDEPTH_BLUE_SIZE,
  EGL_DEPTH_SIZE,            COLOURDEPTH_DEPTH_SIZE,
  EGL_SURFACE_TYPE,          EGL_WINDOW_BIT,
  EGL_RENDERABLE_TYPE,       EGL_OPENGL_ES_BIT,
  EGL_BIND_TO_TEXTURE_RGBA,  EGL_TRUE,
  EGL_NONE
};
#endif

CWinsys Winsys;

CWinsys::CWinsys () {
	screen = NULL;
	for (int i=0; i<NUM_GAME_MODES; i++) {
		modefuncs[i].init   = NULL;
		modefuncs[i].loop   = NULL;
		modefuncs[i].term   = NULL;
		modefuncs[i].keyb   = NULL;
		modefuncs[i].mouse  = NULL;
		modefuncs[i].motion = NULL;
		modefuncs[i].jaxis  = NULL;
		modefuncs[i].jbutt  = NULL;
	}
	new_mode = NO_MODE;
	lasttick = 0;

	joystick = NULL;
	numJoysticks = 0;
	joystick_active = false;

 	resolution[0] = MakeRes (0, 0);
	resolution[1] = MakeRes (800, 600);
	resolution[2] = MakeRes (1024, 768);
	resolution[3] = MakeRes (1152, 864);
	resolution[4] = MakeRes (1280, 960);
	resolution[5] = MakeRes (1280, 1024);
	resolution[6] = MakeRes (1360, 768);
	resolution[7] = MakeRes (1400, 1050);
	resolution[8] = MakeRes (1440, 900);
	resolution[9] = MakeRes (1680, 1050);
	
	auto_x_resolution = 800;
	auto_y_resolution = 600;

	clock_time = 0;
	cur_time = 0;
	lasttick = 0;
	elapsed_time = 0;
	remain_ticks = 0;
}

CWinsys::~CWinsys () {}

TScreenRes CWinsys::MakeRes (int width, int height) {
	TScreenRes res;
	res.width = width;
	res.height = height;
	return res;
}

TScreenRes CWinsys::GetResolution (int idx) {
	if (idx < 0 || idx >= NUM_RESOLUTIONS) return MakeRes (800, 600);
	return resolution[idx];
}

string CWinsys::GetResName (int idx) {
	string line;
	if (idx < 0 || idx >= NUM_RESOLUTIONS) return "800 x 600";
	if (idx == 0) return ("auto");
	line = Int_StrN (resolution[idx].width);
	line += " x " + Int_StrN (resolution[idx].height);
	return line;
}

double CWinsys::CalcScreenScale () {
	double hh = (double)param.y_resolution;
	if (hh < 768) return 0.78; 
	else if (hh == 768) return 1.0;
	else return (hh / 768);
}

/*
typedef struct SDL_Surface {
    Uint32 flags;                           // Read-only 
    SDL_PixelFormat *format;                // Read-only 
    int w, h;                               // Read-only 
    Uint16 pitch;                           // Read-only 
    void *pixels;                           // Read-write 
    SDL_Rect clip_rect;                     // Read-only 
    int refcount;                           // Read-mostly
} SDL_Surface;
*/

void CWinsys::SetupVideoMode (TScreenRes resolution) {
    int bpp = 0;
    switch (param.bpp_mode) {
		case 0:	bpp = 0; break;
		case 1:	bpp = 16; break;
		case 2:	bpp = 32; break;
		default: param.bpp_mode = 0; bpp = 0;
    }

    Uint32 video_flags = ( param.fullscreen ? SDL_FULLSCREEN : 0 );
#if !defined(HAVE_GL_GLES1)
    video_flags |= SDL_OPENGL;
#endif

	if ((screen = SDL_SetVideoMode 
	(resolution.width, resolution.height, bpp, video_flags)) == NULL) {
		Message ("couldn't initialize video",  SDL_GetError()); 
		Message ("set to 800 x 600");
		screen = SDL_SetVideoMode (800, 600, bpp, video_flags);
		param.res_type = 1;
		SaveConfigFile ();
	}
	SDL_Surface *surf = SDL_GetVideoSurface ();
	param.x_resolution = surf->w;
	param.y_resolution = surf->h;
	if (resolution.width == 0 && resolution.height == 0) {
		auto_x_resolution = param.x_resolution;
		auto_y_resolution = param.y_resolution;
	}
 	param.scale = CalcScreenScale ();
	if (param.use_quad_scale) param.scale = sqrt (param.scale);
}

void CWinsys::SetupVideoMode (int idx) {
	if (idx < 0 || idx >= NUM_RESOLUTIONS) SetupVideoMode (MakeRes (800, 600));
	else SetupVideoMode (resolution[idx]);
}

void CWinsys::SetupVideoMode (int width, int height) {
	SetupVideoMode (MakeRes (width, height));
}

void CWinsys::InitJoystick () {
    if (SDL_InitSubSystem (SDL_INIT_JOYSTICK) < 0) {
	Message ("Could not initialize SDL_joystick: %s", SDL_GetError());
	return;
    }

    numJoysticks = SDL_NumJoysticks ();
    if (numJoysticks < 1) {
	joystick = NULL;
	return;		
    }	

    SDL_JoystickEventState (SDL_ENABLE);
    joystick = SDL_JoystickOpen (0);	// first stick with number 0
    if (joystick == NULL) {
	Message ("Cannot open joystick %s", SDL_GetError ());
	return;
    }

    joystick_active = true;
}

void CWinsys::Init () {
    Uint32 sdl_flags = SDL_INIT_VIDEO | SDL_INIT_NOPARACHUTE | SDL_INIT_TIMER;
    if (SDL_Init (sdl_flags) < 0) Message ("Could not initialize SDL");

#if !defined(HAVE_GL_GLES1)
    SDL_GL_SetAttribute (SDL_GL_DOUBLEBUFFER, 1);

    #if defined (USE_STENCIL_BUFFER)
	SDL_GL_SetAttribute (SDL_GL_STENCIL_SIZE, 8);
    #endif
#endif
	
    SetupVideoMode (GetResolution (param.res_type));

#if defined(HAVE_GL_GLES1)
    // use EGL to initialise GLES
    g_x11Display = XOpenDisplay(NULL);
    if (!g_x11Display)
    {
        fprintf(stderr, "ERROR: unable to get display!n");
        exit(-1);
    }

    g_eglDisplay = eglGetDisplay((EGLNativeDisplayType)g_x11Display);
    if (g_eglDisplay == EGL_NO_DISPLAY)
    {
        printf("Unable to initialise EGL display.");
        exit(-1);
    }

    // Initialise egl
    if (!eglInitialize(g_eglDisplay, NULL, NULL))
    {
        printf("Unable to initialise EGL display.");
        exit(-1);
    }

    // Find a matching config
    EGLint numConfigsOut = 0;
    if (eglChooseConfig(g_eglDisplay, g_configAttribs, &g_eglConfig, 1, &numConfigsOut) != EGL_TRUE || numConfigsOut == 0)
    {
        fprintf(stderr, "Unable to find appropriate EGL config.");
        exit(-1);
    }

    // Get the SDL window handle
    SDL_SysWMinfo sysInfo; //Will hold our Window information
    SDL_VERSION(&sysInfo.version); //Set SDL version
    if(SDL_GetWMInfo(&sysInfo) <= 0)
    {
        printf("Unable to get window handle");
        exit(-1);
    }

    g_eglSurface = eglCreateWindowSurface(g_eglDisplay, g_eglConfig, (EGLNativeWindowType)sysInfo.info.x11.window, 0);
    if (g_eglSurface == EGL_NO_SURFACE)
    {
        printf("Unable to create EGL surface!");
        exit(-1);
    }

    // Bind GLES and create the context
    eglBindAPI(EGL_OPENGL_ES_API);
    EGLint contextParams[] = {EGL_CONTEXT_CLIENT_VERSION, 1, EGL_NONE};             // Use GLES version 1.x
    g_eglContext = eglCreateContext(g_eglDisplay, g_eglConfig, NULL, contextParams);
    if (g_eglContext == EGL_NO_CONTEXT)
    {
        printf("Unable to create GLES context!");
        exit(-1);
    }

    if (eglMakeCurrent(g_eglDisplay,  g_eglSurface,  g_eglSurface, g_eglContext) == EGL_FALSE)
    {
        printf("Unable to make GLES context current");
        exit(-1);
    }
#else
    SDL_GL_SetAttribute(SDL_GL_RED_SIZE, COLOURDEPTH_RED_SIZE);
    SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, COLOURDEPTH_GREEN_SIZE);
    SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, COLOURDEPTH_BLUE_SIZE);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, COLOURDEPTH_DEPTH_SIZE);
#endif

    Reshape (param.x_resolution, param.y_resolution);

    SDL_WM_SetCaption (WINDOW_TITLE, WINDOW_TITLE);
    KeyRepeat (false);
    if (USE_JOYSTICK) InitJoystick ();
//  SDL_EnableUNICODE (1);
}

void CWinsys::KeyRepeat (bool repeat) {
    int delay = ( repeat ? SDL_DEFAULT_REPEAT_DELAY : 0 );
    int interval = ( repeat ? SDL_DEFAULT_REPEAT_INTERVAL : 0 );
    SDL_EnableKeyRepeat (delay, interval);
}

void CWinsys::SetFonttype () {
    const char* font = (param.use_papercut_font > 0 ? "pc20" : "bold");
    FT.SetFont (font);
}

void CWinsys::CloseJoystick () {
    if (joystick_active) SDL_JoystickClose (joystick);	
}

void CWinsys::Quit () {
    CloseJoystick ();
    Tex.FreeTextureList ();
    Course.FreeCourseList ();
    Course.ResetCourse ();
    SaveMessages ();
    Audio.Close ();		// frees music and sound as well
    FT.Clear ();
    if (g_game.argument < 1) Players.SavePlayers ();
    Score.SaveHighScore ();

#if defined(HAVE_GL_GLES1)
    eglMakeCurrent(g_eglDisplay, NULL, NULL, EGL_NO_CONTEXT);
    eglDestroySurface(g_eglDisplay, g_eglSurface);
    eglDestroyContext(g_eglDisplay, g_eglContext);
    g_eglSurface = 0;
    g_eglContext = 0;
    g_eglConfig = 0;
    eglTerminate(g_eglDisplay);
    g_eglDisplay = 0;
    XCloseDisplay(g_x11Display);
    g_x11Display = NULL;
#endif

    SDL_Quit ();
    exit (0);
}

void CWinsys::PrintJoystickInfo () {
	if (joystick_active == false) {
		Message ("No joystick found");
		return;
	}
	PrintStr ("");
	PrintStr (SDL_JoystickName (0));
	int num_buttons = SDL_JoystickNumButtons (joystick);
	printf ("Joystick has %d button%s\n", num_buttons, num_buttons == 1 ? "" : "s");
	int num_axes = SDL_JoystickNumAxes (joystick);
	printf ("Joystick has %d ax%ss\n\n", num_axes, num_axes == 1 ? "i" : "e");
}

void CWinsys::SwapBuffers() {
#if defined(HAVE_GL_GLES1)
    eglSwapBuffers(g_eglDisplay, g_eglSurface);
#else
    SDL_GL_SwapBuffers();
#endif
}

void CWinsys::Delay(unsigned int ms) {
    SDL_Delay (ms);
}

// ------------ modes -------------------------------------------------

void CWinsys::SetModeFuncs (
		TGameMode mode, TInitFuncN init, TLoopFuncN loop, TTermFuncN term,
		TKeybFuncN keyb, TMouseFuncN mouse, TMotionFuncN motion,
		TJAxisFuncN jaxis, TJButtFuncN jbutt, TKeybFuncS keyb_spec) {
    modefuncs[mode].init      = init;
    modefuncs[mode].loop      = loop;
    modefuncs[mode].term      = term;
    modefuncs[mode].keyb      = keyb;
    modefuncs[mode].mouse     = mouse;
    modefuncs[mode].motion    = motion;
    modefuncs[mode].jaxis     = jaxis;
    modefuncs[mode].jbutt     = jbutt;
    modefuncs[mode].keyb_spec = keyb_spec;
}

void CWinsys::IdleFunc () {}

bool CWinsys::ModePending () {
	return g_game.mode != new_mode;
}

/*
typedef struct{
  Uint8 scancode;
  SDLKey sym;
  SDLMod mod;
  Uint16 unicode;
} SDL_keysym;*/

void CWinsys::PollEvent () {
    SDL_Event event; 
	SDL_keysym sym;
    unsigned int key, axis;
    int x, y;
	float val;

	while (SDL_PollEvent (&event)) {
		if (ModePending()) {
			IdleFunc ();
    	} else {
			switch (event.type) {
				case SDL_KEYDOWN:
				if (modefuncs[g_game.mode].keyb) {
					SDL_GetMouseState (&x, &y);
					key = event.key.keysym.sym; 
					(modefuncs[g_game.mode].keyb) (key, key >= 256, false, x, y);
				} else if (modefuncs[g_game.mode].keyb_spec) {
					sym = event.key.keysym;
					(modefuncs[g_game.mode].keyb_spec) (sym, false);
				}
				break;
	
				case SDL_KEYUP:
				if (modefuncs[g_game.mode].keyb) {
					SDL_GetMouseState (&x, &y);
					key = event.key.keysym.sym; 
					(modefuncs[g_game.mode].keyb)  (key, key >= 256, true, x, y);
				} else if (modefuncs[g_game.mode].keyb_spec) {
					sym = event.key.keysym;
					(modefuncs[g_game.mode].keyb_spec) (sym, true);
				}
				break;
	
				case SDL_MOUSEBUTTONDOWN:
				case SDL_MOUSEBUTTONUP:
				if (modefuncs[g_game.mode].mouse) {
					(modefuncs[g_game.mode].mouse) 
						(event.button.button, event.button.state,
						event.button.x, event.button.y);
				}
				break;
	
				case SDL_MOUSEMOTION:
					if (modefuncs[g_game.mode].motion) 
					(modefuncs[g_game.mode].motion) (event.motion.x, event.motion.y);
				break;

				case SDL_JOYAXISMOTION:  
				if (joystick_active) {
					axis = event.jaxis.axis;
					if (modefuncs[g_game.mode].jaxis && axis < 2) {
						val = (float)event.jaxis.value / 32768;
							(modefuncs[g_game.mode].jaxis) (axis, val);
					}
				}
				break; 

				case SDL_JOYBUTTONDOWN:  
				case SDL_JOYBUTTONUP:  
				if (joystick_active) {
					if (modefuncs[g_game.mode].jbutt) {
						(modefuncs[g_game.mode].jbutt) 
							(event.jbutton.button, event.jbutton.state);
					}
				}
				break;

				case SDL_VIDEORESIZE:
					param.x_resolution = event.resize.w;
					param.y_resolution = event.resize.h;
					SetupVideoMode (param.res_type);
					Reshape (event.resize.w, event.resize.h);
				break;
			
				case SDL_QUIT: 
					Quit ();
				break;
			}
    	}
	}
}

void CWinsys::ChangeMode () {
	// this function is called when new_mode is set
	// terminate function of previous mode
	if (g_game.mode >= 0 &&  modefuncs[g_game.mode].term != 0) 
	    (modefuncs[g_game.mode].term) ();
	g_game.prev_mode = g_game.mode;

	// init function of new mode
	if (modefuncs[new_mode].init != 0) {
		(modefuncs[new_mode].init) ();
		clock_time = SDL_GetTicks() * 1.e-3;
	}

	g_game.mode = new_mode;
	// new mode is now the current mode.
}

void CWinsys::CallLoopFunction () {
		cur_time = SDL_GetTicks() * 1.e-3;
		g_game.time_step = cur_time - clock_time;
		if (g_game.time_step < 0.0001) g_game.time_step = 0.0001;
		clock_time = cur_time;

		if (modefuncs[g_game.mode].loop != 0) 
			(modefuncs[g_game.mode].loop) (g_game.time_step);	
}

void CWinsys::EventLoop () {
    while (true) {
	PollEvent ();
	if (ModePending()) ChangeMode ();
	CallLoopFunction ();
	Delay (g_game.loopdelay);
    }
}
