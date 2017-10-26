#include "svideoplayer_jni.h"
#include <jni.h>
#include <stdint.h>
#include"android/jnihelper/jni_onload.h"
#include"android/jnihelper/jni_helpers.h"
#include "GlVideoRender.h"
#include "AudioTrackOut.h"
#include "../src/CommonSetting.h"


JNIEXPORT jint JNI_OnLoad(JavaVM *vm, void *reserved)
{
	JNIEnv *env = NULL;

	SetJavaVM(vm);
	CCommonSetting::SetLogLevel(rtc::LS_SENSITIVE);
	CSVPlayer::FFmpegInit();
	if (0 == on_JNI_OnLoad(GetJavaVM(), JNI_VERSION_1_6))
	{
		//LOGD("on_JNI_OnLoad success");
	}
	else
	{
		return -1;
	}
	return JNI_VERSION_1_6;
}

static JNINativeMethod ls_nm[] = {
		{ "newNativeObject", "()J", reinterpret_cast<void*>(
		&CSVideoPlayerWrapJni::newinstance) },
		{ "nativeSetUrl", "(Ljava/lang/String;)V", reinterpret_cast<void*>(
		&CSVideoPlayerWrapJni::nativt_setUrl) },
		{ "nativeStartPlayer", "()I", reinterpret_cast<void*>(
		&CSVideoPlayerWrapJni::native_start) },
		{ "nativePausePlayer", "()I", reinterpret_cast<void*>(
		&CSVideoPlayerWrapJni::native_pause) },
		{ "nativeResumePlayer", "()I", reinterpret_cast<void*>(
		&CSVideoPlayerWrapJni::native_resume) },
		{ "nativePropertySetInt", "(II)V", reinterpret_cast<void*>(
		&CSVideoPlayerWrapJni::native_propertySetInt) },
		{ "nativeStopPlayer", "()I", reinterpret_cast<void*>(
		&CSVideoPlayerWrapJni::native_stop) },
		{ "nativeSeekPlayer", "(I)I", reinterpret_cast<void*>(
		&CSVideoPlayerWrapJni::native_seek) },
		{ "nativeGetState", "()I", reinterpret_cast<void*>(
		&CSVideoPlayerWrapJni::native_getState) },
		{ "nativeSetAudioPlayer", "(I)V", reinterpret_cast<void*>(
		&CSVideoPlayerWrapJni::native_setAudioType) },
		{ "nativeSetVideoRender", "(J)V", reinterpret_cast<void*>(
		&CSVideoPlayerWrapJni::native_setVideoRender) },
		{ "nativeIsPlaying", "()Z", reinterpret_cast<void*>(
		&CSVideoPlayerWrapJni::native_IsPlaying) },
		{ "nativeMuteAudio", "(Z)I", reinterpret_cast<void*>(
		&CSVideoPlayerWrapJni::native_MuteAudio) },
		{ "nativeCaptureImage", "(Ljava/lang/String;)I", reinterpret_cast<void*>(
		&CSVideoPlayerWrapJni::nativt_CaptureImage) },
		{ "nativeGetCurrentPositon", "()I", reinterpret_cast<void*>(
		&CSVideoPlayerWrapJni::native_GetCurrentPosition) },
		{ "nativeGetDuration", "()I", reinterpret_cast<void*>(
		&CSVideoPlayerWrapJni::native_GetDuration) },
		{ "nativeDestroy", "()V", reinterpret_cast<void*>(
		&CSVideoPlayerWrapJni::native_Destroy) }
};

enum PROERTYKEY
{
	KeySetIslive = 1,
};
CRegisterNativeM CSVideoPlayerWrapJni::s_registernm("cxw/cn/svplayer/SVideoPlayer", ls_nm, ARRAYSIZE(ls_nm));

jlong JNICALL CSVideoPlayerWrapJni::newinstance(JNIEnv *env, jobject thiz)
{
	CSVideoPlayerWrapJni* instance = new CSVideoPlayerWrapJni(env, thiz);
	return jlongFromPointer((void*)instance);
}

