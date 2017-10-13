#cmake ../  -DCMAKE_SYSTEM_NAME=Android -DCMAKE_SYSTEM_VERSION=21 -DCMAKE_ANDROID_ARCH_ABI=armeabi-v7a -DCMAKE_ANDROID_NDK=/home/vmuser/android-ndk-r12b/  -DCMAKE_ANDROID_STL_TYPE=gnustl_static
#rm -R CMake* cmake* Makefile

#export ANDROID_NDK=/home/vmuser/android-ndk-r12b/
#export ANDROID_NATIVE_API_LEVEL=15

#$ANDROID_NDK/build/tools/make-standalone-toolchain.sh --platform=android-15 --install-dir=/tmp/android-toolchain --ndk-dir=$ANDROID_NDK --toolchain=arm-linux-androideabi-4.9

#export PATH=$PATH:/tmp/android-toolchain/bin/
#export ANDROID_STANDALONE_TOOLCHAIN=/tmp/android-toolchain/
cmake ../ -DCMAKE_TOOLCHAIN_FILE=../android.toolchain.cmake -DANDROID_ABI="armeabi-v7a" -DANDROID_NATIVE_API_LEVEL=15 -DANDROID_NDK=$NDKROOT -DANDROID_STL=gnustl_static -DCMAKE_BUILD_TYPE=Release
