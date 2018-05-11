#pragma once


class INetworkDefinition;
namespace tinyxml2 { class XMLElement; }



class CNTKWrapperLoader
{
public:
	typedef INetworkDefinition* (__stdcall *getNetworkDefinitionDLL)(tinyxml2::XMLElement* pNode);
	typedef void(__stdcall *setDeviceDLL)(bool useGPU);
	static getNetworkDefinitionDLL getNetworkDefinition;
	static setDeviceDLL setDevice;

	static void Load();
	static void UnLoad();
};

