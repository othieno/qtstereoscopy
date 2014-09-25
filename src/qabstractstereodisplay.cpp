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
#include "qabstractstereodisplay.h"


QSize
QAbstractStereoDisplay::eyeResolution() const
{
   return QSize(resolution().width() / 2, resolution().height() / 2);
}


float
QAbstractStereoDisplay::aspectRatio() const
{
   return static_cast<float>(resolution().width()) / static_cast<float>(resolution().height());
}


QPointF
QAbstractStereoDisplay::normalizedEyePosition() const
{
   qreal x = 0;
   qreal y = 0;

   if (eyeTrackingEnabled())
   {
      x = static_cast<qreal>(eyePosition().x()) / static_cast<qreal>(resolution().width());
      y = static_cast<qreal>(eyePosition().y()) / static_cast<qreal>(resolution().height());
   }
   return QPointF(x, y);
}
