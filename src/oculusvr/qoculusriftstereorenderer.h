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
#ifndef QOCULUSRIFTSTEREORENDERER_H
#define QOCULUSRIFTSTEREORENDERER_H

#include "qabstractstereorenderer.h"
#include "qoculusrift.h"
#include "qstereocamera.h"
#include <QtGui/QOpenGLFunctions>
#include <QtGui/QMatrix4x4>
#include <QtGui/QOpenGLFramebufferObjectFormat>
#include <memory>
#include <array>
#include <OVR_CAPI.h>

union ovrGLConfig;
union ovrGLTexture_s;
typedef ovrGLTexture_s ovrGLTexture;


QT_BEGIN_NAMESPACE

class QOculusRiftStereoRenderer : public QAbstractStereoRenderer
{
   Q_OBJECT
public:
   explicit QOculusRiftStereoRenderer(const unsigned int& index = 0);

   void apply() Q_DECL_OVERRIDE Q_DECL_FINAL;
   bool autoSwapsBuffers() const Q_DECL_OVERRIDE Q_DECL_FINAL;

   QOculusRift& display();

   const float& pixelDensity() const;
   void setPixelDensity(const float& density);

   void freezeEyeUpdates(const ovrEyeType& eye, const bool freeze = true);
protected:
   void initializeWindow(const WId&) Q_DECL_OVERRIDE Q_DECL_FINAL;
   void initializeGL() Q_DECL_OVERRIDE;
   void configureGL();
   void paintGL(const QStereoCamera&, const float&) Q_DECL_OVERRIDE;

   const QStereoCamera& camera(const ovrEyeType& eye, const ovrPosef& headPose);
private:
   QOculusRift _display;

   QOpenGLFramebufferObjectFormat _fboFormat;
   std::unique_ptr<QOpenGLFramebufferObject> _fbo;

   struct ovrGLDeleter Q_DECL_FINAL
   {
      void operator()(ovrGLConfig* const);
      void operator()(ovrGLTexture* const);
   };
   std::unique_ptr<ovrGLConfig, QOculusRiftStereoRenderer::ovrGLDeleter> _apiConfiguration;
   std::unique_ptr<ovrGLTexture[], QOculusRiftStereoRenderer::ovrGLDeleter> _eyeTextureConfigurations;

   std::array<ovrEyeRenderDesc, ovrEye_Count> _eyeRenderConfigurations;

   std::array<QStereoCamera, ovrEye_Count> _cameras;
   std::array<ovrFovPort, ovrEye_Count> _fovs;

   float _pixelDensity;

   bool _forceZeroIPD;

   bool _coherentFBO;
   bool _coherentRenderingConfiguration;
};

QT_END_NAMESPACE

#endif // QOCULUSRIFTSTEREORENDERER_H
