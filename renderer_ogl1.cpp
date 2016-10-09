// wkbre - WK (Battles) recreated game engine
// Copyright (C) 2015-2016 Adrien Geets
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.

#include "global.h"

#define glprocvalid(x) ( (((scpuint)(x)) < -1) || (((scpuint)(x)) > 3) )
#ifndef GL_BGRA_EXT
#define GL_BGRA_EXT 0x80E1

#endif

typedef BOOL (APIENTRY *gli_wglSwapIntervalEXT)(int n);
typedef int (APIENTRY *gli_wglGetSwapIntervalEXT)();

HDC whdc; HGLRC glrc;

void OGL1Quit()
{
	if(fullscreen)
		ChangeDisplaySettings(NULL, 0);
}

struct RBatchOGL1 : public RBatch
{
	batchVertex *vbuf; ushort *ibuf;

	~RBatchOGL1() {delete [] vbuf; delete [] ibuf;}

	void begin() {}
	void end() {}

	void next(uint nverts, uint nindis, batchVertex **vpnt, ushort **ipnt, uint *fi)
	{
		if(nverts > maxverts) ferr("Too many vertices to fit in the batch.");
		if(nindis > maxindis) ferr("Too many indices to fit in the batch.");
		if((curverts + nverts > maxverts) || (curindis + nindis > maxindis))
			flush();

		*vpnt = vbuf + curverts;
		*ipnt = ibuf + curindis;
		*fi = curverts;

		curverts += nverts; curindis += nindis;
	}

	void flush()
	{
		if(!curverts) return;
		glVertexPointer(3, GL_FLOAT, sizeof(batchVertex), &vbuf->x);
		glTexCoordPointer(2, GL_FLOAT, sizeof(batchVertex), &vbuf->u);
		glDrawElements(GL_TRIANGLES, curindis, GL_UNSIGNED_SHORT, ibuf);
		curverts = curindis = 0;
	}
};	

