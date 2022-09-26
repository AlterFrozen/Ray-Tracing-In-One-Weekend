#include "objects.hpp"

bool Sphere::intersectionTest(Ray& ray, float t_min, float t_max)
{
	if (t_min > t_max) throw std::runtime_error("intersectionTest>> t_min is greater than t_max!");

	glm::vec3 oc = ray.origin() - center;
	float a = glm::dot(ray.direction(), ray.direction());
	float b = 2.0 * glm::dot(oc, ray.direction());
	float c = glm::dot(oc, oc) - radius * radius;
	float discriminant = (b * b) - (4 * a * c);

	bool isVaildIntersection = false;
	if (discriminant > 0.0f)
	{
		float temp = (-b - sqrt(discriminant)) / (2.0 * a); // first root
		if (t_min < temp && temp < t_max) isVaildIntersection = true;
		else {
			temp = (-b + sqrt(discriminant)) / (2.0 * a); // second root
			if (t_min < temp && temp < t_max) isVaildIntersection = true;
		}

		if (isVaildIntersection && temp < ray.hitInfo().hit_time_first) // Note that: temp may equal to ray's hit_time_first
		{
			ray.hitInfo().hit_time_first = temp;
			ray.hitInfo().hit_point = ray.cast(ray.hitInfo().hit_time_first);
			ray.hitInfo().hit_point_normal = (ray.hitInfo().hit_point - this->center) / this->radius;
			ray.hitInfo().hit_object_material = this->material;
			getUV(ray.hitInfo().hit_point_normal, ray.hitInfo().hit_point_uv[0], ray.hitInfo().hit_point_uv[1]);
			//ray.hitInfo().path.push_back(this->material);
		}
	}
	return isVaildIntersection;
}


bool Sphere::bindAABB(AABB* aabb)
{
	aabb->info.p_min = this->center - glm::vec3{this->radius, this->radius ,this->radius};
	aabb->info.p_max = this->center + glm::vec3{ this->radius, this->radius ,this->radius };
	return true;
}

bool RectangleXY::intersectionTest(Ray& ray, float t_min, float t_max) 
{
	float t = (k - ray.origin().z) / ray.direction().z;
	if (t < t_min || t > t_max) return false;

	float x = ray.origin().x + t * ray.direction().x;
	float y = ray.origin().y + t * ray.direction().y;
	if (x < x0 || x  >x1 || y  < y0 || y  >y1) return false;

	if (t < ray.hitInfo().hit_time_first)
	{
		ray.hitInfo().hit_time_first = t;
		ray.hitInfo().hit_point = ray.cast(t);
		ray.hitInfo().hit_point_uv[0] = (x - x0) / (x1 - x0);
		ray.hitInfo().hit_point_uv[1] = (y - y0) / (y1 - y0);
		ray.hitInfo().hit_object_material = this->material;
		ray.hitInfo().hit_point_normal = glm::vec3(0, 0, 1);
	}
	return true;
}

bool RectangleXY::bindAABB(AABB* aabb)
{
	aabb->info.p_min = glm::vec3{ x0, y0, k - 0.0001 };
	aabb->info.p_max = glm::vec3{ x1,y1, k + 0.0001 };
	return true;
}

bool RectangleXZ::intersectionTest(Ray& ray, float t_min, float t_max)
{
	float t = (k - ray.origin().y) / ray.direction().y;
	if (t < t_min || t> t_max) return false;

	float x = ray.origin().x + t * ray.direction().x;
	float z = ray.origin().z + t * ray.direction().z;
	if (x < x0 || x > x1 || z <z0 || z > z1) return false;

	if (t < ray.hitInfo().hit_time_first)
	{
		ray.hitInfo().hit_time_first = t;
		ray.hitInfo().hit_point = ray.cast(t);
		ray.hitInfo().hit_point_uv[0] = (x - x0) / (x1 - x0);
		ray.hitInfo().hit_point_uv[1] = (z - z0) / (z1 - z0);
		ray.hitInfo().hit_object_material = this->material;
		ray.hitInfo().hit_point_normal = glm::vec3(0, 1, 0);
	}
	return true;
}

bool RectangleXZ::bindAABB(AABB* aabb)
{
	aabb->info.p_min = glm::vec3{ x0, k - 0.0001, z0 };
	aabb->info.p_max = glm::vec3{ x1, k + 0.0001, z1 };
	return true;
}

