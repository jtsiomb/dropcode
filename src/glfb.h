#ifndef OPENGL_FRAMEBUFFER_H_
#define OPENGL_FRAMEBUFFER_H_

/* show pixels with OpenGL
 *
 * Usage example:
 *   glfb_setup(width, height, GLFB_RGBA32, width * 4);
 *   ...
 *   glfb_update(pixels);
 *   glfb_display();
 */

enum glfb_pixel_format {
	GLFB_IDX8,
	GLFB_RGB15,		/* 555 */
	GLFB_RGB16,		/* 565 */
	GLFB_RGB24,		/* 888 */
	GLFB_RGBA32		/* 8888 (alpha ignored) */
};

enum glfb_filter {
	GLFB_NEAREST,
	GLFB_LINEAR
};

void glfb_setup(int x, int y, enum glfb_pixel_format fmt, int pitch);
void glfb_filter(enum glfb_filter filt);
void glfb_color(int idx, int r, int g, int b);
void glfb_update(void *pixels);
void glfb_display(void);

#endif	/* OPENGL_FRAMEBUFFER_H_ */
