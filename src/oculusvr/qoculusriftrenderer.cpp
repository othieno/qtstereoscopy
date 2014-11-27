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
#include <QtGui/QWindow>
#include <OVR_CAPI_GL.h>


QOculusRiftRenderer::QOculusRiftRenderer(const unsigned int& index, const bool& forceDebugDevice) :
d_ptr(new QOculusRiftRendererPrivate(this, index, forceDebugDevice))
{}


void
QOculusRiftRenderer::apply()
{
   Q_D(QOculusRiftRenderer);

   // Make sure there're no "dirty" rendering configurations before rendering is performed.
   d->configureGL();

   auto& display = d->display();

   const auto& frameTiming = ovrHmd_BeginFrame(display, 0);
   d->bindFBO();

   glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
   for (const auto& eye : display.descriptor().EyeRenderOrder)
   {
      const auto& pose = ovrHmd_BeginEyeRender(display, eye);
      const auto& parameters = d->eyeParameters(eye, pose);

      paintGL(parameters, frameTiming.DeltaSeconds);

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


void
QOculusRiftRenderer::swapBuffers(QOpenGLContext&, QSurface&)
{}


void
QOculusRiftRenderer::ignoreEyeUpdates(const QEye&, const bool)
{
   qCritical("[QtStereoscopy] Implement QOculusRiftRenderer::ignoreEyeUpdates.");
}


QOculusRift&
QOculusRiftRenderer::display()
{
   Q_D(QOculusRiftRenderer);
   return d->display();
}


const QOculusRift&
QOculusRiftRenderer::const_display() const
{
   Q_D(const QOculusRiftRenderer);
   return d->const_display();
}


const float&
QOculusRiftRenderer::pixelDensity() const
{
   Q_D(const QOculusRiftRenderer);
   return d->pixelDensity();
}


void
QOculusRiftRenderer::setPixelDensity(const float& density)
{
   Q_D(QOculusRiftRenderer);
   d->setPixelDensity(density);
}


bool
QOculusRiftRenderer::chromaticAberrationCorrectionEnabled() const
{
   Q_D(const QOculusRiftRenderer);
   return d->isDistortionCapabilityEnabled(ovrDistortionCap_Chromatic);
}


void
QOculusRiftRenderer::enableChromaticAberrationCorrection(const bool enable)
{
   Q_D(QOculusRiftRenderer);
   d->setDistortionCapabilityEnabled(ovrDistortionCap_Chromatic, enable);
}


bool
QOculusRiftRenderer::timewarpEnabled() const
{
   Q_D(const QOculusRiftRenderer);
   return d->isDistortionCapabilityEnabled(ovrDistortionCap_TimeWarp);
}


void
QOculusRiftRenderer::enableTimewarp(const bool enable)
{
   Q_D(QOculusRiftRenderer);
   d->setDistortionCapabilityEnabled(ovrDistortionCap_TimeWarp, enable);
}


bool
QOculusRiftRenderer::vignetteEnabled() const
{
   Q_D(const QOculusRiftRenderer);
   return d->isDistortionCapabilityEnabled(ovrDistortionCap_Vignette);
}


void
QOculusRiftRenderer::enableVignette(const bool enable)
{
   Q_D(QOculusRiftRenderer);
   d->setDistortionCapabilityEnabled(ovrDistortionCap_Vignette, enable);
}


void
QOculusRiftRenderer::initializeWindow(const WId& winId)
{
   auto* const window = QWindow::fromWinId(winId);
   if (window != nullptr)
   {
      Q_D(QOculusRiftRenderer);
      d->configureWindow(*window);
   }
   else
      qWarning("[QtStereoscopy] Warning: Could not configure the target window.");
}


void
QOculusRiftRenderer::initializeGL()
{}


void
QOculusRiftRenderer::paintGL(const QStereoEyeParameters&, const float&)
{}
