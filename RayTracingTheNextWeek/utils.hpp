#pragma once
//STD
#include <random>
#include <numeric>
#include <memory>
#include <cmath>
#include <chrono>
#include <filesystem>

//3rd Party
#include "glm/glm.hpp"

namespace
{
	const auto TIME_START = std::chrono::high_resolution_clock::now();
}

class UniformFloatRandomer
{
public:
	UniformFloatRandomer(float lower, float upper)
		:e((std::chrono::high_resolution_clock::now() - TIME_START).count())
		,uniDis{ std::make_unique< std::uniform_real_distribution<float> >(lower, upper) } {};

	float operator() (void) { return ( * (this->uniDis))(this->e); }
private:
	std::default_random_engine e;
	std::unique_ptr<std::uniform_real_distribution<float>> uniDis;
};

class UniformIntegerRandomer
{
public:
	UniformIntegerRandomer(int lower, int upper)
		:e((std::chrono::high_resolution_clock::now() - TIME_START).count())
		, uniDis{ std::make_unique< std::uniform_int_distribution<int> >(lower, upper) } {};

	float operator() (void) { return (*(this->uniDis))(this->e); }
private:
	std::default_random_engine e;
	std::unique_ptr<std::uniform_int_distribution<int>> uniDis;
};

static auto randfMOneToOne = UniformFloatRandomer(-1.0, 1.0);

glm::vec3 random_in_unit_sphere();
glm::vec3 random_in_unit_disk();

float russianRoulette();

glm::vec3 calculate_reflect(glm::vec3 vector, glm::vec3 normal); 
bool calculate_refract(glm::vec3 ray_in, glm::vec3 normal, float n2_ovew_n1, glm::vec3& result);

float fresnel_schlick(float cosine, float refractive_indice);

void gamma_correction(glm::vec3& color);

const std::string app_path = std::filesystem::current_path().generic_string() + "";//WARN>> Do not add '/' at the end