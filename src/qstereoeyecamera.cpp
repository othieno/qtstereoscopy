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
#include "qstereoeyecamera.h"

float QStereoEyeCamera::nearClippingPlane = 0.01f;
float QStereoEyeCamera::farClippingPlane = 10000.0f;


QStereoEyeCamera::QStereoEyeCamera() :
orthoDistance(0.8f),
_hasProjectionChanged(true)
{}








/**
 * @brief Sets the viewing frustum's clipping plane distances.
void
QOculusRiftStereoRenderer::setClippingPlaneDistances(const float* const near, const float* const far)
{
   bool projectionChanged = false;

   if (near != nullptr && !qFuzzyCompare(_clippingPlaneDistance.near, *near))
   {
      _clippingPlaneDistance.near = *near;
      projectionChanged = true;
   }
   if (far != nullptr && !qFuzzyCompare(_clippingPlaneDistance.far, *far))
   {
      _clippingPlaneDistance.far = *far;
      projectionChanged = true;
   }

   // Mark both projection transform configurations as dirty.
   if (projectionChanged)
      _dirty.projection.fill(true);
}
*/



bool
QStereoEyeCamera::projectionChanged() const
{
   return _hasProjectionChanged;
}


void
QStereoEyeCamera::setProjectionChanged(const bool changed)
{
   _hasProjectionChanged = changed;
}
