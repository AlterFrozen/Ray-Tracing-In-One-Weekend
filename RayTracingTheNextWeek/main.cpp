//#define UNIT_TEST

#ifdef UNIT_TEST
	#include "unit_test.hpp"
#endif

//STD
#include <iostream>
#include <exception>
#include <fstream>
#include <string>
#include <vector>
#include <cstdlib>
#include <random>
#include <format>
#include <chrono>
//3rd Party
#include "glm/glm.hpp"
//My Libs
#include "camera.hpp"
#include "ray.hpp"
#include "objects.hpp"
#include "utils.hpp"
#include "texture.hpp"
#include "materials.hpp"

typedef std::chrono::high_resolution_clock Clock;

class RayTracing
{
private:
	const int range_x = 200, range_y = 100;
	std::vector<std::shared_ptr<Object>> scene;
	Camera camera{ range_x, range_y, 3, 90, static_cast<float>(range_x) / static_cast<float>(range_y), ppm, "result" , scene };
	Clock::time_point time_execute_start = Clock::now();
public:
	void execute()
	{
		//Scene
		initScene("perlin spheres");
		camera.bindBVH(std::make_shared<BVH>(this->scene));

		//Sampling
		camera.setMSAA(100);
		camera.shoot(0.9);
	}

private: //helper
	void initScene(std::string name = "")
	{
		this->scene.clear();
		//Spheres
		const glm::vec3 defaultColor(0.5, 0.5, 0.0);
		std::shared_ptr<Texture> blueDream = std::make_shared<ConstantTexture>(glm::vec3{ 0.1, 0.2, 0.5 });
		std::shared_ptr<Texture> grass = std::make_shared<ConstantTexture>(glm::vec3{ 0.8, 0.8, 0.0 });
		std::shared_ptr<Texture> redish = std::make_shared<ConstantTexture>(glm::vec3{ 0.8, 0.1, 0.0 });
		std::shared_ptr<Texture> silver = std::make_shared<ConstantTexture>(glm::vec3{ 0.7, 0.7, 0.7 });
		std::shared_ptr<Texture> knight = std::make_shared<CheckerTexture>(silver, redish);
		std::shared_ptr<Texture> marble = std::make_shared<NoiseTexture>();
		
		if (name.empty()) //default
		{
			std::cout << "[Scene]  Default Scene\n";
			this->scene.emplace_back(std::make_shared<Sphere>(glm::vec3(0.0, 0.0, -1.0), 0.5, 
				std::make_shared<Lambertian>(knight)));
			this->scene.emplace_back(std::make_shared<Sphere>(glm::vec3(0.0, -100.5, -1.0), 100.0,
				std::make_shared<Lambertian>(knight)));
			this->scene.emplace_back(std::make_shared<Sphere>(glm::vec3(1.0, 0.0, -1.0), 0.5, std::make_shared<Metal>(glm::vec3(0.8, 0.6, 0.2))));
			this->scene.emplace_back(std::make_shared<Sphere>(glm::vec3(-1.0, 0.0, -1.0), 0.5, std::make_shared<Dielectric>(1.5f)));
			//this->scene.emplace_back(std::make_shared<Sphere>(glm::vec3(-1.0, 0.0, -1.0), -0.45, std::make_shared<Dielectric>(1.5f))); // Hollow
		}
		else if (name == "perlin spheres")
		{
			std::cout << "[Scene]  Perlin Spheres\n";
			glm::vec3 lookFrom{ 13,2,3 };
			glm::vec3 lookAt{ 0,0,0 };
			this->camera.setLens(0.0f, 10.0);
			this->camera.setView(lookFrom, lookAt, glm::vec3{ 0,1,0 }, 20);
			this->scene.emplace_back(std::make_shared<Sphere>(glm::vec3(0.0, 2.0, 0.0), 2.0, 
				std::make_shared<Lambertian>(marble)));
			this->scene.emplace_back(std::make_shared<Sphere>(glm::vec3(0.0, -1000.0, 0.0), 1000.0,
				std::make_shared<Lambertian>(marble)));
		}
		else if (name == "textbook cover")
		{
			std::cout << "[Scene] Textbook Cover\n";
			glm::vec3 lookFrom(13, 2, 3);
			glm::vec3 lookAt(0, 0, 0);
			//camera.setLens(2.0, glm::length(lookFrom - lookAt));
			camera.setView(lookFrom, lookAt, glm::vec3(0.0, 1.0, 0.0), 20);

			std::default_random_engine e((Clock::now() - time_execute_start).count());
			std::uniform_real_distribution<float> dist(0.0, 1.0);

			this->scene.emplace_back(std::make_shared<Sphere>(glm::vec3(0.0, -1000, 0.0), 1000, std::make_shared<Lambertian>(knight)));

			for (int a = -11; a < 11; ++a)
			{
				for (int b = -11; b < 11; ++b)
				{
					float choose = dist(e);
					glm::vec3 center(a + 0.9 * dist(e), 0.2, b + 0.9 * dist(e));
					if ((center - glm::vec3(4.0, 2.0, 0.0)).length() > 0.9)
					{
						if (choose < 0.8) // diffuse
						{
							this->scene.emplace_back(std::make_shared<Sphere>(center, 0.2,
								std::make_shared<Lambertian>(std::make_shared<ConstantTexture>(glm::vec3(dist(e) * dist(e), dist(e) * dist(e), dist(e) * dist(e))))));
						}
						else if (choose < 0.95) // metal
						{
							this->scene.emplace_back(std::make_shared<Sphere>(glm::vec3(1.0, 0.0, -1.0), 0.2,
								std::make_shared<Metal>(glm::vec3(0.5 * (1 + dist(e)), 0.5 * (1 + dist(e)), 0.5 * dist(e)))));
						}
						else // glass
						{
							this->scene.emplace_back(std::make_shared<Sphere>(center, 0.2,
								std::make_shared<Dielectric>(1.5f)));
						}
					}
				}
			}
			this->scene.emplace_back(std::make_shared<Sphere>(glm::vec3(-4, 1, 0), 1.0,
				std::make_shared<Lambertian>(blueDream)));
			this->scene.emplace_back(std::make_shared<Sphere>(glm::vec3(4, 1, 0), 1.0,
				std::make_shared<Metal>(glm::vec3(0.7, 0.7, 0.7))));
			this->scene.back().get()->material.get()->fuzz = 0.01;
			this->scene.emplace_back(std::make_shared<Sphere>(glm::vec3(0, 1, 0), 1.0,
				std::make_shared<Dielectric>(1.5f)));
		}// end texture book
		else throw std::runtime_error(std::format("No scene named {} !", name));
	}

public:
	static RayTracing& instance()
	{
		static RayTracing _instance;
		return _instance;
	}

private:
	RayTracing() = default;
	~RayTracing() 
	{ 
		auto duration_s = std::chrono::duration_cast<std::chrono::seconds>(Clock::now() - time_execute_start);
		std::cout << "Run Time>> " << duration_s.count() << " s\n";

		camera.save(); 
	}
};

int main(int argc, char* argv[])
{
	try 
	{
#ifndef UNIT_TEST
		RayTracing::instance().execute();
#else
		UnitTest::execute();
#endif
	} 
	catch(const std::exception& err)
	{
		std::cerr << err.what() << std::endl;
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}
