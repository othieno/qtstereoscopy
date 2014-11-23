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
#include "qoculusrift_test.h"
#include "QOculusRift"


void
QOculusRiftTest::testDebugDeviceInitialState()
{
   QTest::ignoreMessage(QtWarningMsg, "[QtStereoscopy] Warning: Created a debug device. Certain features may not be available.");
   QOculusRift device(0, true);
   QVERIFY(device == device.handle());

   QCOMPARE(device.isDebugDevice(), true);
   QCOMPARE(device.type(), ovrHmd_Other);

   QCOMPARE(device.productName(), QString("Oculus Rift (Debug Device)"));
   QCOMPARE(device.manufacturerName(), QString("Oculus VR"));

   QCOMPARE(device.vendorId(), short(0));
   QCOMPARE(device.productId(), short(0));
   QCOMPARE(device.serialNumber(), QString("N/A"));

   QCOMPARE(device.majorFirmwareVersion(), short(0));
   QCOMPARE(device.minorFirmwareVersion(), short(0));

   QCOMPARE(device.resolution(), QSize(1280, 800));
   QCOMPARE(device.eyeResolution(), QSize(640, 400));
   QCOMPARE(device.aspectRatio(), 1.6f);
   QCOMPARE(device.refreshRate(), 60u);

   QCOMPARE(device.vsyncEnabled(), true);
   QCOMPARE(device.yawCorrectionEnabled(), false);
   QCOMPARE(device.lowPersistenceEnabled(), false);
   QCOMPARE(device.latencyTestingEnabled(), false);
   QCOMPARE(device.dynamicPredictionEnabled(), false);
}


void
QOculusRiftTest::testDebugDeviceTracking()
{
   QTest::ignoreMessage(QtWarningMsg, "[QtStereoscopy] Warning: Created a debug device. Certain features may not be available.");
   QOculusRift device(0, true);

   QCOMPARE(device.trackingAvailable(), false);
   QCOMPARE(device.trackingCameraAvailable(), false);

   QCOMPARE(device.orientationTrackingAvailable(), false);
   QCOMPARE(device.orientationTrackingEnabled(), false);
   device.enableOrientationTracking(true);
   QCOMPARE(device.orientationTrackingEnabled(), false);
   QCOMPARE(device.headOrientation(), QQuaternion(1.0, 0.0, 0.0, 0.0));

   QCOMPARE(device.positionalTrackingAvailable(), false);
   QCOMPARE(device.positionalTrackingEnabled(), false);
   device.enablePositionalTracking(true);
   QCOMPARE(device.positionalTrackingEnabled(), false);
   QCOMPARE(device.headPosition(), QVector3D(0.0, 0.0, 0.0));

   QCOMPARE(device.eyeTrackingAvailable(), false);
   QCOMPARE(device.eyeTrackingEnabled(), false);
   device.enableEyeTracking(true);
   QCOMPARE(device.eyeTrackingEnabled(), false);
   QCOMPARE(device.eyePosition(), QPointF(0, 0));
}


void
QOculusRiftTest::testDebugDeviceMeasurements()
{
   QTest::ignoreMessage(QtWarningMsg, "[QtStereoscopy] Warning: Created a debug device. Certain features may not be available.");
   QOculusRift device(0, true);

   QCOMPARE(device.interpupillaryDistance(), OVR_DEFAULT_IPD);
   QCOMPARE(device.eyeHeight(), OVR_DEFAULT_EYE_HEIGHT);
}


void
QOculusRiftTest::testHardwareDependentCapabilities()
{
   QTest::ignoreMessage(QtWarningMsg, "[QtStereoscopy] Warning: Created a debug device. Certain features may not be available.");
   QOculusRift device(0, true);

   // None of these capabilities should be enabled for the debug device.
   device.enableYawCorrection();
   QCOMPARE(device.yawCorrectionEnabled(), false);

   device.enableLowPersistence();
   QCOMPARE(device.lowPersistenceEnabled(), false);

   device.enableLatencyTesting();
   QCOMPARE(device.latencyTestingEnabled(), false);

   device.enableDynamicPrediction();
   QCOMPARE(device.dynamicPredictionEnabled(), false);
}
