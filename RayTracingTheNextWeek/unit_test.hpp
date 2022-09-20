#pragma once

#include <exception>
#include <numeric>
#include <iostream>
#include <memory>

#include "glm/glm.hpp"

#include "BVH.hpp"
#include "ray.hpp"
#include "objects.hpp"
#include "utils.hpp"


static struct UnitTest
{
	using vec3 = glm::vec3;

	static void execute()
	{
		//AABB
		//while (true)
		//{
		//	Ray ray(random_in_unit_sphere(), random_in_unit_sphere());
		//	AABB box(glm::vec3{ 0, -0.5, -0.5 }, glm::vec3{ 1, 0.5, 0.5 });
		//	if (box.intersectionTest(ray, 0, std::numeric_limits<float>::max())) break;
		//
	}

	template<typename type>
	static void print(type info)
	{
		std::cout << "[Print] >> " << info << std::endl;
	}
};
