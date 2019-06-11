/***
*
*	Copyright (c) 1998, Valve LLC. All rights reserved.
*	
*	This product contains software technology licensed from Id 
*	Software, Inc. ("Id Technology").  Id Technology (c) 1996 Id Software, Inc. 
*	All Rights Reserved.
*
****/
// updates:
// 1-4-99	fixed file texture load and file read bug

////////////////////////////////////////////////////////////////////////
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <gl.h>
//#include <GL/glu.h>
#include "StudioModel.h"
#include "mod_decryptor.h"
#include <algorithm>
#include "TGAlib.h"
#include "bmpread.h"


#pragma warning( disable : 4244 ) // double to float

//extern float			g_bonetransform[MAXSTUDIOBONES][3][4];


StudioModel g_studioModel;

////////////////////////////////////////////////////////////////////////

static int g_texnum = 3;

//Mugsy - upped the maximum texture size to 512. All changes are the replacement of '256'
//with this define, MAX_TEXTURE_DIMS
#define MAX_TEXTURE_DIMS 512	

void StudioModel::UploadTexture(const mstudiotexture_t *ptexture, const byte *data, const byte *pal, int name)
{
	// unsigned *in, int inwidth, int inheight, unsigned *out,  int outwidth, int outheight;
	int		i, j;
	int		row1[MAX_TEXTURE_DIMS], row2[MAX_TEXTURE_DIMS], col1[MAX_TEXTURE_DIMS], col2[MAX_TEXTURE_DIMS];
	const byte *pix1, *pix2, *pix3, *pix4;
	byte	*tex, *out;

	// convert texture to power of 2
	int outwidth;
	for (outwidth = 1; outwidth < ptexture->width; outwidth <<= 1)
		;

	if (outwidth > MAX_TEXTURE_DIMS)
		outwidth = MAX_TEXTURE_DIMS;

	int outheight;
	for (outheight = 1; outheight < ptexture->height; outheight <<= 1)
		;

	if (outheight > MAX_TEXTURE_DIMS)
		outheight = MAX_TEXTURE_DIMS;

	tex = out = (byte *)malloc( outwidth * outheight * 4);
	if (!out)
	{
		return;
	}
/*
	int k = 0;
	for (i = 0; i < ptexture->height; i++)
	{
		for (j = 0; j < ptexture->width; j++)
		{

			in[k++] = pal[data[i * ptexture->width + j] * 3 + 0];
			in[k++] = pal[data[i * ptexture->width + j] * 3 + 1];
			in[k++] = pal[data[i * ptexture->width + j] * 3 + 2];
			in[k++] = 0xff;;
		}
	}

	gluScaleImage (GL_RGBA, ptexture->width, ptexture->height, GL_UNSIGNED_BYTE, in, outwidth, outheight, GL_UNSIGNED_BYTE, out);
	free (in);
*/

	for (i = 0; i < outwidth; i++)
	{
		col1[i] = (int) ((i + 0.25) * (ptexture->width / (float)outwidth));
		col2[i] = (int) ((i + 0.75) * (ptexture->width / (float)outwidth));
	}

	for (i = 0; i < outheight; i++)
	{
		row1[i] = (int) ((i + 0.25) * (ptexture->height / (float)outheight)) * ptexture->width;
		row2[i] = (int) ((i + 0.75) * (ptexture->height / (float)outheight)) * ptexture->width;
	}

	// scale down and convert to 32bit RGB
	for (i=0 ; i<outheight ; i++)
	{
		for (j=0 ; j<outwidth ; j++, out += 4)
		{
			pix1 = &pal[data[row1[i] + col1[j]] * 3];
			pix2 = &pal[data[row1[i] + col2[j]] * 3];
			pix3 = &pal[data[row2[i] + col1[j]] * 3];
			pix4 = &pal[data[row2[i] + col2[j]] * 3];

			out[0] = (pix1[0] + pix2[0] + pix3[0] + pix4[0])>>2;
			out[1] = (pix1[1] + pix2[1] + pix3[1] + pix4[1])>>2;
			out[2] = (pix1[2] + pix2[2] + pix3[2] + pix4[2])>>2;
			out[3] = 0xFF;
		}
	}

	glBindTexture( GL_TEXTURE_2D, name ); //g_texnum );		
	glTexImage2D( GL_TEXTURE_2D, 0, 3/*??*/, outwidth, outheight, 0, GL_RGBA, GL_UNSIGNED_BYTE, tex );
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	// ptexture->width = outwidth;
	// ptexture->height = outheight;
	//ptexture->index = name; //g_texnum;

	free( tex );
}



