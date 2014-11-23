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

   QStereoWindow(const QString& title, QStereoWindow* const parent = nullptr);
   QStereoWindow(Renderer& renderer, QStereoWindow* const parent = nullptr);
   QStereoWindow(Renderer& renderer, const QString& title, QStereoWindow* const parent = nullptr);

   QOpenGLContext& context();
   Renderer& renderer();
private:
   QStereoWindow(Renderer* const renderer, QStereoWindow* const parent);
   QStereoWindow(Renderer* const renderer, const QString& title, QStereoWindow* const parent);

   explicit QStereoWindow(const QStereoWindow&) = delete;
   QStereoWindow& operator=(const QStereoWindow&) = delete;

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


template<class T> QOpenGLContext&
QStereoWindow<T>::context()
{
   return _context;
}


template<class T> T&
QStereoWindow<T>::renderer()
{
   return *_renderer;
}


template<class T> void
QStereoWindow<T>::update()
{
   // Post an update request event in the event pool. Since overflowing the
   // pool is a bad idea, coalesce update requests by adding the aforementioned
   // event iff no prior events of the same kind are pending.
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
   _renderer->swapBuffers(_context, *this);
}


template<class T> bool
QStereoWindow<T>::event(QEvent* const e)
{
   // When an update request is received, this means it is time to draw a new frame. Perform said
   // action and make another update request (or else no more draw calls are performed.)
   if (e->type() == QEvent::UpdateRequest && _context.makeCurrent(this))
   {
      _updateRequestPending = false; // The pending request is being handled.
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
