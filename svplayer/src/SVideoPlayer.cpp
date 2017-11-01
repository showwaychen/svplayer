#include"SVideoPlayer.h"
#include <sys/prctl.h>
#include "Common.h"
#include "Log.h"
#include <stdint.h>
#include <time.h>
#include "CommonSetting.h"

AVPacket flush_pkt = { 0 };

static int reorder_pts = -1; /* 0=off 1=on -1=auto */



void CSVPlayer::NotifyEvent(PlayEvent evt, long data1, void* data2)
{
	if (m_eventobserver != nullptr)
	{
		m_eventobserver->OnEvent(evt, data1, data2);
	}
}

bool CSVPlayer::onRead()
{
	prctl(PR_SET_NAME, "readThread");

	
	int64_t probtime;
	int state;
	int error;
	bool abort, eof;
	int ret = 0;

connect:
	probtime = av_gettime();
	NotifyEvent(PE_Opening);
	setState(kStateConnecting, kErrorNone);

	if ((ret = openUrl())) {
		setState(kStateConnectionFailed, kErrorNone);
		NotifyEvent(PE_OpenedError);
		return false;
	}

	if ((ret = findStreamInfo())) {
		setState(kStateConnectionFailed, kErrorNone);
		NotifyEvent(PE_OpenedError);
		goto fail;
	}

	if ((ret = openStreams())) {
		if (AVERROR(kErrorCodecOpenedFailed) == ret &&
			(!m_width || !m_height) &&
			m_probesize) {
			m_probesize = 0;
			m_max_analyze_duration = 0;
			goto connect;
		}
		setState(kStateConnectionFailed, kErrorNone);
		NotifyEvent(PE_OpenedError);
		goto fail;
	}
	setState(kStateConnected, kErrorNone);
	NotifyEvent(PE_Opened);
	setState(kStateBuffering, kErrorNone);
	NotifyEvent(PE_Buffering);

	if (m_video_ok) {
		m_videoDecodeThread.Start();
		m_videoRefreshThread.Start();
	}

	if (m_audio_ok) {
		m_audioDecodeThread.Start();
	}
	m_timeout = false;
	while (1) {
		if (m_abort || !isPlaying()) {
			LOGI << "stream read thread check abort, abort: , state: "<< m_abort<< m_state;
			break;
		}
		if (m_seek_request) {

			if (AV_NOPTS_VALUE != m_fmt_ctx->start_time && m_seek_pos < m_fmt_ctx->start_time) {
				m_seek_pos = m_fmt_ctx->start_time;
			}
			LOGI << "file duration:"<< m_duration;
			if (!m_islive && (m_seek_pos / (double)AV_TIME_BASE >= m_duration - 1)) {
				LOGW<<"seek reach end";
				m_eof = true;
				if (m_video_ok && !m_video_disable) {
					m_videopktq.Clear();
				}
				if (m_audio_ok && !m_audio_disable) {
					m_audiopktq.Clear();
				}
				break;
			}

			int64_t seek_target = m_seek_pos;
			int64_t seek_min = m_seek_rel > 0 ? seek_target - m_seek_rel + 2 : INT64_MIN;
			int64_t seek_max = m_seek_rel < 0 ? seek_target - m_seek_rel - 2 : INT64_MAX;

			LOGI << "check seek request, target: " << seek_target << ", seek_rel: " << m_seek_rel << " min : " << seek_min << ", max : " << seek_max;

			ret = avformat_seek_file(m_fmt_ctx, -1, seek_min, seek_target, seek_max, m_seek_flag);

			if (ret < 0) {
				LOGW << "seek failed, target: " << seek_target / (double)AV_TIME_BASE<<" , continue to play   ";
				NotifyEvent(PE_PlaySeekFailed);
				m_is_seeking = false;
			}
			else {
				m_seek_check_flag = 0;
				if (m_video_ok && !m_video_disable) {
					m_videopktq.Clear();
					m_videopktq.PushData(&flush_pkt, true);
				}
				if (m_audio_ok && !m_audio_disable) {
					m_audiopktq.Clear();
					m_audiopktq.PushData(&flush_pkt, true);
					
				}
			}
			m_seek_request = 0;
		}
		int pause_code = 0;
		if (m_paused != m_last_paused) {
			if (m_paused) {
				if (kStateBuffering != m_state) {
					m_last_paused = m_paused;
					pause_code = av_read_pause(m_fmt_ctx);
				}
			}
			else {
				m_last_paused = m_paused;
				av_read_play(m_fmt_ctx);
			}
		}

		if (m_paused && kStateBuffering != m_state) {
			usleep(20 * 1000);
			continue;
		}



		m_read_time_us = av_gettime();
		AVPacket* pkt = av_packet_alloc();

		pkt->flags = 0;
		ret = av_read_frame(m_fmt_ctx, pkt);

		m_read_time_us = 0;

		if (ret < 0) {

			if (m_timeout) {
				goto reconnect;
			}

			if ((AVERROR_EOF == ret/* || avio_feof(ctx->fmt_ctx->pb)*/) && !eof) {
				/* we set interrupt callback to interrupt block if we need,
				* but it bring out a problem that if we stop manually or network down,
				* a live stream will reach eof, can not find a reason.
				* Deal with the problem here.
				*/
				if (m_abort) {
					break;
				}
				else {
					if (m_islive) {
						goto reconnect;
					}
				}
				m_eof = true;
				LOGW<<"read reach eof";
				break;
			}

			if (m_fmt_ctx->pb && m_fmt_ctx->pb->error) {
				LOGE<<"stream read error, ret: " << ret<< ", error: "<< m_fmt_ctx->pb->error;
				/* https://ffmpeg.org/pipermail/libav-user/2013-April/004430.html
				* play http vod video, resume after pause a long time(eg. several minutes),
				* av_read_frame might return AVERROR_INVALIDDATA, can not find a reason.
				* Deal with the problem here.
				*/
				if (!m_islive && -1094995529 == ret && -104 == m_fmt_ctx->pb->error) {
					LOGD<<"vod read error after resume, try again";
					continue;
				}
				setState(kStateStoppedWithError, kErrorStreamReadError);
				NotifyEvent(PE_PlayError);
				break;
			}
			continue;
		}
	
		if (m_video_ok && pkt->stream_index == m_video_index) {
			if (!m_videopktq.PushData(pkt, true))
			{
				av_free_packet(pkt);
			}
		}
		else if (m_audio_ok && pkt->stream_index == m_audio_index) {
			if (m_audio_disable) {
				m_audio_disable = false;
				m_avsync_type = AV_SYNC_AUDIO_MASTER;
			}
			if (!m_audiopktq.PushData(pkt, true))
			{
				av_free_packet(pkt);
			}
		}
		else {
			av_free_packet(pkt);
		}

		if (m_audio_ok &&
			!m_audio_disable &&
			!m_audiopktq.Size() /*&&
			av_gettime() - audio_probe_time > 5000000*/) {
			m_audio_disable = true;
			m_avsync_type = AV_SYNC_VIDEO_MASTER;
		}

		int nb_video = (m_video_ok && !m_video_disable) ? m_videopktq.Size() : 0;
		int nb_audio = (m_audio_ok && !m_audio_disable) ? m_audiopktq.Size(): 0;
		if (kStateBuffering == m_state) {
			if (((!m_video_ok || m_video_disable || nb_video >= m_min_buffer_frames)
				|| (!m_audio_ok || m_audio_disable || nb_audio >= m_min_buffer_frames))) {
				setState(kStateReadyToPlay, kErrorNone);
				NotifyEvent(PE_ReadyToPlay);
			}
			else {
				nb_video = nb_video > m_min_buffer_frames ? m_min_buffer_frames : nb_video;
				nb_audio = nb_audio > m_min_buffer_frames ? m_min_buffer_frames : nb_audio;

				int total_video = (m_video_ok && !m_video_disable) ? m_min_buffer_frames : 0;
				int total_audio = (m_audio_ok && !m_audio_disable) ? m_min_buffer_frames : 0;
				//int percent = (nb_audio + nb_video) * 100 / (total_audio + total_video);
				setState(kStateBuffering, kErrorNone);
				NotifyEvent(PE_Buffering);
			}
		}
		else {
			if (m_islive) {
				if ((!m_video_ok || m_video_disable || nb_video > LIVE_VIDEO_PACKET_LIMIT)
					&& (!m_audio_ok || m_audio_disable || nb_audio > LIVE_AUDIO_PACKET_LIMIT)) {
					m_fast_forward = true;
				}
				else {
					m_fast_forward = false;
				}
			}
		}

		usleep(5 * 1000);

	} // while (1)

	abort = m_abort;
	if (!m_eof) {
		state = m_state;
		error = m_error;
	}
	else {
		state = kStateStoppedWithEOF;
		NotifyEvent(PE_StopWithEof);
		error = m_error = kErrorNone;
		m_eof = true;
	}
	if (kStateReconnecting == state) {
		LOGI<<"other thread call reconnect";
		if (abort != 1) {
			LOGI<<"abort != 1 reconnect";
			goto reconnect;
		}
	}
	//m_audiopktq.SetEnable(false);
	//m_videopktq.SetEnable(false);
	StopWaitClean();
	if (kStateStoppedWithError == state) {
	}
	else {
		if (kStateStoppedWithEOF == state) {
		}
		else {
			state = kStateStoppedByUser;
		}
	}
	if (!abort) {
		LOGI<<"stream_read_thread state:  error "<< state<< error;
	}
	goto end;
reconnect:
	setState(kStateReconnecting, kErrorNone);
	m_paused = m_last_paused = 0;
	m_is_seeking = false;
	m_seek_request = false;
	if (!m_islive) {
		seekStreamTo((int64_t)(get_master_clock() * AV_TIME_BASE), (int64_t)(get_master_clock() * AV_TIME_BASE), 0);
	}
	StopWaitClean();
	m_abort = false;
	goto connect;
fail:
	StopWaitClean();
end:
	LOGD<<"stream read thread e, pid:  , tid : "<< (unsigned long)getpid()<< (unsigned long)pthread_self();
   return  false;
}