void
StudioModel::FreeModel ()
{
	if (m_pstudiohdr)
		free (m_pstudiohdr);

	if (m_ptexturehdr && m_owntexmodel)
		free (m_ptexturehdr);

	m_pstudiohdr = m_ptexturehdr = 0;
	m_owntexmodel = false;

	int i;
	for (i = 0; i < 32; i++)
	{
		if (m_panimhdr[i])
		{
			free (m_panimhdr[i]);
			m_panimhdr[i] = 0;
		}
	}

	// deleting textures
	g_texnum -= 3;
	int textures[MAXSTUDIOSKINS];
	for (i = 0; i < g_texnum; i++)
		textures[i] = i + 3;

	glDeleteTextures (g_texnum, (const GLuint *) textures);

	g_texnum = 3;
}



studiohdr_t *StudioModel::LoadModel( const char *modelname )
{
	FILE *fp;
	long size;
	void *buffer;

	if (!modelname)
		return 0;

	// load the model
	if( (fp = fopen( modelname, "rb" )) == NULL)
		return 0;

	fseek( fp, 0, SEEK_END );
	size = ftell( fp );
	fseek( fp, 0, SEEK_SET );

	buffer = malloc( size );
	if (!buffer)
	{
		fclose (fp);
		return 0;
	}

	fread( buffer, size, 1, fp );
	fclose( fp );


	if (strncmp ((const char *) buffer, "IDST", 4) &&
		strncmp ((const char *) buffer, "IDSQ", 4))
	{
		free (buffer);
		return nullptr;
	}

	if (!strncmp ((const char *) buffer, "IDSQ", 4) && !m_pstudiohdr)
	{
		free (buffer);
		return nullptr;
	}

	return (studiohdr_t *)buffer;
}

void StudioModel::LoadModelTextures( const studiohdr_t *phdr)
{
	const byte *pin = reinterpret_cast<const byte *>(phdr);
	const mstudiotexture_t *ptexture = reinterpret_cast<const mstudiotexture_t *>(pin + phdr->textureindex);

	if (phdr->textureindex > 0 && phdr->numtextures <= MAXSTUDIOSKINS)
	{
		int n = phdr->numtextures;
		for (int i = 0; i < n; i++)
		{
			// strcpy( name, mod->name );
			// strcpy( name, ptexture[i].name );
			UploadTexture( &ptexture[i], pin + ptexture[i].index, pin + ptexture[i].width * ptexture[i].height + ptexture[i].index, g_texnum++ );
		}
	}

	// UNDONE: free texture memory
}

void StudioModel::LoadModelTexturesCSO( studiohdr_t *phdr, const char *texturePath)
{
	byte *pin = reinterpret_cast<byte *>(phdr);
	mstudiotexture_t *ptexture = reinterpret_cast<mstudiotexture_t *>(pin + phdr->textureindex);

	if (phdr->textureindex > 0 && phdr->numtextures <= MAXSTUDIOSKINS)
	{
		int n = phdr->numtextures;
		for (int i = 0; i < n; i++)
		{
			// strcpy( name, mod->name );
			// strcpy( name, ptexture[i].name );
			if(isCSOExternalTexture(ptexture[i]))
			{
				std::string path = texturePath;
				path.push_back('/');
				path += (ptexture[i].name);

				if(UploadTextureTGA(&ptexture[i], path.c_str(), g_texnum) || UploadTextureBMP(&ptexture[i], path.c_str(), g_texnum))
				{
					// ...
				}
				else
				{
					UploadTexture( &ptexture[i], pin + ptexture[i].index, pin + ptexture[i].width * ptexture[i].height + ptexture[i].index, g_texnum );
				}
				++g_texnum;
			}
			else
			{
				UploadTexture( &ptexture[i], pin + ptexture[i].index, pin + ptexture[i].width * ptexture[i].height + ptexture[i].index, g_texnum++ );
			}

		}
	}

	// UNDONE: free texture memory
}

