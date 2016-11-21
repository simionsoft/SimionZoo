#include "stdafx.h"
#include "app.h"
#include "logger.h"
#include "world.h"
#include "experiment.h"
#include "SimGod.h"
#include "config.h"


CSimionApp* CSimionApp::m_pAppInstance = 0;

CSimionApp::CSimionApp()
{
	m_pAppInstance = this;
}

CSimionApp::~CSimionApp()
{
	m_pAppInstance = 0;
}

CSimionApp* CSimionApp::get()
{
	return m_pAppInstance;
}
