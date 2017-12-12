#!/bin/bash

LIBMEDIA_TARGET_PLATFORM=android-14
NDK_ROOT_PATH=/Applications/android-ndk-r13
TOOLCHAIN_VERSION=4.9
FFMPEG_TMPDIR=/Users/loneswordman/Desktop/ffmpegtemp

export LIBMEDIA_TARGET_PLATFORM LIBMEDIA_TARGET_ARCH_ABI FFMPEG_TMPDIR NDK_ROOT_PATH TOOLCHAIN_VERSION
LIBMEDIA_TARGET_ARCH_ABI=armeabi-v7a #armeabi



build_aac(){
  cd fdk-aac-0.1.5 && ./config-android.sh && cd ..
}

build_x264(){
	cd libx264 && ./config-android.sh && cd ..
}
build_ffmpeg(){
	cd ffmpeg-3.2.5 && ./config-android.sh && cd ..
}


build_module(){
   cp out/Android.mk  out/${LIBMEDIA_TARGET_ARCH_ABI}/
   ndk-build clean && ndk-build all
}


build_aac

build_x264

build_ffmpeg

build_module