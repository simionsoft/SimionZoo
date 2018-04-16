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

using namespace tinyxml2;

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
	//delete 3d objects
	for (auto it = m_3DgraphicObjects.begin(); it != m_3DgraphicObjects.end(); ++it) delete *it;
	//delete 2d objects
	for (auto it = m_2DgraphicObjects.begin(); it != m_2DgraphicObjects.end(); ++it) delete *it;
	for (auto it = m_cameras.begin(); it != m_cameras.end(); ++it) delete *it;
	for (auto it = m_lights.begin(); it != m_lights.end(); ++it) delete *it;
	if (m_pTextureManager) delete m_pTextureManager;
}

void Renderer::setDataFolder(string dataFolder)
{
	m_dataFolder = dataFolder; m_pTextureManager->setFolder(dataFolder); 
}

string Renderer::getDataFolder()
{
	return m_dataFolder;
}


void Renderer::_drawScene()
{
	Renderer::get()->drawScene();
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

Renderer* Renderer::get()
{
	return m_pInstance;
}

void Renderer::addGraphicObject(GraphicObject3D* pObj)
{
	m_3DgraphicObjects.push_back(pObj);
}

void Renderer::add2DGraphicObject(GraphicObject2D* pObj)
{
	m_2DgraphicObjects.push_back(pObj);
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
	for each (GraphicObject3D* object in m_3DgraphicObjects)
	{
		if (object->name() == name)
		return object;
	}
	return nullptr;
}

GraphicObject2D* Renderer::get2DObjectByName(string name)
{
	for each (GraphicObject2D* object in m_2DgraphicObjects)
	{
		if (object->name() == name)
			return object;
	}
	return nullptr;
}


void Renderer::loadScene(const char* file)
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

	//2d graphic objects
	pObjects = pNode->FirstChildElement(XML_TAG_OBJECTS_2D);
	if (pObjects)
		loadChildren<GraphicObject2D>(pObjects, nullptr, m_2DgraphicObjects);

	//cameras
	loadChildren<Camera>(pNode, XML_TAG_CAMERA, m_cameras);
	if (m_cameras.size() > 0)
		m_pActiveCamera = m_cameras[0];
	else
	{
		m_pActiveCamera = new SimpleCamera();
		m_cameras.push_back(m_pActiveCamera);
		printf("Warning: No camera defined. Default camera created\n");
	}
	//lights
	loadChildren<Light>(pNode, XML_TAG_LIGHT, m_lights);
	if (m_lights.size() == 0)
	{
		printf("Warning: No light defined. Default directional light created\n");
		m_lights.push_back(new DirectionalLight());
	}
}

void Renderer::redraw()
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

void Renderer::drawScene()
{
	//clean the backbuffer
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	if (!m_pActiveCamera)
	{
		m_pActiveCamera = new SimpleCamera();
		m_cameras.push_back(m_pActiveCamera);
	}
	//set 3d view
	m_pActiveCamera->set();
	//set lights
	Light::enable(true);

	for (auto it = m_lights.begin(); it != m_lights.end(); ++it)
	{
		(*it)->set();
	}
	//draw 3d objects in the scene
	Frustum& frustum = m_pActiveCamera->getFrustum();
	m_num3DObjectsDrawn = 0;

	for each(auto object in m_3DgraphicObjects)
	{
		if (frustum.isVisible(object->boundingBox()))
		{
			object->draw();

			if (m_bShowBoundingBoxes)
			{
				object->setTransform();
				drawBoundingBox3D(object->boundingBox());
				object->restoreTransform();
			}
			++m_num3DObjectsDrawn;
		}
	}
	Light::enable(false);
	//set 2d view
	if (m_pActiveCamera) m_pActiveCamera->set2DView();
	//draw 2d objects
	for each (auto object in m_2DgraphicObjects)
	{
		object->setTransform();

		object->draw();

		if (m_bShowBoundingBoxes)
			drawBoundingBox2D(object->boundingBox());

		object->restoreTransform();
	}
}

void Renderer::reshapeWindow(int w,int h)
{
	m_windowWidth = w;
	m_windowHeight = h;
	glViewport(0, 0, (GLsizei)w, (GLsizei)h);
}

Binding* Renderer::getBinding(string externalName)
{
	for each (Binding* binding in m_bindings)
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


void Renderer::drawBoundingBox3D(BoundingBox3D& box) const
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

void Renderer::drawBoundingBox2D(BoundingBox2D& box) const
{
	glBegin(GL_LINES);
	glVertex2d(box.min().x(), box.min().y()); glVertex2d(box.max().x(), box.min().y());
	glVertex2d(box.max().x(), box.min().y()); glVertex2d(box.max().x(), box.max().y());
	glVertex2d(box.max().x(), box.max().y()); glVertex2d(box.min().x(), box.max().y());
	glVertex2d(box.min().x(), box.max().y()); glVertex2d(box.min().x(), box.min().y());
	glEnd();
}