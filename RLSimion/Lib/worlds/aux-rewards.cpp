/*
	SimionZoo: A framework for online model-free Reinforcement Learning on continuous
	control problems

	Copyright (c) 2016 SimionSoft. https://github.com/simionsoft

	Permission is hereby granted, free of charge, to any person obtaining a copy
	of this software and associated documentation files (the "Software"), to deal
	in the Software without restriction, including without limitation the rights
	to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
	copies of the Software, and to permit persons to whom the Software is
	furnished to do so, subject to the following conditions:

	The above copyright notice and this permission notice shall be included in all
	copies or substantial portions of the Software.

	THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
	IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
	FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
	AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
	LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
	OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
	SOFTWARE.
*/

#include "aux-rewards.h"
#include "../../Common/named-var-set.h"
#include "world.h"
#include <algorithm>
#include <math.h>

double static getDistanceBetweenPoints(double x1, double y1, double x2, double y2)
{
	double distance = sqrt(pow(x2 - x1, 2) + pow(y2 - y1, 2));
	return distance;
}

DistanceReward2D::DistanceReward2D(Descriptor& stateDescr, const char* var1xName, const char* var1yName, const char* var2xName, const char* var2yName)
{
	m_var1xId = var1xName;
	m_var1yId = var1yName;
	m_var2xId = var2xName;
	m_var2yId = var2yName;

	//here we assume both variables have the same value range
	m_maxDist = sqrt(stateDescr.getProperties(m_var1xId)->getRangeWidth()
		* stateDescr.getProperties(m_var1xId)->getRangeWidth()
		+ stateDescr.getProperties(m_var1yId)->getRangeWidth()
		* stateDescr.getProperties(m_var1yId)->getRangeWidth());
}

double DistanceReward2D::getReward(const State* s, const Action* a, const State* s_p)
{
	double boxX = s_p->get(m_var1xId);
	double boxY = s_p->get(m_var1yId);
	double targetX = s_p->get(m_var2xId);
	double targetY = s_p->get(m_var2yId);

	double distance = getDistanceBetweenPoints(targetX, targetY, boxX, boxY);

	distance = std::max(distance, 0.0001);

	double reward = 1. - distance / m_maxDist;

	return reward;

}

double DistanceReward2D::getMin()
{
	return 0.0;
}

double DistanceReward2D::getMax()
{
	return 1.0;
}

DistanceReward3D::DistanceReward3D(Descriptor & stateDescr, const char * var1xName, const char * var1yName, const char * var1zName, const char * var1rotxName, const char * var1rotzName, const char* var1vlinearyName, const char * var2xName, const char * var2yName, const char * errorName)
{
	m_error = errorName;

	m_var1xId = var1xName;
	m_var1yId = var1yName;
	m_var1zId = var1zName;
	m_var1rotxId = var1rotxName;
	m_var1rotzId = var1rotzName;
	m_var1vlinearId = var1vlinearyName;
	m_var2xId = var2xName;
	m_var2yId = var2yName;

	//here we assume both variables have the same value range. We do not use z, because before the drone must fly until it starts taking land
	m_maxDist = sqrt(stateDescr.getProperties(m_var1xId)->getRangeWidth()
		* stateDescr.getProperties(m_var1xId)->getRangeWidth()
		+ stateDescr.getProperties(m_var1yId)->getRangeWidth()
		* stateDescr.getProperties(m_var1yId)->getRangeWidth());
}

double DistanceReward3D::getReward(const State * s, const Action * a, const State * s_p)
{
	double errorDistancia = (abs(s_p->get(m_error)) / Drone6DOF::altura);
	double errorX = abs(s->get("errorX"));
	double errorZ = abs(s->get("errorY"));
	if(errorX > 0.001 || errorZ > 0.001)
		errorDistancia += (errorX + errorZ) * 100000;
	/*bool arriba = s_p->get(m_error) < 0.0;
	double droneVY0 = s->get(m_var1vlinearId);
	double droneVY1 = s_p->get(m_var1vlinearId);
	//double d_rotX = abs(s->get(m_var1rotxId))-abs(s_p->get(m_var1rotxId));
	//double d_rotZ = abs(s->get(m_var1rotzId))-abs(s_p->get(m_var1rotzId));
	//double errorRot = (d_rotX + d_rotZ)*factorRot;
	// si está arriba y acelerando
	/*if (arriba && droneVY1 < droneVY0)
		return std::max(getMin(), 1 - errorDistancia * 1.5);
	// si está arriba y no está acelerando lo que haya
	if (arriba)
		return std::max(getMin(), 1 - errorDistancia);
	//está por debajo y frenando*/
		return std::max(1 - errorDistancia, getMin());




	/*
	//la rotación ha empeorado
	if (errorRot <= 0.0)
	{
		//si la rotacion no es mala
		if (abs(s_p->get(m_var1rotxId)) + abs(s_p->get(m_var1rotzId)) < maxRot)
		{
			//si el drone esta por encima y acelerando
			if (arriba && droneVY1 < droneVY0)
				return std::max(getMin(), 1 - errorDistancia*1.5);
			else
				return std::max(1 - errorDistancia,getMin());
		}
		//si la rotacion es mala
		else
			return std::max(getMin(), 1 - errorDistancia * 1.6);;
	}
	else
	{
		//se ha mejorado valor positivo 
		//estamos por encima acelerando
		if ((arriba && droneVY1 < droneVY0))
			errorDistancia*=1.5;
		return std::max(1.0-errorDistancia,0.0);
		
	}
	*/

}

double DistanceReward3D::getMin()
{
	return 0.0;
}

double DistanceReward3D::getMax()
{
	return 1.0;
}
