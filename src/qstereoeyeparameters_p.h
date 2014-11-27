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
#ifndef QSTEREOEYEPARAMETERS_P_H
#define QSTEREOEYEPARAMETERS_P_H

#include "qstereoeyeparameters.h"
#include "qeye.h"
#include <QPointF>
#include <QMatrix4x4>


QT_BEGIN_NAMESPACE

struct QStereoEyeParametersPrivate : public QObject
{
public:
   explicit QStereoEyeParametersPrivate(QStereoEyeParameters* const parent);

   QEye eye;
   QRect viewport;
   QPointF gazePoint;
   QVector3D viewAdjust;
   QVector3D headPosition;
   QQuaternion headOrientation;

   QMatrix4x4 view;
   QMatrix4x4 ortho;
   QMatrix4x4 perspective;

   static float ORTHO_DISTANCE;
   static float NEAR_CLIPPING_DISTANCE;
   static float FAR_CLIPPING_DISTANCE;
};

QT_END_NAMESPACE

#endif // QSTEREOEYEPARAMETERS_P_H
