//STD
#include <iostream>
#include <exception>
#include <fstream>
#include <string>
#include <vector>
#include <cstdlib>
#include <random>
//3rd Party
#include "glm/glm.hpp"
//My Libs
#include "picture.hpp"
#include "ray.hpp"
#include "objects.hpp"
#include "utils.hpp"
#include "materials.hpp"

class RayTracing
{
public:
	void execute()
	{
		//Init
		//pic.defaultPicture();
		int width = pic.width();
		int height = pic.height();
		static std::default_random_engine defaultRandomEngine;
		static std::uniform_real_distribution<float> ufDistribution(0.0, 1.0 - std::numeric_limits<float>::epsilon());
		//Scene
		initScene();

		//Sampling
		for (int row = height - 1; row >= 0; --row)
		{
			for (int col = 0; col < width; ++col)
			{
				//MSAA
				const int msaa_times = 1000;
				glm::vec3 color(0.0, 0.0, 0.0);
				for (int msaa_iter = 0; msaa_iter < msaa_times; ++msaa_iter)
				{
					float randX = ufDistribution(defaultRandomEngine);
					float randY = ufDistribution(defaultRandomEngine);
					float offset_x = (static_cast<float>(col) + randX) / static_cast<float>(width);
					float offset_y = (static_cast<float>(row) + randY) / static_cast<float>(height);
					
					Ray ray(origin, start_point + offset_x * horizontal + offset_y * vertical);
					color += calculateColor(ray, 0);
				}
				color /= static_cast<float>(msaa_times);

				gamma_correction(color);
				pic.write(height - (row + 1), col, color);
			}
		}// end traverse picture
	}

private: //helper
	void initScene()
	{
		//Spheres
		glm::vec3 defaultColor(0.5, 0.5, 0.0);
		this->scene.emplace_back(std::make_shared<Sphere>(glm::vec3(0.0, 0.0, -1.0), 0.5, defaultColor, std::make_shared<Lambertian>(glm::vec3(0.8, 0.3, 0.3))));
		this->scene.emplace_back(std::make_shared<Sphere>(glm::vec3(0.0, -100.5, -1.0), 100.0, defaultColor, std::make_shared<Lambertian>(glm::vec3(0.8, 0.8, 0.0))));
		this->scene.emplace_back(std::make_shared<Sphere>(glm::vec3(1.0, 0.0, -1.0), 0.5, defaultColor, std::make_shared<Metal>(glm::vec3(0.8, 0.6, 0.2))));
		this->scene.emplace_back(std::make_shared<Sphere>(glm::vec3(-1.0, -0.0, -1.0), 0.5, defaultColor, std::make_shared<Metal>(glm::vec3(0.8, 0.8, 0.8))));
	}

	glm::vec3 calculateColor(Ray& ray, int iterationDepth)
	{
		if (iterationDepth > 50) return glm::vec3(0.0, 0.0, 0.0);

		bool intersected = false;
		for (auto& object : this->scene)
		{
			intersected |= object->intersectionTest(ray, 0.0f, std::numeric_limits<float>::max());
		}

		if (intersected)
		{
			//return (ray.hitInfo().hit_point_normal + 1.0f) * 0.5f;//object->color; // Visualize Normal
			glm::vec3 attenuation;
			Ray ray_scattered{};
			if (ray.hitInfo().hit_object_material.lock()->scatter(ray, ray_scattered, attenuation))
				return attenuation * calculateColor(ray_scattered, iterationDepth + 1);
		}

		// Background
		glm::vec3 normalizedDir = glm::normalize(ray.direction());
		float t = 0.5 * (normalizedDir.y + 1.0); // -1.0 <= range(y) <= 1.0
		return (1.0f - t) * glm::vec3(1.0, 1.0, 1.0) + t * glm::vec3(0.5, 0.7, 1.0);
	}
private:
	Picture pic{ 200, 100, 3, ppm, "practice" };
	std::vector<std::shared_ptr<Object>> scene;

	glm::vec3 start_point{ -2.0, -1.0, -1.0 }; // lower_left_corner
	glm::vec3 horizontal{ 4.0, 0.0, 0.0 };
	glm::vec3 vertical{ 0.0, 2.0, 0.0 };
	glm::vec3 origin{ 0.0, 0.0, 0.0 };

public:
	static RayTracing& instance()
	{
		static RayTracing _instance;
		return _instance;
	}

private:
	RayTracing() = default;
	~RayTracing() { pic.save(); }
};

int main(int argc, char* argv[])
{
	try 
	{
		RayTracing::instance().execute();
	} 
	catch(const std::exception& err)
	{
		std::cerr << err.what() << std::endl;
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}