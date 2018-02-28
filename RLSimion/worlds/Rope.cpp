#include "Rope.h"
#include <string>

Rope::Rope(CDynamicModel* pWorld, std::vector<btSoftBody*>* arr): BulletBody()
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

void Rope::reset(CState* s)
{
	updateState(s);
}

void Rope::updateState(CState* s)
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