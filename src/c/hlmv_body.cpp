#include "hlmv.h"
#include "ViewerSettings.h"
#include "StudioModel.h"

void QtGuiApplication1::initBodyparts ()
{
	studiohdr_t *hdr = g_studioModel.getStudioHeader ();
	if (hdr)
	{
		int i;
		mstudiobodyparts_t *pbodyparts = (mstudiobodyparts_t *) ((byte *) hdr + hdr->bodypartindex);

		ui.cBodypart->clear ();
		if (hdr->numbodyparts > 0)
		{
			for (i = 0; i < hdr->numbodyparts; i++)
				ui.cBodypart->addItem (pbodyparts[i].name);

			ui.cBodypart->setCurrentIndex (0);
			setBodypart(0);

			ui.cSubmodel->clear ();
			for (i = 0; i < pbodyparts[0].nummodels; i++)
			{
				char str[64];
				sprintf (str, "Submodel %d", i + 1);
				ui.cSubmodel->addItem (str);
			}
			ui.cSubmodel->setCurrentIndex (0);
			setSubmodel(0);
		}
	}
}



void QtGuiApplication1::setBodypart (int index)
{
	if(index < 0)
		return;

	studiohdr_t *hdr = g_studioModel.getStudioHeader ();
	if (hdr)
	{
		ui.cBodypart->setCurrentIndex (index);
		if (index < hdr->numbodyparts)
		{
			mstudiobodyparts_t *pbodyparts = (mstudiobodyparts_t *) ((byte *) hdr + hdr->bodypartindex);
			ui.cSubmodel->clear ();

			for (int i = 0; i < pbodyparts[index].nummodels; i++)
			{
				char str[64];
				sprintf (str, "Submodel %d", i + 1);
				ui.cSubmodel->addItem (str);
			}
			ui.cSubmodel->setCurrentIndex (0);
			setSubmodel(0);
		}
	}
}

void QtGuiApplication1::setSubmodel (int index)
{
	if(index < 0)
		return;

	g_studioModel.SetBodygroup (ui.cBodypart->currentIndex (), index);
	g_viewerSettings.submodels[ui.cBodypart->currentIndex ()] = index;
}

void QtGuiApplication1::initBoneControllers ()
{
	studiohdr_t *hdr = g_studioModel.getStudioHeader ();
	if (hdr)
	{
		ui.cController->setEnabled (hdr->numbonecontrollers > 0);
		ui.slController->setEnabled (hdr->numbonecontrollers > 0);
		ui.cController->clear ();

		mstudiobonecontroller_t *pbonecontrollers = (mstudiobonecontroller_t *) ((byte *) hdr + hdr->bonecontrollerindex);
		for (int i = 0; i < hdr->numbonecontrollers; i++)
		{
			char str[32];
			if (pbonecontrollers[i].index == 4)
				sprintf (str, "Mouth");
			else
				sprintf (str, "Controller %d", pbonecontrollers[i].index);
			ui.cController->addItem (str);
		}

		if (hdr->numbonecontrollers > 0)
		{
			ui.cController->setCurrentIndex (0);
			ui.slController->setRange ((int) pbonecontrollers[0].start, (int) pbonecontrollers[0].end);
			ui.slController->setValue (0);
		}

	}
}

void QtGuiApplication1::setBoneController (int index)
{
	if(index < 0)
		return;

	studiohdr_t *hdr = g_studioModel.getStudioHeader ();
	if (hdr)
	{
		mstudiobonecontroller_t *pbonecontrollers = (mstudiobonecontroller_t *) ((byte *) hdr + hdr->bonecontrollerindex);
		ui.slController->setRange ((int) pbonecontrollers[index].start, (int) pbonecontrollers[index].end);
		ui.slController->setValue (0);
	}
}



void QtGuiApplication1::setBoneControllerValue (int index, float value)
{
	studiohdr_t *hdr = g_studioModel.getStudioHeader ();
	if (hdr)
	{
		mstudiobonecontroller_t *pbonecontrollers = (mstudiobonecontroller_t *) ((byte *) hdr + hdr->bonecontrollerindex);
		if (pbonecontrollers[index].index == 4)
			g_studioModel.SetMouth (value);
		else
			g_studioModel.SetController (pbonecontrollers[index].index, value);

		g_viewerSettings.controllers[index] = value;
	}
}

void QtGuiApplication1::setBoneControllerCurrentValue (int value)
{
	if(!ui.cController->count())
		return;
	setBoneControllerValue(ui.cController->currentIndex(), static_cast<float>(value));
}

void QtGuiApplication1::initSkins ()
{
	studiohdr_t *hdr = g_studioModel.getTextureHeader ();
	if (hdr)
	{
		ui.cSkin->setEnabled (hdr->numskinfamilies > 0);
		ui.cSkin->clear ();

		for (int i = 0; i < hdr->numskinfamilies; i++)
		{
			char str[32];
			sprintf (str, "Skin %d", i + 1);
			ui.cSkin->addItem (str);
		}

		ui.cSkin->setCurrentIndex (0);
		setSkin(0);
	}
}

void QtGuiApplication1::setModelInfo ()
{
	char str[1024];

	studiohdr_t *studioHdr = g_studioModel.getStudioHeader ();
	studiohdr_t *textureHdr = g_studioModel.getTextureHeader ();

	if (!studioHdr || !textureHdr)
		return;

	sprintf (str,
	         "Bones: %d\n"
	         "Bone Controllers: %d\n"
	         "Hit Boxes: %d\n"
	         "Sequences: %d\n"
	         "Sequence Groups: %d\n",
	         studioHdr->numbones,
	         studioHdr->numbonecontrollers,
	         studioHdr->numhitboxes,
	         studioHdr->numseq,
	         studioHdr->numseqgroups
	);

	ui.lModelInfo1->setText(str);

	sprintf (str,
	         "Textures: %d\n"
	         "Skin Families: %d\n"
	         "Bodyparts: %d\n"
	         "Attachments: %d\n"
	         "Transitions: %d\n",
	         textureHdr->numtextures,
	         textureHdr->numskinfamilies,
	         studioHdr->numbodyparts,
	         studioHdr->numattachments,
	         studioHdr->numtransitions);

	ui.lModelInfo2->setText (str);
}

void QtGuiApplication1::setSkin (int index)
{
	if(index < 0)
		return;

	g_studioModel.SetSkin (index);
	g_viewerSettings.skin = index;
}


