#pragma once

#include <random>

#include "glm/glm.hpp"

#include "ray.hpp"
#include "utils.hpp"

class Ray;

class Material
{
public:
	typedef enum { SPECULAR, DIFFUSE, DIELECTRIC} MaterialTypes;
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

struct Dielectric
	:public Material
{
	Dielectric(float refractive_indice) :refractiveIndice{ refractive_indice }, Material{ DIELECTRIC } {};

	virtual bool scatter(Ray& ray_in, Ray& ray_scatter, glm::vec3& attenuation) const;

	float refractiveIndice = 1.0f; //N(medium) / N(air/vacuum)
};