bool RectangleYZ::intersectionTest(Ray& ray, float t_min, float t_max)
{
	float t = (k - ray.origin().x) / ray.direction().x;
	if (t < t_min || t > t_max) return false;
	
	float y = ray.origin().y + t * ray.direction().y;
	float z = ray.origin().z + t * ray.direction().z;
	if (y < y0 || y > y1 || z < z0 || z > z1) return false;
	
	if (t < ray.hitInfo().hit_time_first)
	{
		ray.hitInfo().hit_time_first = t;
		ray.hitInfo().hit_point = ray.cast(t);
		ray.hitInfo().hit_point_uv[0] = (y - y0) / (y1 - y0);
		ray.hitInfo().hit_point_uv[1] = (z - z0) / (z1 - z0);
		ray.hitInfo().hit_object_material = this->material;
		ray.hitInfo().hit_point_normal = glm::vec3(1, 0, 0);
	}
	return true;
}

bool RectangleYZ::bindAABB(AABB* aabb)
{
	aabb->info.p_min = glm::vec3{ k - 0.0001, y0, z0 };
	aabb->info.p_max = glm::vec3{ k + 0.0001, y1, z1 };
	return true;
}

bool FlipNormal::intersectionTest(Ray& ray, float t_min, float t_max)
{
	bool res = this->host->intersectionTest(ray, t_min, t_max);
	ray.hitInfo().hit_point_normal = -ray.hitInfo().hit_point_normal; //Flip
	return res;
}

bool FlipNormal::bindAABB(AABB* aabb)
{
	return this->host->bindAABB(aabb);
}

bool Translate::intersectionTest(Ray& ray, float t_min, float t_max)
{
	Ray movedRay{ ray.origin() - this->offset, ray.direction()};
	if (this->host->intersectionTest(movedRay, t_min, t_max))
	{
		if (movedRay.hitInfo().hit_time_first < ray.hitInfo().hit_time_first)
		{
			ray.swapHitInfo(movedRay.hitInfo());
			ray.hitInfo().hit_point += this->offset;
		}
		return true;
	}
	else return false;
}

bool Translate::bindAABB(AABB* aabb)
{
	if (this->host->bindAABB(aabb))
	{
		aabb->info.p_min += this->offset;
		aabb->info.p_max += this->offset;
		return true;
	}
	else return false;
}

RotateY::RotateY(float angle, std::shared_ptr<Object> host)
	:host{ host }, hasBox{ false }, radians{ glm::radians(angle) }
{
}

bool RotateY::intersectionTest(Ray& ray, float t_min, float t_max)
{
	glm::vec3 origin = ray.origin();
	glm::vec3 direction = ray.direction();
	origin[0] = this->cos_theta * ray.origin().x - this->sin_theta * ray.origin().z;
	origin[2] = this->sin_theta * ray.origin().x + this->cos_theta * ray.origin().z;
	direction[0] = this->cos_theta * ray.direction().x - this->sin_theta * ray.direction().z;
	direction[2] = this->sin_theta * ray.direction().x + this->cos_theta * ray.direction().z;

	Ray rotatedRay{ origin, direction };

	if (this->host->intersectionTest(rotatedRay, t_min, t_max))
	{
		if (rotatedRay.hitInfo().hit_time_first < ray.hitInfo().hit_time_first)
		{
			ray.swapHitInfo(rotatedRay.hitInfo());
			glm::vec3 p = ray.hitInfo().hit_point;
			glm::vec3 normal = ray.hitInfo().hit_point_normal;
			p[0] = this->cos_theta * ray.hitInfo().hit_point.x + this->sin_theta * ray.hitInfo().hit_point.z;
			p[2] = -this->sin_theta * ray.hitInfo().hit_point.x + this->cos_theta * ray.hitInfo().hit_point.z;
			normal[0] = this->cos_theta * ray.hitInfo().hit_point_normal.x + this->sin_theta * ray.hitInfo().hit_point_normal.z;
			normal[2] = -this->sin_theta * ray.hitInfo().hit_point_normal.x + this->cos_theta * ray.hitInfo().hit_point_normal.z;
			ray.hitInfo().hit_point = p;
			ray.hitInfo().hit_point_normal = normal;
		}
		return true;
	}
	else return false;
}

bool RotateY::bindAABB(AABB* aabb)
{
	this->hasBox = this->host->bindAABB(aabb);

	this->sin_theta = glm::sin(radians);
	this->cos_theta = glm::cos(radians);
	const auto fMax = std::numeric_limits<float>::max();
	const auto fMin = std::numeric_limits<float>::min();
	glm::vec3 minV{ fMax ,fMax ,fMax };
	glm::vec3 maxV{ fMin ,fMin ,fMin };
	for (int i = 0; i < 2; ++i)
		for (int j = 0; j < 2; ++j)
			for (int k = 0; k < 2; ++k)
			{
				float x = i * aabb->maxPoint().x + (1 - i) * aabb->minPoint().x;
				float y = j * aabb->maxPoint().y + (1 - j) * aabb->minPoint().y;
				float z = k * aabb->maxPoint().z + (1 - k) * aabb->minPoint().z;
				float newX = this->cos_theta * x + this->sin_theta * z;
				float newZ = -this->sin_theta * x + this->cos_theta * z;
				glm::vec3 temp{ newX, y, newZ };
				for (int c = 0; c < 3; ++c)
				{
					minV[c] = std::min(temp[c], minV[c]);
					maxV[c] = std::max(temp[c], maxV[c]);
				}
			}
	aabb->info.p_min = minV;
	aabb->info.p_max = maxV;

	return this->hasBox;
}

