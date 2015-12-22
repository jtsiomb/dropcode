#ifndef SANEGL_H_
#define SANEGL_H_

#include "opengl.h"

#ifndef GL_MODELVIEW
#define GL_MODELVIEW		0x1700
#endif
#ifndef GL_PROJECTION
#define GL_PROJECTION		0x1701
#endif
#ifndef GL_TEXTURE
#define GL_TEXTURE			0x1702
#endif

#ifndef GL_POINTS
#define GL_POINTS			0
#endif
#ifndef GL_LINES
#define GL_LINES			1
#endif
#ifndef GL_TRIANGLES
#define GL_TRIANGLES		4
#endif
#ifndef GL_QUADS
#define GL_QUADS			7
#endif

#ifdef GLDEF

#define glMatrixMode		gl_matrix_mode
#define glPushMatrix		gl_push_matrix
#define glPopMatrix			gl_pop_matrix
#define glLoadIdentity		gl_load_identity
#define glLoadMatrixf		gl_load_matrixf
#define glMultMatrixf		gl_mult_matrixf
#define glTranslatef		gl_translatef
#define glRotatef			gl_rotatef
#define glScalef			gl_scalef
#define glOrtho				gl_ortho
#define glFrustum			gl_frustum
#define gluPerspective		glu_perspective

#define glBegin				gl_begin
#define glEnd				gl_end
#define glVertex2f			gl_vertex2f
#define glVertex3f			gl_vertex3f
#define glVertex4f			gl_vertex4f
#define glNormal3f			gl_normal3f
#define glColor3f			gl_color3f
#define glColor4f			gl_color4f
#define glTexCoord1f		gl_texcoord1f
#define glTexCoord2f		gl_texcoord2f
#define glVertexAttrib2f	gl_vertex_attrib2f
#define glVertexAttrib3f	gl_vertex_attrib3f
#define glVertexAttrib4f	gl_vertex_attrib4f
#endif

#ifdef __cplusplus
extern "C" {
#endif

/* matrix stuff */
void gl_matrix_mode(int mmode);
void gl_push_matrix(void);
void gl_pop_matrix(void);
void gl_load_identity(void);
void gl_load_matrixf(const float *mat);
void gl_mult_matrixf(const float *mat);
void gl_translatef(float x, float y, float z);
void gl_rotatef(float angle, float x, float y, float z);
void gl_scalef(float x, float y, float z);
void gl_ortho(float left, float right, float bottom, float top, float near, float far);
void gl_frustum(float left, float right, float bottom, float top, float near, float far);
void glu_perspective(float vfov, float aspect, float near, float far);

void gl_apply_xform(unsigned int prog);


/* immediate mode rendering */
void gl_begin(int prim);
void gl_end(void);

void gl_vertex2f(float x, float y);
void gl_vertex3f(float x, float y, float z);
void gl_vertex4f(float x, float y, float z, float w);

void gl_normal3f(float x, float y, float z);

void gl_color3f(float r, float g, float b);
void gl_color4f(float r, float g, float b, float a);

void gl_texcoord1f(float s);
void gl_texcoord2f(float s, float t);

void gl_vertex_attrib2f(int loc, float x, float y);
void gl_vertex_attrib3f(int loc, float x, float y, float z);
void gl_vertex_attrib4f(int loc, float x, float y, float z, float w);

#ifdef __cplusplus
}
#endif

#endif	/* SANEGL_H_ */
