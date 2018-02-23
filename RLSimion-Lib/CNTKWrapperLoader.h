#pragma once


class IProblem;
namespace tinyxml2 { class XMLElement; }



namespace CNTKWrapper
{
	typedef IProblem* (__stdcall *getProblemInstanceDLL)(tinyxml2::XMLElement* pNode);
	extern getProblemInstanceDLL getProblem;

	void Init();
	void Close();
}

