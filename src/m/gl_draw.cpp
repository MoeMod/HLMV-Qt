//
// Created by MoeMod on 2019-06-07.
//

#include "gl_draw.h"

#include <gl.h>

#include "StudioModel.h"
#include "ViewerSettings.h"

void setupRenderMode ();

// Replaces gluPerspective. Sets the frustum to perspective mode.
// fovY     - Field of vision in degrees in the y direction
// aspect   - Aspect ratio of the viewport
// near    - The near clipping distance
// far     - The far clipping distance
static void perspectiveGL (GLfloat fovY, GLfloat aspect, GLfloat _near, GLfloat _far)
{
	GLfloat w, h;

	h = tan (fovY / 360 * Q_PI) * _near;

	w = h * aspect;

	glFrustum (-w, w, -h, h, _near, _far);
}

static void drawFloor ()
{
	if (g_viewerSettings.use3dfx)
	{
		glBegin (GL_TRIANGLE_STRIP);
		glTexCoord2f (1.0f, 0.0f);
		glVertex3f (100.0f, 100.0f, 0.0f);

		glTexCoord2f (1.0f, 1.0f);
		glVertex3f (100.0f, -100.0f, 0.0f);

		glTexCoord2f (0.0f, 0.0f);
		glVertex3f (-100.0f, 100.0f, 0.0f);

		glTexCoord2f (0.0f, 1.0f);
		glVertex3f (-100.0f, -100.0f, 0.0f);

		glEnd ();
	}
	else
	{
		glBegin (GL_TRIANGLE_STRIP);
		glTexCoord2f (0.0f, 0.0f);
		glVertex3f (-100.0f, 100.0f, 0.0f);

		glTexCoord2f (0.0f, 1.0f);
		glVertex3f (-100.0f, -100.0f, 0.0f);

		glTexCoord2f (1.0f, 0.0f);
		glVertex3f (100.0f, 100.0f, 0.0f);

		glTexCoord2f (1.0f, 1.0f);
		glVertex3f (100.0f, -100.0f, 0.0f);

		glEnd ();
	}
}
void CHLMV_GL_Draw::Think()
{
	auto CurrTime = std::chrono::steady_clock::now();

	g_studioModel.SetBlending (0, 0.0);
	g_studioModel.SetBlending (1, 0.0);

	if (!m_bStopPlaying)
		g_studioModel.AdvanceFrame ((CurrTime - m_PrevTime).count() * g_viewerSettings.speedScale);

	m_PrevTime = CurrTime;
}

