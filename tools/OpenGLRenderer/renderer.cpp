#include "stdafx.h"
#include "renderer.h"
#include "actor.h"
#include "texture-manager.h"
#include "graphic-object.h"
#include "graphic-object-2d.h"
#include "camera.h"
#include "light.h"
#include "xml-load.h"
#include "../GeometryLib/bounding-box.h"

using namespace tinyxml2;

CRenderer* CRenderer::m_pInstance = 0;

CRenderer::CRenderer()
{
	m_pInstance = this;
	m_timer.start();

	m_pTextureManager = new CTextureManager();
	m_pActiveCamera = 0;
}

CRenderer::~CRenderer()
{
	//delete 3d objects
	for (auto it = m_3DgraphicObjects.begin(); it != m_3DgraphicObjects.end(); ++it) delete *it;
	//delete 2d objects
	for (auto it = m_2DgraphicObjects.begin(); it != m_2DgraphicObjects.end(); ++it) delete *it;
	for (auto it = m_cameras.begin(); it != m_cameras.end(); ++it) delete *it;
	for (auto it = m_lights.begin(); it != m_lights.end(); ++it) delete *it;
	if (m_pTextureManager) delete m_pTextureManager;
}

void CRenderer::setDataFolder(string dataFolder)
{
	m_dataFolder = dataFolder; m_pTextureManager->setFolder(dataFolder); 
}

string CRenderer::getDataFolder()
{
	return m_dataFolder;
}


void CRenderer::_drawScene()
{
	CRenderer::get()->drawScene();
}

void CRenderer::_reshapeWindow(int width, int height)
{
	CRenderer::get()->reshapeWindow(width, height);
}

void CRenderer::init(int argc, char** argv, int screenWidth, int screenHeight)
{
	//init window and OpenGL context
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	glutInitWindowSize(screenWidth, screenHeight);
	glutCreateWindow(argv[0]);

	//callback functions
	glutDisplayFunc(_drawScene);
	glutReshapeFunc(_reshapeWindow);

	m_windowWidth = screenWidth;
	m_windowHeight = screenHeight;

	glEnable(GL_DEPTH_TEST);

	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	glClearColor(1.0, 1.0, 1.0, 1.0);
}

CRenderer* CRenderer::get()
{
	return m_pInstance;
}

void CRenderer::addGraphicObject(CGraphicObject* pObj)
{
	m_3DgraphicObjects.push_back(pObj);
}

void CRenderer::add2DGraphicObject(CGraphicObject2D* pObj)
{
	m_2DgraphicObjects.push_back(pObj);
}

CTextureManager* CRenderer::getTextureManager()
{
	return m_pTextureManager;
}

CCamera* CRenderer::getActiveCamera()
{
	return m_pActiveCamera;
}

CGraphicObject* CRenderer::getObjectByName(string name)
{
	for (auto it = m_3DgraphicObjects.begin(); it != m_3DgraphicObjects.end(); ++it)
	{
		if ((*it)->name() == name)
		return (*it);
	}
	return nullptr;
}

void CRenderer::loadScene(const char* file)
{
	tinyxml2::XMLDocument doc;
	doc.LoadFile((m_dataFolder + string(file)).c_str());
	if (doc.Error() == tinyxml2::XML_SUCCESS)
	{
		tinyxml2::XMLElement *pRoot = doc.FirstChildElement(XML_TAG_SCENE);
		if (pRoot)
			loadSceneObjects(pRoot);
	}
}
void CRenderer::loadSceneObjects(tinyxml2::XMLElement* pNode)
{
	//import scene files
	string scenePath;
	tinyxml2::XMLElement* pImportedScene = pNode->FirstChildElement(XML_TAG_IMPORT_SCENE);
	while (pImportedScene != nullptr)
	{
		scenePath = pImportedScene->Attribute(XML_TAG_PATH);
		loadScene(scenePath.c_str());
		pImportedScene = pImportedScene->NextSiblingElement(XML_TAG_IMPORT_SCENE);
	}

		//graphic objects
	loadChildren<CGraphicObject>
			(pNode->FirstChildElement(XML_TAG_OBJECTS), nullptr, m_3DgraphicObjects);

	//cameras
	loadChildren<CCamera>(pNode, XML_TAG_CAMERA, m_cameras);
	if (m_cameras.size() > 0)
		m_pActiveCamera = m_cameras[0];
	else
	{
		m_pActiveCamera = new CSimpleCamera();
		m_cameras.push_back(m_pActiveCamera);
		printf("Warning: No camera defined. Default camera created\n");
	}
	//lights
	loadChildren<CLight>(pNode, XML_TAG_LIGHT, m_lights);
	if (m_lights.size() == 0)
	{
		printf("Warning: No light defined. Default directional light created\n");
		m_lights.push_back(new CDirectionalLight());
	}
}

void CRenderer::redraw()
{
	glutPostRedisplay();
	glutSwapBuffers();
	updateFPS();
}

void CRenderer::updateFPS()
{
	//update FPS
	++m_frameCount;
	double time = m_timer.getElapsedTime();
	if (time > 1.0) //update it every second
	{
		m_fps = ((double)m_frameCount) / time;
		m_frameCount = 0; //reset the frame counter;
		m_timer.start(); //reset the timer
	}
}

