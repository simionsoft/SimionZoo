#pragma once

class CWorld;
class CExperiment;
class CSimGod;
class CParameters;

namespace RLSimion
{
	extern CWorld *g_pWorld;
	extern CExperiment *g_pExperiment;
	extern CSimGod *g_pSimGod;

	void init(CParameters* pParameters);
	void shutdown();
}