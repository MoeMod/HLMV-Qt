#include "hlmv.h"
#include "ViewerSettings.h"
#include "StudioModel.h"

void QtGuiApplication1::initSequences ()
{
	studiohdr_t *hdr = g_studioModel.getStudioHeader ();
	if (hdr)
	{
		ui.cSequence->clear();
		ui.cWpSequence->clear ();
		for (int i = 0; i < hdr->numseq; i++)
		{
			mstudioseqdesc_t *pseqdescs = (mstudioseqdesc_t *) ((byte *) hdr + hdr->seqindex);
			ui.cSequence->addItem (pseqdescs[i].label);
			ui.cWpSequence->addItem (pseqdescs[i].label);
		}

		ui.cSequence->setCurrentRow (0);
		ui.cWpSequence->setCurrentIndex (0);
	}
}

void QtGuiApplication1::setSequence(int index)
{
	if(index < 0)
		return;

	ui.cSequence->setCurrentRow(index);
	ui.cWpSequence->setCurrentIndex (index);

	g_studioModel.SetSequence(index);
	g_viewerSettings.sequence = index;
	setSequenceInfo ();
	setEvent (index);
}

void QtGuiApplication1::setSequenceInfo ()
{
	char str[1024];

	studiohdr_t *hdr = g_studioModel.getStudioHeader ();

	if (!hdr)
		return;

	mstudioseqdesc_t *pseqdesc = (mstudioseqdesc_t *) ((byte *) hdr + hdr->seqindex) + g_viewerSettings.sequence;

	sprintf (str,
	         "Sequence#: %d\n"
	         "Frames: %d\n"
	         "FPS: %d\n"
	         "Blends: %d\n"
	         "# of events: %d\n",
	         g_viewerSettings.sequence,
	         pseqdesc->numframes,
	         (int)pseqdesc->fps,
	         pseqdesc->numblends,
	         pseqdesc->numevents
	);

	ui.lSequenceInfo1->setText (str);
	ui.lSequenceInfo2->setText (str);

	ui.slFrame->setRange(0, pseqdesc->numframes);
	ui.leFrame->setRange(0, pseqdesc->numframes);
	ui.slWeaponFrame->setRange(0, pseqdesc->numframes);
	ui.leWeaponFrame->setRange(0, pseqdesc->numframes);

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

