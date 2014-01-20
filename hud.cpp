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

#include "hud.h"
#include "ogl.h"
#include "textures.h"
#include "spx.h"
#include "particles.h"
#include "font.h"
#include "course.h"


#define SECONDS_IN_MINUTE 60
#define TIME_LABEL_X_OFFSET 12.0
#define TIME_LABEL_Y_OFFSET 12.0
#define TIME_X_OFFSET 20.0
#define TIME_Y_OFFSET 5.0
#define HERRING_ICON_HEIGHT 30.0
#define HERRING_ICON_WIDTH 48.0
#define HERRING_ICON_IMG_SIZE 64.0
#define HERRING_ICON_X_OFFSET 160.0
#define HERRING_ICON_Y_OFFSET 41.0
#define HERRING_COUNT_Y_OFFSET 37.0
#define GAUGE_IMG_SIZE 128
#define ENERGY_GAUGE_BOTTOM 3.0
#define ENERGY_GAUGE_HEIGHT 103.0
#define ENERGY_GAUGE_CENTER_X 71.0
#define ENERGY_GAUGE_CENTER_Y 55.0
#define GAUGE_WIDTH 128.0
#define SPEED_UNITS_Y_OFFSET 4.0
#define SPEEDBAR_OUTER_RADIUS  (ENERGY_GAUGE_CENTER_X)
#define SPEEDBAR_BASE_ANGLE 225
#define SPEEDBAR_MAX_ANGLE 45
#define SPEEDBAR_GREEN_MAX_SPEED  (MAX_PADDLING_SPEED * 3.6)
#define SPEEDBAR_YELLOW_MAX_SPEED 100
#define SPEEDBAR_RED_MAX_SPEED 160
#define SPEEDBAR_GREEN_FRACTION 0.5
#define SPEEDBAR_YELLOW_FRACTION 0.25
#define SPEEDBAR_RED_FRACTION 0.25
#define FPS_X_OFFSET 12
#define FPS_Y_OFFSET 12
#define NUM_FPS_FRAMES 10 
#define CIRCLE_DIVISIONS 10

static TColor energy_background_color = MakeColor( 0.2, 0.2, 0.2, 0.0 );
static TColor energy_foreground_color = MakeColor( 0.54, 0.59, 1.00, 0.5 );
static TColor speedbar_background_color = MakeColor( 0.2, 0.2, 0.2, 0.0 );
static TColor hud_white = MakeColor( 1.0, 1.0, 1.0, 1.0 );

static const TColor text_colour = {0, 0, 0, 1};

static void draw_time(){
	string timestr, secstr, hundrstr;
    int min, sec, hundr;
    GetTimeComponents (g_game.time, &min, &sec, &hundr);
	timestr = Int_StrN (min, 2);
	secstr = Int_StrN (sec, 2);
	hundrstr = Int_StrN (hundr, 2);




	timestr += ":";
	timestr += secstr;

	if (param.use_papercut_font < 2) {
		Tex.DrawNumStr (timestr.c_str(), 20, 10, 1, colWhite);
		Tex.DrawNumStr (hundrstr.c_str(), 136, 10, 0.7, colWhite);
	} else {
		
/*		
	glEnable (GL_LINE_SMOOTH);
	glEnable (GL_BLEND);
	glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glHint (GL_LINE_SMOOTH_HINT, GL_DONT_CARE);
	glLineWidth (1.5);
*/
	glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);
		
		Tex.Draw (T_TIME, 10, 12, 1);
		FT.SetColor (colDYell);
		FT.SetSize (32);
		FT.DrawString (160, 6, hundrstr);
		FT.SetSize (42);
		FT.DrawString (70, 10, timestr);
	}
}

static void draw_herring_count (int herring_count){
	string hcountstr;

	hcountstr = Int_StrN (herring_count, 3);
	if (param.use_papercut_font < 2) {
		Tex.DrawNumStr (hcountstr.c_str(), param.x_resolution - 90, 10, 1, colWhite);
		Tex.Draw (HERRING_ICON, param.x_resolution-160,  -3, 1);
	} else {
		FT.SetColor (colDYell);
		FT.DrawString ( param.x_resolution - 90, 10, hcountstr);
		Tex.Draw (T_YELLHERRING, param.x_resolution-160, 12, 1);
	}
}

TVector2 calc_new_fan_pt (double angle) {
    TVector2 pt;
    pt.x = ENERGY_GAUGE_CENTER_X + cos (ANGLES_TO_RADIANS (angle)) * SPEEDBAR_OUTER_RADIUS;
    pt.y = ENERGY_GAUGE_CENTER_Y + sin (ANGLES_TO_RADIANS (angle)) * SPEEDBAR_OUTER_RADIUS;
    return pt;
}

