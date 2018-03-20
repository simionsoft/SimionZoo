#pragma once

class ConfigNode;

class SetPoint
{
public:
	SetPoint(){}
	virtual ~SetPoint(){}

	virtual double getPointSet(double time)= 0;
};

class FileSetPoint: public SetPoint
{
protected:
	int m_numSteps;
	double *m_pSetPoints;
	double *m_pTimes;
	double m_totalTime;
public:
	FileSetPoint();
	FileSetPoint(const char* filename);
	virtual ~FileSetPoint();

	double getPointSet(double time);
};

class HHFileSetPoint : public FileSetPoint
{
public:
	HHFileSetPoint(const char* filename);
	virtual ~HHFileSetPoint(){}
};

class FixedStepSizeSetPoint: public SetPoint
{
	double m_stepTime;
	double m_lastStepTime;
	double m_lastSetPoint;
	double m_min, m_max;

public:
	FixedStepSizeSetPoint(double stepTime, double min, double max);
	virtual ~FixedStepSizeSetPoint();

	double getPointSet(double time);
};