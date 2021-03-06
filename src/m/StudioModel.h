/***
*
*	Copyright (c) 1998, Valve LLC. All rights reserved.
*	
*	This product contains software technology licensed from Id 
*	Software, Inc. ("Id Technology").  Id Technology (c) 1996 Id Software, Inc. 
*	All Rights Reserved.
*
****/

#ifndef INCLUDED_STUDIOMODEL
#define INCLUDED_STUDIOMODEL



#ifndef byte
typedef unsigned char byte;
#endif // byte



#include "mathlib.h"
#include "studio.h"



class StudioModel
{
public:
	studiohdr_t				*getStudioHeader () const { return m_pstudiohdr; }
	studiohdr_t				*getTextureHeader () const { return m_ptexturehdr; }
	studiohdr_t				*getAnimHeader (int i) const { return m_panimhdr[i]; }

	void					UploadTexture( const mstudiotexture_t *ptexture, const byte *data, const byte *pal, int name );
	bool					UploadTextureTGA( mstudiotexture_t *ptexture, const char *path, int name );
	bool					UploadTextureBMP( mstudiotexture_t *ptexture, const char *path, int name );
	void					FreeModel ();

	studiohdr_t				*LoadModel( const char *modelname );
	void                    LoadModelTextures( const studiohdr_t *phdr );
	void                    LoadModelTexturesCSO( studiohdr_t *phdr, const char *texturePath );
	bool					PostLoadModel ( studiohdr_t *phdr, const char *modelname );

	bool					SaveModel ( const char *modelname );
	void					DrawModel( void );
	void					AdvanceFrame( float dt );
	int						SetFrame (int nFrame);

	void					ExtractBbox( float *mins, float *maxs );

	int						SetSequence( int iSequence );
	int						GetSequence( void );
	void					GetSequenceInfo( float *pflFrameRate, float *pflGroundSpeed );

	float					SetController( int iController, float flValue );
	float					SetMouth( float flValue );
	float					SetBlending( int iBlender, float flValue );
	int						SetBodygroup( int iGroup, int iValue );
	int						SetSkin( int iValue );

	void					scaleMeshes (float scale);
	void					scaleBones (float scale);

	static bool             hasCSOTexture(const studiohdr_t *phdr);

private:
	// entity settings
	vec3_t					m_origin;
	vec3_t					m_angles;	
	int						m_sequence;			// sequence index
	float					m_frame;			// frame
	int						m_bodynum;			// bodypart selection	
	int						m_skinnum;			// skin group selection
	byte					m_controller[4];	// bone controllers
	byte					m_blending[2];		// animation blending
	byte					m_mouth;			// mouth position
	bool					m_owntexmodel;		// do we have a modelT.mdl ?

	// internal data
	studiohdr_t				*m_pstudiohdr;
	mstudiomodel_t			*m_pmodel;

	studiohdr_t				*m_ptexturehdr;
	studiohdr_t				*m_panimhdr[32];

	vec4_t					m_adj;				// FIX: non persistant, make static

	void					CalcBoneAdj( void );
	void					CalcBoneQuaternion( int frame, float s, mstudiobone_t *pbone, mstudioanim_t *panim, float *q );
	void					CalcBonePosition( int frame, float s, mstudiobone_t *pbone, mstudioanim_t *panim, float *pos );
	void					CalcRotations ( vec3_t *pos, vec4_t *q, mstudioseqdesc_t *pseqdesc, mstudioanim_t *panim, float f );
	mstudioanim_t			*GetAnim( mstudioseqdesc_t *pseqdesc );
	void					SlerpBones( vec4_t q1[], vec3_t pos1[], vec4_t q2[], vec3_t pos2[], float s );
	void					SetUpBones ( void );

	void					DrawPoints( void );

	void					Lighting (float *lv, int bone, int flags, vec3_t normal);
	void					Chrome (int *chrome, int bone, vec3_t normal);

	void					SetupLighting( void );

	void					SetupModel ( int bodypart );

	static bool             isCSOExternalTexture(const mstudiotexture_t &ptexture);
};



extern vec3_t g_vright;		// needs to be set to viewer's right in order for chrome to work
extern float g_lambert;		// modifier for pseudo-hemispherical lighting
extern StudioModel g_studioModel;



#endif // INCLUDED_STUDIOMODEL