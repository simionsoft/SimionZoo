#include "stdafx.h"
#include "SimionLogViewer.h"
#include "LogLoader.h"
#include "../OpenGLRenderer/Renderer.h"
#include "../OpenGLRenderer/input-handler.h"
#include "../OpenGLRenderer/text.h"
#include <algorithm>




SimionLogViewer::SimionLogViewer()
{
	m_pInstance = this;

}


SimionLogViewer::~SimionLogViewer()
{
	if (m_pRenderer) delete m_pRenderer;
	if (m_pExperimentLog) delete m_pExperimentLog;
}

void SimionLogViewer::init(int argc,char** argv)
{
	//init the renderer
	m_pRenderer = new CRenderer();
	m_pRenderer->init(argc, argv, 600, 400);

	m_pTimeText = new C2DText(string("Time"), Vector2D(0.1, 0.9), 0);
	m_pEpisodeText = new C2DText(string("Episode"), Vector2D(0.1, 0.85), 0);

	m_pRenderer->add2DGraphicObject(m_pTimeText);
	m_pRenderer->add2DGraphicObject(m_pEpisodeText);

	//init the input handling stuff
	glutSpecialFunc(_onSpecialKeyPressed);
	glutSpecialUpFunc(_onSpecialKeyReleased);
	glutKeyboardFunc(_onKeyPressed);
	glutKeyboardUpFunc(_onKeyReleased);
	glutSetKeyRepeat(GLUT_KEY_REPEAT_OFF);
}


void SimionLogViewer::_onSpecialKeyPressed(int key, int x, int y)
{
	((SimionLogViewer*)IInputHandler::get())->onSpecialKeyPressed(key, x, y);
}

void SimionLogViewer::_onSpecialKeyReleased(int key, int x, int y)
{
	((SimionLogViewer*)IInputHandler::get())->onSpecialKeyReleased(key, x, y);
}

void SimionLogViewer::_onKeyPressed(unsigned char key, int x, int y)
{
	((SimionLogViewer*)IInputHandler::get())->onKeyPressed(key, x, y);
}

void SimionLogViewer::_onKeyReleased(unsigned char key, int x, int y)
{
	((SimionLogViewer*)IInputHandler::get())->onKeyReleased(key, x, y);
}

void SimionLogViewer::onKeyPressed(unsigned char key, int x, int y)
{
	//keyboard callback function
	switch (key)
	{
		case 27: m_bExitRequested = true;
	}
}

void SimionLogViewer::onKeyReleased(unsigned char key, int x, int y)
{
}

void SimionLogViewer::onSpecialKeyPressed(int key, int x, int y)
{
	switch (key)
	{
	case GLUT_KEY_LEFT: previousEpisode(); break;
	case GLUT_KEY_RIGHT: nextEpisode(); break;
	}
}

void SimionLogViewer::onSpecialKeyReleased(int key, int x, int y)
{
}

void SimionLogViewer::nextEpisode()
{
	int index = m_episodeIndex + 1;
	index = index % m_numEpisodes;

	playEpisode(index);
}

void SimionLogViewer::previousEpisode()
{
	int index = m_episodeIndex - 1;
	if (index < 0) index += m_numEpisodes;

	playEpisode(index);
}

void SimionLogViewer::playEpisode(int i)
{
	m_episodeSimTime = 0.0;
	m_episodeIndex = i;
	m_pCurrentEpisode = m_pExperimentLog->getEpisode(i);
	m_episodeLength = m_pCurrentEpisode->getSimTimeLength();
	m_timer.start();
}

bool SimionLogViewer::loadLogFile(string filename)
{
	string sceneFile;
	m_pExperimentLog = new CExperimentLog();
	if (m_pExperimentLog->load(filename, sceneFile))
	{
		m_pRenderer->setDataFolder("../../config/scenes/");
		m_pRenderer->loadScene(sceneFile.c_str());

		//allocate a step object to store the interpolated data
		CEpisode* pFirstEpisode = m_pExperimentLog->getEpisode(0);
		m_pInterpolatedStep = new CStep(pFirstEpisode->getNumValuesPerStep());
		m_numEpisodes = m_pExperimentLog->getNumEpisodes();
	}
	else return false;

	m_timer.start();
	playEpisode(0);
	return true;
}


void SimionLogViewer::interpolateStepData(double t, CEpisode* pInEpisode, CStep* pOutInterpolatedData) const
{
	int step = 0;

	while (step<pInEpisode->getNumSteps() - 1 && t>pInEpisode->getStep(step + 1)->getEpisodeSimTime())
		++step;

	double t1, t0;
	t0 = pInEpisode->getStep(step)->getEpisodeSimTime();
	t1 = pInEpisode->getStep(step + 1)->getEpisodeSimTime();

	double u = (std::max(0.0, t - t0) / (t1 - t0));
	double v0, v1, interpolatedValue;

	u = std::max(0.0, std::min(1.0, u));

	for (int i = 0; i < pInEpisode->getStep(0)->getNumValues(); ++i)
	{
		v0 = pInEpisode->getStep(step)->getValue(i);
		v1 = pInEpisode->getStep(step + 1)->getValue(i);
		interpolatedValue = (1 - u)*v0 + (u)*v1;
		pOutInterpolatedData->setValue(i, interpolatedValue);
	}
}

void SimionLogViewer::handleInput()
{
	//handle input events
	glutMainLoopEvent();
}

void SimionLogViewer::draw()
{
	if (m_pCurrentEpisode == nullptr) return;

	double dt = m_timer.getElapsedTime(true);

	//time
	m_episodeSimTime += dt;
	while (m_episodeSimTime > m_episodeLength) m_episodeSimTime -= m_episodeLength;
	double value;

	//update bindings
	//and interpolate logged data between saved points
	interpolateStepData(m_episodeSimTime, m_pCurrentEpisode, m_pInterpolatedStep);
	m_pTimeText->set(string("Sim. Time: ") + std::to_string(m_episodeSimTime) + string(" / ") 
		+ std::to_string(m_episodeLength));
	m_pEpisodeText->set(string("Episode: ") + std::to_string(m_episodeIndex));
	for (int b = 0; b < m_pRenderer->getNumBindings(); ++b)
	{
		string varName = m_pRenderer->getBindingExternalName(b);
		int variableIndex = m_pExperimentLog->getVariableIndex(varName);
		value = m_pInterpolatedStep->getValue(variableIndex);
		m_pRenderer->updateBinding(varName, value);
	}

	//draw the scene
	m_pRenderer->redraw();
}