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
#ifndef QOCULUSRIFTRENDERER_P_H
#define QOCULUSRIFTRENDERER_P_H

#include "qoculusrift.h"
#include "qstereoeyeparameters.h"
#include <QtGui/QMatrix4x4>
#include <QtGui/QOpenGLFramebufferObjectFormat>
#include <OVR_CAPI.h>

union ovrGLConfig;
union ovrGLTexture_s;
using ovrGLTexture = ovrGLTexture_s;


QT_BEGIN_NAMESPACE

class QOculusRiftRendererPrivate : public QObject
{
public:
   QOculusRiftRendererPrivate(QOculusRiftRenderer* const parent, const unsigned int& index, const bool& forceDebugDevice);

   void configureWindow(QWindow& window);
   void configureGL();
   void apply();

   QOculusRift& display();
   const QOculusRift& const_display() const;

   void bindFBO();
   void releaseFBO();

   const float& pixelDensity() const;
   void setPixelDensity(const float&);

   bool isDistortionCapabilityEnabled(const unsigned int& capability) const;
   void setDistortionCapabilityEnabled(const unsigned int& capability, const bool enable);

   ovrGLTexture& eyeTextureConfiguration(const ovrEyeType& eye);
   const QStereoEyeParameters& eyeParameters(const ovrEyeType& eye, const ovrPosef& pose);
private:
   void configureFBO();
   void configureRendering();

   void* nativeDisplay(QWindow& window);

   QOculusRift display_;

   QScopedPointer<QOpenGLFramebufferObject> fbo_;
   QOpenGLFramebufferObjectFormat fboFormat_;
   bool fboSizeChanged_;
   bool fboFormatChanged_;

   QScopedPointer<ovrGLConfig> apiConfig_;
   QScopedArrayPointer<ovrGLTexture> eyeTextureConfigs_;

   std::array<bool, ovrEye_Count> projectionChanged_;

   std::array<QStereoEyeParameters, ovrEye_Count> eyeParameters_;
   std::array<ovrFovPort,           ovrEye_Count> eyeFov_;
   std::array<ovrEyeRenderDesc,     ovrEye_Count> eyeRenderingInfo_;
   bool eyeRenderingInfoChanged_;

   unsigned int enabledDistortionCapabilities_;
   float pixelDensity_;
   bool forceZeroIPD_;
};

QT_END_NAMESPACE

#endif // QOCULUSRIFTRENDERER_P_H
