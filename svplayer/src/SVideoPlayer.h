#ifndef VIDEO_PLAYER_H_
#define VIDEO_PLAYER_H_

#include <string>
#include <stdint.h>
extern "C"
{
#include "libavformat/avformat.h"
#include "libavcodec/avcodec.h"
#include "libswscale/swscale.h"
#include "libswresample/swresample.h"
#include <pthread.h>

}
#include "VideoRenderBase.h"
#include "AudioOutBase.h"
#include "PlayerEventObserver.h"
#include "Common.h"
#include"base/criticalsection.h"
#include "PlatformThreadEx.h"
#include <memory>
#include "BufferQuene.h"

class CI420Frame
{
public:
	uint8_t* m_framedata = nullptr;
	int m_datasize = 0;
	int m_width;
	int m_height;
	double m_pts;
	CI420Frame(uint8_t *srcdata, int width, int height, double pts)
	{
		m_width = width;
		m_height = m_height;
		m_pts = pts;
		if (srcdata != nullptr)
		{
			m_datasize = width * height * 3 / 2;
			m_framedata = new uint8_t[m_datasize];
			memcpy(m_framedata, srcdata, m_datasize);
		}
	}
	~CI420Frame()
	{
		if (m_framedata != nullptr)
		{
			delete m_framedata;
		}
	}
};
class CSVPlayer
{

	static void AVPacketFreeFun(void* ptr)
	{
		static int totalcount = 0;

		AVPacket* pkt = (AVPacket*)ptr;
		if (ptr != nullptr)
		{
			totalcount++;
			LOGD << "AVPacketFreeFun count = " << totalcount;
			av_free_packet(pkt);
		}
	}

	static void I420FrameFreeFun(void* ptr)
	{
		static int totalcount = 0;

		CI420Frame* pkt = (CI420Frame*)ptr;
		if (ptr != nullptr)
		{
			totalcount++;
			LOGD << "I420FrameFree count = "<<totalcount;
			delete pkt;
		}
	}


	std::string m_url;
	bool m_islive = false;
	bool m_abort = false;
	bool m_eof = false;
	int m_state;
	int m_error;
	// 1(default) audio_master
	int m_avsync_type = 0;
	bool m_paused = false;
	bool m_last_paused = false;

	int m_min_buffer_frames;
	//seek
	bool m_seek_request;
	int m_seek_flag;
	int m_seek_by_bytes;
	int64_t m_seek_pos;
	int64_t m_seek_rel;
	/* refresh thread to notify A/V decode thread it had checked seek */
	int m_seek_check_flag;
	bool m_is_seeking = false;

	float m_duration = 0;

	AVFormatContext *m_fmt_ctx;
	int m_probesize = 0;
	int m_max_analyze_duration = 0;
	int m_probetime = 0;

	int64_t m_read_time_us;

	/* use to judge read timeout */
	bool m_timeout;

	/* live read too much data, speed up*/
	bool m_fast_forward;


	// video params
	bool m_video_ok;
	bool m_video_disable;
	int m_video_index;
	AVStream *m_video_stream;
	AVCodecContext *m_video_ctx;
	struct SwsContext *m_sws_ctx;

	CBufferQuene<AVPacket*, &CSVPlayer::AVPacketFreeFun> m_videopktq;
	int m_width;
	int m_height;
	int m_frame_rate;
	CBufferQuene<CI420Frame*, &CSVPlayer::I420FrameFreeFun> m_videoframeq;

	double m_video_clock;
	int m_render_width;
	int m_render_height;
	int m_show_mode;

	// audio params
	bool m_audio_ok;
	bool m_audio_disable;
	int m_audio_index;
	AVStream *m_audio_stream;
	AVCodecContext *m_audio_ctx;
	struct SwrContext *m_swr_ctx = nullptr;
	CBufferQuene<AVPacket*, &CSVPlayer::AVPacketFreeFun> m_audiopktq;
	int m_channels;
	int m_sample_rate;
	int64_t m_channel_layout;
	enum AVSampleFormat m_sample_fmt;
	int m_audio_min_buffer_size;
	double m_audio_clock;

	//player control
	//audio 
	int m_muted = false;
	//


	rtc::CriticalSection m_prender_cs;
	VideoRenderBase *m_vrender = nullptr;
	rtc::CriticalSection m_audioout_cs;
	AudioOutBase *m_audioout = nullptr;


	PlayEventObserver* m_eventobserver = nullptr;
	void NotifyEvent(PlayEvent evt, long data1 = 0, void* data2 = NULL);
	//
	CPlatformThreadEx<CSVPlayer>  m_readThread;
	bool onRead();

	//pthread_t m_video_decode;
	CPlatformThreadEx<CSVPlayer>  m_videoDecodeThread;
	bool onVideoDecode();

	CPlatformThreadEx<CSVPlayer>  m_audioDecodeThread;
	bool onAudioDecode();

	CPlatformThreadEx<CSVPlayer>  m_videoRefreshThread;
	bool onVideoRender();

	void resetParam();

	int openUrl();
	int findStreamInfo();
	int openStreams();
	int stream_component_open(int stream_index);
	void seekStreamTo(int64_t value, int64_t ref, int byByte);

	void setState(PlayerState state, PlayerError errorn);

	int playSleep(double actual_delay);

	static void ffmpeglog_callback(void* avcl, int level, const char* pFormat, va_list vl);
	static int play_interrupt_cb(void *arg);
	

	double get_master_clock();
	double synchronize_video(AVFrame *src_frame, double pts);
	void StopWaitClean();

	
public:
	static void FFmpegInit();
	static void ToggleFFmpegLog(bool enable);
	CSVPlayer();
	float GetDuration()
	{
		return m_duration;
	}
	int GetCurrentPosition()
	{
		return get_master_clock();
	}
	void setUrl(std::string url)
	{
		m_url = url;
	}
	void setIsLive(bool blive)
	{
		m_islive = blive;
	}
	int startPlayer();
	int pausePlayer();
	int resumePlayer();
	int seekPlayer(int seconds);
	int stopPlayer();
	int resetPlayer();
	int captureImage();
	int speedPlayer(SPEED_RATIO speedratio);
	int muteAudio(bool ismute);

	bool isLive()
	{
		return m_islive;
	}

	bool isPlaying()
	{
		if (m_state < kStateBuffering || (m_state < kStateRecording && m_state >= kStateReconnecting))
		{
			return false;
		}
		return true;
	}
	void setProbeTime(int ptime)
	{
		if (ptime > 0)
		{
			m_probetime = ptime;
		}
	}
	VideoRenderBase* getVideoRender()
	{
		return m_vrender;
	}
	void setVideoRender(VideoRenderBase* vrb)
	{
		rtc::CritScope autolock(&m_prender_cs);
		m_vrender = vrb;
	}

	AudioOutBase* getAudioOut()
	{
		return m_audioout;
	}
	void setAudioOut(AudioOutBase* aob)
	{
		rtc::CritScope autolock(&m_audioout_cs);
		m_audioout = aob;
	}

	void setEventObserver(PlayEventObserver* peo)
	{
		m_eventobserver = peo;
	}

};
#endif 