#include <stdio.h>
#include "meshgen.h"
#include "mesh.h"

// -------- sphere --------

#define SURAD(u)	((u) * 2.0 * M_PI)
#define SVRAD(v)	((v) * M_PI)

static Vector3 sphvec(float theta, float phi)
{
	return Vector3(sin(theta) * sin(phi),
			cos(phi),
			cos(theta) * sin(phi));
}

void gen_sphere(Mesh *mesh, float rad, int usub, int vsub, float urange, float vrange)
{
	if(usub < 4) usub = 4;
	if(vsub < 2) vsub = 2;

	int uverts = usub + 1;
	int vverts = vsub + 1;

	int num_verts = uverts * vverts;
	int num_quads = usub * vsub;
	int num_tri = num_quads * 2;

	mesh->clear();
	Vector3 *varr = (Vector3*)mesh->set_attrib_data(MESH_ATTR_VERTEX, 3, num_verts, 0);
	Vector3 *narr = (Vector3*)mesh->set_attrib_data(MESH_ATTR_NORMAL, 3, num_verts, 0);
	Vector3 *tarr = (Vector3*)mesh->set_attrib_data(MESH_ATTR_TANGENT, 3, num_verts, 0);
	Vector2 *uvarr = (Vector2*)mesh->set_attrib_data(MESH_ATTR_TEXCOORD, 2, num_verts, 0);
	unsigned int *idxarr = mesh->set_index_data(num_tri * 3, 0);

	float du = urange / (float)(uverts - 1);
	float dv = vrange / (float)(vverts - 1);

	float u = 0.0;
	for(int i=0; i<uverts; i++) {
		float theta = u * 2.0 * M_PI;

		float v = 0.0;
		for(int j=0; j<vverts; j++) {
			float phi = v * M_PI;

			Vector3 pos = sphvec(theta, phi);

			*varr++ = pos * rad;
			*narr++ = pos;
			*tarr++ = (sphvec(theta + 0.1f, (float)M_PI / 2.0f) - sphvec(theta - 0.1f, (float)M_PI / 2.0f)).normalized();
			*uvarr++ = Vector2(u * urange, v * vrange);

			if(i < usub && j < vsub) {
				int idx = i * vverts + j;
				*idxarr++ = idx;
				*idxarr++ = idx + 1;
				*idxarr++ = idx + vverts + 1;

				*idxarr++ = idx;
				*idxarr++ = idx + vverts + 1;
				*idxarr++ = idx + vverts;
			}

			v += dv;
		}
		u += du;
	}
}

// -------- torus -----------
static Vector3 torusvec(float theta, float phi, float mr, float rr)
{
	theta = -theta;

	float rx = -cos(phi) * rr + mr;
	float ry = sin(phi) * rr;
	float rz = 0.0;

	float x = rx * sin(theta) + rz * cos(theta);
	float y = ry;
	float z = -rx * cos(theta) + rz * sin(theta);

	return Vector3(x, y, z);
}

