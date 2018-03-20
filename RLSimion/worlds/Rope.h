#pragma once
#include <vector>
#include "..\..\3rd-party\bullet3-2.86\src\BulletSoftBody\btSoftRigidDynamicsWorld.h"
#include "..\..\3rd-party\bullet3-2.86\src\BulletSoftBody\btSoftBodyHelpers.h"
#include "world.h"
#include "BulletBody.h"

using namespace std;

class Rope: public BulletBody
{
	std::vector<btSoftBody*>* m_pSoftBodies;
public:

	Rope(DynamicModel* pWorld, std::vector<btSoftBody*>* arr);
	~Rope() = default;

	//override these two methods from BulletBody
	virtual void updateState(State* s);
	virtual void reset(State* s);
	virtual bool bIsRigidBody() { return false; }
};