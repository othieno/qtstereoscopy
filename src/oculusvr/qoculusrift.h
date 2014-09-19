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
#ifndef QOCULUSRIFT_H
#define QOCULUSRIFT_H

#include "qabstractstereodisplay.h"
#include <OVR_CAPI.h>


QT_BEGIN_NAMESPACE

class QOculusRift : public QAbstractStereoDisplay
{
public:
   explicit QOculusRift(const unsigned int& index = 0);
   ~QOculusRift();

   QString productName() const Q_DECL_OVERRIDE Q_DECL_FINAL;
   QString manufacturerName() const Q_DECL_OVERRIDE Q_DECL_FINAL;

   QSize resolution() const Q_DECL_OVERRIDE Q_DECL_FINAL;
   unsigned int refreshRate() const Q_DECL_OVERRIDE Q_DECL_FINAL;

   float interpupillaryDistance() const Q_DECL_OVERRIDE Q_DECL_FINAL;
   void setInterpupillaryDistance(const float&) Q_DECL_OVERRIDE Q_DECL_FINAL;

   float userHeight() const Q_DECL_OVERRIDE Q_DECL_FINAL;
   void setUserHeight(const float&) Q_DECL_OVERRIDE Q_DECL_FINAL;

   bool orientationTrackingAvailable() const Q_DECL_OVERRIDE Q_DECL_FINAL;
   bool orientationTrackingEnabled() const Q_DECL_OVERRIDE Q_DECL_FINAL;
   void enableOrientationTracking(const bool enable = true) Q_DECL_OVERRIDE Q_DECL_FINAL;
   QQuaternion headOrientation() const Q_DECL_OVERRIDE Q_DECL_FINAL;
   void resetHeadOrientation() Q_DECL_OVERRIDE Q_DECL_FINAL;

   bool positionalTrackingAvailable() const Q_DECL_OVERRIDE Q_DECL_FINAL;
   bool positionalTrackingEnabled() const Q_DECL_OVERRIDE Q_DECL_FINAL;
   void enablePositionalTracking(const bool enable = true) Q_DECL_OVERRIDE Q_DECL_FINAL;
   QVector3D headPosition() const Q_DECL_OVERRIDE Q_DECL_FINAL;
   void resetHeadPosition() Q_DECL_OVERRIDE Q_DECL_FINAL;

   bool eyeTrackingAvailable() const Q_DECL_OVERRIDE Q_DECL_FINAL;
   bool eyeTrackingEnabled() const Q_DECL_OVERRIDE Q_DECL_FINAL;
   void enableEyeTracking(const bool enable) Q_DECL_OVERRIDE Q_DECL_FINAL;
   QPoint eyePosition() const Q_DECL_OVERRIDE Q_DECL_FINAL;

   bool available() const Q_DECL_OVERRIDE Q_DECL_FINAL;

   bool vsyncEnabled() const;
   void enableVsync(const bool enable);

   bool yawCorrectionEnabled() const;
   void enableYawCorrection(const bool enable = true);

   bool lowPersistenceEnabled() const;
   void enableLowPersistence(const bool enable = true);

   bool latencyTestingEnabled() const;
   void enableLatencyTesting(const bool enable = true);

   bool dynamicPredictionEnabled() const;
   void enableDynamicPrediction(const bool enable = true);

   bool chromaticAberrationCorrectionEnabled() const;
   void enableChromaticAberrationCorrection(const bool enable = true);

   bool timewarpEnabled() const;
   void enableTimewarp(const bool enable = true);

   bool vignetteEnabled() const;
   void enableVignette(const bool enable = true);

   const ovrHmd& handle() const;
   operator const ovrHmd&() const;

   const ovrHmdType& type() const;

   const short int& vendorId() const;
   const short int& productId() const;

   QString serialNumber() const;

   void firmwareVersion(short int* const major, short int* const minor) const;

   void cameraFrustumFov(float* const horizontal, float* const vertical) const;
   void cameraFrustumDepth(float* const near, float* const far) const;

   unsigned int supportedHmdCapabilities() const;
   const unsigned int& enabledHmdCapabilities() const;

   unsigned int supportedTrackingCapabilities() const;
   const unsigned int& enabledTrackingCapabilities() const;

   unsigned int supportedDistortionCapabilities() const;
   const unsigned int& enabledDistortionCapabilities() const;

   QPoint windowPosition() const;

   std::array<ovrFovPort, ovrEye_Count> recommendedFov() const;
   std::array<ovrFovPort, ovrEye_Count> maximumFov() const;
   std::array<ovrEyeType, ovrEye_Count> eyeRenderOrder() const;

   QString systemName() const;
   const int& systemId() const;
private:
   /**
    * @brief Updates the HMD capabilities based on the enabled HMD capability bits.
    */
   void updateHmdCapabilities();
   /**
    * @brief Updates the tracking capabilities based on the enabled tracking capability bits.
    */
   void updateTrackingCapabilities();
   /**
    * @brief The device descriptor.
    */
   const ovrHmdDesc _descriptor;

   unsigned int _enabledHmdCapabilities;
   unsigned int _enabledTrackingCapabilities;
   unsigned int _enabledDistortionCapabilities;
};

QT_END_NAMESPACE

#endif // QOCULUSRIFT_H
