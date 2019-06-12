#pragma once

#include <QGL>
#include <memory>

class QGestureEvent;

class MyOpenGLWidget : public QGLWidget
{
	Q_OBJECT

public:
	MyOpenGLWidget(QWidget *parent = Q_NULLPTR);
	~MyOpenGLWidget();

	void initializeGL() override;
	void resizeGL(int w, int h) override;
	void paintGL() override;

	bool event(QEvent *event) override;
	void mousePressEvent(QMouseEvent *event) override;
	void mouseMoveEvent(QMouseEvent *event) override;
	void wheelEvent(QWheelEvent *event) override;

	void gestureEvent(QGestureEvent *event);

signals: // signals

public slots:
	void setLight(bool);
	void onActionSnapshot();
	void idleEvent();
	void setPaused(bool);

private:
	struct impl_t;
	const std::unique_ptr<impl_t> pimpl;

};
