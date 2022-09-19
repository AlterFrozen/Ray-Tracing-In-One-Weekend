#pragma once

#include <cmath>
#include <memory>
#include <random>
#include <iostream>

#include "glm/glm.hpp"
#include "ray.hpp"
#include "objects.hpp"

class Object;
class Bounding
{
public:
	virtual bool intersectionTest(Ray& ray, float t_min, float t_max) const = 0;

	Object* host = nullptr;
};

class AABB
	:public Bounding
{
public:
	struct Info
	{
		glm::vec3 p_min;
		glm::vec3 p_max;
	};
	AABB() = default;

	const glm::vec3& minPoint() const { return this->info.p_min; }
	const glm::vec3& maxPoint() const { return this->info.p_max; }

	virtual bool intersectionTest(Ray& ray, float t_min, float t_max) const;

	static AABB combineAABB(const AABB& boxA, const AABB& boxB);

public:
	Info info;
};

class BVH
{
public:
	struct Node
		:public Bounding
	{
		friend BVH;
		Node() = delete;
		Node(BVH& tree, size_t vergeL, size_t vergeR); // [vergeL, vergeR)

		virtual bool intersectionTest(Ray& ray, float t_min, float t_max) const;

		bool isLeave() const { return (this->bounding->host != nullptr); }

		std::shared_ptr<AABB> bounding; // default AABB
		BVH::Node* left;
		BVH::Node* right;
	};
public:
	BVH() = delete;
	BVH(std::vector<std::shared_ptr<Object>>& objects) :data{ objects } 
	{
		std::cout << "Building Up>> BVH\n";
		root = std::make_unique<Node>(*this, 0, data.size());
	};

	std::unique_ptr<Node> root;

private:
	std::vector<std::shared_ptr<Object>>& data;
};

extern std::vector< BVH::Node* > allnodes;