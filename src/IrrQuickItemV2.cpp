#include "IrrQuickItemV2.h"
#include <QQuickWindow>
#include <irrlicht.h>
#include <QSGNode>
#include "CQGLFunctionsDriver.h"

using namespace scene;
using namespace gui;
using namespace video;
using namespace core;

IrrQuickItem::IrrQuickItem(QQuickItem *parent)
    : QQuickItem(parent)
    , m_t(0)
    , m_renderer(nullptr)
{
//	setFlag( ItemHasContents, true );
//	setAcceptedMouseButtons( Qt::AllButtons );
//	setAcceptHoverEvents( true );

	connect( this, &QQuickItem::windowChanged, this, &IrrQuickItem::handleWindowChanged );
}

void IrrQuickItem::setT(qreal t)
{
	if( t == m_t )
		return;
	m_t = t;
	emit tChanged();
	if(window())
		window()->update();
}

void IrrQuickItem::loadExample(int index)
{
	if(m_renderer)
		m_renderer->loadExample(index);
}

void IrrQuickItem::sync()
{
	if(!m_renderer)
	{
		m_renderer = new GLRenderer();
		connect( window(), &QQuickWindow::beforeRendering, m_renderer, &GLRenderer::paint, Qt::DirectConnection );
	}
	m_renderer->setViewportSize( window()->size() * window()->devicePixelRatio() );
	m_renderer->setT(m_t);
	m_renderer->setWindow( window() );
}

void IrrQuickItem::cleanup()
{
	if(m_renderer)
	{
		delete m_renderer;
		m_renderer = nullptr;
	}
}

void IrrQuickItem::handleWindowChanged(QQuickWindow *window)
{
	if(window)
	{
		connect(window, &QQuickWindow::beforeSynchronizing, this, &IrrQuickItem::sync, Qt::DirectConnection );
		connect(window, &QQuickWindow::sceneGraphInvalidated, this, &IrrQuickItem::cleanup, Qt::DirectConnection );
		window->setClearBeforeRendering(false);
		window->setPersistentSceneGraph(true);
		window->setPersistentOpenGLContext(true);
	}
}

//QSGNode *IrrQuickItem::updatePaintNode(QSGNode *oldNode, QQuickItem::UpdatePaintNodeData *updatePaintNodeData)
//{
//	QSGNode *node = QQuickItem::updatePaintNode(oldNode, updatePaintNodeData);
//	if(!node)
//		node = new QSGNode();
//	node->markDirty(QSGNode::DirtyForceUpdate);
//	return node;
//}

void IrrQuickItem::touchEvent(QTouchEvent *event)
{
	SEvent e;
	e.EventType = EET_TOUCH_INPUT_EVENT;
	switch(event->touchPointStates())
	{
	case Qt::TouchPointPressed:
		e.TouchInput.Event = ETIE_PRESSED_DOWN;
		break;
	case Qt::TouchPointMoved:
		e.TouchInput.Event = ETIE_MOVED;
		break;
	case Qt::TouchPointReleased:
		e.TouchInput.Event = ETIE_LEFT_UP;
		break;
	}
	e.TouchInput.ID = event->touchPoints().first().id();
	e.TouchInput.X = event->touchPoints().first().pos().x() * 200;
	e.TouchInput.Y = event->touchPoints().first().pos().y() * 200;
	m_renderer->m_device->postEventFromUser(e);
}

void IrrQuickItem::mousePressEvent(QMouseEvent *e)
{
	m_pressPos = e->localPos();
}

void IrrQuickItem::mouseReleaseEvent(QMouseEvent *e)
{

}

void IrrQuickItem::keyPressEvent(QKeyEvent *e)
{
	sendKeyEventToIrrlicht( e, true );
}

void IrrQuickItem::keyReleaseEvent(QKeyEvent *e)
{
	sendKeyEventToIrrlicht( e, false );
}

struct SIrrlichtKey
{
	irr::EKEY_CODE code;
	wchar_t ch;
};

