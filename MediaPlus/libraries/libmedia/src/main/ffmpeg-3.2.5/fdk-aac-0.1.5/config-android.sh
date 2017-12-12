#!/bin/bash
#use this bash-script to build libfdk-aac library.


NDK_ROOT_PATH=/Applications/android-ndk-r13
TARGET_HOST=armv7a-linux-androideabi
SYSROOT_ARCH_DIRNAME=arch-arm
OPTIMIZE_OPTION="-O3 -DANDROID -march=armv7-a -mfloat-abi=softfp -mfpu=neon"
TOOLCHAIN_PREFIX=${NDK_ROOT_PATH}/toolchains/arm-linux-androideabi-${TOOLCHAIN_VERSION}/prebuilt/darwin-x86_64/bin/arm-linux-androideabi-
  
  
SYSROOT=${NDK_ROOT_PATH}/platforms/${LIBMEDIA_TARGET_PLATFORM}/${SYSROOT_ARCH_DIRNAME}
PREFIX=${PWD}/../out/${LIBMEDIA_TARGET_ARCH_ABI}

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
  --enable-static=yes --enable-shared=no


make clean
make
make install
