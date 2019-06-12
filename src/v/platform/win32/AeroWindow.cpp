#include "hlmv.h"
#include <QtWin>
#include <QOperatingSystemVersion>

void QtGuiApplication1::SetupAeroWindow()
{
	//this->menuBar()->setVisible(false);
	//ui.centralWidget->layout()->addItem(ui.centralWidget->layout()->takeAt(0));
	if (QOperatingSystemVersion::current() <= QOperatingSystemVersion::Windows7)
	{
		if (QtWin::isCompositionEnabled())
		{
			QtWin::extendFrameIntoClientArea(this, -1, -1, -1, -1);
			setAttribute(Qt::WA_TranslucentBackground, true);
			setAttribute(Qt::WA_NoSystemBackground, false);
		}
		else
		{
			setAttribute(Qt::WA_TranslucentBackground, false);
		}
	}
}