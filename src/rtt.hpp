#ifndef RTT_INCLUDE_ONCE
#define RTT_INCLUDE_ONCE

#include <vlGraphics/Applet.hpp>
#include <vlGraphics/GeometryPrimitives.hpp>
#include <vlGraphics/SceneManagerActorTree.hpp>
#include <vlGraphics/Rendering.hpp>
#include <vlGraphics/Actor.hpp>
#include <vlGraphics/Effect.hpp>
#include <vlCore/Time.hpp>
#include <vlGraphics/Light.hpp>
#include <vlGraphics/RenderingTree.hpp>
#include <vlGraphics/SceneManager.hpp>
#include <vlGraphics/GLSL.hpp>


class RTT: public vl::Applet
{
public:
  RTT(int FBO_Width, int FBO_Height)
  {
    mFBO_Width = FBO_Width;
    mFBO_Height = FBO_Height;
    mFPSTimer.start();
  }

  void initEvent()
  {
      vl::Log::notify(appletInfo());

      vl::ref<vl::RenderingTree> render_tree = new vl::RenderingTree;
      setRendering(render_tree.get());
      mMainRendering = new vl::Rendering;
      mRTT_Rendering = new vl::Rendering;
      render_tree->subRenderings()->push_back(mRTT_Rendering.get());
      render_tree->subRenderings()->push_back(mMainRendering.get());
      mMainRendering->sceneManagers()->push_back(new vl::SceneManagerActorTree);
      mRTT_Rendering->sceneManagers()->push_back(new vl::SceneManagerActorTree);

      // RTT rendering

      mRTT_Rendering->camera()->viewport()->setClearColor( vl::crimson );
      mRTT_Rendering->camera()->viewport()->set(0, 0, mFBO_Width, mFBO_Height);
      mRTT_Rendering->camera()->setProjectionPerspective();
      vl::mat4 m = vl::mat4::getLookAt(vl::vec3(0,0,10.5f), vl::vec3(0,0,0), vl::vec3(0,1,0));
      mRTT_Rendering->camera()->setViewMatrix(m);

      /* use a framebuffer object as render target */
      vl::ref<vl::FramebufferObject> framebuffer_object = openglContext()->createFramebufferObject(mFBO_Width, mFBO_Height);
      mRTT_Rendering->renderer()->setFramebuffer( framebuffer_object.get() );

      /* bind a depth buffer */
      vl::ref<vl::FBODepthBufferAttachment> fbo_depth_attachm = new vl::FBODepthBufferAttachment(vl::DBF_DEPTH_COMPONENT16);
      framebuffer_object->addDepthAttachment( fbo_depth_attachm.get() );

      /* use texture as color buffer */
      vl::ref<vl::Texture> texture = new vl::Texture(mFBO_Width, mFBO_Height, vl::TF_RGBA);
      vl::ref<vl::FBOTexture2DAttachment> fbo_tex_attachm = new vl::FBOTexture2DAttachment(texture.get(), 0, vl::T2DT_TEXTURE_2D);
      framebuffer_object->addTextureAttachment( vl::AP_COLOR_ATTACHMENT0, fbo_tex_attachm.get() );
      framebuffer_object->setDrawBuffer( vl::RDB_COLOR_ATTACHMENT0 );

      // Main rendering

      /* setup camera */
      mMainRendering->camera()->viewport()->setClearColor( vl::midnightblue );
      mMainRendering->camera()->viewport()->set(0, 0, openglContext()->framebuffer()->width(), openglContext()->framebuffer()->height());
      m = vl::mat4::getLookAt(vl::vec3(0,15,25), vl::vec3(0,0,0), vl::vec3(0,1,0));
      mMainRendering->camera()->setViewMatrix(m);

      /* use the opengl window as render target */
      mMainRendering->renderer()->setFramebuffer( openglContext()->framebuffer() );

      /* populate the scene */
      addRings(NULL);
      addCube(texture.get(), NULL);

      bindManipulators( mMainRendering->camera() );
  }

