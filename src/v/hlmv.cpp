#include "hlmv.h"

#include <QFileDialog>
#include <QMessageBox>

#include <QDropEvent>
#include <QDragEnterEvent>
#include <QFile>
#include <QMimeData>

#include "StudioModel.h"
#include "ViewerSettings.h"

#include "mod_decryptor.h"

#include <algorithm>

QtGuiApplication1::QtGuiApplication1(QWidget *parent)
	: QMainWindow(parent)
{
	ui.setupUi(this);

	connect(ui.actionOpen, &QAction::triggered, this, &QtGuiApplication1::OnActionOpen);
	connect(ui.actionSave, &QAction::triggered, this, &QtGuiApplication1::OnActionSave);
	connect(ui.actionClear, &QAction::triggered, this, &QtGuiApplication1::OnActionClear);
	connect(ui.actionAbout, &QAction::triggered, this, &QtGuiApplication1::OnActionAbout);
	updateStatusBar("Drag model file to open");
	setAcceptDrops(true);

#ifdef Q_OS_MACOS
	// Mac specific
	SetupMacWindow();
#endif

}

void QtGuiApplication1::OnActionOpen()
{
	QString filename = QFileDialog::getOpenFileName(this, "Load model file", {}, tr(
			"Model file (*.mdl)"
			));
	if(!filename.isEmpty())
	{
		try
		{
			OpenFile(filename);
		}
		catch (const std::exception &e)
		{
			QMessageBox::warning(this, "Error", e.what());
		}
	}
}

void QtGuiApplication1::OnActionSave()
{
	QString filename = QFileDialog::getSaveFileName(this, "Save model file", {}, tr("Model file (*.*)"));
	if (!filename.isEmpty())
	{
		try
		{

		}
		catch (const std::exception &e)
		{
			QMessageBox::warning(this, "Error", e.what());
		}
	}
}

void QtGuiApplication1::OnActionClear()
{

}

void QtGuiApplication1::OnActionAbout()
{
	QMessageBox::about(this, "About", "Made by MoeMod in Summer 2019.");
}


void QtGuiApplication1::dragEnterEvent(QDragEnterEvent *e)
{
	// only 1 file is accepted
	if (e->mimeData()->urls().size() != 1)
		return;

	e->acceptProposedAction();
}

void QtGuiApplication1::dropEvent(QDropEvent *e)
{
	QList<QUrl> urls = e->mimeData()->urls();
	if (urls.isEmpty())
		return;

	QString filename = urls.first().toLocalFile();

	if (filename.isEmpty())
		return;

	try
	{
		OpenFile(filename);
	}
	catch (const std::exception & e)
	{
		QMessageBox::warning(this, "Error", e.what());
	}
}

void QtGuiApplication1::OpenFile(QString qfilename) noexcept(false)
{
	std::string sfilename = qfilename.toStdString();
	const char *filename = sfilename.c_str();
	g_studioModel.FreeModel ();

	studiohdr_t *phdr = g_studioModel.LoadModel (filename);
	if (!phdr)
		throw std::runtime_error("Error loading model.");

	if(Mod_IsCSOEncryptedModel(phdr))
	{
		if(QMessageBox::question(this,
				"CSO Model Helper",
				"This model is CSO-format and encrypted with IceKey. \nDo you want to decrypt it?"
				) == QMessageBox::Yes)
		{
			Mod_DecryptModel(filename, phdr);
		}
	}

	g_studioModel.LoadModelTextures(phdr);

	if (!g_studioModel.PostLoadModel (phdr, filename))
		throw std::runtime_error("Error post-loading model.");


	initSequences ();
	setEvent (0);
	initBodyparts ();
	initBoneControllers ();
	initSkins ();
	initTextures ();
	centerView ();
	strcpy (g_viewerSettings.modelFile, filename);
	setModelInfo ();
	g_viewerSettings.sequence = 0;
	setSequenceInfo ();
	g_viewerSettings.speedScale = 1.0f;


	setWindowFilePath(qfilename);
	setWindowTitle("HLMV - " + QFileInfo(qfilename).fileName());

	std::fill(std::begin(g_viewerSettings.submodels), std::end(g_viewerSettings.submodels), 0);
	std::fill(std::begin(g_viewerSettings.controllers), std::end(g_viewerSettings.controllers), 0);
}

void QtGuiApplication1::centerView ()
{
	float min[3], max[3];
	g_studioModel.ExtractBbox (min, max);

	float dx = max[0] - min[0];
	float dy = max[1] - min[1];
	float dz = max[2] - min[2];
	float d = dx;
	if (dy > d)
		d = dy;
	if (dz > d)
		d = dz;
	g_viewerSettings.trans[0] = 0;
	g_viewerSettings.trans[1] = min[2] + dz / 2;
	g_viewerSettings.trans[2] = d * 1.0f;
	g_viewerSettings.rot[0] = -90.0f;
	g_viewerSettings.rot[1] = -90.0f;
	g_viewerSettings.rot[2] = 0.0f;
	ui.centralWidget->update ();
}

