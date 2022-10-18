cmake_minimum_required(VERSION 3.6.1)

#include_directories(third_party)

include_directories(src/main/cpp/)

#添加名为name的库，库的源文件可指定，也可用target_sources()后续指定。
#库的类型是STATIC(静态库)/SHARED(动态库)/MODULE(模块库)之一。
add_library( native-lib
             SHARED
#
#             # main game files
             src/main/cpp/native-lib.cpp
             src/main/cpp/Audio.cpp
             src/main/cpp/audio/Mixer.h
#
#             # audio engine
             src/main/cpp/audio/AAssetDataSource.cpp
             src/main/cpp/audio/Player.cpp
#
#             # utility functions
             src/main/cpp/utils/logging.h
             src/main/cpp/utils/UtilityFunctions.cpp
#
             )

#设置TARGET_LIBS变量
set (TARGET_LIBS log android)

if(${USE_FFMPEG})

    MESSAGE("Using FFmpeg extractor")

    add_definitions(-DUSE_FFMPEG=1)
    target_sources( native-lib PRIVATE src/main/cpp/audio/FFMpegExtractor.cpp )

    # Add the local path to FFmpeg, you can use the ${ANDROID_ABI} variable to specify the ABI name
    # e.g. /Users/donturner/Code/ffmpeg/build/${ANDROID_ABI}
    set(FFMPEG_DIR "/path/to/ffmpeg")

    include_directories(native-lib ${FFMPEG_DIR}/include)

    add_library( avformat SHARED IMPORTED)
    set_target_properties(avformat PROPERTIES IMPORTED_LOCATION
            ${FFMPEG_DIR}/lib/libavformat.so)
    add_library( avutil SHARED IMPORTED)
    set_target_properties(avutil PROPERTIES IMPORTED_LOCATION
            ${FFMPEG_DIR}/lib/libavutil.so)
    add_library( avcodec SHARED IMPORTED)
    set_target_properties(avcodec PROPERTIES IMPORTED_LOCATION
            ${FFMPEG_DIR}/lib/libavcodec.so)
    add_library( swresample SHARED IMPORTED)
    set_target_properties(swresample PROPERTIES IMPORTED_LOCATION
            ${FFMPEG_DIR}/lib/libswresample.so)
    set (TARGET_LIBS ${TARGET_LIBS} avformat avutil avcodec swresample)

else()
    MESSAGE("Using NDK media extractor")
    MESSAGE(${TARGET_LIBS})
    add_definitions(-DUSE_FFMPEG=0)
    target_sources( native-lib PRIVATE src/main/cpp/audio/NDKExtractor.cpp )
    set (TARGET_LIBS ${TARGET_LIBS} mediandk)
endif()
MESSAGE(${TARGET_LIBS})

#jni引用so文件：https://juejin.cn/post/7066453944758861837
#此处不加绝对路径会报错，无法编译
set(APP_PATH /Users/jiangjiaojiao/Documents/android/android-projects/WhiteNoise/app)
add_library(oboe SHARED IMPORTED)
set_target_properties(oboe PROPERTIES IMPORTED_LOCATION
        ${APP_PATH}/libs/${ANDROID_ABI}/liboboe.so)
include_directories(native-lib ${APP_PATH}/src/main/head/include)
set (TARGET_LIBS ${TARGET_LIBS} oboe)

#todo 理解该命令
target_link_libraries( native-lib ${TARGET_LIBS} )

#todo 理解该命令
target_compile_options(native-lib
    PRIVATE -std=c++17 -Wall -Werror "$<$<CONFIG:RELEASE>:-Ofast>")
