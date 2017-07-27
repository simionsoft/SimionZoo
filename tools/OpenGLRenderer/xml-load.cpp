#include "stdafx.h"
#include "xml-load.h"
#include "../GeometryLib/matrix44.h"

//////////////////////////////////////////////////////////////////////
//UTILITY FUNCTIONS

void loadColladaMatrix(const char* pText, Matrix44& outMatrix)
{
	//Collada uses column-major matrices:
	//https://forums.khronos.org/showthread.php/11010-matrix-and-collada
	const char* p = pText;
	size_t charCount = strlen(p);
	int col = 0, row = 0;
	for (int row = 0; row < 4; ++row)
	{
		for (int col = 0; col < 4; ++col)
		{
			outMatrix.set(col, row, atof(p));

			while ((size_t)(p - pText) < charCount && *p != ' ') ++p;
			while (*p == ' ') ++p;
		}
	}
}


void loadVector3D(const char* pText, Vector3D& outVec)
{
	const char* p = pText;
	size_t charCount = strlen(p);

	outVec.setX(atof(p));
	while ((size_t)(p - pText) < charCount && *p != ' ') ++p;
	while (*p == ' ') ++p;
	outVec.setY(atof(p));
	while ((size_t)(p - pText) < charCount && *p != ' ') ++p;
	while (*p == ' ') ++p;
	outVec.setZ(atof(p));
	while ((size_t)(p - pText) < charCount && *p != ' ') ++p;
}

unsigned int countChildren(tinyxml2::XMLElement* pParentNode, const char* xmlTag)
{
	unsigned int count = 0;
	tinyxml2::XMLElement* pChild = pParentNode->FirstChildElement(xmlTag);
	while (pChild)
	{
		++count;
		pChild = pChild->NextSiblingElement();
	}
	return count;
}