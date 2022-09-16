#pragma once

#include <numeric>
#include <memory>

#include "glm/glm.hpp"

#include "ray.hpp"
#include "materials.hpp"

class Object
{
public:
	Object(glm::vec3 color, std::shared_ptr<Material> material_info) :color{ color }, material{ material_info } {};
	virtual bool intersectionTest(Ray& ray, float t_min, float t_max) = 0;

public:
	glm::vec3 color;
	std::shared_ptr<Material> material;
};

class Sphere
	:public Object
{
	// Equation of the sphere in vector form>>  dot(point - center, point - center) = R*R
	// Ray Intersection Test>> point = ray
public:
	Sphere() = delete;
	Sphere(glm::vec3 center, float radius, glm::vec3 color, std::shared_ptr<Material> material_info)
		:center{ center }, radius{ radius }, Object{ color , material_info } 
	{
		this->material->fuzz = std::min(radius, 1.0f);
	}

	virtual bool intersectionTest(Ray& ray, float t_min, float t_max);
public:
	glm::vec3 center;
	float radius;
};