#include "texture.hpp"

glm::vec3 CheckerTexture::value(float u, float v, const glm::vec3& p) const
// For spheres, f is proportional to radius
{
	float sines = sin(this->frequency * p.x) * sin(this->frequency * p.y) * sin(this->frequency * p.z);
	if (sines < 0)
		return textureA->value(u, v, p);
	else 
		return textureB->value(u, v, p);
}

glm::vec3 NoiseTexture::value(float u, float v, const glm::vec3& p) const
{
	return glm::vec3(1, 1, 1) * this->noise_perlin.generate(p);
}