bool CSVPlayer::onVideoDecode()
{
	prctl(PR_SET_NAME, "videoDecodeThread");

	int width = m_width;
	int height = m_height;
	int buf_size = width * height * 3 / 2;
	uint8_t *buf = (uint8_t *)av_mallocz(buf_size * sizeof(uint8_t));

	int ret = 0;
	int error = kErrorNone;
	int got_frame = 0;
	double pts = 0.0;
	AVPacket pkt = { 0 };
	av_init_packet(&pkt);
	AVFrame *frame = av_frame_alloc();
	AVFrame *scale_frame = av_frame_alloc();

	bool nb = false;
	int64_t queue_probe_time = av_gettime();
	while (1) {

		if (m_abort || !isPlaying()) {
			LOGI<<"video decode thread check abort, abort:, state:"<< m_abort<< m_state;
			break;
		}

		if (m_paused || (kStateBuffering == m_state && !m_is_seeking)) {
			usleep(20 * 1000);
			continue;
		}
		//LOGI("packet_queue_get video");

		AVPacket* tmp = nullptr;
		nb = m_videopktq.PullData(&tmp, true);
		if (!nb) {
			//LOGI("packet_queue_get no packet");
			if (!m_islive && m_eof) {
				CI420Frame *tmp = new CI420Frame(nullptr, 0, 0, -999.0);
				if (!m_videoframeq.PushData(tmp, true))
				{
					delete tmp;
				}
				break;
			}
			if (av_gettime() - queue_probe_time > 1000000) {
				if (kStateBuffering != m_state) {
					if (!m_islive) {
						m_min_buffer_frames = 80;
					}
					else {
						m_min_buffer_frames = 4;
					}

					setState(kStateBuffering, kErrorNone);
				}
			}

			usleep(20 * 1000);
			continue;
		}
		pkt = *tmp;
		if (pkt.data == flush_pkt.data) {
			LOGD<<"video decode check flush";
			avcodec_flush_buffers(m_video_ctx);
			m_videoframeq.Clear();
			m_videoframeq.PushData(new CI420Frame(nullptr, 0, 0, -111.0), true);
			/* FIXME
			* wait video refresh thread to notify it had checked seek
			* if the timestamp is discontinuous, refresh thread maybe delay a long time,
			* now we set AV_NOSYNC_THRESHOLD 10.0, so the longest time we need to wait is 10s
			*/
			while (!m_seek_check_flag && !m_abort &&
				(m_state >= kStateBuffering && m_state < kStateReconnecting)) {
				usleep(5 * 1000);
			}
			if (AV_SYNC_VIDEO_MASTER == m_avsync_type) {
				if (m_seek_flag & AVSEEK_FLAG_BYTE) {
					m_video_clock = 0;
				}
				else {
					m_video_clock = m_seek_pos / (double)AV_TIME_BASE;
				}
				m_is_seeking = 0;            // seek finished
				NotifyEvent(PE_SeekFinished);
			}
			while (m_is_seeking  && !m_abort &&
				(m_state >= kStateBuffering && m_state < kStateReconnecting)) {
				usleep(5 * 1000);
			}
			continue;
		}

		ret = avcodec_decode_video2(m_video_ctx, frame, &got_frame, &pkt);
		if (ret > 0 && got_frame) {


			if (frame->width != m_width || frame->height != m_height) {
				if (m_islive) {
					m_state = kStateReconnecting;
					m_error = kErrorNone;
				}
				else {
					m_state = kStateStoppedWithEOF;
					NotifyEvent(PE_StopWithEof);
					m_error = kErrorNone;
				}
				break;
			}

			if (m_NeedSaveToFile)
			{
				if (0 == SaveToFile(frame, m_CaptureFilename))
				{
					NotifyEvent(PE_Snapshot_Sus);
				}
				else
				{
					NotifyEvent(PE_Snapshot_Fail);
				}
				m_NeedSaveToFile = false;
			}
			if (-1 == reorder_pts) {
				pts = (double)av_frame_get_best_effort_timestamp(frame);
				LOGD<<"video pts:"<< pts;
			}
			else if (1 == reorder_pts) {
				pts = (double)pkt.pts;
			}
			else {
				pts = (double)pkt.dts;
			}

			LOGD<<"video pts:"<< pts;

			if (AV_NOPTS_VALUE == pts) {
				pts = 0.0;
			}
			LOGD<<"pts time_base: "<< m_video_stream->time_base.den<< m_video_stream->time_base.num;
			pts *= av_q2d(m_video_stream->time_base);


			pts = synchronize_video(frame, pts);


			ret = avpicture_fill((AVPicture*)scale_frame, buf, AV_PIX_FMT_YUV420P, width, height);

			if (ret < 0) {
				error = kErrorUnknown;
				goto fail;
			}

			LOGD<<"pixel format:   420:  "<< m_video_ctx->pix_fmt<<"  "<< AV_PIX_FMT_YUV420P;
			m_sws_ctx = sws_getCachedContext(m_sws_ctx, width, height, m_video_ctx->pix_fmt,
				width, height, AV_PIX_FMT_YUV420P, SWS_X, NULL, NULL, NULL);
			if (!m_sws_ctx) {
				LOGE<<"get cache sws context failed";
				error = kErrorUnknown;
				goto fail;
			}


			ret = sws_scale(m_sws_ctx, (uint8_t const * const *)frame->data, frame->linesize, 0, height,
				scale_frame->data, scale_frame->linesize);
			if (ret < 0) {
				LOGE<<"sws_scale failed";
				error = kErrorUnknown;
				goto fail;
			}
			CI420Frame *tmp = new CI420Frame(buf, width, height, pts);
			if (!m_videoframeq.PushData(tmp, true))
			{
				delete tmp;
			}
		}

		queue_probe_time = av_gettime();

		av_free_packet(&pkt);

		usleep(5 * 1000);

	} 

	goto end;

fail:
	if (kStateStoppedWithError != m_state) {
		m_state = kStateStoppedWithError;
		m_error = error;
	}
	else {
		LOGD<<"error already occured";
	}
end:
	m_videopktq.SetEnable(false);
	m_videoframeq.SetPushEnable(false);
	//m_videoframeq.SetEnable(false);
	av_free_packet(&pkt);
	av_frame_free(&frame);
	av_frame_free(&scale_frame);
	av_freep(&buf);
	LOGD << "video decode thread e, pid:, tid : "<< (unsigned long)getpid()<< (unsigned long)pthread_self();
	return false;
}


