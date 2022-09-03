#include "objects.hpp"

bool Sphere::intersectionTest(Ray& ray, float t_min, float t_max)
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
			ray.hitInfo().hit_object_material = this->material;
		}
	}
	return isVaildIntersection;
}