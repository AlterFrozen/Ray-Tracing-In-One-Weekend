#include "BVH.hpp"
#include <iostream>

std::vector< BVH::Node* > allnodes;

bool AABB::intersectionTest(Ray& ray, float t_min, float t_max) const
{
	/*
	* Ray function: ray = origin + dir * time;
	* Plane funciton of the AABB: plane = linear combination of x,y,z; (Attribution of AA)
	* ==> Intersection Test Funciton: HitPoint[x/y/z] = origin_r[x/y/z] + dir_r[x/y/z] * time_hit[x/y/z]
	* i.e. time_hit[x/y/z]	 = (HitPoint[x/y/z] - origin_r[x/y/z]) / dir_r[x/y/z]
	*/

	for (int axis = 0; axis < 3; ++axis)
	{
		float invD = 1.0f / ray.direction()[axis];
		float t0 = (this->info.p_min[axis] - ray.origin()[axis]) * invD;
		float t1 = (this->info.p_max[axis] - ray.origin()[axis])* invD;

		if (invD < 0.0f) std::swap(t0, t1);

		t_min = std::max(t_min, t0);
		t_max = std::min(t_max, t1);

		if (t_max <= t_min) return false;
	}
	return true;
}

AABB AABB::combineAABB(const AABB& boxA, const AABB& boxB)
{
	AABB aabb{};

	aabb.info.p_min.x = std::min(boxA.minPoint().x, boxB.minPoint().x);
	aabb.info.p_min.y = std::min(boxA.minPoint().y, boxB.minPoint().y);
	aabb.info.p_min.z = std::min(boxA.minPoint().z, boxB.minPoint().z);

	aabb.info.p_max.x = std::max(boxA.maxPoint().x, boxB.maxPoint().x);
	aabb.info.p_max.y = std::max(boxA.maxPoint().y, boxB.maxPoint().y);
	aabb.info.p_max.z = std::max(boxA.maxPoint().z, boxB.maxPoint().z);

	return aabb;
}

BVH::Node::Node(BVH& tree, size_t vergeL, size_t vergeR)
	:left{ nullptr }, right{ nullptr }// isLeave (default)
{
	size_t n = vergeR - vergeL; // [vergeL, vergeR)
	//std::cout << "init Node " << vergeL << ' ' << vergeR << ' ' << n << '\n';
	if (n == 1)
	{
		AABB aabb;
		tree.data[vergeL]->bindAABB(&aabb);
		this->bounding = std::make_shared<AABB>( aabb );
		this->bounding->host = tree.data[vergeL].get(); // bind
		//std::cout << "leave>> " << this->bounding->minPoint()[0] << ' ' << this->bounding->maxPoint()[0] << '\n';
	}
	//else if (n == 2)
	else
	{
		static std::default_random_engine e;
		static std::uniform_int_distribution uniDis(0, 2); //[x,y,z]

		int axis = uniDis(e);
		//std::cout << "Axis:: " << char(axis + 'x') << ' ' << vergeL << ' ' << vergeR << std::endl;
		sort(tree.data.begin() + vergeL, tree.data.begin() + vergeL, 
			[axis](const std::shared_ptr<Object>& objA, const std::shared_ptr<Object>& objB)->bool
			{
				AABB boxA, boxB;
				objA->bindAABB(&boxA);
				objB->bindAABB(&boxB);
				return boxA.info.p_min[axis] < boxB.info.p_min[axis];
			});

		size_t mid = (vergeL + vergeR) >> 1;
		this->left = new BVH::Node(tree, vergeL, mid);
		this->right = new BVH::Node(tree, mid, vergeR);
		allnodes.emplace_back(this);
		allnodes.emplace_back(this->left);
		allnodes.emplace_back(this->right);

		this->bounding = std::make_shared<AABB>(AABB::combineAABB(*this->left->bounding, *this->right->bounding));
		//std::cout << "Combined[x] >> " <<  this->bounding->minPoint()[0] << ' ' << this->bounding->maxPoint()[0] << '\n';
	}// end else
}

bool BVH::Node::intersectionTest(Ray& ray, float t_min, float t_max) const
{
	if (this->bounding->intersectionTest(ray, t_min, t_max))
	{
		Ray::HitInfo leftInfo{};
		bool hitLeft = this->left ? this->left->bounding->intersectionTest(ray, t_min, t_max) : false;
		ray.swapHitInfo(leftInfo);
		bool hitRight = this->right ? this->right->bounding->intersectionTest(ray, t_min, t_max) : false;
		if (hitLeft && hitRight)
		{
			if (leftInfo.hit_time_first < ray.hitInfo().hit_time_first) ray.swapHitInfo(leftInfo);
			return true;
		}
		else if (hitLeft)
		{
			ray.swapHitInfo(leftInfo);
			return true;
		}
		else if (hitRight)
		{
			return true;
		}
		else return false;
	}
	return false;
}