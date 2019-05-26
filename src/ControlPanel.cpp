//
//                 Half-Life Model Viewer (c) 1999 by Mete Ciragan
//
// file:           ControlPanel.cpp
// last modified:  Oct 20 1999, Mete Ciragan
// copyright:      The programs and associated files contained in this
//                 distribution were developed by Mete Ciragan. The programs
//                 are not in the public domain, but they are freely
//                 distributable without licensing fees. These programs are
//                 provided without guarantee or warrantee expressed or
//                 implied.
//
// version:        1.24
//
// email:          mete@swissquake.ch
// web:            http://www.swissquake.ch/chumbalum-soft/
//
#include "ControlPanel.h"
#include "ViewerSettings.h"
#include "StudioModel.h"
#include "GlWindow.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <mx/mx.h>
#include <mx/mxBmp.h>
#include <mx/mxShellExec.h>


extern const char *g_appTitle;



bool swap3dfxgl (bool b);


ControlPanel *g_ControlPanel;
bool g_bStopPlaying;
static int g_nCurrFrame;



ControlPanel::ControlPanel (mxWindow *parent)
: mxWindow (parent, 0, 0, 0, 0, "Control Panel", mxWindow::Normal)
{
	InitViewerSettings ();

	// create tabcontrol with subdialog windows
	tab = new mxTab (this, 0, 0, 0, 0, IDC_TAB);
#ifdef WIN32
	SetWindowLong ((HWND) tab->getHandle (), GWL_EXSTYLE, WS_EX_CLIENTEDGE);
#endif

	mxWindow *wRender = new mxWindow (this, 0, 0, 0, 0);
	tab->add (wRender, "Model Display");
	new mxLabel(wRender, 5, 3, 90, 18, "Render Mode");
	cRenderMode = new mxChoice (wRender, 5, 17, 112, 22, IDC_RENDERMODE);
	cRenderMode->add ("Wireframe");
	cRenderMode->add ("Flat Shaded");
	cRenderMode->add ("Smooth Shaded");
	cRenderMode->add ("Texture Shaded");
	cRenderMode->select (3);
	mxToolTip::add (cRenderMode, "Select Render Mode");
	slTransparency = new mxSlider (wRender, 0, 62, 120, 18, IDC_TRANSPARENCY);
	slTransparency->setRange (0, 100);
	slTransparency->setValue (100);
	lTransparency = new mxLabel(wRender, 5, 45, 120, 18, "Opacity: 100%");
	mxToolTip::add (slTransparency, "Model Opacity");
	cbHitBoxes = new mxCheckBox (wRender, 140, 5, 100, 20, "Show Hit Boxes", IDC_HITBOXES);
	cbBones = new mxCheckBox (wRender, 140, 25, 100, 20, "Show Bones", IDC_BONES);
	cbAttachments = new mxCheckBox (wRender, 140, 45, 110, 20, "Show Attachments", IDC_ATTACHMENTS);
	cbEyePosition = new mxCheckBox (wRender, 140, 65, 110, 20, "Show Eye Position", IDC_EYEPOS);
	cbGround = new mxCheckBox (wRender, 260, 5, 110, 20, "Show Ground", IDC_GROUND);
	cbMirror = new mxCheckBox (wRender, 260, 25, 150, 20, "Mirror Model On Ground", IDC_MIRROR);
	cbBackground = new mxCheckBox (wRender, 260, 45, 110, 20, "Show Background", IDC_BACKGROUND);
	cbWireframeOverlay = new mxCheckBox (wRender, 260, 65, 110, 20, "Wireframe Overlay", IDC_OVERLAYWIREWRAME);

#ifdef HAVE_SCALE
	leMeshScale = new mxLineEdit (wRender, 430, 5, 50, 18, "1.0");
	mxToolTip::add (leMeshScale, "Mesh Scale");
	leBoneScale = new mxLineEdit (wRender, 430, 25, 50, 18, "1.0");
	mxToolTip::add (leBoneScale, "Bone Scale");
	mxButton *bMeshScale = new mxButton (wRender, 485, 5, 50, 18, "Scale Mesh", 10001);
	mxButton *bBoneScale = new mxButton (wRender, 485, 25, 50, 18, "Scale Bones", 10002);
#endif

	lDrawnPolys = new mxLabel (wRender, 430, 65, 112, 18, "Drawn Polys: 0");

	mxWindow *wBody = new mxWindow (this, 0, 0, 0, 0);
	tab->add (wBody, "Body Parts");

	new mxLabel (wBody, 120, 8, 60, 18, "Part");
	cBodypart = new mxChoice (wBody, 5, 5, 112, 22, IDC_BODYPART);
	mxToolTip::add (cBodypart, "Choose a body part");

	new mxLabel (wBody, 120, 33, 60, 18, "Sub-model");
	cSubmodel = new mxChoice (wBody, 5, 30, 112, 22, IDC_SUBMODEL);
	mxToolTip::add (cSubmodel, "Choose a sub-model of the current body part");

	new mxLabel (wBody, 120, 58, 60, 18, "Skin");
	cSkin = new mxChoice (wBody, 5, 55, 100, 22, IDC_SKINS);
	mxToolTip::add (cSkin, "Choose a skin family");

	new mxLabel (wBody, 305, 8, 90, 18, "Controller");
	cController = new mxChoice (wBody, 190, 5, 112, 22, IDC_CONTROLLER);	
	mxToolTip::add (cController, "Choose a bone controller");

	new mxLabel (wBody, 305, 33, 50, 18, "Value");
	slController = new mxSlider (wBody, 185, 30, 117, 18, IDC_CONTROLLERVALUE);
	slController->setRange (0, 45);
	mxToolTip::add (slController, "Change current bone controller value");

	lModelInfo1 = new mxLabel (wBody, 370, 12, 120, 100, "");
	lModelInfo2 = new mxLabel (wBody, 500, 12, 120, 100, "");

	lDrawnPolys2 = new mxLabel (wBody, 190, 58, 112, 18, "Drawn Polys: 0");

	mxWindow *wTexture = new mxWindow (this, 0, 0, 0, 0);
	tab->add (wTexture, "Textures");

	lTexSize = new mxLabel (wTexture, 5, 3, 150, 15, "Texture");
	cTextures = new mxChoice (wTexture, 5, 18, 150, 22, IDC_TEXTURES);
	mxToolTip::add (cTextures, "Choose a texture");

	lTextureScale = new mxLabel (wTexture, 5, 47, 150, 15, "Scale Texture View (1x)");
	slTextureScale = new mxSlider (wTexture, 0, 60, 160, 18, IDC_TEXTURESCALE);
	slTextureScale->setRange (0, 100);
	slTextureScale->setSteps (25, 1);
	mxToolTip::add (slTextureScale, "Scale texture view size");
	cbChrome = new mxCheckBox (wTexture, 180, 5, 70, 18, "Chrome", IDC_CHROME);
	cbAdditive = new mxCheckBox (wTexture, 180, 25, 70, 18, "Additive", IDC_ADDITIVE);
	cbTransparent = new mxCheckBox (wTexture, 180, 45, 75, 18, "Transparent", IDC_TRANSPARENT);
	cbUVMap = new mxCheckBox (wTexture, 300, 5, 90, 18, "Show UV Map", IDC_UVMAP);
	cbUVMapOverlay = new mxCheckBox (wTexture, 300, 25, 112, 18, "Overlay UV Map", IDC_OVERLAYUVMAP);
	cbAA = new mxCheckBox (wTexture, 300, 45, 112, 18, "Anti-Alias Lines", IDC_ANTIALIASING);
	cMesh = new mxChoice (wTexture, 300, 65, 112, 22, IDC_MESH);
	mxToolTip::add (cMesh, "Choose which mesh to show the UVMap for");
	new mxButton (wTexture, 450, 5, 80, 18, "Import Texture", IDC_IMPORTTEXTURE);
	new mxButton (wTexture, 450, 25, 80, 18, "Export Texture", IDC_EXPORTTEXTURE);
	new mxButton (wTexture, 450, 45, 80, 18, "Export UV Map", IDC_EXPORTUVMAP);

	mxWindow *wSequence = new mxWindow (this, 0, 0, 0, 0);
	tab->add (wSequence, "Sequences");
	new mxLabel (wSequence, 5, 3, 200, 15, "Animation Sequence");
	cSequence = new mxChoice (wSequence, 5, 18, 200, 22, IDC_SEQUENCE);
	mxToolTip::add (cSequence, "Select an animation sequence");
	tbStop = new mxToggleButton (wSequence, 5, 46, 60, 18, "Stop", IDC_STOP);
	mxToolTip::add (tbStop, "Stop Playing");
	bPrevFrame = new mxButton (wSequence, 84, 46, 30, 18, "<<", IDC_PREVFRAME);
	bPrevFrame->setEnabled (false);
	mxToolTip::add (bPrevFrame, "Prev Frame");
	leFrame = new mxLineEdit (wSequence, 119, 46, 50, 18, "", IDC_FRAME); 
	leFrame->setEnabled (false);
	mxToolTip::add (leFrame, "Set Frame");
	bNextFrame = new mxButton (wSequence, 174, 46, 30, 18, ">>", IDC_NEXTFRAME);
	bNextFrame->setEnabled (false);
	mxToolTip::add (bNextFrame, "Next Frame");
	slSpeedScale = new mxSlider (wSequence, 0, 70, 165, 18, IDC_SPEEDSCALE);
        slSpeedScale->setRange (0, 200);
        slSpeedScale->setValue (40);
        mxToolTip::add (slSpeedScale, "Playback speed Scale");
	new mxLabel (wSequence, 170, 70, 40, 15, "Speed");
	lSequenceInfo1 = new mxLabel (wSequence, 228, 12, 90, 100, "");
	new mxLabel (wSequence, 320, 3, 40, 15, "Events");
	cEvent = new mxChoice (wSequence, 320, 18, 100, 22, IDC_EVENT);
	mxCheckBox *cbPlaySound = new mxCheckBox (wSequence, 320, 45, 75, 18, "Play Sound", IDC_PLAYSOUND);
	lEventInfo = new mxLabel (wSequence, 440, 12, 340, 100, "");

	mxWindow *wWeaponOrigin = new mxWindow (this, 0, 0, 0, 0);
	tab->add (wWeaponOrigin, "Weapon Origin");
	new mxLabel (wWeaponOrigin, 5, 3, 200, 15, "Animation Sequence");
	cWpSequence = new mxChoice (wWeaponOrigin, 5, 18, 200, 22, IDC_WEAPONSEQUENCE);
	mxToolTip::add (cWpSequence, "Select an animation sequence");
	tbWpStop = new mxToggleButton (wWeaponOrigin, 5, 46, 60, 18, "Stop", IDC_WEAPONSTOP);
	mxToolTip::add (tbWpStop, "Stop Playing");
	mxButton *bWpPrevFrame = new mxButton (wWeaponOrigin, 84, 46, 30, 18, "<<", IDC_PREVFRAME);
	bWpPrevFrame->setEnabled (false);
	mxToolTip::add (bWpPrevFrame, "Prev Frame");
	leWpFrame = new mxLineEdit (wWeaponOrigin, 119, 46, 50, 18, "", IDC_WEAPONFRAME);
	leWpFrame->setEnabled (false);
	mxToolTip::add (leWpFrame, "Set Frame");
	mxButton *bWpNextFrame = new mxButton (wWeaponOrigin, 174, 46, 30, 18, ">>", IDC_NEXTFRAME);
	bWpNextFrame->setEnabled (false);
	mxToolTip::add (bWpNextFrame, "Next Frame");
	mxSlider *slWpSpeedScale = new mxSlider (wWeaponOrigin, 0, 70, 165, 18, IDC_SPEEDSCALE);
        slWpSpeedScale->setRange (0, 200);
        slWpSpeedScale->setValue (40);
        mxToolTip::add (slWpSpeedScale, "Playback speed Scale");
	new mxLabel (wWeaponOrigin, 170, 70, 40, 15, "Speed");
	lSequenceInfo2 = new mxLabel (wWeaponOrigin, 228, 12, 90, 100, "");
	leOriginX = new mxLineEdit (wWeaponOrigin, 320, 5, 72, 18, "0.000000");
	new mxLabel (wWeaponOrigin, 400, 7, 150, 15, "Origin X");
	leOriginY = new mxLineEdit (wWeaponOrigin, 320, 26, 72, 18, "0.000000");
	new mxLabel (wWeaponOrigin, 400, 28, 150, 15, "Origin Y");
	leOriginZ = new mxLineEdit (wWeaponOrigin, 320, 47, 72, 18, "0.000000");
	new mxLabel (wWeaponOrigin, 400, 49, 150, 15, "Origin Z");
	new mxButton (wWeaponOrigin, 320, 68, 72, 18, "Test Origins", IDC_ORIGINSTEST);
	new mxCheckBox (wWeaponOrigin, 460, 5, 112, 22, "Show Crosshair", IDC_CROSSHAIR);
	new mxCheckBox (wWeaponOrigin, 460, 25, 112, 22, "Show Guidelines", IDC_GUIDELINE);
	new mxLabel (wWeaponOrigin, 460, 48, 150, 48, "Changes to the origin must be\nmade by altering the $origin line\nin your model's QC file.");

#ifdef WIN32
	mxWindow *wFullscreen = new mxWindow (this, 0, 0, 0, 0);
	tab->add (wFullscreen, "Fullscreen");

	// Create widgets for the Fullscreen Tab
	mxLabel *lResolution = new mxLabel (wFullscreen, 5, 3, 90, 18, "Resolution");
	//leWidth = new mxLineEdit (wFullscreen, 5, 5, 50, 22, "800");
	//mxLabel *lX = new mxLabel (wFullscreen, 65, 7, 22, 22, "x");
	//leHeight = new mxLineEdit (wFullscreen, 82, 5, 50, 22, "600");
	//cb3dfxOpenGL = new mxCheckBox (wFullscreen, 5, 30, 130, 22, "3Dfx OpenGL");
	mxChoice *cResolution = new mxChoice (wWeaponOrigin, 5, 17, 112, 22, IDC_RESOLUTION);
	cResolution->add ("640 x 480");
	cResolution->add ("800 x 600");
	cResolution->add ("1072 x 768");
	cResolution->add ("1152 x 864");
	cResolution->add ("1280 x 720");
	cResolution->add ("1280 x 768");
	cResolution->add ("1280 x 960");
	cResolution->add ("1280 x 1024");
	cResolution->add ("1360 x 768");
	cResolution->add ("1600 x 768");
	cResolution->add ("1600 x 900");
	cResolution->add ("1600 x 1024");
	cResolution->add ("1600 x 1200");
	cResolution->select (0);
	mxToolTip::add (cResolution, "Select screen resolution");
	mxButton *bView = new mxButton (wFullscreen, 140, 17, 75, 22, "Fullscreen!", IDC_FULLSCREEN);
#endif

	g_ControlPanel = this;
	prevTab = 0;
}



