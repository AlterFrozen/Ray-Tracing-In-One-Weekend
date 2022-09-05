#pragma once
//STD
#include <string>
#include <vector>
#include <memory>
#include <random>
#include <fstream>
#include <cassert>
#include <iostream>
//3rd Party
#include "glm/glm.hpp"
#include "glm/gtc/constants.hpp"

#include "ray.hpp"
#include "objects.hpp"

typedef enum { ppm } PictureFormats;
typedef enum { R, G, B, A } ColorChannels;

class Camera
{
public:
#ifdef PIC_PRECISION_FLOAT
	using precision = float;
#else 
	using precision = int;
#endif
	using PictureLayout = std::vector< std::vector< std::vector<precision>>>; // height / width / channels

	Camera() = delete;
	Camera(int width, int height, int channels, float vFOV_degree, float aspect, 
					PictureFormats format, std::string name,
					std::vector<std::shared_ptr<Object>>& scene)
		: _width{ width }, _height{ height }, _channels{ channels }, _fov_vertical{ vFOV_degree }, _aspect{aspect},
		_format{ format }, _name{ name },
		 _data(  height, std::vector<std::vector<precision>>(  width, std::vector<precision>(channels)  )  ),
		ufDistribution(0.0, 0.999999f),
		scene{ scene }
	{
		this->setView(glm::vec3(0, 0, 0), glm::vec3(0, 0, -1), glm::vec3(0, 1, 0));
	};

public: // Interfaces
	int height() const { return _height; }
	int width() const { return _width; }

	void shoot(float rr_prob);
	void setLens(float r_aperture, float focus_dist);
	void setMSAA(uint32_t rate);
	void setView(glm::vec3 position, glm::vec3 lookAt, glm::vec3 worldUp, float vFov = -1.0);
	void write(int row, int col, const glm::vec3& color);

	void defaultPicture();

	void save();
	void save(std::string absPath);
private:
	glm::vec3 calculateColor(Ray& ray, unsigned int iter_depth);

private:
	std::string _name;
	std::vector<std::shared_ptr<Object>>& scene;
	int _width, _height;
	float _fov_vertical, _aspect;
	int _channels;
	PictureFormats _format = ppm;
	PictureLayout _data;
	uint32_t msaa_times = 1;
	float aperture_radius = -1.0;
	float focus_dist = 1.0f;
	//Sampler Range
	float rr_prob;
	glm::vec3 rayDir;
	glm::vec3 right;
	glm::vec3 cameraUp;
	glm::vec3 lookAt;
	glm::vec3 worldUp;
	glm::vec3 start_point{ -2.0, -1.0, -1.0 }; // lower_left_corner
	glm::vec3 horizontal{ 4.0, 0.0, 0.0 };
	glm::vec3 vertical{ 0.0, 2.0, 0.0 };
	glm::vec3 origin{ 0.0, 0.0, 0.0 };
	//Random Generator
	std::default_random_engine defaultRandomEngine;
	std::uniform_real_distribution<float> ufDistribution;
};