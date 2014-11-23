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
#include "qoculusrift_p.h"

//TODO Remove these after upgrading to SDK 0.4
#define ovr_GetTimeInSeconds() 0.0
#define ovrHmd_GetTrackingState ovrHmd_GetSensorState


QOculusRift::QOculusRift(const unsigned int& index, const bool& forceDebugDevice) :
d_ptr(new QOculusRiftPrivate(this, index, forceDebugDevice))
{}


QOculusRift::operator const ovrHmd&() const
{
   return handle();
}


QString
QOculusRift::productName() const
{
   return isDebugDevice() ? QString("Oculus Rift (Debug Device)") : QString(descriptor().ProductName);
}


QString
QOculusRift::manufacturerName() const
{
   return isDebugDevice() ? QString("Oculus VR") : QString(descriptor().Manufacturer);
}


QSize
QOculusRift::resolution() const
{
   const auto& deviceResolution = descriptor().Resolution;
   return QSize(deviceResolution.w, deviceResolution.h);
}


unsigned int
QOculusRift::refreshRate() const
{
   switch (type())
   {
      case ovrHmd_DK2:
         return 75;
      case ovrHmd_DK1:
      default:
         return 60;
   }
}


bool
QOculusRift::vsyncEnabled() const
{
   Q_D(const QOculusRift);
   return !d->isCapEnabled(ovrHmdCap_NoVSync);
}


void
QOculusRift::enableVsync(const bool enable)
{
   Q_D(QOculusRift);
   d->setCapEnabled(ovrHmdCap_NoVSync, !enable);
}


float
QOculusRift::interpupillaryDistance() const
{
   return ovrHmd_GetFloat(handle(), OVR_KEY_IPD, OVR_DEFAULT_IPD);
}


void
QOculusRift::setInterpupillaryDistance(const float& distance)
{
   if (!isDebugDevice())
   {
      if (Q_UNLIKELY(!ovrHmd_SetFloat(handle(), OVR_KEY_IPD, distance)))
         qCritical("[QtStereoscopy] Error: Could not update the interpupillary distance.");
   }
}


float
QOculusRift::eyeHeight() const
{
   return ovrHmd_GetFloat(handle(), OVR_KEY_EYE_HEIGHT, OVR_DEFAULT_EYE_HEIGHT);
}


void
QOculusRift::setEyeHeight(const float& height)
{
   if (!isDebugDevice())
   {
      if (Q_UNLIKELY(!ovrHmd_SetFloat(handle(), OVR_KEY_EYE_HEIGHT, height)))
         qCritical("[QtStereoscopy] Error: Could not update the eye height.");
   }
}


bool
QOculusRift::trackingAvailable() const
{
   Q_D(const QOculusRift);
   return d->trackingAvailable();
}


bool
QOculusRift::trackingCameraAvailable() const
{
   if (isDebugDevice())
      return false;
   else
   {
      qCritical("[QtStereoscopy] Implement QOculusRift::trackingCameraAvailable.");
      return false;
   }
}


void
QOculusRift::resetTracking()
{
   Q_D(QOculusRift);
   d->resetTracking();
}


bool
QOculusRift::orientationTrackingAvailable() const
{
   // Since all Rifts support orientation tracking, all we need to check is whether or not the
   // tracking sensors are available. The exception to this rule is the debug device, of course.
   return trackingAvailable();
}


bool
QOculusRift::orientationTrackingEnabled() const
{
   if (orientationTrackingAvailable())
   {
      const auto& trackingState = ovrHmd_GetTrackingState(handle(), ovr_GetTimeInSeconds());
      return trackingState.StatusFlags & (ovrStatus_OrientationTracked);
   }
   return false;
}


void
QOculusRift::enableOrientationTracking(const bool enable)
{
   if (orientationTrackingAvailable())
   {
      Q_D(QOculusRift);
      d->setCapEnabled(ovrSensorCap_Orientation, enable);
   }
}


QQuaternion
QOculusRift::headOrientation() const
{
   if (orientationTrackingAvailable())
   {
      const auto& trackingState = ovrHmd_GetTrackingState(handle(), ovr_GetTimeInSeconds());
      if (trackingState.StatusFlags & (ovrStatus_OrientationTracked))
      {
         const auto& orientation = trackingState.Predicted.Pose.Orientation;
         return QQuaternion(orientation.w, orientation.x, orientation.y, orientation.z);
      }
   }
   return QQuaternion(1.0, 0.0, 0.0, 0.0);
}


bool
QOculusRift::positionalTrackingAvailable() const
{
   // If the tracking sensor is available and this device is not a DK1, then positional tracking is supported.
   return trackingAvailable() && type() != ovrHmd_DK1;
}


bool
QOculusRift::positionalTrackingEnabled() const
{
   if (positionalTrackingAvailable())
   {
      const auto& trackingState = ovrHmd_GetTrackingState(handle(), ovr_GetTimeInSeconds());
      return trackingState.StatusFlags & (ovrStatus_PositionTracked);
   }
   return false;
}


void
QOculusRift::enablePositionalTracking(const bool enable)
{
   if (positionalTrackingAvailable())
   {
      Q_D(QOculusRift);
      d->setCapEnabled(ovrSensorCap_Position, enable);
   }
}


QVector3D
QOculusRift::headPosition() const
{
   if (positionalTrackingAvailable())
   {
      const auto& trackingState = ovrHmd_GetTrackingState(handle(), ovr_GetTimeInSeconds());
      if (trackingState.StatusFlags & (ovrStatus_PositionTracked))
      {
         const auto& position = trackingState.Predicted.Pose.Position;
         return QVector3D(position.x, position.y, position.z);
      }
   }
   return QVector3D(0, 0, 0);
}


