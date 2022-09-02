//STD
#include <iostream>
#include <exception>
#include <fstream>
#include <string>
#include <vector>
#include <cstdlib>
//3rd Party
#include "glm/glm.hpp"
//My Libs
#include "picture.hpp"
#include "ray.hpp"
#include "objects.hpp"

class RayTracing
{
public:
	void execute()
	{
		//pic.defaultPicture();
		int width = pic.width();
		int height = pic.height();

		//Scene
		initScene();

		//Sampling
		for (int row = height - 1; row >= 0; --row)
		{
			for (int col = 0; col < width; ++col)
			{
				float offset_x = static_cast<float>(col) / static_cast<float>(width);
				float offset_y = static_cast<float>(row) / static_cast<float>(height);

				//MSAA
				Ray ray(origin, start_point + offset_x * horizontal + offset_y * vertical);
				
				pic.write(height - (row + 1), col, calculateColor(ray));
			}
		}// end traverse picture
	}

private: //helper
	void initScene()
	{
		//Spheres
		this->scene.emplace_back(std::make_shared<Sphere>(glm::vec3(0.0, 0.0, -1.0), 0.5, glm::vec3(1.0, 0.0, 0.0)));
		this->scene.emplace_back(std::make_shared<Sphere>(glm::vec3(0.0, -51.5, -1.0), 50.0, glm::vec3(0.0, 0.7, 0.0)));
	}

	glm::vec3 calculateColor(Ray& ray)
	{
		bool intersected = false;
		for (auto& object : this->scene)
		{
			intersected |= object->intersectionTest(ray, 0.0f, std::numeric_limits<float>::max());
		}
		if (intersected) return (ray.hitInfo().hit_point_normal + 1.0f) * 0.5f;//object->color;

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