#include "stdafx.h"
#include "renderer.h"
#include "scene-actor-3d.h"
#include "texture-manager.h"
#include "graphic-object-3d.h"
#include "graphic-object-2d.h"
#include "camera.h"
#include "light.h"
#include "xml-load.h"
#include "../GeometryLib/bounding-box.h"
#include <algorithm>
#include <iostream>

using namespace tinyxml2;
using namespace std;

Renderer* Renderer::m_pInstance = 0;

Renderer::Renderer()
{
	m_pInstance = this;
	m_timer.start();

	m_pTextureManager = new TextureManager();
	m_pActiveCamera = 0;
}


Renderer::~Renderer()
{
	//delete here all the objects handled by the renderer. Viewports have references to them, but they don't own them
	for (auto obj : m_3DgraphicObjects) delete obj;
	for (auto obj : m_2DgraphicObjects) delete obj;
	for (auto camera : m_cameras) delete camera;
	for (auto light : m_lights) delete light;
	for (auto viewport : m_viewPorts) delete viewport;
	delete m_pTextureManager;
	m_pInstance = nullptr;
}

void Renderer::setDataFolder(string dataFolder)
{
	m_dataFolder = dataFolder; m_pTextureManager->setFolder(dataFolder); 
}

string Renderer::getDataFolder()
{
	return m_dataFolder;
}


void Renderer::_drawViewPorts()
{
	Renderer::get()->drawViewPorts();
}

void Renderer::_reshapeWindow(int width, int height)
{
	Renderer::get()->reshapeWindow(width, height);
}

void Renderer::init(int argc, char** argv, int screenWidth, int screenHeight)
{
	//init window and OpenGL context
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	glutInitWindowSize(screenWidth, screenHeight);
	glutCreateWindow(argv[0]);

	//set the size of the window before creating the viewport
	m_windowWidth = screenWidth;
	m_windowHeight = screenHeight;

	//create the default viewport that covers the whole screen (normalized coordinates are used)
	//this default viewport can be resized (by the user) if more viewports are to be used
	m_pDefaultViewPort = new ViewPort(0, 0, 1.0, 1.0);
	m_viewPorts.push_back(m_pDefaultViewPort);

	//callback functions
	glutDisplayFunc(_drawViewPorts);
	glutReshapeFunc(_reshapeWindow);

	glEnable(GL_DEPTH_TEST);

	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	glClearColor(1.0, 1.0, 1.0, 1.0);
}

Renderer* Renderer::get()
{
	return m_pInstance;
}

void Renderer::addGraphicObject(GraphicObject3D* pObj, ViewPort* pViewPort)
{
	if (pViewPort)
	{
		//if a viewport is given, we add the object to it
		pViewPort->addGraphicObject3D(pObj);
		//since we are using viewports, the object might have been added to another viewport previously, so we only
		//add it to the global list in case it's not there yet
		if (std::find(m_3DgraphicObjects.begin(), m_3DgraphicObjects.end(), pObj) == m_3DgraphicObjects.end())
			m_3DgraphicObjects.push_back(pObj);
	}
	else
	{
		//if no viewport is given, we just assume the default viewport is being used. No need to check if it's already there
		m_pDefaultViewPort->addGraphicObject3D(pObj);
		m_3DgraphicObjects.push_back(pObj);
	}
}

void Renderer::add2DGraphicObject(GraphicObject2D* pObj, ViewPort* pViewPort)
{
	if (pViewPort)
	{
		pViewPort->addGraphicObject2D(pObj);
		if (std::find(m_2DgraphicObjects.begin(), m_2DgraphicObjects.end(), pObj) == m_2DgraphicObjects.end())
			m_2DgraphicObjects.push_back(pObj);
	}
	else
	{
		m_pDefaultViewPort->addGraphicObject2D(pObj);
		m_2DgraphicObjects.push_back(pObj);
	}
}

void Renderer::addLight(Light* pLight, ViewPort* pViewPort)
{
	if (pViewPort)
	{
		pViewPort->addLight(pLight);
		if (std::find(m_lights.begin(),m_lights.end(),pLight) == m_lights.end())
			m_lights.push_back(pLight);
	}
	else
	{
		m_pDefaultViewPort->addLight(pLight);
		m_lights.push_back(pLight);
	}
}

void Renderer::addCamera(Camera* pCamera, ViewPort* pViewPort)
{
	if (pViewPort)
	{
		pViewPort->setActiveCamera(pCamera);
		if (std::find(m_cameras.begin(), m_cameras.end(), pCamera) == m_cameras.end())
			m_cameras.push_back(pCamera);
	}
	else
	{
		m_pDefaultViewPort->setActiveCamera(pCamera);
		m_cameras.push_back(pCamera);
	}
}

ViewPort* Renderer::addViewPort(double minNormX, double minNormY, double maxNormX, double maxNormY)
{
	ViewPort* pViewPort = new ViewPort(minNormX, minNormY, maxNormX, maxNormY);
	m_viewPorts.push_back(pViewPort);
	return pViewPort;
}

TextureManager* Renderer::getTextureManager()
{
	return m_pTextureManager;
}

Camera* Renderer::getActiveCamera()
{
	return m_pActiveCamera;
}

GraphicObject3D* Renderer::get3DObjectByName(string name)
{
	for (GraphicObject3D* object : m_3DgraphicObjects)
	{
		if (object->name() == name)
		return object;
	}
	return nullptr;
}

