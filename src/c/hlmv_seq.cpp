#include "hlmv.h"
#include "ViewerSettings.h"
#include "StudioModel.h"
#include <QInputDialog>

void QtGuiApplication1::editSequence()
{
	studiohdr_t *hdr = g_studioModel.getStudioHeader();

	if (!hdr)
		return;

	if (g_viewerSettings.sequence >= hdr->numseq || g_viewerSettings.sequence < 0)
		return;

	mstudioseqdesc_t *pseqdesc = (mstudioseqdesc_t *)((byte *)hdr + hdr->seqindex) + g_viewerSettings.sequence;

	bool isOK = false;
	QString str = QString("%1, %2")
		.arg(pseqdesc->flags)
		.arg(pseqdesc->fps);

	QString text = QInputDialog::getMultiLineText(
		this,
		tr("Edit events"),
		tr("Format: flags, fps"),
		str,
		&isOK);

	if (isOK)
	{
		QStringList seqstr = text.split(",");
		if (seqstr.size() == 2)
		{
			int flags = seqstr[0].trimmed().toInt(&isOK);
			if (isOK)
				pseqdesc->flags = flags;
			float fps = seqstr[1].trimmed().toFloat(&isOK);
			if (isOK)
				pseqdesc->fps = fps;

			setSequenceInfo();
		}
	}
}

void QtGuiApplication1::initSequences ()
{
	studiohdr_t *hdr = g_studioModel.getStudioHeader();

	if (!hdr)
		return;

	ui.cSequence->clear();
	ui.cWpSequence->clear ();

	for (int i = 0; i < hdr->numseq; i++)
	{
		mstudioseqdesc_t *pseqdescs = (mstudioseqdesc_t *) ((byte *) hdr + hdr->seqindex);
		ui.cSequence->addItem(pseqdescs[i].label);
		ui.cWpSequence->addItem(pseqdescs[i].label);
	}

	ui.cSequence->setCurrentRow(0);
	ui.cWpSequence->setCurrentIndex(0);
}

void QtGuiApplication1::setSequence(int index)
{
	studiohdr_t *hdr = g_studioModel.getStudioHeader();

	if (!hdr)
		return;

	if (index >= hdr->numseq || index < 0)
		return;

	ui.cSequence->setCurrentRow(index);
	ui.cWpSequence->setCurrentIndex (index);

	g_studioModel.SetSequence(index);
	g_viewerSettings.sequence = index;
	setSequenceInfo();
	setEvent (index);
}

void QtGuiApplication1::setSequenceInfo()
{
	studiohdr_t *hdr = g_studioModel.getStudioHeader ();

	if (!hdr)
		return;

	if (g_viewerSettings.sequence >= hdr->numseq || g_viewerSettings.sequence < 0)
		return;

	mstudioseqdesc_t *pseqdesc = (mstudioseqdesc_t *) ((byte *) hdr + hdr->seqindex) + g_viewerSettings.sequence;

	auto str = QString(
		"Sequence: %1\n"
		"Flags: %2\n"
		"FPS: %3\n"		
		"NumofFrames: %4\n"
		"NumofBlends: %5\n"
		"NumofEvents: %6\n")
		.arg(g_viewerSettings.sequence)
		.arg(pseqdesc->flags)
		.arg(pseqdesc->fps)
		.arg(pseqdesc->numframes)
		.arg(pseqdesc->numblends)
		.arg(pseqdesc->numevents);

	ui.lSequenceInfo1->setText(str);
	ui.lSequenceInfo2->setText(str);

	ui.slFrame->setRange(0, pseqdesc->numframes - 1);
	ui.leFrame->setRange(0, pseqdesc->numframes - 1);
	ui.slWeaponFrame->setRange(0, pseqdesc->numframes - 1);
	ui.leWeaponFrame->setRange(0, pseqdesc->numframes - 1);

	ui.slWpSpeedFPS->setValue(pseqdesc->fps);
	ui.slWpSpeedFPS->setRange(0, pseqdesc->fps * 10);
}

void QtGuiApplication1::OnSetAnimationPlaying(bool play)
{
	ui.bPlaySequence->setChecked(play);
	ui.bPlaySequence2->setChecked(play);

	ui.openglwidget->setPaused(!play);

	int showcurrent = play ? 0 : g_studioModel.SetFrame (-1);

	ui.slFrame->setValue(showcurrent);
	ui.leFrame->setValue(showcurrent);
	ui.slWeaponFrame->setValue(showcurrent);
	ui.leWeaponFrame->setValue(showcurrent);

	ui.slFrame->setEnabled (!play);
	ui.leFrame->setEnabled (!play);
	ui.slWeaponFrame->setEnabled (!play);
	ui.leWeaponFrame->setEnabled (!play);
}

void QtGuiApplication1::OnSetAnimationCurrentFrame(int v)
{
	ui.slFrame->setValue (v);
	ui.leFrame->setValue (v);
	ui.slWeaponFrame->setValue (v);
	ui.leWeaponFrame->setValue (v);

	g_studioModel.SetFrame (v);
}

void QtGuiApplication1::OnSetAnimationFPS(double fps)
{
	studiohdr_t *hdr = g_studioModel.getStudioHeader ();
	if (!hdr)
		return;
	mstudioseqdesc_t *pseqdesc = (mstudioseqdesc_t *) ((byte *) hdr + hdr->seqindex) + g_viewerSettings.sequence;
	//g_viewerSettings.speedScale = static_cast<float>(fps) / pseqdesc->fps;
	pseqdesc->fps = fps;

}

