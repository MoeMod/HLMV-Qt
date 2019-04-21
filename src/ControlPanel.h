//
//                 Half-Life Model Viewer (c) 1999 by Mete Ciragan
//
// file:           ControlPanel.h
// last modified:  Oct 20 programs and associated files contained in this
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
#ifndef INCLUDED_CONTROLPANEL
#define INCLUDED_CONTROLPANEL



#ifndef INCLUDED_MXWINDOW
#include <mx/mxWindow.h>
#endif


#define IDC_TAB					1901
#define IDC_RENDERMODE				2001
#define IDC_TRANSPARENCY			2002
#define IDC_GROUND				2003
#define IDC_MIRROR				2004
#define IDC_BACKGROUND				2005
#define IDC_HITBOXES				2006
#define IDC_BONES				2007
#define IDC_ATTACHMENTS				2008
#define IDC_EYEPOS				2009
#define IDC_OVERLAYWIREWRAME			2010

#define IDC_SEQUENCE				3001
#define IDC_SPEEDSCALE				3002
#define IDC_STOP				3003
#define IDC_PREVFRAME				3004
#define IDC_FRAME				3005
#define IDC_NEXTFRAME				3006
#define IDC_EVENT				3008
#define IDC_WEAPONSEQUENCE			3009
#define IDC_WEAPONSTOP				3010
#define IDC_WEAPONFRAME				3011
#define IDC_PLAYSOUND				3012

#define IDC_BODYPART				4001
#define IDC_SUBMODEL				4002
#define IDC_CONTROLLER				4003
#define IDC_CONTROLLERVALUE			4004
#define IDC_SKINS				4005

#define IDC_TEXTURES				5001
#define IDC_EXPORTTEXTURE			5002
#define IDC_IMPORTTEXTURE			5003
#define IDC_TEXTURESCALE			5004
#define IDC_CHROME				5005
#define IDC_ADDITIVE				5006
#define IDC_TRANSPARENT				5007
#define IDC_UVMAP				5008
#define IDC_MESH				5009
#define IDC_OVERLAYUVMAP			5010
#define IDC_ANTIALIASING			5011
#define IDC_EXPORTUVMAP				5012

#define IDC_FULLSCREEN				6001
#define IDC_ORIGINSTEST				6003
#define IDC_CROSSHAIR				6004
#define IDC_GUIDELINE				6005

#define IDC_FULLSCREEN				7001
#define IDC_RESOLUTION				7002


class mxTab;
class mxChoice;
class mxCheckBox;
class mxSlider;
class mxLineEdit;
class mxLabel;
class mxButton;
class mxToggleButton;
class GlWindow;
class TextureWindow;



class ControlPanel : public mxWindow
{
	mxTab *tab;
	mxChoice *cRenderMode;
	mxSlider *slTransparency;
	mxLabel *lTransparency;
	mxCheckBox *cbGround, *cbMirror, *cbBackground;
	mxChoice *cSequence;
	mxChoice *cWpSequence;
	mxSlider *slSpeedScale;
	mxToggleButton *tbStop;
	mxToggleButton *tbWpStop;
	mxButton *bPrevFrame, *bNextFrame;
	mxLineEdit *leFrame;
	mxLineEdit *leWpFrame;
	mxChoice *cBodypart, *cController, *cSubmodel;
	mxSlider *slController;
	mxChoice *cSkin;
	mxLabel *lModelInfo1, *lModelInfo2;
	mxChoice *cTextures;
	mxCheckBox *cbChrome;
	mxLabel *lTexSize;
	mxLineEdit *leWidth, *leHeight;
	mxCheckBox *cb3dfxOpenGL;
	mxLabel *lDrawnPolys;
	mxLabel *lDrawnPolys2;
	mxCheckBox *cbHitBoxes;
	mxCheckBox *cbBones;
	mxCheckBox *cbAttachments;
	mxCheckBox *cbEyePosition;
	mxCheckBox *cbWireframeOverlay;
	mxCheckBox *cbAdditive;
	mxCheckBox *cbTransparent;
	mxCheckBox *cbUVMap;
	mxCheckBox *cbUVMapOverlay;
	mxCheckBox *cbAA;
	mxChoice *cMesh;
	mxSlider *slTextureScale;
	mxLabel *lTextureScale;
	mxLabel *lSequenceInfo1;
	mxLabel *lSequenceInfo2;
	mxChoice *cEvent;
	mxLabel *lEventInfo;

	mxLineEdit *leMeshScale, *leBoneScale;
	mxLineEdit *leOriginX, *leOriginY, *leOriginZ;

	GlWindow *d_GlWindow;
	TextureWindow *d_textureWindow;

public:
	// CREATORS
	ControlPanel (mxWindow *parent);
	virtual ~ControlPanel ();

	// MANIPULATORS
	int handleEvent (mxEvent *event);

	void dumpModelInfo ();
	void loadModel (const char *filename);

	void setRenderMode (int mode);
	void setShowGround (bool b);
	void setMirror (bool b);
	void setShowBackground (bool b);

	void initSequences ();
	void setSequence (int index);
	void setSequenceInfo ();

	void initBodyparts ();
	void setBodypart (int index);
	void setSubmodel (int index);

	void initBoneControllers ();
	void setBoneController (int index);
	void setBoneControllerValue (int index, float value);

	void initSkins ();

	void setModelInfo ();
	void setDrawnPolysInfo ();

	void setMesh (int index);
	void setEvent (int index);
	void setEventInfo (int index);

	void initTextures ();

	void centerView ();
	void saveView ();
	void recallView ();
	void testView ();

	void fullscreen ();

	void setGlWindow (GlWindow *window) { d_GlWindow = window; }

	// no ACCESSORS
	int prevTab;
};



extern ControlPanel *g_ControlPanel;
extern int g_polys;


#endif // INCLUDED_CONTROLPANEL
