#include "CommonSetting_JniWrap.h"
#include "../src/CommonSetting.h"


static JNINativeMethod ls_nm[] = {
		{ "nativeSetLogLevel", "(I)V", reinterpret_cast<void*>(
		&CCommonSetting_JniWrap::nativeSetLogLevel) },
		{ "nativeToggleFFmpegLog", "(Z)V", reinterpret_cast<void*>(
		&CCommonSetting_JniWrap::nativeToggleFFmpegLog) } 
};
CRegisterNativeM CCommonSetting_JniWrap::s_registernm("cxw/cn/svplayer/CommonSetting", ls_nm, ARRAYSIZE(ls_nm));
void JNICALL CCommonSetting_JniWrap::nativeSetLogLevel(JNIEnv *env, jclass thiz, jint level)
{
	CCommonSetting::SetLogLevel(level);
}

void JNICALL CCommonSetting_JniWrap::nativeToggleFFmpegLog(JNIEnv *env, jclass thiz, jboolean enable)
{
	CCommonSetting::ToggleFFmpegLog(enable);

}
