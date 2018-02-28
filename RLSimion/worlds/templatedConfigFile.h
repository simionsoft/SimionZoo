#pragma once

class CTemplatedConfigFile
{
	char *m_pTemplateConfigFileContent;
	char *m_pInstantiatedConfigFile;

	bool m_bTemplateLoaded;

public:

	CTemplatedConfigFile();
	virtual ~CTemplatedConfigFile();

	bool load(const char* inTemplateConfigFile);

	template <typename ...ArgsType>
	bool instantiateConfigFile(const char* outInstantiatedConfigFile, ArgsType... args);

};
