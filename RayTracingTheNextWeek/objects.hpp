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
	struct AddtionalInfo
	{
		bool flip_normal = false;
	};
public:
	Object() = default;
	Object(std::shared_ptr<Material> material_info, AddtionalInfo* additional_info = nullptr) : material{ material_info} {};
	virtual bool intersectionTest(Ray& ray, float t_min, float t_max) = 0;
	virtual bool bindAABB(AABB* aabb) = 0;

public:
	std::shared_ptr<Material> material;
	AddtionalInfo* additional_info;
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

class RectangleXY
	:public Object
{
public:
	RectangleXY() = delete;
	RectangleXY(float x0, float x1, float y0, float y1, float k, std::shared_ptr<Material> material_info)
		:x0{ x0 }, x1{ x1 }, y0{ y0 }, y1{ y1 }, k{ k }, Object{ material_info } {};

	virtual bool intersectionTest(Ray& ray, float t_min, float t_max) ;
	virtual bool bindAABB(AABB* aabb);
private:
	float x0, x1, y0, y1;
	float k;
};

class RectangleXZ
	:public Object
{
public:
	RectangleXZ() = delete;
	RectangleXZ(float x0, float x1, float z0, float z1, float k,  std::shared_ptr<Material> material_info)
		:x0{ x0 }, x1{ x1 }, z0{ z0 }, z1{ z1 }, k{ k }, Object{ material_info } {};

	virtual bool intersectionTest(Ray& ray, float t_min, float t_max);
	virtual bool bindAABB(AABB* aabb);
private:
	float x0, x1, z0, z1;
	float k;
};

class RectangleYZ
	:public Object
{
public:
	RectangleYZ() = delete;
	RectangleYZ(float y0, float y1, float z0, float z1, float k, std::shared_ptr<Material> material_info)
		:y0{ y0 }, y1{ y1 }, z0{ z0 }, z1{ z1 }, k{ k }, Object{ material_info } {};

	virtual bool intersectionTest(Ray& ray, float t_min, float t_max);
	virtual bool bindAABB(AABB* aabb);
private:
	float y0, y1, z0, z1;
	float k;
};

//class ConstantMedium
//	:public Object
//{
//public:
//	ConstantMedium(std::shared_ptr<Bounding> bounding, float dist, std::shared_ptr <Texture> texture)
//		:bounding{ bounding }, dist{ dist }, texture{ texture } {};
//
//private:
//	std::shared_ptr <Bounding> bounding;
//	float dist;
//	std::shared_ptr<Texture> texture
//};