bool CSVPlayer::onAudioDecode()
{
	prctl(PR_SET_NAME, "audioDecodeThread");
	LOGD << "audio decoded thread start";
	//PacketQueue *audio_pktq = &m_audio_pktq;
	int ret = 0;
	int error = kErrorNone;
	int len1 = 0;
	int len2 = 0;
	int decoded_data_size = 0;
	int wanted_nb_samples = 0;
	int dst_nb_samples = 0;
	int resampled_data_size = 0;
	int got_frame = 0;
	int bytes_per_second = m_channels * m_sample_rate * av_get_bytes_per_sample(m_sample_fmt);

	int64_t src_channel_layout = m_channel_layout;
	int src_sample_fmt = m_sample_fmt;
	int src_sample_rate = m_sample_rate;
	int src_channels = m_channels;

	int64_t dst_channel_layout;

	AVPacket pkt = { 0 };
	av_init_packet(&pkt);
	AVFrame *frame = av_frame_alloc();

	uint8_t buf[AVCODEC_MAX_AUDIO_FRAME_SIZE] = { 0 };
	uint8_t mute_buf[AVCODEC_MAX_AUDIO_FRAME_SIZE] = { 0 };

	double last_delay = 0.0;
	double last_pts = 0.0;
	double pts = 0.0;
	double ref_pts = 0.0;
	double diff = 0.0;
	double delay = 0;

	bool nb = false;
	int64_t queue_probe_time = av_gettime();

	if (true){
		rtc::CritScope autolock(&m_audioout_cs);
		if (m_audioout != nullptr && !m_audioout->isInit())
		{
			m_audio_min_buffer_size = m_audioout->initAudioOut(m_channels, m_sample_rate);
			if (m_audio_min_buffer_size <= 0){
				LOGE<<"init AudioTrack failed";
				error = kErrorCodecOpenedFailed;
				goto fail;

			}
			else
			{
				LOGI << "create AudioTrack successfully";
			}
		}
	}
	while (1) {

		if (m_abort || !isPlaying()) {
			LOGI<<"audio decode thread check abort, abort: , state: "<< m_abort<< m_state;
			break;
		}

		if (m_paused || (kStateBuffering == m_state && !m_is_seeking) || m_audio_disable) {
			usleep(50 * 1000);
			continue;
		}
		//nb = packet_queue_get(audio_pktq, &pkt, 0);
		AVPacket* tmp = nullptr;
		nb = m_audiopktq.PullData(&tmp, true);
		if (!nb) {
			if (!m_islive && m_eof) {
				break;
			}
			if (av_gettime() - queue_probe_time > 1000000) {
				if (kStateBuffering != m_state) {
					if (!m_islive) {
						m_min_buffer_frames = 80;
					}
					else {
						m_min_buffer_frames = 4;
					}
					setState(kStateBuffering, kErrorNone);
				}
			}
			usleep(50 * 1000);
			continue;
		}
		pkt = *tmp;
		if (pkt.data == flush_pkt.data) {
			LOGD<<"audio decode check flush after seek";
			avcodec_flush_buffers(m_audio_ctx);
			if (m_video_ok) {
				while (!m_seek_check_flag && !m_abort &&
					(m_state >= kStateBuffering && m_state < kStateReconnecting)) {
					usleep(5 * 1000);
				}
			}
			if (AV_SYNC_VIDEO_MASTER != m_avsync_type) {
				if (m_seek_flag & AVSEEK_FLAG_BYTE) {
					m_audio_clock = 0;
				}
				else {
					m_audio_clock = m_seek_pos / (double)AV_TIME_BASE;
				}
				m_is_seeking = 0;
				NotifyEvent(PE_SeekFinished);
			}
			while (m_is_seeking && !m_abort &&
				(m_state >= kStateBuffering && m_state < kStateReconnecting)) {
				usleep(5 * 1000);
			}
			continue;
		}

		if (AV_NOPTS_VALUE != pkt.pts) {
			pts = pkt.pts * av_q2d(m_audio_stream->time_base);
		}
		else {
			pts = 0.0;
		}


		while (pkt.size > 0) {


			// unreference AVFrame for next decode
			av_frame_unref(frame);

			len1 = avcodec_decode_audio4(m_audio_ctx, frame, &got_frame, &pkt);

			if (len1 < 0) {
				LOGD<<"audio decode failed, get another packet, len1: "<< len1;
				break;
			}

			pkt.data += len1;
			pkt.size -= len1;

			if (got_frame <= 0) {
				LOGD<<"can not decode a packet, try forward";
				continue;
			}


			decoded_data_size = av_samples_get_buffer_size(NULL, frame->channels, frame->nb_samples,
				(AVSampleFormat)(frame->format), 1);


			if (frame->channel_layout &&
				av_frame_get_channels(frame) == av_get_channel_layout_nb_channels(frame->channel_layout)) {
				dst_channel_layout = frame->channel_layout;
			}
			else{
				dst_channel_layout = av_get_default_channel_layout(av_frame_get_channels(frame));
			}


			wanted_nb_samples = frame->nb_samples;
			if (kStateReadyToPlay == m_state) {
				LOGD << "dst layout: " << src_channel_layout;
				setState(kStatePlaying, kErrorNone);
				NotifyEvent(PE_Playing);
			}

			if (!m_muted)
			{
				if ((frame->format != src_sample_fmt
					|| dst_channel_layout != src_channel_layout
					|| frame->sample_rate != src_sample_rate
					|| (wanted_nb_samples != frame->nb_samples)) && !m_swr_ctx) {

					if (m_swr_ctx) {
						swr_free(&m_swr_ctx);
						if (m_islive) {
							m_state = kStateReconnecting;
							m_error = kErrorNone;
						}
						else {
							m_state = kStateStoppedWithEOF;
							NotifyEvent(PE_StopWithEof);
							m_error = kErrorNone;
						}
						break;
					}


					m_swr_ctx = (struct SwrContext *)swr_alloc_set_opts(NULL, src_channel_layout,
						(AVSampleFormat)src_sample_fmt, src_sample_rate, dst_channel_layout,
						(enum AVSampleFormat)frame->format, frame->sample_rate, 0, NULL);


					if (!m_swr_ctx || swr_init(m_swr_ctx) < 0) {
						LOGE << "swr_ctx create failed, try again";
						swr_free(&m_swr_ctx);
						m_swr_ctx = NULL;
						break;
					}


					src_channel_layout = dst_channel_layout;
					src_channels = av_frame_get_channels(frame);
					src_sample_fmt = frame->format;
					src_sample_rate = frame->sample_rate;
				}

				if (m_swr_ctx) {

					const uint8_t **in = (const uint8_t **)frame->extended_data;
					uint8_t *out[] = { buf };


					if (wanted_nb_samples != frame->nb_samples) {
						if (swr_set_compensation(m_swr_ctx,
							(wanted_nb_samples - frame->nb_samples) * m_sample_rate / frame->sample_rate,
							wanted_nb_samples * m_sample_rate / frame->sample_rate) < 0) {
							LOGE << "swr_set_compensation failed";
							break;
						}
					}


					int dst_nb_samples = sizeof(buf) / m_channels / av_get_bytes_per_sample(m_sample_fmt);

					len2 = swr_convert(m_swr_ctx, out, dst_nb_samples, in, frame->nb_samples);


					if (len2 < 0) {
						LOGE << "swr_convert() failed, len2:  " << len2;
						error = kErrorUnknown;
						goto fail;
					}

					if (len2 == dst_nb_samples) {
						swr_init(m_swr_ctx);
					}

					resampled_data_size = len2 * m_channels * av_get_bytes_per_sample(m_sample_fmt);

				}
				else {
					resampled_data_size = decoded_data_size;
					memcpy(buf, frame->data[0], resampled_data_size);
				}

				if (!m_islive) {
					ref_pts = get_master_clock();
					diff = pts - ref_pts;
					delay = 0;
					if (fabs(diff) >= AUDIO_SYNC_THRESHOLD_S) {
						if (pts > ref_pts) {
							LOGD << "audio clock is far ahead, diff:  " << diff;
							m_audio_clock = pts;
							delay = diff;
							ret = playSleep(delay);
							if (-1 == ret) {
								break;
							}
							else if (-2 == ret) {
								continue;
							}
						}
						else {
						}
					}
				}
			}
			

			char *play_buf = NULL;
			if (!m_muted && !m_fast_forward)
			{
				play_buf = (char *)buf;
			}
			else
			{
				play_buf = (char *)mute_buf;
			}
			{
				rtc::CritScope autolock(&m_audioout_cs);
				if (m_audioout != nullptr)
				{
					
					m_audioout->audioPlay((uint8_t*)play_buf, resampled_data_size);
				}
			}
				

			// Keep audio_clock up-to-date
			m_audio_clock = last_pts = pts + (double)resampled_data_size / bytes_per_second;


		} // while decode

		queue_probe_time = av_gettime();

		av_free_packet(&pkt);

		usleep(5 * 1000);

	} // end while (1)

	goto end;

fail:
	if (kStateStoppedWithError != m_state) {
		m_state = kStateStoppedWithError;
		m_error = error;
	}
	else {
		LOGD<<"error already occured";
	}
end:
	m_audiopktq.SetEnable(false);
	av_free_packet(&pkt);
	av_frame_free(&frame);
	LOGD<<"audio decode thread e, pid: , tid: "<< (unsigned long)getpid()<< (unsigned long)pthread_self();
	return false;
}


