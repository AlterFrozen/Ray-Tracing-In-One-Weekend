#pragma once

#include <vector>
#include "utils.hpp"

namespace
{
	auto randfZeroToOne = UniformFloatRandomer(0.0f, 1.0f);
}

class Perlin
{
public:
	Perlin(float frequency = 1.0f);

	float generate(glm::vec3 p, bool lerp = true) const;
private:
	inline float triLerp(glm::vec3 c[2][2][2], float u, float v, float w) const
	{
		// smooth Mach Band via Hermite Cubic
		u = u * u * (3 - 2 * u);
		v = v * v * (3 - 2 * v);
		w = w * w * (3 - 2 * w);

		float acc = 0;
		for (int i = 0; i < 2; i++)
			for (int j = 0; j < 2; j++)
				for (int k = 0; k < 2; k++)
				{
					glm::vec3 weight_v(u - i, v - j, w - k);
					acc += ((i * u + (1 - i) * (1 - u)) * (j * v + (1 - j) * (1 - v)) * (k * w + (1 - k) * (1 - w))) * (glm::dot(c[i][j][k], weight_v));
				}
					
		return acc;
	}
public:
	static bool hasInitialized;

	float frequency = 1.0f;
	static std::vector<float> randFloat;
	static std::vector<glm::vec3> randFloatVec;
	static std::vector<int> perm_x;
	static std::vector<int> perm_y;
	static std::vector<int> perm_z;
};