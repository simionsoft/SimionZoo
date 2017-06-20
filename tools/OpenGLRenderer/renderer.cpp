#include "stdafx.h"
#include "renderer.h"
#include "actor.h"
#include "texture-manager.h"
#include "graphic-object.h"
#include "graphic-object-2d.h"
#include "camera.h"
#include "light.h"
#include "xml-load-utils.h"

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
	glutInitWindowSize(600, 400);
	glutCreateWindow("SimionRenderer");
	//glutFullScreen();

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
	doc.LoadFile((m_dataFolder+string(file)).c_str());
	if (doc.Error() == tinyxml2::XML_SUCCESS)
	{
		tinyxml2::XMLElement *pRoot = doc.FirstChildElement(XML_TAG_SCENE);
		if (pRoot)
			loadChildren<CGraphicObject>
			(pRoot->FirstChildElement(XML_TAG_OBJECTS), nullptr, m_3DgraphicObjects);

		//cameras
		loadChildren<CCamera>(pRoot, XML_TAG_CAMERA, m_cameras);
		if (m_cameras.size() > 0)
			m_pActiveCamera = m_cameras[0];
		else
		{
			m_pActiveCamera = new CSimpleCamera();
			m_cameras.push_back(m_pActiveCamera);
			printf("Warning: No camera defined. Default camera created\n");
		}
		//lights
		loadChildren<CLight>(pRoot, XML_TAG_LIGHT, m_lights);
		if (m_lights.size() == 0)
		{
			printf("Warning: No light defined. Default directional light created\n");
			m_lights.push_back(new CDirectionalLight());
		}
	}
}

void CRenderer::redraw()
{
	glutPostRedisplay();
	glutSwapBuffers();

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

	//set 3d view
	if (m_pActiveCamera) m_pActiveCamera->set();
	//set lights
	CLight::enable(true);

	for (auto it = m_lights.begin(); it != m_lights.end(); ++it)
	{
		(*it)->set();
	}
	//draw 3d objects in the scene
	for (auto it = m_3DgraphicObjects.begin(); it != m_3DgraphicObjects.end(); ++it)
	{
		(*it)->draw();
#ifdef _DEBUG
		(*it)->drawBoundingBox();
#endif

	}
	CLight::enable(false);
	//set 2d view
	if (m_pActiveCamera) m_pActiveCamera->set2DView();
	//draw 2d objects
	for (auto it = m_2DgraphicObjects.begin(); it != m_2DgraphicObjects.end(); ++it)
	{
		(*it)->draw();
#ifdef _DEBUG
		(*it)->boundingBox().draw();
#endif
	}
}

void CRenderer::reshapeWindow(int w,int h)
{
	m_windowWidth = w;
	m_windowHeight = h;
	glViewport(0, 0, (GLsizei)w, (GLsizei)h);
}

void CRenderer::registerBinding(Binding* pBinding)
{
	m_bindings.push_back(pBinding);
	//printf("Binding registered: %s -> %s\n", m_bindings[m_bindings.size()-1]->externalName.c_str()
	//	, m_bindings[m_bindings.size() - 1]->internalName.c_str());
}

int CRenderer::getNumBindings()
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
	//update the value if the binding's index is in range
	if (i >= 0 && i < m_bindings.size())
	{
		m_bindings[i]->pObj->update(m_bindings[i]->internalName, value);
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