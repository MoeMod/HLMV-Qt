#include "hlmv.h"

#include <QMessageBox>
#include <QFileDialog>
#include <QScreen>

QtGuiApplication1::QtGuiApplication1(QWidget *parent)
	: QMainWindow(parent)
{
	this->setMinimumWidth(QApplication::primaryScreen()->availableVirtualSize().width() / 2);
	this->setMinimumHeight(width() / 3 * 2);

	ui.setupUi(this);

	connect(ui.actionOpen, &QAction::triggered, this, &QtGuiApplication1::OnActionOpen);
	connect(ui.actionSave, &QAction::triggered, this, &QtGuiApplication1::OnActionSave);
	connect(ui.actionClear, &QAction::triggered, this, &QtGuiApplication1::OnActionClear);
	connect(ui.actionAbout, &QAction::triggered, this, &QtGuiApplication1::OnActionAbout);
	updateStatusBar("Drag model file to open");
	setAcceptDrops(true);

	SetupPlatformWindow();
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
	QString filename = QFileDialog::getSaveFileName(this, "Save model file", {}, tr("Model file (*.mdl)"));
	if (!filename.isEmpty())
	{
		try
		{
			SaveFile(filename);
		}
		catch (const std::exception &e)
		{
			QMessageBox::warning(this, "Error", e.what());
		}
	}
}

void QtGuiApplication1::OnActionClear()
{
	CloseFile();
}

void QtGuiApplication1::OnActionAbout()
{
	QMessageBox::about(this, "About", "Made by MoeMod in Summer 2019.");
}

