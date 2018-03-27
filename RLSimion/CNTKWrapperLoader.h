#pragma once


class INetworkDefinition;
namespace tinyxml2 { class XMLElement; }



class CNTKWrapperLoader
{
public:
	typedef INetworkDefinition* (__stdcall *getProblemInstanceDLL)(tinyxml2::XMLElement* pNode);
	static getProblemInstanceDLL getProblem;

	static void Load();
	static void UnLoad();
};