bool CSVPlayer::onVideoRender()
{
	prctl(PR_SET_NAME, "videoRenderThread");

	//PictureQueue *video_picq = &m_video_picq;
	int width = m_width;
	int height = m_height;
	int buf_size = width * height * 3 / 2;
	uint8_t *buf = (uint8_t *)av_mallocz(buf_size * sizeof(uint8_t));

	double pts = 0.0;
	double last_pts = 0.0;
	double ref_pts = 0.0;
	double diff = 0.0;
	double delay = 0.0;
	double actual_delay = 0.0;
	double last_delay = 40e-3;
	double sync_threshold = 0.0;
	double frame_timer = (double)av_gettime() / 1000000.0;

	int ret = 0;

	{
		rtc::CritScope autolock(&m_prender_cs);
		if (m_vrender != nullptr)
		{
			m_vrender->setSrcImageSize(m_width, m_height);
		}
	}
	while (1) {

		if (m_abort || !isPlaying()) {
			LOGI << "video refresh thread check abort, abort: , state: " << m_abort << m_state;
			break;
		}

		if (m_paused || (kStateBuffering == m_state && !m_is_seeking)) {
			usleep(20 * 1000);
			continue;
		}

		CI420Frame *i420frame = nullptr;
		bool gotdata = m_videoframeq.PullData(&i420frame, true);
		if (gotdata)
		{
			m_render_width = i420frame->m_width;
			m_render_height = i420frame->m_height;
			pts = i420frame->m_pts;
			if (i420frame->m_framedata == nullptr)
			{
				memset(buf, 0, buf_size);
			}
			else
			{
				memcpy(buf, i420frame->m_framedata, buf_size);
			}
			delete i420frame;
		}

		if (-999.0 == pts) {
			LOGD << "video refresh thread check eof";
			break;
		}
		else if (-111.0 == pts) {
			frame_timer = (double)av_gettime() / 1000000.0;
			m_seek_check_flag = 1;
			LOGD << "video refresh check seek flag";
			continue;
		}

		delay = pts - last_pts;
		LOGD << "delay: , last_delay:  pts: " << delay << last_delay << pts;
		if (delay <= 0 || delay >= 1.0) {
			delay = last_delay;
		}
		LOGD << "delay:  " << delay;
		actual_delay = last_delay = delay;
		last_pts = pts;

		if (AV_SYNC_VIDEO_MASTER != m_avsync_type) {

			ref_pts = get_master_clock();
			diff = pts - ref_pts;
			LOGD << "diff: " << diff;
			sync_threshold = (delay > AV_SYNC_THRESHOLD_S) ? delay : AV_SYNC_THRESHOLD_S;
			LOGD << "pts: , ref pts:  , sync_threshold:  " << pts << ref_pts << sync_threshold;
			//这里计算最终的延时播放时间，也就是delay的值
			if (fabs(diff) < AV_NOSYNC_THRESHOLD_S) {

				if (diff <= -sync_threshold) {
					
					delay = 0;
				}
				else if (diff >= sync_threshold) {
					delay = diff;
				}

			}
			else {
				continue;
			}
			actual_delay = delay;
			if (actual_delay < -AV_DROP_THRESHOLD_S) {
				// too late, drop a frame
				continue;
			}
			else {
				if (actual_delay < AV_SYNC_THRESHOLD_S) {
					actual_delay = AV_SYNC_THRESHOLD_S;
				}
			}
			LOGD<<"actual delay: "<< actual_delay;
		}
		else {
			if (m_fast_forward) {
				actual_delay = AV_SYNC_THRESHOLD_S;
			}
		}

		if (kStateReadyToPlay == m_state) {
			setState(kStatePlaying, kErrorNone);
			NotifyEvent(PE_Playing);
		}


		if (!m_fast_forward) {
			LOGD << "m_fast_forward false";
				rtc::CritScope autolock(&m_prender_cs);
				if (m_vrender != nullptr)
				{
					m_vrender->renderImage(buf, buf_size);
				}
				else
				{
					LOGW << "render is null";
				}
		}

		ret = playSleep(actual_delay);
		if (-1 == ret) {
			break;
		}
		else if (-2 == ret) {
			continue;
		}

	} // while (1)

	m_videoframeq.SetEnable(false);
	m_videoframeq.Clear();
	av_freep(&buf);
	return false;
}

