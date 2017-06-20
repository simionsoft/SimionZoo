#pragma once
#include <vector>
#include "BulletSoftBody\btSoftRigidDynamicsWorld.h"
#include "BulletSoftBody\btSoftBodyHelpers.h"
#include "world.h"
using namespace std;

class Rope
{

public:

	Rope(CDynamicModel* pWorld, std::vector<btSoftBody*>* arr);
	~Rope() = default;
	virtual void updateRopePoints(CState* s, std::vector<btSoftBody*>* arr);
};