void CHLMV_GL_Draw::Draw()
{
	glClearColor (g_viewerSettings.bgColor[0], g_viewerSettings.bgColor[1], g_viewerSettings.bgColor[2], 0.0f);

	if (g_viewerSettings.useStencil)
		glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
	else
		glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glViewport (0, 0, w2 (), h2 ());

	glEnable(GL_BLEND);

	//
	// show textures
	//

	if (g_viewerSettings.showTexture)
	{
		glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glMatrixMode (GL_PROJECTION);
		glLoadIdentity ();

		glOrtho (0.0f, (float) w2 (), (float) h2 (), 0.0f, 1.0f, -1.0f);

		studiohdr_t *hdr = g_studioModel.getTextureHeader ();
		if (hdr)
		{
			mstudiotexture_t *ptextures = (mstudiotexture_t *) ((byte *) hdr + hdr->textureindex);
			float w = (float) ptextures[g_viewerSettings.texture].width * g_viewerSettings.textureScale;
			float h = (float) ptextures[g_viewerSettings.texture].height * g_viewerSettings.textureScale;

			glMatrixMode (GL_MODELVIEW);
			glPushMatrix ();
			glLoadIdentity ();

			glDisable (GL_CULL_FACE);
			glDisable (GL_DEPTH_TEST);

			glPolygonMode (GL_FRONT_AND_BACK, GL_FILL);
			float x = ((float) w2 () - w) / 2;
			float y = ((float) h2 () - h) / 2;

			glDisable (GL_TEXTURE_2D);
			if (g_viewerSettings.showUVMap)
			{
				if (g_viewerSettings.showUVMapOverlay)
				{
					glColor4f (0.0f, 0.0f, 1.0f, 1.0f);
					glRectf (x, y, x + w, y + h);
					glEnable (GL_TEXTURE_2D);
					glColor4f (1.0f, 1.0f, 1.0f, 1.0f);
					glBindTexture (GL_TEXTURE_2D, g_viewerSettings.texture + 3); //d_textureNames[0]);

					glTexParameterf (GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
					glTexParameterf (GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
					glBegin (GL_TRIANGLE_STRIP);

					glTexCoord2f (0, 0);
					glVertex2f (x, y);

					glTexCoord2f (1, 0);
					glVertex2f (x + w, y);

					glTexCoord2f (0, 1);
					glVertex2f (x, y + h);

					glTexCoord2f (1, 1);
					glVertex2f (x + w, y + h);

					glEnd ();

				}
				else
				{
					glColor4f (0.0f, 0.0f, 0.0f, 1.0f);
					glRectf (x, y, x + w, y + h);
				}

				hdr = g_studioModel.getStudioHeader ();

				if (hdr)
				{
					float s, t;
					int i, j, k, l;
					int index = -1;
					short *pskinref = (short *)((byte *)hdr + hdr->skinindex);

					for (k = 0; k < hdr->numbodyparts; k++)
					{
						mstudiobodyparts_t *pbodyparts = (mstudiobodyparts_t *) ((byte *) hdr + hdr->bodypartindex) + k;

						for (j = 0; j < pbodyparts->nummodels; j++)
						{
							mstudiomodel_t *pmodels = (mstudiomodel_t *) ((byte *) hdr + pbodyparts->modelindex) + j;

							for (l = 0; l < pmodels->nummesh; l++)
							{
								float s, t;
								short *ptricmds;
								int tri_type;
								mstudiomesh_t *pmesh = (mstudiomesh_t *) ((byte *) hdr + pmodels->meshindex) + l;

								++index;
								if (pmesh->skinref != g_viewerSettings.texture
								    || (g_viewerSettings.mesh != index && !g_viewerSettings.showAllMeshes))
									continue;

								ptricmds = (short *)((byte *)hdr + pmesh->triindex);

								s = 1.0/(float)ptextures[pskinref[pmesh->skinref]].width;
								t = 1.0/(float)ptextures[pskinref[pmesh->skinref]].height;

								glColor4f (1.0f, 1.0f, 1.0f, 1.0f);
								glPolygonMode (GL_FRONT_AND_BACK, GL_LINE);
								glDisable (GL_TEXTURE_2D);
								glDisable (GL_CULL_FACE);
								glDisable (GL_DEPTH_TEST);
								if (g_viewerSettings.showSmoothLines)
									glEnable (GL_LINE_SMOOTH);
								else
									glDisable (GL_LINE_SMOOTH);
								glLineWidth (1.0);

								while ((i = *(ptricmds++)))
								{
									if (i < 0)
									{
										tri_type = GL_TRIANGLE_FAN;
										i = -i;
									}
									else
									{
										tri_type = GL_TRIANGLE_STRIP;
									}

									glBegin (tri_type);
									for ( ; i > 0; i--, ptricmds += 4)
									{
										glVertex3f (ptricmds[2] * s * w + x , ptricmds[3] * t * h + y , 1.0f);
									}
									glEnd ( );
									glDisable (GL_LINE_SMOOTH);
								}
							}
						}
					}
				}
			}
			else
			{
				glColor4f (0.0f, 0.0f, 0.0f, 0.0f);
				glRectf (x, y, x + w, y + h);

				glEnable (GL_TEXTURE_2D);
				glColor4f (1.0f, 1.0f, 1.0f, 1.0f);
				glBindTexture (GL_TEXTURE_2D, g_viewerSettings.texture + 3); //d_textureNames[0]);

				glTexParameterf (GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
				glTexParameterf (GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

				glBegin (GL_TRIANGLE_STRIP);

				glTexCoord2f (0, 0);
				glVertex2f (x, y);

				glTexCoord2f (1, 0);
				glVertex2f (x + w, y);

				glTexCoord2f (0, 1);
				glVertex2f (x, y + h);

				glTexCoord2f (1, 1);
				glVertex2f (x + w, y + h);

				glEnd ();
			}
			glPopMatrix ();

			glClear (GL_DEPTH_BUFFER_BIT);
			glBindTexture (GL_TEXTURE_2D, 0);
		}
		return;
	}

	//
	// draw background
	//

	if (g_viewerSettings.showBackground && d_textureNames[0] && !g_viewerSettings.showTexture)
	{
		glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glMatrixMode (GL_PROJECTION);
		glLoadIdentity ();

		glOrtho (0.0f, 1.0f, 1.0f, 0.0f, 1.0f, -1.0f);

		glMatrixMode (GL_MODELVIEW);
		glPushMatrix ();
		glLoadIdentity ();

		glDisable (GL_CULL_FACE);
		glEnable (GL_TEXTURE_2D);

		glColor4f (1.0f, 1.0f, 1.0f, 1.0f);
		glPolygonMode (GL_FRONT_AND_BACK, GL_FILL);

		glBindTexture (GL_TEXTURE_2D, d_textureNames[0]);

		glBegin (GL_TRIANGLE_STRIP);

		glTexCoord2f (0, 0);
		glVertex2f (0, 0);

		glTexCoord2f (1, 0);
		glVertex2f (1, 0);

		glTexCoord2f (0, 1);
		glVertex2f (0, 1);

		glTexCoord2f (1, 1);
		glVertex2f (1, 1);

		glEnd ();

		glPopMatrix ();

		glClear (GL_DEPTH_BUFFER_BIT);
		glBindTexture (GL_TEXTURE_2D, 0);
	}

	glMatrixMode (GL_PROJECTION);
	glLoadIdentity ();
	perspectiveGL (g_viewerSettings.yaw, (GLfloat) w () / (GLfloat) h (), 1.0f, 4096.0f);

	glMatrixMode (GL_MODELVIEW);
	glPushMatrix ();
	glLoadIdentity ();

	float trans[3] = {g_viewerSettings.trans[0], g_viewerSettings.trans[1], g_viewerSettings.trans[2]};
	float rot[3] = {g_viewerSettings.rot[0], g_viewerSettings.rot[1], g_viewerSettings.rot[2]};

	if(g_viewerSettings.vieworiginmode)
	{
		float dx = g_viewerSettings.vieworigintrans[0];
		float dy = g_viewerSettings.vieworigintrans[1];
		float dz = g_viewerSettings.vieworigintrans[2];

		trans[0] = -dx - 0.03f;
		trans[1] = dz + 1.0172f;
		trans[2] = dy - 0.02f;
		rot[0] = -90.0f;
		rot[1] = 90.0f;
		rot[2] = 0.0f;
	}

	glTranslatef(-trans[0], -trans[1], -trans[2]);
	glRotatef (rot[0], 1.0f, 0.0f, 0.0f);
	glRotatef (rot[1], 0.0f, 0.0f, 1.0f);

	// setup stencil buffer
	if (g_viewerSettings.useStencil)
	{
		/* Don't update color or depth. */
		glDisable(GL_DEPTH_TEST);
		glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);

		/* Draw 1 into the stencil buffer. */
		glEnable(GL_STENCIL_TEST);
		glStencilOp(GL_REPLACE, GL_REPLACE, GL_REPLACE);
		glStencilFunc(GL_ALWAYS, 1, 0xffffffff);

		/* Now render floor; floor pixels just get their stencil set to 1. */
		drawFloor();

		/* Re-enable update of color and depth. */
		glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
		glEnable(GL_DEPTH_TEST);

		/* Now, only render where stencil is set to 1. */
		glStencilFunc(GL_EQUAL, 1, 0xffffffff);  /* draw if ==1 */
		glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);
	}

	g_vright[0] = g_vright[1] = trans[2];

	if (g_viewerSettings.mirror)
	{
		glPushMatrix ();
		glScalef (1, 1, -1);
		glCullFace (GL_BACK);
		setupRenderMode ();
		g_studioModel.DrawModel ();
		glPopMatrix ();
	}

	if (g_viewerSettings.useStencil)
		glDisable (GL_STENCIL_TEST);

	setupRenderMode ();

	glCullFace (GL_FRONT);
	g_studioModel.DrawModel ();

	//
	// draw ground
	//

	if (g_viewerSettings.showGround)
	{
		glPolygonMode (GL_FRONT_AND_BACK, GL_FILL);
		glEnable (GL_DEPTH_TEST);
		glEnable (GL_CULL_FACE);

		if (g_viewerSettings.useStencil)
			glFrontFace (GL_CW);
		else
			glDisable (GL_CULL_FACE);

		glEnable (GL_BLEND);
		if (!d_textureNames[1])
		{
			glDisable (GL_TEXTURE_2D);
			glColor4f (g_viewerSettings.gColor[0], g_viewerSettings.gColor[1], g_viewerSettings.gColor[2], 0.7f);
			glBindTexture (GL_TEXTURE_2D, 0);
		}
		else
		{
			glEnable (GL_TEXTURE_2D);
			glColor4f (1.0f, 1.0f, 1.0f, 0.6f);
			glBindTexture (GL_TEXTURE_2D, d_textureNames[1]);
		}

		glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		drawFloor ();

		glDisable (GL_BLEND);

		if (g_viewerSettings.useStencil)
		{
			glCullFace (GL_BACK);
			glColor4f (0.1f, 0.1f, 0.1f, 1.0f);
			glBindTexture (GL_TEXTURE_2D, 0);
			drawFloor ();

			glFrontFace (GL_CCW);
		}
		else
			glEnable (GL_CULL_FACE);
	}

	if (g_viewerSettings.showCrosshair)
	{
		glMatrixMode (GL_PROJECTION);
		glPushMatrix ();
		glLoadIdentity ();
		glOrtho (0.0f, w2 (), h2 (), 0.0, 0.0, 1.0);
		glMatrixMode (GL_MODELVIEW);
		glLoadIdentity ();
		glDisable (GL_DEPTH_TEST);
		glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glEnable (GL_BLEND);
		glPolygonMode (GL_FRONT_AND_BACK, GL_FILL);
		float w = (w2 () - 27.0f) * 0.5f;
		float h = (h2 () - 27.0f) * 0.5f;
		glDisable (GL_TEXTURE_2D);
		glColor4f (g_viewerSettings.guColor[0], g_viewerSettings.guColor[1], g_viewerSettings.guColor[2], 1.0f);
		float w2 = w + 10.0f;
		float w3 = w + 12.0f;
		float w4 = w + 13.0f;
		float w5 = w + 14.0f;
		float w6 = w + 15.0f;
		float w7 = w + 17.0f;
		float w8 = w + 27.0f;

		float h2 = h + 10.0f;
		float h3 = h + 12.0f;
		float h4 = h + 13.0f;
		float h5 = h + 14.0f;
		float h6 = h + 15.0f;
		float h7 = h + 17.0f;
		float h8 = h + 27.0f;
		glRectf (w4, h, w5, h2);
		glRectf (w4, h7, w5, h8);
		glRectf (w, h4, w2, h5);
		glRectf (w7, h4, w8, h5);
		glRectf (w4, h4, w5, h5);
		glColor4f (g_viewerSettings.guColor[0], g_viewerSettings.guColor[1], g_viewerSettings.guColor[2], 0.25f);
		glRectf (w3, h, w4, h2);
		glRectf (w5, h, w6, h2);
		glRectf (w3, h7, w4, h8);
		glRectf (w5, h7, w6, h8);
		glRectf (w, h3, w2, h4);
		glRectf (w, h5, w2, h6);
		glRectf (w7, h3, w8, h4);
		glRectf (w7, h5, w8, h6);
		glRectf (w4, h3, w5, h4);
		glRectf (w4, h5, w5, h6);
		glRectf (w3, h4, w4, h5);
		glRectf (w5, h4, w6, h5);
		glColor4f (g_viewerSettings.guColor[0], g_viewerSettings.guColor[1], g_viewerSettings.guColor[2], 0.125f);
		glRectf (w3, h3, w4, h4);
		glRectf (w5, h3, w6, h4);
		glRectf (w3, h5, w4, h6);
		glRectf (w5, h5, w6, h6);
	}

	if (g_viewerSettings.showGuideLines)
	{
		glMatrixMode (GL_PROJECTION);
		glPushMatrix ();
		glLoadIdentity ();

		glOrtho (0.0f, w2 (), h2 (), 0.0f, 0.0f, 1.0f);

		glMatrixMode (GL_MODELVIEW);
		glLoadIdentity ();

		glDisable (GL_DEPTH_TEST);
		glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glEnable (GL_BLEND);

		glPolygonMode (GL_FRONT_AND_BACK, GL_FILL);
		glDisable (GL_TEXTURE_2D);

		glColor4f (g_viewerSettings.guColor[0], g_viewerSettings.guColor[1], g_viewerSettings.guColor[2], 0.5f);

		glEnable (GL_LINE_SMOOTH);
		glEnable (GL_LINE_STIPPLE);

		glLineWidth (1.0f);
		glLineStipple (2, 0xFAFA);

		float w = w2 () - 0.5f;
		glBegin (GL_LINES);
		glVertex2f (w, h2 () + 14.0f);
		glVertex2f (w, h2 ());
		glEnd ();

		glDisable (GL_LINE_STIPPLE);
		glLineWidth (3.0f);

		glBegin (GL_LINES);
		w = w2 () * 0.085f;
		glVertex2f (w, 0.0f);
		glVertex2f (w, h2 ());
		w = w2 () * 0.915f;
		glVertex2f (w, 0.0f);
		glVertex2f (w, h2 ());
		glEnd ();

		glLineWidth (1.0f);
		glDisable (GL_LINE_SMOOTH);
	}

	glPopMatrix ();

}
