#pragma once
#include "../WindowsUtils/Timer.h"


class IInputHandler
{
protected:
	static IInputHandler* m_pInstance;
public:
	static IInputHandler* get();
	
	virtual void handleInput() = 0;
};

class CFreeCameraInputHandler: public IInputHandler
{
	CTimer m_timer;

	bool m_bWPressed = false;
	bool m_bSPressed = false;
	bool m_bDPressed = false;
	bool m_bAPressed = false;
	bool m_bUpArrowPressed = false;
	bool m_bDownArrowPressed = false;
	bool m_bLeftArrowPressed = false;
	bool m_bRightArrowPressed = false;
	bool m_bShftPressed = false;
	bool m_bSpacePressed = false;
	bool m_real_time_execution_disabled = false;

	static void _onKeyPressed(unsigned char key, int x, int y);
	static void _onKeyReleased(unsigned char key, int x, int y);
	static void _onSpecialKeyPressed(int key, int x, int y);
	static void _onSpecialKeyReleased(int key, int x, int y);

	void onKeyPressed(unsigned char key, int x, int y);
	void onKeyReleased(unsigned char key, int x, int y);
	void onSpecialKeyPressed(int key, int x, int y);
	void onSpecialKeyReleased(int key, int x, int y);

	bool bMoveForward() const { return !m_bShftPressed && (m_bWPressed || m_bUpArrowPressed); }
	bool bMoveBackward() const { return !m_bShftPressed && (m_bSPressed || m_bDownArrowPressed); }
	bool bYawRight() const { return !m_bShftPressed && 
		(((m_bAPressed || m_bLeftArrowPressed) && !bMoveBackward())
		|| ((m_bDPressed || m_bRightArrowPressed) && bMoveBackward())); }
	bool bYawLeft() const { return !m_bShftPressed && 
		(((m_bAPressed || m_bLeftArrowPressed) && bMoveBackward())
		|| ((m_bDPressed || m_bRightArrowPressed) && !bMoveBackward())); }
	bool bMoveUp() const { return m_bShftPressed && ( m_bWPressed || m_bUpArrowPressed); }
	bool bMoveDown() const { return m_bShftPressed && (m_bSPressed || m_bDownArrowPressed); }
	bool bMoveLeft() const { return m_bShftPressed && (m_bAPressed || m_bLeftArrowPressed); }
	bool bMoveRight() const { return m_bShftPressed && (m_bDPressed || m_bRightArrowPressed); }

public:
	CFreeCameraInputHandler();
	virtual ~CFreeCameraInputHandler();

	virtual void handleInput();

	bool getRealTimeExecutionDisabled() {return m_real_time_execution_disabled;}
};

