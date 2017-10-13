#ifndef COMMONSETTING_JNIWRAP_H_
#define COMMONSETTING_JNIWRAP_H_
#include "android/jnihelper/jni_classloader.h"

class CCommonSetting_JniWrap
{
public:
	static CRegisterNativeM s_registernm;
	static void JNICALL nativeSetLogLevel(JNIEnv *env, jclass thiz, jint level);
	static void JNICALL nativeToggleFFmpegLog(JNIEnv *env, jclass thiz, jboolean enable);

};
#endif 
