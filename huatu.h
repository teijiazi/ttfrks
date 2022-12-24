#ifndef HUATU_H
#define HUATU_H

#include<types.h>
#include <QWidget>
#include <QPainter>
#include <QMouseEvent>
#include "tableGlyfLoca.h"
#include <QGestureEvent>
#include <QScrollArea>
#include <QScrollBar>

class huatu : public QWidget
{
    Q_OBJECT
public:
    explicit huatu(QWidget *parent=nullptr);
    void paintEvent(QPaintEvent *event)override;
    bool isTTFok;
    void setZuobiao(uint16	WinAscent,
                    uint16	WinDescent,
                    int16	xHeight,
                    int16	CapHeight, int fzzx, GLYFTable *vglyf=nullptr);
    //double toPingmuY(int16 ttfY){return suofangBeishu*(-ttfY+usWinAscent+kjy);}
    //double toPingmuX(int16 ttfX){return suofangBeishu*(ttfX+kjx);}
    double toPingmuY(int16 ttfY,int xx=0,int yy=0){
        Q_UNUSED(xx);
        Q_UNUSED(yy);
        return suofangBeishu*((-ttfY+usWinAscent)*shouciSuofang+kjy);}
    double toPingmuX(int16 ttfX,int xx=0,int yy=0){
        Q_UNUSED(xx);
        Q_UNUSED(yy);
        return suofangBeishu*(ttfX*shouciSuofang+kjx);}
    void kanlunkuo(lunkuoFanwei lfw,
                   int16 lsb, uint16 aw, int32 vglyphIndex, int xsHeight, QList<fuzhuxian>* fzx);
    void init();
    void mouseMoveEvent(QMouseEvent *event)override;
    void mousePressEvent(QMouseEvent * event)override;
    void mouseReleaseEvent(QMouseEvent * event)override;
    void wheelEvent(QWheelEvent *event)override;
    void keyPressEvent(QKeyEvent *event) override;
    void suofang(bool isFangda);
    void setScroll(QScrollArea *xsa);
    int16 zx;
    void showPoint(bool isShowPoint);
    void showLunkuoIndex(bool isShowLunkuoIdx);
    void updateFuzhuxian();
    bool huaZuobiao;
    lunkuoFanwei xuanzeBianjie; //!< 选择框的边界。删除时用来确定选择范围
    int lksel;                  //!< 选中的轮廓,-1表示没有选中
    bool xianshiDian;           //!< true=显示锚点 false=不显示
    void mouseDoubleClickEvent(QMouseEvent *event)override;
private:
    uint emSize;
    uint16	usWinAscent;
    int16	sWinDescent;
    int16	sxHeight;
    int16	sCapHeight;
    uint16	advanceWidth;
    int16	lsb;
    uint16 kjx;//!<屏幕坐标系
    uint16 kjy;//!<屏幕坐标系
    uint ox;
    uint oy;
    double	pmWinAscent;
    double	pmWinDescent;
    double	pmxHeight;
    double	pmCapHeight;
    double   pmzx;
    double   pmBaseline;
    double   pmOx;
    double   pmOy;
    void setBhZuobiao();
    void huajizhunxian(QPainter &hh, QPen &hb);
    void huazifu(QPainter &hh,QPen&hb);
    int toTTFy(double pingmuY){return usWinAscent-((pingmuY/suofangBeishu)-kjy)/shouciSuofang;}
    int toTTFx(double pingmuX){return ((pingmuX/suofangBeishu)-kjx)/shouciSuofang;}
    int16 ttfxMin;
    int16 ttfxMax;
    int16 ttfyMin;
    int16 ttfyMax;
    int16 ttflsb;
    uint16 ttfaw;
    double pmxMin;
    double pmxMax;
    double pmyMin;
    double pmyMax;
    double pmPP1;//!<lsb线的x坐标
    double pmPP2;//!<aw线的x坐标
    QList<fuzhuxian>*zfFzx;
    QPainterPath fzxpp;
    QPainterPath zfpp;//!<一个字符的全部轮廓
    LPP lkppList;     //!<列表：每个单独的轮廓
    LPP zfddList;     //!<列表：每个单独的轮廓上的点
    LPP lkhPPList;    //!<轮廓序号
    double suofangBeishu;
    double shouciSuofang;
    void setBhZifu();
    QScrollArea *sa;
    void huaZuoyoujianju(QPainter &hh, QPen &hb);
    bool xianshiLunkuohao;
    //选择
    bool zaihuaxian;//!<true=正在拖拽画框
    QPointF xuanzheBegin;
    QSizeF xuanzeSize;
    int32 glyphIndex;
    GLYFTable* pGlyf;
signals:
    void showMousePos(int ttfX,int ttfY);
    void updateBfb(QString suofangBeishu);
    void tsx(int fuzhuxianIdx);
    void updataMaodianUI(bool xianshidian);
};

#endif // HUATU_H
