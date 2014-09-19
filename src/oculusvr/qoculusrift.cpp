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


// The number of instances signals when LibOVR is initialized and destroyed.
static std::atomic<unsigned int> _NUM_INSTANCES(0);


QOculusRift::QOculusRift(const unsigned int& index) :
_descriptor(),
_enabledHmdCapabilities(0),
_enabledTrackingCapabilities(0),
_enabledDistortionCapabilities(0)
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

   // Populate the descriptor and enable all supported capabilities.
   ovrHmd_GetDesc(deviceHandle, const_cast<ovrHmdDesc*>(&_descriptor));

   _enabledHmdCapabilities = supportedHmdCapabilities();
   updateHmdCapabilities();
   _enabledTrackingCapabilities = supportedTrackingCapabilities();
   updateTrackingCapabilities();
   _enabledDistortionCapabilities = supportedDistortionCapabilities();
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
QOculusRift::orientationTrackingAvailable() const
{
   qCritical("[QtStereoscopy] Implement QOculusRift::orientationTrackingAvailable.");
   return false;
}


bool
QOculusRift::orientationTrackingEnabled() const
{
   if (orientationTrackingAvailable())
   {
      qCritical("[QtStereoscopy] Implement QOculusRift::orientationTrackingEnabled.");
      return false;
   }
   else
      return false;
}


void
QOculusRift::enableOrientationTracking(const bool enable)
{
   if (orientationTrackingAvailable() && enable != orientationTrackingEnabled())
   {
      qCritical("[QtStereoscopy] Implement QOculusRift::enableOrientationTracking.");
      updateTrackingCapabilities();
   }
}


QQuaternion
QOculusRift::headOrientation() const
{
   if (orientationTrackingAvailable())
   {
      qCritical("[QtStereoscopy] Implement QOculusRift::headOrientation.");
      return QQuaternion(0.0, 0.0, 0.0, 0.0);
   }
   else
      return QQuaternion(0.0, 0.0, 0.0, 0.0);
}


void
QOculusRift::resetHeadOrientation()
{
   if (orientationTrackingEnabled())
   {
      qCritical("[QtStereoscopy] Implement QOculusRift::resetHeadOrientation.");
   }
}


bool
QOculusRift::positionalTrackingAvailable() const
{
   qCritical("[QtStereoscopy] Implement QOculusRift::positionalTrackingAvailable.");
   return false;
}


bool
QOculusRift::positionalTrackingEnabled() const
{
   if (positionalTrackingAvailable())
   {
      qCritical("[QtStereoscopy] Implement QOculusRift::positionalTrackingEnabled.");
      return false;
   }
   else
      return false;
}


void
QOculusRift::enablePositionalTracking(const bool enable)
{
   if (positionalTrackingAvailable() && enable != positionalTrackingEnabled())
   {
      qCritical("[QtStereoscopy] Implement QOculusRift::enablePositionalTracking.");
      updateTrackingCapabilities();
   }
}


QVector3D
QOculusRift::headPosition() const
{
   if (positionalTrackingAvailable())
   {
      qCritical("[QtStereoscopy] Implement QOculusRift::headPosition.");
      return QVector3D(0, 0, 0);
   }
   else
      return QVector3D(0, 0, 0);
}


void
QOculusRift::resetHeadPosition()
{
   if (positionalTrackingEnabled())
   {
      qCritical("[QtStereoscopy] Implement QOculusRift::resetHeadPosition.");
   }
}


bool
QOculusRift::eyeTrackingAvailable() const
{
   qCritical("[QtStereoscopy] Implement QOculusRift::eyeTrackingAvailable.");
   return false;
}


bool
QOculusRift::eyeTrackingEnabled() const
{
   if (eyeTrackingAvailable())
   {
      qCritical("[QtStereoscopy] Implement QOculusRift::eyeTrackingEnabled.");
      return false;
   }
   else
      return false;
}


void
QOculusRift::enableEyeTracking(const bool enable)
{
   if (eyeTrackingAvailable() && enable != eyeTrackingEnabled())
   {
      qCritical("[QtStereoscopy] Implement QOculusRift::enableEyeTracking.");
      updateTrackingCapabilities();
   }
}


QPoint
QOculusRift::eyePosition() const
{
   if (eyeTrackingEnabled())
   {
      qCritical("[QtStereoscopy] Implement QOculusRift::getEyePosition.");
      return QPoint(0, 0);
   }
   else
      return QPoint(0, 0);
}


bool
QOculusRift::available() const
{
   qCritical("[QtStereoscopy] Implement QOculusRift::available.");
   return true;
}


bool
QOculusRift::vsyncEnabled() const
{
   qCritical("[QtStereoscopy] Implement QOculusRift::vsyncEnabled.");
   return false;
}


void
QOculusRift::enableVsync(const bool)
{
   qCritical("[QtStereoscopy] Implement QOculusRift::enableVsync.");
   updateHmdCapabilities();
}


bool
QOculusRift::yawCorrectionEnabled() const
{
   qCritical("[QtStereoscopy] Implement QOculusRift::yawCorrectionEnabled.");
   return false;
}


