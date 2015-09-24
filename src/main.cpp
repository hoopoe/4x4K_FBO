#include <iostream>
#include <QApplication>
#include <vlCore/VisualizationLibrary.hpp>
#include <vlQt5/Qt5Widget.hpp>
#include "rtt.hpp"

using namespace std;
using namespace vl;
using namespace vlQt5;

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    /* init Visualization Library */
    VisualizationLibrary::init();

    /* setup the OpenGL context format */
    OpenGLContextFormat format;
    format.setDoubleBuffer(true);
    format.setRGBABits( 8,8,8,8 );
    format.setDepthBufferBits(24);
    format.setStencilBufferBits(8);
    format.setFullscreen(false);
    format.setVSync(true);
    //format.setMultisampleSamples(16);
    //format.setMultisample(true);

    /* create the applet to be run */
    //Ultra HD 3840 × 2160
    int rttWidth = 1920 * 2;
    int rttHeight = 1080 * 2;
    ref<Applet> applet = new RTT(rttWidth, rttHeight);
    applet->setAppletName("RTT Test");
    applet->initialize();
    /* create a native Qt5 window */
    ref<vlQt5::Qt5Widget> qt5_window = new vlQt5::Qt5Widget;
    /* bind the applet so it receives all the GUI events related to the OpenGLContext */
    qt5_window->addEventListener(applet.get());
    /* target the window so we can render on it */
    applet->rendering()->as<Rendering>()->renderer()->setFramebuffer( qt5_window->framebuffer() );
    /* black background */
    applet->rendering()->as<Rendering>()->camera()->viewport()->setClearColor( black );
    /* define the camera position and orientation */
    vec3 eye    = vec3(0,10,35); // camera position
    vec3 center = vec3(0,0,0);   // point the camera is looking at
    vec3 up     = vec3(0,1,0);   // up direction
    mat4 view_mat = mat4::getLookAt(eye, center, up);
    applet->rendering()->as<Rendering>()->camera()->setViewMatrix( view_mat );
    /* Initialize the OpenGL context and window properties */
    int x = 0;
    int y = 0;
//    int width = 512;
//    int height= 512;
    int width = rttWidth;
    int height= rttHeight;
    qt5_window->initQt5Widget( "Visualization Library on Qt5 - RTT Test", format, NULL, x, y, width, height );
//    qt5_window->setContinuousUpdate(false);

    // hack for windows to set flag WS_POPUP of the window style and reset flags of ExStyle
//    HWND hwndId = (HWND)qt5_window->winId();
//    ShowWindow(hwndId, SW_HIDE);
//    SetWindowLong(hwndId, GWL_STYLE, WS_OVERLAPPED | WS_CLIPSIBLINGS | WS_VISIBLE | WS_POPUP);
//    SetWindowLong(hwndId, GWL_EXSTYLE, WS_EX_LEFT | WS_EX_LTRREADING | WS_EX_RIGHTSCROLLBAR);
//    ShowWindow(hwndId, SW_SHOW);

    /* show the window */
    qt5_window->show();

    /* run the Win32 message loop */
    int val = app.exec();

    /* deallocate the window with all the OpenGL resources before shutting down Visualization Library */
    qt5_window = NULL;

    /* shutdown Visualization Library */
    VisualizationLibrary::shutdown();

    return val;
}
