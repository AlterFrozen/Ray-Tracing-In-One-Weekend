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
		//}

		//BVH
		{
			std::vector<std::shared_ptr<Object>> scene;
			const glm::vec3 defaultColor(0.5, 0.5, 0.0);
			scene.emplace_back(std::make_shared<Sphere>(glm::vec3(0.5, 0.0, 0.0), 0.5, defaultColor, std::make_shared<Lambertian>(glm::vec3(0.1, 0.2, 0.5))));
			scene.emplace_back(std::make_shared<Sphere>(glm::vec3(-0.5, 0.0, 0.0), 0.5, defaultColor, std::make_shared<Lambertian>(glm::vec3(0.1, 0.2, 0.5))));
			std::cout << scene.size() << std::endl;
			BVH bvh{ scene };

			auto* root = bvh.root.get();
			std::cout << "RRRR >> " << (root == nullptr) << '\n';
			print(allnodes.size());
			int nodeCNT = 0;
			auto dfs = [&](BVH::Node* node, auto&& _dfs)->void
			{
				if (!node) return;
				std::cout << ++nodeCNT << '\n';
				if (node->bounding->host != nullptr) std::cout << "Leave\n";

				_dfs(node->left, _dfs);
				_dfs(node->right, _dfs);
			};
			dfs(root, dfs);
			print("hi");
			for (auto* node : allnodes) if (node->isLeave()) std::cout << "CCLeave\n";
		}
	}

	template<typename type>
	static void print(type info)
	{
		std::cout << "[Print] >> " << info << std::endl;
	}
};
