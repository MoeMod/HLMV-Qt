#include "hlmv.h"
#include "ViewerSettings.h"

void QtGuiApplication1::OnTabChanged(int n)
{
	g_viewerSettings.showTexture = (n == 2);

	if (n == 4)
	{
		g_viewerSettings.vieworiginmode = true;
		g_viewerSettings.yaw = 74.0f;
	}
	else
	{
		g_viewerSettings.yaw = 65.0f;
		g_viewerSettings.vieworiginmode = false;
	}
}

