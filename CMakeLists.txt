project(4x4K_FBO)

cmake_minimum_required(VERSION 2.8)
SET( 3DPARTY_DIR "${CMAKE_SOURCE_DIR}/3dparty" )
SET( CMAKE_AUTOMOC ON )

include_directories( "src" )

# OpenGL
find_package(OpenGL REQUIRED)
#set(VL_OPENGL_LIBRARIES ${OPENGL_LIBRARIES})

# Qt5
find_package(Qt5Core REQUIRED)
find_package(Qt5OpenGL REQUIRED)
find_package(Qt5Widgets REQUIRED)
include_directories(${Qt5Core_INCLUDE_DIRS})
include_directories(${Qt5OpenGL_INCLUDE_DIRS})
include_directories(${Qt5Widgets_INCLUDE_DIRS})

#VL
set( CMAKE_MODULE_PATH "${3DPARTY_DIR}/VL/install/cmake" )
set(VL_ROOT "${3DPARTY_DIR}/VL/install")
find_package(VL COMPONENTS VLCore VLGraphics VLQt5 ZLib FreeType JPG PNG TIFF REQUIRED)
include_directories(${VL_INCLUDE_DIRS})

set(QT_LIBRARIES
        ${Qt5Core_LIBRARIES}
        ${Qt5Core_QTMAIN_LIBRARIES}
        ${Qt5OpenGL_LIBRARIES}
        ${Qt5Widgets_LIBRARIES}
)

SET( CMAKE_INCLUDE_CURRENT_DIR ON )
SET( CMAKE_INCLUDE_DIRECTORIES_PROJECT_BEFORE ON )

################################################################################
# Subdirectories
################################################################################

add_subdirectory("src")
