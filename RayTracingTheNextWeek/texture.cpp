#include "texture.hpp"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.hpp"

glm::vec3 CheckerTexture::value(float u, float v, const glm::vec3& p) const
// For spheres, f is proportional to radius
{
	float sines = sin(this->frequency * p.x) * sin(this->frequency * p.y) * sin(this->frequency * p.z);
	if (sines < 0)
		return textureA->value(0, 0, p);
	else 
		return textureB->value(0, 0, p);
}

glm::vec3 NoiseTexture::value(float u, float v, const glm::vec3& p) const
{
	return glm::vec3(1.0f, 1.0f, 1.0f) * (0.5f * (1.0f + sin(this->frequency * p.z + 10.0f * this->noise_perlin.generate(p, true))));
}

glm::vec3 Bedrock::value(float u, float v, const glm::vec3& p) const
{
	return glm::vec3(1.0f, 1.0f, 1.0f) * this->noise_perlin.generate(p, false, false);
}

ImageTexture::ImageTexture(const std::string& image_path)
	:nx{ 0 }, ny{ 0 }, nc{ 0 }
{
	this->data = stbi_load(image_path.c_str(), &(this->nx), &(this->ny), &(this->nc), 0);
	//std::cout << "load image>> " << image_path << ' ' << this->nx << ' ' << this->ny << ' ' << this->nc << '\n';
}

glm::vec3 ImageTexture::value(float u, float v, const glm::vec3& p) const
{
	assert((0.0f <= u && u <= 1.0f && 0.0f <= v && v <= 1.0f) && "UV range error");
	int i = u * this->nx;
	int j = (1 - v) * this->ny - 0.001;
	i = std::clamp(i, 0, this->nx - 1);
	j = std::clamp(j, 0, this->ny - 1);
	//std::cout << u << '+' << v << '-' << i << ' ' << j << '\n';
	glm::vec3 color{};
	color[0] = static_cast<int>(this->data[3 * i + 3 * this->nx * j + 0]) / 255.0f;
	color[1] = static_cast<int>(this->data[3 * i + 3 * this->nx * j + 1]) / 255.0f;
	color[2] = static_cast<int>(this->data[3 * i + 3 * this->nx * j + 2]) / 255.0f;
	//std::cout << color.r << ' ' << color.g << ' ' << color.b << '\n';
	return color;
}