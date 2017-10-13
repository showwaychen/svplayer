#ifndef COMMON_SETTING_H_
#define COMMON_SETTING_H_
#include "base/logging.h"
#include"Common.h"
class CCommonSetting
{
public:
	static void SetLogLevel(int level);
	static void ToggleFFmpegLog(bool enable);
	static void StartLeakMemDetect();
	static void StopLeakMemDetect();
};
#endif /* COMMON_SETTING_H_ */