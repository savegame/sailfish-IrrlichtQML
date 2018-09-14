#ifndef GLWIDGET_H
#define GLWIDGET_H

#include <QWindow>
#include <QOpenGLFunctions>

#include <QOpenGLWindow>

//class GLWindow : public QWindow, protected QOpenGLFunctions
class GLWindow : public QOpenGLWindow
{
public:
	GLWindow();

protected:
	void initializeGL();
	void paintGL();
	void resizeGL(int width, int height);

private:
	QOpenGLContext *m_context;
};

#endif // GLWIDGET_H
