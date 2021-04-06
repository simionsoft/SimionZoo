#pragma once
#include "../reward.h"
#include "drone-6-dof.h"
class NamedVarSet;
typedef NamedVarSet State;
typedef NamedVarSet Action;
typedef NamedVarSet Reward;
class Descriptor;

class DistanceReward2D : public IRewardComponent
{
	const char *m_var1xId = nullptr, *m_var1yId = nullptr, *m_var2xId = nullptr, *m_var2yId = nullptr;
	double m_maxDist= 1.0;
public:
	DistanceReward2D(Descriptor& stateDescr, const char* var1xName, const char* var1yName, const char* var2xName, const char* var2yName);
	double getReward(const State *s, const Action *a, const State *s_p);
	const char* getName() { return "reward"; }
	double getMin();
	double getMax();
};

class DistanceReward3D : public IRewardComponent
{
	const char *m_error = nullptr;
	const char *m_var1xId = nullptr, *m_var1yId = nullptr, *m_var1zId = nullptr, *m_var1vlinearId=nullptr, *m_var1rotxId = nullptr, *m_var1rotzId = nullptr, *m_var2xId = nullptr, *m_var2yId = nullptr;
	double m_maxDist = 1.0;
	const double maxRot = 0.0000001;//3.14159265358979323846 / 6;
	const double factorRot = 10000000;
	const double factorAltura = 50;
	const double targetHeight = Drone6DOF::height;
public:
	//7 parametros de x y z x y rot x rot y
	DistanceReward3D(Descriptor& stateDescr, const char* var1xName, const char* var1yName, const char* var1zName, const char* var1rotxName, const char* var1rotyName, const char* var1vlinearyName , const char* var2xName, const char* var2yName, const char * errorName);
	double getReward(const State *s, const Action *a, const State *s_p);
	const char* getName() { return "reward"; }
	double getMin();
	double getMax();
};