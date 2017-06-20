#include "../stdafx.h"
#include "Rope.h"
#include <string>

Rope::Rope(CDynamicModel* pWorld, std::vector<btSoftBody*>* arr)
{
	unsigned int j, i;
	for (j = 0; j < arr->size(); j++)
	{
		for (i = 0; i < arr->at(j)->m_links.size(); ++i)
		{
			pWorld->addStateVariable((string("rope") + to_string(j)+string("_p")+to_string(i)+string("X")).c_str(), "m", -20.0, 20.0);
			pWorld->addStateVariable((string("rope") + to_string(j) + string("_p") + to_string(i) + string("Y")).c_str(), "m", -20.0, 20.0);
			pWorld->addStateVariable((string("rope") + to_string(j) + string("_p") + to_string(i) + string("Z")).c_str(), "m", -20.0, 20.0);
		}
	}
}

void Rope::updateRopePoints(CState* s, std::vector<btSoftBody*>* arr)
{
	unsigned int j, i;
	for (j = 0; j < arr->size(); j++)
	{
		for (i = 0; i < arr->at(j)->m_links.size(); ++i)
		{
			const btSoftBody::Link&	bodyLink = arr->at(j)->m_links[i];
			double x = bodyLink.m_n[0]->m_x.m_floats[0];
			double y = bodyLink.m_n[0]->m_x.m_floats[1];
			double z = bodyLink.m_n[0]->m_x.m_floats[2];
			s->set((string("rope") + to_string(j) + string("_p") + to_string(i) + string("X")).c_str(), x);
			s->set((string("rope") + to_string(j) + string("_p") + to_string(i) + string("Y")).c_str(), y);
			s->set((string("rope") + to_string(j) + string("_p") + to_string(i) + string("Z")).c_str(), z);
		}
	}
}