SIrrlichtKey convertToIrrlichtKey( int key )
{
	SIrrlichtKey irrKey;
	irrKey.code = (irr::EKEY_CODE)(0);
	irrKey.ch = (wchar_t)(0);

	// Letters A..Z and numbers 0..9 are mapped directly
	if ( (key >= Qt::Key_A && key <= Qt::Key_Z) || (key >= Qt::Key_0 && key <= Qt::Key_9) )
	{
		irrKey.code = (irr::EKEY_CODE)( key );
		irrKey.ch = (wchar_t)( key );
	}
	else

		// Dang, map keys individually
		switch( key )
		{
		case Qt::Key_Up:
			irrKey.code = irr::KEY_UP;
			break;

		case Qt::Key_Down:
			irrKey.code = irr::KEY_DOWN;
			break;

		case Qt::Key_Left:
			irrKey.code = irr::KEY_LEFT;
			break;

		case Qt::Key_Right:
			irrKey.code = irr::KEY_RIGHT;
			break;
		}
	return irrKey;
}

void IrrQuickItem::sendKeyEventToIrrlicht(QKeyEvent *event, bool pressedDown)
{
	irr::SEvent irrEvent;

	irrEvent.EventType = irr::EET_KEY_INPUT_EVENT;

	SIrrlichtKey irrKey = convertToIrrlichtKey( event->key() );

	if ( irrKey.code == 0 ) return; // Could not find a match for this key

	irrEvent.KeyInput.Key = irrKey.code;
	irrEvent.KeyInput.Control = ((event->modifiers() & Qt::ControlModifier) != 0);
	irrEvent.KeyInput.Shift = ((event->modifiers() & Qt::ShiftModifier) != 0);
	irrEvent.KeyInput.Char = irrKey.ch;
	irrEvent.KeyInput.PressedDown = pressedDown;

	if(m_renderer && m_renderer->m_device)
		m_renderer->m_device->postEventFromUser( irrEvent );
}



struct VertexData
{
	QVector3D position;
	QVector2D texCoord;
};

GLRenderer::GLRenderer(QObject *parent)
    : QObject(parent)
    , m_t(0)
    , m_device(nullptr)
    , m_driver(nullptr)
    , m_scene(nullptr)
{
	init = &GLRenderer::_first_init;
	//    m_rotation = 0;
}

GLRenderer::~GLRenderer()
{
	m_device->drop();
}

void GLRenderer::setViewportSize(const QSize &size)
{
	//	return;
	m_viewportSize = size;
	if(!m_device)
		return;
	if(m_device->getSceneManager()->getActiveCamera())
		m_device->getSceneManager()->getActiveCamera()->setAspectRatio((float)size.width()/(float)size.height());
}

void GLRenderer::createCube()
{
	if(!m_device)
		return;
	scene::ISceneManager *scene = m_device->getSceneManager();
	video::IVideoDriver *driver = m_device->getVideoDriver();
	scene::ISceneNode *cube = scene->addCubeSceneNode(2);
	io::path p = _MEDIA_PATH;
	p += "irrlicht2_lf.jpg";
	video::ITexture *texture = driver->getTexture(p);
	if(texture)
		cube->setMaterialTexture(0,texture);

	if(scene->getActiveCamera() == NULL)
	{
		scene::ICameraSceneNode *cam = scene->addCameraSceneNode();
		cam->setPosition( core::vector3df(-5,0,0) );
		cam->setTarget(cube->getPosition());
	}

	scene::ISceneNodeAnimator *anim = scene->createRotationAnimator( core::vector3df(1.0,1.0,0) );
	cube->addAnimator(anim);
	anim->drop();

	cube = scene->addBillboardSceneNode(NULL, core::dimension2df(3,3) );
	cube->setMaterialType(EMT_TRANSPARENT_ALPHA_CHANNEL);
	cube->setMaterialTexture(0,texture);
}

void GLRenderer::setCamera()
{

}

void GLRenderer::loadExample(int index)
{
	if(!clear_scene()) {
		init = &GLRenderer::_empty_init;
		return;
	}

	switch (index) {
	case 1:
		init = &GLRenderer::_load_example_1;
		break;
	case 2:
		init = &GLRenderer::_load_example_2;
		break;
	case 11:
		init = &GLRenderer::_load_example_11;
		break;
	case 16:
		init = &GLRenderer::_load_example_16;
		break;
	}

	createCube(); // this garantue of two differend materials in scene
	// some hack ?while error of shader program not fixed

}

