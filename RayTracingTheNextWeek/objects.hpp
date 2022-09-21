#pragma once

#include <numeric>
#include <memory>

#include "glm/glm.hpp"
#include "glm/gtc/constants.hpp"

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

private:
	inline void getUV(const glm::vec3& p, float& result_u, float& result_v)
	{
		// Note that our definition of the coordinate
		float phi = atan2(p.z, p.x);
		float theta = asin(p.y);
		result_u = 1.0f - (phi + glm::pi<float>()) / glm::two_pi<float>();
		result_v = (theta + glm::half_pi<float>()) / glm::pi<float>();
	}

public:
	glm::vec3 center;
	float radius;
};