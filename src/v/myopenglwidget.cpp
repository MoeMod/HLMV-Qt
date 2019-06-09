#include <QEvent>
#include <QApplication>
#include <QClipboard>
#include <QBitmap>
#include <QMessageBox>
#include <QFileDialog>
#include <QVector3D>
#include <QMouseEvent>
#include <QTimer>

#include "ViewerSettings.h"

#include "gl_draw.h"
#include "myopenglwidget.h"

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

void MyOpenGLWidget::mousePressEvent(QMouseEvent *event)
{
	pimpl->m_LastMousePos = event->pos();
}


template<class T> void qNormalizeAngle(T &angle)
{
	while (angle < 0)
		angle += 360 * 16;
	while (angle > 360)
		angle -= 360 * 16;
}

void MyOpenGLWidget::mouseMoveEvent(QMouseEvent *event)
{
	int dx = event->x() - pimpl->m_LastMousePos.x();
	int dy = event->y() - pimpl->m_LastMousePos.y();

	if (event->buttons() & Qt::LeftButton) {

		g_viewerSettings.rot[0] += dy;
		g_viewerSettings.rot[1] += dx;
		qNormalizeAngle(dx);
		qNormalizeAngle(dy);

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
	g_viewerSettings.trans[2] += event->delta();

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

		QString filename = QFileDialog::getSaveFileName(this, {}, {}, "JPG file (*.jpg);;BMP file (*.bmp);;PNG file (*.png);;TIFF file (*.tiff)");

		if(!filename.isEmpty())
			qi.save(filename);

	}
}

void MyOpenGLWidget::setPaused(bool x)
{
	pimpl->m_gldraw.SetStopPlaying(x);
}
