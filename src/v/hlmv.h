#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_hlmv.h"

class QtGuiApplication1 : public QMainWindow
{
	Q_OBJECT

public:
	QtGuiApplication1(QWidget *parent = Q_NULLPTR);

	friend class MyOpenGLWidget;

public slots:
	void OnActionOpen();
	void OnActionSave();
	void OnActionClear();
	void OnActionAbout();
	void OnTabChanged(int);

	void setSequence(int);
	void OnSetAnimationPlaying(bool);
	void OnSetAnimationCurrentFrame(int);
	void OnSetAnimationFPS(double);

	void setEventInfo(int index);
	void setMesh(int index);

	void setTextureCurrent(int index);
	void setTextureScale(int value);
	void updateTextureFlags();
	void updateTextureUVDisplay();
	void onImportTexture();
	void onExportTexture();

	void setBodypart (int index);
	void setSubmodel (int index);
	void setSkin (int index);
	void setBoneController (int index);
	void setBoneControllerCurrentValue (int value);

	void updateViewSettings();
	void updateWeaponOriginSettings();

signals:
	void updateStatusBar(QString str);

protected:
	void dragEnterEvent(QDragEnterEvent *e) override;
	void dropEvent(QDropEvent *e) override;

public:
	void OpenFile(QString str) noexcept(false);
	void SaveFile(QString str) noexcept(false);
	void CloseFile();
	void initSequences ();
	void setSequenceInfo ();
	void setEvent (int seq);
	void initTextures ();
	void initMeshList (int index);
	void initBodyparts();
	void initBoneControllers ();
	void setBoneControllerValue (int index, float value);
	void initSkins ();
	void setModelInfo ();
	void centerView();

public:
#ifdef Q_OS_MAC
	void SetupMacWindow();
	void SetupPlatformWindow() { SetupMacWindow(); }
#elif defined(Q_OS_WIN32)
	void SetupAeroWindow();
	void SetupPlatformWindow() { SetupAeroWindow(); }
#else
	void SetupPlatformWindow() { }
#endif

public:
	Ui::QtGuiApplication1Class ui;
};
