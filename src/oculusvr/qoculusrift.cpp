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
#include "qoculusrift.h"
#include <atomic>
#if defined(Q_OS_LINUX)
#define OVR_OS_LINUX
#elif defined(Q_OS_MAC)
#define OVR_OS_MAC
#elif defined(Q_OS_WIN32)
#define OVR_OS_WIN32
#endif
#include <OVR.h>
#include <OVR_CAPI_GL.h>


//TODO Remove these after upgrading to SDK 0.4
#define ovr_GetTimeInSeconds() 0.0
#define ovrHmd_GetTrackingState ovrHmd_GetSensorState


// The number of instances signals when LibOVR is initialized and destroyed.
static std::atomic<unsigned int> _NUM_INSTANCES(0);


QOculusRift::QOculusRift(const unsigned int& index) :
_descriptor(),
_enabledHmdCapabilities(0),
_enabledTrackingCapabilities(0)
{
   // Initialize LibOVR iff this is the first instance.
   if (!_NUM_INSTANCES++)
   {
      if (Q_UNLIKELY(!ovr_Initialize()))
         qFatal("[QtStereoscopy] Error: Could not initialize LibOVR!");
   }

   // Make sure the device index is valid, i.e. if it is greater than zero, it must
   // be strictly less than the value returned by ovrHmd_Detect.
   if (index && index >= static_cast<unsigned int>(ovrHmd_Detect()))
      qFatal("[QtStereoscopy] Error: Could not find a device with index '%d'.", index);

   // Instantiate an HMD device. If no hardware is detected, create a debug device.
   ovrHmd deviceHandle = ovrHmd_Create(index);
   if (deviceHandle == nullptr)
   {
      deviceHandle = ovrHmd_CreateDebug(ovrHmd_DK1);
      if (Q_UNLIKELY(deviceHandle == nullptr))
         qFatal("[QtStereoscopy] Error: Could not open any Oculus Rift devices.");
      else
         qWarning("[QtStereoscopy] Warning: Created a debug device. Certain features may not be available.");
   }

   // Populate the descriptor.
   ovrHmd_GetDesc(deviceHandle, const_cast<ovrHmdDesc*>(&_descriptor));

   // Correct supported tracking capabilities.
   // TODO Check if this has been modified in SDK 0.4
   if (trackingAvailable() && _descriptor.Type == ovrHmd_DK1)
   {
      auto& caps = const_cast<unsigned int&>(_descriptor.SensorCaps);
      caps = caps & ~ovrSensorCap_Position & ~ovrSensorCap_YawCorrection;
   }

   // Enable all HMD capabilities, but do not disable VSYNC!
   _enabledHmdCapabilities = supportedHmdCapabilities() & ~ovrHmdCap_NoVSync;
   _enabledHmdCapabilities |= ovrHmdCap_LowPersistence; //TODO Remove statement after upgrading to SDK 0.4.
   updateHmdCapabilities();

   _enabledTrackingCapabilities = supportedTrackingCapabilities();
   updateTrackingCapabilities();
}


QOculusRift::~QOculusRift()
{
   // Destroy the device.
   ovrHmd_Destroy(_descriptor.Handle);

   // Shutdown LibOVR iff this is the last instance.
   if (!--_NUM_INSTANCES)
      ovr_Shutdown();
}


QOculusRift::operator const ovrHmd&() const
{
   return _descriptor.Handle;
}


QString
QOculusRift::productName() const
{
   return QString(_descriptor.ProductName);
}


QString
QOculusRift::manufacturerName() const
{
   return QString(_descriptor.Manufacturer);
}


QSize
QOculusRift::resolution() const
{
   return QSize(_descriptor.Resolution.w, _descriptor.Resolution.h);
}


unsigned int
QOculusRift::refreshRate() const
{
   switch (_descriptor.Type)
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
   return (_enabledHmdCapabilities & ovrHmdCap_NoVSync) != ovrHmdCap_NoVSync;
}


void
QOculusRift::enableVsync(const bool enable)
{
   if (enable != vsyncEnabled())
   {
      if (enable)
         _enabledHmdCapabilities &= ~ovrHmdCap_NoVSync;
      else
         _enabledHmdCapabilities |= ovrHmdCap_NoVSync;

      updateHmdCapabilities();
   }
}


float
QOculusRift::interpupillaryDistance() const
{
   return ovrHmd_GetFloat(_descriptor.Handle, OVR_KEY_IPD, OVR_DEFAULT_IPD);
}


void
QOculusRift::setInterpupillaryDistance(const float& distance)
{
   if (Q_UNLIKELY(!ovrHmd_SetFloat(_descriptor.Handle, OVR_KEY_IPD, distance)))
      qCritical("[QtStereoscopy] Error: Could not update the interpupillary distance.");
}


