#ifndef FZYY_H
#define FZYY_H

#include <QFrame>
#include <QSpinBox>
#include <QLabel>
#include <QToolButton>
#include "tableGlyfLoca.h"
#include "../qtJiemianKuozhan/spineditex.h"

class ToolButtonEx:public QToolButton{
public:
    ToolButtonEx(QWidget *parent, int vpianyi, QColor wenziColor, GLYFTable* vGlyf);
    void huazifu(int32 glyphIndex);
    void jisuanSuofang(uint16 vwinAscent, uint16 vwinDscent);
    void setText(QString x);
    void setGlyf(GLYFTable *vglyf);
    void clear();
private:
    void paintEvent(QPaintEvent *event)override;
    QPainterPath zfpp;
    double toPingmuY(int16 ttfY, int x, int y);
    double toPingmuX(int16 ttfX, int x, int y);
    double suofang;
    uint16 winAscent;
    uint16 winDscent;
    QString btnText;
    int pianyi;
    QColor wenziColor;
    GLYFTable* pGlyf;
};

class Fzyy : public QFrame
{
    Q_OBJECT
    ToolButtonEx *yyidx;
    QLabel *lb1;
    SpinEditEx *offx;
    QLabel *lb2;
    SpinEditEx *offy;
    QLabel *lb4;
    QLabel *lb5;
    QLabel *lb6;
    QLabel *lb7;
    DoubleSpinEditEx *xs;
    DoubleSpinEditEx *ys;
    DoubleSpinEditEx *sf01;
    DoubleSpinEditEx *sf10;
    QLabel *lb3;
    QToolButton *btn;
    QToolButton *btnJian;
    int currentIndex;
    GLYFTable* pGlyf;
public:
    explicit Fzyy(QWidget *parent, int idx, GLYFTable* glyf, uint16 usWinAscent, uint16 usWinDescent);
    /// 填表。填写fzcs
    void tb(const BujianCanshu &fzcs, int curentI);
    /// 隐藏+按钮
    void hidebtn();
    void ychm();
    static int getHeight();
    void updateBujianIdx(uint32 bujianGlyphIndex);
    /// 整体移动时，修改部件的x坐标
    bool updateX(int delta);
    /// 整体移动时，修改部件的y坐标
    bool updateY(int delta);
signals:
    void bujianGaibian(int vbujianIdx,BujianGaibian bg,int xy=0,double scale=0);
private slots:
    void xuanzeBujianZhunbei();
    void jiaclicked();
    void jianclicked();
    /// 编辑修改y偏移
    void yChanged(int y);
    /// 编辑修改x偏移
    void xChanged(int x);
    /// 编辑修改缩放参数
    void scaleChanged(int bianjiBiaoji, double newvalue);
};

#endif // FZYY_H
