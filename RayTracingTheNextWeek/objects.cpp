#include "objects.hpp"

bool Sphere::intersectionTest(Ray& ray, float t_min, float t_max)
{
	if (t_min > t_max) throw std::runtime_error("intersectionTest>> t_min is greater than t_max!");

	glm::vec3 oc = ray.origin() - center;
	float a = glm::dot(ray.direction(), ray.direction());
	float b = 2.0 * glm::dot(oc, ray.direction());
	float c = glm::dot(oc, oc) - radius * radius;
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
			ray.hitInfo().hit_point_normal = (ray.hitInfo().hit_point - this->center) / this->radius;
			ray.hitInfo().hit_object_material = this->material;
			getUV(ray.hitInfo().hit_point_normal, ray.hitInfo().hit_point_uv[0], ray.hitInfo().hit_point_uv[1]);
			//ray.hitInfo().path.push_back(this->material);
		}
	}
	return isVaildIntersection;
}


bool Sphere::bindAABB(AABB* aabb)
{
	aabb->info.p_min = this->center - glm::vec3{this->radius, this->radius ,this->radius};
	aabb->info.p_max = this->center + glm::vec3{ this->radius, this->radius ,this->radius };
	return true;
}

bool RectangleXY::intersectionTest(Ray& ray, float t_min, float t_max) 
{
	float t = (k - ray.origin().z) / ray.direction().z;
	if (t < t_min || t > t_max) return false;

	float x = ray.origin().x + t * ray.direction().x;
	float y = ray.origin().y + t * ray.direction().y;
	if (x < x0 || x  >x1 || y  < y0 || y  >y1) return false;

	if (t < ray.hitInfo().hit_time_first)
	{
		ray.hitInfo().hit_time_first = t;
		ray.hitInfo().hit_point = ray.cast(t);
		ray.hitInfo().hit_point_uv[0] = (x - x0) / (x1 - x0);
		ray.hitInfo().hit_point_uv[1] = (y - y0) / (y1 - y0);
		ray.hitInfo().hit_object_material = this->material;
		ray.hitInfo().hit_point_normal = glm::vec3(0, 0, 1);
	}
	return true;
}

bool RectangleXY::bindAABB(AABB* aabb)
{
	aabb->info.p_min = glm::vec3{ x0, y0, k - 0.0001 };
	aabb->info.p_max = glm::vec3{ x1,y1, k + 0.0001 };
	return true;
}

bool RectangleXZ::intersectionTest(Ray& ray, float t_min, float t_max)
{
	float t = (k - ray.origin().y) / ray.direction().y;
	if (t < t_min || t> t_max) return false;

	float x = ray.origin().x + t * ray.direction().x;
	float z = ray.origin().z + t * ray.direction().z;
	if (x < x0 || x > x1 || z <z0 || z > z1) return false;

	if (t < ray.hitInfo().hit_time_first)
	{
		ray.hitInfo().hit_time_first = t;
		ray.hitInfo().hit_point = ray.cast(t);
		ray.hitInfo().hit_point_uv[0] = (x - x0) / (x1 - x0);
		ray.hitInfo().hit_point_uv[1] = (z - z0) / (z1 - z0);
		ray.hitInfo().hit_object_material = this->material;
		ray.hitInfo().hit_point_normal = glm::vec3(0, 1, 0);
	}
	return true;
}

bool RectangleXZ::bindAABB(AABB* aabb)
{
	aabb->info.p_min = glm::vec3{ x0, k - 0.0001, z0 };
	aabb->info.p_max = glm::vec3{ x1, k + 0.0001, z1 };
	return true;
}

bool RectangleYZ::intersectionTest(Ray& ray, float t_min, float t_max)
{
	float t = (k - ray.origin().x) / ray.direction().x;
	if (t < t_min || t > t_max) return false;
	
	float y = ray.origin().y + t * ray.direction().y;
	float z = ray.origin().z + t * ray.direction().z;
	if (y < y0 || y > y1 || z < z0 || z > z1) return false;
	
	if (t < ray.hitInfo().hit_time_first)
	{
		ray.hitInfo().hit_time_first = t;
		ray.hitInfo().hit_point = ray.cast(t);
		ray.hitInfo().hit_point_uv[0] = (y - y0) / (y1 - y0);
		ray.hitInfo().hit_point_uv[1] = (z - z0) / (z1 - z0);
		ray.hitInfo().hit_object_material = this->material;
		ray.hitInfo().hit_point_normal = glm::vec3(1, 0, 0);
	}
	return true;
}

bool RectangleYZ::bindAABB(AABB* aabb)
{
	aabb->info.p_min = glm::vec3{ k - 0.0001, y0, z0 };
	aabb->info.p_max = glm::vec3{ k + 0.0001, y1, z1 };
	return true;
}