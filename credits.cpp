/* --------------------------------------------------------------------
EXTREME TUXRACER

Copyright (C) 2010 Extreme Tuxracer Team

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
GNU General Public License for more details.
---------------------------------------------------------------------*/

#include "credits.h"
#include "audio.h"
#include "ogl.h"
#include "particles.h"
#include "textures.h"
#include "font.h"
#include "gui.h"
#include "spx.h"

#define TOP_Y 160
#define BOTT_Y 64
#define OFFS_SCALE_FACTOR 1.2

static TCredits CreditList[MAX_CREDITS];
static int numCredits = 0;

static TVector2 cursor_pos = {0, 0};
static double y_offset = 0;
static TColor bgcol;
static bool moving = true;

void LoadCreditList () {
	CSPList list(MAX_CREDITS);

	string creditfile;
	int i;
	double offset;
	string item;
	string line;

	if (!list.Load (param.data_dir, "credits.lst")) {
		Message ("could not load credits list");
		return;
	}

	for (i=0; i<list.Count(); i++) {
		line = list.Line(i);
		CreditList[i].text = SPStrN (line, "text", "");

		offset = SPFloatN (line, "offs", 0) * OFFS_SCALE_FACTOR * param.scale;
		if (i>0) CreditList[i].offs = CreditList[i-1].offs + (int)offset;
		else CreditList[i].offs = offset;

		CreditList[i].col = SPIntN (line, "col", 0);
		CreditList[i].size = SPFloatN (line, "size", 1.0);
		numCredits = i + 1;
	}
}

void DrawCreditsText (double time_step){
    double w = (double)param.x_resolution;
    double h = (double)param.y_resolution;
    double offs = 0.0;
    int i;
    TColor col;
    if (moving) y_offset += time_step * 30;

    for (i=0; i < numCredits; i++) {
	offs = h - 100 - y_offset + CreditList[i].offs;

	col = ( CreditList[i].col == 0 ? colWhite : colDYell );
	FT.SetColor (col);
	FT.AutoSizeN (CreditList[i].size);
	FT.DrawString (-1, (int)offs, CreditList[i].text);
    }

    glDisable (GL_TEXTURE_2D);
    glEnableClientState (GL_VERTEX_ARRAY);
    glEnableClientState (GL_COLOR_ARRAY);

    const GLfloat col1[] = {
        bgcol.r, bgcol.g, bgcol.b, bgcol.a,
        bgcol.r, bgcol.g, bgcol.b, bgcol.a,
        bgcol.r, bgcol.g, bgcol.b, bgcol.a,
        bgcol.r, bgcol.g, bgcol.b, bgcol.a,
    };
    const GLfloat col2[] = {
        bgcol.r, bgcol.g, bgcol.b, bgcol.a,
        bgcol.r, bgcol.g, bgcol.b, bgcol.a,
        bgcol.r, bgcol.g, bgcol.b, 0,
        bgcol.r, bgcol.g, bgcol.b, 0
    };

    const GLfloat vtx1[] = {
        0, 0,
        w, 0,
        w, BOTT_Y,
        0, BOTT_Y
    };
    const GLfloat vtx2[] = {
        0, BOTT_Y,
        w, BOTT_Y,
        w, BOTT_Y + 30,
        0, BOTT_Y + 30
    };

    glColorPointer(4, GL_FLOAT, 0, col1);
    glVertexPointer(2, GL_FLOAT, 0, vtx1);
    glDrawArrays(GL_TRIANGLE_FAN,0,4);

    glColorPointer(4, GL_FLOAT, 0, col2);
    glVertexPointer(2, GL_FLOAT, 0, vtx2);
    glDrawArrays(GL_TRIANGLE_FAN,0,4);

    const GLfloat vtx3[] = {
        0, h - TOP_Y,
        w, h - TOP_Y,
        w, h,
        0, h
    };
    const GLfloat vtx4[] = {
        w, h - TOP_Y,
        0, h - TOP_Y,
        0, h - TOP_Y - 30,
        w, h - TOP_Y - 30
    };

    glColorPointer(4, GL_FLOAT, 0, col1);
    glVertexPointer(2, GL_FLOAT, 0, vtx3);
    glDrawArrays(GL_TRIANGLE_FAN,0,4);

    glColorPointer(4, GL_FLOAT, 0, col2);
    glVertexPointer(2, GL_FLOAT, 0, vtx4);
    glDrawArrays(GL_TRIANGLE_FAN,0,4);

    glColor4f (1, 1, 1, 1 );

    glDisableClientState (GL_COLOR_ARRAY);
    glDisableClientState (GL_VERTEX_ARRAY);
    glEnable (GL_TEXTURE_2D);
    if (offs < TOP_Y) y_offset = 0;
}