float
QOculusRift::userHeight() const
{
   return ovrHmd_GetFloat(_descriptor.Handle, OVR_KEY_EYE_HEIGHT, OVR_DEFAULT_EYE_HEIGHT);
}


void
QOculusRift::setUserHeight(const float& height)
{
   if (Q_UNLIKELY(!ovrHmd_SetFloat(_descriptor.Handle, OVR_KEY_EYE_HEIGHT, height)))
      qCritical("[QtStereoscopy] Error: Could not update the eye height.");
}


bool
QOculusRift::trackingAvailable() const
{
   // There's no sensor hardware for debug devices.
   if (_descriptor.Type == ovrHmd_Other)
      return false;
   else
   {
      qCritical("[QtStereoscopy] Implement QOculusRift::trackingAvailable.");
      return true;
   }
}


bool
QOculusRift::trackingCameraAvailable() const
{
   qCritical("[QtStereoscopy] Implement QOculusRift::trackingCameraAvailable.");
   return false;
}


void
QOculusRift::resetTracking()
{
   qCritical("[QtStereoscopy] Implement QOculusRift::resetTracking.");

   if (trackingAvailable())
      ovrHmd_ResetSensor(_descriptor.Handle);
}


bool
QOculusRift::orientationTrackingAvailable() const
{
   // Since all Rifts support orientation tracking, all we need to check is
   // whether or not tracking is available.
   return trackingAvailable();
}


bool
QOculusRift::orientationTrackingEnabled() const
{
   if (orientationTrackingAvailable())
   {
      const auto& trackingState = ovrHmd_GetTrackingState(_descriptor.Handle, ovr_GetTimeInSeconds());
      return trackingState.StatusFlags & (ovrStatus_OrientationTracked);
   }
   return false;
}


void
QOculusRift::enableOrientationTracking(const bool enable)
{
   if (orientationTrackingAvailable() && enable != orientationTrackingEnabled())
   {
      if (enable)
         _enabledTrackingCapabilities |= ovrSensorCap_Orientation;
      else
         _enabledTrackingCapabilities &= ~ovrSensorCap_Orientation;

      updateTrackingCapabilities();
   }
}


