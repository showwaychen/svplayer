#ifndef SVIDEOPLAYER_JNI_H_
#define SVIDEOPLAYER_JNI_H_
//#include <webrtc\android\jnihelper\jni_classloader.h>
#include "android/jnihelper/jni_classloader.h"
#include "../src/SVideoPlayer.h"

enum AUDIOPLAYER_TYPE
{
	AudioPlayType_AudioTrack = 1
};
class CSVideoPlayerWrapJni :public PlayEventObserver
{

	CSVPlayer* m_svplayer = nullptr;

	
	JNIEnv* jni_;
	//jclass j_class_;
	jobject m_javaObject;
	//jmethodID m_initAudioTrack;
	//jmethodID m_audioPlay;
	jmethodID m_javaEvenCallback;

	static CSVideoPlayerWrapJni* GetInst(JNIEnv* jni, jobject j_player);
protected:
	 void OnEvent(PlayEvent evt, long data1, void* data2) override;

public:
	static CRegisterNativeM s_registernm;
	static jlong JNICALL newinstance(JNIEnv *env, jobject thiz);
	static void JNICALL nativt_setUrl(JNIEnv *env, jobject thiz, jstring url);
	static jint JNICALL native_start(JNIEnv *env, jobject thiz);
	static jint JNICALL native_pause(JNIEnv *env, jobject thiz);
	static jint JNICALL native_resume(JNIEnv *env, jobject thiz);
	static jint JNICALL native_stop(JNIEnv *env, jobject thiz);
	static jint JNICALL native_seek(JNIEnv *env, jobject thiz, jint seconds);
	static jint JNICALL native_setAudioType(JNIEnv* env, jobject thiz, jint type);
	static jint JNICALL native_setVideoRender(JNIEnv* env, jobject thiz, jlong type);
	static jboolean JNICALL native_IsPlaying(JNIEnv *env, jobject thiz);
	static void JNICALL native_propertySetInt(JNIEnv *env, jobject thiz, jint key, jint value);
	static jint JNICALL native_getState(JNIEnv *env, jobject thiz);
	static jint JNICALL native_MuteAudio(JNIEnv *env, jobject thiz, jboolean bMute);
	static jint JNICALL native_GetCurrentPosition(JNIEnv *env, jobject thiz);
	static jint JNICALL native_GetDuration(JNIEnv *env, jobject thiz);
	static jint JNICALL nativt_CaptureImage(JNIEnv *env, jobject thiz, jstring path);
	static void JNICALL native_Destroy(JNIEnv *env, jobject thiz);



	CSVideoPlayerWrapJni(JNIEnv* env, jobject thiz);

	void propertySetInt(int key, int value);
	void setAudioType(int type);
	void setVideoRender(long videorender);
	~CSVideoPlayerWrapJni();
};
#endif 
