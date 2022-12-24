#ifndef TUPIANBIANBIJI_H
#define TUPIANBIANBIJI_H

#include <QObject>
#include "types.h"

class TupianBianBiji : public QObject
{
    Q_OBJECT
public:
    explicit TupianBianBiji(QObject *parent = nullptr);

    void gsm();
signals:

};

#endif // TUPIANBIANBIJI_H
