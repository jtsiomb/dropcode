#ifndef CMESHGEN_H_
#define CMESHGEN_H_

#include "cmesh.h"

void gen_sphere(struct cmesh *mesh, float rad, int usub, int vsub, float urange, float vrange);
void gen_geosphere(struct cmesh *mesh, float rad, int subdiv, int hemi);
void gen_torus(struct cmesh *mesh, float mainrad, float ringrad, int usub, int vsub,
		float urange, float vrange);
void gen_cylinder(struct cmesh *mesh, float rad, float height, int usub, int vsub,
		int capsub, float urange, float vrange);
void gen_cone(struct cmesh *mesh, float rad, float height, int usub, int vsub,
		int capsub, float urange, float vrange);
void gen_plane(struct cmesh *mesh, float width, float height, int usub, int vsub);
void gen_heightmap(struct cmesh *mesh, float width, float height, int usub, int vsub,
		float (*hf)(float, float, void*), void *hfdata);
void gen_box(struct cmesh *mesh, float xsz, float ysz, float zsz, int usub, int vsub);

void gen_revol(struct cmesh *mesh, int usub, int vsub, cgm_vec2 (*rfunc)(float, float, void*),
		cgm_vec2 (*nfunc)(float, float, void*), void *cls);

/* callback args: (float u, float v, void *cls) -> Vec2 XZ offset u,v in [0, 1] */
void gen_sweep(struct cmesh *mesh, float height, int usub, int vsub,
		cgm_vec2 (*sfunc)(float, float, void*), void *cls);

#endif	/* CMESHGEN_H_ */
