#pragma once

#include "app.h"
#include "../tools/WindowsUtils/Timer.h"
#define MAX_PATH_SIZE 1024

class Renderer;
class IInputHandler;
class Text2D;
class Meter2D;
class NamedVarSet;
using State = NamedVarSet;
using Action = NamedVarSet;
using namespace std;

class RLSimionApp : public SimionApp
{
	//Rendering
	Timer m_timer;
	Renderer *m_pRenderer = 0;
	IInputHandler *m_pInputHandler = 0;
	void initRenderer(string sceneFile);
	void updateScene(State* s, Action* a);
	Text2D* m_pProgressText= 0;
	vector<Meter2D*> m_pStatsUIMeters;
	vector<Meter2D*> m_pStateUIMeters;
	vector<Meter2D*> m_pActionUIMeters;

	void update2DMeters(State* s, Action* a);
public:

	RLSimionApp(ConfigNode* pParameters);
	virtual ~RLSimionApp();

	void run();
};