void gen_torus(Mesh *mesh, float mainrad, float ringrad, int usub, int vsub, float urange, float vrange)
{
	if(usub < 4) usub = 4;
	if(vsub < 2) vsub = 2;

	int uverts = usub + 1;
	int vverts = vsub + 1;

	int num_verts = uverts * vverts;
	int num_quads = usub * vsub;
	int num_tri = num_quads * 2;

	mesh->clear();
	Vector3 *varr = (Vector3*)mesh->set_attrib_data(MESH_ATTR_VERTEX, 3, num_verts, 0);
	Vector3 *narr = (Vector3*)mesh->set_attrib_data(MESH_ATTR_NORMAL, 3, num_verts, 0);
	Vector3 *tarr = (Vector3*)mesh->set_attrib_data(MESH_ATTR_TANGENT, 3, num_verts, 0);
	Vector2 *uvarr = (Vector2*)mesh->set_attrib_data(MESH_ATTR_TEXCOORD, 2, num_verts, 0);
	unsigned int *idxarr = mesh->set_index_data(num_tri * 3, 0);

	float du = urange / (float)(uverts - 1);
	float dv = vrange / (float)(vverts - 1);

	float u = 0.0;
	for(int i=0; i<uverts; i++) {
		float theta = u * 2.0 * M_PI;

		float v = 0.0;
		for(int j=0; j<vverts; j++) {
			float phi = v * 2.0 * M_PI;

			Vector3 pos = torusvec(theta, phi, mainrad, ringrad);
			Vector3 cent = torusvec(theta, phi, mainrad, 0.0);

			*varr++ = pos;
			*narr++ = (pos - cent) / ringrad;

			Vector3 pprev = torusvec(theta - 0.1f, phi, mainrad, ringrad);
			Vector3 pnext = torusvec(theta + 0.1f, phi, mainrad, ringrad);

			*tarr++ = (pnext - pprev).normalized();
			*uvarr++ = Vector2(u * urange, v * vrange);

			if(i < usub && j < vsub) {
				int idx = i * vverts + j;
				*idxarr++ = idx;
				*idxarr++ = idx + 1;
				*idxarr++ = idx + vverts + 1;

				*idxarr++ = idx;
				*idxarr++ = idx + vverts + 1;
				*idxarr++ = idx + vverts;
			}

			v += dv;
		}
		u += du;
	}
}


// -------- cylinder --------

static Vector3 cylvec(float theta, float height)
{
	return Vector3(sin(theta), height, cos(theta));
}

void gen_cylinder(Mesh *mesh, float rad, float height, int usub, int vsub, int capsub, float urange, float vrange)
{
	if(usub < 4) usub = 4;
	if(vsub < 1) vsub = 1;

	int uverts = usub + 1;
	int vverts = vsub + 1;

	int num_body_verts = uverts * vverts;
	int num_body_quads = usub * vsub;
	int num_body_tri = num_body_quads * 2;

	int capvverts = capsub ? capsub + 1 : 0;
	int num_cap_verts = uverts * capvverts;
	int num_cap_quads = usub * capsub;
	int num_cap_tri = num_cap_quads * 2;

	int num_verts = num_body_verts + num_cap_verts * 2;
	int num_tri = num_body_tri + num_cap_tri * 2;

	mesh->clear();
	Vector3 *varr = (Vector3*)mesh->set_attrib_data(MESH_ATTR_VERTEX, 3, num_verts, 0);
	Vector3 *narr = (Vector3*)mesh->set_attrib_data(MESH_ATTR_NORMAL, 3, num_verts, 0);
	Vector3 *tarr = (Vector3*)mesh->set_attrib_data(MESH_ATTR_TANGENT, 3, num_verts, 0);
	Vector2 *uvarr = (Vector2*)mesh->set_attrib_data(MESH_ATTR_TEXCOORD, 2, num_verts, 0);
	unsigned int *idxarr = mesh->set_index_data(num_tri * 3, 0);

	float du = urange / (float)(uverts - 1);
	float dv = vrange / (float)(vverts - 1);

	float u = 0.0;
	for(int i=0; i<uverts; i++) {
		float theta = SURAD(u);

		float v = 0.0;
		for(int j=0; j<vverts; j++) {
			float y = (v - 0.5) * height;
			Vector3 pos = cylvec(theta, y);

			*varr++ = Vector3(pos.x * rad, pos.y, pos.z * rad);
			*narr++ = Vector3(pos.x, 0.0, pos.z);
			*tarr++ = (cylvec(theta + 0.1, 0.0) - cylvec(theta - 0.1, 0.0)).normalized();
			*uvarr++ = Vector2(u * urange, v * vrange);

			if(i < usub && j < vsub) {
				int idx = i * vverts + j;

				*idxarr++ = idx;
				*idxarr++ = idx + vverts + 1;
				*idxarr++ = idx + 1;

				*idxarr++ = idx;
				*idxarr++ = idx + vverts;
				*idxarr++ = idx + vverts + 1;
			}

			v += dv;
		}
		u += du;
	}


	// now the cap!
	if(!capsub) {
		return;
	}

	dv = 1.0 / (float)(capvverts - 1);

	u = 0.0;
	for(int i=0; i<uverts; i++) {
		float theta = SURAD(u);

		float v = 0.0;
		for(int j=0; j<capvverts; j++) {
			float r = v * rad;

			Vector3 pos = cylvec(theta, height / 2.0) * r;
			pos.y = height / 2.0;
			Vector3 tang = (cylvec(theta + 0.1, 0.0) - cylvec(theta - 0.1, 0.0)).normalized();

			*varr++ = pos;
			*narr++ = Vector3(0, 1, 0);
			*tarr++ = tang;
			*uvarr++ = Vector2(u * urange, v);

			pos.y = -height / 2.0;
			*varr++ = pos;
			*narr++ = Vector3(0, -1, 0);
			*tarr++ = -tang;
			*uvarr++ = Vector2(u * urange, v);

			if(i < usub && j < capsub) {
				unsigned int idx = num_body_verts + (i * capvverts + j) * 2;

				unsigned int vidx[4] = {
					idx,
					idx + capvverts * 2,
					idx + (capvverts + 1) * 2,
					idx + 2
				};

				*idxarr++ = vidx[0];
				*idxarr++ = vidx[2];
				*idxarr++ = vidx[1];
				*idxarr++ = vidx[0];
				*idxarr++ = vidx[3];
				*idxarr++ = vidx[2];

				*idxarr++ = vidx[0] + 1;
				*idxarr++ = vidx[1] + 1;
				*idxarr++ = vidx[2] + 1;
				*idxarr++ = vidx[0] + 1;
				*idxarr++ = vidx[2] + 1;
				*idxarr++ = vidx[3] + 1;
			}

			v += dv;
		}
		u += du;
	}
}

