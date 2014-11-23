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
#ifndef QOCULUSRIFTSTEREORENDERER_P_H
#define QOCULUSRIFTSTEREORENDERER_P_H

#include "qoculusrift.h"
#include "qstereoeyecamera.h"
#include <QtGui/QMatrix4x4>
#include <QtGui/QOpenGLFramebufferObjectFormat>
#include <OVR_CAPI.h>

union ovrGLConfig;
union ovrGLTexture_s;
using ovrGLTexture = ovrGLTexture_s;


QT_BEGIN_NAMESPACE

class QOculusRiftStereoRendererPrivate : public QObject
{
public:
   QOculusRiftStereoRendererPrivate(QOculusRiftStereoRenderer* const parent, const unsigned int& index, const bool& forceDebugDevice);

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
   const QStereoEyeCamera& eyeCamera(const ovrEyeType& eye, const ovrPosef& headPose);
private:
   void* nativeDisplay(QWindow& window);

   QOculusRift _display;

   QOpenGLFramebufferObjectFormat _fboFormat;
   QScopedPointer<QOpenGLFramebufferObject> _fbo;

   bool _fboChanged;

   QScopedPointer<ovrGLConfig> _apiConfig;
   QScopedArrayPointer<ovrGLTexture> _eyeTextureConfigs;

   std::array<ovrEyeRenderDesc, ovrEye_Count> _eyeRenderConfigs;
   std::array<QStereoEyeCamera, ovrEye_Count> _eyeCameras;
   std::array<ovrFovPort, ovrEye_Count> _eyeFovs;

   bool _eyeRenderConfigsChanged;

   unsigned int _enabledDistortionCapabilities;
   float _pixelDensity;
   bool _forceZeroIPD;
};

QT_END_NAMESPACE

#endif // QOCULUSRIFTSTEREORENDERER_P_H