ControlPanel::~ControlPanel ()
{
}



int
ControlPanel::handleEvent (mxEvent *event)
{
	char str[128];

	if (event->event == mxEvent::Size)
	{
		tab->setBounds (0, 0, event->width, event->height);
		return 1;
	}

	switch (event->action)
	{
		case IDC_TAB:
		{
			g_viewerSettings.showTexture = (tab->getSelectedIndex () == 2);

			if (tab->getSelectedIndex () == 4)
			{
				VectorCopy (g_viewerSettings.rot, g_viewerSettings.rotOld2);
				VectorCopy (g_viewerSettings.trans, g_viewerSettings.transOld2);
				g_viewerSettings.yaw = 74.0f;
				testView ();
			}
			else
			{
				g_viewerSettings.yaw = 65.0f;
			}

			if (4 == prevTab)
			{
				VectorCopy (g_viewerSettings.rotOld2, g_viewerSettings.rot);
				VectorCopy (g_viewerSettings.transOld2, g_viewerSettings.trans);
			}
			prevTab = tab->getSelectedIndex ();
		}
		break;

		case IDC_RENDERMODE:
		{
			int index = cRenderMode->getSelectedIndex ();
			if (index >= 0)
			{
				setRenderMode (index);
			}
		}
		break;

		case IDC_TRANSPARENCY:
		{
			int value = slTransparency->getValue ();
			g_viewerSettings.transparency = (float) value / 100.0f;
			sprintf (str, "Opacity: %d%%", value);
			lTransparency->setLabel (str);
		}
		break;

		case IDC_GROUND:
			setShowGround (((mxCheckBox *) event->widget)->isChecked ());
			break;

		case IDC_MIRROR:
			setMirror (((mxCheckBox *) event->widget)->isChecked ());
			break;

		case IDC_BACKGROUND:
			setShowBackground (((mxCheckBox *) event->widget)->isChecked ());
			break;

		case IDC_HITBOXES:
			g_viewerSettings.showHitBoxes = ((mxCheckBox *) event->widget)->isChecked ();
			break;

		case IDC_BONES:
			g_viewerSettings.showBones = ((mxCheckBox *) event->widget)->isChecked ();
			break;

		case IDC_ATTACHMENTS:
			g_viewerSettings.showAttachments = ((mxCheckBox *) event->widget)->isChecked ();
			break;

		case IDC_CROSSHAIR:
			g_viewerSettings.showCrosshair = ((mxCheckBox *) event->widget)->isChecked ();
			break;

		case IDC_GUIDELINE:
			g_viewerSettings.showGuideLines = ((mxCheckBox *) event->widget)->isChecked ();
			break;

		case IDC_EYEPOS:
			g_viewerSettings.showEyePosition = ((mxCheckBox *) event->widget)->isChecked ();
			break;

		case IDC_UVMAP:
			g_viewerSettings.showUVMap = ((mxCheckBox *) event->widget)->isChecked ();
			break;

		case IDC_OVERLAYUVMAP:
			g_viewerSettings.showUVMapOverlay = ((mxCheckBox *) event->widget)->isChecked ();
			break;

		case IDC_ANTIALIASING:
			g_viewerSettings.showSmoothLines = ((mxCheckBox *) event->widget)->isChecked ();
			break;

		case IDC_SEQUENCE:
		{
			int index = cSequence->getSelectedIndex ();
			if (index >= 0)
			{
				setSequence (index);
				setEvent (index);
			}
		}
		break;

		case IDC_WEAPONSEQUENCE:
		{
			int index = cWpSequence->getSelectedIndex ();
			if (index >= 0)
			{
				setSequence (index);
				setEvent (index);
			}
		}
		break;

		case IDC_EVENT:
		{
			int index = cEvent->getSelectedIndex ();
			if (index >= 0)
			{
				setEventInfo (index);
			}
		}
		break;

		case IDC_SPEEDSCALE:
		{
			int v = ((mxSlider *) event->widget)->getValue ();
			g_viewerSettings.speedScale = (float) (v * 5) / 200.0f;
		}
		break;

		case IDC_STOP:
		{
			if (tbStop->isChecked ())
			{
				tbStop->setLabel ("Play");
				tbWpStop->setChecked (true);
				tbWpStop->setLabel ("Play");
				g_bStopPlaying = true;
				g_nCurrFrame = g_studioModel.SetFrame (-1);
				sprintf (str, "%d", g_nCurrFrame);
				leFrame->setLabel (str);
				leWpFrame->setLabel (str);
				bPrevFrame->setEnabled (true);
				leFrame->setEnabled (true);
				leWpFrame->setEnabled (true);
				bNextFrame->setEnabled (true);
			}
			else
			{
				tbStop->setLabel ("Stop");
				tbWpStop->setChecked (false);
				tbWpStop->setLabel ("Stop");
				g_bStopPlaying = false;
				bPrevFrame->setEnabled (false);
				leFrame->setEnabled (false);
				leWpFrame->setEnabled (false);
				bNextFrame->setEnabled (false);
			}
		}
		break;

		case IDC_WEAPONSTOP:
		{
			if (tbWpStop->isChecked ())
			{
				tbStop->setLabel ("Play");
				tbStop->setChecked (true);
				tbWpStop->setLabel ("Play");
				g_bStopPlaying = true;
				g_nCurrFrame = g_studioModel.SetFrame (-1);
				sprintf (str, "%d", g_nCurrFrame);
				leFrame->setLabel (str);
				leWpFrame->setLabel (str);
				bPrevFrame->setEnabled (true);
				leFrame->setEnabled (true);
				leWpFrame->setEnabled (true);
				bNextFrame->setEnabled (true);
			}
			else
			{
				tbStop->setLabel ("Stop");
				tbStop->setChecked (false);
				tbWpStop->setLabel ("Stop");
				g_bStopPlaying = false;
				bPrevFrame->setEnabled (false);
				leFrame->setEnabled (false);
				leWpFrame->setEnabled (false);
				bNextFrame->setEnabled (false);
			}
		}
		break;

		case IDC_PREVFRAME:
		{
			g_nCurrFrame = g_studioModel.SetFrame (g_nCurrFrame - 1);
			sprintf (str, "%d", g_nCurrFrame);
			leFrame->setLabel (str);
			leWpFrame->setLabel (str);
		}
		break;

		case IDC_FRAME:
		case IDC_WEAPONFRAME:
		{
			g_nCurrFrame = atoi (leFrame->getLabel ());
			g_nCurrFrame = g_studioModel.SetFrame (g_nCurrFrame);
		}
		break;

		case IDC_NEXTFRAME:
		{
			g_nCurrFrame = g_studioModel.SetFrame (g_nCurrFrame + 1);
			sprintf (str, "%d", g_nCurrFrame);
			leFrame->setLabel (str);
			leWpFrame->setLabel (str);
		}
		break;

		case IDC_BODYPART:
		{
			int index = cBodypart->getSelectedIndex ();
			if (index >= 0)
			{
				setBodypart (index);
			}
		}
		break;

		case IDC_SUBMODEL:
		{
			int index = cSubmodel->getSelectedIndex ();
			if (index >= 0)
			{
				setSubmodel (index);
			}
		}
		break;

		case IDC_CONTROLLER:
		{
			int index = cController->getSelectedIndex ();
			if (index >= 0)
				setBoneController (index);
		}
		break;

		case IDC_CONTROLLERVALUE:
		{
			int index = cController->getSelectedIndex ();
			if (index >= 0)
				setBoneControllerValue (index, (float) slController->getValue ());
		}
		break;

		case IDC_SKINS:
		{
			int index = cSkin->getSelectedIndex ();
			if (index >= 0)
			{
				g_studioModel.SetSkin (index);
				g_viewerSettings.skin = index;
				d_GlWindow->redraw ();
			}
		}
		break;

		case IDC_MESH:
		{
			int index = cMesh->getSelectedIndex ();
			if (index >= 0)
			{
				g_viewerSettings.showAllMeshes = true;
				if (g_viewerSettings.meshCount <= 1 || g_viewerSettings.meshCount >= index + 1)
					g_viewerSettings.showAllMeshes = false;
				g_viewerSettings.mesh = index;
			}
		}
		break;

		case IDC_TEXTURES:
		{
			int index = cTextures->getSelectedIndex ();
			if (index >= 0)
			{
				g_viewerSettings.texture = index;
				studiohdr_t *hdr = g_studioModel.getTextureHeader ();
				if (hdr)
				{
					mstudiotexture_t *ptexture = (mstudiotexture_t *) ((byte *) hdr + hdr->textureindex) + index;
					sprintf (str, "Texture (size: %d x %d)", ptexture->width, ptexture->height);
					lTexSize->setLabel (str);
					cbChrome->setChecked ((ptexture->flags & STUDIO_NF_CHROME) == STUDIO_NF_CHROME);
					cbAdditive->setChecked ((ptexture->flags & STUDIO_NF_ADDITIVE) == STUDIO_NF_ADDITIVE);
					cbTransparent->setChecked ((ptexture->flags & STUDIO_NF_TRANSPARENT) == STUDIO_NF_TRANSPARENT);
				}
				setMesh (index);
				d_GlWindow->redraw ();
			}
		}
		break;

		case IDC_ORIGINSTEST:
		{
			testView ();
		}
		break;

		case IDC_CHROME:
		{
			studiohdr_t *hdr = g_studioModel.getTextureHeader ();
			if (hdr)
			{
				mstudiotexture_t *ptexture = (mstudiotexture_t *) ((byte *) hdr + hdr->textureindex) + g_viewerSettings.texture;
				if (cbChrome->isChecked ())
				{
					ptexture->flags &= ~STUDIO_NF_TRANSPARENT;
					ptexture->flags |= STUDIO_NF_CHROME;
					cbTransparent->setChecked (0);
				}
				else
					ptexture->flags &= ~STUDIO_NF_CHROME;
			}
		}
		break;

		case IDC_ADDITIVE:
		{
			studiohdr_t *hdr = g_studioModel.getTextureHeader ();
			if (hdr)
			{
				mstudiotexture_t *ptexture = (mstudiotexture_t *) ((byte *) hdr + hdr->textureindex) + g_viewerSettings.texture;
				if (cbAdditive->isChecked ())
				{
					ptexture->flags &= ~STUDIO_NF_TRANSPARENT;
					ptexture->flags |= STUDIO_NF_ADDITIVE;
					cbTransparent->setChecked (0);
				}
				else
					ptexture->flags &= ~STUDIO_NF_ADDITIVE;
			}
		}
		break;

		case IDC_TRANSPARENT:
		{
			studiohdr_t *hdr = g_studioModel.getTextureHeader ();
			if (hdr)
			{
				mstudiotexture_t *ptexture = (mstudiotexture_t *) ((byte *) hdr + hdr->textureindex) + g_viewerSettings.texture;
				if (cbTransparent->isChecked ())
				{
					ptexture->flags &= ~(STUDIO_NF_ADDITIVE|STUDIO_NF_CHROME);
					ptexture->flags |= STUDIO_NF_TRANSPARENT;
					cbChrome->setChecked (0);
					cbAdditive->setChecked (0);
				}
				else
				ptexture->flags &= ~STUDIO_NF_TRANSPARENT;
			}
		}
		break;

		case IDC_EXPORTTEXTURE:
		{
			char *ptr = (char *) mxGetSaveFileName (this, "", "*.bmp");
			if (!ptr)
				break;

			char filename[256];
			char ext[16];

			strcpy (filename, ptr);
			strcpy (ext, mx_getextension (filename));
			if (mx_strcasecmp (ext, ".bmp"))
				strcat (filename, ".bmp");

			studiohdr_t *phdr = g_studioModel.getTextureHeader ();
			if (phdr)
			{
				mxImage image;
				mstudiotexture_t *ptexture = (mstudiotexture_t *) ((byte *) phdr + phdr->textureindex) + g_viewerSettings.texture;
				image.width = ptexture->width;
				image.height = ptexture->height;
				image.bpp = 8;
				image.data = (void *) ((byte *) phdr + ptexture->index);
				image.palette = (void *) ((byte *) phdr + ptexture->width * ptexture->height + ptexture->index);
				if (!mxBmpWrite (filename, &image))
					mxMessageBox (this, "Error writing .BMP texture.", g_appTitle, MX_MB_OK | MX_MB_ERROR);
				image.data = 0;
				image.palette = 0;
			}
		}
		break;

		case IDC_IMPORTTEXTURE:
		{
			char *ptr = (char *) mxGetOpenFileName (this, "", "*.bmp");
			if (!ptr)
				break;

			char filename[256];
			char ext[16];

			strcpy (filename, ptr);
			strcpy (ext, mx_getextension (filename));
			if (mx_strcasecmp (ext, ".bmp"))
				strcat (filename, ".bmp");

			mxImage *image = mxBmpRead (filename);
			if (!image)
			{
				mxMessageBox (this, "Error loading .BMP texture.", g_appTitle, MX_MB_OK | MX_MB_ERROR);
				return 1;
			}

			if (!image->palette)
			{
				delete image;
				mxMessageBox (this, "Error loading .BMP texture.  Must be 8-bit!", g_appTitle, MX_MB_OK | MX_MB_ERROR);
				return 1;
			}

			studiohdr_t *phdr = g_studioModel.getTextureHeader ();
			if (phdr)
			{
				mstudiotexture_t *ptexture = (mstudiotexture_t *) ((byte *) phdr + phdr->textureindex) + g_viewerSettings.texture;
				if (image->width == ptexture->width && image->height == ptexture->height)
				{
					memcpy ((byte *) phdr + ptexture->index, image->data, image->width * image->height);
					memcpy ((byte *) phdr + ptexture->index + image->width * image->height, image->palette, 768);

					g_studioModel.UploadTexture (ptexture, (byte *) phdr + ptexture->index, (byte *) phdr + ptexture->index + image->width * image->height, g_viewerSettings.texture + 3);
				}
				else
					mxMessageBox (this, "Texture must be of same size.", g_appTitle, MX_MB_OK | MX_MB_ERROR);
			}

			delete image;
			d_GlWindow->redraw ();
		}
		break;

		case IDC_TEXTURESCALE:
		{
			float texScale = ((mxSlider *) event->widget)->getValue ();
			g_viewerSettings.textureScale = 1.0f + texScale * 4.0f / 100.0f;
			sprintf (str, "Scale Texture View (%.fx)", g_viewerSettings.textureScale);
			lTextureScale->setLabel (str);
			//d_GlWindow->redraw ();
		}
		break;

#ifdef HAVE_SCALE
		case 10001:
		{
			float scale = (float) atof (leMeshScale->getLabel ());
			if (scale > 0.0f)
			{
				g_studioModel.scaleMeshes (scale);
			}
		}
		break;

		case 10002:
		{
			float scale = (float) atof (leBoneScale->getLabel ());
			if (scale > 0.0f)
			{
				g_studioModel.scaleBones (scale);
			}
		}
		break;
#endif

#ifdef WIN32
		case IDC_FULLSCREEN:
			fullscreen ();
			break;
#endif
	}

	return 1;
}



