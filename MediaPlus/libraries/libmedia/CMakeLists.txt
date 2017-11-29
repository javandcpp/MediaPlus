# Sets the minimum version of CMake required to build the native
# library. You should either keep the default value or only pass a
# value of 3.4.0 or lower.

cmake_minimum_required(VERSION 3.4.1)

# Creates and names a library, sets it as either STATIC
# or SHARED, and provides the relative paths to its source code.
# You can define multiple libraries, and CMake builds it for you.
# Gradle automatically packages shared libraries with your APK.


#包含头文件

set(include-opencvheaders ${CMAKE_SOURCE_DIR}/src/main/cpp/opencv)
include_directories(${include-opencvheaders})

set(include-yuvheaders ${CMAKE_SOURCE_DIR}/src/main/cpp/yuv)
include_directories(${include-yuvheaders})

set(include-ffmpegheaders ${CMAKE_SOURCE_DIR}/src/main/cpp/ffmpeg)
include_directories(${include-ffmpegheaders})

set(include-sdl2headers ${CMAKE_SOURCE_DIR}/src/main/cpp/sdl2/ffmpeg/sdl2)
include_directories(${include-sdl2headers})

#set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/libs/${ANDROID_ABI})


add_library(libyuv SHARED IMPORTED)
set_target_properties(libyuv
                      PROPERTIES
                      IMPORTED_LOCATION ${CMAKE_SOURCE_DIR}/src/main/jniLibs/armeabi-v7a/libyuv.so)


add_library(ffmpeg SHARED IMPORTED)
set_target_properties(ffmpeg
                      PROPERTIES
                      IMPORTED_LOCATION ${CMAKE_SOURCE_DIR}/src/main/jniLibs/armeabi-v7a/libffmpeg.so)



file(GLOB native_core_srcs "src/main/cpp/core/*.cpp")
file(GLOB native_jni_srcs "src/main/cpp/jni/*.cpp")
file(GLOB native_event_srcs "src/main/cpp/event/*.cpp")

add_library( # Sets the name of the library.
             Media
             # Sets the library as a shared library.
             SHARED

             # Provides a relative path to your source file(s).
             # Associated headers in the same location as their source
             # file are automatically included.
             ${native_jni_srcs}
             ${native_core_srcs}
             ${native_event_srcs}
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


#target_link_libraries(Media  ${log-lib} avcodec avfilter avformat avutil swresample swscale avdevice avpostproc yuv log android)
target_link_libraries(Media  ${log-lib} libyuv ffmpeg log android)