void JNICALL CSVideoPlayerWrapJni::nativt_setUrl(JNIEnv *env, jobject thiz, jstring url)
{
	CSVideoPlayerWrapJni *instance = GetInst(env, thiz);
	if (instance != NULL)
	{
		if (url) {
			instance->m_svplayer->setUrl(env->GetStringUTFChars(url, JNI_FALSE));
		}
	}
}

jint JNICALL CSVideoPlayerWrapJni::native_start(JNIEnv *env, jobject thiz)
{
	CSVideoPlayerWrapJni *instance = GetInst(env, thiz);
	if (instance != NULL)
	{
		return instance->m_svplayer->startPlayer();
	}
	return -1;
}

jint JNICALL CSVideoPlayerWrapJni::native_pause(JNIEnv *env, jobject thiz)
{
	CSVideoPlayerWrapJni *instance = GetInst(env, thiz);
	if (instance != NULL)
	{
		return instance->m_svplayer->pausePlayer();
	}
	return -1;
}

jint JNICALL CSVideoPlayerWrapJni::native_resume(JNIEnv *env, jobject thiz)
{
	CSVideoPlayerWrapJni *instance = GetInst(env, thiz);
	if (instance != NULL)
	{
		return instance->m_svplayer->resumePlayer();
	}
	return -1;
}

jint JNICALL CSVideoPlayerWrapJni::native_stop(JNIEnv *env, jobject thiz)
{
	CSVideoPlayerWrapJni *instance = GetInst(env, thiz);
	if (instance != NULL)
	{
		return instance->m_svplayer->stopPlayer();
	}
	return -1;
}

jint JNICALL CSVideoPlayerWrapJni::native_seek(JNIEnv *env, jobject thiz, jint seconds)
{
	CSVideoPlayerWrapJni *instance = GetInst(env, thiz);
	if (instance != NULL)
	{
		return instance->m_svplayer->seekPlayer(seconds);
	}
	return -1;
}

jint JNICALL CSVideoPlayerWrapJni::native_setAudioType(JNIEnv* env, jobject thiz, jint type)
{
	CSVideoPlayerWrapJni *instance = GetInst(env, thiz);
	if (instance != NULL)
	{
		instance->setAudioType(type);
		return 0;
	}
	return -1;
}

jint JNICALL CSVideoPlayerWrapJni::native_setVideoRender(JNIEnv* env, jobject thiz, jlong type)
{
	CSVideoPlayerWrapJni *instance = GetInst(env, thiz);
	if (instance != NULL)
	{
		 instance->setVideoRender(type);
		 return 0;
	}
	return -1;
}

jboolean JNICALL CSVideoPlayerWrapJni::native_IsPlaying(JNIEnv *env, jobject thiz)
{
	CSVideoPlayerWrapJni *instance = GetInst(env, thiz);
	if (instance != NULL)
	{
		return instance->m_svplayer->isPlaying();
	}
	return false;
}

void JNICALL CSVideoPlayerWrapJni::native_propertySetInt(JNIEnv *env, jobject thiz, jint key, jint value)
{
	CSVideoPlayerWrapJni *instance = GetInst(env, thiz);
	if (instance != NULL)
	{
		instance->propertySetInt(key, value);
	}
}

jint JNICALL CSVideoPlayerWrapJni::native_getState(JNIEnv *env, jobject thiz)
{
	CSVideoPlayerWrapJni *instance = GetInst(env, thiz);
	if (instance != NULL)
	{
		return instance->m_svplayer->startPlayer();
	}
	return 0;
}

jint JNICALL CSVideoPlayerWrapJni::native_MuteAudio(JNIEnv *env, jobject thiz, jboolean bMute)
{
	CSVideoPlayerWrapJni *instance = GetInst(env, thiz);
	if (instance != NULL)
	{
		return instance->m_svplayer->muteAudio(bMute);
	}
	return 0;
}

jint JNICALL CSVideoPlayerWrapJni::native_GetCurrentPosition(JNIEnv *env, jobject thiz)
{
	CSVideoPlayerWrapJni *instance = GetInst(env, thiz);
	if (instance != NULL)
	{
		return instance->m_svplayer->GetCurrentPosition();
	}
	return 0;
}