// -------- cone --------

static Vector3 conevec(float theta, float y, float height)
{
	float scale = 1.0 - y / height;
	return Vector3(sin(theta) * scale, y, cos(theta) * scale);
}

void gen_cone(Mesh *mesh, float rad, float height, int usub, int vsub, int capsub, float urange, float vrange)
{
	if(usub < 4) usub = 4;
	if(vsub < 1) vsub = 1;

	int uverts = usub + 1;
	int vverts = vsub + 1;

	int num_body_verts = uverts * vverts;
	int num_body_quads = usub * vsub;
	int num_body_tri = num_body_quads * 2;

	int capvverts = capsub ? capsub + 1 : 0;
	int num_cap_verts = uverts * capvverts;
	int num_cap_quads = usub * capsub;
	int num_cap_tri = num_cap_quads * 2;

	int num_verts = num_body_verts + num_cap_verts;
	int num_tri = num_body_tri + num_cap_tri;

	mesh->clear();
	Vector3 *varr = (Vector3*)mesh->set_attrib_data(MESH_ATTR_VERTEX, 3, num_verts, 0);
	Vector3 *narr = (Vector3*)mesh->set_attrib_data(MESH_ATTR_NORMAL, 3, num_verts, 0);
	Vector3 *tarr = (Vector3*)mesh->set_attrib_data(MESH_ATTR_TANGENT, 3, num_verts, 0);
	Vector2 *uvarr = (Vector2*)mesh->set_attrib_data(MESH_ATTR_TEXCOORD, 2, num_verts, 0);
	unsigned int *idxarr = mesh->set_index_data(num_tri * 3, 0);

	float du = urange / (float)(uverts - 1);
	float dv = vrange / (float)(vverts - 1);

	float u = 0.0;
	for(int i=0; i<uverts; i++) {
		float theta = SURAD(u);

		float v = 0.0;
		for(int j=0; j<vverts; j++) {
			float y = v * height;
			Vector3 pos = conevec(theta, y, height);

			Vector3 tang = (conevec(theta + 0.1, 0.0, height) - conevec(theta - 0.1, 0.0, height)).normalized();
			Vector3 bitang = (conevec(theta, y + 0.1, height) - pos).normalized();

			*varr++ = Vector3(pos.x * rad, pos.y, pos.z * rad);
			*narr++ = cross_product(tang, bitang);
			*tarr++ = tang;
			*uvarr++ = Vector2(u * urange, v * vrange);

			if(i < usub && j < vsub) {
				int idx = i * vverts + j;

				*idxarr++ = idx;
				*idxarr++ = idx + vverts + 1;
				*idxarr++ = idx + 1;

				*idxarr++ = idx;
				*idxarr++ = idx + vverts;
				*idxarr++ = idx + vverts + 1;
			}

			v += dv;
		}
		u += du;
	}


	// now the bottom cap!
	if(!capsub) {
		return;
	}

	dv = 1.0 / (float)(capvverts - 1);

	u = 0.0;
	for(int i=0; i<uverts; i++) {
		float theta = SURAD(u);

		float v = 0.0;
		for(int j=0; j<capvverts; j++) {
			float r = v * rad;

			Vector3 pos = conevec(theta, 0.0, height) * r;
			Vector3 tang = (cylvec(theta + 0.1, 0.0) - cylvec(theta - 0.1, 0.0)).normalized();

			*varr++ = pos;
			*narr++ = Vector3(0, -1, 0);
			*tarr++ = tang;
			*uvarr++ = Vector2(u * urange, v);

			if(i < usub && j < capsub) {
				unsigned int idx = num_body_verts + i * capvverts + j;

				unsigned int vidx[4] = {
					idx,
					idx + capvverts,
					idx + (capvverts + 1),
					idx + 1
				};

				*idxarr++ = vidx[0];
				*idxarr++ = vidx[1];
				*idxarr++ = vidx[2];
				*idxarr++ = vidx[0];
				*idxarr++ = vidx[2];
				*idxarr++ = vidx[3];
			}

			v += dv;
		}
		u += du;
	}
}


