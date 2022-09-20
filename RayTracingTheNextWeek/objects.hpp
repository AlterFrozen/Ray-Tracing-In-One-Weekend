#pragma once

#include <numeric>
#include <memory>

#include "glm/glm.hpp"

#include "ray.hpp"
#include "materials.hpp"
#include "BVH.hpp"

class AABB;
class Object
{
public:
	Object(std::shared_ptr<Material> material_info) : material{ material_info } {};
	virtual bool intersectionTest(Ray& ray, float t_min, float t_max) = 0;
	virtual bool bindAABB(AABB* aabb) = 0;

public:
	std::shared_ptr<Material> material;
};

class Sphere
	:public Object
{
	// Equation of the sphere in vector form>>  dot(point - center, point - center) = R*R
	// Ray Intersection Test>> point = ray
public:
	Sphere() = delete;
	Sphere(glm::vec3 center, float radius, std::shared_ptr<Material> material_info)
		:center{ center }, radius{ radius }, Object{ material_info }
	{
		this->material->fuzz = std::min(radius, 1.0f);
	}

	virtual bool intersectionTest(Ray& ray, float t_min, float t_max);
	virtual bool bindAABB(AABB* aabb);
public:
	glm::vec3 center;
	float radius;
};