void draw_partial_tri_fan (double fraction) {
    int i;
    TVector2 pt;

    double cur_angle = SPEEDBAR_BASE_ANGLE;
    double angle_incr = 360.0 / CIRCLE_DIVISIONS;
    double angle = SPEEDBAR_BASE_ANGLE + (SPEEDBAR_MAX_ANGLE - SPEEDBAR_BASE_ANGLE) * fraction;
    int divs = (int)((SPEEDBAR_BASE_ANGLE - angle) * CIRCLE_DIVISIONS / 360.0);

    int num_vertices = 0;
    GLfloat* vtx = (GLfloat*) alloca(2*(divs+4)*sizeof(GLfloat));
    GLfloat* tex = (GLfloat*) alloca(2*(divs+4)*sizeof(GLfloat));
    GLfloat *pvtx = vtx;
    GLfloat *ptex = tex;

    // start tri fan
    *pvtx++ = ENERGY_GAUGE_CENTER_X; *pvtx++ = ENERGY_GAUGE_CENTER_Y;
    *ptex++ = ENERGY_GAUGE_CENTER_X / GAUGE_IMG_SIZE; *ptex++ = ENERGY_GAUGE_CENTER_Y / GAUGE_IMG_SIZE;
    num_vertices++; 

    pt = calc_new_fan_pt (SPEEDBAR_BASE_ANGLE);
    *pvtx++ = pt.x; *pvtx++ = pt.y;
    *ptex++ = pt.x / GAUGE_IMG_SIZE; *ptex++ = pt.y / GAUGE_IMG_SIZE;
    num_vertices++;

    for (i=0; i<divs; i++) {
	cur_angle -= angle_incr;
	pt = calc_new_fan_pt (cur_angle);
	*pvtx++ = pt.x; *pvtx++ = pt.y;
	*ptex++ = pt.x / GAUGE_IMG_SIZE; *ptex++ = pt.y / GAUGE_IMG_SIZE;
	num_vertices++;
    }

    if  (cur_angle > angle + EPS) {
	cur_angle = angle;
	pt = calc_new_fan_pt (cur_angle);
	*pvtx++ = pt.x; *pvtx++ = pt.y;
	*ptex++ = pt.x / GAUGE_IMG_SIZE; *ptex++ = pt.y / GAUGE_IMG_SIZE;
	num_vertices++;
    }

    if  (num_vertices > 2) {
	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	glVertexPointer(2, GL_FLOAT, 0, vtx);
	glTexCoordPointer(2, GL_FLOAT, 0, tex);
	glDrawArrays(GL_TRIANGLE_FAN, 0, num_vertices);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	glDisableClientState(GL_VERTEX_ARRAY);
    }
}

