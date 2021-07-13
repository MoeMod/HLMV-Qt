#include "hlmv.h"
#include "ViewerSettings.h"
#include "StudioModel.h"
#include <QVariant>
#include <QInputDialog>

void QtGuiApplication1::editEvents()
{
	bool ok = false;

	studiohdr_t *hdr = g_studioModel.getStudioHeader();

	if (!hdr)
	{
		return;
	}

	int seq = ui.cEvent->currentData().toInt(&ok);

	if (!ok)
	{
		seq = ui.cSequence->currentRow();
		if (seq < 0 || seq >= hdr->numseq)
			return;
	}

	QString str;

	mstudioseqdesc_t *pseqdescs = (mstudioseqdesc_t *)(((byte *)hdr + hdr->seqindex)) + seq;

	for (int i = 0; i < pseqdescs->numevents; ++i)
	{
		mstudioevent_t *pevents = (mstudioevent_t *)((byte *)hdr + pseqdescs->eventindex) + i;

		str += QString("%1, %2, %3, %4\n")
			.arg(pevents->frame)
			.arg(pevents->event)
			.arg(pevents->type)
			.arg(pevents->event == 5004 ? QString(pevents->options) : "");
	}
	
	bool isOK = false;
	QString text = QInputDialog::getMultiLineText(
		this,
		tr("Edit events"),
		tr("Format: frame, event, type, options (string)"),
		str,
		&isOK);

	if (isOK)
	{
		std::vector<mstudioevent_t> evlist;

		QStringList lines = text.split("\n");

		for (int i = 0; i < lines.size(); ++i)
		{
			QStringList evstr = lines[i].split(",");
			if (evstr.size() == 4)
			{
				mstudioevent_t ev = {0};

				ev.frame = evstr[0].trimmed().toInt(&isOK);
				if (!isOK)
					continue;

				ev.event = evstr[1].trimmed().toInt(&isOK);
				if (!isOK)
					continue;

				ev.type = evstr[2].trimmed().toInt(&isOK);
				if (!isOK)
					continue;

				if (ev.event == 5004)
				{
					auto optstr = evstr[3].trimmed().toLocal8Bit();
					memcpy(ev.options, optstr.data(), std::min(optstr.size(), 63) );
				}
				evlist.emplace_back(ev);
			}
		}

		if (evlist.size() > 0)
		{
			if (pseqdescs->numevents < evlist.size())
			{
				int original_length = hdr->texturedataindex;
				auto original_texturedata = ((byte *)hdr + hdr->texturedataindex);
				int texturedata_length = hdr->length - hdr->texturedataindex;

				int extend_length = sizeof(mstudioevent_t) * evlist.size();
				hdr->texturedataindex += extend_length;
				hdr->length += extend_length;

				auto new_texturedata = ((byte *)hdr + hdr->texturedataindex);
				memmove(new_texturedata, original_texturedata, texturedata_length);

				pseqdescs->eventindex = original_length;

				auto ptexture = (mstudiotexture_t *)((byte *)hdr + hdr->textureindex);
				for (int i = 0; i < hdr->numtextures; i++)
				{
					ptexture->index += extend_length;
					ptexture++;
				}
			}

			if (pseqdescs->eventindex)
			{
				mstudioevent_t *pevents_new = (mstudioevent_t *)((byte *)hdr + pseqdescs->eventindex);
				memcpy(pevents_new, evlist.data(), sizeof(mstudioevent_t) * evlist.size());
			}
		}
		pseqdescs->numevents = evlist.size();
	}
}

void QtGuiApplication1::setEventInfo(int index)
{
	QString str;

	studiohdr_t *hdr = g_studioModel.getStudioHeader();

	if (!hdr)
	{
		ui.lEventInfo->setText("");
		return;
	}

	bool ok = false;

	int seq = ui.cEvent->currentData().toInt(&ok);

	if (!ok)
	{
		ui.lEventInfo->setText("");
		return;
	}

	mstudioseqdesc_t *pseqdescs = (mstudioseqdesc_t *) (((byte *)hdr + hdr->seqindex)) + seq;

	if (index >= 0 && index < pseqdescs->numevents)
	{
		mstudioevent_t *pevents = (mstudioevent_t *) ((byte *)hdr + pseqdescs->eventindex) + index;

		str = QString("Frame: %1, Event: %2, Type: %3\nOptions: %4")
			.arg(pevents->frame)
			.arg(pevents->event)
			.arg(pevents->type)
			.arg(pevents->event == 5004 ? QString(pevents->options) : "");
	}

	ui.lEventInfo->setText (str);
}

void QtGuiApplication1::setEvent (int seq)
{
	char str[64];

	studiohdr_t *hdr = g_studioModel.getStudioHeader ();

	if (!hdr)
		return;

	mstudioseqdesc_t *pseqdescs = (mstudioseqdesc_t *) ((byte *)hdr + hdr->seqindex);

	ui.cEvent->clear ();
	for (int i = 0; i < pseqdescs[seq].numevents; i++)
	{
		sprintf(str, "Event %d", i);
		ui.cEvent->addItem(str, QVariant(seq));
	}
	ui.cEvent->setCurrentIndex (0);
	setEventInfo(0);
}