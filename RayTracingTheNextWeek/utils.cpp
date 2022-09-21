#include "utils.hpp"

void gamma_correction(glm::vec3& color)
{
	color.r = sqrt(color.r);
	color.g = sqrt(color.g);
	color.b = sqrt(color.b);
}

glm::vec3 calculate_reflect(glm::vec3 vector, glm::vec3 normal)
{
	return vector + (2.0f * glm::dot(-vector, normal) * normal);
}

bool calculate_refract(glm::vec3 ray_in, glm::vec3 outward_normal, float nStart_over_nOut, glm::vec3& result)
{
	glm::vec3 uniformV = glm::normalize(ray_in);
	float cosStart = glm::dot(uniformV, - outward_normal);
	float sinStart = 1.0f - cosStart * cosStart;
	float cosOut = 1.0f - (nStart_over_nOut * nStart_over_nOut) * sinStart;
	if (cosOut > 0.0f)
	{
		result = nStart_over_nOut * ray_in + (  (nStart_over_nOut * cosStart - sqrt(abs(cosOut))) * outward_normal  );
		//result = nStart_over_nOut * (ray_in - outward_normal * cosStart) - outward_normal * sqrt(cosOut);
		return true;
	}
	else return false; // total inner refraction
}

float fresnel_schlick(float cosine, float refractive_indice)
{
	float r0 = (1.0f - refractive_indice) / (1.0f + refractive_indice);
	r0 *= r0;
	return r0 + (1.0f - r0) * std::pow((1.0f - cosine), 5.0f);
}

glm::vec3 random_in_unit_sphere()
{
	glm::vec3 innerPoint;
	do {
		innerPoint = glm::vec3(randfMOneToOne(), randfMOneToOne(), randfMOneToOne());
	} while (glm::dot(innerPoint, innerPoint) >= 1.0);
	return innerPoint;
}

glm::vec3 random_in_unit_disk()
{
	glm::vec3 innerPoint;
	do {
		innerPoint = glm::vec3(randfMOneToOne(), randfMOneToOne(), 0.0f);
	} while (glm::dot(innerPoint, innerPoint) > 1.0);
	return innerPoint;
}

float russianRoulette()
{
	static std::default_random_engine defaultEngine;
	static std::uniform_real_distribution<float> russian_roulette(0.0, 1.0);
	return russian_roulette(defaultEngine);
}
