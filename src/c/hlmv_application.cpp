#include "hlmv_application.h"
#include "ViewerSettings.h"

#include <QFileOpenEvent>

HLMVApplication::HLMVApplication(int &argc, char **argv) : QApplication(argc, argv)
{
	InitViewerSettings();

	setAttribute(Qt::AA_EnableHighDpiScaling, true);
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

}

bool HLMVApplication::event(QEvent *event)
{
	if (event->type() == QEvent::FileOpen)
	{
		QString filename = static_cast<QFileOpenEvent *>(event)->file();
		try
		{
			w.OpenFile(filename);
		}
		catch (...)
		{

		}
		return true;
	}

	return QApplication::event(event);
}
