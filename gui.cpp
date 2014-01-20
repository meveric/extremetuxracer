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

#include "gui.h"
#include "textures.h"
#include "font.h"

#define CURSOR_SIZE 10

static int cursor_x = -100;
static int cursor_y = -100;

static TMouseRect MouseArr[MAX_MOUSERECTS];
static int numMouseRect = 0;
static TArrow Arrows[MAX_ARROWS];
static int numArrows = 0;
static int selArrow = -1;
static int selType = -1;
static TTextButton TextButtons[MAX_TEXTBUTTONS];
static int numTextButtons = 0;
static TIconButton IconButtons[MAX_ICONBUTTONS];
static int numIconButtons = 0;
static TCheckbox Checkboxes[MAX_CHECKBOXES];
static int numCheckboxes = 0;

void ResetWidgets () {
	numMouseRect = 0;
	numArrows = 0;
	numTextButtons = 0;
	numIconButtons = 0;
	numCheckboxes = 0;
}

void DrawCursor () {
	Tex.Draw (MOUSECURSOR, cursor_x, cursor_y, 
		CURSOR_SIZE  * (double)param.x_resolution / 14000);
}

void AddMouseRect (int left, int top, int width, int height,
		int focus, int dir, int arrnr, TWidgetType type) {
	TRect r;
	
	if (numMouseRect >= MAX_MOUSERECTS) return;
	r.left = left;
	r.top = top;
	r.width = width;
	r.height = height;
	MouseArr[numMouseRect].rect = r;
	MouseArr[numMouseRect].focus = focus;
	MouseArr[numMouseRect].dir = dir;
	MouseArr[numMouseRect].arrnr = arrnr;
	MouseArr[numMouseRect].type = type;
	numMouseRect++;
}

void AddArrow (int x, int y, int dir, int focus) {
	if (numArrows >= MAX_ARROWS) return;
	Arrows[numArrows].x = x;
	Arrows[numArrows].y = y;
	Arrows[numArrows].dir = dir;
	Arrows[numArrows].focus = focus;
	AddMouseRect (x, y, 32, 16, focus, dir, numArrows, W_ARROW);
	numArrows++;
}

void AddTextButton (const char *text, int x, int y, int focus, double ftsize) {
	if (numTextButtons >= MAX_TEXTBUTTONS) return;	
	TextButtons[numTextButtons].y = y;	
	TextButtons[numTextButtons].text = text;	
	TextButtons[numTextButtons].focus = focus;	

	if (ftsize < 0) ftsize = FT.AutoSizeN (4);
	
	TextButtons[numTextButtons].ftsize = ftsize;	
	FT.SetSize (ftsize);
	double len = FT.GetTextWidth (text);
	if (x == CENTER) x = (int)((param.x_resolution - len) / 2);
	TextButtons[numTextButtons].x = x;
	int offs = (int)(ftsize / 5);
	AddMouseRect (x-20, y+offs, (int)len + 40, (int)(ftsize+offs), focus, 0, 
		numTextButtons, W_TEXTBUTTON);
	numTextButtons++;	
}

void AddTextButton (const string text, int x, int y, int focus, double ftsize) {
	AddTextButton (text.c_str(), x, y, focus, ftsize);
}

void AddTextButtonN (const char *text, int x, int y, int focus, int rel_ftsize) {
	double siz = FT.AutoSizeN (rel_ftsize);
	AddTextButton (text, y, y, focus, siz);
}

void AddTextButtonN (const string text, int x, int y, int focus, int rel_ftsize) {
	double siz = FT.AutoSizeN (rel_ftsize);
	AddTextButton (text, y, y, focus, siz);
}

void PrintTextButton (int nr, int focus) {
	TColor col = colWhite;
	if (focus == TextButtons[nr].focus) col = colDYell;
	if (nr >= numTextButtons) return;

	FT.SetColor (col);
	FT.SetSize (TextButtons[nr].ftsize);
	FT.DrawString (TextButtons[nr].x, TextButtons[nr].y, TextButtons[nr].text);
}

void AddCheckbox (int x, int y, int focus, int width, const string tag) {
	if (numCheckboxes >= MAX_CHECKBOXES) return;	
	Checkboxes[numCheckboxes].x = x;
	Checkboxes[numCheckboxes].y = y;
	Checkboxes[numCheckboxes].focus = focus;
	Checkboxes[numCheckboxes].width = width;
	Checkboxes[numCheckboxes].tag = tag;
	AddMouseRect (x+width-32, y, 32, 32, focus, 0, numCheckboxes, W_CHECKBOX);
	numCheckboxes++;
}

