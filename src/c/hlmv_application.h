#ifndef HLMV_APPLICATION_H
#define HLMV_APPLICATION_H

#include <QApplication>
#include "v/hlmv.h"

class HLMVApplication : public QApplication
{
public:
	HLMVApplication(int &argc, char **argv);

public:
	bool event(QEvent *event) override;

public:
	QtGuiApplication1 w;
};

#endif //HLMV_APPLICATION_H
