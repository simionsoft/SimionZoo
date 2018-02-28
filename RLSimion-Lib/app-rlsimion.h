#pragma once

#include "app.h"
#include "../tools/WindowsUtils/Timer.h"
#define MAX_PATH_SIZE 1024

class CRenderer;
class IInputHandler;
class C2DText;
class C2DMeter;
using namespace std;

class RLSimionApp : public CSimionApp
{
	//Rendering
	CTimer m_timer;
	CRenderer *m_pRenderer = 0;
	IInputHandler *m_pInputHandler = 0;
	void initRenderer(string sceneFile);
	void updateScene(CState* s);
	C2DText* m_pProgressText= 0;
	vector<C2DMeter*> m_pStatsText;
public:

	RLSimionApp(CConfigNode* pParameters);
	virtual ~RLSimionApp();

	void run();
};