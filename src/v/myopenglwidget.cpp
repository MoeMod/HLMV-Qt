#include <QEvent>
#include <QApplication>
#include <QClipboard>
#include <QBitmap>
#include <QMessageBox>
#include <QFileDialog>
#include <QVector3D>
#include <QMouseEvent>
#include <QTimer>
#include <QGestureEvent>
#include <QNativeGestureEvent>

#include "ViewerSettings.h"

#include "gl_draw.h"
#include "myopenglwidget.h"

#include "qt_image.h"

struct MyOpenGLWidget::impl_t
{
	explicit impl_t(MyOpenGLWidget *p) : m_Timer(p) {}

	QPoint m_LastMousePos;

	QTimer m_Timer;
	CHLMV_GL_Draw m_gldraw;
};

MyOpenGLWidget::MyOpenGLWidget(QWidget *parent) :
	pimpl(new impl_t(this))
{
	QObject::connect (&pimpl->m_Timer, SIGNAL (timeout ()), this, SLOT (idleEvent ()));
	pimpl->m_Timer.start ();

	grabGesture(Qt::PinchGesture);
	//QApplication::setAttribute(Qt::AA_SynthesizeMouseForUnhandledTouchEvents, false);
	//QApplication::setAttribute(Qt::AA_SynthesizeTouchForUnhandledMouseEvents, false);
}

MyOpenGLWidget::~MyOpenGLWidget() = default;

void MyOpenGLWidget::idleEvent()
{
	pimpl->m_gldraw.Think();

	if (!g_viewerSettings.pause)
		this->updateGL ();
}

void MyOpenGLWidget::initializeGL()
{

}

void MyOpenGLWidget::resizeGL(int w, int h)
{
	pimpl->m_gldraw.SetSize(width(), height(), w, h);
}

void MyOpenGLWidget::paintGL()
{
	pimpl->m_gldraw.Draw();
}

bool MyOpenGLWidget::event(QEvent *event)
{
	switch(event->type()){
		case QEvent::Gesture:
		{
			gestureEvent(static_cast<QGestureEvent *>(event));
			return true;
		}
		case QEvent::TouchBegin:
		case QEvent::TouchUpdate:
		case QEvent::TouchEnd:
		{
			touchEvent(static_cast<QTouchEvent *>(event));
			return true;
		}
		default:
			break;
	}
	return QGLWidget::event(event);
}

void MyOpenGLWidget::gestureEvent(QGestureEvent *event)
{
	// 2-fingers on both touchpad and touchscreen
	QPinchGesture *pinch = static_cast<QPinchGesture *>(event->gesture(Qt::PinchGesture));
	if (pinch)
	{
		QPinchGesture::ChangeFlags changeFlags = pinch->changeFlags();
		if (changeFlags & QPinchGesture::ScaleFactorChanged)
		{
			qreal scale_delta = pinch->scaleFactor();
			g_viewerSettings.trans[2] /= scale_delta * scale_delta;
		}

		if(changeFlags & QPinchGesture::CenterPointChanged)
		{
			QPointF position_delta = pinch->centerPoint() - pinch->lastCenterPoint();
			g_viewerSettings.trans[0] -= position_delta.x() / 4;
			g_viewerSettings.trans[1] += position_delta.y() / 4;
		}
	}

	// 3-fingers
	/*
	QPanGesture *pan = static_cast<QPanGesture *>(event->gesture(Qt::PanGesture));
	if (pan)
	{
		QPointF position_delta = pan->delta();
		g_viewerSettings.trans[0] -= position_delta.x();
		g_viewerSettings.trans[1] += position_delta.y();
	}
	*/

	updateGL();
}

void MyOpenGLWidget::touchEvent(QTouchEvent *event)
{
	if (event->touchPoints().size() != 1)
		return;

	const QTouchEvent::TouchPoint &tp = event->touchPoints().first();
	if (1)
	{
		auto delta = tp.pos() - tp.lastPos();
		g_viewerSettings.rot[0] += delta.y();
		g_viewerSettings.rot[1] += delta.x();
		updateGL();
	}
}

void MyOpenGLWidget::mousePressEvent(QMouseEvent *event)
{
	if (event->source() != Qt::MouseEventNotSynthesized)
		return;
	pimpl->m_LastMousePos = event->pos();
}

void MyOpenGLWidget::mouseMoveEvent(QMouseEvent *event)
{
	if (event->source() != Qt::MouseEventNotSynthesized)
		return;
	
	int dx = event->x() - pimpl->m_LastMousePos.x();
	int dy = event->y() - pimpl->m_LastMousePos.y();

	if (event->buttons() & Qt::LeftButton) {

		g_viewerSettings.rot[0] += dy;
		g_viewerSettings.rot[1] += dx;

		updateGL();
	}
	else if (event->buttons() & Qt::RightButton) {
		g_viewerSettings.trans[0] -= dx;
		g_viewerSettings.trans[1] += dy;

		updateGL();
	}

	pimpl->m_LastMousePos = event->pos();
}

void MyOpenGLWidget::wheelEvent(QWheelEvent *event)
{
	if(event->source() == Qt::MouseEventSynthesizedBySystem)
	{
		// macOS touchpad
		auto delta = event->pixelDelta();
		g_viewerSettings.trans[0] -= delta.x();
		g_viewerSettings.trans[1] += delta.y();
	}
	else
	{
		g_viewerSettings.trans[2] += event->delta();
	}
	updateGL();
}

void MyOpenGLWidget::setLight(bool b)
{
	b ? glEnable(GL_LIGHTING) : glDisable(GL_LIGHTING);
	b ? glEnable(GL_LIGHT0) : glDisable(GL_LIGHT0);
	updateGL();
}

void MyOpenGLWidget::onActionSnapshot()
{
	const int w = pimpl->m_gldraw.w2();
	const int h = pimpl->m_gldraw.h2();

	QImage qi( w, h, QImage::Format::Format_RGB888);

	updateGL();
	glReadPixels(0, 0, w, h, GL_RGB, GL_UNSIGNED_BYTE, qi.bits());

	qi = std::move(qi).mirrored();

	QClipboard *clipboard = QApplication::clipboard();
	clipboard->setPixmap(QPixmap::fromImage(qi));

	if(QMessageBox::question(this,
			"Save",
			"Image has been copied to clipboard.\nDo you want to save it to a file?",
			QMessageBox::StandardButtons(QMessageBox::Save | QMessageBox::Cancel)
			) == QMessageBox::Save)
	{

		QString filename = getSaveFileImagePath(this, {}, {});

		if(!filename.isEmpty())
			try
			{
				saveImageTo(qi, filename);
			}
			catch(const std::exception &e)
			{
				QMessageBox::critical(this, "Error", e.what()), void();
			}

	}
}

void MyOpenGLWidget::setPaused(bool x)
{
	pimpl->m_gldraw.SetStopPlaying(x);
}
