#include <assert.h>
#include <float.h>
#include <algorithm>
#include "geom.h"

GeomObject::~GeomObject()
{
}


Sphere::Sphere()
{
	radius = 1.0;
}

Sphere::Sphere(const Vector3 &cent, float radius)
	: center(cent)
{
	this->radius = radius;
}

void Sphere::set_union(const GeomObject *obj1, const GeomObject *obj2)
{
	const Sphere *sph1 = dynamic_cast<const Sphere*>(obj1);
	const Sphere *sph2 = dynamic_cast<const Sphere*>(obj2);

	if(!sph1 || !sph2) {
		fprintf(stderr, "Sphere::set_union: arguments must be spheres");
		return;
	}

	float dist = (sph1->center - sph2->center).length();
	float surf_dist = dist - (sph1->radius + sph2->radius);
	float d1 = sph1->radius + surf_dist / 2.0;
	float d2 = sph2->radius + surf_dist / 2.0;
	float t = d1 / (d1 + d2);

	if(t < 0.0) t = 0.0;
	if(t > 1.0) t = 1.0;

	center = sph1->center * t + sph2->center * (1.0 - t);
	radius = std::max(dist * t + sph2->radius, dist * (1.0f - t) + sph1->radius);
}

void Sphere::set_intersection(const GeomObject *obj1, const GeomObject *obj2)
{
	fprintf(stderr, "Sphere::intersection undefined\n");
}

bool Sphere::intersect(const Ray &ray, HitPoint *hit) const
{
	float a = dot_product(ray.dir, ray.dir);
	float b = 2.0 * ray.dir.x * (ray.origin.x - center.x) +
		2.0 * ray.dir.y * (ray.origin.y - center.y) +
		2.0 * ray.dir.z * (ray.origin.z - center.z);
	float c = dot_product(ray.origin, ray.origin) + dot_product(center, center) -
		2.0 * dot_product(ray.origin, center) - radius * radius;

	float discr = b * b - 4.0 * a * c;
	if(discr < 1e-4) {
		return false;
	}

	float sqrt_discr = sqrt(discr);
	float t0 = (-b + sqrt_discr) / (2.0 * a);
	float t1 = (-b - sqrt_discr) / (2.0 * a);

	if(t0 < 1e-4)
		t0 = t1;
	if(t1 < 1e-4)
		t1 = t0;

	float t = t0 < t1 ? t0 : t1;
	if(t < 1e-4) {
		return false;
	}

	// fill the HitPoint structure
	if(hit) {
		hit->obj = this;
		hit->dist = t;
		hit->pos = ray.origin + ray.dir * t;
		hit->normal = (hit->pos - center) / radius;
	}
	return true;
}


AABox::AABox()
{
}

AABox::AABox(const Vector3 &vmin, const Vector3 &vmax)
	: min(vmin), max(vmax)
{
}

void AABox::set_union(const GeomObject *obj1, const GeomObject *obj2)
{
	const AABox *box1 = dynamic_cast<const AABox*>(obj1);
	const AABox *box2 = dynamic_cast<const AABox*>(obj2);

	if(!box1 || !box2) {
		fprintf(stderr, "AABox::set_union: arguments must be AABoxes too\n");
		return;
	}

	min.x = std::min(box1->min.x, box2->min.x);
	min.y = std::min(box1->min.y, box2->min.y);
	min.z = std::min(box1->min.z, box2->min.z);

	max.x = std::max(box1->max.x, box2->max.x);
	max.y = std::max(box1->max.y, box2->max.y);
	max.z = std::max(box1->max.z, box2->max.z);
}

void AABox::set_intersection(const GeomObject *obj1, const GeomObject *obj2)
{
	const AABox *box1 = dynamic_cast<const AABox*>(obj1);
	const AABox *box2 = dynamic_cast<const AABox*>(obj2);

	if(!box1 || !box2) {
		fprintf(stderr, "AABox::set_intersection: arguments must be AABoxes too\n");
		return;
	}

	for(int i=0; i<3; i++) {
		min[i] = std::max(box1->min[i], box2->min[i]);
		max[i] = std::min(box1->max[i], box2->max[i]);

		if(max[i] < min[i]) {
			max[i] = min[i];
		}
	}
}

