#pragma once

#include <random>

#include "glm/glm.hpp"

#include "ray.hpp"
#include "utils.hpp"
#include "texture.hpp"

class Ray;

typedef enum { SPECULAR, DIFFUSE, DIELECTRIC, LIGHT , ISOTROPIC} MaterialTypes;

class Material
{
public:
	Material() = delete;
	Material(MaterialTypes material_type) :material_type{ material_type } {  };

	virtual bool scatter(Ray& ray_in, Ray& ray_scatter, glm::vec3& attenuation) const = 0;
	virtual glm::vec3 emitted(float u, float v, const glm::vec3& p) const { return { 0,0,0 }; } // return Black(No Emition)

public:
	MaterialTypes material_type;
	float fuzz = 0.0f;
};

struct Lambertian // Diffuse
	:public Material
{
	Lambertian(std::shared_ptr<Texture> abedo) :abedo{ abedo }, Material{ DIFFUSE } {};

	virtual bool scatter(Ray& ray_in, Ray& ray_scatter, glm::vec3& attenuation) const;

	std::shared_ptr<Texture> abedo; 
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

struct DiffuseLight
	:public Material
{
public:
	DiffuseLight() = delete;
	DiffuseLight(std::shared_ptr<Texture> light_info) :emit{ light_info }, Material{ LIGHT } {};
	virtual bool scatter(Ray& ray_in, Ray& ray_scatter, glm::vec3& attenuation) const { return false; } 
	virtual glm::vec3 emitted(float u, float v, const glm::vec3& p) const
	{
		return this->emit->value(u, v, p);
	}

private:
	std::shared_ptr<Texture> emit;
};

struct Isotropic
	:public Material
{
public:
	Isotropic() = delete;
	Isotropic(std::shared_ptr<Texture> texture) :emit{ texture }, Material{ ISOTROPIC } {};
	virtual bool scatter(Ray& ray_in, Ray& ray_scatter, glm::vec3& attenuation) const;
	virtual glm::vec3 emitted(float u, float v, const glm::vec3& p) const
	{
		return this->emit->value(u, v, p);
	}

private:
	std::shared_ptr<Texture> emit;
};