void draw_gauge (double speed, double energy) {
    double y;
    double speedbar_frac;

    set_gl_options (GAUGE_BARS);

    if (Tex.TexID (GAUGE_ENERGY) < 1) return;
    if (Tex.TexID (GAUGE_SPEED) < 1) return;
    if (Tex.TexID (GAUGE_OUTLINE) < 1) return;
	
    Tex.BindTex (GAUGE_ENERGY);

    glPushMatrix();
	glTranslatef (param.x_resolution - GAUGE_WIDTH, 0, 0);
	Tex.BindTex (GAUGE_ENERGY);
	y = ENERGY_GAUGE_BOTTOM + energy * ENERGY_GAUGE_HEIGHT;

	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);

	const GLfloat vtx_bg[] = {
	    0.0, y,
	    GAUGE_IMG_SIZE, y,
	    GAUGE_IMG_SIZE, GAUGE_IMG_SIZE,
	    0.0, GAUGE_IMG_SIZE
	};
	const GLfloat tex_bg[] = {
	    0.0, y / GAUGE_IMG_SIZE,
	    1.0, y / GAUGE_IMG_SIZE,
	    1.0, 1.0,
	    0.0, 1.0
	};	
	const GLfloat vtx_fg[] = {
	    0.0, 0.0,
	    GAUGE_IMG_SIZE, 0.0,
	    GAUGE_IMG_SIZE, y,
	    0.0, y
	};
	const GLfloat tex_fg[] = {
	    0.0, 0.0,
	    1.0, 0.0,
	    1.0, y / GAUGE_IMG_SIZE,
	    0.0, y / GAUGE_IMG_SIZE
	};

	glColor4f (energy_background_color.r, energy_background_color.g, energy_background_color.b, energy_background_color.a);
	glVertexPointer(2, GL_FLOAT, 0, vtx_bg);
	glTexCoordPointer(2, GL_FLOAT, 0, tex_bg);
	glDrawArrays(GL_TRIANGLE_FAN,0,4);

	glColor4f (energy_foreground_color.r, energy_foreground_color.g, energy_foreground_color.b, energy_foreground_color.a);
	glVertexPointer(2, GL_FLOAT, 0, vtx_fg);
	glTexCoordPointer(2, GL_FLOAT, 0, tex_fg);
	glDrawArrays(GL_TRIANGLE_FAN,0,4);

	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	glDisableClientState(GL_VERTEX_ARRAY);

	speedbar_frac = 0.0;

	if  (speed > SPEEDBAR_GREEN_MAX_SPEED) {
	    speedbar_frac = SPEEDBAR_GREEN_FRACTION;
	    
	    if  (speed > SPEEDBAR_YELLOW_MAX_SPEED) {
			speedbar_frac += SPEEDBAR_YELLOW_FRACTION;
			if  (speed > SPEEDBAR_RED_MAX_SPEED) {
				speedbar_frac += SPEEDBAR_RED_FRACTION;
			} else {
				speedbar_frac += (speed - SPEEDBAR_YELLOW_MAX_SPEED) /
				(SPEEDBAR_RED_MAX_SPEED - SPEEDBAR_YELLOW_MAX_SPEED) * SPEEDBAR_RED_FRACTION;
			}
	    } else {
			speedbar_frac += (speed - SPEEDBAR_GREEN_MAX_SPEED) /
				(SPEEDBAR_YELLOW_MAX_SPEED - SPEEDBAR_GREEN_MAX_SPEED) * SPEEDBAR_YELLOW_FRACTION;
	    }	    
	} else {
	    speedbar_frac +=  speed/SPEEDBAR_GREEN_MAX_SPEED * SPEEDBAR_GREEN_FRACTION;
	}

	glColor4f (speedbar_background_color.r, speedbar_background_color.g, speedbar_background_color.b, speedbar_background_color.a);
	Tex.BindTex (GAUGE_SPEED);
	draw_partial_tri_fan (1.0);
	glColor4f (hud_white.r, hud_white.g, hud_white.b, hud_white.a);
	draw_partial_tri_fan (min (1.0, speedbar_frac));

	const GLfloat vtx_ol[] = {
	    0.0, 0.0,
	    GAUGE_IMG_SIZE, 0.0,
	    GAUGE_IMG_SIZE, GAUGE_IMG_SIZE,
	    0.0, GAUGE_IMG_SIZE
	};
	const GLfloat tex_ol[] = {
	    0.0, 0.0,
	    1.0, 0.0,
	    1.0, 1.0,
	    0.0, 1.0
	};

	Tex.BindTex (GAUGE_OUTLINE);
	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	glColor4f (hud_white.r, hud_white.g, hud_white.b, hud_white.a);
	glVertexPointer(2, GL_FLOAT, 0, vtx_ol);
	glTexCoordPointer(2, GL_FLOAT, 0, tex_ol);
	glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	glDisableClientState(GL_VERTEX_ARRAY);

    glPopMatrix();
}

void DrawSpeed (double speed) {
	string speedstr;
	speedstr = Int_StrN ((int)speed, 3);
	if (param.use_papercut_font < 2) {
		Tex.DrawNumStr (speedstr.c_str(), 
			param.x_resolution - 85, param.y_resolution-74, 1, colWhite);
	} else {
		FT.SetColor (colDDYell);
		FT.DrawString (param.x_resolution-82, param.y_resolution-80, speedstr);
	}
}

void DrawWind (double dir, double speed) {
    Tex.Draw (SPEEDMETER, 10, param.y_resolution - 150, 1.0);
    glPushMatrix ();
	const GLfloat vtx[] = {
		-3.0f, 0.0f,
		+3.0f, 0.0f,
		+3.0f, -speed,
		-3.0f, -speed
	};
	
	glDisable (GL_TEXTURE_2D );
	glEnableClientState(GL_VERTEX_ARRAY);
	glColor4f (1, 0, 0, 0.5);
	glTranslatef (82, 77, 0);
	glRotatef (dir, 0, 0, 1);
	glVertexPointer(2, GL_FLOAT, 0, vtx);
	glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
	glDisableClientState(GL_VERTEX_ARRAY);
    glPopMatrix ();
    Tex.Draw (SPEED_KNOB, 74, param.y_resolution - 84, 1.0);
}

