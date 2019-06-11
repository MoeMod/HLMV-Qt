#include "hlmv.h"
#include "StudioModel.h"
#include "ViewerSettings.h"
#include "mod_decryptor.h"

#include <QDropEvent>
#include <QDragEnterEvent>
#include <QMimeData>
#include <QFile>
#include <QMessageBox>
#include <QDir>
#include <QFileInfo>

#include <algorithm>

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

	bool bCSOTexture = false;
	if(Mod_IsCSOEncryptedModel(phdr))
	{
		if(QMessageBox::question(this,
		                         "CSO Model Helper",
		                         "This model is CSO-format and encrypted with IceKey. \nDo you want to decrypt it?"
		) == QMessageBox::Yes)
		{
			Mod_DecryptModel(filename, phdr);
			bCSOTexture = true;
		}
	}

	if(!bCSOTexture && g_studioModel.hasCSOTexture(phdr))
	{
		if(QMessageBox::question(this,
		                         "CSO Model Helper",
		                         "This model is CSO-format and has external textures. \nDo you want to load them?"
		) == QMessageBox::Yes)
		{
			bCSOTexture = true;
		}
	}


	if (!g_studioModel.PostLoadModel (phdr, filename))
		throw std::runtime_error("Error post-loading model.");

	if(bCSOTexture)
		g_studioModel.LoadModelTexturesCSO(phdr, (QFileInfo(qfilename).path() + "/texture").toStdString().c_str());
	else
		g_studioModel.LoadModelTextures(phdr);


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

void QtGuiApplication1::SaveFile(QString qfilename) noexcept(false)
{
	std::string sfilename = qfilename.toStdString();
	const char *filename = sfilename.c_str();

	if (!filename || !filename[0])
		throw std::runtime_error("Empty file name.");

	if (!g_studioModel.getStudioHeader())
		throw std::runtime_error("No file opened.");

	bool result = g_studioModel.SaveModel(filename);
	if(!result)
		throw std::runtime_error("Error writing file.");
}

void QtGuiApplication1::CloseFile()
{
	g_studioModel.FreeModel();

	ui.cSequence->clear();
	ui.cWpSequence->clear ();
	ui.lSequenceInfo1->clear();
	ui.lSequenceInfo2->clear();
	ui.slWeaponFrame->setRange(0, 0);
	ui.leWeaponFrame->setRange(0, 0);
	ui.slWpSpeedFPS->setRange(0, 0);

	ui.cEvent->clear ();
	ui.lEventInfo->clear ();

	ui.cSubmodel->clear ();
	ui.cBodypart->clear ();
	ui.cController->clear ();
	ui.cSkin->clear ();

	ui.cTextures->clear ();
	ui.cMesh->clear ();

	strcpy (g_viewerSettings.modelFile, "");
	ui.lModelInfo1->clear();
	ui.lModelInfo2->clear();
}
