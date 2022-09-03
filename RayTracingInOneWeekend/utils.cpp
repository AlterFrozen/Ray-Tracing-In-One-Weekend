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

glm::vec3 random_in_unit_sphere()
{
	static std::default_random_engine defaultRandomEngine;
	static std::uniform_real_distribution<float> ufDistribution(-1.0, 1.0);

	glm::vec3 innerPoint;
	do {
		innerPoint = glm::vec3(ufDistribution(defaultRandomEngine), ufDistribution(defaultRandomEngine), ufDistribution(defaultRandomEngine));
	} while (glm::dot(innerPoint, innerPoint) >= 1.0);
	return innerPoint;
}