void PrintCheckbox (int nr, int focus, bool state) {
	TColor col = colWhite;
	TCheckbox *box = &Checkboxes[nr];
	if (focus == box->focus) col = colDYell;
	if (nr >= numCheckboxes) return;
	Tex.Draw (CHECKBOX, box->x + box->width - 32, box->y, 1.0);
	if (state) Tex.Draw (CHECKMARK_SMALL, box->x + box->width - 32, box->y, 1.0);
	FT.SetColor (col);
	FT.DrawString (box->x, box->y, box->tag);
}

void AddIconButton (int x, int y, int focus, GLuint texid, double size) {
	if (numIconButtons >= MAX_ICONBUTTONS) return;	
	IconButtons[numIconButtons].x = x;	
	IconButtons[numIconButtons].y = y;	
	IconButtons[numIconButtons].focus = focus;	
	IconButtons[numIconButtons].texid = texid;	
	IconButtons[numIconButtons].size = size;	
	AddMouseRect (x, y, 32, 32, focus, 0, numIconButtons, W_ICONBUTTON);
	numIconButtons++;
}

void PrintIconButton (int nr, int focus, int state) {
	if (state < 0 || state >= 4) return;
	TColor framecol = colWhite;
	if (focus == IconButtons[nr].focus) framecol = colDYell;
	if (nr >= numIconButtons) return;

	int size = (int)IconButtons[nr].size;
	int line = 3;
	int framesize = size + 2 * line; 
 	int t = param.y_resolution - IconButtons[nr].y;
	int y = t - size;
	int x = IconButtons[nr].x;
	int r = x + size;

	DrawFrameX (IconButtons[nr].x-line, IconButtons[nr].y-line, 
				framesize, framesize, line, colBlack, framecol, 1.0);

	glEnable (GL_TEXTURE_2D);
	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);

	glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glBindTexture (GL_TEXTURE_2D, IconButtons[nr].texid);
	glColor4f (1.0, 1.0, 1.0, 1.0);

	const GLfloat tex[4][8] = {
		{ 0.0,0.5, 0.5,0.5, 0.5,1.0, 0.0,1.0 },
		{ 0.5,0.5, 1.0,0.5, 1.0,1.0, 0.5,1.0 },
		{ 0.0,0.0, 0.5,0.0, 0.5,0.5, 0.0,0.5 },
		{ 0.5,0.0, 1.0,0.0, 1.0,0.5, 0.5,0.5 }
	};
	const GLfloat vtx[8] = {
		x,y, r,y, r,t, x,t
	};

	if( ( 0 <= state ) && ( state <= 3 ) )
	{
#if defined(HAVE_GL_GLES1)

#else
		glTexCoordPointer(2, GL_FLOAT, 0, tex[state]);
		glVertexPointer(2, GL_FLOAT, 0, vtx);
		glDrawArrays(GL_TRIANGLE_FAN,0,4);
#endif
	}

	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	glDisableClientState(GL_VERTEX_ARRAY);
}

void DrawArrow (int x, int y, int dir, bool active, int sel) {
	double textl[6] = {0.5, 0.0, 0.5, 0.5, 0.0, 0.5};		
	double textr[6] = {1.0, 0.5, 1.0, 1.0, 0.5, 1.0};
	double texbl[6] = {0.25, 0.25, 0.75, 0.00, 0.00, 0.50};
	double texbr[6] = {0.50, 0.50, 1.00, 0.25, 0.25, 0.75};
	double texleft, texright, textop, texbottom;
	TVector2 bl, tr;

	int type;	 
	if (active) type = 3 * dir + 1 + sel; else type = 3 * dir;
	
	bl.x = x;
	bl.y = param.y_resolution - y - 16;
	tr.x = x + 32;
	tr.y = param.y_resolution - y;
		
	texleft = textl[type];
	texright = textr[type];
	texbottom = texbl[type];	
	textop = texbr[type];
	
	glEnable (GL_TEXTURE_2D );
	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);

	glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	Tex.BindTex (LB_ARROWS);
	glColor4f (1.0, 1.0, 1.0, 1.0);
	
	const GLfloat tex[] = {
		texleft,  texbottom,
		texright, texbottom,
		texright, textop,
		texleft,  textop
	};
	const GLfloat vtx[] = {
		bl.x, bl.y,
		tr.x, bl.y,
		tr.x, tr.y,
		bl.x, tr.y
	};

	glTexCoordPointer(2, GL_FLOAT, 0, tex);
	glVertexPointer(2, GL_FLOAT, 0, vtx);
	glDrawArrays(GL_TRIANGLE_FAN,0,4);

	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	glDisableClientState(GL_VERTEX_ARRAY);
}