void CSVPlayer::resetParam()
{
	m_abort = 0;
	m_eof = 0;
	m_state = kStateNone;
	m_error = kErrorNone;
	m_paused = m_last_paused = 0;
	m_avsync_type = AV_SYNC_AUDIO_MASTER; // default sync to audio

	m_seek_by_bytes = -1;
	m_seek_request = false;
	m_seek_flag = 0;
	m_seek_pos = 0;
	m_seek_rel = 0;
	m_is_seeking = false;

	m_fast_forward = 0;

	m_duration = 0.0;
	m_fmt_ctx = NULL;

	m_probesize = 0;
	m_max_analyze_duration = 0;
	// video params
	m_video_index = -1;
	m_video_stream = NULL;
	m_video_ctx = NULL;
	m_sws_ctx = NULL;
	m_video_ok = false;
	m_video_disable = true;
	m_width = 0;
	m_height = 0;
	//m_frame_rate = 0;
	m_video_clock = 0.0;
	m_render_height = 0;
	m_render_width = 0;
	//m_render_buf = NULL;
	//m_rotate_render_buf = NULL;

	// audio params
	m_audio_index = -1;
	m_audio_stream = NULL;
	m_audio_ctx = NULL;
	//m_swr_ctx = NULL;
	m_audio_ok = false;
	m_audio_disable = true;
	m_channels = 0;
	m_sample_rate = 0;
	m_channel_layout = 0;
	m_sample_fmt = AV_SAMPLE_FMT_S16;
	m_audio_min_buffer_size = 0;
	m_audio_clock = 0.0;
	//m_muted = 0;

	m_min_buffer_frames = MIN_FRAMES;

	
}

