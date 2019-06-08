#include "hlmv.h"
#include "ViewerSettings.h"
#include "StudioModel.h"

void QtGuiApplication1::setEventInfo(int index)
{
	char str[1024];

	studiohdr_t *hdr = g_studioModel.getStudioHeader ();

	if (!hdr)
		return;

	mstudioseqdesc_t *pseqdescs = (mstudioseqdesc_t *) ((byte *)hdr + hdr->seqindex);

	if(!ui.cEvent->count())
	{
		ui.lEventInfo->setText ("");
		return;
	}

	if (pseqdescs->numevents)
	{
		mstudioevent_t *pevents = (mstudioevent_t *) ((byte *)hdr + pseqdescs->eventindex) + index;

		sprintf (str,
		         "Frame: %d    "
		         "Event: %d    "
		         "Type: %d\n"
		         "Options: %s",
		         pevents->frame,
		         pevents->event,
		         pevents->type,
		         pevents->options
		);
	}
	else
	{
		str[0] = '\0';
	}
	ui.lEventInfo->setText (str);
}

void QtGuiApplication1::setEvent (int seq)
{
	char str[64];

	studiohdr_t *hdr = g_studioModel.getStudioHeader ();

	if (!hdr)
		return;

	mstudioseqdesc_t *pseqdescs = (mstudioseqdesc_t *) ((byte *)hdr + hdr->seqindex);  //g_viewerSettings.sequence;

	ui.cEvent->clear ();
	if (0 < pseqdescs[seq].numevents)
	{
		for (int i = 1; i <= pseqdescs[seq].numevents; i++)
		{
			sprintf (str, "Event %d", i);
			ui.cEvent->addItem (str);
		}
	}
	ui.cEvent->setCurrentIndex (0);
	setEventInfo(0);
}