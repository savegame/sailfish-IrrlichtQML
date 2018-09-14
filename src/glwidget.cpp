#include "glwidget.h"

#include <QOpenGLContext>
#include <QOpenGLFunctions>

GLWindow::GLWindow(/*QScreen *screen*/)
//    : QWindow(screen), QOpenGLFunctions()
    : QOpenGLWindow()
{
	setVisibility(QWindow::AutomaticVisibility);

	setSurfaceType(OpenGLSurface);
	create();

	// Create an OpenGL context
	m_context = context();/*new QOpenGLContext;
	m_context->create();
	m_context->setScreen(screen);*/

//	// Setup scene and render it
//	initializeGL();
//	paintGL();
}

void GLWindow::initializeGL()
{
//	m_context->makeCurrent(this);
	makeCurrent();
	QOpenGLFunctions *f = context()->functions();
	f->initializeOpenGLFunctions();
	m_context = context();
//	m_context->functions()->initializeOpenGLFunctions();
//	initializeOpenGLFunctions();
	update();
}

void GLWindow::paintGL()
{
//	m_context->makeCurrent(this);
	makeCurrent();
	m_context->functions()->glClearColor( 0.2, 0.2, 1.0, 1.0 );
//	glActiveTexture(GL_TEXTURE1);
//	glBindTexture(GL_TEXTURE_2D, textureId);
//	...
	m_context->swapBuffers(this);
//	m_context->doneCurrent();
	doneCurrent();
	update();
}

void GLWindow::resizeGL(int width, int height)
{

}
