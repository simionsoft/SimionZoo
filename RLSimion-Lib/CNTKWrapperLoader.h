#pragma once


class IProblem;
namespace tinyxml2 { class XMLElement; }



class CNTKWrapperLoader
{
public:
	typedef IProblem* (__stdcall *getProblemInstanceDLL)(tinyxml2::XMLElement* pNode);
	static getProblemInstanceDLL getProblem;

	static void Load();
	static void UnLoad();
};

