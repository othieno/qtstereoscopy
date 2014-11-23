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
#ifndef QOCULUSRIFT_P_H
#define QOCULUSRIFT_P_H

#include "qoculusrift.h"
#include <atomic>


QT_BEGIN_NAMESPACE

class QOculusRiftPrivate : public QObject
{
public:
   QOculusRiftPrivate(QOculusRift* const parent, const unsigned int& index, const bool& forceDebugDevice);
   ~QOculusRiftPrivate();

   const bool& isDebugDevice() const;
   const ovrHmd& handle() const;
   const ovrHmdDesc& descriptor() const;

   bool trackingAvailable() const;
   void resetTracking();

   bool isCapEnabled(const unsigned int& capability) const;
   void setCapEnabled(const unsigned int& capability, const bool enable);
private:
   bool isHmdCap(const unsigned int& capability) const;
   void updateHmdCaps();

   bool isTrackingCap(const unsigned int& capability) const;
   void updateTrackingCaps();

   static std::atomic<unsigned int> N_DEVICE_INSTANCES;

   ovrHmdDesc _descriptor;
   bool _isDebugDevice;
   struct
   {
      unsigned int hmd;
      unsigned int tracking;
   } _enabledCaps;
};

QT_END_NAMESPACE

#endif // QOCULUSRIFT_P_H
