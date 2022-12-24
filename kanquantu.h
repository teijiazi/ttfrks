#ifndef KANQUANTU_H
#define KANQUANTU_H

#include <QGraphicsView>

class Kanquantu : public QGraphicsView
{
    Q_OBJECT
public:
    explicit Kanquantu(QWidget *parent);
    void resizeEvent(QResizeEvent *se) override;
signals:

};

#endif // KANQUANTU_H
