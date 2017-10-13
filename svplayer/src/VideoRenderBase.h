#ifndef VIDEO_RENDER_BASE_H_
#define VIDEO_RENDER_BASE_H_
class VideoRenderBase
{
	
protected:
	int m_rotate = 0;
	int m_SrcWidth = 0;
	int m_SrcHeight = 0;

	int m_ShowWidth = 0;
	int m_ShowHeight = 0;
public:

	virtual void setRotate(int rotate)
	{
		m_rotate = rotate;
	}
	int getRotate()
	{
		return m_rotate;
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