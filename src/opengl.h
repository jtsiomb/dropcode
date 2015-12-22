/*
Stereoscopic tunnel for iOS.
Copyright (C) 2011-2015 John Tsiombikas <nuclear@member.fsf.org>

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/


#ifndef OPENGL_H_
#define OPENGL_H_

#ifdef HAVE_CONFIG_H_
#include "config.h"
#endif

#if defined(IPHONE) || defined(__IPHONE__)
#include <OpenGLES/ES2/gl.h>

#define glClearDepth	glClearDepthf
#define GLDEF
#include "sanegl.h"

#elif defined(ANDROID)
#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>
#define GLDEF
#include "sanegl.h"

#else

#include <GL/glew.h>

#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif	/* __APPLE__ */

#endif	/* IPHONE */

#endif	/* OPENGL_H_ */
