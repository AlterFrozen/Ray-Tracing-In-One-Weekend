#include "noise.hpp"

//Perlin
bool Perlin::hasInitialized = false;
std::vector<float> Perlin::randFloat(256);
std::vector<glm::vec3> Perlin::randFloatVec(256);
std::vector<int> Perlin::perm_x(256);
std::vector<int> Perlin::perm_y(256);
std::vector<int> Perlin::perm_z(256);

Perlin::Perlin(float frequency/* = 1.0f*/)
	:frequency{ frequency }
{
	if (Perlin::hasInitialized) return;
	else Perlin::hasInitialized = true;

	auto randfMOneToOne = UniformFloatRandomer(-1.0f, 1.0f);

	for (int i = 0; i < Perlin::randFloat.size(); ++i)
	{
		perm_x[i] = perm_y[i] = perm_z[i] = i;
		randFloat[i] = randfZeroToOne();
		randFloatVec[i] = glm::normalize(glm::vec3(randfMOneToOne(), randfMOneToOne(), randfMOneToOne()));
		// std::cout << randFloatVec[i].x << ' ' << randFloatVec[i].y << ' ' << randFloatVec[i].z << '\n';
	}

	//x
	for (int i = Perlin::randFloat.size() - 1; i > 0; --i)
	{
		int target = static_cast<int>(randfZeroToOne() * (i + 1));
		std::swap(perm_x[i], perm_x[target]);
	}

	//y
	for (int i = Perlin::randFloat.size() - 1; i > 0; --i)
	{
		int target = static_cast<int>(randfZeroToOne() * (i + 1));
		std::swap(perm_y[i], perm_y[target]);
	}

	//z
	for (int i = Perlin::randFloat.size() - 1; i > 0; --i)
	{
		int target = static_cast<int>(randfZeroToOne() * (i + 1));
		std::swap(perm_z[i], perm_z[target]);
	}
}

float Perlin::generate(glm::vec3 p, bool lerp/* = true */) const
{
	p *= this->frequency;
	if (lerp)
	{
		float u = p.x - floor(p.x);
		float v = p.y - floor(p.y);
		float w = p.z - floor(p.z);

		int i = floor(p.x);
		int j = floor(p.y);
		int k = floor(p.z);

		glm::vec3 c[2][2][2]{};
		for (int di = 0; di < 2; ++di)
			for (int dj = 0; dj < 2; ++dj)
				for (int dk = 0; dk < 2; ++dk)
					c[di][dj][dk] = Perlin::randFloatVec[Perlin::perm_x[(i + di) & 255] ^ Perlin::perm_y[(j + dj) & 255] ^ Perlin::perm_z[(k + dk) & 255]];
		return this->triLerp(c, u, v, w);
	}
	else
	{
		int i = static_cast<int>(4 * p.x) & 255;
		int j = static_cast<int>(4 * p.y) & 255;
		int k = static_cast<int>(4 * p.z) & 255;

		return Perlin::randFloat[Perlin::perm_x[i] ^ Perlin::perm_y[j] ^ Perlin::perm_z[k]];
	}
}