#pragma once
//STD
#include <random>
#include <numeric>

//3rd Party
#include "glm/glm.hpp"

glm::vec3 random_in_unit_sphere();

glm::vec3 calculate_reflect(glm::vec3 vector, glm::vec3 normal);

void gamma_correction(glm::vec3& color);