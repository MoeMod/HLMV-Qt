//
// Created by MoeMod on 2019-06-07.
//

#ifndef HLMV_GL_DRAW_H
#define HLMV_GL_DRAW_H

#include <chrono>


class CHLMV_GL_Draw
{
public:
	CHLMV_GL_Draw() : m_bStopPlaying(false) {}

public:
	void Think();
	void Draw();

public:
	void SetSize(int w, int h, int w2, int h2)
	{
		m_w = w;
		m_h = h;
		m_w2 = w2;
		m_h2 = h2;
	}
	void SetStopPlaying(bool stop)
	{
		m_bStopPlaying = stop;
	}

public:
	int w() const { return m_w; }
	int h() const { return m_h; }
	int w2() const { return m_w2; }
	int h2() const { return m_h2; }

private:
	int m_w;
	int m_h;
	int m_w2;
	int m_h2;
	std::chrono::time_point<std::chrono::steady_clock, std::chrono::duration<float>> m_PrevTime;
	int d_textureNames[2];
	bool m_bStopPlaying;
};


#endif //HLMV_GL_DRAW_H
