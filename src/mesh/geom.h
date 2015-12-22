#ifndef GEOMOBJ_H_
#define GEOMOBJ_H_

#include "vmath/vmath.h"

class GeomObject;
class SceneNode;

struct HitPoint {
	float dist;				//< parametric distance along the ray
	Vector3 pos;			//< position of intersection (orig + dir * dist)
	Vector3 normal;			//< normal at the point of intersection
	const void *obj;		//< pointer to the intersected object
	const SceneNode *node;
	Ray ray;
};

class GeomObject {
public:
	virtual ~GeomObject();

	virtual void set_union(const GeomObject *obj1, const GeomObject *obj2) = 0;
	virtual void set_intersection(const GeomObject *obj1, const GeomObject *obj2) = 0;

	virtual bool intersect(const Ray &ray, HitPoint *hit = 0) const = 0;
};

class Sphere : public GeomObject {
public:
	Vector3 center;
	float radius;

	Sphere();
	Sphere(const Vector3 &center, float radius);

	void set_union(const GeomObject *obj1, const GeomObject *obj2);
	void set_intersection(const GeomObject *obj1, const GeomObject *obj2);

	bool intersect(const Ray &ray, HitPoint *hit = 0) const;
};

class AABox : public GeomObject {
public:
	Vector3 min, max;

	AABox();
	AABox(const Vector3 &min, const Vector3 &max);

	void set_union(const GeomObject *obj1, const GeomObject *obj2);
	void set_intersection(const GeomObject *obj1, const GeomObject *obj2);

	bool intersect(const Ray &ray, HitPoint *hit = 0) const;
};

class Plane : public GeomObject {
public:
	Vector3 pt, normal;

	Plane();
	Plane(const Vector3 &pt, const Vector3 &normal);
	Plane(const Vector3 &p1, const Vector3 &p2, const Vector3 &p3);
	Plane(const Vector3 &normal, float dist);

	void set_union(const GeomObject *obj1, const GeomObject *obj2);
	void set_intersection(const GeomObject *obj1, const GeomObject *obj2);

	bool intersect(const Ray &ray, HitPoint *hit = 0) const;
};

float sphere_distance(const Vector3 &cent, float rad, const Vector3 &pt);
float capsule_distance(const Vector3 &a, float ra, const Vector3 &b, float rb, const Vector3 &pt);

#endif	// GEOMOBJ_H_
