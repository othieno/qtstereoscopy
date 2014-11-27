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
#include "qoculusriftrenderer.h"
#include "qoculusriftrenderer_p.h"
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


QOculusRiftRendererPrivate::QOculusRiftRendererPrivate
(
   QOculusRiftRenderer* const parent,
   const unsigned int& index,
   const bool& forceDebugDevice
) :
QObject(parent),
display_(index, forceDebugDevice),
fbo_(nullptr),
fboSizeChanged_(true),
fboFormatChanged_(true),
apiConfig_(new ovrGLConfig),
eyeTextureConfigs_(new ovrGLTexture[ovrEye_Count]),
projectionChanged_({true, true}),
eyeFov_(display_.recommendedFov()),
eyeRenderingInfoChanged_(true),
enabledDistortionCapabilities_(display_.supportedDistortionCapabilities()),
pixelDensity_(1.0f),
forceZeroIPD_(false)
{
   if (apiConfig_ == nullptr && eyeTextureConfigs_ == nullptr)
      qFatal("[QtStereoscopy] Error: Could not instantiate a QOculusRiftRenderer.");

   fboFormat_.setAttachment(QOpenGLFramebufferObject::Depth);
   fboFormat_.setTextureTarget(GL_TEXTURE_2D);

   // Configure the API for use with the Oculus SDK.
   const auto& resolution = display_.resolution();

   auto& Header = apiConfig_->OGL.Header;
   Header.API = ovrRenderAPI_OpenGL;
   Header.RTSize.w = resolution.width();
   Header.RTSize.h = resolution.height();
   Header.Multisample = 0;

   // Initialize eye value for each eye parameter.
   for (int i = 0; i < 2; ++i)
      eyeParameters_[i].setEye(static_cast<QEye>(i));
}


void
QOculusRiftRendererPrivate::configureWindow(QWindow& window)
{
#if defined(Q_OS_LINUX)
   apiConfig_->OGL.Disp = static_cast<Display*>(nativeDisplay(window));
   apiConfig_->OGL.Win = static_cast<Window>(window.winId());
#elif defined(Q_OS_WIN32)
   apiConfig_->OGL.Window = static_cast<HWND>(winId);
#endif
   eyeRenderingInfoChanged_ = true;
}


void
QOculusRiftRendererPrivate::configureGL()
{
/*
 * FIXME
   // Toggle multisampling.
   const auto& enableMultisampling = isMultisamplingEnabled();
   auto& multisample = apiConfig_uration->OGL.Header.Multisample;
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
      eyeRenderingInfoChanged_ = true;
   }
*/


   if (fboSizeChanged_ || fboFormatChanged_)
   {
      configureFBO();
      fboSizeChanged_ = false;
      fboFormatChanged_ = false;
   }

   if (eyeRenderingInfoChanged_)
   {
      configureRendering();
      eyeRenderingInfoChanged_ = false;
   }
}


QOculusRift&
QOculusRiftRendererPrivate::display()
{
   return display_;
}


const QOculusRift&
QOculusRiftRendererPrivate::const_display() const
{
   return display_;
}


void
QOculusRiftRendererPrivate::bindFBO()
{
   fbo_->bind();
}


void
QOculusRiftRendererPrivate::releaseFBO()
{
   fbo_->release();
}


const float&
QOculusRiftRendererPrivate::pixelDensity() const
{
   return pixelDensity_;
}


void
QOculusRiftRendererPrivate::setPixelDensity(const float& density)
{
   if (!qFuzzyCompare(pixelDensity_, density))
   {
      constexpr float MIN = QOculusRiftRenderer::minPixelDensity();
      constexpr float MAX = QOculusRiftRenderer::maxPixelDensity();

      pixelDensity_ =
      density < MIN ? MIN :
      density > MAX ? MAX : density;

      // When the pixel density is changed, the framebuffer object needs to be resized.
      fboSizeChanged_ = true;
   }
}


bool
QOculusRiftRendererPrivate::isDistortionCapabilityEnabled(const unsigned int& capability) const
{
   return (enabledDistortionCapabilities_ & capability) == capability;
}


void
QOculusRiftRendererPrivate::setDistortionCapabilityEnabled(const unsigned int& capability, const bool enable)
{
   if (enable != isDistortionCapabilityEnabled(capability))
   {
      if (enable)
         enabledDistortionCapabilities_ |= capability;
      else
         enabledDistortionCapabilities_ &= ~capability;

      eyeRenderingInfoChanged_ = true;
   }
}


ovrGLTexture&
QOculusRiftRendererPrivate::eyeTextureConfiguration(const ovrEyeType& eye)
{
   return eyeTextureConfigs_[eye];
}