bool StudioModel::PostLoadModel( studiohdr_t *phdr, const char *modelname )
{
	studiohdr_t *ptexturehdr = nullptr;
	bool owntexmodel = false;

	// preload textures
	if (phdr->numtextures == 0)
	{
		char texturename[256];

		strcpy( texturename, modelname );
		strcpy( &texturename[strlen(texturename) - 4], "T.mdl" );

		ptexturehdr = LoadModel( texturename );
		if (!ptexturehdr)
		{
			FreeModel ();
			return false;
		}
		owntexmodel = true;
	}
	else
	{
		ptexturehdr = phdr;
		owntexmodel = false;
	}

	// preload animations
	if (phdr->numseqgroups > 1)
	{
		for (int i = 1; i < phdr->numseqgroups; i++)
		{
			char seqgroupname[256];

			strcpy( seqgroupname, modelname );
			sprintf( &seqgroupname[strlen(seqgroupname) - 4], "%02d.mdl", i );

			m_panimhdr[i] = LoadModel( seqgroupname );
			if (!m_panimhdr[i])
			{
				FreeModel ();
				return false;
			}
		}
	}

	m_pstudiohdr = phdr;
	m_ptexturehdr = ptexturehdr;
	m_owntexmodel = owntexmodel;

	SetSequence (0);
	SetController (0, 0.0f);
	SetController (1, 0.0f);
	SetController (2, 0.0f);
	SetController (3, 0.0f);
	SetMouth (0.0f);

	int n;
	for (n = 0; n < phdr->numbodyparts; n++)
		SetBodygroup (n, 0);

	SetSkin (0);
/*
	vec3_t mins, maxs;
	ExtractBbox (mins, maxs);
	if (mins[2] < 5.0f)
		m_origin[2] = -mins[2];
*/

	return true;
}



bool StudioModel::SaveModel ( const char *modelname )
{
	if (!modelname)
		return false;

	if (!m_pstudiohdr)
		return false;

	FILE *file;
	
	file = fopen (modelname, "wb");
	if (!file)
		return false;

	fwrite (m_pstudiohdr, sizeof (byte), m_pstudiohdr->length, file);
	fclose (file);

	// write texture model
	if (m_owntexmodel && m_ptexturehdr)
	{
		char texturename[256];

		strcpy( texturename, modelname );
		strcpy( &texturename[strlen(texturename) - 4], "T.mdl" );

		file = fopen (texturename, "wb");
		if (file)
		{
			fwrite (m_ptexturehdr, sizeof (byte), m_ptexturehdr->length, file);
			fclose (file);
		}
	}

	// write seq groups
	if (m_pstudiohdr->numseqgroups > 1)
	{
		for (int i = 1; i < m_pstudiohdr->numseqgroups; i++)
		{
			char seqgroupname[256];

			strcpy( seqgroupname, modelname );
			sprintf( &seqgroupname[strlen(seqgroupname) - 4], "%02d.mdl", i );

			file = fopen (seqgroupname, "wb");
			if (file)
			{
				fwrite (m_panimhdr[i], sizeof (byte), m_panimhdr[i]->length, file);
				fclose (file);
			}
		}
	}

	return true;
}



////////////////////////////////////////////////////////////////////////

int StudioModel::GetSequence( )
{
	return m_sequence;
}

int StudioModel::SetSequence( int iSequence )
{
	if (iSequence > m_pstudiohdr->numseq)
		return m_sequence;

	m_sequence = iSequence;
	m_frame = 0;

	return m_sequence;
}


void StudioModel::ExtractBbox( float *mins, float *maxs )
{
	mstudioseqdesc_t	*pseqdesc;

	pseqdesc = (mstudioseqdesc_t *)((byte *)m_pstudiohdr + m_pstudiohdr->seqindex);
	
	mins[0] = pseqdesc[ m_sequence ].bbmin[0];
	mins[1] = pseqdesc[ m_sequence ].bbmin[1];
	mins[2] = pseqdesc[ m_sequence ].bbmin[2];

	maxs[0] = pseqdesc[ m_sequence ].bbmax[0];
	maxs[1] = pseqdesc[ m_sequence ].bbmax[1];
	maxs[2] = pseqdesc[ m_sequence ].bbmax[2];
}