QOculusRift::TrackingFrustum
QOculusRift::positionalTrackingFrustum() const
{
   qCritical("[QtStereoscopy] Implement QOculusRift::positionalTrackingFrustum.");
   return TrackingFrustum
   {
      0,
      0,
      0,
      0
   };
}


bool
QOculusRift::eyeTrackingAvailable() const
{
   return false;
}


bool
QOculusRift::eyeTrackingEnabled() const
{
   return false;
}


void
QOculusRift::enableEyeTracking(const bool)
{}


QPointF
QOculusRift::eyePosition() const
{
   return QPointF(0, 0);
}


bool
QOculusRift::yawCorrectionEnabled() const
{
   Q_D(const QOculusRift);
   return d->isCapEnabled(ovrSensorCap_YawCorrection);
}


void
QOculusRift::enableYawCorrection(const bool enable)
{
   if (trackingAvailable())
   {
      Q_D(QOculusRift);
      d->setCapEnabled(ovrSensorCap_YawCorrection, enable);
   }
}


bool
QOculusRift::lowPersistenceEnabled() const
{
   Q_D(const QOculusRift);
   return d->isCapEnabled(ovrHmdCap_LowPersistence);
}


void
QOculusRift::enableLowPersistence(const bool enable)
{
   if (!isDebugDevice()) //TODO Should the following condition be added: '&& type() != ovrHmd_DK1)'?
   {
      Q_D(QOculusRift);
      d->setCapEnabled(ovrHmdCap_LowPersistence, enable);
   }
}


bool
QOculusRift::latencyTestingEnabled() const
{
   Q_D(const QOculusRift);
   return d->isCapEnabled(ovrHmdCap_LatencyTest);
}


void
QOculusRift::enableLatencyTesting(const bool enable)
{
   if (!isDebugDevice())
   {
      Q_D(QOculusRift);
      d->setCapEnabled(ovrHmdCap_LatencyTest, enable);
   }
}


bool
QOculusRift::dynamicPredictionEnabled() const
{
   Q_D(const QOculusRift);
   return d->isCapEnabled(ovrHmdCap_DynamicPrediction);
}


void
QOculusRift::enableDynamicPrediction(const bool enable)
{
   if (trackingAvailable())
   {
      Q_D(QOculusRift);
      d->setCapEnabled(ovrHmdCap_DynamicPrediction, enable);
   }
}


const ovrHmdDesc&
QOculusRift::descriptor() const
{
   Q_D(const QOculusRift);
   return d->descriptor();
}


const ovrHmdType&
QOculusRift::type() const
{
   return descriptor().Type;
}


const bool&
QOculusRift::isDebugDevice() const
{
   Q_D(const QOculusRift);
   return d->isDebugDevice();
}


const ovrHmd&
QOculusRift::handle() const
{
   Q_D(const QOculusRift);
   return d->handle();
}


short int
QOculusRift::vendorId() const
{
   if (isDebugDevice())
      return 0;
   else
   {
      qCritical("[QtStereoscopy] Implement QOculusRift::vendorId.");
      return -1;
   }
}


short int
QOculusRift::productId() const
{
   if (isDebugDevice())
      return 0;
   else
   {
      qCritical("[QtStereoscopy] Implement QOculusRift::productId.");
      return -1;
   }
}


QString
QOculusRift::serialNumber() const
{
   if (isDebugDevice())
      return QString("N/A");
   else
   {
      qCritical("[QtStereoscopy] Implement QOculusRift::getSerialNumber.");
      return QString("N/A");
   }
}


short int
QOculusRift::majorFirmwareVersion() const
{
   if (isDebugDevice())
      return 0;
   else
   {
      qCritical("[QtStereoscopy] Implement QOculusRift::majorFirmwareVersion.");
      return -1;
   }
}


short int
QOculusRift::minorFirmwareVersion() const
{
   if (isDebugDevice())
      return 0;
   else
   {
      qCritical("[QtStereoscopy] Implement QOculusRift::minorFirmwareVersion.");
      return -1;
   }
}


QOculusRift::FirmwareVersion
QOculusRift::firmwareVersion() const
{
   return FirmwareVersion({majorFirmwareVersion(), minorFirmwareVersion()});
}


const unsigned int&
QOculusRift::supportedHmdCapabilities() const
{
   return descriptor().HmdCaps;
}


const unsigned int&
QOculusRift::supportedTrackingCapabilities() const
{
   return descriptor().SensorCaps;
}


const unsigned int&
QOculusRift::supportedDistortionCapabilities() const
{
   return descriptor().DistortionCaps;
}


QPoint
QOculusRift::windowPosition() const
{
   const auto& p = descriptor().WindowsPos;
   return QPoint(p.x, p.y);
}


std::array<ovrFovPort, ovrEye_Count>
QOculusRift::recommendedFov() const
{
   std::array<ovrFovPort, ovrEye_Count> fovs;
   std::copy(std::begin(descriptor().DefaultEyeFov), std::end(descriptor().DefaultEyeFov), fovs.begin());
   return fovs;
}


std::array<ovrFovPort, ovrEye_Count>
QOculusRift::maximumFov() const
{
   std::array<ovrFovPort, ovrEye_Count> fovs;
   std::copy(std::begin(descriptor().MaxEyeFov), std::end(descriptor().MaxEyeFov), fovs.begin());
   return fovs;
}


QString
QOculusRift::systemName() const
{
   return QString(descriptor().DisplayDeviceName);
}


const int&
QOculusRift::systemId() const
{
   return descriptor().DisplayId;
}
