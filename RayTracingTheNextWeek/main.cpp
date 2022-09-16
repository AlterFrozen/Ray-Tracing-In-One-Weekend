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
		initScene();

		//Sampling
		camera.setMSAA(100);
		camera.shoot(0.9);
	}

private: //helper
	void initScene(std::string name = "")
	{
		this->scene.clear();
		//Spheres
		glm::vec3 defaultColor(0.5, 0.5, 0.0);
		
		if (name.empty()) //default
		{
			std::cout << "[Scene]  Default Scene\n";
			this->scene.emplace_back(std::make_shared<Sphere>(glm::vec3(0.0, 0.0, -1.0), 0.5, defaultColor, std::make_shared<Lambertian>(glm::vec3(0.1, 0.2, 0.5))));
			this->scene.emplace_back(std::make_shared<Sphere>(glm::vec3(0.0, -100.5, -1.0), 100.0, defaultColor, std::make_shared<Lambertian>(glm::vec3(0.8, 0.8, 0.0))));
			this->scene.emplace_back(std::make_shared<Sphere>(glm::vec3(1.0, 0.0, -1.0), 0.5, defaultColor, std::make_shared<Metal>(glm::vec3(0.8, 0.6, 0.2))));
			this->scene.emplace_back(std::make_shared<Sphere>(glm::vec3(-1.0, 0.0, -1.0), 0.5, defaultColor, std::make_shared<Dielectric>(1.5f)));
			this->scene.emplace_back(std::make_shared<Sphere>(glm::vec3(-1.0, 0.0, -1.0), -0.45, defaultColor, std::make_shared<Dielectric>(1.5f))); // Hollow
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

		camera.save(); 
	}
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
