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
#ifndef QABSTRACTSTEREODISPLAY_H
#define QABSTRACTSTEREODISPLAY_H

#include <QtCore/QObject>
#include <QtCore/QSize>
#include <QtCore/QString>
#include <QtGui/QQuaternion>
#include "qeye.h"


QT_BEGIN_NAMESPACE

class QAbstractStereoDisplay : public QObject
{
public:
   virtual QString productName() const = 0;
   virtual QString manufacturerName() const = 0;

   virtual QSize resolution() const = 0;
   virtual QSize eyeResolution(const QEye& eye) const;
           QSize halfResolution() const;
           float aspectRatio() const;
   virtual unsigned int refreshRate() const = 0;

   virtual bool vsyncEnabled() const = 0;
   virtual void enableVsync(const bool enable) = 0;

   virtual float interpupillaryDistance() const = 0;
   virtual void setInterpupillaryDistance(const float& distance) = 0;

   virtual float eyeHeight() const = 0;
   virtual void setEyeHeight(const float& height) = 0;

   virtual bool trackingAvailable() const = 0;

   virtual bool orientationTrackingAvailable() const = 0;
   virtual bool orientationTrackingEnabled() const = 0;
   virtual void enableOrientationTracking(const bool enable) = 0;
   virtual QQuaternion headOrientation() const = 0;

   virtual bool positionalTrackingAvailable() const = 0;
   virtual bool positionalTrackingEnabled() const = 0;
   virtual void enablePositionalTracking(const bool enable) = 0;
   virtual QVector3D headPosition() const = 0;

   virtual bool eyeTrackingAvailable() const = 0;
   virtual bool eyeTrackingEnabled(const QEye& eye) const = 0;
           bool eyeTrackingEnabled() const;
   virtual void enableEyeTracking(const QEye& eye, const bool enable) = 0;
           void enableEyeTracking(const bool enable);
   virtual QPointF gazePoint(const QEye& eye) const = 0;
};

QT_END_NAMESPACE

#endif // QABSTRACTSTEREODISPLAY_H
