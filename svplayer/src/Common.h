#ifndef COMMON_H_
#define COMMON_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <pthread.h>
#include<unistd.h>
#include <jni.h>

extern "C"
{
#include "libavformat/avformat.h"
#include "libavcodec/avcodec.h"
#include "libswscale/swscale.h"
#include "libswresample/swresample.h"
#include"libavutil/time.h"
}

#define VERSION		"1.2"
#define READ_TIMEOUT_LIMIT_US          (5 * 1000 * 1000)
#define BITRATE_CALC_INTERVAL_US       (10 * 1000 * 1000)
#define TIMER_INTERVAL_US              (300 * 1000)

#define AV_SYNC_THRESHOLD_S            (0.010)
#define AV_NOSYNC_THRESHOLD_S          (100000.00)
#define AV_DROP_THRESHOLD_S            (3.000)
#define AUDIO_SYNC_THRESHOLD_S         (1.000)
#define AV_DELAY_STEP_S                (0.050)

#define AVCODEC_MAX_AUDIO_FRAME_SIZE   (192000 * 2)  // 2 second of 48khz 32bit audio

#define SAVE_YUV_FILE 0

#define PATH_SIZE                      (256)

#define MAX_QUEUE_SIZE                 (15 * 1024 * 1024)
#define MIN_FRAMES                     (4)
#define LIVE_VIDEO_PACKET_LIMIT        (25)
#define LIVE_AUDIO_PACKET_LIMIT        (50)

enum {
    AV_SYNC_AUDIO_MASTER = 0,
    AV_SYNC_VIDEO_MASTER,
    AV_SYNC_EXTERNAL_CLOCK,
};

typedef enum {
    kShowModeAspectFit = 0,
    kShowModeAspectFill,
    kShowModeFill,
	kShowModeAspectAuto,
} ShowMode;
enum RotationMode
{
	kRotate0 = 0,  // No rotation.
	kRotate90 = 90,  // Rotate 90 degrees clockwise.
	kRotate180 = 180,  // Rotate 180 degrees.
	kRotate270 = 270,  // Rotate 270 degrees clockwise.
};
typedef enum {
    kErrorNone = 0,
    kErrorInvalidUrl,
    kErrorUnsupportedProtocol,
    kErrorOpenUrlError,
    kErrorStreamsNotAvailable,
    kErrorStreamInfoNotFound,
    kErrorOpenStreamFailed,
    kErrorCodecNotFound,
    kErrorCodecOpenedFailed,
    kErrorStreamReadError,
    kErrorMemoryOut,
    kErrorNullPointer,
    kErrorCreateThreadError,
    kErrorUnknown,
	kErrorRecordError,
} PlayerError;

typedef enum {
    kStateNone = 0,
    kStateConnecting,
    kStateConnected,
    kStateBuffering,
    kStateReadyToPlay,
    kStatePlaying,
    kStatePaused,
    kStateSeekFinished,
    kStateSeekFailed,
    kStateTimeout,
    kStateReconnecting,
    kStateConnectionFailed,
    kStateStoppedWithError,
    kStateStoppedWithEOF,
    kStateStoppedByUser,
	kStateRecording,
	kStateRecordFinished,
	kStateRecordError,
} PlayerState;

//²¥·ÅÊÂ¼þ
enum PlayEvent
{
	PE_None = 0,
	PE_Opening,
	PE_Opened,
	PE_OpenedError,
	PE_Buffering,
	PE_ReadyToPlay,
	PE_Playing,
	PE_Record_Fail,
	PE_Recording,
	PE_Snapshot_Sus,
	PE_Snapshot_Fail,
	PE_PlayError,
	PE_PlaySeekFailed,
	PE_SeekFinished,
	PE_StopWithEof
};

enum SPEED_RATIO
{
	SPEED1,
	SPEED2,
	SPEED3,
	SPEED4
};

#endif /* COMMON_H_ */