void GLRenderer::_first_init()
{
	initializeOpenGLFunctions();
	irr::SIrrlichtCreationParameters params;

	params.DriverType = EDT_QOGLDUNCTIONS;//driverType;
	params.DeviceType = EIDT_CONSOLE;
	params.WindowId = NULL;
	params.Bits = 32;
	params.Doublebuffer = true;
	params.Fullscreen = false;
	params.IgnoreInput = false;
	params.Stencilbuffer = true;
	params.Stereobuffer = false;
	params.WindowSize.Width = m_window->geometry().width();
	params.WindowSize.Height = m_window->geometry().height();
	params.AntiAlias = 0;
	params.OGLES2ShaderPath = _MEDIA_PATH;
	params.OGLES2ShaderPath += "Shaders/";
	params.Vsync = false;
	params.qOpenGLFunctions = this;

	m_device = createDeviceEx(params);
	m_driver = (CQGLFunctionsDriver*)m_device->getVideoDriver();
	m_scene = m_device->getSceneManager();
	createCube();

	init = &GLRenderer::_empty_init;
}

bool GLRenderer::clear_scene()
{
	if(!m_device)
		return  false;
	m_device->getSceneManager()->clear();
	m_device->getGUIEnvironment()->clear();
	return true;
}

const io::path GLRenderer::getExampleMediaPath()
{
	io::path p = _MEDIA_PATH;
	return  p;
}

void GLRenderer::_load_example_1()
{
	/** begin example 1 code */
	IVideoDriver* driver = m_device->getVideoDriver();
	ISceneManager* smgr = m_device->getSceneManager();
	IGUIEnvironment* guienv = m_device->getGUIEnvironment();

	guienv->addStaticText(L"Hello World! This is Irrlicht with the burnings software renderer!",
	    rect<s32>(10,10,260,22), true);

	const io::path mediaPath =  getExampleMediaPath();

	IAnimatedMesh* mesh = smgr->getMesh(mediaPath + "sydney.md2");
	if (!mesh)
	{
		// TODO here need use signal of bad scene, for QML
		clear_scene();
		init = &GLRenderer::_empty_init;
		return;
	}
	IAnimatedMeshSceneNode* node = smgr->addAnimatedMeshSceneNode( mesh );

	if (node)
	{
		node->setMaterialFlag(EMF_LIGHTING, false);
		node->setMD2Animation(scene::EMAT_STAND);
		node->setMaterialTexture( 0, driver->getTexture(mediaPath + "sydney.bmp") );
	}

	smgr->addCameraSceneNode(0, vector3df(0,30,-40), vector3df(0,5,0));
	/** end example 1 */

	/** set epmty fuctions pointer */
	init = &GLRenderer::_empty_init;
}

void GLRenderer::_load_example_2()
{
	video::IVideoDriver* driver = m_device->getVideoDriver();
	scene::ISceneManager* smgr = m_device->getSceneManager();

	m_device->getFileSystem()->addFileArchive(getExampleMediaPath() + "map-20kdm2.pk3");
	scene::IAnimatedMesh* mesh = smgr->getMesh("20kdm2.bsp");
	scene::ISceneNode* node = 0;

	if (mesh)
		node = smgr->addOctreeSceneNode(mesh->getMesh(0), 0, -1, 1024);
//		node = smgr->addMeshSceneNode(mesh->getMesh(0));

	if (node)
		node->setPosition(core::vector3df(-1300,-144,-1249));

	smgr->addCameraSceneNodeFPS(0,1.0f);

	/*
	The mouse cursor needs not be visible, so we hide it via the
	irr::IrrlichtDevice::ICursorControl.
	*/
//	m_device->getCursorControl()->setVisible(false);

	/** set epmty fuctions pointer */
	init = &GLRenderer::_empty_init;
}

