#!/bin/bash

NDK=/Applications/android-ndk-r13
ANDROID_API=android-14

PREFIX=$(pwd)/android/arm

SYSROOT=$NDK/platforms/$ANDROID_API/arch-arm

TARGET_HOST=armv7a-linux-androideabi
YSROOT_ARCH_DIRNAME=arch-arm
OPTIMIZE_OPTION="-O3 -DANDROID -march=armv7-a -mfloat-abi=softfp -mfpu=neon"
TOOLCHAIN_PREFIX=${NDK}/toolchains/arm-linux-androideabi-4.9/prebuilt/darwin-x86_64/bin/arm-linux-androideabi-
    
CPU=arm-v7a


export CFLAGS="${OPTIMIZE_OPTION}"
export LDFLAGS="-Wl,-dynamic-linker=/system/bin/linker"
export CC="${TOOLCHAIN_PREFIX}gcc --sysroot=$SYSROOT"
export CXX="${TOOLCHAIN_PREFIX}g++ --sysroot=$SYSROOT"
export STRIP="${TOOLCHAIN_PREFIX}strip"
export RANLIB="${TOOLCHAIN_PREFIX}ranlib"
export AR="${TOOLCHAIN_PREFIX}ar"


./configure --prefix=$PREFIX \
  --host=$TARGET_HOST \
  --with-sysroot=$SYSROOT \
  --enable-static=yes --enable-shared=yes
  

$ADDITIONAL_CONFIGURE_FLAG
make clean
make -j16
make install
