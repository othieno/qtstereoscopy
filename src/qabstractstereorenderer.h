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
#ifndef QABSTRACTSTEREORENDERER_H
#define QABSTRACTSTEREORENDERER_H

#include <QtCore/QObject>
#include <QtGui/QOpenGLFunctions>
#include <QtGui/qwindowdefs.h>


QT_BEGIN_NAMESPACE

class QStereoEyeCamera;
template<class T> class QStereoWindow;

class QAbstractStereoRenderer : public QObject, protected QOpenGLFunctions
{
public:
   template<class T> void initialize(QStereoWindow<T>& window);

   virtual void apply() = 0;
   virtual bool autoSwapsBuffers() const;
           void setViewport(const QRect& viewport);
protected:
   QAbstractStereoRenderer() = default;

   virtual void initializeWindow(const WId& windowId);
   virtual void initializeGL() = 0;
   virtual void paintGL(const QStereoEyeCamera& camera, const float& dt) = 0;
};


template<class T> void
QAbstractStereoRenderer::initialize(QStereoWindow<T>& window)
{
   initializeOpenGLFunctions();
   initializeWindow(window.winId());
   initializeGL();
}

QT_END_NAMESPACE

#endif // QABSTRACTSTEREORENDERER_H
