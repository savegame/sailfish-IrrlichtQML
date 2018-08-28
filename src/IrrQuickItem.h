#ifndef IRRQUICKITEM_H
#define IRRQUICKITEM_H

#include <QtCore/qglobal.h>
#include <QQuickItem>
#include <QRect>
#include <IrrlichtDevice.h>
#include <IVideoDriver.h>

using namespace irr;
using namespace video;
using namespace core;
using namespace scene;
class QOpenGLContext;

class IrrlichtQuickItem : public QQuickItem
{
	Q_OBJECT
public:
	IrrlichtQuickItem();
	~IrrlichtQuickItem();

	typedef void (IrrlichtQuickItem::*tFunc)();
	tFunc init;
	tFunc init_materials;


	Q_INVOKABLE void loadExample(int index);
protected:
	void createCube();
	bool clear_scene();

	void _first_init();
	void _recreate_materials();

	void _empty_func() {}

	const io::path getExampleMediaPath();

	void _load_example_1();
	void _load_example_2();
	void _load_example_11();
	void _load_example_16();

signals:
	void updateSignal();

protected slots:
	void windowChangedSlot( QQuickWindow* window );
	void windowStateChanged(Qt::WindowState s);
	void cleanup();

protected:
	QSGNode* updatePaintNode( QSGNode* oldNode, UpdatePaintNodeData* updatePaintNodeData ) override;
	void touchEvent(QTouchEvent *event) override;
	void focusInEvent(QFocusEvent * event) override
	{
		forceActiveFocus();
		event->accept();
	}

	QRectF _geometry;
	IVideoDriver    *m_driver;
	ISceneManager   *m_scene;
	IrrlichtDevice  *m_device;
	QOpenGLContext  *m_context;
};

#endif // IRRQUICKITEM_H
