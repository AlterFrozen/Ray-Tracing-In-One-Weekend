#pragma once

#include <memory>

#include "glm/glm.hpp"

#include "noise.hpp"

class Texture
{
public:
	virtual glm::vec3 value(float u, float v, const glm::vec3& p) const = 0;
};

class ConstantTexture
	:public Texture
{
public:
	ConstantTexture() = default;
	ConstantTexture(glm::vec3 color) : color{ color } {};

	virtual glm::vec3 value(float u, float v, const glm::vec3& p) const { return this->color; }
public:
	glm::vec3 color;
};

class CheckerTexture
	:public Texture
{
public:
	CheckerTexture() = default;
	CheckerTexture(std::shared_ptr<Texture> textureA, std::shared_ptr<Texture> textureB, unsigned int frequency = 10)
		:textureA{ textureA }, textureB{ textureB }, frequency{ frequency } {};

	virtual glm::vec3 value(float u, float v, const glm::vec3& p) const;
private:
	unsigned int frequency = 10;
	std::shared_ptr<Texture> textureA;
	std::shared_ptr<Texture> textureB;
};

class NoiseTexture
	:public Texture
{
public:
	NoiseTexture() = default;
	virtual glm::vec3 value(float u, float v, const glm::vec3& p) const;

private:
	Perlin noise_perlin;
};