// -------- plane --------

void gen_plane(Mesh *mesh, float width, float height, int usub, int vsub)
{
	gen_heightmap(mesh, width, height, usub, vsub, 0);
}


// ----- heightmap ------

void gen_heightmap(Mesh *mesh, float width, float height, int usub, int vsub, float (*hf)(float, float, void*), void *hfdata)
{
	if(usub < 1) usub = 1;
	if(vsub < 1) vsub = 1;

	mesh->clear();

	int uverts = usub + 1;
	int vverts = vsub + 1;
	int num_verts = uverts * vverts;

	int num_quads = usub * vsub;
	int num_tri = num_quads * 2;

	Vector3 *varr = (Vector3*)mesh->set_attrib_data(MESH_ATTR_VERTEX, 3, num_verts, 0);
	Vector3 *narr = (Vector3*)mesh->set_attrib_data(MESH_ATTR_NORMAL, 3, num_verts, 0);
	Vector3 *tarr = (Vector3*)mesh->set_attrib_data(MESH_ATTR_TANGENT, 3, num_verts, 0);
	Vector2 *uvarr = (Vector2*)mesh->set_attrib_data(MESH_ATTR_TEXCOORD, 2, num_verts, 0);
	unsigned int *idxarr = mesh->set_index_data(num_tri * 3, 0);

	float du = 1.0 / (float)usub;
	float dv = 1.0 / (float)vsub;

	float u = 0.0;
	for(int i=0; i<uverts; i++) {
		float v = 0.0;
		for(int j=0; j<vverts; j++) {
			float x = (u - 0.5) * width;
			float y = (v - 0.5) * height;
			float z = hf ? hf(u, v, hfdata) : 0.0;

			Vector3 normal = Vector3(0, 0, 1);
			if(hf) {
				float u1z = hf(u + du, v, hfdata);
				float v1z = hf(u, v + dv, hfdata);

				Vector3 tang = Vector3(du * width, 0, u1z - z);
				Vector3 bitan = Vector3(0, dv * height, v1z - z);
				normal = cross_product(tang, bitan).normalized();
			}

			*varr++ = Vector3(x, y, z);
			*narr++ = normal;
			*tarr++ = Vector3(1, 0, 0);
			*uvarr++ = Vector2(u, v);

			if(i < usub && j < vsub) {
				int idx = i * vverts + j;

				*idxarr++ = idx;
				*idxarr++ = idx + vverts + 1;
				*idxarr++ = idx + 1;

				*idxarr++ = idx;
				*idxarr++ = idx + vverts;
				*idxarr++ = idx + vverts + 1;
			}

			v += dv;
		}
		u += du;
	}
}

