#include "materials.hpp"

bool Lambertian::scatter(Ray& ray_in, Ray& ray_scatter, glm::vec3& attenuation) const
{
	glm::vec3 s = ray_in.hitInfo().hit_point + ray_in.hitInfo().hit_point_normal + random_in_unit_sphere();

	ray_scatter = Ray(ray_in.hitInfo().hit_point, s - ray_in.hitInfo().hit_point);
	attenuation = this->abedo->value(ray_in.hitInfo().hit_point_uv[0], ray_in.hitInfo().hit_point_uv[1], ray_in.hitInfo().hit_point);
	return true;
}

bool Metal::scatter(Ray& ray_in, Ray& ray_scatter, glm::vec3& attenuation) const
{
	glm::vec3 reflected = glm::normalize(calculate_reflect(ray_in.direction(), ray_in.hitInfo().hit_point_normal));
	ray_scatter = Ray(ray_in.hitInfo().hit_point, reflected + this->fuzz * random_in_unit_sphere());
	attenuation = this->abedo;
	return (glm::dot(ray_scatter.direction(), ray_in.hitInfo().hit_point_normal) > 0.0f);
}

bool Dielectric::scatter(Ray& ray_in, Ray& ray_scatter, glm::vec3& attenuation) const
{
	glm::vec3 outward_normal;
	float nstart_over_nout;
	attenuation = glm::vec3(1.0, 1.0, 1.0); // Glass surface absorbs nothing
	float cosine;

	if (glm::dot(ray_in.direction(), ray_in.hitInfo().hit_point_normal) > 0.0f) 
	{
		outward_normal = - ray_in.hitInfo().hit_point_normal;
		nstart_over_nout = this->refractiveIndice / 1.0; // medium -> air
		cosine = this->refractiveIndice * glm::dot(ray_in.direction(), ray_in.hitInfo().hit_point_normal) / ray_in.direction().length();
	}
	else
	{
		outward_normal = ray_in.hitInfo().hit_point_normal;
		nstart_over_nout = 1.0 / this->refractiveIndice; // air -> medium
		cosine = - glm::dot(ray_in.direction(), ray_in.hitInfo().hit_point_normal) / ray_in.direction().length();
	}

	glm::vec3 ray_refracted;
	glm::vec3 ray_reflected = calculate_reflect(ray_in.direction(), - outward_normal);
	float reflect_prob = 0.0f;

	if (calculate_refract(ray_in.direction(), outward_normal, nstart_over_nout, ray_refracted))
	{
		reflect_prob = fresnel_schlick(cosine, this->refractiveIndice);
	} else 	reflect_prob = 1.0f;

	static std::default_random_engine e;
	static std::uniform_real_distribution<float> uni_real_distri(0.0f, 1.0f);

	bool isReflective = uni_real_distri(e) < reflect_prob;
	if (isReflective) ray_scatter = Ray(ray_in.hitInfo().hit_point, ray_reflected);
	else ray_scatter = Ray(ray_in.hitInfo().hit_point, ray_refracted);

	return true; // continue since the reflection is random now
}