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


QOculusRiftStereoRenderer::QOculusRiftStereoRenderer(const unsigned int& index) :
_display(index),
_fbo(nullptr),
_fboChanged(true),
_apiConfiguration(new ovrGLConfig),
_eyeTextureConfigurations(new ovrGLTexture[ovrEye_Count]),
_eyeRenderConfigurationsChanged(true),
_eyeFovs(_display.recommendedFov()),
_enabledDistortionCapabilities(_display.supportedDistortionCapabilities()),
_pixelDensity(1.0f),
_forceZeroIPD(false)
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


void
QOculusRiftStereoRenderer::apply()
{
   // Make sure there're no "dirty" rendering configurations before rendering is performed.
   configureGL();

   // Draw a frame.
   const auto& frameTiming = ovrHmd_BeginFrame(_display, 0);
   _fbo->bind();

   glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
   for (const auto& eye : _display.descriptor().EyeRenderOrder)
   {
      const auto& pose = ovrHmd_BeginEyeRender(_display, eye);
      paintGL(eyeCamera(eye, pose), frameTiming.DeltaSeconds);
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


QOculusRift&
QOculusRiftStereoRenderer::display()
{
   return _display;
}


const float&
QOculusRiftStereoRenderer::pixelDensity() const
{
   return _pixelDensity;
}


void
QOculusRiftStereoRenderer::setPixelDensity(const float& density)
{
   // Note that when the pixel density is changed, the framebuffer object needs to be resized.
   if (!qFuzzyCompare(_pixelDensity, density))
   {
      _pixelDensity = density;
      _fboChanged = true;
   }
}


bool
QOculusRiftStereoRenderer::chromaticAberrationCorrectionEnabled() const
{
   return (_enabledDistortionCapabilities & ovrDistortionCap_Chromatic) == ovrDistortionCap_Chromatic;
}


void
QOculusRiftStereoRenderer::enableChromaticAberrationCorrection(const bool enable)
{
   if (enable != chromaticAberrationCorrectionEnabled())
   {
      if (enable)
         _enabledDistortionCapabilities |= ovrDistortionCap_Chromatic;
      else
         _enabledDistortionCapabilities &= ~ovrDistortionCap_Chromatic;

      _eyeRenderConfigurationsChanged = true;
   }
}


bool
QOculusRiftStereoRenderer::timewarpEnabled() const
{
   return (_enabledDistortionCapabilities & ovrDistortionCap_TimeWarp) == ovrDistortionCap_TimeWarp;
}


void
QOculusRiftStereoRenderer::enableTimewarp(const bool enable)
{
   if (enable != timewarpEnabled())
   {
      if (enable)
         _enabledDistortionCapabilities |= ovrDistortionCap_TimeWarp;
      else
         _enabledDistortionCapabilities &= ~ovrDistortionCap_TimeWarp;

      _eyeRenderConfigurationsChanged = true;
   }
}


bool
QOculusRiftStereoRenderer::vignetteEnabled() const
{
   return (_enabledDistortionCapabilities & ovrDistortionCap_Vignette) == ovrDistortionCap_Vignette;
}


void
QOculusRiftStereoRenderer::enableVignette(const bool enable)
{
   if (enable != vignetteEnabled())
   {
      if (enable)
         _enabledDistortionCapabilities |= ovrDistortionCap_Vignette;
      else
         _enabledDistortionCapabilities &= ~ovrDistortionCap_Vignette;

      _eyeRenderConfigurationsChanged = true;
   }
}


void
QOculusRiftStereoRenderer::freezeEyeUpdates(const ovrEyeType&, const bool)
{
   qCritical("[QtStereoscopy] Implement QOculusRiftStereoRenderer::freezeEyeUpdates.");
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
      _eyeRenderConfigurationsChanged = true;
   }
   else
      qWarning("[QtStereoscopy] Warning: Could not configure the target window.");
}


void
QOculusRiftStereoRenderer::initializeGL()
{}


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
      _eyeRenderConfigurationsChanged = true;
   }
*/
   // Configure the framebuffer object.
   if (_fboChanged)
   {
      // Calculate the FBO's new resolution.
      const auto& sizeL = ovrHmd_GetFovTextureSize(_display, ovrEye_Left,  _eyeFovs[ovrEye_Left],  _pixelDensity);
      const auto& sizeR = ovrHmd_GetFovTextureSize(_display, ovrEye_Right, _eyeFovs[ovrEye_Right], _pixelDensity);
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
            auto& camera = _eyeCameras[i];
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

      // Mark the framebuffer as usable, however since it has been resized, the
      // rendering configuration needs to be updated.
      _fboChanged = false;
      _eyeRenderConfigurationsChanged = true;
   }

   // Update the rendering configuration.
   if (_eyeRenderConfigurationsChanged)
   {
      const ovrFovPort* const fovs = _eyeFovs.data();
      const ovrRenderAPIConfig* const apiConfig = &(_apiConfiguration->Config);
      ovrEyeRenderDesc* const renderConfigs = _eyeRenderConfigurations.data();
      if (!ovrHmd_ConfigureRendering(_display, apiConfig, _enabledDistortionCapabilities, fovs, renderConfigs))
         qFatal("[QtStereoscopy] Error: Could not update the render configuration.");

      if (_forceZeroIPD)
      {
         for (auto& config : _eyeRenderConfigurations)
            config.ViewAdjust = OVR::Vector3f(0);
      }

      // Set the view adjust vectors.
      for (const auto& eye : _display.descriptor().EyeRenderOrder)
      {
         auto& camera = _eyeCameras[eye];
         const auto& eyeRenderConfig = _eyeRenderConfigurations[eye];
         const auto& ovrViewAdjust = eyeRenderConfig.ViewAdjust;
         camera.viewAdjust = QVector3D(ovrViewAdjust.x, ovrViewAdjust.y, ovrViewAdjust.z);
      }
      _eyeRenderConfigurationsChanged = false;
   }
}


void
QOculusRiftStereoRenderer::paintGL(const QStereoEyeCamera&, const float&)
{}


const QStereoEyeCamera&
QOculusRiftStereoRenderer::eyeCamera(const ovrEyeType& eye, const ovrPosef& headPose)
{
   auto& camera = _eyeCameras[eye];
   const auto& eyeRenderConfig = _eyeRenderConfigurations[eye];
   const auto& viewAdjust = eyeRenderConfig.ViewAdjust;
   const auto& orientation = headPose.Orientation;

   // Calculate the view transformation matrix from the eye's view offset and the head's orientation.
   QMatrix4x4 T;
   T.translate(viewAdjust.x, viewAdjust.y, viewAdjust.z);

   QMatrix4x4 R;
   R.rotate(QQuaternion(orientation.w, orientation.x, orientation.y, orientation.z).conjugate());

   camera.view = T * R;

   // Calculate the projection transformation matrices.
   if (camera.projectionChanged())
   {
      const auto& ovrFov = eyeRenderConfig.Fov;
      const auto& znear = camera.nearClippingPlane;
      const auto& zfar = camera.farClippingPlane;
      const auto& viewAdjustX = viewAdjust.x;
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
      camera.setProjectionChanged(false);
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
