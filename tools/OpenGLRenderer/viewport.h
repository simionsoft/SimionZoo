#pragma once

#include <vector>
using namespace std;
class Camera;
class Light;
class GraphicObject3D;
class GraphicObject2D;

class ViewPort
{
	friend class Renderer;

	//absolute coordinates in pixels: [0,0] is the bottom-left corner
	unsigned int m_minScreenX, m_maxScreenX;
	unsigned int m_minScreenY, m_maxScreenY;

	Camera* m_pActiveCamera = nullptr;
	vector <Light*> m_lights;

	vector<GraphicObject3D*> m_3DgraphicObjects;
	vector<GraphicObject2D*> m_2DgraphicObjects;

	void drawAxes();
	bool m_bDrawBoundingBoxes = false;
public:

	//all interfaces use normalized coordinates in range [0,0] - [1,1]
	ViewPort(double minNormScreenX, double minNormScreenY, double maxNormScreenX, double maxNormScreenY);
	virtual ~ViewPort();

	void resize(double minNormScreenX, double minNormScreenY, double maxNormScreenX, double maxNormScreenY);
protected:
	void addGraphicObject3D(GraphicObject3D* pObj) { m_3DgraphicObjects.push_back(pObj); }
	void addGraphicObject2D(GraphicObject2D* pObj) { m_2DgraphicObjects.push_back(pObj); }
	void addLight(Light* pLight) { m_lights.push_back(pLight); }
	void addLights(vector<Light*>& pLights) { m_lights.insert(m_lights.end(), pLights.begin(), pLights.end()); }
	void setActiveCamera(Camera* pCamera) { m_pActiveCamera = pCamera; }
	int draw();

	void drawBoundingBoxes(bool enable) { m_bDrawBoundingBoxes = enable; }
};

