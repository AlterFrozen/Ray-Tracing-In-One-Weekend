#pragma once

#include "glm/glm.hpp"

class Ray
{
public:
	struct HitInfo
	{
		float hit_time_first = std::numeric_limits<float>::max();
		glm::vec3 hit_point;
		glm::vec3 hit_point_normal;
	};

	Ray() = delete;
	Ray(const glm::vec3& origin, const glm::vec3& direction) :_origin{ origin }, _direction{ direction }, _hitInfo{} {};

	glm::vec3 cast(float t) const { return _origin + t * _direction; }

	glm::vec3 origin() const { return this->_origin; }
	glm::vec3 direction() const { return this->_direction; }
	HitInfo& hitInfo() { return this->_hitInfo; }
private:
	glm::vec3 _origin;
	glm::vec3 _direction;
	HitInfo _hitInfo;
};