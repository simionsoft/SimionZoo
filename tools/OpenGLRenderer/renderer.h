#pragma once
#include <vector>
#include "../WindowsUtils/Timer.h"

class CGraphicObject;
class CGraphicObject2D;
class CTextureManager;
class Binding;
class Bindable;
class CCamera;
class CLight;
class BoundingBox3D;
class BoundingBox2D;
using namespace std;
namespace tinyxml2 { class XMLElement; }

class CRenderer
{
	CTimer m_timer;
	int m_frameCount = 0;
	double m_fps = 0.0;
	unsigned int m_num3DObjectsDrawn = 0;

	CTextureManager* m_pTextureManager;

	CCamera* m_pActiveCamera;
	vector <CCamera*> m_cameras;

	vector <CLight*> m_lights;

	string m_dataFolder;

	static CRenderer* m_pInstance;

	vector<CGraphicObject*> m_3DgraphicObjects;
	vector<CGraphicObject2D*> m_2DgraphicObjects;

	vector <Binding*> m_bindings;

	int m_windowWidth= 1024, m_windowHeight= 768;

	void reshapeWindow(int w, int h);
	void drawScene();
	static void _drawScene();
	static void _reshapeWindow(int width, int height);
	void loadSceneObjects(tinyxml2::XMLElement* pNode);
	Binding* getBinding(string externalName);

	void drawBoundingBox3D(BoundingBox3D& box) const;
	void drawBoundingBox2D(BoundingBox2D& box) const;

	bool m_bShowBoundingBoxes= false;
public:
	CRenderer();
	virtual ~CRenderer();

	void getWindowsSize(int& sizeX, int& sizeY) { sizeX = m_windowWidth; sizeY = m_windowHeight; }
	double getFPS() { return m_fps; }
	void updateFPS();
	unsigned int getNum3DObjectsDrawn() { return m_num3DObjectsDrawn; }

	void setDataFolder(string dataFolder);
	string getDataFolder();

	void loadScene(const char* file);

	void redraw();

	void init(int argc, char** argv, int sizeX, int sizeY);

	void addGraphicObject(CGraphicObject* pObj);
	void add2DGraphicObject(CGraphicObject2D* pObj);

	static CRenderer* get();
	CTextureManager* getTextureManager();
	CCamera* getActiveCamera();

	CGraphicObject* getObjectByName(string name);

	void showBoundingBoxes(bool bShow) { m_bShowBoundingBoxes = bShow; }
	bool bShowBoundingBoxes() { return m_bShowBoundingBoxes; }

	//this is called by Bindable objects at initialization time
	template <typename T>
	void registerBinding(string externalName, T& obj, string internalName)
	{
		Binding* pBinding = getBinding(externalName);
		if (pBinding == nullptr)
		{
			//No binding registered yet for the external name (i.e, the state variable's name)
			pBinding = new Binding(externalName, obj, internalName);
		}
		else
		{
			//we simply add the new bound object
			pBinding->addBoundObject(new BoundObject<T>(obj, internalName));
		}
		m_bindings.push_back(pBinding);
	}

	//These methods should be called from outside the renderer
	size_t getNumBindings();
	string getBindingExternalName(unsigned int i);
	bool updateBinding(unsigned int i, double value);
	bool updateBinding(string bindingExternalName, double value);
};