// active is true if the arrow can be clicked on. If the value has reached the end
// of range, active must be set to false
// nr is the index in arrowarray Arrows
void PrintArrow (int nr, bool active) {
	int sel = 0;
	if (nr >= numArrows) return;
	if ((nr == selArrow) && (selType == W_ARROW) && active) sel = 1;
	DrawArrow (Arrows[nr].x, Arrows[nr].y, Arrows[nr].dir, active, sel);		
}

bool Inside (int x, int y, int idx) {
	if (idx >= numMouseRect) return false;
	if (x >= MouseArr[idx].rect.left 
		&& x <= MouseArr[idx].rect.left + MouseArr[idx].rect.width
		&& y >= MouseArr[idx].rect.top
		&& y <= MouseArr[idx].rect.top + MouseArr[idx].rect.height) {
		return true;
	} else return false;
}

void GetFocus (int x, int y, int *focus, int *dir) {
	int i;
	cursor_x = x;
	cursor_y = y;
	for (i=0; i<numMouseRect; i++) {
		if (Inside (x,y,i)) {
			*focus = MouseArr[i].focus;
			*dir = MouseArr[i].dir;
			selArrow = MouseArr[i].arrnr;
			selType = MouseArr[i].type;
			return;
		}
	}
	*focus = -1;
	*dir = -1;
	selArrow = -1;
	selType = -1;
}

void DrawFrameX (int x, int y, int w, int h, int line, 
		TColor backcol, TColor framecol, double transp) {
    double yy = param.y_resolution - y - h;
 
    if (x < 0) x = (param.x_resolution -w) / 2;
    glPushMatrix();
	glDisable (GL_TEXTURE_2D);
    
	glTranslatef (x, yy, 0);

	glEnableClientState(GL_VERTEX_ARRAY);
	
	const GLfloat vtx1[] = {
		0, 0,
		w, 0,
		w, h,
		0, h
	};
	const GLfloat vtx2[] = {
	    0 + line, 0 + line,
	    w - line, 0 + line,
	    w - line, h - line,
	    0 + line, h - line
	};

	glColor4f (framecol.r, framecol.g, framecol.b, transp); 
	glVertexPointer(2, GL_FLOAT, 0, vtx1);
	glDrawArrays(GL_TRIANGLE_FAN,0,4);

	glColor4f (backcol.r, backcol.g, backcol.b, transp);
	glVertexPointer(2, GL_FLOAT, 0, vtx2);
	glDrawArrays(GL_TRIANGLE_FAN,0,4);

	glDisableClientState(GL_VERTEX_ARRAY);
	glEnable (GL_TEXTURE_2D);
    glPopMatrix();
}

void DrawLevel (int x, int y, int level, double fact) {
    TVector2 bl, tr;
	double lev[4] = {0.0, 0.75, 0.5, 0.25}; 
	double bott, top;
	
	bl.x = x;
	bl.y = param.y_resolution - y - 32;
	tr.x = x + 95;
	tr.y = param.y_resolution - y;

	bott = lev[level];
	top = bott + 0.25;
	
	glEnable (GL_TEXTURE_2D);
	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);

	glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	Tex.BindTex (STARS);
	glColor4f (1.0, 1.0, 1.0, 1.0);

	const GLfloat tex[] = {
		0.00, bott,
		0.75, bott,
		0.75, top,
		0.00, top
	};
	const GLfloat vtx[] = {
		bl.x, bl.y,
		tr.x, bl.y,
		tr.x, tr.y,
		bl.x, tr.y
	};

	glTexCoordPointer(2, GL_FLOAT, 0, tex);
	glVertexPointer(2, GL_FLOAT, 0, vtx);
	glDrawArrays(GL_TRIANGLE_FAN,0,4);

	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	glDisableClientState(GL_VERTEX_ARRAY);
}

