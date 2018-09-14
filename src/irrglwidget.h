#ifndef IRRGLWIDGET_H
#define IRRGLWIDGET_H

//#include <QOpenGLWidget>
#include <QGLWidget>
#include <irrlicht.h>

using namespace irr;
using namespace scene;
using namespace video;
using namespace core;

class IrrGLWidget : public QGLWidget
{
	Q_OBJECT
public:
//	IrrGLWidget(QWidget* parent = Q_NULLPTR, Qt::WindowFlags f = Qt::WindowFlags());
	IrrGLWidget(QWidget *parent = Q_NULLPTR);
	~IrrGLWidget();
protected:
	virtual void initializeGL() override;
	virtual void resizeGL(int w, int h) override;
	virtual void paintGL() override;
//	virtual void paintUnderGL() override;
//	virtual void paintOverGL() override;

	void _load_example_16();
private:
	IrrlichtDevice *m_device;
	ISceneManager  *m_scene;
	SColor          m_bgColor;
};

#endif // IRRGLWIDGET_H
