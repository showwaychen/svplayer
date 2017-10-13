#ifndef AUDIO_OUT_BASE_H_
#define AUDIO_OUT_BASE_H_
#include <stdint.h>

class AudioOutBase
{
protected:
	bool m_isInit = false;
public:
	bool isInit()
	{
		return m_isInit;
	}
	virtual int initAudioOut(int nchannels, int nsampleRate) = 0;
	virtual int audioPlay(uint8_t* pdata, int nsize) = 0;
	virtual ~AudioOutBase()
	{
	}
};


#endif 