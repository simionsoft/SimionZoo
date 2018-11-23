#pragma once

#include "../../tools/System/CrossPlatform.h"

#ifndef MAX_CONFIG_FILE_SIZE
#define MAX_CONFIG_FILE_SIZE 10000
#endif

class TemplatedConfigFile
{
	char *m_pTemplateConfigFileContent;
	char *m_pInstantiatedConfigFile;

	bool m_bTemplateLoaded;

public:

	TemplatedConfigFile();
	virtual ~TemplatedConfigFile();

	bool load(const char* inTemplateConfigFile);

	template <typename ...ArgsType>
	bool instantiateConfigFile(const char* outInstantiatedConfigFile, ArgsType... args)
	{
		if (!m_bTemplateLoaded) return false;

		CrossPlatform::Sprintf_s(m_pInstantiatedConfigFile, MAX_CONFIG_FILE_SIZE, m_pTemplateConfigFileContent, args...);

		FILE* outputInstance;
		CrossPlatform::Fopen_s(&outputInstance, outInstantiatedConfigFile, "w");

		if (outputInstance)
		{
			fwrite(m_pInstantiatedConfigFile, 1, strlen(m_pInstantiatedConfigFile), outputInstance);
			fclose(outputInstance);
			return true;
		}
		return false;
	}

};