void
QOculusRift::enableYawCorrection(const bool)
{
   qCritical("[QtStereoscopy] Implement QOculusRift::enableYawCorrection.");
   updateTrackingCapabilities();
}


bool
QOculusRift::lowPersistenceEnabled() const
{
   qCritical("[QtStereoscopy] Implement QOculusRift::lowPersistenceEnabled.");
   return false;
}


void
QOculusRift::enableLowPersistence(const bool)
{
   qCritical("[QtStereoscopy] Implement QOculusRift::enableLowPersistence.");
   updateHmdCapabilities();
}


bool
QOculusRift::latencyTestingEnabled() const
{
   qCritical("[QtStereoscopy] Implement QOculusRift::latencyTestingEnabled.");
   return false;
}


void
QOculusRift::enableLatencyTesting(const bool)
{
   qCritical("[QtStereoscopy] Implement QOculusRift::enableLatencyTesting.");
   updateHmdCapabilities();
}


bool
QOculusRift::dynamicPredictionEnabled() const
{
   qCritical("[QtStereoscopy] Implement QOculusRift::dynamicPredictionEnabled.");
   return false;
}


void
QOculusRift::enableDynamicPrediction(const bool)
{
   qCritical("[QtStereoscopy] Implement QOculusRift::enableDynamicPrediction.");
   updateHmdCapabilities();
}


bool
QOculusRift::chromaticAberrationCorrectionEnabled() const
{
   qCritical("[QtStereoscopy] Implement QOculusRift::chromaticAberrationCorrectionEnabled.");
   return false;
}


void
QOculusRift::enableChromaticAberrationCorrection(const bool)
{
   qCritical("[QtStereoscopy] Implement QOculusRift::enableChromaticAberrationCorrection.");
}


bool
QOculusRift::timewarpEnabled() const
{
   qCritical("[QtStereoscopy] Implement QOculusRift::timewarpEnabled.");
   return false;
}


void
QOculusRift::enableTimewarp(const bool)
{
   qCritical("[QtStereoscopy] Implement QOculusRift::enableTimewarp.");
}


bool
QOculusRift::vignetteEnabled() const
{
   qCritical("[QtStereoscopy] Implement QOculusRift::vignetteEnabled.");
   return false;
}


void
QOculusRift::enableVignette(const bool)
{
   qCritical("[QtStereoscopy] Implement QOculusRift::enableVignette.");
}



const ovrHmd&
QOculusRift::handle() const
{
   return _descriptor.Handle;
}


const ovrHmdType&
QOculusRift::type() const
{
   return _descriptor.Type;
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


void
QOculusRift::cameraFrustumFov(float* const, float* const) const
{
   qCritical("[QtStereoscopy] Implement QOculusRift::getCameraFrustumFov.");
}


void
QOculusRift::cameraFrustumDepth(float* const, float* const) const
{
   qCritical("[QtStereoscopy] Implement QOculusRift::getCameraFrustumDepth.");
}


unsigned int
QOculusRift::supportedHmdCapabilities() const
{
   return _descriptor.HmdCaps;
}


const unsigned int&
QOculusRift::enabledHmdCapabilities() const
{
   return _enabledHmdCapabilities;
}


unsigned int
QOculusRift::supportedTrackingCapabilities() const
{
   qCritical("[QtStereoscopy] Implement QOculusRift::supportedTrackingCapabilities.");
   return 0;
/*
   // Enable tracking capabilities if the hardware is available.
   if (isTrackingAvailable())
   {
      switch (_descriptor.Type)
      {
         // Positional tracking and yaw correction are not supported by the debug device or the DK1.
         case ovrHmd_Other:
         case ovrHmd_DK1:
            return _descriptor.SensorCaps & ~ovrSensorCap_Position & ~ovrSensorCap_YawCorrection;
         default:
            return _descriptor.SensorCaps;
      }
   }
   else
      return 0;
*/
}


const unsigned int&
QOculusRift::enabledTrackingCapabilities() const
{
   return _enabledTrackingCapabilities;
}


unsigned int
QOculusRift::supportedDistortionCapabilities() const
{
   return _descriptor.DistortionCaps;
}


const unsigned int&
QOculusRift::enabledDistortionCapabilities() const
{
   return _enabledDistortionCapabilities;
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


std::array<ovrEyeType, ovrEye_Count>
QOculusRift::eyeRenderOrder() const
{
   std::array<ovrEyeType, ovrEye_Count> order;
   std::copy(std::begin(_descriptor.EyeRenderOrder), std::end(_descriptor.EyeRenderOrder), order.begin());
   return order;
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
   // If no tracking capabilities are activated, turn off the sensor. If, on the other hand, certain
   // tracking capabilities are set, and tracking is available, configure the sensor appropriately.
   if (!_enabledTrackingCapabilities)
      ovrHmd_StopSensor(_descriptor.Handle);
   else
   {
      if (!ovrHmd_StartSensor(_descriptor.Handle, supportedTrackingCapabilities(), _enabledTrackingCapabilities))
         qFatal("[QtStereoscopy] Error: Could not initialize the device's tracking sensor.");
   }
}
