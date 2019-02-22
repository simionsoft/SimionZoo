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

#include "Rope.h"
#include <string>

Rope::Rope(DynamicModel* pWorld, std::vector<btSoftBody*>* arr): BulletBody()
{
	m_pSoftBodies = arr;

	unsigned int j;
	int i;
	for (j = 0; j < arr->size(); j++)
	{
		for (i = 0; i < arr->at(j)->m_links.size(); ++i)
		{
			pWorld->addStateVariable((string("rope") + to_string(j)+string("-p")+to_string(i)+string("-x")).c_str(), "m", -20.0, 20.0);
			pWorld->addStateVariable((string("rope") + to_string(j) + string("-p") + to_string(i) + string("-y")).c_str(), "m", -20.0, 20.0);
			pWorld->addStateVariable((string("rope") + to_string(j) + string("-p") + to_string(i) + string("-z")).c_str(), "m", -20.0, 20.0);
		}
	}
}

void Rope::reset(State* s)
{
	updateState(s);
}

void Rope::updateState(State* s)
{
	unsigned int j;
	int i;
	for (j = 0; j < m_pSoftBodies->size(); j++)
	{
		for (i = 0; i < m_pSoftBodies->at(j)->m_links.size(); ++i)
		{
			const btSoftBody::Link&	bodyLink = m_pSoftBodies->at(j)->m_links[i];
			double x = bodyLink.m_n[0]->m_x.m_floats[0];
			double y = bodyLink.m_n[0]->m_x.m_floats[1];
			double z = bodyLink.m_n[0]->m_x.m_floats[2];
			s->set((string("rope") + to_string(j) + string("-p") + to_string(i) + string("-x")).c_str(), x);
			s->set((string("rope") + to_string(j) + string("-p") + to_string(i) + string("-y")).c_str(), y);
			s->set((string("rope") + to_string(j) + string("-p") + to_string(i) + string("-z")).c_str(), z);
		}
	}
}