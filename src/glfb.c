#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <GL/gl.h>
#include "glfb.h"

static void conv_idx8(void *pixels);
static void conv_rgb15(void *pixels);
static void conv_rgb16(void *pixels);
static void conv_rgb24(void *pixels);
static void conv_rgba32(void *pixels);
static unsigned int next_pow2(unsigned int x);

static int width, height, scansz;
static enum glfb_pixel_format pixfmt;
static unsigned int min_filt = GL_NEAREST, mag_filt = GL_NEAREST;
static uint32_t *convbuf;
static int convbuf_size;

static uint32_t cmap[256];

static unsigned int tex;
static int tex_width, tex_height;
static float tex_sx, tex_sy;

static void (*convert[])(void*) = {
	conv_idx8, conv_rgb15, conv_rgb16, conv_rgb24, conv_rgba32
};


void glfb_setup(int x, int y, enum glfb_pixel_format fmt, int pitch)
{
	int tx, ty, newsz;

	tx = next_pow2(x);
	ty = next_pow2(y);

	if(!tex || tx < tex_width || ty < height || fmt != pixfmt) {
		if(!tex) {
			glGenTextures(1, &tex);
			glBindTexture(GL_TEXTURE_2D, tex);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, min_filt);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, mag_filt);
			glEnable(GL_TEXTURE_2D);
		}
		glBindTexture(GL_TEXTURE_2D, tex);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, tx, ty, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);

		tex_width = tx;
		tex_height = ty;
	}

	tex_sx = (float)x / tex_width;
	tex_sy = (float)y / tex_height;

	scansz = pitch;

	newsz = x * y * 4;
	if(convbuf_size < newsz) {
		free(convbuf);
		if(!(convbuf = malloc(newsz))) {
			fprintf(stderr, "glfb: failed to allocate conversion buffer\n");
			convbuf = 0;
		}
		convbuf_size = newsz;
	}

	width = x;
	height = y;
	pixfmt = fmt;
}

void glfb_filter(enum glfb_filter filt)
{
	switch(filt) {
	case GLFB_NEAREST:
		min_filt = mag_filt = GL_NEAREST;
		break;
	case GLFB_LINEAR:
		min_filt = mag_filt = GL_LINEAR;
		break;
	}
}


void glfb_color(int idx, int r, int g, int b)
{
	if(idx < 0 || idx >= 255) return;

	cmap[idx] = (b << 16) | (g << 8) | r;
}

void glfb_update(void *pixels)
{
	convert[pixfmt](pixels);
	glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, width, height, GL_RGBA, GL_UNSIGNED_BYTE, convbuf);
}

void glfb_display(void)
{
	glClearColor(0.2, 0.2, 0.2, 1);
	glClear(GL_COLOR_BUFFER_BIT);

	glBegin(GL_TRIANGLES);
	glTexCoord2f(0, tex_sy * 2.0f);
	glVertex2f(-1, -3);
	glTexCoord2f(tex_sx * 2.0f, 0);
	glVertex2f(3, 1);
	glTexCoord2f(0, 0);
	glVertex2f(-1, 1);
	glEnd();
}

static void conv_idx8(void *pixels)
{
	int i, j;
	unsigned char *sptr = pixels;
	uint32_t *dptr = convbuf;

	for(i=0; i<height; i++) {
		for(j=0; j<width; j++) {
			*dptr++ = cmap[sptr[j]];
		}
		sptr += scansz;
	}
}

static void conv_rgb15(void *pixels)
{
	int i, j;
	uint16_t pix;
	uint32_t r, g, b;
	uint16_t *sptr = pixels;
	uint32_t *dptr = convbuf;

	for(i=0; i<height; i++) {
		for(j=0; j<width; j++) {
			pix = sptr[j];
			r = ((pix & 0x1f) << 3) | (pix & 7);
			g = ((pix & 0x3e0) >> 2) | ((pix >> 5) & 7);
			b = ((pix & 0x7c00) >> 7) | ((pix >> 10) & 7);
			*dptr++ = r | (g << 8) | (b << 16);
		}
		sptr = (uint16_t*)((char*)sptr + scansz);
	}
}

static void conv_rgb16(void *pixels)
{
	int i, j;
	uint16_t pix;
	uint32_t r, g, b;
	uint16_t *sptr = pixels;
	uint32_t *dptr = convbuf;

	for(i=0; i<height; i++) {
		for(j=0; j<width; j++) {
			pix = sptr[j];
			r = ((pix & 0x1f) << 3) | (pix & 7);
			g = ((pix & 0x7e0) >> 3) | ((pix >> 5) & 3);
			b = ((pix & 0xf800) >> 8) | ((pix >> 11) & 7);
			*dptr++ = r | (g << 8) | (b << 16);
		}
		sptr = (uint16_t*)((char*)sptr + scansz);
	}
}

static void conv_rgb24(void *pixels)
{
	int i, j;
	uint32_t r, g, b;
	unsigned char *sptr, *scanptr = pixels;
	uint32_t *dptr = convbuf;

	for(i=0; i<height; i++) {
		sptr = scanptr;
		for(j=0; j<width; j++) {
			r = sptr[0];
			g = sptr[1];
			b = sptr[2];
			sptr += 3;
			*dptr++ = r | (g << 8) | (b << 16);
		}
		scanptr += scansz;
	}
}

static void conv_rgba32(void *pixels)
{
	int i, j;
	uint32_t pix, r, g, b;
	uint32_t *sptr = pixels;
	uint32_t *dptr = convbuf;

	for(i=0; i<height; i++) {
		for(j=0; j<width; j++) {
			pix = sptr[j];
			r = (pix >> 16) & 0xff;
			g = (pix >> 8) & 0xff;
			b = pix & 0xff;
			*dptr++ = r | (g << 8) | (b << 16);
		}
		sptr = (uint32_t*)((char*)sptr + scansz);
	}
}

static unsigned int next_pow2(unsigned int x)
{
	x--;
	x |= x >> 1;
	x |= x >> 2;
	x |= x >> 4;
	x |= x >> 8;
	x |= x >> 16;
	return x + 1;
}

