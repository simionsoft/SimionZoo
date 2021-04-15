#pragma once

#include "world.h"
#include "../../../3rd-party/bullet3-2.86/src/btBulletDynamicsCommon.h"


class Robot;
class BulletBody;
class BulletPhysics;
class SetPoint;

//Move box with 2 robots
class Drone6DOFControl : public DynamicModel
{
	/// All-Simulation variables
	double MASS_TARGET;
	double MASS_ROBOT;
	double MASS_GROUND;

	/// State variables
	size_t m_error_z, m_error_x, m_error_y;

	size_t m_d_error_x, m_d_error_y, m_d_error_z;

	size_t m_target_x, m_target_y, m_target_z;
	size_t m_base_x, m_base_y, m_base_z;
	size_t m_base_rot_x, m_base_rot_y, m_base_rot_z;
	size_t m_base_angular_x, m_base_angular_y, m_base_angular_z;
	size_t m_base_linear_x, m_base_linear_y, m_base_linear_z;

	size_t m_drone1_x, m_drone1_y, m_drone1_Z;
	size_t m_drone1_rot_x, m_drone1_rot_y, m_drone1_rot_z;
	size_t m_angular_drone1_X, m_angular_drone1_Y, m_angular_drone1_Z;
	size_t m_drone1_linear_x, m_drone1_linear_y, m_drone1_linear_z;

	size_t m_drone2_x, m_drone2_y, m_drone2_z;
	size_t m_drone2_rot_x, m_drone2_rot_y, m_drone2_rot_z;
	size_t m_drone2_linear_x, m_drone2_linear_y, m_drone2_linear_z;
	size_t m_drone2_angular_x, m_drone2_angular_y, m_drone2_angular_z;

	size_t m_drone3_x, m_drone3_y, m_drone3_z;
	size_t m_drone3_rot_x, m_drone3_rot_y, m_drone3_rot_z;
	size_t m_drone3_angular_x, m_drone3_angular_y, m_drone3_angular_z;
	size_t m_drone3_linear_x, m_drone3_linear_y, m_drone3_linear_z;

	size_t m_drone4_x, m_drone4_y, m_drone4_z;
	size_t m_drone4_rot_x, m_drone4_rot_y, m_drone4_rot_z;
	size_t m_drone4_linear_x, m_drone4_linear_y, m_drone4_linear_z;
	size_t m_drone4_angular_x, m_drone4_angular_y, m_drone4_angular_z;


	// Action variables
	size_t m_force_1_1;
	size_t m_force_1_2;
	size_t m_force_1_3;
	size_t m_force_1_4;
	size_t m_force_2_1;
	size_t m_force_2_2;
	size_t m_force_2_3;
	size_t m_force_2_4;
	size_t m_force_3_1;
	size_t m_force_3_2;
	size_t m_force_3_3;
	size_t m_force_3_4;
	size_t m_force_4_1;
	size_t m_force_4_2;
	size_t m_force_4_3;
	size_t m_force_4_4;


	///inicializaci�n
	BulletPhysics* m_pBulletPhysics;
	SetPoint* m_pSetpoint;

public:
	Drone6DOFControl(ConfigNode* pParameters);
	virtual ~Drone6DOFControl();
	void reset(State* s);
	void executeAction(State* s, const Action* a, double dt);

};