bool AABox::intersect(const Ray &ray, HitPoint *hit) const
{
	Vector3 param[2] = {min, max};
	Vector3 inv_dir(1.0 / ray.dir.x, 1.0 / ray.dir.y, 1.0 / ray.dir.z);
	int sign[3] = {inv_dir.x < 0, inv_dir.y < 0, inv_dir.z < 0};

	float tmin = (param[sign[0]].x - ray.origin.x) * inv_dir.x;
	float tmax = (param[1 - sign[0]].x - ray.origin.x) * inv_dir.x;
	float tymin = (param[sign[1]].y - ray.origin.y) * inv_dir.y;
	float tymax = (param[1 - sign[1]].y - ray.origin.y) * inv_dir.y;

	if(tmin > tymax || tymin > tmax) {
		return false;
	}
	if(tymin > tmin) {
		tmin = tymin;
	}
	if(tymax < tmax) {
		tmax = tymax;
	}

	float tzmin = (param[sign[2]].z - ray.origin.z) * inv_dir.z;
	float tzmax = (param[1 - sign[2]].z - ray.origin.z) * inv_dir.z;

	if(tmin > tzmax || tzmin > tmax) {
		return false;
	}
	if(tzmin > tmin) {
		tmin = tzmin;
	}
	if(tzmax < tmax) {
		tmax = tzmax;
	}

	float t = tmin < 1e-4 ? tmax : tmin;
	if(t >= 1e-4) {

		if(hit) {
			hit->obj = this;
			hit->dist = t;
			hit->pos = ray.origin + ray.dir * t;

			float min_dist = FLT_MAX;
			Vector3 offs = min + (max - min) / 2.0;
			Vector3 local_hit = hit->pos - offs;

			static const Vector3 axis[] = {
				Vector3(1, 0, 0), Vector3(0, 1, 0), Vector3(0, 0, 1)
			};
			//int tcidx[][2] = {{2, 1}, {0, 2}, {0, 1}};

			for(int i=0; i<3; i++) {
				float dist = fabs((max[i] - offs[i]) - fabs(local_hit[i]));
				if(dist < min_dist) {
					min_dist = dist;
					hit->normal = axis[i] * (local_hit[i] < 0.0 ? 1.0 : -1.0);
					//hit->texcoord = Vector2(hit->pos[tcidx[i][0]], hit->pos[tcidx[i][1]]);
				}
			}
		}
		return true;
	}
	return false;

}

Plane::Plane()
	: normal(0.0, 1.0, 0.0)
{
}

Plane::Plane(const Vector3 &p, const Vector3 &norm)
	: pt(p)
{
	normal = norm.normalized();
}

Plane::Plane(const Vector3 &p1, const Vector3 &p2, const Vector3 &p3)
	: pt(p1)
{
	normal = cross_product(p2 - p1, p3 - p1).normalized();
}

Plane::Plane(const Vector3 &normal, float dist)
{
	this->normal = normal.normalized();
	pt = this->normal * dist;
}

void Plane::set_union(const GeomObject *obj1, const GeomObject *obj2)
{
	fprintf(stderr, "Plane::set_union undefined\n");
}

void Plane::set_intersection(const GeomObject *obj1, const GeomObject *obj2)
{
	fprintf(stderr, "Plane::set_intersection undefined\n");
}

bool Plane::intersect(const Ray &ray, HitPoint *hit) const
{
	float ndotdir = dot_product(normal, ray.dir);
	if(fabs(ndotdir) < 1e-4) {
		return false;
	}

	if(hit) {
		Vector3 ptdir = pt - ray.origin;
		float t = dot_product(normal, ptdir) / ndotdir;

		hit->pos = ray.origin + ray.dir * t;
		hit->normal = normal;
		hit->obj = this;
	}
	return true;
}

float sphere_distance(const Vector3 &cent, float rad, const Vector3 &pt)
{
	return (pt - cent).length() - rad;
}

// TODO version which takes both radii into account
float capsule_distance(const Vector3 &a, float ra, const Vector3 &b, float rb, const Vector3 &pt)
{
	Vector3 ab_dir = b - a;
	float ab_len_sq = ab_dir.length_sq();

	if(fabs(ab_len_sq) < 1e-5) {
		// if a == b, the capsule is a sphere with radius the maximum of the capsule radii
		return sphere_distance(a, std::max(ra, rb), pt);
	}
	float ab_len = sqrt(ab_len_sq);

	Vector3 ap_dir = pt - a;

	float t = dot_product(ap_dir, ab_dir / ab_len) / ab_len;
	if(t < 0.0) {
		return sphere_distance(a, ra, pt);
	}
	if(t >= 1.0) {
		return sphere_distance(b, rb, pt);
	}

	Vector3 pproj = a + ab_dir * t;
	return (pproj - pt).length() - ra;
}

#if 0
float capsule_distance(const Vector3 &a, float ra, const Vector3 &b, float rb, const Vector3 &pt)
{
	Vector3 ab_dir = b - a;

	if(fabs(ab_dir.length_sq()) < 1e-5) {
		// if a == b, the capsule is a sphere with radius the maximum of the capsule radii
		return sphere_distance(a, std::max(ra, rb), pt);
	}
	float ab_len = ab_dir.length();

	Vector3 ap_dir = pt - a;
	Vector3 rotaxis = cross_product(ab_dir, ap_dir).normalized();

	Matrix4x4 rmat;
	rmat.set_rotation(rotaxis, M_PI / 2.0);
	Vector3 right = ab_dir.transformed(rmat) / ab_len;

	// XXX I think this check is redundant, always false, due to the cross product order
	//assert(dot_product(right, ab_dir) >= 0.0);
	if(dot_product(right, ab_dir) < 0.0) {
		right = -right;
	}
	Vector3 aa = a + right * ra;
	Vector3 bb = b + right * rb;

	// project pt to the line segment bb-aa, see if the projection lies within the interval [0, 1)
	Vector3 aabb_dir = bb - aa;
	float aabb_len = aabb_dir.length();
	Vector3 aap_dir = pt - aa;

	float t = dot_product(aap_dir, aabb_dir / aabb_len) / aabb_len;
	if(t < 0.0) {
		return sphere_distance(a, ra, pt);
	}
	if(t >= 1.0) {
		return sphere_distance(b, rb, pt);
	}

	Vector3 ppt = aa + aabb_dir * t;
	Vector3 norm = ppt - pt;
	float dist = norm.length();

	if(dot_product(norm, right) < 0.0) {
		// inside the cone
		dist = -dist;
	}
	return dist;
}
#endif
