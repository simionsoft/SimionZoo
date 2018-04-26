#include "stdafx.h"
#include "SimionLogViewer.h"
#include "LogLoader.h"
#include "../OpenGLRenderer/Renderer.h"
#include "../OpenGLRenderer/input-handler.h"
#include "../OpenGLRenderer/text.h"
#include "../OpenGLRenderer/basic-shapes-2d.h"
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
	m_pRenderer = new Renderer();
	m_pRenderer->init(argc, argv, 600, 400);

	m_pTimeText = new Text2D(string("Time"), Vector2D(0.05, 0.9), 0.5);
	m_pEpisodeText = new Text2D(string("Episode"), Vector2D(0.05, 0.85), 0.5);
	m_pPlaybackRateText = new Text2D(string("Playback Rate"), Vector2D(0.8, 0.9), 0.5);

	m_pRenderer->add2DGraphicObject(m_pTimeText);
	m_pRenderer->add2DGraphicObject(m_pEpisodeText);
	m_pRenderer->add2DGraphicObject(m_pPlaybackRateText);

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
	case GLUT_KEY_UP: faster(); break;
	case GLUT_KEY_DOWN: slower(); break;
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

void SimionLogViewer::faster()
{
	switch (m_playbackMode)
	{
	case PlaybackMode::Quarter: m_playbackMode = PlaybackMode::Half; break;
	case PlaybackMode::Half: m_playbackMode = PlaybackMode::Normal; break;
	case PlaybackMode::Normal: m_playbackMode = PlaybackMode::x2; break;
	case PlaybackMode::x2: m_playbackMode = PlaybackMode::x4; break;
	}
}

double SimionLogViewer::getPlaybackRate()
{
	switch (m_playbackMode)
	{
	case PlaybackMode::Quarter: return 0.25;
	case PlaybackMode::Half: return 0.5;
	case PlaybackMode::Normal: return 1.0;
	case PlaybackMode::x2: return 2.0;
	case PlaybackMode::x4: return 4.0;
	}
	return 1.0;
}

void SimionLogViewer::slower()
{
	switch (m_playbackMode)
	{
	case PlaybackMode::Half: m_playbackMode = PlaybackMode::Quarter; break;
	case PlaybackMode::Normal: m_playbackMode = PlaybackMode::Half; break;
	case PlaybackMode::x2: m_playbackMode = PlaybackMode::Normal; break;
	case PlaybackMode::x4: m_playbackMode = PlaybackMode::x2; break;
	}
}

bool SimionLogViewer::loadLogFile(string filename)
{
	string sceneFile;
	m_pExperimentLog = new ExperimentLog();
	if (m_pExperimentLog->load(filename, sceneFile))
	{
		m_pRenderer->setDataFolder("..\\config\\scenes\\");
		m_pRenderer->loadScene(sceneFile.c_str());

		//allocate a step object to store the interpolated data
		Episode* pFirstEpisode = m_pExperimentLog->getEpisode(0);
		m_pInterpolatedStep = new Step(pFirstEpisode->getNumValuesPerStep());
		m_numEpisodes = m_pExperimentLog->getNumEpisodes();
	}
	else return false;

	//add meter2D objects to show variable values
	Descriptor& logDescriptor = m_pExperimentLog->getDescriptor();
	Meter2D* pMeter;
	Vector2D origin = Vector2D(0.05, 0.8);
	Vector2D size = Vector2D(0.3, 0.03);
	Vector2D offset = Vector2D(0.0, 0.04);
	double depth = 0.25;
	for (int i = 0; i < logDescriptor.size(); i++)
	{
		pMeter = new Meter2D(logDescriptor[i].getName(), origin, size, depth);
		pMeter->setValueRange(Range(logDescriptor[i].getMin(), logDescriptor[i].getMax()));
		m_pRenderer->add2DGraphicObject(pMeter);
		m_variableMeters.push_back(pMeter);
		origin -= offset;
	}

	m_timer.start();
	playEpisode(0);
	return true;
}


void SimionLogViewer::interpolateStepData(double t, Episode* pInEpisode, Step* pOutInterpolatedData) const
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

	Descriptor& descriptor = m_pExperimentLog->getDescriptor();

	for (int i = 0; i < pInEpisode->getStep(0)->getNumValues(); ++i)
	{
		v0 = pInEpisode->getStep(step)->getValue(i);
		v1 = pInEpisode->getStep(step + 1)->getValue(i);
		//if the variable is circular and the two values lay on opposite sides of the range, make them closer
		if (descriptor[i].bIsCircular() && abs(v0 - v1) > descriptor[i].getRangeWidth()*0.8)
		{
			if (v0 < v1)
				v1 -= descriptor[i].getRangeWidth();
			else
				v1 += descriptor[i].getRangeWidth();

			if (abs(v0 - v1) > 0.5)
				printf("jump");
		}
		interpolatedValue = (1 - u)*v0 + (u)*v1;
		pOutInterpolatedData->setValue(i, interpolatedValue);
	}
}

void SimionLogViewer::handleInput()
{
	//handle input events
	glutMainLoopEvent();
}

#include <sstream>
#include <iomanip>

template <typename T>
std::string to_string_with_precision(const T a_value, const int n = 6)
{
	std::ostringstream out;
	out << std::fixed << std::setprecision(n)  << a_value;
	return out.str();
}

void SimionLogViewer::draw()
{
	if (m_pCurrentEpisode == nullptr || m_episodeLength==0.0) return;

	double playbackRate = getPlaybackRate();
	double dt = m_timer.getElapsedTime(true)*playbackRate;

	//time
	m_episodeSimTime += dt;
	while (m_episodeSimTime > m_episodeLength) m_episodeSimTime -= m_episodeLength;
	double value;

	//update texts on screen
	m_pTimeText->set(string("Sim. Time: ") + to_string_with_precision(m_episodeSimTime, 2) + string(" / ")
		+ to_string_with_precision(m_episodeLength, 2));
	m_pEpisodeText->set(string("Episode: ") + std::to_string(m_episodeIndex));
	m_pPlaybackRateText->set(string("Rate: ") + to_string_with_precision(playbackRate, 2));

	//interpolate logged data between saved points
	interpolateStepData(m_episodeSimTime, m_pCurrentEpisode, m_pInterpolatedStep);

	//update variable meters
	Descriptor& logDescriptor = m_pExperimentLog->getDescriptor();
	for (int i = 0; i < logDescriptor.size(); i++)
		m_variableMeters[i]->setValue(m_pInterpolatedStep->getValue(i));

	//update bindings
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