struct OGL1Renderer : public IRenderer
{

void InitGL()
{
	glShadeModel(GL_SMOOTH);
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClearDepth(1.0f);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	glEnable(GL_TEXTURE_2D);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glMatrixMode(GL_MODELVIEW);
}

void ResizeGL()
{
	glViewport(0, 0, scrw, scrh);
}

void Init()
{
	whdc = GetDC(hWindow);

	// Set the pixel format
	PIXELFORMATDESCRIPTOR pfd;
	memset(&pfd, 0, sizeof(PIXELFORMATDESCRIPTOR)); // Be sure that pfd is filled with 0.
	pfd.nSize = sizeof(PIXELFORMATDESCRIPTOR);
	pfd.nVersion = 1;
	pfd.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
	pfd.iPixelType = PFD_TYPE_RGBA;
	pfd.cColorBits = 24;
	pfd.cDepthBits = 32;
	pfd.dwLayerMask = PFD_MAIN_PLANE;
	int i = ChoosePixelFormat(whdc, &pfd);
	SetPixelFormat(whdc, i, &pfd);

	glrc = wglCreateContext(whdc);
	wglMakeCurrent(whdc, glrc);
	//char *glexts = (char*)glGetString(GL_EXTENSIONS);
	//printf("%s\n", glexts);

	if(fullscreen)
	{
		DEVMODE dm;
		memset(&dm, 0, sizeof(dm));
		dm.dmSize = sizeof(dm);
		dm.dmPelsWidth = scrw;
		dm.dmPelsHeight = scrh;
		//dm.dmBitsPerPel = 32;
		dm.dmFields = DM_PELSWIDTH | DM_PELSHEIGHT; // | DM_BITSPERPEL;
		ChangeDisplaySettings(&dm, CDS_FULLSCREEN);
	}

	// Set VSYNC if enabled
	if(VSYNCenabled)
	{
		gli_wglSwapIntervalEXT wglSwapIntervalEXT = (gli_wglSwapIntervalEXT)wglGetProcAddress("wglSwapIntervalEXT");
		//gli_wglGetSwapIntervalEXT wglGetSwapIntervalEXT = (gli_wglGetSwapIntervalEXT)wglGetProcAddress("wglGetSwapIntervalEXT");
		if(glprocvalid(wglSwapIntervalEXT))
		{
			//__asm int 3
			//printf(":)\n");
			if(!wglSwapIntervalEXT(1))
				printf("wglSwapIntervalEXT returned FALSE.\n");
			//if(wglGetSwapIntervalEXT)
			//	printf("wglGetSwapIntervalEXT returns %i.\n", wglGetSwapIntervalEXT());
		}
		else printf("wglSwapIntervalEXT unsupported.\n");
	}

	atexit(OGL1Quit);
	InitGL(); ResizeGL();
}

void Reset()
{
	ResizeGL();
}

void BeginDrawing()
{
	uint c = mapfogcolor;
	glClearColor(((c>>16)&255)/255.0f, ((c>>8)&255)/255.0f, (c&255)/255.0f, ((c>>24)&255)/255.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void EndDrawing()
{
	SwapBuffers(whdc); drawframes++;
	//glFinish();
}

texture CreateTexture(Bitmap *bm, int mipmaps)
{
	Bitmap *c = bm;
	if(bm->form != BMFORMAT_B8G8R8A8)
		c = ConvertBitmapToB8G8R8A8(bm);

	texture t;
	glGenTextures(1, &t.gl);
	glBindTexture(GL_TEXTURE_2D, t.gl);
	if(mipmaps != 1)
		gluBuild2DMipmaps(GL_TEXTURE_2D, 4, c->w, c->h, GL_BGRA_EXT, GL_UNSIGNED_BYTE, c->pix);
	else
		glTexImage2D(GL_TEXTURE_2D, 0, 4, c->w, c->h, 0, GL_BGRA_EXT, GL_UNSIGNED_BYTE, c->pix);

	if(bm->form != BMFORMAT_B8G8R8A8)
		FreeBitmap(c);
	return t;
}

void FreeTexture(texture t)
{
	glDeleteTextures(1, &t.gl);
}

void SetTransformMatrix(Matrix *m)
{
	glLoadMatrixf(m->v);
}

void SetTexture(uint x, texture t)
{
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, t.gl);
}

void NoTexture(uint x)
{
	glDisable(GL_TEXTURE_2D);
}

void SetFog()
{
	glEnable(GL_FOG);
	float fc[4]; int c = mapfogcolor;
	fc[0] = ((c>>16)&255)/255.0f;
	fc[1] = ((c>> 8)&255)/255.0f;
	fc[2] = ((c    )&255)/255.0f;
	fc[3] = ((c>>24)&255)/255.0f;
	//fc[0] = fc[1] = fc[2] = fc[3] = 0;
	glFogi(GL_FOG_MODE, GL_LINEAR);
	glFogf(GL_FOG_START, farzvalue/2);
	glFogf(GL_FOG_END, farzvalue-8);
	glFogfv(GL_FOG_COLOR, fc);
}

void DisableFog()
{
	glDisable(GL_FOG);
}

void EnableAlphaTest()
{
	;
}

void DisableAlphaTest()
{
	;
}

void EnableColorBlend()
{
	;
}

void DisableColorBlend()
{
	glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
}

void SetBlendColor(int c)
{
	glColor4ub((c>>16)&255, (c>>8)&255, c&255, (c>>24)&255);
}

void InitRectDrawing()
{
	glDisable(GL_ALPHA_TEST);
	glDisable(GL_CULL_FACE);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	Matrix m; CreateZeroMatrix(&m);
	m._11 = 2.0f / scrw;
	m._22 = -2.0f / scrh;
	m._41 = -1;
	m._42 = 1;
	m._44 = 1;
	glLoadMatrixf(m.v);
}

void DrawRect(int x, int y, int w, int h, int c, float u, float v, float o, float p)
{
	glColor4ub((c>>16)&255, (c>>8)&255, c&255, (c>>24)&255);
	glBegin(GL_TRIANGLE_STRIP);
		glTexCoord2f(u  , v  ); glVertex3f(x  , y  , 0.5f);
		glTexCoord2f(u+o, v  ); glVertex3f(x+w, y  , 0.5f);
		glTexCoord2f(u  , v+p); glVertex3f(x  , y+h, 0.5f);
		glTexCoord2f(u+o, v+p); glVertex3f(x+w, y+h, 0.5f);
	glEnd();
}

void DrawGradientRect(int x, int y, int w, int h, int c0, int c1, int c2, int c3)
{
	glBegin(GL_TRIANGLE_STRIP);
		glColor4ub((c0>>16)&255, (c0>>8)&255, c0&255, (c0>>24)&255);
		glTexCoord2f(0, 0); glVertex3f(x  , y  , 0.5f);
		glColor4ub((c1>>16)&255, (c1>>8)&255, c1&255, (c1>>24)&255);
		glTexCoord2f(1, 0); glVertex3f(x+w, y  , 0.5f);
		glColor4ub((c2>>16)&255, (c2>>8)&255, c2&255, (c2>>24)&255);
		glTexCoord2f(0, 1); glVertex3f(x  , y+h, 0.5f);
		glColor4ub((c3>>16)&255, (c3>>8)&255, c3&255, (c3>>24)&255);
		glTexCoord2f(1, 1); glVertex3f(x+w, y+h, 0.5f);
	glEnd();
}

void CreateMapPart(MapPart *p, int x, int y, int w, int h)
{
	p->glverts = new float[(h+1)*(w+1)*5];
	float *vp = p->glverts;
	for(int b = 0; b < h+1; b++)
	for(int a = 0; a < w+1; a++)
	{
		*(vp++) = a+x;
		*(vp++) = himap[(b+y)*(mapwidth+1)+(a+x)];
		*(vp++) = b+y;
		*(vp++) = a+x; *(vp++) = b+y;
	}

	p->glindices = new GLushort[(2*(w+1)+1)*h];
	GLushort *os = p->glindices;
	for(int cy = 0; cy < h; cy++)
	{
		if(!(cy&1))
		{
			for(int cx = 0; cx < w+1; cx++)
			{
				*(os++) = cy*(w+1)+cx;
				*(os++) = (cy+1)*(w+1)+cx;
				if(cx == w) *(os++) = (cy+1)*(w+1)+cx;
			}
		} else {
			for(int cx = w+1-1; cx >= 0; cx--)
			{
				*(os++) = cy*(w+1)+cx;
				*(os++) = (cy+1)*(w+1)+cx;
				if(cx == 0) *(os++) = (cy+1)*(w+1)+cx;
			}
		}
	}
}

void FreeMapPart(MapPart *p)
{
	delete [] p->glverts; delete [] p->glindices;
}

void BeginMapDrawing()
{
	glDisable(GL_ALPHA_TEST);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	glDisable(GL_BLEND);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	glDisableClientState(GL_COLOR_ARRAY);
}

void DrawPart(MapPart *p)
{
	int x = p->x, y = p->y, w = p->w, h = p->h;

	glVertexPointer(3, GL_FLOAT, 5*4, p->glverts);
	glTexCoordPointer(2, GL_FLOAT, 5*4, &(p->glverts[3]));
	glDrawElements(GL_TRIANGLE_STRIP, (2*(w+1)+1)*h, GL_UNSIGNED_SHORT, p->glindices);
}

void CreateMesh(Mesh *m)
{
	;
}

void BeginMeshDrawing()
{
	glEnable(GL_TEXTURE_2D);
	glAlphaFunc(GL_GEQUAL, 240.0f/255.0f);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_FRONT);
	glDisable(GL_BLEND);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	glDisableClientState(GL_COLOR_ARRAY);
}

void DrawMesh(Mesh *m, int iwtcolor)
{
	glVertexPointer(3, GL_FLOAT, 3*4, m->mverts.gb.memory);
	glTexCoordPointer(2, GL_FLOAT, 2*4, m->muvlist[iwtcolor]->gb.memory);
	for(int g = 0; g < m->ngrp; g++)
	{
		if(m->lstmatflags[g]) glEnable(GL_ALPHA_TEST);
		else glDisable(GL_ALPHA_TEST);
		glBindTexture(GL_TEXTURE_2D, m->lstmattex[g].gl);
		glDrawElements(GL_TRIANGLES, m->mstartix[g+1]-m->mstartix[g], GL_UNSIGNED_SHORT, ((short*)m->mindices.gb.memory) + m->mstartix[g]);
	}
}

RBatch *CreateBatch(int mv, int mi)
{
	//ferr("Batch unsupported in OpenGL driver.");
	RBatchOGL1 *b = new RBatchOGL1;
	b->maxverts = mv; b->maxindis = mi;
	b->curverts = b->curindis = 0;
	b->vbuf = new batchVertex[mv];
	b->ibuf = new ushort[mi];
	return b;
}

void BeginBatchDrawing()
{
	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	glDisableClientState(GL_COLOR_ARRAY);
	glColor4ub(255, 255, 255, 255);
}

};

IRenderer *CreateOGL1Renderer() {return new OGL1Renderer;}
