#include "arctobazer.h"

arctoBazer::arctoBazer(QObject *parent) : QObject(parent)
{

}




void arctoBazer::arc_to_bezier(double cx, double cy, double rx, double ry,
                   double start_angle, double sweep_angle,
                   double* curve){
    double x0 = cos(sweep_angle / 2.0);
    double y0 = sin(sweep_angle / 2.0);
    double tx = (1.0 - x0) * 4.0 / 3.0;
    double ty = y0 - tx * x0 / y0;
    double px[4];
    double py[4];
    px[0] =  x0;
    py[0] = -y0;
    px[1] =  x0 + tx;
    py[1] = -ty;
    px[2] =  x0 + tx;
    py[2] =  ty;
    px[3] =  x0;
    py[3] =  y0;
    double sn = sin(start_angle + sweep_angle / 2.0);
    double cs = cos(start_angle + sweep_angle / 2.0);
    unsigned i;
    for(i = 0; i < 4; i++)
    {
        curve[i * 2]     = cx + rx * (px[i] * cs - py[i] * sn);
        curve[i * 2 + 1] = cy + ry * (px[i] * sn + py[i] * cs);
    }
}


