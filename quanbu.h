#ifndef QUANBU_H
#define QUANBU_H

#include<types.h>
#include <QWidget>
#include <QPainter>
#include "tableCmap.h"
#include "tableGlyfLoca.h"
#include <QScrollArea>
#include <QScrollBar>
#include <QMouseEvent>

class FzPara{
public:
    bool xuanBujian;  //true 正在选部件
    int bujianIdx;    //需要替换的部件的索引，如一个组合字需要3个部件，0,1,2...
    int32 zhGlyphIdx; //组合字的glphIdx
};

enum shanchuXuanze{
    sxOK,//可以删
    sxFileNotOpen,
    sxNoSelect,
    sxMAX
};

const QString shanchuXuanzeStr[sxMAX]={
    "","字体文件未打开","没有选中"
};

enum xiugaiGongxiang{
    xgOk,
    xgMeiyouxuan,
    xgDuoxuan,
    xgZhubianma,
    xgYicunzai,
    xgWuxiao,
    xgMAX
};

const QString xiugaiGongxiangStr[xgMAX]={
    "","先选择字符再改编码","一次修改一个字符","主要编码已存在",
    "共享编码已存在","没有检测到有效编码"
};

class quanbu : public QWidget
{
    Q_OBJECT
public:
    explicit quanbu(QWidget *parent=nullptr);
    bool isTTFok;
    void paintEvent(QPaintEvent *event)override;
    void mousePressEvent(QMouseEvent * event)override;
    void mouseDoubleClickEvent(QMouseEvent *event)override;
    void keyPressEvent(QKeyEvent *event) override;
    void setWenzi(GLYFTable *glyf, HMTXTable *hmtx, uint16 vwinAscent, uint16 vwinDscent);
    void setScroll(QScrollArea *xsa);
    /// 按照编码查找，找到返回glyphindex 找不到返回-1
    int32 chazhao(uint32 uni, int congjikaishi=1);
    void init();
    shanchuXuanze deleteWenziJiance(s32vector &glyphIdxSel);
    xiugaiGongxiang gongxiangGlyph(QString bmstr);
    int next();
    int pre();
    /// 分类显示 visShowall=true:全部显示，false=清除选择。
    void showAll(bool visShowall);
    s32vector xianshiZifu;//二次索引
    int currentSelidx();
    uint32 getBianmaFirst(int32 showIdx);
    int32 getGlyphIndex(int32 showIdx);
    void deleteWenziRepaint();
    int32 getBianmaAll(int32 showIdx, u32vector &bmall);
    QString &getBianmaAll(int32 showIdx, QString &bmall);
    /// 点击查找按钮（ctrlF,Enter),找到后屏幕移动到相应位置。
    bool btnChazhao(uint32 uni);
    QString getZifuFirst(int32 showIdx);
    void setSel(int32 selidx);
    ///获取选择的图形id
    void getXuanze(s32vector &glyphIdxSel);
    void fuzhiWenzi();
    void fuzhiBianma();
    void fuzhiLunkuoDian();
    void zhantieBiji();
    void saveFzPara(int vbujianIdx, int32 zhGlyphIdx);
    bool bunengXiugai();
    void clearFzPara();
    int32 tihuanZifu(const Biji &lkd, int lks, bool baochiWeizhi, bool shangxiaBuyuejie);
    void xuanLianbizi(bool zaiXuan);
    void fuzhiBianmaAll();
public slots:
    void zifuReady(CMAPTable *pcmap);
private:
    int zifuKuan;//间距,宽高相同
    int biaoqian;//间距
    u32vector *qbzifu;
    QMap<uint32,u32vector> *shareZifu;
    QScrollArea *sa;
    GLYFTable* pGlyf;
    HMTXTable* pHmtx;
    int ox;//!<图区左上角
    int oy;
    int32 selIdx;//!<查找选中的字符行列和索引
    int32 selIdxShift;
    ///根据鼠标位置确定glyphID,检测shift是否按下。返回true=点击在有效区域，同时修改selIdx selIdxshift
    bool mousePosToGlyphId(const QPoint& mousePos, bool &inShift);
    void updateBianma();
    bool isShowall;
    //把字画在小格子里
    double suofang;
    uint16 winAscent;
    uint16 winDscent;
    FzPara fzpara;
    double toPingmuY(int16 ttfY, int wzy, int buyong);
    double toPingmuX(int16 ttfX, int wzx, int pp2);
    bool zaiXuanLianbizi;
    /// 选部件自动点击。查找到部件字符时使用。
    void xuanbujianZidongDianji();
signals:
    void chaziOk(QString& bianma,QString& bmms);
    void kanlunkuo(bool xx=false, bool fromBujianDelete=false);
    void bujianXuanhao(int bujianIdx, BujianGaibian bg, int bujianGlyphIndex, double scale=0);
    void errorshow(const QString &x);
    void xuanhaoLianbizi(int32 lianbiziGlyphIndex);
    /// 记录滚动条位置
    void jizhuWeizhi(int vpos);
};

#endif // QUANBU_H
