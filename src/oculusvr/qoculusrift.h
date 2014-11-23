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

class QOculusRiftPrivate;
class QOculusRift Q_DECL_FINAL : public QAbstractStereoDisplay
{
public:
   struct TrackingFrustum
   {
      float horizontalFovInRadians;
      float verticalFovInRadians;
      float nearClippingDistanceInMeters;
      float farClippingDistanceInMeters;
   };

   struct FirmwareVersion
   {
      short int major;
      short int minor;
   };

   QOculusRift(const unsigned int& index = 0, const bool& forceDebugDevice = false);

   QString productName() const Q_DECL_OVERRIDE Q_DECL_FINAL;
   QString manufacturerName() const Q_DECL_OVERRIDE Q_DECL_FINAL;

   QSize resolution() const Q_DECL_OVERRIDE Q_DECL_FINAL;
   unsigned int refreshRate() const Q_DECL_OVERRIDE Q_DECL_FINAL;

   bool vsyncEnabled() const Q_DECL_OVERRIDE Q_DECL_FINAL;
   void enableVsync(const bool enable) Q_DECL_OVERRIDE Q_DECL_FINAL;

   float interpupillaryDistance() const Q_DECL_OVERRIDE Q_DECL_FINAL;
   void setInterpupillaryDistance(const float&) Q_DECL_OVERRIDE Q_DECL_FINAL;

   float eyeHeight() const Q_DECL_OVERRIDE Q_DECL_FINAL;
   void setEyeHeight(const float&) Q_DECL_OVERRIDE Q_DECL_FINAL;

   bool trackingAvailable() const Q_DECL_OVERRIDE Q_DECL_FINAL;
   bool trackingCameraAvailable() const;
   void resetTracking();

   bool orientationTrackingAvailable() const Q_DECL_OVERRIDE Q_DECL_FINAL;
   bool orientationTrackingEnabled() const Q_DECL_OVERRIDE Q_DECL_FINAL;
   void enableOrientationTracking(const bool enable = true) Q_DECL_OVERRIDE Q_DECL_FINAL;
   QQuaternion headOrientation() const Q_DECL_OVERRIDE Q_DECL_FINAL;

   bool positionalTrackingAvailable() const Q_DECL_OVERRIDE Q_DECL_FINAL;
   bool positionalTrackingEnabled() const Q_DECL_OVERRIDE Q_DECL_FINAL;
   void enablePositionalTracking(const bool enable = true) Q_DECL_OVERRIDE Q_DECL_FINAL;
   QVector3D headPosition() const Q_DECL_OVERRIDE Q_DECL_FINAL;
   TrackingFrustum positionalTrackingFrustum() const;

   bool eyeTrackingAvailable() const Q_DECL_OVERRIDE Q_DECL_FINAL;
   bool eyeTrackingEnabled() const Q_DECL_OVERRIDE Q_DECL_FINAL;
   void enableEyeTracking(const bool enable) Q_DECL_OVERRIDE Q_DECL_FINAL;
   QPointF eyePosition() const Q_DECL_OVERRIDE Q_DECL_FINAL;

   bool yawCorrectionEnabled() const;
   void enableYawCorrection(const bool enable = true);

   bool lowPersistenceEnabled() const;
   void enableLowPersistence(const bool enable = true);

   bool latencyTestingEnabled() const;
   void enableLatencyTesting(const bool enable = true);

   bool dynamicPredictionEnabled() const;
   void enableDynamicPrediction(const bool enable = true);

   const ovrHmdDesc& descriptor() const;
   const ovrHmdType& type() const;
   const bool& isDebugDevice() const;
   const ovrHmd& handle() const;
   operator const ovrHmd&() const;

   short int vendorId() const;
   short int productId() const;

   QString serialNumber() const;

   short int majorFirmwareVersion() const;
   short int minorFirmwareVersion() const;
   FirmwareVersion firmwareVersion() const;

   const unsigned int& supportedHmdCapabilities() const;
   const unsigned int& supportedTrackingCapabilities() const;
   const unsigned int& supportedDistortionCapabilities() const;

   QPoint windowPosition() const;

   std::array<ovrFovPort, ovrEye_Count> recommendedFov() const;
   std::array<ovrFovPort, ovrEye_Count> maximumFov() const;

   QString systemName() const;
   const int& systemId() const;
private:
   QOculusRiftPrivate* const d_ptr;
   Q_DECLARE_PRIVATE(QOculusRift);
};

QT_END_NAMESPACE

#endif // QOCULUSRIFT_H
