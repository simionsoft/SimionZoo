#pragma once

class CWorld;
class CExperiment;
class CSimGod;

namespace RLSimion
{
	extern CWorld *g_pWorld;
	extern CExperiment *g_pExperiment;
	extern CSimGod *g_pSimGod;

	void init(tinyxml2::XMLElement* pParameters);
	void shutdown();
}