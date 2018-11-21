#pragma once
#include "../System/Timer.h"


class IInputHandler
{
protected:
	static IInputHandler* m_pInstance;
public:
	static IInputHandler* get();

	virtual ~IInputHandler() = 0;
	
	virtual void handleInput() = 0;
};


#define NUM_KEYS 256

class FreeCameraInputHandler: public IInputHandler
{
	Timer m_timer;

	bool m_keyboardState[256];
	bool m_specialKeysState[256];

	static void _onKeyPressed(unsigned char key, int x, int y);
	static void _onKeyReleased(unsigned char key, int x, int y);
	static void _onSpecialKeyPressed(int key, int x, int y);
	static void _onSpecialKeyReleased(int key, int x, int y);

	void onKeyPressed(unsigned char key, int x, int y);
	void onKeyReleased(unsigned char key, int x, int y);
	void onSpecialKeyPressed(int key, int x, int y);
	void onSpecialKeyReleased(int key, int x, int y);

	bool keyPressed(unsigned char key) const;
	bool specialKeyPressed(unsigned char key) const;

	bool bMoveForward() const;
	bool bMoveBackward() const;
	bool bYawRight() const;
	bool bYawLeft() const;
	bool bMoveUp() const;
	bool bMoveDown() const;
	bool bMoveLeft() const;
	bool bMoveRight() const;
public:
	FreeCameraInputHandler();
	virtual ~FreeCameraInputHandler();

	virtual void handleInput();
};