QQuaternion
QOculusRift::headOrientation() const
{
   //TODO Make sure the quaternion is correct!
   if (orientationTrackingAvailable())
   {
      const auto& trackingState = ovrHmd_GetTrackingState(_descriptor.Handle, ovr_GetTimeInSeconds());
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
   return trackingAvailable() && _descriptor.Type != ovrHmd_DK1;
}


bool
QOculusRift::positionalTrackingEnabled() const
{
   if (positionalTrackingAvailable())
   {
      const auto& trackingState = ovrHmd_GetTrackingState(_descriptor.Handle, ovr_GetTimeInSeconds());
      return trackingState.StatusFlags & (ovrStatus_PositionTracked);
   }
   return false;
}


void
QOculusRift::enablePositionalTracking(const bool enable)
{
   if (positionalTrackingAvailable() && enable != positionalTrackingEnabled())
   {
      if (enable)
         _enabledTrackingCapabilities |= ovrSensorCap_Position;
      else
         _enabledTrackingCapabilities &= ~ovrSensorCap_Position;

      updateTrackingCapabilities();
   }
}


QVector3D
QOculusRift::headPosition() const
{
   if (positionalTrackingAvailable())
   {
      const auto& trackingState = ovrHmd_GetTrackingState(_descriptor.Handle, ovr_GetTimeInSeconds());
      if (trackingState.StatusFlags & (ovrStatus_PositionTracked))
      {
         const auto& position = trackingState.Predicted.Pose.Position;
         return QVector3D(position.x, position.y, position.z);
      }
   }
   return QVector3D(0, 0, 0);
}


void
QOculusRift::positionalTrackingFrustum(float* const hFov, float* const vFov, float* const near, float* const far) const
{
   qCritical("[QtStereoscopy] Implement QOculusRift::positionalTrackingFrustum.");

   if (hFov != nullptr)
      *hFov = 0.0f;

   if (vFov != nullptr)
      *vFov = 0.0f;

   if (near != nullptr)
      *near = 0.0f;

   if (far != nullptr)
      *far = 0.0f;
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
   return positionalTrackingEnabled() &&
   (_enabledTrackingCapabilities & ovrSensorCap_YawCorrection) != ovrSensorCap_YawCorrection;
}


void
QOculusRift::enableYawCorrection(const bool enable)
{
   if (enable != yawCorrectionEnabled())
   {
      if (enable)
         _enabledTrackingCapabilities &= ~ovrSensorCap_YawCorrection;
      else
         _enabledTrackingCapabilities |= ovrSensorCap_YawCorrection;

      updateTrackingCapabilities();
   }
}


bool
QOculusRift::lowPersistenceEnabled() const
{
   return (_enabledHmdCapabilities & ovrHmdCap_LowPersistence) == ovrHmdCap_LowPersistence;
}


void
QOculusRift::enableLowPersistence(const bool enable)
{
   if (enable != lowPersistenceEnabled())
   {
      if (enable)
         _enabledHmdCapabilities |= ovrHmdCap_LowPersistence;
      else
         _enabledHmdCapabilities &= ~ovrHmdCap_LowPersistence;

      updateHmdCapabilities();
   }
}


bool
QOculusRift::latencyTestingEnabled() const
{
   return (_enabledHmdCapabilities & ovrHmdCap_LatencyTest) == ovrHmdCap_LatencyTest;
}


void
QOculusRift::enableLatencyTesting(const bool enable)
{
   if (enable != latencyTestingEnabled())
   {
      if (enable)
         _enabledHmdCapabilities |= ovrHmdCap_LatencyTest;
      else
         _enabledHmdCapabilities &= ~ovrHmdCap_LatencyTest;

      updateHmdCapabilities();
   }
}


bool
QOculusRift::dynamicPredictionEnabled() const
{
   return (_enabledHmdCapabilities & ovrHmdCap_DynamicPrediction) == ovrHmdCap_DynamicPrediction;
}


void
QOculusRift::enableDynamicPrediction(const bool enable)
{
   if (enable != dynamicPredictionEnabled())
   {
      if (enable)
         _enabledHmdCapabilities |= ovrHmdCap_DynamicPrediction;
      else
         _enabledHmdCapabilities &= ~ovrHmdCap_DynamicPrediction;

      updateHmdCapabilities();
   }
}


const ovrHmdDesc&
QOculusRift::descriptor() const
{
   return _descriptor;
}


const ovrHmdType&
QOculusRift::type() const
{
   return _descriptor.Type;
}


const ovrHmd&
QOculusRift::handle() const
{
   return _descriptor.Handle;
}


const short int&
QOculusRift::vendorId() const
{
   qCritical("[QtStereoscopy] Implement QOculusRift::getVendorId.");
   static const short tmp = 0;
   return tmp;
}


const short int&
QOculusRift::productId() const
{
   qCritical("[QtStereoscopy] Implement QOculusRift::getProductId.");
   static const short tmp = 0;
   return tmp;
}


QString
QOculusRift::serialNumber() const
{
   qCritical("[QtStereoscopy] Implement QOculusRift::getSerialNumber.");
   return QString("--");
}


void
QOculusRift::firmwareVersion(short int* const, short int* const) const
{
   qCritical("[QtStereoscopy] Implement QOculusRift::getFirmwareVersion.");
}


const unsigned int&
QOculusRift::supportedHmdCapabilities() const
{
   return _descriptor.HmdCaps;
}


const unsigned int&
QOculusRift::supportedTrackingCapabilities() const
{
   return _descriptor.SensorCaps;
}


const unsigned int&
QOculusRift::supportedDistortionCapabilities() const
{
   return _descriptor.DistortionCaps;
}


QPoint
QOculusRift::windowPosition() const
{
   return QPoint(_descriptor.WindowsPos.x, _descriptor.WindowsPos.y);
}


std::array<ovrFovPort, ovrEye_Count>
QOculusRift::recommendedFov() const
{
   std::array<ovrFovPort, ovrEye_Count> fovs;
   std::copy(std::begin(_descriptor.DefaultEyeFov), std::end(_descriptor.DefaultEyeFov), fovs.begin());
   return fovs;
}


std::array<ovrFovPort, ovrEye_Count>
QOculusRift::maximumFov() const
{
   std::array<ovrFovPort, ovrEye_Count> fovs;
   std::copy(std::begin(_descriptor.MaxEyeFov), std::end(_descriptor.MaxEyeFov), fovs.begin());
   return fovs;
}


QString
QOculusRift::systemName() const
{
   return QString(_descriptor.DisplayDeviceName);
}


const int&
QOculusRift::systemId() const
{
   return _descriptor.DisplayId;
}


void
QOculusRift::updateHmdCapabilities()
{
   ovrHmd_SetEnabledCaps(_descriptor.Handle, _enabledHmdCapabilities);
}


void
QOculusRift::updateTrackingCapabilities()
{
   if (trackingAvailable())
   {
      // If no tracking capabilities are activated, turn off the sensor. If, on the other hand, certain
      // tracking capabilities are set, and tracking is available, configure the sensor appropriately.
      if (!_enabledTrackingCapabilities)
         ovrHmd_StopSensor(_descriptor.Handle);
      else
      {
         if (!ovrHmd_StartSensor(_descriptor.Handle, supportedTrackingCapabilities(), _enabledTrackingCapabilities))
            qFatal("[QtStereoscopy] Error: Could not initialize the device's tracking sensors.");
      }
   }
}
