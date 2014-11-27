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
#include <QCoreApplication>
#include <QOculusRift>
#include <QTimer>
#include <iostream>
#include <iomanip>


std::ostream& operator<<(std::ostream&, const QVector3D&);
std::ostream& operator<<(std::ostream&, const QQuaternion&);


int main(int argc, char** argv)
{
   QCoreApplication application(argc, argv);

   const QOculusRift rift;
   const auto& trackingAvailable = rift.trackingAvailable();

   std::cout
   << std::boolalpha
   << "Interpupillary distance: " << rift.interpupillaryDistance() << "mm" << std::endl
   << "Low persistence enabled: " << rift.lowPersistenceEnabled() << std::endl
   << "Latency testing enabled: " << rift.latencyTestingEnabled() << std::endl
   << "Sensor tracking available: " << trackingAvailable << std::endl;

   if (!trackingAvailable)
   {
      qWarning("[QtStereoscopy] Warning: No available tracking sensors. Closing application...");
      return 0;
   }
   else
   {
      std::cout
      << std::setiosflags(std::ios::fixed)
      << std::setprecision(3)
      << "Tracking camera available: " << rift.trackingCameraAvailable() << std::endl
      << "Orientation tracking enabled: " << rift.orientationTrackingEnabled() << std::endl
      << "Magnetic yaw correction enabled: " << rift.yawCorrectionEnabled() << std::endl
      << "Positional tracking enabled: " << rift.positionalTrackingEnabled() << std::endl
      << "Left eye tracking enabled: " << rift.eyeTrackingEnabled(QEye::Left) << std::endl
      << "Right eye tracking enabled: " << rift.eyeTrackingEnabled(QEye::Right) << std::endl;

      QTimer updateTimer;
      QCoreApplication::connect(&updateTimer, &QTimer::timeout, [&rift]()
      {
         std::cout
         << "Position: " << rift.headPosition()
         << "\t\t"
         << "Orientation: " << rift.headOrientation()
         << std::endl;
      });
      updateTimer.start(300);

      return application.exec();
   }
}


std::ostream&
operator<<(std::ostream& os, const QVector3D& v)
{
   os << "(" << std::setw(8) << v.x() << ", " << std::setw(8) << v.y() << ", " << std::setw(8) << v.z() << ")";
   return os;
}


std::ostream&
operator<<(std::ostream& os, const QQuaternion& q)
{
   os << "(" << q.vector() << ", " << std::setw(8) << q.scalar() << ")";
   return os;
}
