
#ifndef HLMV_QT_IMAGE_H
#define HLMV_QT_IMAGE_H

#include <QImage>

QString getOpenFileImagePath(QWidget *parent = Q_NULLPTR,
                             const QString &caption = QString(),
                             const QString &dir = QString());

QString getSaveFileImagePath(QWidget *parent = Q_NULLPTR,
                             const QString &caption = QString(),
                             const QString &dir = QString());

QImage getOpenImage(const QString &filename) noexcept(false);
void saveImageTo(const QImage& what, const QString &filename) noexcept(false);

#endif //HLMV_QT_IMAGE_H
