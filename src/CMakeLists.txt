set(
    4x4K_FBO_HPP_SRCS
    rtt.hpp
)

set(
    4x4K_FBO_HEADERS
#    mainwindow.h
)

set(
    4x4K_FBO_SRCS
    main.cpp
#    mainwindow.cpp
)

#QT5_WRAP_UI(
#    4x4K_FBO_Ui_SRCS
#    mainwindow.ui
#)

add_definitions(-DVL_STATIC_LINKING)
#add_definitions(-DNDEBUG)

add_executable(4x4K_FBO WIN32 ${4x4K_FBO_SRCS} ${4x4K_FBO_HEADERS} ${4x4K_FBO_HPP_SRCS})

set_target_properties(4x4K_FBO PROPERTIES DEBUG_POSTFIX "d")


target_link_libraries(4x4K_FBO
    ${QT_LIBRARIES}
    ${OPENGL_LIBRARIES}
    ${VL_LIBRARIES}
)