void
ControlPanel::dumpModelInfo ()
{
	studiohdr_t *hdr = g_studioModel.getStudioHeader ();
	if (hdr)
	{
		remove ("midump.txt");
		FILE *file = fopen ("midump.txt", "wt");
		if (file)
		{
			byte *phdr = (byte *) hdr;
			int i;

			fprintf (file, "id: %c%c%c%c\n", phdr[0], phdr[1], phdr[2], phdr[3]);
			fprintf (file, "version: %d\n", hdr->version);
			fprintf (file, "name: \"%s\"\n", hdr->name);
			fprintf (file, "length: %d\n\n", hdr->length);

			fprintf (file, "eyeposition: %f %f %f\n", hdr->eyeposition[0], hdr->eyeposition[1], hdr->eyeposition[2]);
			fprintf (file, "min: %f %f %f\n", hdr->min[0], hdr->min[1], hdr->min[2]);
			fprintf (file, "max: %f %f %f\n", hdr->max[0], hdr->max[1], hdr->max[2]);
			fprintf (file, "bbmin: %f %f %f\n", hdr->bbmin[0], hdr->bbmin[1], hdr->bbmin[2]);
			fprintf (file, "bbmax: %f %f %f\n", hdr->bbmax[0], hdr->bbmax[1], hdr->bbmax[2]);
			
			fprintf (file, "flags: %d\n\n", hdr->flags);

			fprintf (file, "numbones: %d\n", hdr->numbones);
			for (i = 0; i < hdr->numbones; i++)
			{
				mstudiobone_t *pbones = (mstudiobone_t *) (phdr + hdr->boneindex);
				fprintf (file, "\nbone %d.name: \"%s\"\n", i + 1, pbones[i].name);
				fprintf (file, "bone %d.parent: %d\n", i + 1, pbones[i].parent);
				fprintf (file, "bone %d.flags: %d\n", i + 1, pbones[i].flags);
				fprintf (file, "bone %d.bonecontroller: %d %d %d %d %d %d\n", i + 1, pbones[i].bonecontroller[0], pbones[i].bonecontroller[1], pbones[i].bonecontroller[2], pbones[i].bonecontroller[3], pbones[i].bonecontroller[4], pbones[i].bonecontroller[5]);
				fprintf (file, "bone %d.value: %f %f %f %f %f %f\n", i + 1, pbones[i].value[0], pbones[i].value[1], pbones[i].value[2], pbones[i].value[3], pbones[i].value[4], pbones[i].value[5]);
				fprintf (file, "bone %d.scale: %f %f %f %f %f %f\n", i + 1, pbones[i].scale[0], pbones[i].scale[1], pbones[i].scale[2], pbones[i].scale[3], pbones[i].scale[4], pbones[i].scale[5]);
			}

			fprintf (file, "\nnumbonecontrollers: %d\n", hdr->numbonecontrollers);
			for (i = 0; i < hdr->numbonecontrollers; i++)
			{
				mstudiobonecontroller_t *pbonecontrollers = (mstudiobonecontroller_t *) (phdr + hdr->bonecontrollerindex);
				fprintf (file, "\nbonecontroller %d.bone: %d\n", i + 1, pbonecontrollers[i].bone);
				fprintf (file, "bonecontroller %d.type: %d\n", i + 1, pbonecontrollers[i].type);
				fprintf (file, "bonecontroller %d.start: %f\n", i + 1, pbonecontrollers[i].start);
				fprintf (file, "bonecontroller %d.end: %f\n", i + 1, pbonecontrollers[i].end);
				fprintf (file, "bonecontroller %d.rest: %d\n", i + 1, pbonecontrollers[i].rest);
				fprintf (file, "bonecontroller %d.index: %d\n", i + 1, pbonecontrollers[i].index);
			}

			fprintf (file, "\nnumhitboxes: %d\n", hdr->numhitboxes);
			for (i = 0; i < hdr->numhitboxes; i++)
			{
				mstudiobbox_t *pbboxes = (mstudiobbox_t *) (phdr + hdr->hitboxindex);
				fprintf (file, "\nhitbox %d.bone: %d\n", i + 1, pbboxes[i].bone);
				fprintf (file, "hitbox %d.group: %d\n", i + 1, pbboxes[i].group);
				fprintf (file, "hitbox %d.bbmin: %f %f %f\n", i + 1, pbboxes[i].bbmin[0], pbboxes[i].bbmin[1], pbboxes[i].bbmin[2]);
				fprintf (file, "hitbox %d.bbmax: %f %f %f\n", i + 1, pbboxes[i].bbmax[0], pbboxes[i].bbmax[1], pbboxes[i].bbmax[2]);
			}

			fprintf (file, "\nnumseq: %d\n", hdr->numseq);
			for (i = 0; i < hdr->numseq; i++)
			{
				mstudioseqdesc_t *pseqdescs = (mstudioseqdesc_t *) (phdr + hdr->seqindex);
				fprintf (file, "\nseqdesc %d.label: \"%s\"\n", i + 1, pseqdescs[i].label);
				fprintf (file, "seqdesc %d.fps: %f\n", i + 1, pseqdescs[i].fps);
				fprintf (file, "seqdesc %d.flags: %d\n", i + 1, pseqdescs[i].flags);
				fprintf (file, "<...>\n");
			}

			fprintf (file, "\nnumseqgroups: %d\n", hdr->numseqgroups);
			for (i = 0; i < hdr->numseqgroups; i++)
			{
				mstudioseqgroup_t *pseqgroups = (mstudioseqgroup_t *) (phdr + hdr->seqgroupindex);
				fprintf (file, "\nseqgroup %d.label: \"%s\"\n", i + 1, pseqgroups[i].label);
				fprintf (file, "\nseqgroup %d.namel: \"%s\"\n", i + 1, pseqgroups[i].name);
				fprintf (file, "\nseqgroup %d.data: %d\n", i + 1, pseqgroups[i].data);
			}

			hdr = g_studioModel.getTextureHeader ();
			fprintf (file, "\nnumtextures: %d\n", hdr->numtextures);
			fprintf (file, "textureindex: %d\n", hdr->textureindex);
			fprintf (file, "texturedataindex: %d\n", hdr->texturedataindex);
			for (i = 0; i < hdr->numtextures; i++)
			{
				mstudiotexture_t *ptextures = (mstudiotexture_t *) ((byte *) hdr + hdr->textureindex);
				fprintf (file, "\ntexture %d.name: \"%s\"\n", i + 1, ptextures[i].name);
				fprintf (file, "texture %d.flags: %d\n", i + 1, ptextures[i].flags);
				fprintf (file, "texture %d.width: %d\n", i + 1, ptextures[i].width);
				fprintf (file, "texture %d.height: %d\n", i + 1, ptextures[i].height);
				fprintf (file, "texture %d.index: %d\n", i + 1, ptextures[i].index);
			}

			fprintf (file, "\nnumskinref: %d\n", hdr->numskinref);
			fprintf (file, "numskinfamilies: %d\n", hdr->numskinfamilies);

			hdr = g_studioModel.getStudioHeader ();
			fprintf (file, "\nnumbodyparts: %d\n", hdr->numbodyparts);
			for (i = 0; i < hdr->numbodyparts; i++)
			{
				mstudiobodyparts_t *pbodyparts = (mstudiobodyparts_t *) ((byte *) hdr + hdr->bodypartindex);
				fprintf (file, "\nbodypart %d.name: \"%s\"\n", i + 1, pbodyparts[i].name);
				fprintf (file, "bodypart %d.nummodels: %d\n", i + 1, pbodyparts[i].nummodels);
				fprintf (file, "bodypart %d.base: %d\n", i + 1, pbodyparts[i].base);
				fprintf (file, "bodypart %d.modelindex: %d\n", i + 1, pbodyparts[i].modelindex);
			}

			fprintf (file, "\nnumattachments: %d\n", hdr->numattachments);
			for (i = 0; i < hdr->numattachments; i++)
			{
				mstudioattachment_t *pattachments = (mstudioattachment_t *) ((byte *) hdr + hdr->attachmentindex);
				fprintf (file, "attachment %d.name: \"%s\"\n", i + 1, pattachments[i].name);
			}

			fclose (file);

			mx_shellexec (this, "midump.txt");
		}
	}
}



