#include "v/hlmv.h"
#include "hlmv_application.h"
#include "ViewerSettings.h"

int main(int argc, char *argv[])
{
	InitViewerSettings();

	HLMVApplication a(argc, argv);

	return a.exec();
}
