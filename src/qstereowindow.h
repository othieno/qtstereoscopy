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
#ifndef QSTEREOWINDOW_H
#define QSTEREOWINDOW_H

#include <QtCore/QCoreApplication>
#include <QtGui/QOpenGLContext>
#include <QtGui/QWindow>


QT_BEGIN_NAMESPACE

class QAbstractStereoRenderer;

template<class Renderer>
class QStereoWindow Q_DECL_FINAL : public QWindow
{
   static_assert(std::is_base_of<QAbstractStereoRenderer, Renderer>::value, "Renderer must derive from QAbstractStereoRenderer.");
   static_assert(std::is_default_constructible<Renderer>::value, "Renderer requires a default constructor.");
public:
   explicit QStereoWindow(QStereoWindow* const parent = nullptr);
   explicit QStereoWindow(const QStereoWindow&) = delete;

   QStereoWindow(const QString& title, QStereoWindow* const parent = nullptr);
   QStereoWindow(Renderer& renderer, QStereoWindow* const parent = nullptr);
   QStereoWindow(Renderer& renderer, const QString& title, QStereoWindow* const parent = nullptr);

   QStereoWindow& operator=(const QStereoWindow&) = delete;
private:
   QStereoWindow(Renderer* const renderer, QStereoWindow* const parent);
   QStereoWindow(Renderer* const renderer, const QString& title, QStereoWindow* const parent);

   void update();
   void paintGL();

   bool event(QEvent* const e) Q_DECL_OVERRIDE;
   void exposeEvent(QExposeEvent* const e) Q_DECL_OVERRIDE;

   QOpenGLContext _context;
   Renderer* const _renderer;
   bool _updateRequestPending;
};


template<class T>
QStereoWindow<T>::QStereoWindow(QStereoWindow* const parent) :
QStereoWindow(new T, parent)
{
   // Change the renderer's ownership to manage dynamic memory automatically.
   _renderer->setParent(this);
}


template<class T>
QStereoWindow<T>::QStereoWindow(const QString& title, QStereoWindow* const parent) :
QStereoWindow(parent)
{
   setTitle(title);
}


template<class T>
QStereoWindow<T>::QStereoWindow(T& renderer, QStereoWindow* const parent) :
QStereoWindow(&renderer, parent)
{}


template<class T>
QStereoWindow<T>::QStereoWindow(T& renderer, const QString& title, QStereoWindow* const parent) :
QStereoWindow(&renderer, title, parent)
{}


template<class T>
QStereoWindow<T>::QStereoWindow(T* const renderer, QStereoWindow* const parent) :
_renderer(renderer),
_updateRequestPending(false)
{
   setSurfaceType(QWindow::OpenGLSurface);
   if (Q_UNLIKELY(!supportsOpenGL()))
      qFatal("[QtStereoscopy] Error: This system is not OpenGL compatible.");

   if (_renderer == nullptr)
      qFatal("[QtStereoscopy] Error: QStereoWindow renderer is null.");

   if (parent != nullptr)
      setParent(parent);
}


template<class T>
QStereoWindow<T>::QStereoWindow(T* const renderer, const QString& title, QStereoWindow* const parent) :
QStereoWindow(renderer, parent)
{
   setTitle(title);
}


template<class T> void
QStereoWindow<T>::update()
{
   if (!_updateRequestPending)
   {
      _updateRequestPending = true;
      QCoreApplication::postEvent(this, new QEvent(QEvent::UpdateRequest));
   }
}


template<class T> void
QStereoWindow<T>::paintGL()
{
   _renderer->apply();
   if (!_renderer->autoSwapsBuffers())
      _context.swapBuffers(this);
}


template<class T> bool
QStereoWindow<T>::event(QEvent* const e)
{
   if (e->type() == QEvent::UpdateRequest && _updateRequestPending && _context.makeCurrent(this))
   {
      _updateRequestPending = false;

      // Draw a new frame and make an update request (or else drawing ends after the call to paintGL).
      paintGL();
      update();
   }
   return QWindow::event(e);
}


template<class T> void
QStereoWindow<T>::exposeEvent(QExposeEvent* const e)
{
   // When the window is exposed the first time, its OpenGL context and renderer need to be initialized.
   if (isExposed() && !_context.isValid())
   {
      _context.setFormat(requestedFormat());
      if (_context.create() && _context.makeCurrent(this))
      {
         _renderer->initialize(*this);
         update();
      }
      else
         qFatal("[QtStereoscopy] Error: Could not create an OpenGL context.");
   }
   QWindow::exposeEvent(e);
}

QT_END_NAMESPACE

#endif // QSTEREOWINDOW_H