void StudioModel::GetSequenceInfo( float *pflFrameRate, float *pflGroundSpeed )
{
	mstudioseqdesc_t	*pseqdesc;

	pseqdesc = (mstudioseqdesc_t *)((byte *)m_pstudiohdr + m_pstudiohdr->seqindex) + (int)m_sequence;

	if (pseqdesc->numframes > 1)
	{
		*pflFrameRate = 256 * pseqdesc->fps / (pseqdesc->numframes - 1);
		*pflGroundSpeed = sqrt( pseqdesc->linearmovement[0]*pseqdesc->linearmovement[0]+ pseqdesc->linearmovement[1]*pseqdesc->linearmovement[1]+ pseqdesc->linearmovement[2]*pseqdesc->linearmovement[2] );
		*pflGroundSpeed = *pflGroundSpeed * pseqdesc->fps / (pseqdesc->numframes - 1);
	}
	else
	{
		*pflFrameRate = 256.0;
		*pflGroundSpeed = 0.0;
	}
}



float StudioModel::SetController( int iController, float flValue )
{
	if (!m_pstudiohdr)
		return 0.0f;

	mstudiobonecontroller_t	*pbonecontroller = (mstudiobonecontroller_t *)((byte *)m_pstudiohdr + m_pstudiohdr->bonecontrollerindex);

	// find first controller that matches the index
	int i;
	for (i = 0; i < m_pstudiohdr->numbonecontrollers; i++, pbonecontroller++)
	{
		if (pbonecontroller->index == iController)
			break;
	}
	if (i >= m_pstudiohdr->numbonecontrollers)
		return flValue;

	// wrap 0..360 if it's a rotational controller
	if (pbonecontroller->type & (STUDIO_XR | STUDIO_YR | STUDIO_ZR))
	{
		// ugly hack, invert value if end < start
		if (pbonecontroller->end < pbonecontroller->start)
			flValue = -flValue;

		// does the controller not wrap?
		if (pbonecontroller->start + 359.0 >= pbonecontroller->end)
		{
			if (flValue > ((pbonecontroller->start + pbonecontroller->end) / 2.0) + 180)
				flValue = flValue - 360;
			if (flValue < ((pbonecontroller->start + pbonecontroller->end) / 2.0) - 180)
				flValue = flValue + 360;
		}
		else
		{
			if (flValue > 360)
				flValue = flValue - (int)(flValue / 360.0) * 360.0;
			else if (flValue < 0)
				flValue = flValue + (int)((flValue / -360.0) + 1) * 360.0;
		}
	}

	int setting = (int) (255 * (flValue - pbonecontroller->start) /
	(pbonecontroller->end - pbonecontroller->start));

	if (setting < 0) setting = 0;
	if (setting > 255) setting = 255;
	m_controller[iController] = setting;

	return setting * (1.0 / 255.0) * (pbonecontroller->end - pbonecontroller->start) + pbonecontroller->start;
}


float StudioModel::SetMouth( float flValue )
{
	if (!m_pstudiohdr)
		return 0.0f;

	mstudiobonecontroller_t	*pbonecontroller = (mstudiobonecontroller_t *)((byte *)m_pstudiohdr + m_pstudiohdr->bonecontrollerindex);

	// find first controller that matches the mouth
	for (int i = 0; i < m_pstudiohdr->numbonecontrollers; i++, pbonecontroller++)
	{
		if (pbonecontroller->index == 4)
			break;
	}

	// wrap 0..360 if it's a rotational controller
	if (pbonecontroller->type & (STUDIO_XR | STUDIO_YR | STUDIO_ZR))
	{
		// ugly hack, invert value if end < start
		if (pbonecontroller->end < pbonecontroller->start)
			flValue = -flValue;

		// does the controller not wrap?
		if (pbonecontroller->start + 359.0 >= pbonecontroller->end)
		{
			if (flValue > ((pbonecontroller->start + pbonecontroller->end) / 2.0) + 180)
				flValue = flValue - 360;
			if (flValue < ((pbonecontroller->start + pbonecontroller->end) / 2.0) - 180)
				flValue = flValue + 360;
		}
		else
		{
			if (flValue > 360)
				flValue = flValue - (int)(flValue / 360.0) * 360.0;
			else if (flValue < 0)
				flValue = flValue + (int)((flValue / -360.0) + 1) * 360.0;
		}
	}

	int setting = (int) (64 * (flValue - pbonecontroller->start) / (pbonecontroller->end - pbonecontroller->start));

	if (setting < 0) setting = 0;
	if (setting > 64) setting = 64;
	m_mouth = setting;

	return setting * (1.0 / 64.0) * (pbonecontroller->end - pbonecontroller->start) + pbonecontroller->start;
}


