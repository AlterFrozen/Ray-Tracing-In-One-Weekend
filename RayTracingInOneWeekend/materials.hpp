#pragma once

#include "glm/glm.hpp"

#include "ray.hpp"
#include "utils.hpp"

class Ray;

class Material
{
public:
	typedef enum { SPECULAR, DIFFUSE, TRANSPARENT } MaterialTypes;
	Material() = delete;
	Material(MaterialTypes material_type) :material_type{ material_type } {  };

	virtual bool scatter(Ray& ray_in, Ray& ray_scatter, glm::vec3& attenuation) const = 0;

public:
	MaterialTypes material_type;
	float fuzz = 0.0f;
};

struct Lambertian // Diffuse
	:public Material
{
	Lambertian(glm::vec3 abedo) :abedo{ abedo }, Material{ DIFFUSE } {};

	virtual bool scatter(Ray& ray_in, Ray& ray_scatter, glm::vec3& attenuation) const;

	glm::vec3 abedo; 
};

struct Metal // Specular
	:public Material
{
	Metal(glm::vec3 abedo) :abedo{ abedo }, Material{ SPECULAR } {};

	virtual bool scatter(Ray& ray_in, Ray& ray_scatter, glm::vec3& attenuation) const;

	glm::vec3 abedo;
};