// ----- box ------
void gen_box(Mesh *mesh, float xsz, float ysz, float zsz, int usub, int vsub)
{
	static const float face_angles[][2] = {
		{0, 0},
		{M_PI / 2.0, 0},
		{M_PI, 0},
		{3.0 * M_PI / 2.0, 0},
		{0, M_PI / 2.0},
		{0, -M_PI / 2.0}
	};

	if(usub < 1) usub = 1;
	if(vsub < 1) vsub = 1;

	mesh->clear();

	for(int i=0; i<6; i++) {
		Matrix4x4 xform, dir_xform;
		Mesh m;

		gen_plane(&m, 1, 1, usub, vsub);
		xform.rotate(Vector3(face_angles[i][1], face_angles[i][0], 0));
		dir_xform = xform;
		xform.translate(Vector3(0, 0, 0.5));
		m.apply_xform(xform, dir_xform);

		mesh->append(m);
	}

	Matrix4x4 scale;
	scale.set_scaling(Vector3(xsz, ysz, zsz));
	mesh->apply_xform(scale, Matrix4x4::identity);
}

/*
void gen_box(Mesh *mesh, float xsz, float ysz, float zsz)
{
	mesh->clear();

	const int num_faces = 6;
	int num_verts = num_faces * 4;
	int num_tri = num_faces * 2;

	float x = xsz / 2.0;
	float y = ysz / 2.0;
	float z = zsz / 2.0;

	Vector3 *varr = (Vector3*)mesh->set_attrib_data(MESH_ATTR_VERTEX, 3, num_verts, 0);
	Vector3 *narr = (Vector3*)mesh->set_attrib_data(MESH_ATTR_NORMAL, 3, num_verts, 0);
	Vector3 *tarr = (Vector3*)mesh->set_attrib_data(MESH_ATTR_TANGENT, 3, num_verts, 0);
	Vector2 *uvarr = (Vector2*)mesh->set_attrib_data(MESH_ATTR_TEXCOORD, 2, num_verts, 0);
	unsigned int *idxarr = mesh->set_index_data(num_tri * 3, 0);

	static const Vector2 uv[] = { Vector2(0, 0), Vector2(1, 0), Vector2(1, 1), Vector2(0, 1) };

	// front
	for(int i=0; i<4; i++) {
		*narr++ = Vector3(0, 0, 1);
		*tarr++ = Vector3(1, 0, 0);
		*uvarr++ = uv[i];
	}
	*varr++ = Vector3(-x, -y, z);
	*varr++ = Vector3(x, -y, z);
	*varr++ = Vector3(x, y, z);
	*varr++ = Vector3(-x, y, z);
	// right
	for(int i=0; i<4; i++) {
		*narr++ = Vector3(1, 0, 0);
		*tarr++ = Vector3(0, 0, -1);
		*uvarr++ = uv[i];
	}
	*varr++ = Vector3(x, -y, z);
	*varr++ = Vector3(x, -y, -z);
	*varr++ = Vector3(x, y, -z);
	*varr++ = Vector3(x, y, z);
	// back
	for(int i=0; i<4; i++) {
		*narr++ = Vector3(0, 0, -1);
		*tarr++ = Vector3(-1, 0, 0);
		*uvarr++ = uv[i];
	}
	*varr++ = Vector3(x, -y, -z);
	*varr++ = Vector3(-x, -y, -z);
	*varr++ = Vector3(-x, y, -z);
	*varr++ = Vector3(x, y, -z);
	// left
	for(int i=0; i<4; i++) {
		*narr++ = Vector3(-1, 0, 0);
		*tarr++ = Vector3(0, 0, 1);
		*uvarr++ = uv[i];
	}
	*varr++ = Vector3(-x, -y, -z);
	*varr++ = Vector3(-x, -y, z);
	*varr++ = Vector3(-x, y, z);
	*varr++ = Vector3(-x, y, -z);
	// top
	for(int i=0; i<4; i++) {
		*narr++ = Vector3(0, 1, 0);
		*tarr++ = Vector3(1, 0, 0);
		*uvarr++ = uv[i];
	}
	*varr++ = Vector3(-x, y, z);
	*varr++ = Vector3(x, y, z);
	*varr++ = Vector3(x, y, -z);
	*varr++ = Vector3(-x, y, -z);
	// bottom
	for(int i=0; i<4; i++) {
		*narr++ = Vector3(0, -1, 0);
		*tarr++ = Vector3(1, 0, 0);
		*uvarr++ = uv[i];
	}
	*varr++ = Vector3(-x, -y, -z);
	*varr++ = Vector3(x, -y, -z);
	*varr++ = Vector3(x, -y, z);
	*varr++ = Vector3(-x, -y, z);

	// index array
	static const int faceidx[] = {0, 1, 2, 0, 2, 3};
	for(int i=0; i<num_faces; i++) {
		for(int j=0; j<6; j++) {
			*idxarr++ = faceidx[j] + i * 4;
		}
	}
}
*/