Box::Box(glm::vec3 p_min, glm::vec3 p_max, std::shared_ptr<Material> material_info)
	:p_min{ p_min }, p_max{ p_max }, Object{ material_info } 
{
	this->faces.reserve(6);
	this->faces.emplace_back(std::make_shared<RectangleXY>(p_min.x, p_max.x, p_min.y, p_max.y, p_max.z, this->material));
	this->faces.emplace_back(std::make_shared <FlipNormal>(std::make_shared<RectangleXY>(p_min.x, p_max.x, p_min.y, p_max.y, p_min.z, this->material)));

	this->faces.emplace_back(std::make_shared<RectangleXZ>(p_min.x, p_max.x, p_min.z, p_max.z, p_max.y, this->material)); 
	this->faces.emplace_back(std::make_shared <FlipNormal>(std::make_shared<RectangleXZ>(p_min.x, p_max.x, p_min.z, p_max.z, p_min.y, this->material)));// Note that the standard direction of face is different from Cornell Box

	this->faces.emplace_back(std::make_shared<RectangleYZ>(p_min.y, p_max.y, p_min.z, p_max.z, p_max.x, this->material));
	this->faces.emplace_back(std::make_shared <FlipNormal>(std::make_shared<RectangleYZ>(p_min.y, p_max.y, p_min.z, p_max.z, p_min.x, this->material)));
}

bool Box::intersectionTest(Ray& ray, float t_min, float t_max)
{
	bool res = false;
	for (auto& face : this->faces) res |= face->intersectionTest(ray, t_min, t_max);
	return res;
}

bool Box::bindAABB(AABB* aabb)
{
	aabb->info.p_min = this->p_min;
	aabb->info.p_max = this->p_max;
	return true;
}

bool ConstantMedium::intersectionTest(Ray& ray, float t_min, float t_max)
{
	static auto randfZeroToOne = UniformFloatRandomer(0.0f, 1.0f);
	bool db = (randfZeroToOne() < 0.00001);
	db = false;
	Ray probeA{ ray }, probeB{ ray };
	if (this->host->intersectionTest(probeA, std::numeric_limits<float>::min(), std::numeric_limits<float>::max()))
	{
		if (this->host->intersectionTest(probeB, probeA.hitInfo().hit_time_first + 0.001, std::numeric_limits<float>::max()))
		{
			if (db) std::cerr << "\n (t0 , t1) = " << probeA.hitInfo().hit_time_first << " , " << probeB.hitInfo().hit_time_first << std::endl;;
			probeA.hitInfo().hit_time_first = std::min(t_min, probeA.hitInfo().hit_time_first);
			probeB.hitInfo().hit_time_first = std::max(t_max, probeB.hitInfo().hit_time_first);
			if (probeA.hitInfo().hit_time_first > probeB.hitInfo().hit_time_first) return false;
			
			probeA.hitInfo().hit_time_first = probeA.hitInfo().hit_time_first < 0 ? 0 : probeA.hitInfo().hit_time_first;

			float distance_inside_boundary = (probeB.hitInfo().hit_time_first - probeA.hitInfo().hit_time_first) * ray.direction().length();
			float hit_distance = -(1.0 / this->density) * glm::log(randfZeroToOne());
			if (hit_distance < distance_inside_boundary)
			{
				float paticipatedTime = probeA.hitInfo().hit_time_first + (hit_distance / ray.direction().length());
				if (paticipatedTime < ray.hitInfo().hit_time_first)
				{
					//if (db) std::cerr << "hit_distance = " << hit_distance << std::endl;
					ray.hitInfo().hit_time_first = paticipatedTime;
					//if (db) std::cerr << "ray.first_time_hit = " << ray.hitInfo().hit_time_first << std::endl;
					ray.hitInfo().hit_point = ray.cast(ray.hitInfo().hit_time_first);
					//if (db) std::cerr << "ray.hit_point = " << ray.hi
					ray.hitInfo().hit_point_normal = glm::normalize(glm::vec3{ randfZeroToOne(),randfZeroToOne(),randfZeroToOne() }); // arbitrary
					ray.hitInfo().hit_object_material = this->material;
				}
				return true;
			}
		}
	}
	return false;
}

bool ConstantMedium::bindAABB(AABB* aabb)
{
	return this->host->bindAABB(aabb);
}