float StudioModel::SetBlending( int iBlender, float flValue )
{
	mstudioseqdesc_t	*pseqdesc;

	if (!m_pstudiohdr)
		return 0.0f;

	pseqdesc = (mstudioseqdesc_t *)((byte *)m_pstudiohdr + m_pstudiohdr->seqindex) + (int)m_sequence;

	if (pseqdesc->blendtype[iBlender] == 0)
		return flValue;

	if (pseqdesc->blendtype[iBlender] & (STUDIO_XR | STUDIO_YR | STUDIO_ZR))
	{
		// ugly hack, invert value if end < start
		if (pseqdesc->blendend[iBlender] < pseqdesc->blendstart[iBlender])
			flValue = -flValue;

		// does the controller not wrap?
		if (pseqdesc->blendstart[iBlender] + 359.0 >= pseqdesc->blendend[iBlender])
		{
			if (flValue > ((pseqdesc->blendstart[iBlender] + pseqdesc->blendend[iBlender]) / 2.0) + 180)
				flValue = flValue - 360;
			if (flValue < ((pseqdesc->blendstart[iBlender] + pseqdesc->blendend[iBlender]) / 2.0) - 180)
				flValue = flValue + 360;
		}
	}

	int setting = (int) (255 * (flValue - pseqdesc->blendstart[iBlender]) / (pseqdesc->blendend[iBlender] - pseqdesc->blendstart[iBlender]));

	if (setting < 0) setting = 0;
	if (setting > 255) setting = 255;

	m_blending[iBlender] = setting;

	return setting * (1.0 / 255.0) * (pseqdesc->blendend[iBlender] - pseqdesc->blendstart[iBlender]) + pseqdesc->blendstart[iBlender];
}



int StudioModel::SetBodygroup( int iGroup, int iValue )
{
	if (!m_pstudiohdr)
		return 0;

	if (iGroup > m_pstudiohdr->numbodyparts)
		return -1;

	mstudiobodyparts_t *pbodypart = (mstudiobodyparts_t *)((byte *)m_pstudiohdr + m_pstudiohdr->bodypartindex) + iGroup;

	int iCurrent = (m_bodynum / pbodypart->base) % pbodypart->nummodels;

	if (iValue >= pbodypart->nummodels)
		return iCurrent;

	m_bodynum = (m_bodynum - (iCurrent * pbodypart->base) + (iValue * pbodypart->base));

	return iValue;
}


int StudioModel::SetSkin( int iValue )
{
	if (!m_ptexturehdr)
		return 0;

	if (iValue >= m_ptexturehdr->numskinfamilies)
	{
		return m_skinnum;
	}

	m_skinnum = iValue;

	return iValue;
}