void GLRenderer::_load_example_11()
{
	video::IVideoDriver* driver = m_device->getVideoDriver();
	scene::ISceneManager* smgr = m_device->getSceneManager();
	gui::IGUIEnvironment* env = m_device->getGUIEnvironment();

	driver->setTextureCreationFlag(video::ETCF_ALWAYS_32_BIT, true);

	const io::path mediaPath = getExampleMediaPath();

	// add irrlicht logo
	env->addImage(driver->getTexture(mediaPath + "irrlichtlogo3.png"),
	    core::position2d<s32>(10,10));

	// add camera
	scene::ICameraSceneNode* camera = smgr->addCameraSceneNodeFPS();
	camera->setPosition(core::vector3df(-200,200,-200));

	// disable mouse cursor
//	m_device->getCursorControl()->setVisible(false);
	driver->setFog(video::SColor(0,138,125,81), video::EFT_FOG_LINEAR, 250, 1000, .003f, true, false);

	scene::IAnimatedMesh* roomMesh = smgr->getMesh(mediaPath + "room.3ds");
	scene::ISceneNode* room = 0;
	scene::ISceneNode* earth = 0;

	if (roomMesh)
	{
		smgr->getMeshManipulator()->makePlanarTextureMapping(
		        roomMesh->getMesh(0), 0.003f);

		video::ITexture* normalMap =
		    driver->getTexture(mediaPath + "rockwall_height.bmp");

		if (normalMap)
			driver->makeNormalMapTexture(normalMap, 9.0f);
/*
		// The Normal Map and the displacement map/height map in the alpha channel
		video::ITexture* normalMap =
			driver->getTexture(mediaPath + "rockwall_NRM.tga");
*/
		scene::IMesh* tangentMesh = smgr->getMeshManipulator()->
		        createMeshWithTangents(roomMesh->getMesh(0));

		room = smgr->addMeshSceneNode(tangentMesh);
		room->setMaterialTexture(0,
		        driver->getTexture(mediaPath + "rockwall.jpg"));
		room->setMaterialTexture(1, normalMap);

		// Stones don't glitter..
		room->getMaterial(0).SpecularColor.set(0,0,0,0);
		room->getMaterial(0).Shininess = 0.f;

		room->setMaterialFlag(video::EMF_FOG_ENABLE, true);
		room->setMaterialType(video::EMT_PARALLAX_MAP_SOLID);
		// adjust height for parallax effect
		room->getMaterial(0).MaterialTypeParam = 1.f / 64.f;

		// drop mesh because we created it with a create.. call.
		tangentMesh->drop();
	}

	scene::IAnimatedMesh* earthMesh = smgr->getMesh(mediaPath + "earth.x");
	if (earthMesh)
	{
		//perform various task with the mesh manipulator
		scene::IMeshManipulator *manipulator = smgr->getMeshManipulator();

		// create mesh copy with tangent information from original earth.x mesh
		scene::IMesh* tangentSphereMesh =
		    manipulator->createMeshWithTangents(earthMesh->getMesh(0));

		// set the alpha value of all vertices to 200
		manipulator->setVertexColorAlpha(tangentSphereMesh, 200);

		// scale the mesh by factor 50
		core::matrix4 m;
		m.setScale ( core::vector3df(50,50,50) );
		manipulator->transform( tangentSphereMesh, m );

		earth = smgr->addMeshSceneNode(tangentSphereMesh);

		earth->setPosition(core::vector3df(-70,130,45));

		// load heightmap, create normal map from it and set it
		video::ITexture* earthNormalMap = driver->getTexture(mediaPath + "earthbump.jpg");
		if (earthNormalMap)
		{
			driver->makeNormalMapTexture(earthNormalMap, 20.0f);
			earth->setMaterialTexture(1, earthNormalMap);
			earth->setMaterialType(video::EMT_NORMAL_MAP_TRANSPARENT_VERTEX_ALPHA);
		}

		// adjust material settings
		earth->setMaterialFlag(video::EMF_FOG_ENABLE, true);

		// add rotation animator
		scene::ISceneNodeAnimator* anim =
		    smgr->createRotationAnimator(core::vector3df(0,0.1f,0));
		earth->addAnimator(anim);
		anim->drop();

		// drop mesh because we created it with a create.. call.
		tangentSphereMesh->drop();
	}

	scene::ILightSceneNode* light1 =
	    smgr->addLightSceneNode(0, core::vector3df(0,0,0),
	    video::SColorf(0.5f, 1.0f, 0.5f, 0.0f), 800.0f);

	// add fly circle animator to light 1
	scene::ISceneNodeAnimator* anim =
	    smgr->createFlyCircleAnimator (core::vector3df(50,300,0),190.0f, -0.003f);
	light1->addAnimator(anim);
	anim->drop();

	// attach billboard to the light
	scene::IBillboardSceneNode* bill =
	    smgr->addBillboardSceneNode(light1, core::dimension2d<f32>(60, 60));

	bill->setMaterialFlag(video::EMF_LIGHTING, false);
	bill->setMaterialFlag(video::EMF_ZWRITE_ENABLE, false);
	bill->setMaterialType(video::EMT_TRANSPARENT_ADD_COLOR);
	bill->setMaterialTexture(0, driver->getTexture(mediaPath + "particlegreen.jpg"));

	// add light 2 (red)
	scene::ISceneNode* light2 =
	    smgr->addLightSceneNode(0, core::vector3df(0,0,0),
	    video::SColorf(1.0f, 0.2f, 0.2f, 0.0f), 800.0f);

	// add fly circle animator to light 2
	anim = smgr->createFlyCircleAnimator(core::vector3df(0,150,0), 200.0f,
	        0.001f, core::vector3df(0.2f, 0.9f, 0.f));
	light2->addAnimator(anim);
	anim->drop();

	// attach billboard to light
	bill = smgr->addBillboardSceneNode(light2, core::dimension2d<f32>(120, 120));
	bill->setMaterialFlag(video::EMF_LIGHTING, false);
	bill->setMaterialFlag(video::EMF_ZWRITE_ENABLE, false);
	bill->setMaterialType(video::EMT_TRANSPARENT_ADD_COLOR);
	bill->setMaterialTexture(0, driver->getTexture(mediaPath + "particlered.bmp"));

	// add particle system
	scene::IParticleSystemSceneNode* ps =
	    smgr->addParticleSystemSceneNode(false, light2);

	// create and set emitter
	scene::IParticleEmitter* em = ps->createBoxEmitter(
	    core::aabbox3d<f32>(-3,0,-3,3,1,3),
	    core::vector3df(0.0f,0.03f,0.0f),
	    80,100,
	    video::SColor(10,255,255,255), video::SColor(10,255,255,255),
	    400,1100);
	em->setMinStartSize(core::dimension2d<f32>(30.0f, 40.0f));
	em->setMaxStartSize(core::dimension2d<f32>(30.0f, 40.0f));

	ps->setEmitter(em);
	em->drop();

	// create and set affector
	scene::IParticleAffector* paf = ps->createFadeOutParticleAffector();
	ps->addAffector(paf);
	paf->drop();

	// adjust some material settings
	ps->setMaterialFlag(video::EMF_LIGHTING, false);
	ps->setMaterialFlag(video::EMF_ZWRITE_ENABLE, false);
	ps->setMaterialTexture(0, driver->getTexture(mediaPath + "fireball.bmp"));
	ps->setMaterialType(video::EMT_TRANSPARENT_ADD_COLOR);

	/** set epmty fuctions pointer */
	init = &GLRenderer::_empty_init;
}

