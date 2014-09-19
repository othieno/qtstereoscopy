/*
 * The MIT License (MIT)
 *
 * Copyright (c) 2014 Jeremy Othieno.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */
#include "qoculusriftstereorenderer.h"
#include <qpa/qplatformnativeinterface.h>
#include <QtGui/QGuiApplication>
#include <QtGui/QOpenGLFramebufferObject>
#include <QtGui/QWindow>
#if defined(Q_OS_LINUX)
#define OVR_OS_LINUX
#elif defined(Q_OS_MAC)
#define OVR_OS_MAC
#elif defined(Q_OS_WIN32)
#define OVR_OS_WIN32
#endif
#include <OVR_CAPI_GL.h>
#include <OVR.h>

/**
 * @brief Constructs a QOculusRiftStereoRenderer that is attached to an OpenGL capable window
 * and the Oculus Rift at the specified index.
 * @param window the OpenGL window.
 * @param index the Oculus Rift HMD device's index.
 */
QOculusRiftStereoRenderer::QOculusRiftStereoRenderer(const unsigned int& index) :
_display(index),
_fbo(nullptr),
_apiConfiguration(new ovrGLConfig),
_eyeTextureConfigurations(new ovrGLTexture[ovrEye_Count]),
_fovs(_display.recommendedFov()),
_pixelDensity(1.0f),
_forceZeroIPD(false),
_coherentFBO(false),
_coherentRenderingConfiguration(false)
{
   if (_apiConfiguration == nullptr && _eyeTextureConfigurations == nullptr)
      qFatal("[QtStereoscopy] Error: Could not instantiate a QOculusRiftStereoRenderer.");

   _fboFormat.setAttachment(QOpenGLFramebufferObject::Depth);
   _fboFormat.setTextureTarget(GL_TEXTURE_2D);

   // Configure the API for use with the Oculus SDK.
   const auto& resolution = _display.resolution();

   auto& Header = _apiConfiguration->OGL.Header;
   Header.API = ovrRenderAPI_OpenGL;
   Header.RTSize.w = resolution.width();
   Header.RTSize.h = resolution.height();
   Header.Multisample = 0;
}


/**
 * @brief Draws a scene with the stereoscopic model used by the Oculus Rift.
 */
void
QOculusRiftStereoRenderer::apply()
{
   // Make sure there're no "dirty" rendering configurations before rendering is performed.
   configureGL();

   // Draw a frame.
   const auto& frameTiming = ovrHmd_BeginFrame(_display, 0);
   _fbo->bind();

   glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
   for (const auto& eye : _display.eyeRenderOrder())
   {
      const auto& pose = ovrHmd_BeginEyeRender(_display, eye);
      paintGL(camera(eye, pose), frameTiming.DeltaSeconds);
      ovrHmd_EndEyeRender(_display, eye, pose, &_eyeTextureConfigurations[eye].Texture);
   }

   _fbo->release();
   ovrHmd_EndFrame(_display);

   // TODO Remove this block when ovrHmd_EndFrame cleans up after itself correctly.
   {
      glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
      glBindBuffer(GL_ARRAY_BUFFER, 0);
      glUseProgram(0);
   }
}


bool
QOculusRiftStereoRenderer::autoSwapsBuffers() const
{
   return true;
}


/**
 * @brief Returns the Oculus Rift display.
 */
QOculusRift&
QOculusRiftStereoRenderer::display()
{
   return _display;
}


/**
 * @brief Freeze updates for a specified eye.
 */
void
QOculusRiftStereoRenderer::freezeEyeUpdates(const ovrEyeType&, const bool)
{
   qFatal("[QtStereoscopy] Implement QOculusRiftStereoRenderer::freezeEyeUpdates.");
}

/**
 * @brief Return the pixel density.
 */
const float&
QOculusRiftStereoRenderer::pixelDensity() const
{
   return _pixelDensity;
}

/**
 * @brief Set the pixel density.
 * @param density the pixel density to set.
 */
