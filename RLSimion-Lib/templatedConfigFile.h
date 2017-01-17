#pragma once

class CTemplatedConfigFile
{
	char *m_pTemplateConfigFileContent;
	char *m_pInstantiatedConfigFile;
	char *m_pAuxBuffer;
	bool m_bTemplateLoaded;

	void instantiate(double value);
	void instantiate(int value);
	void instantiate(const char* value);


	template <typename FirstArgType, typename ...RestArgsType>
	void instantiate(FirstArgType firstArgValue, RestArgsType... restArgs);
public:

	CTemplatedConfigFile();
	virtual ~CTemplatedConfigFile();

	bool load(const char* inTemplateConfigFile);



	template <typename FirstArgType, typename ...RestArgsType>
	bool instantiateConfigFile(const char* outInstantiatedConfigFile, FirstArgType firstArgValue, RestArgsType... restArgs);

};
