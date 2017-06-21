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
using namespace std;


class CRenderer
{
	CTimer m_timer;
	int m_frameCount = 0;
	double m_fps = 0.0;

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

	Binding* getBinding(string externalName);

	bool m_bShowBoundingBoxes= false;
public:
	CRenderer();
	virtual ~CRenderer();

	void getWindowsSize(int& sizeX, int& sizeY) { sizeX = m_windowWidth; sizeY = m_windowHeight; }
	double getFPS() { return m_fps; }
	void updateFPS();

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
	void registerBinding(string externalName, Bindable* pObj, string internalName);

	//These methods should be called from outside the renderer
	int getNumBindings();
	string getBindingExternalName(unsigned int i);
	bool updateBinding(unsigned int i, double value);
	bool updateBinding(string bindingExternalName, double value);
};

