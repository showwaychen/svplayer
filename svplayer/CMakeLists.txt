cmake_minimum_required(VERSION 2.6)

#set(CMAKE_SYSTEM_NAME Android)
#set(CMAKE_SYSTEM_VERSION 21) # API level
##set(CMAKE_ANDROID_ARCH_ABI arm64-v8a)
#set(CMAKE_ANDROID_NDK /path/to/android-ndk)
#set(CMAKE_ANDROID_STL_TYPE gnustl_static)
project (SVPLAYER)



#defined										 
set(CMAKE_CXX_FLAGS "-Wall -std=c++11 -D__STDC_CONSTANT_MACROS -D__STDC_LIMIT_MACROS -funwind-tables  -Wno-unused-label -Wno-maybe-uninitialized -Wno-unused-but-set-variable -Wno-deprecated-declarations -Wno-comment -Wno-unused-variable -Wno-sign-compare")
if(${CMAKE_BUILD_TYPE} MATCHES "Release")
    set(CMAKE_CXX_FLAGS " ${CMAKE_CXX_FLAGS} -O3")
else()
    set(CMAKE_CXX_FLAGS " ${CMAKE_CXX_FLAGS} -g3")
endif()
set(CMAKE_C_FLAGS "-D__STDC_CONSTANT_MACROS")
ADD_DEFINITIONS( -DWEBRTC_ANDROID -DWEBRTC_POSIX)
#includes			
set(ALL_LIB_PATH  ${CMAKE_CURRENT_SOURCE_DIR}/../thirdparts/build/${ANDROID_ABI})	

set(BASELIBROOT_INCLUDEDIR ${CMAKE_CURRENT_SOURCE_DIR}/../thirdparts/baselib)
set(BASELIB_INCLUDEDIR ${BASELIBROOT_INCLUDEDIR}/webrtc)
set(BASELIB_PATH ${CMAKE_CURRENT_SOURCE_DIR}/../thirdparts/baselib/android/lib/)
set(FFMPEG_INCLUDEDIR ${CMAKE_CURRENT_SOURCE_DIR}/../thirdparts/ffmpeg)
#set(LEAKTRACER_INCLUDEDIR ${CMAKE_CURRENT_SOURCE_DIR}/../thirdparts/libleaktracer/include)
include_directories("${FFMPEG_INCLUDEDIR}" "${BASELIB_INCLUDEDIR}" "${BASELIBROOT_INCLUDEDIR}" )			

#library	
set(FFMPEGLIB ${ALL_LIB_PATH}/lib/libavformat.a 
				${ALL_LIB_PATH}/lib/libavcodec.a 
				${ALL_LIB_PATH}/lib/libavutil.a 
				${ALL_LIB_PATH}/lib/libswresample.a 
				${ALL_LIB_PATH}/lib/libswscale.a)
set(BASELIB_LIB  ${BASELIB_PATH}/libbaselib.a)	
#set(LEAKTRACER_LIB  ${CMAKE_CURRENT_SOURCE_DIR}/../thirdparts/libleaktracer/android/lib/libleaktracer.a)		
set (STATICLIBS ${FFMPEGLIB} ${BASELIB_LIB} )
#project build
file(GLOB  PLAYER_SOURCES_FILES 		${CMAKE_CURRENT_SOURCE_DIR}/src/*.h
										 ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp
										 ${CMAKE_CURRENT_SOURCE_DIR}/android/*.cpp 
										 ${CMAKE_CURRENT_SOURCE_DIR}/android/*.h
										 )
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/libs/${ANDROID_ABI})										 
add_library( 
             svplayer
             SHARED
             ${PLAYER_SOURCES_FILES} )
find_library( # Sets the name of the path variable.
              log-lib
              # Specifies the name of the NDK library that
              # you want CMake to locate.
              log )
find_library( 
              jnigraphics-lib
              jnigraphics )
find_library( 
              GLESv1_CM-lib
              GLESv1_CM )	
find_library( 
              GLESv2-lib
              GLESv2 )			  
target_link_libraries( 
                       svplayer
                       ${log-lib} ${jnigraphics-lib} ${GLESv1_CM-lib} ${GLESv2-lib} ${STATICLIBS})
if(${CMAKE_BUILD_TYPE} MATCHES "Release")
    ADD_CUSTOM_COMMAND(TARGET svplayer
    POST_BUILD
    COMMAND   ${CMAKE_STRIP} ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/*.so )		
endif()			
			   
#install(TARGETS svplayer
#                LIBRARY DESTINATION lib)					
