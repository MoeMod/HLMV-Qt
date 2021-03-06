#include "hlmv.h"
#include "ViewerSettings.h"
#include "StudioModel.h"
#include "qt_image.h"


#include <QFileDialog>
#include <QMessageBox>
#include <QBitmap>
#include <QColor>

void QtGuiApplication1::initTextures ()
{
	studiohdr_t *hdr = g_studioModel.getTextureHeader ();
	if (hdr)
	{
		ui.cTextures->clear ();
		mstudiotexture_t *ptextures = (mstudiotexture_t *) ((byte *) hdr + hdr->textureindex);
		for (int i = 0; i < hdr->numtextures; i++)
			ui.cTextures->addItem (ptextures[i].name);
		ui.cTextures->setCurrentIndex (0);
		g_viewerSettings.texture = 0;
		if (hdr->numtextures > 0)
		{
			ui.cbChrome->setChecked ((ptextures->flags & STUDIO_NF_CHROME) == STUDIO_NF_CHROME);
			ui.cbAdditive->setChecked ((ptextures->flags & STUDIO_NF_ADDITIVE) == STUDIO_NF_ADDITIVE);
			ui.cbTransparent->setChecked ((ptextures->flags & STUDIO_NF_TRANSPARENT) == STUDIO_NF_TRANSPARENT);
			ui.cbFullBright->setChecked ((ptextures->flags & STUDIO_NF_FULLBRIGHT) == STUDIO_NF_FULLBRIGHT);
			initMeshList(0);
		}
	}
}

void QtGuiApplication1::setMesh (int index)
{
	if (index >= 0)
	{
		g_viewerSettings.showAllMeshes = true;
		if (g_viewerSettings.meshCount >= 1 && index < g_viewerSettings.meshCount)
			g_viewerSettings.showAllMeshes = false;
		g_viewerSettings.mesh = index;
	}
}

void QtGuiApplication1::initMeshList (int index)
{
	char str[64];
	int i, j, k = 0, l;

	studiohdr_t *hdr = g_studioModel.getStudioHeader ();

	if (!hdr)
		return;

	ui.cMesh->clear ();
	for (i = 0; i < hdr->numbodyparts; i++)
	{
		mstudiobodyparts_t *pbodyparts = (mstudiobodyparts_t *) ((byte *) hdr + hdr->bodypartindex) + i;
		for (j = 0; j < pbodyparts->nummodels; j++)
		{
			mstudiomodel_t *pmodels = (mstudiomodel_t *) ((byte *) hdr + pbodyparts->modelindex) + j;
			for (l = 0; l < pmodels->nummesh; l++)
			{
				mstudiomesh_t *pmesh = (mstudiomesh_t *) ((byte *) hdr + pmodels->meshindex) + l;
				if (pmesh->skinref == index)
				{
					sprintf (str, "Mesh %d", ++k);
					ui.cMesh->addItem (str);
				}
			}
		}
	}

	ui.cMesh->addItem ("All");

	g_viewerSettings.mesh = 0;
	g_viewerSettings.meshCount = k;
	ui.cMesh->setCurrentIndex (k);
	setMesh(k);
}

void QtGuiApplication1::setTextureCurrent(int index)
{
	if (index < 0)
		return;

	g_viewerSettings.texture = index;
	studiohdr_t *hdr = g_studioModel.getTextureHeader ();
	if (hdr)
	{
		mstudiotexture_t *ptexture = (mstudiotexture_t *) ((byte *) hdr + hdr->textureindex) + index;
		ui.lTexSize->setText (QString().sprintf("Texture (size: %d x %d)", ptexture->width, ptexture->height));
		ui.cbChrome->setChecked ((ptexture->flags & STUDIO_NF_CHROME) == STUDIO_NF_CHROME);
		ui.cbAdditive->setChecked ((ptexture->flags & STUDIO_NF_ADDITIVE) == STUDIO_NF_ADDITIVE);
		ui.cbTransparent->setChecked ((ptexture->flags & STUDIO_NF_TRANSPARENT) == STUDIO_NF_TRANSPARENT);
		ui.cbFullBright->setChecked ((ptexture->flags & STUDIO_NF_FULLBRIGHT) == STUDIO_NF_FULLBRIGHT);
	}
	initMeshList (index);
	ui.openglwidget->update ();
}

