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
#include "qstereoeyeparameters_p.h"


QStereoEyeParameters::QStereoEyeParameters() :
d_ptr(new QStereoEyeParametersPrivate(this))
{}


const QEye&
QStereoEyeParameters::eye() const
{
   Q_D(const QStereoEyeParameters);
   return d->eye;
}


void
QStereoEyeParameters::setEye(const QEye& eye)
{
   Q_D(QStereoEyeParameters);
   d->eye = eye;
}


const QPointF&
QStereoEyeParameters::gazePoint() const
{
   Q_D(const QStereoEyeParameters);
   return d->gazePoint;
}


void
QStereoEyeParameters::setGazePoint(const QPointF& point)
{
   Q_D(QStereoEyeParameters);
   d->gazePoint = point;
}


const QQuaternion&
QStereoEyeParameters::headOrientation() const
{
   Q_D(const QStereoEyeParameters);
   return d->headOrientation;
}


void
QStereoEyeParameters::setHeadOrientation(const QQuaternion& orientation)
{
   Q_D(QStereoEyeParameters);
   d->headOrientation = orientation;
}


const QVector3D&
QStereoEyeParameters::headPosition() const
{
   Q_D(const QStereoEyeParameters);
   return d->headPosition;
}


void
QStereoEyeParameters::setHeadPosition(const QVector3D& position)
{
   Q_D(QStereoEyeParameters);
   d->headPosition = position;
}


const QMatrix4x4&
QStereoEyeParameters::view() const
{
   Q_D(const QStereoEyeParameters);
   return d->view;
}


void
QStereoEyeParameters::setView(const QMatrix4x4& view)
{
   Q_D(QStereoEyeParameters);
   d->view = view;
}


const QVector3D&
QStereoEyeParameters::viewAdjust() const
{
   Q_D(const QStereoEyeParameters);
   return d->viewAdjust;
}


void
QStereoEyeParameters::setViewAdjust(const QVector3D& adjust)
{
   Q_D(QStereoEyeParameters);
   d->viewAdjust = adjust;
}


const QRect&
QStereoEyeParameters::viewport() const
{
   Q_D(const QStereoEyeParameters);
   return d->viewport;
}


void
QStereoEyeParameters::setViewport(const QRect& viewport)
{
   Q_D(QStereoEyeParameters);
   d->viewport = viewport;
}


const QMatrix4x4&
QStereoEyeParameters::perspective() const
{
   Q_D(const QStereoEyeParameters);
   return d->perspective;
}


void
QStereoEyeParameters::setPerspective(const QMatrix4x4& perspective)
{
   Q_D(QStereoEyeParameters);
   d->perspective = perspective;
}


const QMatrix4x4&
QStereoEyeParameters::ortho() const
{
   Q_D(const QStereoEyeParameters);
   return d->ortho;
}


void
QStereoEyeParameters::setOrtho(const QMatrix4x4& ortho)
{
   Q_D(QStereoEyeParameters);
   d->ortho = ortho;
}


const float&
QStereoEyeParameters::orthoDistance()
{
   return QStereoEyeParametersPrivate::ORTHO_DISTANCE;
}


void
QStereoEyeParameters::setOrthoDistance(const float& distance)
{
   QStereoEyeParametersPrivate::ORTHO_DISTANCE = distance;
}


const float&
QStereoEyeParameters::nearClippingDistance()
{
   return QStereoEyeParametersPrivate::NEAR_CLIPPING_DISTANCE;
}


void
QStereoEyeParameters::setNearClippingDistance(const float& distance)
{
   QStereoEyeParametersPrivate::NEAR_CLIPPING_DISTANCE = distance;
}


const float&
QStereoEyeParameters::farClippingDistance()
{
   return QStereoEyeParametersPrivate::FAR_CLIPPING_DISTANCE;
}


void
QStereoEyeParameters::setFarClippingDistance(const float& distance)
{
   QStereoEyeParametersPrivate::FAR_CLIPPING_DISTANCE = distance;
}