void
ControlPanel::loadModel (const char *filename)
{
	g_studioModel.FreeModel ();
	if (g_studioModel.LoadModel ((char *) filename))
	{
		if (g_studioModel.PostLoadModel ((char *) filename))
		{
			initSequences ();
			setEvent (0);
			initBodyparts ();
			initBoneControllers ();
			initSkins ();
			initTextures ();
			centerView ();
			strcpy (g_viewerSettings.modelFile, filename);
			setModelInfo ();
			g_viewerSettings.sequence = 0;
			setSequenceInfo ();
			g_viewerSettings.speedScale = 1.0f;
			slSpeedScale->setValue (40);
			int i;
			for (i = 0; i < 32; i++)
				g_viewerSettings.submodels[i] = 0;
			for (i = 0; i < 8; i++)
				g_viewerSettings.controllers[i] = 0;

			mx_setcwd (mx_getpath (filename));
		}
		else
			mxMessageBox (this, "Error post-loading model.", g_appTitle, MX_MB_ERROR | MX_MB_OK);
	}
	else
		mxMessageBox (this, "Error loading model.", g_appTitle, MX_MB_ERROR | MX_MB_OK);
}



void
ControlPanel::setRenderMode (int mode)
{
	g_viewerSettings.renderMode = mode;
	d_GlWindow->redraw ();
}