void DrawBackLogo (int x, int y, double size) {
    GLuint w, h;
    GLfloat width, height, top, bott, left, right;

    glEnable (GL_TEXTURE_2D);
    glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glBindTexture (GL_TEXTURE_2D, Tex.TexID (T_TITLE));

    w = Tex.TexWidth (T_TITLE);
    h = Tex.TexHeight (T_TITLE);

    width  = w * size;
    height = h * size;

    top = param.y_resolution - y;
    bott = top - height;

    left = ( x >= 0 ? x : (param.x_resolution - width) / 2 );
    right = left + width;

    const GLfloat tex[] = { 0,0, 1,0, 1,1, 0,1 };
    const GLfloat vtx[] = {
         left, bott,
        right, bott,
        right, top,
         left, top
    };

    glEnableClientState (GL_VERTEX_ARRAY);
    glEnableClientState (GL_TEXTURE_COORD_ARRAY);

    glColor4f (1.0, 1.0, 1.0, 0.4);
    glVertexPointer(2, GL_FLOAT, 0, vtx);
    glTexCoordPointer(2, GL_FLOAT, 0, tex);
    glDrawArrays(GL_TRIANGLE_FAN,0,4);

    glDisableClientState (GL_TEXTURE_COORD_ARRAY);
    glDisableClientState (GL_VERTEX_ARRAY);
}

void CreditsKeys (unsigned int key, bool special, bool release, int x, int y){
	if (release) return;
	switch (key) {
		case 109: moving = !moving; break;
		case 9: param.ui_snow = !param.ui_snow; break;
		default: Winsys.SetMode (GAME_TYPE_SELECT); 
	}
}

static void CreditsMouseFunc (int button, int state, int x, int y ){
	if (state == 1) Winsys.SetMode (GAME_TYPE_SELECT); 
}

void CreditsMotionFunc (int x, int y ){
    TVector2 old_pos;
	
    if (Winsys.ModePending ()) return; 	    
    y = param.y_resolution - y;
    old_pos = cursor_pos;
    cursor_pos = MakeVector2 (x, y);

    if (old_pos.x != x || old_pos.y != y) {
	if (param.ui_snow) push_ui_snow (cursor_pos);
    }
}

void CreditsInit () {
	Music.Play (param.credits_music, -1);
	y_offset = 0;
	moving = true;
	bgcol = colBackgr;
}

void CreditsLoop (double time_step) {
	int ww = param.x_resolution;
	int hh = param.y_resolution;
	
	Music.Update ();    
	check_gl_error();
    ClearRenderContext ();
    set_gl_options (GUI);
    SetupGuiDisplay ();

//	DrawBackLogo (-1,  AutoYPos (200), 1.0);
	DrawCreditsText (time_step);
	if (param.ui_snow) {
		update_ui_snow (time_step);
		draw_ui_snow();
    }
	Tex.Draw (BOTTOM_LEFT, 0, hh-256, 1);
	Tex.Draw (BOTTOM_RIGHT, ww-256, hh-256, 1);
	Tex.Draw (TOP_LEFT, 0, 0, 1);
	Tex.Draw (TOP_RIGHT, ww-256, 0, 1);
 	Tex.Draw (T_TITLE_SMALL, CENTER, AutoYPosN (5), param.scale);
	

	Reshape (ww, hh);
    Winsys.SwapBuffers();
}

void CreditsTerm () {
}

void credits_register() {
	Winsys.SetModeFuncs (CREDITS, CreditsInit, CreditsLoop, CreditsTerm,
 		CreditsKeys, CreditsMouseFunc, CreditsMotionFunc, NULL, NULL, NULL);
}


