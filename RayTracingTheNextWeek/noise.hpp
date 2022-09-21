#pragma once

#include <vector>
#include <iostream>
#include "utils.hpp"

namespace
{
	auto randfZeroToOne = UniformFloatRandomer(0.0f, 1.0f);
}

class Perlin
{
public:
	Perlin(float frequency = 1.0f);

	float generate(const glm::vec3& po, bool genTurbulence = false, bool lerp = true) const;

private:
	inline float triLerp(glm::vec3 c[2][2][2], float u, float v, float w) const
	{
		// smooth Mach Band via Hermite Cubic
		float uu = u * u * (3 - 2 * u);
		float vv = v * v * (3 - 2 * v);
		float ww = w * w * (3 - 2 * w);

		float acc = 0;
		for (int i = 0; i < 2; i++)
			for (int j = 0; j < 2; j++)
				for (int k = 0; k < 2; k++)
				{
					glm::vec3 weight_v(u - i, v - j, w - k);
					acc += ((i * uu + (1 - i) * (1 - uu)) * (j * vv + (1 - j) * (1 - vv)) * (k * ww + (1 - k) * (1 - ww))) * (glm::dot(c[i][j][k], weight_v));
				}

		return fabs(acc); // textbook:: return acc, however, my result shows some errors.
	}

	inline float turbulence(const glm::vec3& p, int depth = 7) const
	{
		float acc = 0;
		glm::vec3 temp_p = p;
		float weight = 1.0f;
		for (int i = 0; i < depth; ++i)
		{
			acc += weight * this->generate(temp_p);
			weight *= 0.5;
			temp_p *= 2.0;
		}
		return fabs(acc);
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