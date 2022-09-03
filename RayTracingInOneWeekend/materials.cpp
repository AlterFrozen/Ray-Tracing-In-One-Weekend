#include "materials.hpp"


bool Lambertian::scatter(Ray& ray_in, Ray& ray_scatter, glm::vec3& attenuation) const
{
	glm::vec3 s = ray_in.hitInfo().hit_point + ray_in.hitInfo().hit_point_normal + random_in_unit_sphere();

	ray_scatter = Ray(ray_in.hitInfo().hit_point, s - ray_in.hitInfo().hit_point);
	attenuation = this->abedo;
	return true;
}

bool Metal::scatter(Ray& ray_in, Ray& ray_scatter, glm::vec3& attenuation) const
{
	glm::vec3 reflected = glm::normalize(calculate_reflect(ray_in.direction(), ray_in.hitInfo().hit_point_normal));
	ray_scatter = Ray(ray_in.hitInfo().hit_point, reflected + this->fuzz * random_in_unit_sphere());
	attenuation = this->abedo;
	return (glm::dot(ray_scatter.direction(), ray_in.hitInfo().hit_point_normal) > 0.0f);
}