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
std::atomic<unsigned int> QOculusRiftPrivate::N_DEVICE_INSTANCES(0);


QOculusRiftPrivate::QOculusRiftPrivate
(
   QOculusRift* const parent,
   const unsigned int& index,
   const bool& forceDebugDevice
) :
QObject(parent),
_isDebugDevice(forceDebugDevice),
_enabledCaps({0, 0})
{
   // Initialize LibOVR iff this is the first instance.
   if (!N_DEVICE_INSTANCES++)
   {
      if (Q_UNLIKELY(!ovr_Initialize()))
         qFatal("[QtStereoscopy] Error: Could not initialize LibOVR!");
   }

   // Make sure the device index is valid, i.e. if it is greater than zero, it must
   // be strictly less than the value returned by ovrHmd_Detect.
   if (index && index >= static_cast<unsigned int>(ovrHmd_Detect()))
      qFatal("[QtStereoscopy] Error: Could not find a device with index '%d'.", index);

   // Instantiate an HMD device. If no hardware is detected, create a debug device.
   ovrHmd deviceHandle = forceDebugDevice ? nullptr : ovrHmd_Create(index);

   _isDebugDevice = (deviceHandle == nullptr);
   if (_isDebugDevice)
   {
      deviceHandle = ovrHmd_CreateDebug(ovrHmd_DK1);
      if (Q_UNLIKELY(deviceHandle == nullptr))
         qFatal("[QtStereoscopy] Error: Could not open any Oculus Rift devices.");
      else
         qWarning("[QtStereoscopy] Warning: Created a debug device. Certain features may not be available.");
   }

   // Populate the descriptor. If this is a debug device, then remove all tracking capabilities.
   ovrHmd_GetDesc(deviceHandle, const_cast<ovrHmdDesc*>(&_descriptor));
   if (_isDebugDevice)
      const_cast<unsigned int&>(_descriptor.SensorCaps) = 0;

// TODO Check if the following is necessary in SDK 0.4+ and remove if necessary.
#ifndef DISABLE_IN_SDK_0_4
   // Disable positional tracking and yaw correction for the DK1.
   if (trackingAvailable() && _descriptor.Type == ovrHmd_DK1)
   {
      auto& caps = const_cast<unsigned int&>(_descriptor.SensorCaps);
      caps = caps & ~ovrSensorCap_Position & ~ovrSensorCap_YawCorrection;
   }
#endif
   // Enable all HMD capabilities, but do not disable VSYNC!
   _enabledCaps.hmd = _descriptor.HmdCaps & ~ovrHmdCap_NoVSync;
   updateHmdCaps();

   _enabledCaps.tracking = _descriptor.SensorCaps;
   updateTrackingCaps();
}


QOculusRiftPrivate::~QOculusRiftPrivate()
{
   // Destroy the device.
   ovrHmd_Destroy(_descriptor.Handle);

   // Shutdown LibOVR iff this is the last device instance.
   if (!--N_DEVICE_INSTANCES)
      ovr_Shutdown();
}


const bool&
QOculusRiftPrivate::isDebugDevice() const
{
   return _isDebugDevice;
}


const ovrHmd&
QOculusRiftPrivate::handle() const
{
   return _descriptor.Handle;
}


const ovrHmdDesc&
QOculusRiftPrivate::descriptor() const
{
   return _descriptor;
}


bool
QOculusRiftPrivate::trackingAvailable() const
{
   if (isDebugDevice())
      return false;
   else
   {
      qCritical("[QtStereoscopy] Implement QOculusRift::trackingAvailable.");
      return true;
   }
}


void
QOculusRiftPrivate::resetTracking()
{
   if (trackingAvailable())
      ovrHmd_ResetSensor(handle());
}


bool
QOculusRiftPrivate::isCapEnabled(const unsigned int& cap) const
{
   return
   isHmdCap(cap)      ? (_enabledCaps.hmd & cap) == cap      :
   isTrackingCap(cap) ? (_enabledCaps.tracking & cap) == cap : false;
}


void
QOculusRiftPrivate::setCapEnabled(const unsigned int& cap, const bool enable)
{
   if (enable != isCapEnabled(cap))
   {
      const auto& writeBit = [this, &cap, &enable](unsigned int& bitField)
      {
         if (enable)
            bitField |= cap;
         else
            bitField &= ~cap;
      };

      if (isHmdCap(cap))
      {
         writeBit(_enabledCaps.hmd);
         updateHmdCaps();
      }
      else if (isTrackingCap(cap))
      {
         writeBit(_enabledCaps.tracking);
         updateTrackingCaps();
      }
   }
}


bool
QOculusRiftPrivate::isHmdCap(const unsigned int& cap) const
{
   switch (cap)
   {
      case ovrHmdCap_NoVSync:
      case ovrHmdCap_LowPersistence:
      case ovrHmdCap_LatencyTest:
      case ovrHmdCap_DynamicPrediction:
         return true;
      default:
         return false;
   }
}


void
QOculusRiftPrivate::updateHmdCaps()
{
   ovrHmd_SetEnabledCaps(_descriptor.Handle, _enabledCaps.hmd);
}


bool
QOculusRiftPrivate::isTrackingCap(const unsigned int& cap) const
{
   switch (cap)
   {
      case ovrSensorCap_Orientation:
      case ovrSensorCap_Position:
      case ovrSensorCap_YawCorrection:
         return true;
      default:
         return false;
   }
}


void
QOculusRiftPrivate::updateTrackingCaps()
{
   // If no tracking capabilities are activated, turn off the sensor. If, on the other hand, certain
   // tracking capabilities are set, and tracking is available, configure the sensor appropriately.
   if (trackingAvailable())
   {
      if (!_enabledCaps.tracking)
         ovrHmd_StopSensor(_descriptor.Handle);
      else
      {
         if (!ovrHmd_StartSensor(_descriptor.Handle, _descriptor.SensorCaps, _enabledCaps.tracking))
            qFatal("[QtStereoscopy] Error: Could not initialize the device's tracking sensors.");
      }
   }
}