void
ControlPanel::setShowGround (bool b)
{
	g_viewerSettings.showGround = b;
	cbGround->setChecked (b);
	if (!b)
	{
		cbMirror->setChecked (b);
		g_viewerSettings.mirror = b;
	}
}



void
ControlPanel::setMirror (bool b)
{
	g_viewerSettings.useStencil = (!g_viewerSettings.use3dfx && b);
	g_viewerSettings.mirror = b;
	cbMirror->setChecked (b);
	if (b)
	{
		cbGround->setChecked (b);
		g_viewerSettings.showGround = b;
	}
}



void
ControlPanel::setShowBackground (bool b)
{
	g_viewerSettings.showBackground = b;
	cbBackground->setChecked (b);
}



void
ControlPanel::initSequences ()
{
	studiohdr_t *hdr = g_studioModel.getStudioHeader ();
	if (hdr)
	{
		cSequence->removeAll ();
		cWpSequence->removeAll ();
		for (int i = 0; i < hdr->numseq; i++)
		{
			mstudioseqdesc_t *pseqdescs = (mstudioseqdesc_t *) ((byte *) hdr + hdr->seqindex);
			cSequence->add (pseqdescs[i].label);
			cWpSequence->add (pseqdescs[i].label);
		}

		cSequence->select (0);
		cWpSequence->select (0);
	}
}