const QStereoEyeParameters&
QOculusRiftRendererPrivate::eyeParameters(const ovrEyeType& eye, const ovrPosef& pose)
{
         auto& eyeParams = eyeParameters_[eye];
   const auto& eyeRenderInfo = eyeRenderingInfo_[eye];
   const auto& eyeViewAdjust = QVector3D(eyeRenderInfo.ViewAdjust.x, eyeRenderInfo.ViewAdjust.y, eyeRenderInfo.ViewAdjust.z);
   const auto& orientation = QQuaternion(pose.Orientation.w, pose.Orientation.x, pose.Orientation.y, pose.Orientation.z);
   const auto& position = QVector3D(pose.Position.x, pose.Position.y, pose.Position.z);

   QMatrix4x4 view;
   view.translate(eyeViewAdjust);
   view.rotate(orientation.conjugate());

   eyeParams.setViewAdjust(eyeViewAdjust);
   eyeParams.setHeadOrientation(orientation);
   eyeParams.setHeadPosition(position);
   eyeParams.setView(view);

   auto& eyeProjectionChanged = projectionChanged_[eye];
   if (eyeProjectionChanged)
   {
      const auto& fov = eyeRenderInfo.Fov;
      const auto& znear = QStereoEyeParameters::nearClippingDistance();
      const auto& zfar = QStereoEyeParameters::farClippingDistance();
      const auto& orthoDistance = QStereoEyeParameters::orthoDistance();
      const auto& orthoScale = OVR::Vector2f(1.0f) / OVR::Vector2f(eyeRenderInfo.PixelsPerTanAngleAtCenter);
      const auto& ovrPerspective = ovrMatrix4f_Projection(fov, znear, zfar, true);
      const auto& ovrOrtho = ovrMatrix4f_OrthoSubProjection(ovrPerspective, orthoScale, orthoDistance, eyeViewAdjust.x());

      // Convert OVR::Matrix4f into QMatrix4x4.
      auto& perspective = const_cast<QMatrix4x4&>(eyeParams.perspective());
      auto& ortho = const_cast<QMatrix4x4&>(eyeParams.ortho());

      for (unsigned int i = 0; i < 4; ++i)
      {
         const auto& P = ovrPerspective.M[i];
         const auto& O = ovrOrtho.M[i];

         perspective.setRow(i, QVector4D(P[0], P[1], P[2], P[3]));
         ortho.setRow(i, QVector4D(O[0], O[1], O[2], O[3]));
      }
      eyeProjectionChanged = false;
   }
   return eyeParams;
}


void
QOculusRiftRendererPrivate::configureFBO()
{
   // Calculate the FBO's new resolution.
   const auto& sizeL = ovrHmd_GetFovTextureSize(display_, ovrEye_Left,  eyeFov_[ovrEye_Left],  pixelDensity_);
   const auto& sizeR = ovrHmd_GetFovTextureSize(display_, ovrEye_Right, eyeFov_[ovrEye_Right], pixelDensity_);
   const auto& size  = QSize(sizeL.w + sizeR.w, std::max(sizeL.h, sizeR.h));

   // Reset the FBO by allocating a new one.
   fbo_.reset(new QOpenGLFramebufferObject(size, fboFormat_));
   if (fbo_ == nullptr || fbo_->size() != size || !fbo_->isValid())
      qFatal("[QtStereoscopy] Error: Could not resize the framebuffer object.");

   const auto& w = size.width();
   const auto& h = size.height();

   for (unsigned int i = 0; i < ovrEye_Count; ++i)
   {
      const auto& viewport = QRect(i * ((w + 1) * 0.5f), 0, w * 0.5f, h);
            auto& OGL = eyeTextureConfigs_[i].OGL;

      // Update the API.
      OGL.TexId = fbo_->texture();
      OGL.Header.TextureSize.w = w;
      OGL.Header.TextureSize.h = h;
      OGL.Header.RenderViewport.Pos.x = viewport.x();
      OGL.Header.RenderViewport.Pos.y = viewport.y();
      OGL.Header.RenderViewport.Size.w = viewport.width();
      OGL.Header.RenderViewport.Size.h = viewport.height();

      // Update each eye's render viewport.
      eyeParameters_[i].setViewport(viewport);
   }
   // Since the FBO has changed, the render configurations need to be updated too.
   eyeRenderingInfoChanged_ = true;
}


void
QOculusRiftRendererPrivate::configureRendering()
{
   const ovrFovPort* const fovs = eyeFov_.data();
   const ovrRenderAPIConfig* const apiConfig = &(apiConfig_->Config);
   ovrEyeRenderDesc* const renderConfigs = eyeRenderingInfo_.data();
   if (!ovrHmd_ConfigureRendering(display_, apiConfig, enabledDistortionCapabilities_, fovs, renderConfigs))
      qFatal("[QtStereoscopy] Error: Could not update the render configuration.");

   if (forceZeroIPD_)
   {
      for (auto& info : eyeRenderingInfo_)
         info.ViewAdjust = OVR::Vector3f(0);
   }

   // Set the view adjust vectors.
   for (const auto& eye : display_.descriptor().EyeRenderOrder)
   {
            auto& parameters = eyeParameters_[eye];
      const auto& eyeRenderConfig = eyeRenderingInfo_[eye];
      const auto& eyeViewAdjust = eyeRenderConfig.ViewAdjust;

      parameters.setViewAdjust(QVector3D(eyeViewAdjust.x, eyeViewAdjust.y, eyeViewAdjust.z));
   }
}


void*
QOculusRiftRendererPrivate::nativeDisplay(QWindow& window)
{
   return QGuiApplication::platformNativeInterface()->nativeResourceForWindow(QByteArray("display"), &window);
}
