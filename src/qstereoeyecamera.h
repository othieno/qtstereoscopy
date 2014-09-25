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
#ifndef QSTEREOEYECAMERA_H
#define QSTEREOEYECAMERA_H

#include <QtGui/QVector3D>
#include <QtGui/QMatrix4x4>


QT_BEGIN_NAMESPACE

struct QStereoEyeCamera
{
   QStereoEyeCamera();

   bool projectionChanged() const;
   void setProjectionChanged(const bool changed);

   QMatrix4x4 view;
   QMatrix4x4 perspective;
   QMatrix4x4 ortho;

   static float nearClippingPlane;
   static float farClippingPlane;

   float orthoDistance;
   QVector3D viewAdjust;
   QRect viewport;
private:
   bool _hasProjectionChanged;
};

QT_END_NAMESPACE

#endif // QSTEREOEYECAMERA_H
