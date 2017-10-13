#include "CommonSetting.h"
#include "SVideoPlayer.h"
#if 0
#include "MemoryTrace.hpp"
#endif
#include <fstream>

void CCommonSetting::SetLogLevel(int level)
{
	rtc::LogMessage::SetMinLogSeverity((rtc::LoggingSeverity)level);
	rtc::LogMessage::LogToDebug((rtc::LoggingSeverity)rtc::LogMessage::GetMinLogSeverity());

}

void CCommonSetting::ToggleFFmpegLog(bool enable)
{
	CSVPlayer::ToggleFFmpegLog(enable);
	/*if (enable)
	{
		StartLeakMemDetect();
	}
	else
	{
		StopLeakMemDetect();
	}*/
}

void CCommonSetting::StartLeakMemDetect()
{
#if 0
	leaktracer::MemoryTrace::GetInstance().startMonitoringAllThreads();

#endif

}
void CCommonSetting::StopLeakMemDetect()
{
#if 0
	//leaktracer::MemoryTrace::GetInstance().stopMonitoringAllocations();
	leaktracer::MemoryTrace::GetInstance().stopAllMonitoring();
	std::ofstream oleaks;
	oleaks.open("/storage/emulated/0/leaks.out", std::ios_base::out);
	if (oleaks.is_open())
		leaktracer::MemoryTrace::GetInstance().writeLeaks(oleaks);
	else
		LOGE << "Failed to write to \"leaks.out\"\n";

#endif
	
}
