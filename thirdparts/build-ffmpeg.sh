#!/bin/bash

FF_ARCH="all32"

if [ $# -lt 1 ]; then
	echo "-->not specify a architecture, use 'all32' as default"
elif [ $# -gt 1 ]; then
	echo "-->too many architectures arguments, architecture must be one of 'armv5|armv7a|arm32|arm64|arm|x86|x86_64|mips|mips64|all32|all64|all'"
	exit 1
else
	FF_ARCH=$1
	echo "-->specify architecture : $FF_ARCH"
fi

if [ x$NDKROOT = "x" ]
then 
    echo ERROR:NDKROOT env variable is not set
	exit 1;
fi
# Environment
LOCAL_PATH=`pwd`
NDK=$NDKROOT
NDK64=$NDKROOT
ANDROID_PLATFORM=android-15
GCC_VER=4.9
GCC64_VER=4.9
CROSS_PREFIX=
CFG_FLAGS=
EXTRA_CFLAGS=
EXTRA_LDFLAGS=
BUILD_PATH=$LOCAL_PATH/build
PLATFORM=
PREBUILT=
PREFIX=

CFLAGS="-O3 -Wall -pipe \
    -std=c99 \
    -ffast-math \
    -fstrict-aliasing -Werror=strict-aliasing \
    -Wno-psabi -Wa,--noexecstack \
    -DANDROID -DNDEBUG"
	
echo "-->local path : $LOCAL_PATH"
echo "-->NDK path : $NDK"

if [[ $FF_ARCH = all* ]]; then
	echo "-->remove build path and recreate it"
	rm -rf $BUILD_PATH
	mkdir -p $BUILD_PATH
fi

function show_options ()
{
echo "****************** show options begin ******************"
echo "-->Android platform : $ANDROID_PLATFORM"
echo "-->GCC version : $GCC_VER"
echo "-->Cross prefix : $CROSS_PREFIX"
echo "-->CFG flags : $CFG_FLAGS"
echo "-->Extra cflags : $EXTRA_CFLAGS"
echo "-->Extra ldflags : $EXTRA_LDFLAGS"
echo "-->Platform : $PLATFORM"
echo "-->Prebuild : $PREBUILT"
echo "-->Build prefix : $PREFIX"
echo "****************** show options end ******************"
}
	
function build ()
{
show_options
cd ffmpeg
chmod +x configure *.sh
make clean
./configure \
${CFG_FLAGS} \
--target-os=linux \
--prefix=$PREFIX \
--enable-version3 \
--enable-gpl \
--disable-debug \
--enable-asm \
--enable-inline-asm \
--enable-optimizations \
--disable-shared \
--enable-static \
\
--disable-programs \
--disable-doc \
--disable-avdevice \
--disable-postproc \
--disable-avfilter \
--disable-w32threads \
--disable-avdevice \
--enable-pthreads \
--enable-network \
\
--disable-everything \
--enable-decoder=h264 \
--enable-decoder=mpeg4 \
--enable-decoder=aac \
\
--disable-demuxers \
--enable-demuxer=flv \
--enable-demuxer=mov \
\
--disable-muxers \
--enable-muxer=flv \
 \
--enable-protocol=rtmp \
--enable-protocol=http \
--enable-protocol=file \
\
--disable-zlib \
--disable-bzlib \
\
--cc=$PREBUILT/bin/${CROSS_PREFIX}-gcc \
--cross-prefix=$PREBUILT/bin/${CROSS_PREFIX}- \
--nm=$PREBUILT/bin/${CROSS_PREFIX}-nm \
--sysroot=$PLATFORM \
--extra-cflags="${CFLAGS} ${EXTRA_CFLAGS}" \
--extra-ldflags="${EXTRA_LDFLAGS}"

make -j4 install
cd -
}

# 1
function build_armv5 ()
{
echo "build armv5"
FF_ARCH=arm
CROSS_PREFIX=arm-linux-androideabi
ANDROID_PLATFORM=android-15
PLATFORM=$NDK/platforms/${ANDROID_PLATFORM}/arch-${FF_ARCH}
PREBUILT=$NDK/toolchains/${CROSS_PREFIX}-${GCC_VER}/prebuilt/linux-x86_64
CFG_FLAGS=" --arch=arm"
EXTRA_CFLAGS=
EXTRA_LDFLAGS=
PREFIX=$BUILD_PATH/armeabi
build
echo "build armv5 end"
}

# 2
function build_armv7a ()
{
echo "build armv7a"
FF_ARCH=arm
CROSS_PREFIX=arm-linux-androideabi
ANDROID_PLATFORM=android-15
PLATFORM=$NDK/platforms/${ANDROID_PLATFORM}/arch-${FF_ARCH}
PREBUILT=$NDK/toolchains/${CROSS_PREFIX}-${GCC_VER}/prebuilt/linux-x86_64
CFG_FLAGS=" --arch=arm --cpu=cortex-a8 --enable-neon --enable-thumb --disable-armv5te --disable-armv6 --disable-armv6t2"
EXTRA_CFLAGS=" -march=armv7-a -mcpu=cortex-a8 -mfpu=vfpv3-d16 -mfloat-abi=softfp -mthumb"
EXTRA_LDFLAGS=" -Wl,--fix-cortex-a8"
PREFIX=$BUILD_PATH/armeabi-v7a
build
echo "build armv7a end"
}

# 3
function build_arm64-v8a ()
{
echo "build arm64-v8a"
FF_ARCH=arm64
CROSS_PREFIX=aarch64-linux-android
CFG_FLAGS=" --arch=aarch64 --enable-yasm"
ANDROID_PLATFORM=android-21
EXTRA_CFLAGS=
EXTRA_LDFLAGS=
PLATFORM=$NDK64/platforms/${ANDROID_PLATFORM}/arch-${FF_ARCH}
PREBUILT=$NDK64/toolchains/${CROSS_PREFIX}-${GCC64_VER}/prebuilt/linux-x86_64
PREFIX=$BUILD_PATH/arm64-v8a
build
echo "build arm64-v8a end"
}

# 4
function build_arm32 ()
{
build_armv5
build_armv7a
}

# 5
function build_arm64 ()
{
build_arm64-v8a
}

# 6
function build_arm ()
{
build_arm32
build_arm64
}

# 7
function build_x86 ()
{
echo "build x86"
FF_ARCH=x86
CROSS_PREFIX=i686-linux-android
ANDROID_PLATFORM=android-15
CFG_FLAGS=" --arch=x86 --cpu=i686 --enable-yasm"
EXTRA_CFLAGS=" -march=atom -msse3 -ffast-math -mfpmath=sse"
EXTRA_LDFLAGS=
PLATFORM=$NDK/platforms/${ANDROID_PLATFORM}/arch-${FF_ARCH}
PREBUILT=$NDK/toolchains/${FF_ARCH}-${GCC_VER}/prebuilt/linux-x86_64
PREFIX=$BUILD_PATH/x86
build
echo "build x86 end"
}

# 8
function build_x86_64 ()
{
echo "build x86_64"
FF_ARCH=x86_64
CROSS_PREFIX=x86_64-linux-android
ANDROID_PLATFORM=android-21
CFG_FLAGS=" --arch=x86_64 --enable-yasm"
EXTRA_CFLAGS=" -march=atom -msse3 -ffast-math -mfpmath=sse"
EXTRA_LDFLAGS=
PLATFORM=$NDK64/platforms/${ANDROID_PLATFORM}/arch-${FF_ARCH}
PREBUILT=$NDK64/toolchains/${FF_ARCH}-${GCC64_VER}/prebuilt/linux-x86_64
PREFIX=$BUILD_PATH/x86_64
build
echo "build x86_64 end"
}

# 9
function build_mips ()
{
echo "build mips"
FF_ARCH=mips
CROSS_PREFIX=mipsel-linux-android
ANDROID_PLATFORM=android-15
CFG_FLAGS=" --arch=mips --enable-yasm"
EXTRA_CFLAGS=
EXTRA_LDFLAGS=" -lm"
PLATFORM=$NDK/platforms/${ANDROID_PLATFORM}/arch-${FF_ARCH}
PREBUILT=$NDK/toolchains/${CROSS_PREFIX}-${GCC_VER}/prebuilt/linux-x86_64
PREFIX=$BUILD_PATH/mips
build
echo "build mips end"
}

# 10
function build_mips64 ()
{
echo "build mips64"
FF_ARCH=mips64
CROSS_PREFIX=mips64el-linux-android
ANDROID_PLATFORM=android-21
CFG_FLAGS=" --arch=mips64 --enable-yasm"
EXTRA_CFLAGS=
EXTRA_LDFLAGS=" -lm"
PLATFORM=$NDK64/platforms/${ANDROID_PLATFORM}/arch-${FF_ARCH}
PREBUILT=$NDK64/toolchains/${CROSS_PREFIX}-${GCC64_VER}/prebuilt/linux-x86_64
PREFIX=$BUILD_PATH/mips64
build
echo "build mips64 end"
}

# 11
function build_all32 ()
{
build_arm32
build_x86
build_mips
}

# 12
function build_all64 ()
{
build_arm64
build_x86_64
build_mips64
}

# 13
function build_all ()
{
build_all32
build_all64
}

case $FF_ARCH in
	armv5)
	echo "****** build armv5 ******"
	build_armv5
	;;
	armv7a)
	echo "****** build armv7a ******"
	build_armv7a
	;;
	arm32)
	echo "****** build armv5 armv7a ******"
	build_arm32
	;;
	arm64)
	echo "****** build arm64-v8a ******"
	build_arm64
	;;
	arm)
	echo "****** build armv5 armv7a arm64-v8a ******"
	build_arm
	;;
	x86)
	echo "****** build x86 ******"
	build_x86
	;;
	x86_64)
	echo "****** build x86_64 ******"
	build_x86_64
	;;
	mips)
	echo "****** build mips ******"
	build_mips
	;;
	mips64)
	echo "****** build mips64 ******"
	build_mips64
	;;
	all32)
	echo "****** build all32(armv5 armv7a x86 mips) ******"
	build_all32
	;;
	all64)
	echo "****** build all64(arm74-v8a x86_64 mips64) ******"
	build_all64
	;;
	all)
	echo "****** build all(armv5 armv7a arm64-v8a x86 x86_64 mips mips64) ******"
	build_all
	;;
	*)
	echo "-->architecture:$FF_ARCH is not supported, architecture must be one of 'armv5|armv7a|arm32|arm64|arm|x86|x86_64|mips|mips64|all32|all64|all'"
	;;
esac
