#pragma once
#include "../WindowsUtils/timer.h"
#include "../OpenGLRenderer/input-handler.h"
#include <string>
using namespace std;

class Renderer;
class ExperimentLog;
class Text2D;
class Episode;
class Step;

class SimionLogViewer: public IInputHandler
{
	double m_episodeLength;
	int m_episodeIndex;
	int m_numEpisodes;
	double m_episodeSimTime = 0.0;

	Step* 	m_pInterpolatedStep;
	Episode* m_pCurrentEpisode;

	ExperimentLog* m_pExperimentLog= nullptr;
	Renderer* m_pRenderer= nullptr;

	Text2D * m_pTimeText;
	Text2D * m_pEpisodeText;

	bool m_bExitRequested = false;

	Timer m_timer;

	static void _onKeyPressed(unsigned char key, int x, int y);
	static void _onKeyReleased(unsigned char key, int x, int y);
	static void _onSpecialKeyPressed(int key, int x, int y);
	static void _onSpecialKeyReleased(int key, int x, int y);

	void onKeyPressed(unsigned char key, int x, int y);
	void onKeyReleased(unsigned char key, int x, int y);
	void onSpecialKeyPressed(int key, int x, int y);
	void onSpecialKeyReleased(int key, int x, int y);

	void nextEpisode();
	void previousEpisode();
	void playEpisode(int i);



	void interpolateStepData(double t, Episode* pInEpisode, Step* pOutInterpolatedData) const;
public:
	SimionLogViewer();
	virtual ~SimionLogViewer();

	void init(int argc, char** argv);
	bool loadLogFile(string filename);

	bool bExitRequested() const { return m_bExitRequested; }

	//drawing
	void draw();

	//input handling
	void handleInput();
};