  void addCube(vl::Texture* texture1, vl::Texture* texture2)
  {
    vl::ref<vl::Light> light = new vl::Light;
    light->setAmbient( vl::fvec4( .1f, .1f, .1f, 1.0f) );
    light->setSpecular( vl::fvec4( .1f, .1f, .1f, 1.0f) );
    vl::ref<vl::Effect> effect1 = new vl::Effect;
    effect1->shader()->setRenderState( light.get(), 0 );
    effect1->shader()->gocLightModel()->setTwoSide(true);
    effect1->shader()->enable(vl::EN_LIGHTING);
    effect1->shader()->enable(vl::EN_DEPTH_TEST);
    effect1->shader()->gocTextureSampler(0)->setTexture( texture1 );
    effect1->shader()->gocTexEnv(0)->setMode(vl::TEM_MODULATE);
    // TD_TEXTURE_2D_MULTISAMPLE requires a special fragment shader
//    if (texture1->dimension() == vl::TD_TEXTURE_2D_MULTISAMPLE)
//    {
//      effect1->shader()->gocGLSLProgram()->attachShader( new vl::GLSLFragmentShader("/glsl/tex_multisample.fs") );
//      effect1->shader()->gocGLSLProgram()->gocUniform("ms_texture")->setUniformI(0);
//    }

    // ground plane
    const vl::real size = 50;
    vl::ref<vl::Geometry> ground = vl::makeGrid( vl::vec3(0,0,0), size, size, 2, 2, true, vl::fvec2(0,0), vl::fvec2(1,1) );
    ground->computeNormals();
    mMainRendering->sceneManagers()->at(0)->as<vl::SceneManagerActorTree>()->tree()->addActor( ground.get(), effect1.get() );

    if (texture2)
    {
      // box #1
      vl::ref<vl::Geometry> box1 = vl::makeBox( vl::vec3(-7,5,0), 10,10,10);
      box1->computeNormals();
      mMainRendering->sceneManagers()->at(0)->as<vl::SceneManagerActorTree>()->tree()->addActor( box1.get(), effect1.get() );

      // box #2
      vl::ref<vl::Effect> effect2 = new vl::Effect;
      effect2->shader()->shallowCopyFrom(*effect1->shader());
      vl::ref<vl::TextureSampler> texture_sampler = new vl::TextureSampler;
      texture_sampler->setTexture(texture2);
      effect2->shader()->setRenderState(texture_sampler.get(), 0);

      vl::ref<vl::Geometry> box2 = vl::makeBox( vl::vec3(+7,5,0), 10,10,10);
      box2->computeNormals();
      mMainRendering->sceneManagers()->at(0)->as<vl::SceneManagerActorTree>()->tree()->addActor( box2.get(), effect2.get() );
    }
    else
    {
      // box #1
      vl::ref<vl::Geometry> box1 = vl::makeBox( vl::vec3(0,5,0), 10,10,10);
      box1->computeNormals();
      mMainRendering->sceneManagers()->at(0)->as<vl::SceneManagerActorTree>()->tree()->addActor( box1.get(), effect1.get() );
    }

  }

  // helper function
  vl::Actor* addActor(vl::Rendering* rendering, vl::Renderable* renderable, vl::Effect* eff, vl::Transform* tr)
  {
    return rendering->sceneManagers()->at(0)->as<vl::SceneManagerActorTree>()->tree()->addActor(renderable, eff, tr);
  }

  // populates ring scene on mRTT_Rendering
  void addRings(vl::GLSLProgram* glsl)
  {
    vl::ref<vl::Effect> effect = new vl::Effect;
    effect->shader()->setRenderState(glsl);
    vl::ref<vl::Light> light = new vl::Light;
    light->setAmbient( vl::fvec4( .1f, .1f, .1f, 1.0f) );
    light->setSpecular( vl::fvec4( .9f, .9f, .9f, 1.0f) );
    effect->shader()->setRenderState( light.get(), 0 );
    effect->shader()->enable(vl::EN_LIGHTING);
    effect->shader()->enable(vl::EN_DEPTH_TEST);
    effect->shader()->enable(vl::EN_CULL_FACE);
    effect->shader()->gocMaterial()->setDiffuse( vl::yellow );

    mTransfRing1 = new vl::Transform;
    vl::ref<vl::Geometry> ring1;
    ring1 = vl::makeTorus( vl::vec3(0,0,0), 10,0.5, 20,100);
    addActor(mRTT_Rendering.get(), ring1.get(), effect.get(), mTransfRing1.get());

    mTransfRing2 = new vl::Transform;
    vl::ref<vl::Geometry> ring2;
    ring2= vl::makeTorus( vl::vec3(0,0,0), 9,0.5, 20,100);
    addActor(mRTT_Rendering.get(), ring2.get(), effect.get(), mTransfRing2.get());

    mTransfRing3 = new vl::Transform;
    vl::ref<vl::Geometry> ring3;
    ring3= vl::makeTorus( vl::vec3(0,0,0), 8,0.5, 20,100);
    addActor(mRTT_Rendering.get(), ring3.get(), effect.get(), mTransfRing3.get());

    mTransfRing4 = new vl::Transform;
    vl::ref<vl::Geometry> ring4;
    ring4= vl::makeTorus( vl::vec3(0,0,0), 7,0.5, 20,100);
    addActor(mRTT_Rendering.get(), ring4.get(), effect.get(), mTransfRing4.get());

    mTransfRing5 = new vl::Transform;
    vl::ref<vl::Geometry> ring5;
    ring5= vl::makeTorus( vl::vec3(0,0,0), 6,0.5, 20,100);
    addActor(mRTT_Rendering.get(), ring5.get(), effect.get(), mTransfRing5.get());

    // update transform hierarchy every frame
    mRTT_Rendering->transform()->addChild(mTransfRing1.get());
    mTransfRing1->addChild(mTransfRing2.get());
    mTransfRing2->addChild(mTransfRing3.get());
    mTransfRing3->addChild(mTransfRing4.get());
    mTransfRing4->addChild(mTransfRing5.get());
  }


