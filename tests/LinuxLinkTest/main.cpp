#include <cstdio>

#include "../../3rd-party/tinyxml2/tinyxml2.h"
#include "../../tools/GeometryLib/vector3d.h"
#include "../../tools/OpenGLRenderer/renderer.h"

#include <iostream>
using namespace std;

int main()
{
	Vector3D myvec(2, 3, 1);
	double l= myvec.length();
	cout << "Vector length= " << l << "\n";
	tinyxml2::XMLDocument doc;
	doc.LoadFile("test.xml");
	if (!doc.Error())
	{
		tinyxml2::XMLElement* pRoot = doc.RootElement();
		tinyxml2::XMLElement* pChild = pRoot->FirstChildElement();
		while (pChild)
		{
			cout << "Child: " << pChild->Name() << "\n";
			pChild = pChild->NextSiblingElement();
		}
	}
	Renderer renderer;
	//int argc = 1;
	//const char* argv = "test";
	//renderer.init(argc, &argv, 800, 600);
    return 0;
}