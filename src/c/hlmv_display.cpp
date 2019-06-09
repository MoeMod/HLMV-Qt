#include "hlmv.h"
#include "ViewerSettings.h"
#include "StudioModel.h"

void QtGuiApplication1::updateViewSettings()
{
	g_viewerSettings.renderMode = ui.cRenderMode->currentIndex();

	int value = ui.slTransparency->value ();
	g_viewerSettings.transparency = (float) value / 100.0f;
	ui.lTransparency->setText (QString().sprintf("Opacity: %d%%", value));

	g_viewerSettings.mirror = ui.cbMirror->isChecked();
	g_viewerSettings.showGround = ui.cbGround->isChecked();
	g_viewerSettings.showBackground = ui.cbBackground->isChecked();
	g_viewerSettings.showWireframeOverlay = ui.cbWireframeOverlay->isChecked();

	g_viewerSettings.useStencil = (!g_viewerSettings.use3dfx && g_viewerSettings.mirror);

	g_viewerSettings.showHitBoxes = ui.cbHitBoxes->isChecked();
	g_viewerSettings.showBones = ui.cbBones->isChecked();
	g_viewerSettings.showAttachments = ui.cbAttachments->isChecked();
	g_viewerSettings.showEyePosition = ui.cbEyePosition->isChecked();

	g_studioModel.scaleMeshes (static_cast<float>(ui.leMeshScale->value()));
	g_studioModel.scaleBones (static_cast<float>(ui.leBoneScale->value()));

	ui.openglwidget->update ();
}