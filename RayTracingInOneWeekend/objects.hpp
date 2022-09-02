#pragma once

#include <numeric>

#include "glm/glm.hpp"
#include "ray.hpp"

class Object
{
public:
	Object(glm::vec3 color) :color{ color }{};
	virtual bool intersectionTest(Ray& ray, float t_min, float t_max) = 0;

public:
	glm::vec3 color;
};

class Sphere
	:public Object
{
	// Equation of the sphere in vector form>>  dot(point - center, point - center) = R*R
	// Ray Intersection Test>> point = ray
public:
	Sphere() = delete;
	Sphere(glm::vec3 center, float radius, glm::vec3 color) :center{ center }, radius{ radius }, Object{ color } {};

	virtual bool intersectionTest(Ray& ray, float t_min, float t_max)
	{
		if (t_min > t_max) throw std::runtime_error("intersectionTest>> t_min is greater than t_max!");

		glm::vec3 oc = ray.origin() - center;
		float a = glm::dot(ray.direction(), ray.direction());
		float b = 2.0 * dot(oc, ray.direction());
		float c = dot(oc, oc) - radius * radius;
		float discriminant = (b * b) - (4 * a * c);

		bool isVaildIntersection = false;
		if (discriminant > 0.0f)
		{
			float temp = (-b - sqrt(discriminant)) / (2.0 * a); // first root
			if (t_min < temp && temp < t_max) isVaildIntersection = true;
			else {
				temp = (-b + sqrt(discriminant)) / (2.0 * a); // second root
				if (t_min < temp && temp < t_max) isVaildIntersection = true;
			}

			if (isVaildIntersection && temp < ray.hitInfo().hit_time_first) // Note that: temp may equal to ray's hit_time_first
			{
				ray.hitInfo().hit_time_first = temp;
				ray.hitInfo().hit_point = ray.cast(ray.hitInfo().hit_time_first);
				ray.hitInfo().hit_point_normal = glm::normalize(ray.hitInfo().hit_point - this->center);
			}
		}
		return isVaildIntersection;
	}
public:
	glm::vec3 center;
	float radius;
};