  void updateEvent()
  {
    vl::Applet::updateEvent();

    if ( mFPSTimer.elapsed() > 2 )
    {
      mFPSTimer.start();
      openglContext()->setWindowTitle( vl::Say("[%.1n] %s") << fps() << appletName()  + " - " + vl::String("VL ") + vl::VisualizationLibrary::versionString() );
      vl::Log::print( vl::Say("FPS=%.1n\n") << fps() );
    }
  }

  virtual void updateScene()
  {
    mX  = vl::Time::currentTime() * 45*2;
    mY  = vl::Time::currentTime() * 45*2.1f;
    mX2 = vl::Time::currentTime() * 45*2.2f;
    mY2 = vl::Time::currentTime() * 45*2.3f;
    mX3 = vl::Time::currentTime() * 45*2.4f;

    mTransfRing1->setLocalMatrix( vl::mat4::getRotation(mX,  1,0,0) );
    mTransfRing2->setLocalMatrix( vl::mat4::getRotation(mY,  0,1,0) );
    mTransfRing3->setLocalMatrix( vl::mat4::getRotation(mX2, 1,0,0) );
    mTransfRing4->setLocalMatrix( vl::mat4::getRotation(mY2, 0,1,0) );
    mTransfRing5->setLocalMatrix( vl::mat4::getRotation(mX3, 1,0,0) );
  }

  virtual void destroyEvent()
  {
    Applet::destroyEvent();

    // release all OpenGL resources
    setRendering(NULL);
    mMainRendering = NULL;
    mRTT_Rendering = NULL;
  }

  void resizeEvent(int w, int h)
  {
    mMainRendering->camera()->viewport()->setWidth ( w );
    mMainRendering->camera()->viewport()->setHeight( h );
    mMainRendering->camera()->setProjectionPerspective();
  }

  void keyPressEvent(unsigned short ch, vl::EKey key)
  {
    Applet::keyPressEvent(ch, key);

    if (ch == '1')
    {
      vl::Log::print("Show text fps.\n");
    }
    else
    if (ch == '2')
    {
      vl::Log::print("Start moving.\n");
    }
    else
    if (ch == '3')
    {
      vl::Log::print("Stop moving.\n");
    }
  }

  virtual vl::String appletInfo()
  {
    return "Applet info: " + appletName() + "\n" +
    "Keys:\n" +
    "- Escape: quits the application.\n" +
    "- T:  enables the TrackballManipulator.\n" +
    "- F:  enables the GhostCameraManipulator (use A/D S/W keys).\n" +
    "- F1: toggles fullscreen mode if supported.\n" +
    "- F5: saves a screenshot of the current OpenGL window.\n" +
    "- C:  toggles the continuous update of the OpenGL window.\n" +
    "- U:  force update of the OpenGL window.\n" +
    "\n";
  }

private:
  vl::Time mFPSTimer;

protected:
  vl::real mX, mY, mX2, mY2, mX3;
  vl::ref<vl::Transform> mTransfRing1;
  vl::ref<vl::Transform> mTransfRing2;
  vl::ref<vl::Transform> mTransfRing3;
  vl::ref<vl::Transform> mTransfRing4;
  vl::ref<vl::Transform> mTransfRing5;
  vl::ref<vl::Rendering> mMainRendering;
  vl::ref<vl::Rendering> mRTT_Rendering;
  int mFBO_Width, mFBO_Height;
};

#endif
