#pragma once
#include <vector>
#include "BulletSoftBody\btSoftRigidDynamicsWorld.h"
#include "BulletSoftBody\btSoftBodyHelpers.h"
#include "world.h"
#include "BulletBody.h"

using namespace std;

class Rope: public BulletBody
{
	std::vector<btSoftBody*>* m_pSoftBodies;
public:

	Rope(CDynamicModel* pWorld, std::vector<btSoftBody*>* arr);
	~Rope() = default;

	//override these two methods from BulletBody
	virtual void updateState(CState* s);
	virtual void reset(CState* s);
	virtual bool bIsRigidBody() { return false; }
};