int CSVPlayer::openUrl()
{
	LOGD << "open url s"<< m_url;
	int nErr = 0;
	if (m_abort) {
		return AVERROR(kErrorOpenUrlError);
	}
	if (m_fmt_ctx) {
		avformat_close_input(&m_fmt_ctx);
	}

	m_fmt_ctx = avformat_alloc_context();
	m_fmt_ctx->interrupt_callback.callback = play_interrupt_cb;
	m_fmt_ctx->interrupt_callback.opaque = this;

	int64_t time_s = av_gettime();
	m_read_time_us = time_s;
	//m_fmt_ctx->flags |= AVFMT_FLAG_NOBUFFER;
	m_fmt_ctx->flags |= AVFMT_FLAG_NONBLOCK;

	if (m_probesize) {
		m_fmt_ctx->probesize = m_probesize;
		m_fmt_ctx->max_analyze_duration = m_max_analyze_duration;
	}
	AVDictionary *opt = nullptr;
	av_dict_set(&opt, "timeout", NULL, 0);
	LOGD << "probesize =   aduration = " << m_fmt_ctx->probesize << "  " << m_fmt_ctx->max_analyze_duration;
	if ((nErr = avformat_open_input(&m_fmt_ctx, m_url.c_str(), NULL, &opt)) != 0) {
		LOGD << "avformat_open_input failed s nErr = "<< nErr;
		char errbuf[256];
		const char *errbuf_ptr = errbuf;  
		if (nErr != 1)
		{
			if (av_strerror(nErr, errbuf, sizeof(errbuf)) < 0)
			{
				errbuf_ptr = strerror(AVUNERROR(nErr));
			}
			LOGD<< errbuf_ptr;
		}

		return AVERROR(kErrorOpenUrlError);
	}

	LOGI<<"open input takes time: "<< (av_gettime() - time_s) / 1000.0 << " ms";

	LOGD << "open url e";
	return kErrorNone;
}

int CSVPlayer::findStreamInfo()
{
	LOGD << "find stream info s";

	if (m_abort) {
		LOGE<<"find stream info check abort";
		return AVERROR(kErrorStreamInfoNotFound);
	}



	int64_t time_s = av_gettime();


	if (avformat_find_stream_info(m_fmt_ctx, NULL) < 0) {
		LOGE<<"avformat_find_stream_info failed";
		return AVERROR(kErrorStreamInfoNotFound);
	}
	LOGD<<"nb_streams: "<< m_fmt_ctx->nb_streams;

	m_duration = (float)(m_fmt_ctx->duration / 1000000LL);
	m_duration = (m_duration < 0.0) ? 0.0 : m_duration;
	LOGD << "video duration is = " << m_duration;

	if (m_fmt_ctx->pb) {
		m_fmt_ctx->pb->eof_reached = 0;
	}

	if (m_seek_by_bytes < 0) {
		m_seek_by_bytes = !!(m_fmt_ctx->iformat->flags & AVFMT_TS_DISCONT) &&
			strcmp("ogg", m_fmt_ctx->iformat->name);
	}

	LOGD << "find stream info e";
	return kErrorNone;
}

int CSVPlayer::openStreams()
{
	LOGD << "open sterams s nb_streams = " << m_fmt_ctx->nb_streams;

	int i = 0;
	int ret = kErrorNone;

	for (i = 0; i < m_fmt_ctx->nb_streams; ++i) {
		m_fmt_ctx->streams[i]->discard = AVDISCARD_ALL;
	}

	for (i = 0; i < m_fmt_ctx->nb_streams; ++i) {
		LOGD<<"codec_type:"<< m_fmt_ctx->streams[i]->codec->codec_type;
		if (AVMEDIA_TYPE_AUDIO == m_fmt_ctx->streams[i]->codec->codec_type &&
			m_audio_index < 0) {
			m_audio_index = i;
		}
		if (AVMEDIA_TYPE_VIDEO == m_fmt_ctx->streams[i]->codec->codec_type &&
			m_video_index < 0) {
			m_video_index = i;
		}
	}

	if (m_video_index >= 0) {
		if ((ret = stream_component_open(m_video_index))) {
			LOGE<<"open video stream failed";
			return ret;
		}
	}

	if (m_audio_index >= 0) {
		if ((ret = stream_component_open(m_audio_index))) {
			LOGE<<"open audio stream failed";
			//return ret;
			m_audio_index = -1;
		}
	}

	if (!m_video_ok) {
		LOGE << "could not open stream";
		return AVERROR(kErrorOpenStreamFailed);
	}

	if (m_audio_ok) {
		m_avsync_type = AV_SYNC_AUDIO_MASTER;
	}
	else {
		m_avsync_type = AV_SYNC_VIDEO_MASTER;
	}

	LOGD << "open streams e";
	return kErrorNone;
}

int CSVPlayer::stream_component_open(int stream_index)
{
	LOGD<<"stream_component_open s, index:  "<< stream_index;

	AVStream *stream = m_fmt_ctx->streams[stream_index];
	AVCodecContext *codec_ctx = stream->codec;
	AVCodec *codec = NULL;

	if (!(codec = avcodec_find_decoder(codec_ctx->codec_id))) {
		LOGE << "could not find decoder, name:  " << avcodec_get_name(codec_ctx->codec_id);
		return AVERROR(kErrorCodecNotFound);
	}

	codec_ctx->workaround_bugs = 1;
	codec_ctx->lowres = 0;
	if (codec_ctx->lowres > codec->max_lowres) {
		codec_ctx->lowres = codec->max_lowres;
	}

	/*codec_ctx->idct_algo = FF_IDCT_AUTO;
	codec_ctx->skip_frame = AVDISCARD_DEFAULT;
	codec_ctx->skip_idct = AVDISCARD_DEFAULT;
	codec_ctx->skip_loop_filter = AVDISCARD_DEFAULT;*/

	codec_ctx->idct_algo = FF_IDCT_AUTO;
	//codec_ctx->skip_frame = AVDISCARD_ALL;
	//codec_ctx->skip_idct = AVDISCARD_DEFAULT;
	codec_ctx->skip_loop_filter = AVDISCARD_ALL;
	codec_ctx->error_concealment = 3;

	if (codec_ctx->lowres) {
		codec_ctx->flags |= CODEC_FLAG_EMU_EDGE;
		}

		if (codec->capabilities & CODEC_CAP_DR1) {
		codec_ctx->flags |= CODEC_FLAG_EMU_EDGE;
		}

	if (avcodec_open2(codec_ctx, codec, NULL)) {
		LOGE << "unsupported codec";
		return AVERROR(kErrorCodecOpenedFailed);
	}

	switch (codec_ctx->codec_type) {
	case AVMEDIA_TYPE_AUDIO:
		m_audio_stream = stream;
		m_audio_ctx = codec_ctx;
		m_channels = codec_ctx->channels;
		m_channel_layout = av_get_default_channel_layout(codec_ctx->channels);
		m_sample_rate = codec_ctx->sample_rate;
		m_sample_fmt = AV_SAMPLE_FMT_S16;
		m_audiopktq.SetMaxCount(30);
		stream->discard = AVDISCARD_DEFAULT;
		m_audio_disable = false;
		m_audio_ok = true;
		LOGD << "audio ok  sample_rate: "<<m_sample_rate<<" channel_layout: "<< m_channel_layout<< " sample_fmt: "<< m_sample_fmt;
		break;
	case AVMEDIA_TYPE_VIDEO:
		m_video_stream = stream;
		m_video_ctx = codec_ctx;
		if (codec_ctx->width && codec_ctx->height) {
			m_width = codec_ctx->width;
			m_height = codec_ctx->height;
		}
		else {
			m_width = codec_ctx->coded_width;
			m_height = codec_ctx->coded_height;
		}
		if (!m_width || !m_height) {
				LOGE<<"parse video width and height failed";
				return AVERROR(kErrorCodecOpenedFailed);
		}
		
		m_frame_rate = (int)av_q2d(stream->r_frame_rate);
		m_videopktq.SetMaxCount(30);
		m_videoframeq.SetMaxCount(2);
		

		

		stream->discard = AVDISCARD_DEFAULT;
		m_video_disable = false;
		m_video_ok = true;
		LOGD << "video ok width "<< m_width<< " height "<< m_height;
		break;
	default:
		break;
	}

	LOGD<<"stream_component_open e, stream index: "<< stream_index;
	return kErrorNone;
}

