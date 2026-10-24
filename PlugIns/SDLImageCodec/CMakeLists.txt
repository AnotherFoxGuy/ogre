#-------------------------------------------------------------------
# This file is part of the CMake build system for OGRE
#     (Object-oriented Graphics Rendering Engine)
# For the latest info, see http://www.ogre3d.org/
#
# The contents of this file are placed in the public domain. Feel
# free to make use of it in any way you like.
#-------------------------------------------------------------------

file(GLOB HEADER_FILES "${CMAKE_CURRENT_SOURCE_DIR}/include/*.h")
list(APPEND HEADER_FILES ${PROJECT_BINARY_DIR}/include/OgreSDLImageCodecExports.h)
set(SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/src/OgreSDLImageCodec.cpp")

if(UNIX)
  set_source_files_properties(src/OgreSDLImageCodec.cpp PROPERTIES  
    COMPILE_FLAGS "-Wno-cast-qual -Wno-unused-function -Wno-missing-declarations -Wno-type-limits")
endif()

add_library(Codec_SDLImage ${OGRE_LIB_TYPE} ${HEADER_FILES} ${SOURCES})
target_link_libraries(Codec_SDLImage PUBLIC OgreMain)

target_include_directories(Codec_SDLImage PUBLIC 
  "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>"
  $<INSTALL_INTERFACE:include/OGRE/Plugins/STBICodec>)

if(CMAKE_COMPILER_IS_GNUCXX)
  # workaround gcc5 bug on Ubuntu 16.04 regarding __builtin_cpu_supports
  target_link_libraries(Codec_SDLImage PRIVATE gcc)
endif()

if(ZLIB_FOUND)
  target_compile_definitions(Codec_SDLImage PRIVATE HAVE_ZLIB)
  target_link_libraries(Codec_SDLImage PRIVATE ZLIB::ZLIB)
endif()
target_link_libraries(Codec_SDLImage PRIVATE SDL2_image::SDL2_image)

ogre_config_framework(Codec_SDLImage)
ogre_config_plugin(Codec_SDLImage)
generate_export_header(Codec_SDLImage 
    EXPORT_MACRO_NAME _OgreSDLImageCodecExport
    EXPORT_FILE_NAME ${PROJECT_BINARY_DIR}/include/OgreSDLImageCodecExports.h)
install(FILES ${HEADER_FILES} DESTINATION include/OGRE/Plugins/STBICodec)
