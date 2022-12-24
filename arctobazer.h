#ifndef ARCTOBAZER_H
#define ARCTOBAZER_H

#include <QObject>
#include "types.h"

class arctoBazer : public QObject
{
    Q_OBJECT
public:
    explicit arctoBazer(QObject *parent = nullptr);

    void arc_to_bezier(double cx, double cy, double rx, double ry, double start_angle, double sweep_angle, double *curve);
signals:

};

#endif // ARCTOBAZER_H
