#include "irrglwidget.h"
#include <QOpenGLFunctions>
#include <QGLFunctions>
#include <QDebug>
using namespace irr;

io::path getExampleMediaPath() {
	return _IRR_MEDIA;
}

IrrGLWidget::IrrGLWidget(QWidget *parent)
    : QGLWidget(parent)
    , m_scene(nullptr)
    , m_device(nullptr)
{
	m_bgColor = SColor(255,150,150,255);
//	setAttribute( Qt::WA_PaintOnScreen );
}

IrrGLWidget::~IrrGLWidget()
{
	if(m_device)
		m_device->drop();
}

void IrrGLWidget::initializeGL()
{
	if(!context()->isValid())
		qWarning() << "OpenGL Context is invalid";
	irr::SIrrlichtCreationParameters params;

//	context()->functions()
//	context()->functions()->initializeOpenGLFunctions();

//	params.DriverType = EDT_QOGLDUNCTIONS;//driverType;
//	params.DeviceType = EIDT_CONSOLE;
//	params.WindowId = NULL;
//	params.Bits = 32;
//	params.Doublebuffer = true;
//	params.Fullscreen = false;
//	params.IgnoreInput = false;
//	params.Stencilbuffer = true;
//	params.Stereobuffer = false;
//	params.WindowSize.Width = width();
//	params.WindowSize.Height = height();
//	params.AntiAlias = 0;
//	params.OGLES2ShaderPath = _MEDIA_PATH;
//	params.OGLES2ShaderPath += "Shaders/";
//	params.Vsync = false;
//	params.qOpenGLFunctions = context()->functions();

//	m_device = irr::createDeviceEx(params);

//	if(m_device)
//	{
//		m_scene = m_device->getSceneManager();

//		ICameraSceneNode *cam = m_scene->addCameraSceneNode(0, vector3df(0,0,-20) );
//		cam->setTarget( vector3df(0,0,0) );

//		io::path path = _MEDIA_PATH;
//		path += "rockwall.jpg";
//		ITexture *texture = m_device->getVideoDriver()->getTexture( path );

//		IMeshSceneNode *cube = m_scene->addCubeSceneNode(1,0);
//		cube->setPosition( vector3df(0,0,0) );
//		cube->setMaterialFlag(EMF_LIGHTING,false);
//		cube->setMaterialTexture(0,texture);

//		ISceneNodeAnimator *anim = m_scene->createRotationAnimator( vector3df(1,1,1) );
//		cube->addAnimator(anim);
//		anim->drop();
//	}
}

void IrrGLWidget::resizeGL(int w, int h)
{
	if ( m_device != 0 )
	{
		irr::core::dimension2du size;
		size.Width = w;
		size.Height = h;
		core::dimension2du ss = m_device->getVideoDriver()->getScreenSize();
		m_device->getVideoDriver()->OnResize( size );

		irr::scene::ICameraSceneNode *cam = m_scene->getActiveCamera();
		if ( cam != NULL )
		{
			cam->setAspectRatio((float)w/(float)h);
			core::stringc m = "Render size changed form ";
			m += ss.Width;
			m += "x";
			m += ss.Height;
			m += " to ";
			m += m_device->getVideoDriver()->getScreenSize().Width;
			m += "x";
			m += m_device->getVideoDriver()->getScreenSize().Height;
			m_device->getLogger()->log( m.c_str(), ELL_DEBUG);
		}
		else
			m_device->getLogger()->log( "Camera not created", ELL_WARNING);
	}
}

void IrrGLWidget::paintGL()
{
	if( !context()->isValid() )
		return;
	makeCurrent();
//	m_device->run();
//	context()->functions()->glC
//	context()->functions()->glClearColor(1,1,1,1);
//	qglClearColor(Qt::red);
//	context()->functions()->
//	m_device->getVideoDriver()->beginScene( true, true, SColor(255,255,255,255) );
//	m_device->getSceneManager()->drawAll();
//	m_device->getVideoDriver()->endScene();
	// now draw foreground 3D
//	context()->functions()->glClear(GL_DEPTH_BUFFER_BIT);
//	m_cursor->render();
//	m_brush->renderSelected();
//	/** driver->endScene() */
//	context()->functions()->glFlush();
//	QSurface *s = context()->surface();
//	context()->swapBuffers( context()->surface() );
	context()->swapBuffers();
	doneCurrent();
	update();
}

//void IrrGLWidget::paintUnderGL()
//{
//	QOpenGLWindow::paintUnderGL();
//}

//void IrrGLWidget::paintOverGL()
//{
//	QOpenGLWindow::paintOverGL();
//}

void IrrGLWidget::_load_example_16()
{
	if(!m_device)
		return;

	const io::path mediaPath = getExampleMediaPath();

	//! add our private media directory to the file system
	m_device->getFileSystem()->addFileArchive(mediaPath);

	IVideoDriver *m_driver = m_device->getVideoDriver();

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
	const core::position2di pos((s32)geometry().width()- 128, (s32)geometry().height() - 64);
}
