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
#include "qoculusriftstereorenderer_p.h"
#include "qoculusrift_p.h"
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


QOculusRiftStereoRendererPrivate::QOculusRiftStereoRendererPrivate
(
   QOculusRiftStereoRenderer* const parent,
   const unsigned int& index,
   const bool& forceDebugDevice
) :
QObject(parent),
_display(index, forceDebugDevice),
_fbo(nullptr),
_fboChanged(true),
_apiConfig(new ovrGLConfig),
_eyeTextureConfigs(new ovrGLTexture[ovrEye_Count]),
_eyeFovs(_display.recommendedFov()),
_eyeRenderConfigsChanged(true),
_enabledDistortionCapabilities(_display.supportedDistortionCapabilities()),
_pixelDensity(1.0f),
_forceZeroIPD(false)
{
   if (_apiConfig == nullptr && _eyeTextureConfigs == nullptr)
      qFatal("[QtStereoscopy] Error: Could not instantiate a QOculusRiftStereoRenderer.");

   _fboFormat.setAttachment(QOpenGLFramebufferObject::Depth);
   _fboFormat.setTextureTarget(GL_TEXTURE_2D);

   // Configure the API for use with the Oculus SDK.
   const auto& resolution = _display.resolution();

   auto& Header = _apiConfig->OGL.Header;
   Header.API = ovrRenderAPI_OpenGL;
   Header.RTSize.w = resolution.width();
   Header.RTSize.h = resolution.height();
   Header.Multisample = 0;
}


void
QOculusRiftStereoRendererPrivate::configureWindow(QWindow& window)
{
#if defined(Q_OS_LINUX)
   _apiConfig->OGL.Disp = static_cast<Display*>(nativeDisplay(window));
   _apiConfig->OGL.Win = static_cast<Window>(window.winId());
#elif defined(Q_OS_WIN32)
   _apiConfig->OGL.Window = static_cast<HWND>(winId);
#endif
   _eyeRenderConfigsChanged = true;
}


void
QOculusRiftStereoRendererPrivate::configureGL()
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
      _eyeRenderConfigsChanged = true;
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
            auto& OGL = _eyeTextureConfigs[i].OGL;
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
      _eyeRenderConfigsChanged = true;
   }

   // Update the rendering configuration.
   if (_eyeRenderConfigsChanged)
   {
      const ovrFovPort* const fovs = _eyeFovs.data();
      const ovrRenderAPIConfig* const apiConfig = &(_apiConfig->Config);
      ovrEyeRenderDesc* const renderConfigs = _eyeRenderConfigs.data();
      if (!ovrHmd_ConfigureRendering(_display, apiConfig, _enabledDistortionCapabilities, fovs, renderConfigs))
         qFatal("[QtStereoscopy] Error: Could not update the render configuration.");

      if (_forceZeroIPD)
      {
         for (auto& config : _eyeRenderConfigs)
            config.ViewAdjust = OVR::Vector3f(0);
      }

      // Set the view adjust vectors.
      for (const auto& eye : _display.descriptor().EyeRenderOrder)
      {
               auto& camera = _eyeCameras[eye];
         const auto& eyeRenderConfig = _eyeRenderConfigs[eye];
         const auto& ovrViewAdjust = eyeRenderConfig.ViewAdjust;

         camera.viewAdjust = QVector3D(ovrViewAdjust.x, ovrViewAdjust.y, ovrViewAdjust.z);
      }
      _eyeRenderConfigsChanged = false;
   }
}


QOculusRift&
QOculusRiftStereoRendererPrivate::display()
{
   return _display;
}


const QOculusRift&
QOculusRiftStereoRendererPrivate::const_display() const
{
   return _display;
}


void
QOculusRiftStereoRendererPrivate::bindFBO()
{
   _fbo->bind();
}


void
QOculusRiftStereoRendererPrivate::releaseFBO()
{
   _fbo->release();
}


const float&
QOculusRiftStereoRendererPrivate::pixelDensity() const
{
   return _pixelDensity;
}


void
QOculusRiftStereoRendererPrivate::setPixelDensity(const float& density)
{
   if (!qFuzzyCompare(_pixelDensity, density))
   {
      const float& min = QOculusRiftStereoRenderer::minPixelDensity();
      const float& max = QOculusRiftStereoRenderer::maxPixelDensity();

      _pixelDensity =
      density < min ? min :
      density > max ? max : density;

      // When the pixel density is changed, the framebuffer object needs to be resized.
      _fboChanged = true;
   }
}


bool
QOculusRiftStereoRendererPrivate::isDistortionCapabilityEnabled(const unsigned int& capability) const
{
   return (_enabledDistortionCapabilities & capability) == capability;
}


void
QOculusRiftStereoRendererPrivate::setDistortionCapabilityEnabled(const unsigned int& capability, const bool enable)
{
   if (enable != isDistortionCapabilityEnabled(capability))
   {
      if (enable)
         _enabledDistortionCapabilities |= capability;
      else
         _enabledDistortionCapabilities &= ~capability;

      _eyeRenderConfigsChanged = true;
   }
}


ovrGLTexture&
QOculusRiftStereoRendererPrivate::eyeTextureConfiguration(const ovrEyeType& eye)
{
   return _eyeTextureConfigs[eye];
}


const QStereoEyeCamera&
QOculusRiftStereoRendererPrivate::eyeCamera(const ovrEyeType& eye, const ovrPosef& headPose)
{
         auto& camera = _eyeCameras[eye];
   const auto& eyeRenderConfig = _eyeRenderConfigs[eye];
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


void*
QOculusRiftStereoRendererPrivate::nativeDisplay(QWindow& window)
{
   return QGuiApplication::platformNativeInterface()->nativeResourceForWindow(QByteArray("display"), &window);
}