void
ControlPanel::setSequence (int index)
{
	cSequence->select (index);
	cWpSequence->select (index);
	g_studioModel.SetSequence(index);
	g_viewerSettings.sequence = index;
	setSequenceInfo ();
}



void
ControlPanel::setSequenceInfo ()
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

	lSequenceInfo1->setLabel (str);
}



void
ControlPanel::setEvent (int index)
{
	char str[64];

	studiohdr_t *hdr = g_studioModel.getStudioHeader ();

	if (!hdr)
		return;

	mstudioseqdesc_t *pseqdescs = (mstudioseqdesc_t *) ((byte *)hdr + hdr->seqindex);  //g_viewerSettings.sequence;

	cEvent->removeAll ();
	if (0 < pseqdescs[index].numevents)
	{
		for (int i = 1; i <= pseqdescs[index].numevents; i++)
		{
			sprintf (str, "Event %d", i);
			cEvent->add (str);
		}
	}
	cEvent->select (0);
	setEventInfo (0);
}



void
ControlPanel::setEventInfo (int index)
{
	char str[1024];

	studiohdr_t *hdr = g_studioModel.getStudioHeader ();

	if (!hdr)
		return;

	mstudioseqdesc_t *pseqdescs = (mstudioseqdesc_t *) ((byte *)hdr + hdr->seqindex);

	if (pseqdescs->numevents)
	{
		mstudioevent_t *pevents = (mstudioevent_t *) ((byte *)hdr + pseqdescs->eventindex) + index;

		sprintf (str,
			"Frame: %d\n"
			"Event: %d\n"
			"Options: %s\n"
			"Type: %d\n",
			pevents->frame,
			pevents->event,
			pevents->options,
			pevents->type
			);
	}
	else
	{
		str[0] = '\0';
	}
	lEventInfo->setLabel (str);
}