void StudioModel::scaleMeshes (float scale)
{
	if (!m_pstudiohdr)
		return;

	int i, j, k;

	// scale verts
	int tmp = m_bodynum;
	for (i = 0; i < m_pstudiohdr->numbodyparts; i++)
	{
		mstudiobodyparts_t *pbodypart = (mstudiobodyparts_t *)((byte *)m_pstudiohdr + m_pstudiohdr->bodypartindex) + i;
		for (j = 0; j < pbodypart->nummodels; j++)
		{
			SetBodygroup (i, j);
			SetupModel (i);

			vec3_t *pstudioverts = (vec3_t *)((byte *)m_pstudiohdr + m_pmodel->vertindex);

			for (k = 0; k < m_pmodel->numverts; k++)
				VectorScale (pstudioverts[k], scale, pstudioverts[k]);
		}
	}

	m_bodynum = tmp;

	// scale complex hitboxes
	mstudiobbox_t *pbboxes = (mstudiobbox_t *) ((byte *) m_pstudiohdr + m_pstudiohdr->hitboxindex);
	for (i = 0; i < m_pstudiohdr->numhitboxes; i++)
	{
		VectorScale (pbboxes[i].bbmin, scale, pbboxes[i].bbmin);
		VectorScale (pbboxes[i].bbmax, scale, pbboxes[i].bbmax);
	}

	// scale bounding boxes
	mstudioseqdesc_t *pseqdesc = (mstudioseqdesc_t *)((byte *)m_pstudiohdr + m_pstudiohdr->seqindex);
	for (i = 0; i < m_pstudiohdr->numseq; i++)
	{
		VectorScale (pseqdesc[i].bbmin, scale, pseqdesc[i].bbmin);
		VectorScale (pseqdesc[i].bbmax, scale, pseqdesc[i].bbmax);
	}

	// maybe scale exeposition, pivots, attachments
}



void StudioModel::scaleBones (float scale)
{
	if (!m_pstudiohdr)
		return;

	mstudiobone_t *pbones = (mstudiobone_t *) ((byte *) m_pstudiohdr + m_pstudiohdr->boneindex);
	for (int i = 0; i < m_pstudiohdr->numbones; i++)
	{
		for (int j = 0; j < 3; j++)
		{
			pbones[i].value[j] *= scale;
			pbones[i].scale[j] *= scale;
		}
	}	
}

bool StudioModel::hasCSOTexture(const studiohdr_t *phdr)
{
	const byte *pin = reinterpret_cast<const byte *>(phdr);
	const mstudiotexture_t *ptextures = reinterpret_cast<const mstudiotexture_t *>(pin + phdr->textureindex);

	if (phdr->textureindex > 0 && phdr->numtextures <= MAXSTUDIOSKINS)
	{
		int n = phdr->numtextures;
		return std::any_of(ptextures, ptextures + n, isCSOExternalTexture);
	}

	return false;
}
bool StudioModel::isCSOExternalTexture(const mstudiotexture_t &texture)
{
	if(texture.width == 4 && texture.height == 1)
		return true;
	if(texture.name[0] == '#' || texture.name[0] == '$')
		return true;

	return false;
}

bool StudioModel::UploadTextureTGA(mstudiotexture_t *ptexture, const char *path, int name)
{
	tImageTGA *tga = tgaLoad(path);
	if(tga->status != TGA_OK)
		return false;
	std::unique_ptr<tImageTGA, decltype(&tgaDestroy)> ptga(tga, tgaDestroy);

	glBindTexture( GL_TEXTURE_2D, name ); //g_texnum );
	if(tga->pixelDepth == 32)
		glTexImage2D( GL_TEXTURE_2D, 0, GL_RGBA8, tga->width, tga->height, 0, GL_RGBA, GL_UNSIGNED_BYTE, tga->imageData );
	else
		glTexImage2D( GL_TEXTURE_2D, 0, GL_RGB5_A1, tga->width, tga->height, 0, GL_RGBA, GL_UNSIGNED_BYTE, tga->imageData );
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	ptexture->width = tga->width;
	ptexture->height = tga->height;

	return true;
}

bool StudioModel::UploadTextureBMP(mstudiotexture_t *ptexture, const char *path, int name)
{
	bmpread_t bitmap;
	if(!bmpread(path, BMPREAD_TOP_DOWN, &bitmap))
	{
		return false;
	}
	std::unique_ptr<bmpread_t, decltype(&bmpread_free)> pbmp(&bitmap, bmpread_free);

	/* At this point, bitmap.width and .height hold the pixel dimensions of the
	 * file, and bitmap.data holds the raw pixel data in RGB triplets.
	 */

	glBindTexture(GL_TEXTURE_2D, name);

	glTexImage2D(GL_TEXTURE_2D, 0, 3, bitmap.width, bitmap.height, 0, GL_RGB, GL_UNSIGNED_BYTE, bitmap.data);

	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	ptexture->width = bitmap.width;
	ptexture->height = bitmap.height;

	return true;
}
