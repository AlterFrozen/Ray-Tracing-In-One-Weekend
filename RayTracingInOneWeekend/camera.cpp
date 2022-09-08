#include "camera.hpp"

void Camera::shoot(float rr_prob)
{
	this->rr_prob = rr_prob;
	this->rr_recursion_upper = std::ceil(std::log(0.001) / std::log(rr_prob));

	long long totalSamplingNum = _height * _width * msaa_times / 100;
	std::cout << "Total Sampling>> " << totalSamplingNum  * 100 << '\n';
	long long curSamplingNum = 0;
	long long curProgression = 0;
	long long totalProgression = 0;
	for (int row = _height - 1; row >= 0; --row)
	{
		for (int col = 0; col < _width; ++col)
		{
			//MSAA
			glm::vec3 color(0.0, 0.0, 0.0);
			for (int msaa_iter = 0; msaa_iter < this->msaa_times; ++msaa_iter)
			{
				float randX = this->ufDistribution(defaultRandomEngine);
				float randY = ufDistribution(defaultRandomEngine);
				float offset_x = (static_cast<float>(col) + randX) / static_cast<float>(_width);
				float offset_y = (static_cast<float>(row) + randY) / static_cast<float>(_height);

				Ray ray;
				if (this->aperture_radius <= 0.0f) ray = Ray(origin, start_point + offset_x * horizontal + offset_y * vertical - origin);
				else
				{
					glm::vec3 randomRadius = random_in_unit_disk();
					glm::vec3 offset = this->right * randomRadius.x + this->cameraUp * randomRadius.y;
					ray = Ray(origin + offset, start_point + offset_x * horizontal + offset_y * vertical - origin - offset);
				}
				color += calculateColor(ray, 0);

				curSamplingNum++;
				curProgression = curSamplingNum / totalSamplingNum;
				if (curProgression - totalProgression >= 10)
				{
					totalProgression = curProgression;
					std::cout << "Progressing>> " << totalProgression << std::endl;
				}
			}
			color /= static_cast<float>(this->msaa_times);
			//color /= rr_prob;
			
			gamma_correction(color);
			this->write(_height - (row + 1), col, color);
		}
	}// end traverse scene
}

void Camera::setMSAA(uint32_t rate)
{
	this->msaa_times = rate;
}

void Camera::setView(glm::vec3 position, glm::vec3 lookAt, glm::vec3 worldUp, float vFov /*= -1.0*/)
{
	this->_fov_vertical = vFov > 0.0f ? vFov : this->_fov_vertical;
	float theta = glm::radians<float>(this->_fov_vertical);
	float half_height = tan(theta / 2.0);
	float half_width = _aspect * half_height;

	this->origin = position;
	this->lookAt = lookAt;
	this->worldUp = worldUp;
	this->rayDir = glm::normalize(position - lookAt);
	this->right = glm::normalize(glm::cross(worldUp, rayDir));
	this->cameraUp = cross(rayDir, right);
	this->start_point = glm::vec3(-half_width, -half_height, -1.0f);
	this->start_point = this->origin - half_width * focus_dist * right - half_height * focus_dist * cameraUp - focus_dist * rayDir;
	this->horizontal = 2 * half_width * focus_dist * right;
	this->vertical = 2 * half_height * focus_dist * cameraUp;
}

glm::vec3 Camera::calculateColor(Ray& ray, unsigned int iter_depth)
{
	if (iter_depth > this->rr_recursion_upper) return glm::vec3(0.0, 0.0, 0.0);
	bool intersected = false;
	for (auto& object : this->scene)
	{
		intersected |= object->intersectionTest(ray, 0.0f, std::numeric_limits<float>::max());
	}

	if (intersected)
	{ // At least 10 bounces
		if (russianRoulette() > this->rr_prob) return glm::vec3(0.0, 0.0, 0.0);

		//return (ray.hitInfo().hit_point_normal + 1.0f) * 0.5f;//object->color; // Visualize Normal
		glm::vec3 attenuation;
		Ray ray_scattered{};
		if (ray.hitInfo().hit_object_material.lock()->scatter(ray, ray_scattered, attenuation))
		{
			return attenuation * calculateColor(ray_scattered, iter_depth + 1) / this->rr_prob;
		}
	}

	// Background
	glm::vec3 normalizedDir = glm::normalize(ray.direction());
	float t = 0.5 * (normalizedDir.y + 1.0); // -1.0 <= range(y) <= 1.0
	return (1.0f - t) * glm::vec3(1.0, 1.0, 1.0) + t * glm::vec3(0.5, 0.7, 1.0);
}

void Camera::write(int row, int col, const glm::vec3& color)
{
	assert(_channels >= 3 && "The color channels less than three!");
	assert((row >= 0 && row < _height&& col >= 0 && col < _width) && "Illegal position for writing!");
	this->_data[row][col][R] = static_cast<int>(255.99 * color.r);
	this->_data[row][col][G] = static_cast<int>(255.99 * color.g);
	this->_data[row][col][B] = static_cast<int>(255.99 * color.b);
}

void Camera::setLens(float r_aperture, float focus_dist)
{
	assert(r > 0 && "The radius of aperture at least greater than 0!");
	this->aperture_radius = r_aperture;
	this->focus_dist = focus_dist;
	setView(this->origin, this->lookAt, this->worldUp);
}

void Camera::defaultPicture()
{
	assert(_channels >= 3 && "The color channels less than three!");
	if (_format == ppm)
	{
		for (int rcol = _width - 1; rcol >= 0; --rcol)
		{
			for (int row = 0; row < _height; ++row)
			{
				glm::vec3 color{ static_cast<float>(row) / static_cast<float>(_height) ,
											  static_cast<float>(rcol) / static_cast<float>(_width) ,
											  0.2f };
				_data[row][_width - (1 + rcol)][R] = static_cast<int>(255.99 * color[R]);
				_data[row][_width - (1 + rcol)][G] = static_cast<int>(255.99 * color[G]);
				_data[row][_width - (1 + rcol)][B] = static_cast<int>(255.99 * color[B]);
			}
		}
	}
}

void Camera::save()
{
	std::string filePath = _name;
	if (_format == ppm) filePath += ".ppm";

	std::ofstream fout(filePath);
	if (!fout.is_open()) throw std::runtime_error("Failed to save the file!\n");

	fout << "P3\n" << _width << ' ' << _height << '\n' << 255 << '\n'; //P3 means colors are in ASCII
	for (int row = 0; row < _height; ++row)
	{
		for (int col = 0; col < _width; ++col)
		{
			for (int channel = 0; channel < _channels; ++channel)
			{
				fout << _data[row][col][channel] << (channel == _channels - 1 ? '\n' : ' ');
			}
		}
	}
	fout.close();
}

void Camera::save(std::string absPath)
{
	std::string filePath = absPath + '\\' + _name;
	if (_format == ppm) filePath += ".ppm";

	std::ofstream fout(filePath);
	if (!fout.is_open()) throw std::runtime_error("Failed to save the file!\n");

	fout << "P3\n" << _width << ' ' << _height << '\n' << 255 << '\n'; //P3 means colors are in ASCII
	for (int row = 0; row < _height; ++row)
	{
		for (int col = 0; col < _width; ++col)
		{
			for (int channel = 0; channel < _channels; ++channel)
			{
				fout << _data[row][col][channel] << (channel == _channels - 1 ? '\n' : ' ');
			}
		}
	}
	fout.close();
}