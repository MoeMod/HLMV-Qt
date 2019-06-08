#include "v/hlmv.h"
#include <QtWidgets/QApplication>
#include "ViewerSettings.h"

int main(int argc, char *argv[])
{
	InitViewerSettings();

	QApplication a(argc, argv);
	QtGuiApplication1 w;
	w.show();
	if(argc >= 2)
	{
		try
		{
			w.OpenFile(argv[1]);
		}
		catch(...)
		{
			// eat up the file-opening exception
			// and then invalid argument will be ignored
		}
	}
	return a.exec();
}
