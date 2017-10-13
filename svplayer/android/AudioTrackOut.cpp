#include "AudioTrackOut.h"
#include "android/jnihelper/jni_helpers.h"

jobject CAudioTrackOut::NewObject(JNIEnv* jni, jclass pjclass, const char* name, const char* signature, ...)
{
		va_list args;
		va_start(args, signature);
		jobject obj = jni->NewObjectV(pjclass, jni->GetMethodID(pjclass, name, signature),
			args);
		//CHECK_EXCEPTION(jni_) << "Error during NewObjectV";
		va_end(args);
		//return rtc::scoped_ptr<GlobalRef>(new GlobalRef(jni_, obj));
		return jni->NewGlobalRef(obj);
}

CAudioTrackOut::CAudioTrackOut() :jni_(GetEnv(GetJavaVM())), m_classname("cxw/cn/svplayer/AudioTrackPlay")
{
	jclass pj_class_ = jni_->FindClass(m_classname);
	j_class_ = reinterpret_cast<jclass>(jni_->NewGlobalRef(pj_class_));
	//m_initAudioTrack = jni_->GetMethodID(j_class_, "InitAudioTrack", "(II)I");
	//m_audioPlay = jni_->GetMethodID(j_class_, "AudioPlay", "([BI)I");
	m_javaObject = NewObject(jni_, pj_class_, "<init>", "()V");
}

CAudioTrackOut::~CAudioTrackOut()
{
	jni_->DeleteGlobalRef(j_class_);
	jni_->DeleteGlobalRef(m_javaObject);
}

int CAudioTrackOut::initAudioOut(int nchannels, int nsampleRate)
{
	int ret = 0;
	AttachThreadScoped attachthread(GetJavaVM());
	JNIEnv* jnienv = attachthread.env();
	//jclass javaclass = jnienv->GetObjectClass(m_javaObject);
	ret = jnienv->CallIntMethod(m_javaObject, jnienv->GetMethodID(j_class_, "InitAudioTrack", "(II)I"), nchannels, nsampleRate);
	//ret = jni_->CallIntMethod(m_javaObject, m_initAudioTrack, nchannels, nsampleRate);
	m_isInit = true;
	return ret;
}

int CAudioTrackOut::audioPlay(uint8_t* pdata, int nsize)
{
	int ret = 0;
	AttachThreadScoped attachthread(GetJavaVM());
	JNIEnv * jni = attachthread.env();
	jbyteArray jbuffer = jni->NewByteArray(nsize);
	jni->SetByteArrayRegion(jbuffer, 0, nsize, (jbyte *)pdata);
	ret = jni->CallIntMethod(m_javaObject, jni->GetMethodID(j_class_, "AudioPlay", "([BI)I"), jbuffer, nsize);
	 return ret;
}