void
ControlPanel::setMesh (int index)
{
	char str[64];
	int i, j, k = 0, l;

	studiohdr_t *hdr = g_studioModel.getStudioHeader ();

	if (!hdr)
		return;

	cMesh->removeAll ();
	for (i = 0; i < hdr->numbodyparts; i++)
	{
		mstudiobodyparts_t *pbodyparts = (mstudiobodyparts_t *) ((byte *) hdr + hdr->bodypartindex) + i;
		for (j = 0; j < pbodyparts->nummodels; j++)
		{
			mstudiomodel_t *pmodels = (mstudiomodel_t *) ((byte *) hdr + pbodyparts->modelindex) + j;
			for (l = 0; l < pmodels->nummesh; l++)
			{
				mstudiomesh_t *pmesh = (mstudiomesh_t *) ((byte *) hdr + pmodels->meshindex) + l;
				if (pmesh->skinref == index)
				{
					sprintf (str, "Mesh %d", ++k);
					cMesh->add (str);
				}
			}
		}
	}

	if (1 < k)
		cMesh->add ("All");

	cMesh->select (0);
	g_viewerSettings.mesh = 0;
	g_viewerSettings.meshCount = k;
}



void
ControlPanel::initBodyparts ()
{
	studiohdr_t *hdr = g_studioModel.getStudioHeader ();
	if (hdr)
	{
		int i;
		mstudiobodyparts_t *pbodyparts = (mstudiobodyparts_t *) ((byte *) hdr + hdr->bodypartindex);

		cBodypart->removeAll ();
		if (hdr->numbodyparts > 0)
		{
			for (i = 0; i < hdr->numbodyparts; i++)
				cBodypart->add (pbodyparts[i].name);

			cBodypart->select (0);

			cSubmodel->removeAll ();
			for (i = 0; i < pbodyparts[0].nummodels; i++)
			{
				char str[64];
				sprintf (str, "Submodel %d", i + 1);
				cSubmodel->add (str);
			}
			cSubmodel->select (0);
		}
	}
}



