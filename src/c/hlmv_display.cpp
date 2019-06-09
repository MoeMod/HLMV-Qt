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

void QtGuiApplication1::centerView ()
{
	float min[3], max[3];
	g_studioModel.ExtractBbox (min, max);

	float dx = max[0] - min[0];
	float dy = max[1] - min[1];
	float dz = max[2] - min[2];
	float d = dx;
	if (dy > d)
		d = dy;
	if (dz > d)
		d = dz;
	g_viewerSettings.trans[0] = 0;
	g_viewerSettings.trans[1] = min[2] + dz / 2;
	g_viewerSettings.trans[2] = d * 1.0f;
	g_viewerSettings.rot[0] = -90.0f;
	g_viewerSettings.rot[1] = -90.0f;
	g_viewerSettings.rot[2] = 0.0f;
	ui.centralWidget->update ();
}
