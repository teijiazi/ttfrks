#include "kanquantu.h"
#include "../rizhi/rizhi.h"

Kanquantu::Kanquantu(QWidget *parent) : QGraphicsView(parent)
{
    setGeometry(0,0,parent->width(),parent->height());
}

void Kanquantu::resizeEvent(QResizeEvent *se)
{
    Q_UNUSED(se);
    if(this->scene()){
        this->fitInView(0,0,
                        this->scene()->width(),
                        this->scene()->height(),
                        Qt::KeepAspectRatio);
    }
}
