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
#include "qoculusriftrenderer_test.h"
#include "QOculusRiftRenderer"
#include "QStereoWindow"


void
QOculusRiftRendererTest::testDebugDeviceInitialState()
{
   QTest::ignoreMessage(QtWarningMsg, "[QtStereoscopy] Warning: Created a debug device. Certain features may not be available.");
   QOculusRiftRenderer renderer(0, true);

   QCOMPARE(renderer.const_display().isDebugDevice(), true);
   QCOMPARE(renderer.pixelDensity(), 1.0f);

   QCOMPARE(renderer.chromaticAberrationCorrectionEnabled(), true);
   QCOMPARE(renderer.timewarpEnabled(), true);
   QCOMPARE(renderer.vignetteEnabled(), true);
}


void
QOculusRiftRendererTest::testDebugDeviceWindow()
{
/*
   int argc = 0;
   char** argv = nullptr;

   QGuiApplication app(argc, argv);
   QStereoWindow<QOculusRiftRenderer> window;

   window.showNormal();

   app.exec();
*/
}


void
QOculusRiftRendererTest::testDebugDevicePixelDensity()
{
   QFETCH(float, actual);
   QFETCH(float, expected);

   QTest::ignoreMessage(QtWarningMsg, "[QtStereoscopy] Warning: Created a debug device. Certain features may not be available.");
   QOculusRiftRenderer renderer(0, true);

   renderer.setPixelDensity(actual);
   QCOMPARE(renderer.pixelDensity(), expected);
}


void
QOculusRiftRendererTest::testDebugDevicePixelDensity_data()
{
   constexpr float MIN_PIXEL_DENSITY = QOculusRiftRenderer::minPixelDensity();
   constexpr float MAX_PIXEL_DENSITY = QOculusRiftRenderer::maxPixelDensity();

   QTest::addColumn<float>("actual");
   QTest::addColumn<float>("expected");

   QTest::newRow("Minimum pixel density") << 0.0f << MIN_PIXEL_DENSITY;
   QTest::newRow("Default pixel density") << 1.0f << 1.0f;
   QTest::newRow("Maximum pixel density") << 8.0f << MAX_PIXEL_DENSITY;

   QTest::newRow("Small pixel density")   << 0.5f << 0.5f;
   QTest::newRow("Doubled pixel density") << 2.0f << 2.0f;
   QTest::newRow("Tripled pixel density") << 3.0f << 3.0f;

   QTest::newRow("Slightly smaller than minimum pixel density") << MIN_PIXEL_DENSITY - 0.05f << MIN_PIXEL_DENSITY;
   QTest::newRow("Slightly larger than maximum pixel density") << MAX_PIXEL_DENSITY + 0.05f << MAX_PIXEL_DENSITY;
}
