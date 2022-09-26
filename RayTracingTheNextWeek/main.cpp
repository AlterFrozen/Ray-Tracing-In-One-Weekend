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
	const int range_x = 400, range_y = 400;
	const float vFov = static_cast<float>(range_x) / static_cast<float>(range_y);
	std::vector<std::shared_ptr<Object>> scene;
	Camera camera{ range_x, range_y, 3, 90, vFov,
									ppm, "result" , scene, Environment::nature };
	Clock::time_point time_execute_start = Clock::now();
public:
	void execute()
	{
		//Scene
		initScene("Ray Tracing The Next Week");
		camera.bindBVH(std::make_shared<BVH>(this->scene)); // make sure that BVH is on if using Instance Proxy(e.g. RotateY)

		//Sampling
		camera.setMSAA(10000);
		camera.setHDR(ToneMapping::ACES);
		camera.shoot(0.95);

		camera.save();
	}

private: //helper
	void initScene(std::string name = "")
	{
		this->scene.clear();

		//Textures
		std::shared_ptr<Texture> blueDream = std::make_shared<ConstantTexture>(glm::vec3{ 0.1, 0.2, 0.5 });
		std::shared_ptr<Texture> grass = std::make_shared<ConstantTexture>(glm::vec3{ 0.8, 0.8, 0.0 });
		std::shared_ptr<Texture> redish = std::make_shared<ConstantTexture>(glm::vec3{ 0.65, 0.05, 0.05 });
		std::shared_ptr<Texture> white = std::make_shared<ConstantTexture>(glm::vec3{ 0.73, 0.73, 0.73 });
		std::shared_ptr<Texture> green = std::make_shared<ConstantTexture>(glm::vec3{ 0.12, 0.45, 0.15 });
		std::shared_ptr<Texture> silver = std::make_shared<ConstantTexture>(glm::vec3{ 0.7, 0.7, 0.7 });
		std::shared_ptr<Texture> sunny = std::make_shared<ConstantTexture>(glm::vec3{ 1.0, 1.0, 1.0 });
		std::shared_ptr<Texture> knight = std::make_shared<CheckerTexture>(silver, redish);
		std::shared_ptr<Texture> bedrock = std::make_shared<Bedrock>(1.0);
		std::shared_ptr<Texture> marble = std::make_shared<NoiseTexture>(1.0);
		std::shared_ptr<Texture> earth = std::make_shared<ImageTexture>(app_path + "/textures/earth.jpg");
		
		if (name.empty()) //default
		{
			std::cout << "[Scene]  Default Scene\n";
			this->scene.emplace_back(std::make_shared <RotateY>(360 , std::make_shared<Sphere>(glm::vec3(0.0, 0.0, -1.0), 0.5,
				std::make_shared<Lambertian>(knight))));
			this->scene.emplace_back(std::make_shared<Sphere>(glm::vec3(0.0, -100.5, -1.0), 100.0,
				std::make_shared<Lambertian>(knight)));
			this->scene.emplace_back(std::make_shared<Sphere>(glm::vec3(1.0, 0.0, -1.0), 0.5, std::make_shared<Metal>(glm::vec3(0.8, 0.6, 0.2))));
			this->scene.emplace_back(std::make_shared<Sphere>(glm::vec3(-1.0, 0.0, -1.0), 0.5, std::make_shared<Dielectric>(1.5f)));
			this->scene.emplace_back(std::make_shared<Sphere>(glm::vec3(-1.0, 0.0, -1.0), -0.45, std::make_shared<Dielectric>(1.5f))); // Hollow
		}
		else if (name == "Cornell Box")
		{
			std::cout << "[Scene] Cornell Box\n";
			camera.setEnvironment(Environment::darkness);

			this->camera.setView({ 278,278,-800 }, { 278,278,0 }, glm::vec3{ 0,1,0 }, 40);
			this->camera.setLens(0.0, 10.0);

			// Ornaments
			this->scene.emplace_back(
				std::make_shared<Translate>(glm::vec3{ 130, 0, 65 },
					std::make_shared <RotateY>(-18,
						std::make_shared<Box>(glm::vec3{ 0,0,0 }, glm::vec3{ 165,165,165 }, 
							std::make_shared<Lambertian>(white)))));
			this->scene.emplace_back(
				std::make_shared<Translate>(glm::vec3{ 265, 0, 295 },
					std::make_shared <RotateY>(15,
						std::make_shared<Box>(glm::vec3{ 0,0,0 }, glm::vec3{ 165,330,165 },
							std::make_shared<Lambertian>(white)))));
			//this->scene.emplace_back(std::make_shared <RotateY>(30, std::make_shared<Box>(glm::vec3{ 265,0,295 }, glm::vec3{ 430,330,460 }, std::make_shared<Lambertian>(white))));

			// Scene
			std::shared_ptr<Texture> light = std::make_shared<ConstantTexture>(glm::vec3{ 15,15,15 });
			this->scene.emplace_back(std::make_shared<RectangleXZ>(213, 343, 227, 332, 554, std::make_shared<DiffuseLight>(light)));

			this->scene.emplace_back(std::make_shared<FlipNormal>(std::make_shared<RectangleYZ>(0, 555, 0, 555, 555, std::make_shared<Lambertian>(green))));
			this->scene.emplace_back(std::make_shared<RectangleYZ>(0, 555, 0, 555, 0, std::make_shared<Lambertian>(redish)));

			this->scene.emplace_back(std::make_shared<RectangleXZ>(0, 555, 0, 555, 0, std::make_shared<Lambertian>(white)));
			this->scene.emplace_back(std::make_shared<FlipNormal>(std::make_shared<RectangleXZ>(0, 555, 0, 555, 555, std::make_shared<Lambertian>(white))));

			this->scene.emplace_back(std::make_shared<FlipNormal>(std::make_shared<RectangleXY>(0, 555, 0, 555, 555, std::make_shared<Lambertian>(white))));
		}
		else if (name == "Smoky Cornell Box")
		{
			std::cout << "[Scene]  Smoky Cornell Box\n";
			camera.setEnvironment(Environment::darkness);

			this->camera.setView({ 278,278,-800 }, { 278,278,0 }, glm::vec3{ 0,1,0 }, 40);
			this->camera.setLens(0.0, 10.0);

			// Ornaments
			this->scene.emplace_back(
				std::make_shared<ConstantMedium>(0.005f, redish,
					std::make_shared<Translate>(glm::vec3{ 130, 0, 65 },
						std::make_shared <RotateY>(-18,
							std::make_shared<Box>(glm::vec3{ 0,0,0 }, glm::vec3{ 165,165,165 }, 
								std::make_shared<Lambertian>(white))))));
			this->scene.emplace_back(
				std::make_shared<ConstantMedium>(0.005f, green,
					std::make_shared<Translate>(glm::vec3{ 265, 0, 295 },
						std::make_shared <RotateY>(15,
							std::make_shared<Box>(glm::vec3{ 0,0,0 }, glm::vec3{ 165,330,165 },
								std::make_shared<Lambertian>(white))))));

			// Scene
			std::shared_ptr<Texture> light = std::make_shared<ConstantTexture>(glm::vec3{ 20,20,20 });
			this->scene.emplace_back(std::make_shared<RectangleXZ>(213, 343, 227, 332, 554, std::make_shared<DiffuseLight>(light)));

			this->scene.emplace_back(std::make_shared<FlipNormal>(std::make_shared<RectangleYZ>(0, 555, 0, 555, 555, std::make_shared<Lambertian>(green))));
			this->scene.emplace_back(std::make_shared<RectangleYZ>(0, 555, 0, 555, 0, std::make_shared<Lambertian>(redish)));

			this->scene.emplace_back(std::make_shared<RectangleXZ>(0, 555, 0, 555, 0, std::make_shared<Lambertian>(white)));
			this->scene.emplace_back(std::make_shared<FlipNormal>(std::make_shared<RectangleXZ>(0, 555, 0, 555, 555, std::make_shared<Lambertian>(white))));

			this->scene.emplace_back(std::make_shared<FlipNormal>(std::make_shared<RectangleXY>(0, 555, 0, 555, 555, std::make_shared<Lambertian>(white))));
		}
		else if (name == "Cornell Box [Hack Version]")
		{
			std::cout << "[Scene]  Cornell Box[beta]\n";
			camera.setEnvironment(Environment::booming);
			this->camera.setView({ 278,278,-800 }, { 278,278,0 }, glm::vec3{ 0,1,0 }, 40);
			this->camera.setLens(0.0, 10.0);

			//Extra
			this->scene.emplace_back(std::make_shared<Sphere>(glm::vec3((153.0 + 403.0) / 2.0, 280, (392.0 + 554.0) / 2.0), 200,
				std::make_shared<Lambertian>(earth)));

			//Cornell Box
			std::shared_ptr<Texture> light = std::make_shared<ConstantTexture>(glm::vec3{ 10, 10, 10 });
			this->scene.emplace_back(std::make_shared<RectangleXZ>(153, 403, 167, 392, 554, std::make_shared<DiffuseLight>(light)));

			this->scene.emplace_back(std::make_shared<Sphere>(glm::vec3(-20000.0, 280, 0.0), 20000, // left
				std::make_shared<Lambertian>(redish)));
			this->scene.emplace_back(std::make_shared<Sphere>(glm::vec3(20555.0, 280, 0.0), 20000, // right
				std::make_shared<Lambertian>(green)));
			this->scene.emplace_back(std::make_shared<Sphere>(glm::vec3(278, 20555, 0.0), 20000, // top
				std::make_shared<Lambertian>(white)));
			this->scene.emplace_back(std::make_shared<Sphere>(glm::vec3(278, -20000, 0.0), 20000, // bottom
				std::make_shared<Lambertian>(white)));
			this->scene.emplace_back(std::make_shared<Sphere>(glm::vec3(278, 0, 20555), 20000, // back
				std::make_shared<Lambertian>(white)));

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
		else if (name == "earth")
		{
			std::cout << "[Scene]  Earth\n";
			camera.setEnvironment(Environment::bright);
			this->scene.emplace_back(std::make_shared<Sphere>(glm::vec3(0.0, 0.0, -1.0), 0.5,
				std::make_shared<Lambertian>(earth)));
		}
		else if (name == "zen")
		{
			std::cout << "[Scene]  Zen\n";
			camera.setEnvironment(Environment::darkness);

			std::shared_ptr<Texture> light = std::make_shared<ConstantTexture>(glm::vec3{ 4.0,4.0,4.0 });

			this->scene.emplace_back(std::make_shared<Sphere>(glm::vec3(0.0, 4, -1.0), 2,
				std::make_shared<DiffuseLight>(light)));
			this->scene.emplace_back(std::make_shared<Sphere>(glm::vec3(0.0, -100.5, -1.0), 100, 
				std::make_shared<Lambertian>(marble)));
			this->scene.emplace_back(std::make_shared<Sphere>(glm::vec3(0.0, 0, -1.0), 0.5,
				std::make_shared<Lambertian>(marble)));	
		}
		else if (name == "Ray Tracing In One Weekend")
		{
			std::cout << "[Scene] Ray Tracing In One Weekend\n";
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
		}// end texture book A
		else if (name == "Ray Tracing The Next Week")
		{
			std::cout << "[Scene] Ray Tracing The Next Week\n";
			camera.setEnvironment(Environment::darkness);
			this->camera.setView({ 478,278,-600 }, { 278,278,0 }, { 0,1,0 }, 40);
			this->camera.setLens(0.0, 10.0);

			const int sqrtGroundSize = 20;
			const int ballDensity = 1000;

			auto ground = std::make_shared< Lambertian>(std::make_shared<ConstantTexture>(glm::vec3{ 0.48, 0.83, 0.53 }));
			auto elf = std::make_shared <DiffuseLight>(blueDream);
			auto foam = std::make_shared< Lambertian>(white);
			std::shared_ptr<Texture> light = std::make_shared<ConstantTexture>(glm::vec3{ 10,10,10 });
		

			for (int i =0 ; i < sqrtGroundSize; ++i)
				for (int j = 0; j < sqrtGroundSize; ++j)
				{
					float w = 100.0f;
					float x0 = -1000.0f + i * w;
					float z0 = -1000.0f + j * w;
					float y0 = 0.0f;
					float x1 = x0 + w;
					float y1 = 100 * (randfZeroToOne() + 0.01);
					float z1 = z0 + w;
					this->scene.emplace_back(std::make_shared<Box>(glm::vec3{ x0,y0,z0 }, glm::vec3{ x1,y1,z1 }, ground));

					/*this->scene.emplace_back(
						std::make_shared<ConstantMedium>(0.05, blueDream,
								std::make_shared<Sphere>(glm::vec3(x1, y1 + 100 + 50 * randfMOneToOne(), z1), 5.0f, elf)));*/
				}

			this->scene.emplace_back(std::make_shared<RectangleXZ>(123, 423, 147, 412, 554, std::make_shared<DiffuseLight>(light)));
			//Middle Dielectric
			this->scene.emplace_back(std::make_shared<Sphere>(glm::vec3(200, 170, 45), 60, std::make_shared<Lambertian>(knight)));
			this->scene.emplace_back(std::make_shared<Sphere>(glm::vec3(0, 150, 145), 50, std::make_shared<Metal>(glm::vec3(0.8, 0.8, 0.9))));

			auto boundary = std::make_shared<Sphere>(glm::vec3(360, 150, 145), 70, std::make_shared<Dielectric>(10.5));
			this->scene.emplace_back(boundary);
			//this->scene.emplace_back(std::make_shared<ConstantMedium>(0.1, 
			//	std::make_shared<ConstantTexture>(glm::vec3{ 0.2, 0.4, 0.9 }), boundary));
			boundary = std::make_shared<Sphere>(glm::vec3(0, 0, 0), 5000, std::make_shared<Dielectric>(1.5));
			this->scene.emplace_back(std::make_shared<ConstantMedium>(0.0001, sunny, boundary));
			
			this->scene.emplace_back(std::make_shared<Sphere>(glm::vec3(400,200,400), 100,
				std::make_shared<Lambertian>(earth)));
			this->scene.emplace_back(std::make_shared<Sphere>(glm::vec3(220, 280, 300), 80,
				std::make_shared<Lambertian>(std::make_shared<NoiseTexture>(0.1))));

			for (int i = 0; i < ballDensity; ++i)
			{
				this->scene.emplace_back(
						std::make_shared<Translate>(glm::vec3{ -100, 270, 395 },
							std::make_shared <RotateY>(15,
								std::make_shared<Sphere>(glm::vec3{ 165*randfZeroToOne(), 165 * randfZeroToOne() ,165 * randfZeroToOne() },
									10, foam))));
			}		
		}
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
