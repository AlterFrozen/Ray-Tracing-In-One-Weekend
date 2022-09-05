#pragma once
//STD
#include <random>
#include <numeric>
#include <cmath>

//3rd Party
#include "glm/glm.hpp"

namespace
{
	static std::default_random_engine defaultRandomEngine;
	static std::uniform_real_distribution<float> ufDistribution(-1.0, 1.0);
}

glm::vec3 random_in_unit_sphere();
glm::vec3 random_in_unit_disk();

float russianRoulette();

glm::vec3 calculate_reflect(glm::vec3 vector, glm::vec3 normal); 
bool calculate_refract(glm::vec3 ray_in, glm::vec3 normal, float n2_ovew_n1, glm::vec3& result);

float fresnel_schlick(float cosine, float refractive_indice);

void gamma_correction(glm::vec3& color);