void
QOculusRiftStereoRenderer::setPixelDensity(const float& density)
{
   // Note that when the pixel density is changed, the framebuffer object needs to be resized.
   if (!qFuzzyCompare(_pixelDensity, density))
   {
      _pixelDensity = density;
      _coherentFBO = false;
   }
}


void
QOculusRiftStereoRenderer::initializeWindow(const WId& winId)
{
   auto* const window = QWindow::fromWinId(winId);
   if (window != nullptr)
   {
#if defined(Q_OS_LINUX)
      void* const nativeDisplay =
      QGuiApplication::platformNativeInterface()->nativeResourceForWindow(QByteArray("display"), window);

      _apiConfiguration->OGL.Disp = static_cast<Display*>(nativeDisplay);
      _apiConfiguration->OGL.Win = static_cast<Window>(winId);
#elif defined(Q_OS_WIN32)
      _apiConfiguration->OGL.Window = static_cast<HWND>(winId);
#endif
      _coherentRenderingConfiguration = false;
   }
   else
      qWarning("[QtStereoscopy] Warning: Could not configure the target window.");
}


void
QOculusRiftStereoRenderer::initializeGL()
{}

/**
 * @brief Configures the window's OpenGL context for use with the Oculus SDK.
 */
void
QOculusRiftStereoRenderer::configureGL()
{
/*
 * FIXME
   // Toggle multisampling.
   const auto& enableMultisampling = isMultisamplingEnabled();
   auto& multisample = _apiConfiguration->OGL.Header.Multisample;
   if (static_cast<bool>(multisample) != enableMultisampling)
   {
      if (enableMultisampling)
      {
         glEnable(GL_MULTISAMPLE);
         multisample = 1;
      }
      else
      {
         glDisable(GL_MULTISAMPLE);
         multisample = 0;
      }
      _coherentRenderingConfiguration = false;
   }
*/
   // Configure the framebuffer object.
   if (!_coherentFBO)
   {
      // Calculate the FBO's new resolution.
      const auto& sizeL = ovrHmd_GetFovTextureSize(_display, ovrEye_Left,  _fovs[ovrEye_Left],  _pixelDensity);
      const auto& sizeR = ovrHmd_GetFovTextureSize(_display, ovrEye_Right, _fovs[ovrEye_Right], _pixelDensity);
      const auto& newSize = QSize(sizeL.w + sizeR.w, std::max(sizeL.h, sizeR.h));

      // Resets (re-instantiates) the framebuffer object if the size or format differs.
      if (_fbo == nullptr || _fbo->size() != newSize || _fbo->format() != _fboFormat)
      {
         _fbo.reset(new QOpenGLFramebufferObject(newSize, _fboFormat));
         if (_fbo == nullptr || !_fbo->isValid())
            qFatal("[QtStereoscopy] Error: Could not configure the framebuffer object.");

         const auto& w = _fbo->size().width();
         const auto& h = _fbo->size().height();

         // Configure eye texture configurations.
         for (unsigned int i = 0; i < ovrEye_Count; ++i)
         {
            auto& OGL = _eyeTextureConfigurations[i].OGL;
            auto& header = OGL.Header;

            OGL.TexId = _fbo->texture();

            // Set the camera viewport.
            auto& camera = _cameras[i];
            auto& viewport = camera.viewport;
            viewport = QRect(i * ((w + 1) * 0.5f), 0, w * 0.5f, h);

            // Set the API viewport.
            header.TextureSize.w = w;
            header.TextureSize.h = h;
            header.RenderViewport.Pos.x = viewport.x();
            header.RenderViewport.Pos.y = viewport.y();
            header.RenderViewport.Size.w = viewport.width();
            header.RenderViewport.Size.h = viewport.height();
         }
      }

      // Mark the framebuffer as coherent, however since it has been resized, the
      // rendering configuration needs to be updated.
      _coherentFBO = true;
      _coherentRenderingConfiguration = false;
   }

   // Update the rendering configuration.
   if (!_coherentRenderingConfiguration)
   {
      const ovrFovPort* const fovs = _fovs.data();
      const ovrRenderAPIConfig* const apiConfig = &(_apiConfiguration->Config);
      const unsigned int& distortionCapabilities = _display.enabledDistortionCapabilities();
      ovrEyeRenderDesc* const renderConfigs = _eyeRenderConfigurations.data();
      if (!ovrHmd_ConfigureRendering(_display, apiConfig, distortionCapabilities, fovs, renderConfigs))
         qFatal("[QtStereoscopy] Error: Could not update the render configuration.");

      if (_forceZeroIPD)
      {
         for (auto& config : _eyeRenderConfigurations)
            config.ViewAdjust = OVR::Vector3f(0);
      }

      // Set the view adjust vectors.
      for (const auto& eye : _display.eyeRenderOrder())
      {
         auto& camera = _cameras[eye];
         const auto& eyeRenderConfig = _eyeRenderConfigurations[eye];
         const auto& ovrViewAdjust = eyeRenderConfig.ViewAdjust;
         camera.viewAdjust = QVector3D(ovrViewAdjust.x, ovrViewAdjust.y, ovrViewAdjust.z);
      }
      _coherentRenderingConfiguration = true;
   }
}


