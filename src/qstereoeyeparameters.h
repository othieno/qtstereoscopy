#ifndef QSTEREOEYEPARAMETERS_H
#define QSTEREOEYEPARAMETERS_H

#include <QObject>
#include "qeye.h"


QT_BEGIN_NAMESPACE

class QStereoEyeParametersPrivate;
class QStereoEyeParameters : QObject
{
public:
   QStereoEyeParameters();

   const QEye& eye() const;
   void setEye(const QEye& eye);

   const QPointF& gazePoint() const;
   void setGazePoint(const QPointF& point);

   const QQuaternion& headOrientation() const;
   void setHeadOrientation(const QQuaternion& orientation);

   const QVector3D& headPosition() const;
   void setHeadPosition(const QVector3D& position);

   const QMatrix4x4& view() const;
   void setView(const QMatrix4x4& view);

   const QVector3D& viewAdjust() const;
   void setViewAdjust(const QVector3D& adjust);

   const QRect& viewport() const;
   void setViewport(const QRect& viewport);

   const QMatrix4x4& perspective() const;
   void setPerspective(const QMatrix4x4& perspective);

   const QMatrix4x4& ortho() const;
   void setOrtho(const QMatrix4x4& ortho);

   static const float& orthoDistance();
   static void setOrthoDistance(const float& distance);

   static const float& nearClippingDistance();
   static void setNearClippingDistance(const float& distance);

   static const float& farClippingDistance();
   static void setFarClippingDistance(const float& distance);
private:
   QStereoEyeParametersPrivate* const d_ptr;
   Q_DECLARE_PRIVATE(QStereoEyeParameters);
};

QT_END_NAMESPACE

#endif // QSTEREOEYEPARAMETERS_H