void CSVPlayer::seekStreamTo(int64_t value, int64_t ref, int byByte)
{
	if (!m_seek_request) {
		m_seek_pos = value;
		m_seek_rel = ref;
		m_seek_flag &= ~AVSEEK_FLAG_BYTE;
		if (byByte) {
			m_seek_flag |= AVSEEK_FLAG_BYTE;
		}
		m_seek_request = true;
		m_is_seeking = true;
	}
}

void CSVPlayer::setState(PlayerState state, PlayerError errorn)
{
	m_state = state;
}

int CSVPlayer::playSleep(double actual_delay)
{
	actual_delay = actual_delay < 0 ? 0.0 : actual_delay;
	actual_delay = actual_delay > 10.0 ? 0.0 : actual_delay;
	if (m_fast_forward) {
		actual_delay = AV_SYNC_THRESHOLD_S;
	}
	double delay_step = 0.0;
	do {
		if (m_abort || !isPlaying()) {
			return -1;
		}
		if (m_is_seeking || m_paused || kStateBuffering == m_state) {
			return -2;
		}
		delay_step += AV_DELAY_STEP_S;
		if (delay_step >= actual_delay) {
			delay_step = actual_delay - (delay_step - AV_DELAY_STEP_S);
			delay_step = FFMAX(0, delay_step);
			usleep((int)(delay_step * 1000 * 1000));
			break;
		}
		usleep((int)(AV_DELAY_STEP_S * 1000 * 1000));
	} while (1);

	return 0;
}

void CSVPlayer::ffmpeglog_callback(void* avcl, int level, const char* pFormat, va_list vl)
{
	char buf[256] = { 0 };
	vsnprintf(buf, 255, pFormat, vl);
	LOGD << "ffmpegLog" << buf;
}

int CSVPlayer::play_interrupt_cb(void *arg)
{
	CSVPlayer *ins = (CSVPlayer *)arg;
	if (ins->m_abort) {
		return 1;
	}

	if (ins->m_read_time_us > 0) {
		if ((av_gettime() - ins->m_read_time_us) >= READ_TIMEOUT_LIMIT_US) {
			ins->m_timeout = true;
			LOGW << "play_interrupt_cb time out";
			return 1;
		}
	}

	return 0;
}

int CSVPlayer::SaveToFile(AVFrame *frame, const std::string &filename)
{
	AVOutputFormat* pof = av_oformat_next(NULL);
	while (pof != NULL)
	{
		LOGI << "out format = " << pof->name<<" extensions = "<<pof->extensions;
		pof = av_oformat_next(pof);
	}
	
	LOGI << "SaveToFile start";
	int ret = 0;
	//确保图像是AV_PIX_FMT_YUV420P
	AVFormatContext* pFormatCtx = NULL;
	AVOutputFormat* fmt = NULL;
	AVStream* video_st = NULL;
	AVCodecContext* pCodecCtx = NULL;
	AVCodec* pCodec = NULL;

	
	pFormatCtx = avformat_alloc_context();
	//Guess format  
	fmt = av_guess_format("mjpeg", NULL, NULL);
	if (fmt == NULL)
	{
		ret = -1;
		LOGE<<"av_guess_format fail.";
		return -1;
	}
	pFormatCtx->oformat = fmt;
	//Output URL  
	if (avio_open(&pFormatCtx->pb, filename.c_str(), AVIO_FLAG_READ_WRITE) < 0)
	{
		LOGE<<"Couldn't open output file.";
		return -1;
	}
	//Method 2. More simple  but will crash  when wirte_frame. don't konw why
	/*LOGI << "avformat_alloc_output_context2 start filename = " << filename;
	ret = avformat_alloc_output_context2(&pFormatCtx, NULL, "mjpeg", filename.c_str());
	if (ret< 0)
	{
	LOGE << "avformat_alloc_output_context2 failed";
	return -1;
	}*/
	//fmt = pFormatCtx->oformat;

	video_st = avformat_new_stream(pFormatCtx, pCodec);
	if (video_st == NULL)
	{
		return -1;
	}
	pCodecCtx = video_st->codec;
	pCodecCtx->codec_id = fmt->video_codec;
	pCodecCtx->codec_type = AVMEDIA_TYPE_VIDEO;
	pCodecCtx->pix_fmt = AV_PIX_FMT_YUVJ420P;

	pCodecCtx->width = frame->width;
	pCodecCtx->height = frame->height;

	pCodecCtx->time_base.num = 1;
	pCodecCtx->time_base.den = 25;
	//输出格式信息
	//av_dump_format(pFormatCtx, 0, filename.c_str(), 1);

	pCodec = avcodec_find_encoder(pCodecCtx->codec_id);
	if (!pCodec)
	{
		LOGE<<"cann't find image encoder！";
		return -1;
	}

	if (avcodec_open2(pCodecCtx, pCodec, NULL) < 0)
	{
		LOGE<<"cann't open encoder！";
		return -1;
	}


	int nRet = avformat_write_header(pFormatCtx, NULL);
	if (nRet < 0) {
		LOGE << "Error occurred when opening output file: ! av_err2str(nRet)";
		return -1;
	}

	AVPacket *pkt = av_packet_alloc();
	int pktsize = avpicture_get_size(pCodecCtx->pix_fmt, pCodecCtx->width, pCodecCtx->height);
	//av_new_packet(pkt, pktsize);

	int got_picture = 0;
	//

	ret = avcodec_encode_video2(pCodecCtx, pkt, frame, &got_picture);
	if (ret < 0)
	{
		LOGE<<"encode error！";
		return -1;
	}
	if (got_picture == 1)
	{
		pkt->stream_index = video_st->index;

		ret = av_write_frame(pFormatCtx, pkt);
		if (0 != ret) {
			LOGE << "av_write_frame failed. av_err2str(nRet)";
		}
	}
	av_free_packet(pkt);

	av_write_trailer(pFormatCtx);

	if (video_st)
	{
		avcodec_close(video_st->codec);
	}
	avio_close(pFormatCtx->pb);
	avformat_free_context(pFormatCtx);
	LOGI << "SaveToFile end";

	return 0;
}