static inline Vector3 rev_vert(float u, float v, Vector2 (*rf)(float, float, void*), void *cls)
{
	Vector2 pos = rf(u, v, cls);

	float angle = u * 2.0 * M_PI;
	float x = pos.x * cos(angle);
	float y = pos.y;
	float z = pos.x * sin(angle);

	return Vector3(x, y, z);
}

// ------ surface of revolution -------
void gen_revol(Mesh *mesh, int usub, int vsub, Vector2 (*rfunc)(float, float, void*), void *cls)
{
	gen_revol(mesh, usub, vsub, rfunc, 0, cls);
}

void gen_revol(Mesh *mesh, int usub, int vsub, Vector2 (*rfunc)(float, float, void*),
		Vector2 (*nfunc)(float, float, void*), void *cls)
{
	if(!rfunc) return;
	if(usub < 3) usub = 3;
	if(vsub < 1) vsub = 1;

	mesh->clear();

	int uverts = usub + 1;
	int vverts = vsub + 1;
	int num_verts = uverts * vverts;

	int num_quads = usub * vsub;
	int num_tri = num_quads * 2;

	Vector3 *varr = (Vector3*)mesh->set_attrib_data(MESH_ATTR_VERTEX, 3, num_verts, 0);
	Vector3 *narr = (Vector3*)mesh->set_attrib_data(MESH_ATTR_NORMAL, 3, num_verts, 0);
	Vector3 *tarr = (Vector3*)mesh->set_attrib_data(MESH_ATTR_TANGENT, 3, num_verts, 0);
	Vector2 *uvarr = (Vector2*)mesh->set_attrib_data(MESH_ATTR_TEXCOORD, 2, num_verts, 0);
	unsigned int *idxarr = mesh->set_index_data(num_tri * 3, 0);

	float du = 1.0 / (float)(uverts - 1);
	float dv = 1.0 / (float)(vverts - 1);

	float u = 0.0;
	for(int i=0; i<uverts; i++) {
		float v = 0.0;
		for(int j=0; j<vverts; j++) {
			Vector3 pos = rev_vert(u, v, rfunc, cls);

			Vector3 nextu = rev_vert(fmod(u + du, 1.0), v, rfunc, cls);
			Vector3 tang = nextu - pos;
			if(tang.length_sq() < 1e-6) {
				float new_v = v > 0.5 ? v - dv * 0.25 : v + dv * 0.25;
				nextu = rev_vert(fmod(u + du, 1.0), new_v, rfunc, cls);
				tang = nextu - pos;
			}

			Vector3 normal;
			if(nfunc) {
				normal = rev_vert(u, v, nfunc, cls);
			} else {
				Vector3 nextv = rev_vert(u, v + dv, rfunc, cls);
				Vector3 bitan = nextv - pos;
				if(bitan.length_sq() < 1e-6) {
					nextv = rev_vert(u, v - dv, rfunc, cls);
					bitan = pos - nextv;
				}

				normal = cross_product(tang, bitan);
			}

			*varr++ = pos;
			*narr++ = normal.normalized();
			*tarr++ = tang.normalized();
			*uvarr++ = Vector2(u, v);

			if(i < usub && j < vsub) {
				int idx = i * vverts + j;

				*idxarr++ = idx;
				*idxarr++ = idx + vverts + 1;
				*idxarr++ = idx + 1;

				*idxarr++ = idx;
				*idxarr++ = idx + vverts;
				*idxarr++ = idx + vverts + 1;
			}

			v += dv;
		}
		u += du;
	}
}


