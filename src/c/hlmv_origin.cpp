#include "hlmv.h"
#include "ViewerSettings.h"
#include "StudioModel.h"


void QtGuiApplication1::updateWeaponOriginSettings()
{
	g_viewerSettings.showCrosshair = ui.cbCrossHair->isChecked();
	g_viewerSettings.showGuideLines = ui.cbGuildlines->isChecked();
	g_viewerSettings.righthand = ui.cbRightHand->isChecked();

	g_viewerSettings.vieworigintrans[0] = static_cast<float>(ui.leOriginX->value());
	g_viewerSettings.vieworigintrans[1] = static_cast<float>(ui.leOriginY->value());
	g_viewerSettings.vieworigintrans[2] = static_cast<float>(ui.leOriginZ->value());

}