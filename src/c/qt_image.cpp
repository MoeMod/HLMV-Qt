#include "qt_image.h"

#include "TGAlib.h"

#include <QFileDialog>
#include <QMessageBox>

QString getOpenFileImagePath(QWidget *parent, const QString &caption, const QString &dir)
{
	return QFileDialog::getOpenFileName(parent, caption, dir, "Image file(*.bmp;*.jpg;*.png;*.tiff;*.gif;*.tga);;BMP file (*.bmp);;JPG file (*.jpg);;PNG file (*.png);;TIFF file (*.tiff);;GIF file (*.gif);;TGA file (*.tga)");
}

QString getSaveFileImagePath(QWidget *parent, const QString &caption, const QString &dir)
{
	return QFileDialog::getSaveFileName(parent, caption, dir, "BMP file (*.bmp);;JPG file (*.jpg);;PNG file (*.png);;TIFF file (*.tiff);;GIF file(*.gif)");
}

QImage getOpenImage(const QString &filename) noexcept(false)
{
	if(filename.isEmpty())
		throw std::runtime_error("empty path");

	const QFileInfo file(filename);
	if(!file.exists())
		throw std::runtime_error("Image not exists : " + filename.toStdString());

	if(!file.suffix().compare("tga", Qt::CaseInsensitive))
	{
		// note that const char * will expire on next line
		std::unique_ptr<tImageTGA, QImageCleanupFunction> tga(
				tgaLoad(file.absoluteFilePath().toStdString().c_str()),
				+[](void *p){tgaDestroy(reinterpret_cast<tImageTGA *>(p));}
		);

		if(!tga || tga->status != TGA_OK)
			throw std::runtime_error("Error reading tga file");

		QImage::Format format = QImage::Format_Invalid;
		if(tga->pixelDepth == 32)
			format = QImage::Format_RGBA8888;
		else if(tga->pixelDepth == 24)
			format = QImage::Format_RGB888;

		if(format == QImage::Format_Invalid)
			throw std::runtime_error("TGA format not supported");

		return QImage(tga->imageData, tga->width, tga->height, format, tga.get_deleter(), tga.release());
	}
	else
	{
		const QImage qiOriginal(file.absoluteFilePath());
		if(qiOriginal.isNull())
			throw std::runtime_error("Image not available");

		return qiOriginal;
	}
}

void saveImageTo(const QImage& what, const QString &filename) noexcept(false)
{
	return what.save(filename) ? void() : throw("Error writing texture to : " + filename.toStdString());
}
