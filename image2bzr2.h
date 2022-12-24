#ifndef IMAGE2BZR2_H
#define IMAGE2BZR2_H

#include <QObject>
#include "tableGlyfLoca.h"

#include "opencv2/imgproc.hpp"
#include "opencv2/imgcodecs.hpp"

#include "../rizhi/rizhi.h"
using namespace std;
using namespace cv;
class Dian
{
public:
    Dian(int vx=0,int vy=0):x(vx),y(vy){}
    int x;
    int y;
};

class BaocunCanshu{
public:
    BaocunCanshu():imgpath("E:/文字"),
        isGaosi(false),
        isFanse(false),
        menxian(25),
        moshi(RETR_LIST),
        nihe(CHAIN_APPROX_SIMPLE),
        lvbo(1),//(2*lvbo+1)=3为初始值
        cl(3),
        guaidian(1.29),
        zhixian(0.50){}
    QString imgpath;
    bool isGaosi;
    bool isFanse;
    int menxian;
    int moshi;
    int nihe;
    uint lvbo;//(2*lvbo+1)>1为有效值
    double cl;
    double guaidian;
    double zhixian;
};
class Image2Bzr2 : public QObject
{
    Q_OBJECT
public:
    explicit Image2Bzr2(QObject *parent = nullptr);
    void getLKfrom(QString &tpFilePath, uint16 ascent);
    void xiugaiCanshu(Biji &biji, int &lks,
                      bool isGaosi, bool isFanse, int menxian, double vzhixian, uint lvbo, double guaidian, bool isFenduan, int nihe=-1);
    BaocunCanshu bc;
    void lisandian2Bezier(std::vector<std::vector<Dian> > &contours, Biji &biji);
    double juliB2AC(const Dian &a, const Dian &b, const Dian &c);
    void fenduan(vector<Dian> &bianjiedian, u16vector &tezhengdian);
    double juliAB(const Dian &a, const Dian &b);
    void zuoquxian(const vector<Dian> &bianjiedian, const u16vector &tezhengdian, Biji &biji);
    void piliangShiyong(Biji &biji, int &lks);
private:
    Mat yuanshiHeibaitu;
    uint16 usWinAscent;
    bool xianshifenduan;
signals:

};

#endif // IMAGE2BZR2_H