void DrawBonus (int x, int y, int max, int num) {
	TVector2 bl, tr;
	double bott, top;
	int i;
		
	bl.y = param.y_resolution - y - 32;
	tr.y = param.y_resolution - y;

	glEnable (GL_TEXTURE_2D);
	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);

	glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	Tex.BindTex (TUXBONUS);

	glColor4f (1.0, 1.0, 1.0, 1.0);

	for (i=0; i<max; i++) {
		bl.x = x + i * 40;
		tr.x = bl.x + 32;
	
		bott = ( i < num ? 0.5 : 0.0 );
		top = bott + 0.5;

		const GLfloat tex[] = {
			0, bott,
			1, bott,
			1, top,
			0, top
		};
		const GLfloat vtx[] = {
			bl.x, bl.y,
			tr.x, bl.y,
			tr.x, tr.y,
			bl.x, tr.y
		};

		glTexCoordPointer(2, GL_FLOAT, 0, tex);
		glVertexPointer(2, GL_FLOAT, 0, vtx);
		glDrawArrays(GL_TRIANGLE_FAN,0,4);
	}

	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	glDisableClientState(GL_VERTEX_ARRAY);
}

void DrawBonusExt (int y, int numraces, int num) {
    TVector2 bl, tr;
	double bott, top;
	int i;

	//TColor col1 = MakeColor (0.3, 0.5, 0.7, 1);
	TColor col2 = MakeColor (0.45, 0.65, 0.85, 1);
	//TColor col3 = MakeColor (0.6, 0.8, 1.0, 1);
	//TColor gold = MakeColor (1, 1, 0, 1);

	int lleft[3];
	int majr, minr;

	int maxtux = numraces * 3;
	if (num > maxtux) return;

	int framewidth = numraces * 40 + 8;
	int totalwidth = framewidth * 3 + 8;
	int xleft = (param.x_resolution - totalwidth) / 2;
	lleft[0] = xleft;
	lleft[1] = xleft + framewidth + 4;
	lleft[2] = xleft + framewidth + framewidth + 8;

	DrawFrameX (lleft[0], y, framewidth, 40, 1, col2, colBlack, 1);
	DrawFrameX (lleft[1], y, framewidth, 40, 1, col2, colBlack, 1);
	DrawFrameX (lleft[2], y, framewidth, 40, 1, col2, colBlack, 1);
	if (param.use_papercut_font > 0) FT.SetSize (20); else FT.SetSize (15);
	bl.y = param.y_resolution - y - 32 -4;
	tr.y = param.y_resolution - y - 0 -4;

	glEnable (GL_TEXTURE_2D);
	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);

	glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	Tex.BindTex (TUXBONUS);
	glColor4f (1.0, 1.0, 1.0, 1.0);

	for (i=0; i<maxtux; i++) {
		majr = (int)(i/numraces);
		minr = i - majr * numraces;
		if (majr > 2) majr = 2;
		bl.x = lleft[majr] + minr * 40 + 6;
		tr.x = bl.x + 32;

		// with tux outlines:
		// if (i<num) bott = 0.5; else bott = 0.0;
		// top = bott + 0.5;
		if (i<num) {
			bott = 0.5;
			top = 1.0;

			const GLfloat tex[] = {
				0.00, bott,
				1.00, bott,
				1.00, top,
				0.00, top
			};
			const GLfloat vtx[] = {
				bl.x, bl.y,
				tr.x, bl.y,
				tr.x, tr.y,
				bl.x, tr.y
			};

			glTexCoordPointer(2, GL_FLOAT, 0, tex);
			glVertexPointer(2, GL_FLOAT, 0, vtx);
			glDrawArrays(GL_TRIANGLE_FAN,0,4);
		}
	}

	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	glDisableClientState(GL_VERTEX_ARRAY);
}

// ------------------ new ---------------------------------------------

int AutoYPosN (double percent) {
	double hh = (double)param.y_resolution;
	double po = hh * percent / 100;
	return (int)(po);
}

TArea AutoAreaN (double top_perc, double bott_perc, int w) {
	TArea res;
	res.top = AutoYPosN (top_perc);
	res.bottom = AutoYPosN (bott_perc);
 	if (w > param.x_resolution) w = param.x_resolution;
	double left = (param.x_resolution - w) / 2;
	res.left = (int) left;
	res.right = param.x_resolution - res.left;
	return res;
}







