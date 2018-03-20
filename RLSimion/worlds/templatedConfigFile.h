#pragma once

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
	bool instantiateConfigFile(const char* outInstantiatedConfigFile, ArgsType... args);

};