static inline Vector3 sweep_vert(float u, float v, float height, Vector2 (*sf)(float, float, void*), void *cls)
{
	Vector2 pos = sf(u, v, cls);

	float x = pos.x;
	float y = v * height;
	float z = pos.y;

	return Vector3(x, y, z);
}

// ---- sweep shape along a path ----
void gen_sweep(Mesh *mesh, float height, int usub, int vsub, Vector2 (*sfunc)(float, float, void*), void *cls)
{
	if(!sfunc) return;
	if(usub < 3) usub = 3;
	if(vsub < 1) vsub = 1;

	mesh->clear();

	int uverts = usub + 1;
	int vverts = vsub + 1;
	int num_verts = uverts * vverts;

	int num_quads = usub * vsub;
	int num_tri = num_quads * 2;

	Vector3 *varr = (Vector3*)mesh->set_attrib_data(MESH_ATTR_VERTEX, 3, num_verts, 0);
	Vector3 *narr = (Vector3*)mesh->set_attrib_data(MESH_ATTR_NORMAL, 3, num_verts, 0);
	Vector3 *tarr = (Vector3*)mesh->set_attrib_data(MESH_ATTR_TANGENT, 3, num_verts, 0);
	Vector2 *uvarr = (Vector2*)mesh->set_attrib_data(MESH_ATTR_TEXCOORD, 2, num_verts, 0);
	unsigned int *idxarr = mesh->set_index_data(num_tri * 3, 0);

	float du = 1.0 / (float)(uverts - 1);
	float dv = 1.0 / (float)(vverts - 1);

	float u = 0.0;
	for(int i=0; i<uverts; i++) {
		float v = 0.0;
		for(int j=0; j<vverts; j++) {
			Vector3 pos = sweep_vert(u, v, height, sfunc, cls);

			Vector3 nextu = sweep_vert(fmod(u + du, 1.0), v, height, sfunc, cls);
			Vector3 tang = nextu - pos;
			if(tang.length_sq() < 1e-6) {
				float new_v = v > 0.5 ? v - dv * 0.25 : v + dv * 0.25;
				nextu = sweep_vert(fmod(u + du, 1.0), new_v, height, sfunc, cls);
				tang = nextu - pos;
			}

			Vector3 normal;
			Vector3 nextv = sweep_vert(u, v + dv, height, sfunc, cls);
			Vector3 bitan = nextv - pos;
			if(bitan.length_sq() < 1e-6) {
				nextv = sweep_vert(u, v - dv, height, sfunc, cls);
				bitan = pos - nextv;
			}

			normal = cross_product(tang, bitan);

			*varr++ = pos;
			*narr++ = normal.normalized();
			*tarr++ = tang.normalized();
			*uvarr++ = Vector2(u, v);

			if(i < usub && j < vsub) {
				int idx = i * vverts + j;

				*idxarr++ = idx;
				*idxarr++ = idx + vverts + 1;
				*idxarr++ = idx + 1;

				*idxarr++ = idx;
				*idxarr++ = idx + vverts;
				*idxarr++ = idx + vverts + 1;
			}

			v += dv;
		}
		u += du;
	}
}
