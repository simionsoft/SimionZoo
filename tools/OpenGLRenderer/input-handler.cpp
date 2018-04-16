#include "stdafx.h"
#include "input-handler.h"
#include "renderer.h"
#include "camera.h"

#define MOVE_SPEED 10.0
#define ROTATION_SPEED 1.0
#define PI 3.1416f

IInputHandler* IInputHandler::m_pInstance = 0;

FreeCameraInputHandler::FreeCameraInputHandler()
{
	m_pInstance = this;

	glutSpecialFunc(_onSpecialKeyPressed);
	glutSpecialUpFunc(_onSpecialKeyReleased);
	glutKeyboardFunc(_onKeyPressed);
	glutKeyboardUpFunc(_onKeyReleased);
	glutSetKeyRepeat(GLUT_KEY_REPEAT_OFF);

	m_timer.start();
}

IInputHandler* IInputHandler::get()
{
	return m_pInstance;
}


FreeCameraInputHandler::~FreeCameraInputHandler()
{
}

void FreeCameraInputHandler::_onSpecialKeyPressed(int key, int x, int y)
{
	((FreeCameraInputHandler*)IInputHandler::get())->onSpecialKeyPressed(key, x, y);
}

void FreeCameraInputHandler::_onSpecialKeyReleased(int key, int x, int y)
{
	((FreeCameraInputHandler*)IInputHandler::get())->onSpecialKeyReleased(key, x, y);
}

void FreeCameraInputHandler::_onKeyPressed(unsigned char key, int x, int y)
{
	((FreeCameraInputHandler*)IInputHandler::get())->onKeyPressed(key, x, y);
}

void FreeCameraInputHandler::_onKeyReleased(unsigned char key, int x, int y)
{
	((FreeCameraInputHandler*)IInputHandler::get())->onKeyReleased(key, x, y);
}

void FreeCameraInputHandler::onKeyPressed(unsigned char key, int x, int y)
{
	//keyboard callback function
	switch (key)
	{
	case 'w': m_bWPressed = true; m_bShftPressed = false; break;
	case 'W': m_bWPressed = true; m_bShftPressed = true; break;
	case 's': m_bSPressed = true; m_bShftPressed = false; break;
	case 'S': m_bSPressed = true; m_bShftPressed = true; break;
	case 'a': m_bAPressed = true; m_bShftPressed = false; break;
	case 'A': m_bAPressed = true; m_bShftPressed = true; break;
	case 'd': m_bDPressed = true; m_bShftPressed = false; break;
	case 'D': m_bDPressed = true; m_bShftPressed = true; break;
	case 32: m_bSpacePressed = true; m_bShftPressed = false; break;
	case 27: exit(0);
	}
}

void FreeCameraInputHandler::onKeyReleased(unsigned char key, int x, int y)
{
	//keyboard callback function
	switch (key)
	{
	case 'w':
	case 'W': m_bWPressed = false; break;
	case 's':
	case 'S': m_bSPressed = false; break;
	case 'a':
	case 'A': m_bAPressed = false; break;
	case 'd':
	case 'D': m_bDPressed = false; break;
	case 32:
		m_real_time_execution_disabled = !m_real_time_execution_disabled;
		m_bSpacePressed = false;
		break;
	}
}

void FreeCameraInputHandler::onSpecialKeyPressed(int key, int x, int y)
{
	switch (key)
	{
	case GLUT_KEY_UP: m_bUpArrowPressed = true; break;
	case GLUT_KEY_DOWN: m_bDownArrowPressed = true; break;
	case GLUT_KEY_LEFT: m_bLeftArrowPressed = true; break;
	case GLUT_KEY_RIGHT: m_bRightArrowPressed = true; break;
	case GLUT_KEY_CTRL_L: m_bLeftCtrlPressed = true; break;
	}
}

void FreeCameraInputHandler::onSpecialKeyReleased(int key, int x, int y)
{
	switch (key)
	{
	case GLUT_KEY_UP: m_bUpArrowPressed = false; break;
	case GLUT_KEY_DOWN: m_bDownArrowPressed = false; break;
	case GLUT_KEY_LEFT: m_bLeftArrowPressed = false; break;
	case GLUT_KEY_RIGHT: m_bRightArrowPressed = false; break;
	case GLUT_KEY_CTRL_L: m_bLeftCtrlPressed = false; break;
	}
}

void FreeCameraInputHandler::handleInput()
{
	double dt = m_timer.getElapsedTime(true);

	glutMainLoopEvent();

	Camera* pCamera = Renderer::get()->getActiveCamera();
	if (!pCamera) return;

	Matrix44 tras = pCamera->getTransformMatrix();
	Vector3D res = tras*Vector3D(0, 0, -1);
	Quaternion quat;

	double yaw = pCamera->getTransform().rotation().yaw();
	if (bMoveForward())
		pCamera->addWorldOffset(Vector3D(-dt*MOVE_SPEED*sin(yaw), 0.0, -dt*MOVE_SPEED*cos(yaw)));
	if (bMoveBackward())
		pCamera->addWorldOffset(Vector3D(dt*MOVE_SPEED*sin(yaw), 0.0, dt*MOVE_SPEED*cos(yaw)));
	if (bYawRight())
	{
		quat = pCamera->getRotation();
		pCamera->setRotation(Quaternion(quat.yaw() + ROTATION_SPEED * dt, quat.pitch(), 0.0));
	}
	if (bYawLeft())
	{
		quat = pCamera->getRotation();
		pCamera->setRotation(Quaternion(quat.yaw() - ROTATION_SPEED * dt, quat.pitch(), 0.0));
	}
	if (bMoveUp())
		pCamera->addLocalOffset(Vector3D(0.0, dt*MOVE_SPEED, 0.0));
	if (bMoveDown())
		pCamera->addLocalOffset(Vector3D(0.0, -dt*MOVE_SPEED, 0.0));
	if (bMoveLeft())
		pCamera->addLocalOffset(Vector3D(dt*MOVE_SPEED, 0.0, 0.0));
	if (bMoveRight())
		pCamera->addLocalOffset(Vector3D(-dt*MOVE_SPEED, 0.0, 0.0));
}