double CSVPlayer::get_master_clock()
{
	double interval;
	double actual_interval;
	switch (m_avsync_type) {
	case AV_SYNC_AUDIO_MASTER:
		return m_audio_clock;
	case AV_SYNC_VIDEO_MASTER:
		return m_video_clock;
	case AV_SYNC_EXTERNAL_CLOCK:
	default:
		return m_audio_clock;
	}
}

double CSVPlayer::synchronize_video(AVFrame *src_frame, double pts)
{
	double frame_delay;
	if (0 != pts) {
		// if we have pts, set video clock to it
		m_video_clock = pts;
	}
	else {
		// if we aren't given a pts, set it to the clock
		pts = m_video_clock;
	}

	// update the video clock
	frame_delay = av_q2d(m_video_stream->codec->time_base);
	// if we are repeating a frame, adjust clock accordingly
	frame_delay += src_frame->repeat_pict * (frame_delay * 0.5);
	m_video_clock += frame_delay;
	return pts;
}

void CSVPlayer::StopWaitClean()
{
	LOGD << "StopWaitClean 1";
	m_audiopktq.SetPushEnable(false);
	m_videopktq.SetPushEnable(false);
	//m_videoframeq.SetEnable(false);
	m_audioDecodeThread.Stop();
	LOGD << "m_audioDecodeThread STOPED";
	m_videoDecodeThread.Stop();
	LOGD << "m_videoDecodeThread STOPED";
	m_videoRefreshThread.Stop();
	LOGD << "m_videoRefreshThread STOPED";
	LOGD << "StopWaitClean 2";
	setState(kStateNone, kErrorNone);
	if (m_audio_ok)
	{
		LOGD << "audio free";
		m_audiopktq.Clear();
		avcodec_close(m_audio_ctx);
		if (m_swr_ctx != nullptr)
		{
			swr_free(&m_swr_ctx);
			m_swr_ctx = nullptr;
		}
		m_audio_stream = nullptr;
		m_audio_index = -1;
		m_audio_ok = false;
	}

	if (m_video_ok)
	{
		LOGD << "video free";
		m_videopktq.Clear();
		m_videoframeq.Clear();

		avcodec_close(m_video_ctx);
		if (m_sws_ctx)
		{
			sws_freeContext(m_sws_ctx);
			m_sws_ctx = nullptr;
		}
	}

	if (m_fmt_ctx != nullptr)
	{
		avformat_close_input(&m_fmt_ctx);
		m_fmt_ctx = nullptr;
	}
}

void CSVPlayer::FFmpegInit()
{
	av_register_all();
	avcodec_register_all();
	avformat_network_init();
	av_init_packet(&flush_pkt);
	flush_pkt.data = (uint8_t*)"FLUSH";

	LOGI << "libpalyer version = "<< VERSION;

}

void CSVPlayer::ToggleFFmpegLog(bool enable)
{
	av_log_set_callback(enable?ffmpeglog_callback:NULL);
	av_log_set_level(enable?AV_LOG_DEBUG:AV_LOG_WARNING);
}

CSVPlayer::CSVPlayer()
	:m_readThread(this, &CSVPlayer::onRead, "readThread"),
	m_videoDecodeThread(this, &CSVPlayer::onVideoDecode, "videoDecodeThread"),
	m_audioDecodeThread(this, &CSVPlayer::onAudioDecode, "audioDecodeThread"),
	m_videoRefreshThread(this, &CSVPlayer::onVideoRender, "videoRenderThread")
{
	resetParam();
}

int CSVPlayer::startPlayer()
{
	if (isPlaying())
	{
		LOGW << "is in playing new url =  old url = " << m_url << m_url;
		return 0;
	}

	int error = kErrorNone;

	if (m_url.empty())
	{
		error = kErrorInvalidUrl;
		return error;
	}
	m_audiopktq.SetEnable(true);
	m_videopktq.SetEnable(true);
	m_videoframeq.SetEnable(true);
	m_readThread.Start();
	return kErrorNone;
}

int CSVPlayer::pausePlayer()
{
	if ( m_abort ||
		m_paused || m_state >= kStateReconnecting) {
		return 0;
	}
	m_paused = true;
	return 0;
}

int CSVPlayer::resumePlayer()
{
	if (m_abort ||
		m_state >= kStateReconnecting || !m_paused) {
		return 0;
	}
	m_paused = false;
	return 0;
}

int CSVPlayer::seekPlayer(int seconds)
{
	if (m_abort || m_state >= kStateReconnecting) {
		return 0;
	}
	seekStreamTo((int64_t)(seconds * AV_TIME_BASE), (int64_t)(seconds * AV_TIME_BASE), 0);
	return 0;
}

int CSVPlayer::stopPlayer()
{
	LOGD << "stopPlayer";
	if (m_abort) {
		return 0;
	}
	m_abort = true;
	m_readThread.Stop();
	return 0;
}

int CSVPlayer::resetPlayer()
{
	return 0;
}

int CSVPlayer::captureImage(const std::string& imagepath)
{
	m_NeedSaveToFile = true;
	m_CaptureFilename = imagepath;
	return 0;
}

int CSVPlayer::speedPlayer(SPEED_RATIO speedratio)
{
	return 0;
}

int CSVPlayer::muteAudio(bool ismute)
{
	m_muted = ismute;
	return 0;
}

