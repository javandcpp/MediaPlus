# Sets the minimum version of CMake required to build the native
# library. You should either keep the default value or only pass a
# value of 3.4.0 or lower.

cmake_minimum_required(VERSION 3.4.1)

# Creates and names a library, sets it as either STATIC
# or SHARED, and provides the relative paths to its source code.
# You can define multiple libraries, and CMake builds it for you.
# Gradle automatically packages shared libraries with your APK.


#包含头文件

set(include-headers ${CMAKE_SOURCE_DIR}/src/main/cpp/include)
include_directories(${include-headers})

set(include-sdl2headers ${CMAKE_SOURCE_DIR}/src/main/cpp/sdl2/include)
include_directories(${include-sdl2headers})

#添加依赖库
add_library(avcodec SHARED IMPORTED)
set_target_properties(avcodec
                        PROPERTIES
                         IMPORTED_LOCATION ${CMAKE_SOURCE_DIR}/src/main/jniLibs/armeabi-v7a/libavcodec.so)

#添加依赖库

#添加依赖库
 add_library(avfilter SHARED IMPORTED)
 set_target_properties( avfilter
                        PROPERTIES
                        IMPORTED_LOCATION ${CMAKE_SOURCE_DIR}/src/main/jniLibs/armeabi-v7a/libavfilter.so )
#添加依赖库
 add_library(avformat SHARED IMPORTED)
 set_target_properties( avformat
                        PROPERTIES
                        IMPORTED_LOCATION ${CMAKE_SOURCE_DIR}/src/main/jniLibs/armeabi-v7a/libavformat.so )
#添加依赖库
 add_library(avutil SHARED IMPORTED)
 set_target_properties( avutil
                        PROPERTIES
                        IMPORTED_LOCATION ${CMAKE_SOURCE_DIR}/src/main/jniLibs/armeabi-v7a/libavutil.so )
#添加依赖库

#添加依赖库
 add_library(swresample SHARED IMPORTED)
 set_target_properties( swresample
                        PROPERTIES
                        IMPORTED_LOCATION ${CMAKE_SOURCE_DIR}/src/main/jniLibs/armeabi-v7a/libswresample.so )

#添加依赖库
 add_library(swscale SHARED IMPORTED)
 set_target_properties( swscale
                        PROPERTIES
                        IMPORTED_LOCATION ${CMAKE_SOURCE_DIR}/src/main/jniLibs/armeabi-v7a/libswscale.so )

add_library(fdkaac SHARED IMPORTED)
set_target_properties( fdkaac
                       PROPERTIES
                       IMPORTED_LOCATION ${CMAKE_SOURCE_DIR}/src/main/jniLibs/armeabi-v7a/libfdk-aac.so )

add_library(avpostproc SHARED IMPORTED)
set_target_properties( avpostproc
                       PROPERTIES
                       IMPORTED_LOCATION ${CMAKE_SOURCE_DIR}/src/main/jniLibs/armeabi-v7a/libpostproc.so )

add_library(avdevice SHARED IMPORTED)
set_target_properties( avdevice
                       PROPERTIES
                       IMPORTED_LOCATION ${CMAKE_SOURCE_DIR}/src/main/jniLibs/armeabi-v7a/libavdevice.so )


add_library( # Sets the name of the library.
             Media
             # Sets the library as a shared library.
             SHARED

             # Provides a relative path to your source file(s).
             # Associated headers in the same location as their source
             # file are automatically included.
             src/main/cpp/native-lib.cpp
             src/main/cpp/jni_encode_h264.cpp
             src/main/cpp/debug.cpp
             src/main/cpp/jni_ffmpeg_config.cpp
             src/main/cpp/jni_ffmpeg_bridge.cpp
             src/main/cpp/jni_encode_aac.cpp
             src/main/cpp/jni_handler.cpp
             src/main/cpp/threadsafe_queue.cpp
             src/main/cpp/NativeCrashHandler.cpp
             src/main/cpp/jni_push_stream.c
             src/main/cpp/jni_pull_stream.cpp
             src/main/cpp/jni_trans_stream.cpp
             src/main/cpp/jni_transstream.cpp
             )

# Searches for a specified prebuilt library and stores the path as a
# variable. Because system libraries are included in the search path by
# default, you only need to specify the name of the public NDK library
# you want to add. CMake verifies that the library exists before
# completing its build.

find_library( # Sets the name of the path variable.
              log-lib

              # Specifies the name of the NDK library that
              # you want CMake to locate.
              log )

# Specifies libraries CMake should link to your target library. You
# can link multiple libraries, such as libraries you define in the
# build script, prebuilt third-party libraries, or system libraries.


target_link_libraries( # Specifies the target library.
                       Media

                       # Links the target library to the log library
                       # included in the NDK.
                       ${log-lib}
                        avcodec
                        avfilter
                        avformat
                        avutil
                        swresample
                        swscale
                        fdkaac
                        avdevice
                        avpostproc
                        log
                        android   #这句很重要,涉及android中库的调用
                        )
