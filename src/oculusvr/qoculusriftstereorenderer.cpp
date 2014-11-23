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
#include <QtGui/QWindow>
#include <OVR_CAPI_GL.h>


QOculusRiftStereoRenderer::QOculusRiftStereoRenderer(const unsigned int& index, const bool& forceDebugDevice) :
d_ptr(new QOculusRiftStereoRendererPrivate(this, index, forceDebugDevice))
{}


void
QOculusRiftStereoRenderer::apply()
{
   Q_D(QOculusRiftStereoRenderer);

   // Make sure there're no "dirty" rendering configurations before rendering is performed.
   d->configureGL();

   auto& display = d->display();

   const auto& frameTiming = ovrHmd_BeginFrame(display, 0);
   d->bindFBO();

   glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
   for (const auto& eye : display.descriptor().EyeRenderOrder)
   {
      const auto& pose = ovrHmd_BeginEyeRender(display, eye);
      const auto& camera = d->eyeCamera(eye, pose);

      paintGL(camera, frameTiming.DeltaSeconds);

      ovrHmd_EndEyeRender(display, eye, pose, &d->eyeTextureConfiguration(eye).Texture);
   }

   d->releaseFBO();
   ovrHmd_EndFrame(display);

   // TODO Remove this block when ovrHmd_EndFrame cleans up after itself correctly.
   {
      glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
      glBindBuffer(GL_ARRAY_BUFFER, 0);
      glUseProgram(0);
   }
}


QOculusRift&
QOculusRiftStereoRenderer::display()
{
   Q_D(QOculusRiftStereoRenderer);
   return d->display();
}


const QOculusRift&
QOculusRiftStereoRenderer::const_display() const
{
   Q_D(const QOculusRiftStereoRenderer);
   return d->const_display();
}


const float&
QOculusRiftStereoRenderer::pixelDensity() const
{
   Q_D(const QOculusRiftStereoRenderer);
   return d->pixelDensity();
}


void
QOculusRiftStereoRenderer::setPixelDensity(const float& density)
{
   Q_D(QOculusRiftStereoRenderer);
   d->setPixelDensity(density);
}


bool
QOculusRiftStereoRenderer::chromaticAberrationCorrectionEnabled() const
{
   Q_D(const QOculusRiftStereoRenderer);
   return d->isDistortionCapabilityEnabled(ovrDistortionCap_Chromatic);
}


void
QOculusRiftStereoRenderer::enableChromaticAberrationCorrection(const bool enable)
{
   Q_D(QOculusRiftStereoRenderer);
   d->setDistortionCapabilityEnabled(ovrDistortionCap_Chromatic, enable);
}


bool
QOculusRiftStereoRenderer::timewarpEnabled() const
{
   Q_D(const QOculusRiftStereoRenderer);
   return d->isDistortionCapabilityEnabled(ovrDistortionCap_TimeWarp);
}


void
QOculusRiftStereoRenderer::enableTimewarp(const bool enable)
{
   Q_D(QOculusRiftStereoRenderer);
   d->setDistortionCapabilityEnabled(ovrDistortionCap_TimeWarp, enable);
}


bool
QOculusRiftStereoRenderer::vignetteEnabled() const
{
   Q_D(const QOculusRiftStereoRenderer);
   return d->isDistortionCapabilityEnabled(ovrDistortionCap_Vignette);
}


void
QOculusRiftStereoRenderer::enableVignette(const bool enable)
{
   Q_D(QOculusRiftStereoRenderer);
   d->setDistortionCapabilityEnabled(ovrDistortionCap_Vignette, enable);
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
      Q_D(QOculusRiftStereoRenderer);
      d->configureWindow(*window);
   }
   else
      qWarning("[QtStereoscopy] Warning: Could not configure the target window.");
}


void
QOculusRiftStereoRenderer::initializeGL()
{}


void
QOculusRiftStereoRenderer::paintGL(const QStereoEyeCamera&, const float&)
{}