void DrawWind2 (float dir, float speed, CControl *ctrl) {
	string windstr;
	if (g_game.wind_id < 1) return;

	Tex.Draw (SPEEDMETER, 0, param.y_resolution-140, 1.0);
	glDisable (GL_TEXTURE_2D );
	glEnableClientState(GL_VERTEX_ARRAY);

	float alpha, red, blue, len;
	len = 45;
	if (speed <= 50) {
		alpha = speed / 50;
		red = 0;
	} else {
		alpha = 1.0;
		red = (speed - 50) / 50;
	}
	blue = 1.0 - red;

	const GLfloat vtx1[] = {
		-5.0f, 0.0f,
		+5.0f, 0.0f,
		+5.0f, -len,
		-5.0f, -len
	};

	glPushMatrix ();
		glColor4f (red, 0, blue, alpha);
		glTranslatef (72, 66, 0);
		glRotatef (dir, 0, 0, 1);
		glVertexPointer(2, GL_FLOAT, 0, vtx1);
		glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
	glPopMatrix ();

	// direction indicator
	TVector3 movdir = ctrl->cvel;
	NormVectorN (movdir);
	float dir_angle = atan (movdir.x / movdir.z) * 57.3;

	const GLfloat vtx2[] = {
		-2.0f, 0.0f,
		+2.0f, 0.0f,
		+2.0f, -50.0f,
		-2.0f, -50.0f
	};

	glPushMatrix ();
		glColor4f (0, 0.5, 0, 1.0);
		glTranslatef (72, 66, 0);
		glRotatef (dir_angle + 180, 0, 0, 1);
		glVertexPointer(2, GL_FLOAT, 0, vtx2);
	glPopMatrix ();

	glDisableClientState(GL_VERTEX_ARRAY);
	glEnable (GL_TEXTURE_2D );

 	Tex.Draw (SPEED_KNOB, 64, param.y_resolution - 74, 1.0);
	windstr = Int_StrN ((int)speed, 3);
	if (param.use_papercut_font < 2) {
		Tex.DrawNumStr (windstr.c_str(), 130, param.y_resolution - 55, 1, colWhite);
	} else {
		FT.SetColor (colBlue);
		FT.DrawString (130, param.y_resolution - 55, windstr);
	}
}

const  int   maxFrames = 50;
static int   numFrames = 0;
static float averagefps = 0;
static float sumTime = 0;

void DrawFps () {
	if (g_game.mode != RACING) return;
	string fpsstr;
	TColor col;

	if (numFrames >= maxFrames) {
		averagefps = 1 / sumTime * maxFrames;
		numFrames = 0;
		sumTime = 0;
	} else {
		sumTime += g_game.time_step;
		numFrames++;
	}
	if (averagefps < 1) return;

	if (param.display_fps) {
		if (averagefps >= 35) col = colWhite; else col = colRed;
		fpsstr = Float_StrN (averagefps, 0);
		if (param.use_papercut_font < 2) {
			Tex.DrawNumStr (fpsstr.c_str(), (param.x_resolution - 60) / 2, 10, 1, colWhite);
		} else {
			FT.SetColor (colDYell);
			FT.DrawString ((param.x_resolution - 60) / 2, 10, fpsstr);
		}
	}
}

void DrawPercentBar (float fact, float x, float y) {
	const GLfloat vtx[] = {
		x,    y,
		x+32, y,
		x+32, y+fact*128,
		x,    y+fact*128
	};
	const GLfloat tex[] = {
		0.0f, 0.0f,
		1.0f, 0.0f,
		1.0f, fact,
		0.0f, fact
	};

	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);

	Tex.BindTex (T_ENERGY_MASK);
	glColor4f (1.0, 1.0, 1.0, 1.0);
 
	glVertexPointer(2, GL_FLOAT, 0, vtx);
	glTexCoordPointer(2, GL_FLOAT, 0, tex);
	glDrawArrays(GL_TRIANGLE_FAN, 0, 4);

	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
}

void DrawCoursePosition (CControl *ctrl) {
	double pl, pw;
	Course.GetPlayDimensions (&pw, &pl);
	double fact = ctrl->cpos.z / pl;
	if (fact > 1.0) fact = 1.0;
	glEnable (GL_TEXTURE_2D );
	DrawPercentBar (-fact, param.x_resolution - 48, 280-128);
	Tex.Draw (T_MASK_OUTLINE, param.x_resolution - 48, param.y_resolution - 280, 1.0);
}

// -------------------------------------------------------
void DrawHud (CControl *ctrl) {
    TVector3 vel;
    double speed;

    if (!param.show_hud) return;
    vel = ctrl->cvel;
    speed = NormVector (&vel);
    SetupGuiDisplay ();

    draw_gauge (speed * 3.6, ctrl->jump_amt);
    set_gl_options (TEXFONT);
    glColor4f (1, 1, 1, 1);
    draw_time();
    draw_herring_count (g_game.herring);
    DrawSpeed (speed * 3.6);
    DrawFps ();
    DrawCoursePosition (ctrl);
    if (g_game.wind_id > 0) DrawWind2 (Wind.Angle (), Wind.Speed (), ctrl);
}




