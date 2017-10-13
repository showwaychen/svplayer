#ifndef AUDIO_TRACK_OUT_H_
#define AUDIO_TRACK_OUT_H_
#include "../src/AudioOutBase.h"
#include "jni.h"

class CAudioTrackOut : public AudioOutBase
{
	JNIEnv* jni_;
	jclass j_class_;
	jobject m_javaObject;
	jmethodID m_initAudioTrack;
	jmethodID m_audioPlay;

	const char* m_classname;
	static jobject NewObject(JNIEnv* jni, jclass pjclass, const char* name, const char* signature, ...);
public:
	CAudioTrackOut();
	~CAudioTrackOut();

	virtual int initAudioOut(int nchannels, int nsampleRate) override;

	virtual int audioPlay(uint8_t* pdata, int nsize) override;

};
#endif 

