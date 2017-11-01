#ifndef VIDEO_RENDER_BASE_H_
#define VIDEO_RENDER_BASE_H_
#include "Common.h"
class VideoRenderBase
{
	
protected:
	int m_rotate = 0;
	ShowMode m_eShowMode;
	int m_SrcWidth = 0;
	int m_SrcHeight = 0;

	int m_ShowWidth = 0;
	int m_ShowHeight = 0;
public:
	VideoRenderBase()
	{
		m_eShowMode = kShowModeAspectAuto;
		m_SrcWidth = 0;
		m_SrcHeight = 0;
		m_rotate = 0;
		m_ShowWidth = 0;
		m_ShowHeight = 0;
	}
	virtual void setRotate(int rotate)
	{
		m_rotate = rotate;
	}
	int getRotate()
	{
		return m_rotate;
	}
	void SetShowMode(ShowMode smode)
	{
		m_eShowMode = smode;
	}
	virtual void setSrcImageSize(int width, int height)
	{
		m_SrcWidth = width;
		m_SrcHeight = height;
	}
	void getShowSize(int& width, int& height)
	{
		width = m_ShowWidth;
		height = m_ShowHeight;
	}
	//only for i420.
	virtual int renderImage(uint8_t *pimgdata, int nsize) = 0;
	virtual ~VideoRenderBase()
	{
	}
};
#endif 