jint JNICALL CSVideoPlayerWrapJni::native_GetDuration(JNIEnv *env, jobject thiz)
{
	CSVideoPlayerWrapJni *instance = GetInst(env, thiz);
	if (instance != NULL)
	{
		return instance->m_svplayer->GetDuration();
	}
	return 0;
}

jint JNICALL CSVideoPlayerWrapJni::nativt_CaptureImage(JNIEnv *env, jobject thiz, jstring path)
{
	CSVideoPlayerWrapJni *instance = GetInst(env, thiz);
	if (instance != NULL)
	{
		return instance->m_svplayer->captureImage(env->GetStringUTFChars(path, JNI_FALSE));
	}
	return -1;
}

void JNICALL CSVideoPlayerWrapJni::native_Destroy(JNIEnv *env, jobject thiz)
{
	CSVideoPlayerWrapJni *instance = GetInst(env, thiz);
	if (instance != NULL)
	{
		delete instance;
		LOGD << "destroy a player instance";
	}
}

CSVideoPlayerWrapJni::CSVideoPlayerWrapJni(JNIEnv* env, jobject thiz) :jni_(env)
{
	//LOGD("new CXploreMediaSourceJni object");
	m_javaObject = jni_->NewGlobalRef(thiz);
	m_svplayer = new CSVPlayer;
	m_svplayer->setEventObserver(this);
	jclass j_class = jni_->GetObjectClass(thiz);
	//m_javaEvenCallback = jni_->GetMethodID(j_class, "evenCallBack", "(IJ)V");
}

void CSVideoPlayerWrapJni::propertySetInt(int key, int value)
{
	if (KeySetIslive == key)
	{
		m_svplayer->setIsLive((value == 0)?false:true);
	}
}

void CSVideoPlayerWrapJni::setAudioType(int type)
{
	if (type == AudioPlayType_AudioTrack)
	{
		m_svplayer->setAudioOut(new CAudioTrackOut());
	}
}

void CSVideoPlayerWrapJni::setVideoRender(long videorender)
{
	VideoRenderBase *vrb = reinterpret_cast<VideoRenderBase*>(videorender);
	if (vrb != nullptr)
	{
		m_svplayer->setVideoRender(vrb);
	}

}

CSVideoPlayerWrapJni::~CSVideoPlayerWrapJni()
{
	m_svplayer->stopPlayer();
	
	AudioOutBase *audioout = m_svplayer->getAudioOut();
	if (audioout != nullptr)
	{
		delete audioout;
	}
	m_svplayer->setAudioOut(nullptr);
	m_svplayer->setEventObserver(nullptr);
	m_svplayer->setVideoRender(nullptr);
	delete m_svplayer;
	m_svplayer = nullptr;
	AttachThreadScoped attachthread(GetJavaVM());
	JNIEnv* jnienv = attachthread.env();
	jnienv->DeleteGlobalRef(m_javaObject);
	MYCHECK_EXCEPTION(jnienv, "DeleteGlobalRef error");
	jni_ = NULL;

}

CSVideoPlayerWrapJni* CSVideoPlayerWrapJni::GetInst(JNIEnv* jni, jobject j_player)
{
	jclass j_class = jni->GetObjectClass(j_player);
	jfieldID nativeobject_id = jni->GetFieldID(j_class, "m_nativeObject", "J");
	MYCHECK_EXCEPTION(jni, "GetInst failed");
	jlong j_p = jni->GetLongField(j_player, nativeobject_id);
	MYCHECK_EXCEPTION(jni, "GetInst failed");

	return reinterpret_cast<CSVideoPlayerWrapJni*>(j_p);
}

void CSVideoPlayerWrapJni::OnEvent(PlayEvent evt, long data1, void* data2)
{
	AttachThreadScoped attachthread(GetJavaVM());
	JNIEnv *jni = attachthread.env();
	jclass j_class = jni->GetObjectClass(m_javaObject);
	jni->CallVoidMethod(m_javaObject, jni->GetMethodID(j_class, "evenCallBack", "(IJ)V"), evt, data1);
}