/**
 * @brief Draws a scene in a specified stereoscopic configuration.
 */
void
QOculusRiftStereoRenderer::paintGL(const QStereoCamera&, const float&)
{}


const QStereoCamera&
QOculusRiftStereoRenderer::camera(const ovrEyeType& eye, const ovrPosef& headPose)
{
   auto& camera = _cameras[eye];
   const auto& eyeRenderConfig = _eyeRenderConfigurations[eye];

   // Calculate the view transformation matrix.
   const auto& ovrViewAdjust = eyeRenderConfig.ViewAdjust;
   const auto& ovrViewTranslation = OVR::Matrix4f::Translation(ovrViewAdjust);
   const auto& ovrViewOrientation = OVR::Matrix4f(OVR::Quatf(headPose.Orientation).Inverted());
   const auto* ovrViewData = &(ovrViewTranslation * ovrViewOrientation).M[0][0];

   for (unsigned int i = 0; i < 4; ++i)
   {
      camera.view(i, 0) = *ovrViewData++;
      camera.view(i, 1) = *ovrViewData++;
      camera.view(i, 2) = *ovrViewData++;
      camera.view(i, 3) = *ovrViewData++;
   }

   // Calculate the projection transformation matrices.
   if (!camera.projectionCoherent())
   {
      const auto& ovrFov = eyeRenderConfig.Fov;
      const auto& znear = camera.nearClippingPlane;
      const auto& zfar = camera.farClippingPlane;
      const auto& viewAdjustX = ovrViewAdjust.x;
      const auto& orthoScale = OVR::Vector2f(1.0f) / OVR::Vector2f(eyeRenderConfig.PixelsPerTanAngleAtCenter);
      const auto& ovrPerspective = ovrMatrix4f_Projection(ovrFov, znear, zfar, true);
      const auto* ovrPerspectiveData = &ovrPerspective.M[0][0];
      const auto& ovrOrtho = ovrMatrix4f_OrthoSubProjection(ovrPerspective, orthoScale, camera.orthoDistance, viewAdjustX);
      const auto* ovrOrthoData = &ovrOrtho.M[0][0];

      for (unsigned int i = 0; i < 4; ++i)
      {
         camera.perspective(i, 0) = *ovrPerspectiveData++;
         camera.perspective(i, 1) = *ovrPerspectiveData++;
         camera.perspective(i, 2) = *ovrPerspectiveData++;
         camera.perspective(i, 3) = *ovrPerspectiveData++;

         camera.ortho(i, 0) = *ovrOrthoData++;
         camera.ortho(i, 1) = *ovrOrthoData++;
         camera.ortho(i, 2) = *ovrOrthoData++;
         camera.ortho(i, 3) = *ovrOrthoData++;
      }
      camera.setProjectionCoherent(true);
   }
   return camera;
}


void
QOculusRiftStereoRenderer::ovrGLDeleter::operator()(ovrGLConfig* const ptr)
{
   delete ptr;
}


void
QOculusRiftStereoRenderer::ovrGLDeleter::operator()(ovrGLTexture* const ptr)
{
   delete ptr;
}