void GLRenderer::_load_example_16()
{
	if(!m_device)
		return;

	const io::path mediaPath = getExampleMediaPath();

	//! add our private media directory to the file system
	m_device->getFileSystem()->addFileArchive(mediaPath);

	/*
		To display the Quake 3 map, we first need to load it. Quake 3 maps
		are packed into .pk3 files, which are nothing other than .zip files.
		So we add the .pk3 file to our FileSystem. After it was added,
		we are able to read from the files in that archive as they would
		directly be stored on disk.
		*/
	m_device->getFileSystem()->addFileArchive(getExampleMediaPath() + "map-20kdm2.pk3");

	// Quake3 Shader controls Z-Writing
	m_scene->getParameters()->setAttribute(scene::ALLOW_ZWRITE_ON_TRANSPARENT, true);

	scene::IQ3LevelMesh* const mesh =
	        (scene::IQ3LevelMesh*) m_scene->getMesh("maps/20kdm2.bsp");

	/*
			add the geometry mesh to the Scene ( polygon & patches )
			The Geometry mesh is optimised for faster drawing
		*/
	scene::ISceneNode* node = 0;
	if (mesh)
	{
		scene::IMesh * const geometry = mesh->getMesh(quake3::E_Q3_MESH_GEOMETRY);
		node = m_scene->addOctreeSceneNode(geometry, 0, -1, 4096);
	}

	// create an event receiver for making screenshots
//	CScreenShotFactory screenshotFactory(m_device, mapname, node);
//	device->setEventReceiver(&screenshotFactory);

	if ( mesh )
	{
		// the additional mesh can be quite huge and is unoptimized
		const scene::IMesh * const additional_mesh = mesh->getMesh(quake3::E_Q3_MESH_ITEMS);

#ifdef SHOW_SHADER_NAME
		gui::IGUIFont *font = m_device->getGUIEnvironment()->getFont(mediaPath + "fontlucida.png");
		u32 count = 0;
#endif

		for ( u32 i = 0; i!= additional_mesh->getMeshBufferCount(); ++i )
		{
			const IMeshBuffer* meshBuffer = additional_mesh->getMeshBuffer(i);
			const video::SMaterial& material = meshBuffer->getMaterial();

			// The ShaderIndex is stored in the material parameter
			const s32 shaderIndex = (s32) material.MaterialTypeParam2;

			// the meshbuffer can be rendered without additional support, or it has no shader
			const quake3::IShader *shader = mesh->getShader(shaderIndex);
			if (0 == shader)
			{
				continue;
			}

			node = m_scene->addQuake3SceneNode(meshBuffer, shader);

#ifdef SHOW_SHADER_NAME
			count += 1;
			core::stringw name( node->getName() );
			node = m_scene->addBillboardTextSceneNode(
			            font, name.c_str(), node,
			            core::dimension2d<f32>(80.0f, 8.0f),
			            core::vector3df(0, 10, 0));
#endif
		}
	}

	scene::ICameraSceneNode* viewCamera = m_scene->addCameraSceneNodeFPS(0,1.0f);

	if ( mesh )
	{
		quake3::tQ3EntityList &entityList = mesh->getEntityList();

		quake3::IEntity search;
		search.name = "info_player_deathmatch";

		s32 index = entityList.binary_search(search);
		if (index >= 0)
		{
			s32 notEndList;
			do
			{
				const quake3::SVarGroup *group = entityList[index].getGroup(1);

				u32 parsepos = 0;
				const core::vector3df pos =
				        quake3::getAsVector3df(group->get("origin"), parsepos);

				parsepos = 0;
				const f32 angle = quake3::getAsFloat(group->get("angle"), parsepos);

				core::vector3df target(0.f, 0.f, 1.f);
				target.rotateXZBy(angle);

				//				viewCamera->setPosition(pos);
				//				camera->setTarget(pos + target);

				++index;
				/*
					notEndList = (	index < (s32) entityList.size () &&
									entityList[index].name == search.name &&
									(device->getTimer()->getRealTime() >> 3 ) & 1
								);
	*/
				notEndList = index == 2;
			} while ( notEndList );
		}
	}
	viewCamera->setPosition(core::vector3df(1300,144,1249));
	/*
		The mouse cursor needs not to be visible, so we make it invisible.
		*/

//	m_device->getCursorControl()->setVisible(false);

	// load the engine logo
	m_device->getGUIEnvironment()->addImage(m_driver->getTexture("irrlichtlogo2.png"),
	              core::position2d<s32>(10, 10));

	// show the driver logo
	const core::position2di pos((s32)window()->geometry().width()- 128, (s32)window()->geometry().height() - 64);

	/** set epmty fuctions pointer */
	init = &GLRenderer::_empty_init;
}

void GLRenderer::paint()
{
	(this->*init)();
//	reinterpret_cast<CQGLFunctionsDriver*>(m_device)->m_functions = m_window->openglContext()->functions();
//	m_driver->setMaterial(IdentityMaterial);
//	/*m_window->openglContext()->functions()->*/glUseProgram( 0 );
	m_device->run();
	m_device->getVideoDriver()->beginScene(true,true, SColor(255,140,140,200) );
	m_device->getSceneManager()->drawAll();
	m_device->getGUIEnvironment()->drawAll();
	m_device->getVideoDriver()->endScene();
	m_window->resetOpenGLState();
}
