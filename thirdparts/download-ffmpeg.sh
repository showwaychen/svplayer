
set -e

LOCAL_PATH=`pwd`
FFMPEGPATH=$LOCAL_PATH/FFmpeg-n3.3.4
if [ ! -d "$FFMPEGPATH" ]; then
wget -O FFmpeg-n3.3.4.zip https://codeload.github.com/FFmpeg/FFmpeg/zip/n3.3.4
unzip FFmpeg-n3.3.4.zip
ln -s FFmpeg-n3.3.4 ffmpeg
fi
