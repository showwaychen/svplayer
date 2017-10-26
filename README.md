### svplayer

#  a android video player
### My Build Environment
- Android
 - [NDK r12b]
 - Android Studio 2.3.1
 - Gradle 2.2.3
### Features
    cpu: ARMv7a
    video-output:  OpenGL ES 2.0
	audio-output: AudioTrack
###  Build
    export NDKROOT=  your ndk root path
	
    cd thirdparts
    ./download-baselib.sh
    ./download-ffmpeg.sh
    ./build-ffmpeg.sh
    ./build-baselib.sh

   
    cd svplayer/build
    ./build.sh
    make -j4

    output directory is svplayer/libs/armeabi-v7a