GraphicObject2D* Renderer::get2DObjectByName(string name)
{
	for (GraphicObject2D* object : m_2DgraphicObjects)
	{
		if (object->name() == name)
			return object;
	}
	return nullptr;
}

void Renderer::logMessage(string message)
{
	if (m_bVerbose)
		cout << message << "\n";
}

void Renderer::loadScene(const char* file)
{
	tinyxml2::XMLDocument doc;
	string fullFilename = m_dataFolder + string(file);
	doc.LoadFile(fullFilename.c_str());
	logMessage(string("Opening scene file: ") + fullFilename);
	if (doc.Error() == tinyxml2::XML_SUCCESS)
	{
		logMessage("Success");
		tinyxml2::XMLElement *pRoot = doc.FirstChildElement(XML_TAG_SCENE);
		if (pRoot)
			loadSceneObjects(pRoot);
	}
	else logMessage("ERROR: File not found");
}
void Renderer::loadSceneObjects(tinyxml2::XMLElement* pNode)
{
	//scene attributes
	Color backgroundColor;
	tinyxml2::XMLElement* pBgrColor = pNode->FirstChildElement(XML_TAG_BACKGROUND_COLOR);
	if (pBgrColor)
	{
		XML::load(pBgrColor, backgroundColor);
		glClearColor(backgroundColor.r(), backgroundColor.g(), backgroundColor.b(),1.0);
	}

	//import scene files
	string scenePath;
	tinyxml2::XMLElement* pImportedScene = pNode->FirstChildElement(XML_TAG_IMPORT_SCENE);
	while (pImportedScene != nullptr)
	{
		scenePath = pImportedScene->Attribute(XML_TAG_PATH);
		loadScene(scenePath.c_str());

		pImportedScene = pImportedScene->NextSiblingElement(XML_TAG_IMPORT_SCENE);
	}

	//3d graphic objects
	tinyxml2::XMLElement* pObjects = pNode->FirstChildElement(XML_TAG_OBJECTS);
	if (pObjects)
		loadChildren<GraphicObject3D>(pObjects, nullptr, m_3DgraphicObjects);
	//we need to add loaded objects to the default viewport
	for (GraphicObject3D* obj : m_3DgraphicObjects)
	{
		logMessage(string("Loading 3D object: ") + obj->name());
		m_pDefaultViewPort->addGraphicObject3D(obj);
	}

	//2d graphic objects
	pObjects = pNode->FirstChildElement(XML_TAG_OBJECTS_2D);
	if (pObjects)
		loadChildren<GraphicObject2D>(pObjects, nullptr, m_2DgraphicObjects);
	for (GraphicObject2D* obj : m_2DgraphicObjects)
	{
		logMessage(string("Loading 2D object: ") + obj->name());
		m_pDefaultViewPort->addGraphicObject2D(obj);
	}

	//cameras
	loadChildren<Camera>(pNode, XML_TAG_CAMERA, m_cameras);
	if (m_cameras.size() > 0)
		m_pActiveCamera = m_cameras[0];
	else
	{
		m_pActiveCamera = new SimpleCamera();
		printf("Warning: No camera defined in the scene file. Default camera created\n");
	}
	m_pDefaultViewPort->setActiveCamera(m_pActiveCamera);

	//lights
	loadChildren<Light>(pNode, XML_TAG_LIGHT, m_lights);
	if (m_lights.size() == 0)
	{
		Light* pLight= new DirectionalLight();
		m_lights.push_back(pLight);
		printf("Warning: No light defined in the scene file. Default directional light created\n");
	}
	m_pDefaultViewPort->addLights(m_lights);
}

void Renderer::draw()
{
	glutPostRedisplay();
	glutSwapBuffers();
	updateFPS();
}

void Renderer::updateFPS()
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

void Renderer::drawViewPorts()
{
	//if no camera has been defined, create a basic one
	if (m_pActiveCamera == nullptr)
	{
		m_pActiveCamera = new SimpleCamera();
		m_cameras.push_back(m_pActiveCamera);
	}

	//clean the backbuffer
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	m_num3DObjectsDrawn = 0;
	for (ViewPort* pViewPort : m_viewPorts)
	{
		m_num3DObjectsDrawn+= pViewPort->draw();
	}
}

void Renderer::reshapeWindow(int w,int h)
{
	m_windowWidth = w;
	m_windowHeight = h;
	//glViewport(0, 0, (GLsizei)w, (GLsizei)h); //this is now set by each viewport
}

Binding* Renderer::getBinding(string externalName)
{
	for (Binding* binding : m_bindings)
	{
		if (binding->externalName == externalName)
			return binding;
	}
	return nullptr;
}


size_t Renderer::getNumBindings()
{
	return m_bindings.size();
}

string Renderer::getBindingExternalName(unsigned int i)
{
	if (i >= 0 && i < (int)m_bindings.size())
		return m_bindings[i]->externalName;
	return string("N/A");
}

bool Renderer::updateBinding(unsigned int i, double value)
{
	//update the value if the binding's index is in range
	if (i >= 0 && i < m_bindings.size())
	{
		m_bindings[i]->update(value);

		return true;
	}
	return false;
}

bool Renderer::updateBinding(string bindingExternalName, double value)
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

void Renderer::drawBoundingBoxes(bool enable, ViewPort* pViewPort)
{
	//if no viewport is given, we just update all the viewports
	if (pViewPort)
		pViewPort->drawBoundingBoxes(enable);
	else
		for (ViewPort* pViewPort : m_viewPorts)
			pViewPort->drawBoundingBoxes(enable);
}