void QtGuiApplication1::updateTextureFlags()
{
	studiohdr_t *hdr = g_studioModel.getTextureHeader ();
	if (hdr)
	{
		mstudiotexture_t *ptexture = (mstudiotexture_t *) ((byte *) hdr + hdr->textureindex) + g_viewerSettings.texture;
		if (ui.cbChrome->isChecked ())
		{
			ptexture->flags &= ~STUDIO_NF_TRANSPARENT;
			ptexture->flags |= STUDIO_NF_CHROME;
			ui.cbTransparent->setChecked (false);
		}
		else
			ptexture->flags &= ~STUDIO_NF_CHROME;

		if (ui.cbAdditive->isChecked ())
		{
			ptexture->flags &= ~STUDIO_NF_TRANSPARENT;
			ptexture->flags |= STUDIO_NF_ADDITIVE;
			ui.cbTransparent->setChecked (false);
		}
		else
			ptexture->flags &= ~STUDIO_NF_ADDITIVE;

		if (ui.cbTransparent->isChecked ())
		{
			ptexture->flags &= ~(STUDIO_NF_ADDITIVE|STUDIO_NF_CHROME);
			ptexture->flags |= STUDIO_NF_TRANSPARENT;
			ui.cbChrome->setChecked (false);
			ui.cbAdditive->setChecked (false);
		}
		else
			ptexture->flags &= ~STUDIO_NF_TRANSPARENT;

		if (ui.cbFullBright->isChecked ())
		{
			ptexture->flags |= STUDIO_NF_FULLBRIGHT;
		}
		else
			ptexture->flags &= ~STUDIO_NF_FULLBRIGHT;
	}
}

void QtGuiApplication1::updateTextureUVDisplay()
{
	g_viewerSettings.showUVMap = ui.cbUVMap->isChecked ();
	g_viewerSettings.showUVMapOverlay = ui.cbUVMapOverlay->isChecked ();
	g_viewerSettings.showSmoothLines = ui.cbAA->isChecked ();
}

void QtGuiApplication1::setTextureScale(int texScale)
{
	g_viewerSettings.textureScale = 1.0f + static_cast<float>(texScale) * 4.0f / 100.0f;
	ui.lTextureScale->setText (QString().sprintf("Scale Texture View (%.fx)", g_viewerSettings.textureScale));
	//d_GlWindow->redraw ();
}

void QtGuiApplication1::onImportTexture()
{
	try
	{
		const QString filename = getOpenFileImagePath(this);
		if(filename.isEmpty())
			return;

		const QImage qiOriginal = getOpenImage(filename); // may throw

		studiohdr_t *phdr = g_studioModel.getTextureHeader ();
		if (!phdr)
			throw std::runtime_error("studiohdr_t *phdr not available");

		mstudiotexture_t *ptexture = (mstudiotexture_t *) ((byte *) phdr + phdr->textureindex) + g_viewerSettings.texture;

		const QImage qi = qiOriginal.scaled(ptexture->width, ptexture->height).convertToFormat(QImage::Format::Format_Indexed8);
		if(qi.isNull())
			throw std::runtime_error("Can not transform texture");

		std::copy_n (qi.bits(), ptexture->width * ptexture->height, ((byte *) phdr + ptexture->index));

		// RGB_RGB_RGB_ => RGBRGB
		QVector<QRgb> qv = qi.colorTable();
		for(std::size_t i = 0; i< 256; ++i)
		{
			byte *p = (byte *) phdr + ptexture->index + ptexture->width * ptexture->height + 3 * i;
			QColor color = qv[i];
			p[0] = static_cast<byte>(color.red());
			p[1] = static_cast<byte>(color.green());
			p[2] = static_cast<byte>(color.blue());
		}

		g_studioModel.UploadTexture (ptexture, (byte *) phdr + ptexture->index, (byte *) phdr + ptexture->index + ptexture->width * ptexture->height, g_viewerSettings.texture + 3);

		ui.openglwidget->update ();
	}
	catch(const std::exception &e)
	{
		return QMessageBox::critical(this, "Error", e.what()), void();
	}
}

void QtGuiApplication1::onExportTexture()
{
	try
	{
		studiohdr_t *phdr = g_studioModel.getTextureHeader ();
		phdr ? void() : throw std::runtime_error("studiohdr_t *phdr not available");

		mstudiotexture_t *ptexture = (mstudiotexture_t *) ((byte *) phdr + phdr->textureindex) + g_viewerSettings.texture;
		ptexture ? void() : throw std::runtime_error("mstudiotexture_t *ptexture not available");

		QString filename = getSaveFileImagePath(this, {}, ptexture->name);
		if(filename.isEmpty())
			return;

		QImage qi(  ((byte *) phdr + ptexture->index),
		            ptexture->width,
		            ptexture->height,
		            QImage::Format::Format_Indexed8
		);
		qi.isNull() ? void() : throw std::runtime_error("cannot convert to QImage");

		// RGBRGB => RGB_RGB_RGB_
		{
			QVector<QRgb> qv(256);
			for(std::size_t i = 0; i< 256; ++i)
			{
				const byte *p = (byte *) phdr + ptexture->index + ptexture->width * ptexture->height + 3 * i;
				QRgb rgb = QColor(p[0], p[1], p[2]).rgb();
				qv[i] = rgb;
			}
			qi.setColorTable(std::move(qv));
		}

		saveImageTo(qi, filename);
	}
	catch (const std::exception & e)
	{
		return QMessageBox::critical(this, "Error", e.what()), void();
	}
}