void
ControlPanel::setBodypart (int index)
{
	studiohdr_t *hdr = g_studioModel.getStudioHeader ();
	if (hdr)
	{
		//cBodypart->setEn
		cBodypart->select (index);
		if (index < hdr->numbodyparts)
		{
			mstudiobodyparts_t *pbodyparts = (mstudiobodyparts_t *) ((byte *) hdr + hdr->bodypartindex);
			cSubmodel->removeAll ();
		
			for (int i = 0; i < pbodyparts[index].nummodels; i++)
			{
				char str[64];
				sprintf (str, "Submodel %d", i + 1);
				cSubmodel->add (str);
			}
			cSubmodel->select (0);
			//g_studioModel.SetBodygroup (index, 0);
		}
	}
}



void
ControlPanel::setSubmodel (int index)
{
	g_studioModel.SetBodygroup (cBodypart->getSelectedIndex (), index);
	g_viewerSettings.submodels[cBodypart->getSelectedIndex ()] = index;
}



void
ControlPanel::initBoneControllers ()
{
	studiohdr_t *hdr = g_studioModel.getStudioHeader ();
	if (hdr)
	{
		cController->setEnabled (hdr->numbonecontrollers > 0);
		slController->setEnabled (hdr->numbonecontrollers > 0);
		cController->removeAll ();

		mstudiobonecontroller_t *pbonecontrollers = (mstudiobonecontroller_t *) ((byte *) hdr + hdr->bonecontrollerindex);
		for (int i = 0; i < hdr->numbonecontrollers; i++)
		{
			char str[32];
			if (pbonecontrollers[i].index == 4)
				sprintf (str, "Mouth");
			else
				sprintf (str, "Controller %d", pbonecontrollers[i].index);
			cController->add (str);
		}

		if (hdr->numbonecontrollers > 0)
		{
			cController->select (0);
			slController->setRange ((int) pbonecontrollers[0].start, (int) pbonecontrollers[0].end);
			slController->setValue (0);
		}

	}
}



void
ControlPanel::setBoneController (int index)
{
	studiohdr_t *hdr = g_studioModel.getStudioHeader ();
	if (hdr)
	{
		mstudiobonecontroller_t *pbonecontrollers = (mstudiobonecontroller_t *) ((byte *) hdr + hdr->bonecontrollerindex);
		slController->setRange ((int) pbonecontrollers[index].start, (int) pbonecontrollers[index].end);
		slController->setValue (0);
	}
}



void
ControlPanel::setBoneControllerValue (int index, float value)
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



void
ControlPanel::initSkins ()
{
	studiohdr_t *hdr = g_studioModel.getTextureHeader ();
	if (hdr)
	{
		cSkin->setEnabled (hdr->numskinfamilies > 0);
		cSkin->removeAll ();

		for (int i = 0; i < hdr->numskinfamilies; i++)
		{
			char str[32];
			sprintf (str, "Skin %d", i + 1);
			cSkin->add (str);
		}

		cSkin->select (0);
		g_studioModel.SetSkin (0);
		g_viewerSettings.skin = 0;
	}
}



void
ControlPanel::setModelInfo ()
{
	char str[1024];

	studiohdr_t *studioHdr = g_studioModel.getStudioHeader ();
	studiohdr_t *textureHdr = g_studioModel.getTextureHeader ();

	if (!!studioHdr || !textureHdr)
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

	lModelInfo1->setLabel (str);

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

	lModelInfo2->setLabel (str);
}



void
ControlPanel::setDrawnPolysInfo ()
{
	char str[256];

	sprintf (str, "Drawn Polys: %d", g_polys);
	lDrawnPolys->setLabel (str);
	lDrawnPolys2->setLabel (str);
}



void
ControlPanel::initTextures ()
{
	studiohdr_t *hdr = g_studioModel.getTextureHeader ();
	if (hdr)
	{
		cTextures->removeAll ();
		mstudiotexture_t *ptextures = (mstudiotexture_t *) ((byte *) hdr + hdr->textureindex);
		for (int i = 0; i < hdr->numtextures; i++)
			cTextures->add (ptextures[i].name);
		cTextures->select (0);
		g_viewerSettings.texture = 0;
		if (hdr->numtextures > 0)
		{
			cbChrome->setChecked ((ptextures->flags & STUDIO_NF_CHROME) == STUDIO_NF_CHROME);
			cbAdditive->setChecked ((ptextures->flags & STUDIO_NF_ADDITIVE) == STUDIO_NF_ADDITIVE);
			cbTransparent->setChecked ((ptextures->flags & STUDIO_NF_TRANSPARENT) == STUDIO_NF_TRANSPARENT);
			setMesh(0);
		}
	}
}



void
ControlPanel::centerView ()
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
	d_GlWindow->redraw ();
}



void
ControlPanel::saveView ()
{
	VectorCopy(g_viewerSettings.rot, g_viewerSettings.rotOld);
	VectorCopy(g_viewerSettings.trans, g_viewerSettings.transOld);
}



void
ControlPanel::recallView ()
{
	VectorCopy(g_viewerSettings.rotOld, g_viewerSettings.rot);
	VectorCopy(g_viewerSettings.transOld, g_viewerSettings.trans);
}



void
ControlPanel::testView ()
{
	float dx = atof(leOriginX->getLabel());
	float dy = atof(leOriginY->getLabel());
	float dz = atof(leOriginZ->getLabel());

	g_viewerSettings.trans[0] = -dx - 0.03f;
	g_viewerSettings.trans[1] = dz + 1.0172f;
	g_viewerSettings.trans[2] = dy - 0.02f;
	g_viewerSettings.rot[0] = -90.0f;
	g_viewerSettings.rot[1] = 90.0f;
	g_viewerSettings.rot[2] = 0.0f;
}



#ifdef WIN32
void
ControlPanel::fullscreen ()
{
	//g_viewerSettings.use3dfx = cb3dfxOpenGL->isChecked ();
	swap3dfxgl (g_viewerSettings.use3dfx);

	char szName[256];

	GetModuleFileName (NULL, szName, 256);
	char *ptr = strrchr (szName, '\\');
	*ptr = '\0';
	SetCurrentDirectory (szName);

	g_viewerSettings.width = atoi (leWidth->getLabel ());
	g_viewerSettings.height = atoi (leHeight->getLabel ());
	g_viewerSettings.cds = true;
	//g_viewerSettings.use3dfx = cb3dfxOpenGL->isChecked ();

	if (SaveViewerSettings ("hlmv.cfg"))
	{
		g_viewerSettings.pause = true;
		g_viewerSettings.use3dfx = false;
		WinExec ("hlmv.exe -fullscreen", SW_SHOW);
	}
}
#endif