void CRenderer::drawScene()
{
	//clean the backbuffer
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	if (!m_pActiveCamera) return;
	//set 3d view
	m_pActiveCamera->set();
	//set lights
	CLight::enable(true);

	for (auto it = m_lights.begin(); it != m_lights.end(); ++it)
	{
		(*it)->set();
	}
	//draw 3d objects in the scene
	Frustum& frustum = m_pActiveCamera->getFrustum();
	m_num3DObjectsDrawn = 0;

	for (auto it = m_3DgraphicObjects.begin(); it != m_3DgraphicObjects.end(); ++it)
	{
		if (frustum.isVisible((*it)->boundingBox()))
		{
			(*it)->draw();

			if (m_bShowBoundingBoxes)
			{
				(*it)->setTransform();
				drawBoundingBox3D((*it)->boundingBox());
				(*it)->restoreTransform();
			}
			++m_num3DObjectsDrawn;
		}
	}
	CLight::enable(false);
	//set 2d view
	if (m_pActiveCamera) m_pActiveCamera->set2DView();
	//draw 2d objects
	for (auto it = m_2DgraphicObjects.begin(); it != m_2DgraphicObjects.end(); ++it)
	{
		(*it)->draw();

		if (m_bShowBoundingBoxes)
			drawBoundingBox2D((*it)->boundingBox());
	}
}

void CRenderer::reshapeWindow(int w,int h)
{
	m_windowWidth = w;
	m_windowHeight = h;
	glViewport(0, 0, (GLsizei)w, (GLsizei)h);
}

Binding* CRenderer::getBinding(string externalName)
{
	for (auto it = m_bindings.begin(); it != m_bindings.end(); ++it)
	{
		if ((*it)->externalName == externalName)
			return *it;
	}
	return nullptr;
}

void CRenderer::registerBinding(string externalName, Bindable* pObj, string internalName)
{
	Binding* pBinding = getBinding(externalName);
	if (pBinding == nullptr)
	{
		//No binding registered yet for the external name (i.e, the state variable's name)
		pBinding = new Binding(externalName, pObj, internalName);
	}
	else
	{
		//we simply add the new bound object
		pBinding->addBoundObject(new BoundObject(pObj, internalName));
	}
	m_bindings.push_back(pBinding);
}

size_t CRenderer::getNumBindings()
{
	return m_bindings.size();
}

string CRenderer::getBindingExternalName(unsigned int i)
{
	if (i >= 0 && i < (int)m_bindings.size())
		return m_bindings[i]->externalName;
	return string("");
}

bool CRenderer::updateBinding(unsigned int i, double value)
{
	BoundObject* pBoundObj;

	//update the value if the binding's index is in range
	if (i >= 0 && i < m_bindings.size())
	{
		for (unsigned int obj = 0; obj < m_bindings[i]->getNumBoundObjects(); ++obj)
		{
			pBoundObj = m_bindings[i]->getBoundObject(obj);
			pBoundObj->pObj->update(pBoundObj->internalName, value);
		}
		return true;
	}
	return false;
}

bool CRenderer::updateBinding(string bindingExternalName, double value)
{
	int i = 0;
	for (auto it = m_bindings.begin(); it != m_bindings.end(); ++it)
	{
		if ((*it)->externalName == bindingExternalName)
			return updateBinding(i, value);

		++i;
	}
	return false;
}


void CRenderer::drawBoundingBox3D(BoundingBox3D& box) const
{
	glBegin(GL_LINES);
	//FRONT
	glVertex3d(box.min().x(), box.min().y(), box.max().z());
	glVertex3d(box.max().x(), box.min().y(), box.max().z());
	glVertex3d(box.max().x(), box.min().y(), box.max().z());
	glVertex3d(box.max().x(), box.max().y(), box.max().z());
	glVertex3d(box.max().x(), box.max().y(), box.max().z());
	glVertex3d(box.min().x(), box.max().y(), box.max().z());
	glVertex3d(box.min().x(), box.max().y(), box.max().z());
	glVertex3d(box.min().x(), box.min().y(), box.max().z());
	//BACK
	glVertex3d(box.min().x(), box.min().y(), box.min().z());
	glVertex3d(box.max().x(), box.min().y(), box.min().z());
	glVertex3d(box.max().x(), box.min().y(), box.min().z());
	glVertex3d(box.max().x(), box.max().y(), box.min().z());
	glVertex3d(box.max().x(), box.max().y(), box.min().z());
	glVertex3d(box.min().x(), box.max().y(), box.min().z());
	glVertex3d(box.min().x(), box.max().y(), box.min().z());
	glVertex3d(box.min().x(), box.min().y(), box.min().z());
	//4 lines between front face and back face
	glVertex3d(box.min().x(), box.min().y(), box.min().z());
	glVertex3d(box.min().x(), box.min().y(), box.max().z());
	glVertex3d(box.max().x(), box.min().y(), box.min().z());
	glVertex3d(box.max().x(), box.min().y(), box.max().z());
	glVertex3d(box.max().x(), box.max().y(), box.min().z());
	glVertex3d(box.max().x(), box.max().y(), box.max().z());
	glVertex3d(box.min().x(), box.max().y(), box.min().z());
	glVertex3d(box.min().x(), box.max().y(), box.max().z());
	glEnd();
}

void CRenderer::drawBoundingBox2D(BoundingBox2D& box) const
{
	glBegin(GL_LINES);
	glVertex2d(box.min().x(), box.min().y()); glVertex2d(box.max().x(), box.min().y());
	glVertex2d(box.max().x(), box.min().y()); glVertex2d(box.max().x(), box.max().y());
	glVertex2d(box.max().x(), box.max().y()); glVertex2d(box.min().x(), box.max().y());
	glVertex2d(box.min().x(), box.max().y()); glVertex2d(box.min().x(), box.min().y());
	glEnd();
}