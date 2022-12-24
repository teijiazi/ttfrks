#include "fzyy.h"
#include <QPainter>
#include <QPaintEvent>

static const int lbHeight=25;
static const int lbVerCap=26;
static const int fzyyHeight=7*lbVerCap;
Fzyy::Fzyy(QWidget *parent, int idx, GLYFTable *glyf,uint16 usWinAscent,uint16 usWinDescent) :
    QFrame(parent),currentIndex(idx),pGlyf(glyf)
{
    if(idx%2==0){
        setStyleSheet(QString::fromUtf8("background-color: rgb(255, 201, 255);"));
    }else{
        setStyleSheet(QString::fromUtf8("background-color: rgb(235, 181, 235);"));
    }
    int poslb=0;//!<标记一个控件的y坐标
    yyidx = new ToolButtonEx(this,70/6,QColor(255,255,0,200),this->pGlyf);
    yyidx->setGeometry(QRect(41, poslb, 70, lbHeight));
    yyidx->jisuanSuofang(usWinAscent,usWinDescent);
    yyidx->setStyleSheet(QString::fromUtf8("background-color: rgb(135, 191, 95);\n"
                                           "color: rgb(255, 255, 0);"));
    QString kjj=QString("F%1").arg(idx+1);
    yyidx->setShortcut(QKeySequence(kjj));
    yyidx->setToolTip(kjj);
    connect(yyidx,&QToolButton::clicked,this,&Fzyy::xuanzeBujianZhunbei);

    lb1 = new QLabel(this);
    lb1->setGeometry(QRect(0, poslb, 41, lbHeight));
    lb1->setAlignment(Qt::AlignCenter);
    lb1->setText("引用图");
    //
    poslb+=lbVerCap;
    lb2 = new QLabel(this);
    lb2->setGeometry(QRect(0, poslb, 41, lbHeight));
    lb2->setAlignment(Qt::AlignCenter);
    lb2->setText("x偏移");
    offx = new SpinEditEx(lb2,this,&Fzyy::xChanged,false);
    offx->setEnabled(false);
    //
    poslb+=lbVerCap;
    lb3 = new QLabel(this);
    lb3->setAlignment(Qt::AlignCenter);
    lb3->setGeometry(QRect(0, poslb, 41, lbHeight));
    lb3->setText("y偏移");
    offy = new SpinEditEx(lb3,this,&Fzyy::yChanged,false);

    poslb+=lbVerCap;
    lb4 = new QLabel(this);
    lb4->setAlignment(Qt::AlignCenter);
    lb4->setGeometry(QRect(0, poslb, 41, lbHeight));
    lb4->setText("Xscale");
    xs = new DoubleSpinEditEx(lb4,this,&Fzyy::scaleChanged,true, bgXScale);
    xs->setValue(1.0);
    poslb+=lbVerCap;
    lb5 = new QLabel(this);
    lb5->setAlignment(Qt::AlignCenter);
    lb5->setGeometry(QRect(0, poslb, 41, lbHeight));
    lb5->setText("Yscale");
    ys = new DoubleSpinEditEx(lb5,this,&Fzyy::scaleChanged,true, bgYScale);
    ys->setValue(1.0);
    poslb+=lbVerCap;
    lb6 = new QLabel(this);
    lb6->setAlignment(Qt::AlignCenter);
    lb6->setGeometry(QRect(0, poslb, 41, lbHeight));
    lb6->setText("scale01");
    sf01 = new DoubleSpinEditEx(lb6,this,&Fzyy::scaleChanged,true, bgScale01);
    //
    btn = new QToolButton(this);
    btn->setGeometry(QRect(95, poslb, lbHeight, lbHeight));
    btn->setStyleSheet(QString::fromUtf8("background-color: rgb(170, 255, 0);"));
    btn->setText("+");
    connect(btn,&QToolButton::clicked,this,&Fzyy::jiaclicked);
    poslb+=lbVerCap;
    lb7 = new QLabel(this);
    lb7->setAlignment(Qt::AlignCenter);
    lb7->setGeometry(QRect(0, poslb, 41, lbHeight));
    lb7->setText("scale10");
    sf10 = new DoubleSpinEditEx(lb7,this,&Fzyy::scaleChanged,true, bgScale10);
    //
    btnJian = new QToolButton(this);
    btnJian->setGeometry(QRect(95, poslb, lbHeight, lbHeight));
    btnJian->setStyleSheet(QString::fromUtf8("background-color: rgb(170, 255, 0);"));
    btnJian->setText("-");
    connect(btnJian,&QToolButton::clicked,this,&Fzyy::jianclicked);
    //
    setGeometry(QRect(0, idx*fzyyHeight, 120, fzyyHeight));
}

void Fzyy::xChanged(int x){
    bujianGaibian(currentIndex, bgX, x);
}
void Fzyy::yChanged(int y){
    bujianGaibian(currentIndex, bgY, y);
}
void Fzyy::scaleChanged(int bianjiBiaoji, double newvalue){
    bujianGaibian(currentIndex, (BujianGaibian)bianjiBiaoji, 0, newvalue);
}

void Fzyy::jiaclicked(){
    btn->hide();
    //
    bujianGaibian(currentIndex,bgAppend);
}

void Fzyy::jianclicked(){
    bujianGaibian(currentIndex,bgDelete);
}

void Fzyy::xuanzeBujianZhunbei()
{
    bujianGaibian(currentIndex,bgHuanyige);
}

void Fzyy::tb(const BujianCanshu &fzcs,int curentI)
{
    currentIndex=curentI;
    PBiji pb=nullptr;
    if(fzcs.glyphIndex!=0){
        pb=pGlyf->getBzrList(fzcs.glyphIndex);
    }
    yyidx->huazifu(fzcs.glyphIndex);
    yyidx->setText(QString::number(fzcs.glyphIndex));
    offx->setValue(fzcs.offsetX);
    offy->setValue(fzcs.offsetY);
    xs->setValue(fzcs.scalex);
    ys->setValue(fzcs.scaley);
    sf01->setValue(fzcs.scale01);
    sf10->setValue(fzcs.scale10);
    btn->show();
    offx->setEnabled(!!pb);
    offy->setEnabled(!!pb);
    xs->setEnabled(!!pb);
    ys->setEnabled(!!pb);
    sf01->setEnabled(!!pb);
    sf10->setEnabled(!!pb);
    yyidx->repaint();
    show();
}

void Fzyy::hidebtn(){
    btn->hide();
}

void Fzyy::ychm(){
    currentIndex=-1;
    hide();
}

void Fzyy::updateBujianIdx(uint32 bujianGlyphIndex){
    PBiji pb=pGlyf->getBzrList(bujianGlyphIndex);
    yyidx->huazifu(bujianGlyphIndex);
    yyidx->setText(QString::number(bujianGlyphIndex));
    offx->setEnabled(!!pb);
    offy->setEnabled(!!pb);
    xs->setEnabled(!!pb);
    ys->setEnabled(!!pb);
    sf01->setEnabled(!!pb);
    sf10->setEnabled(!!pb);
}

bool Fzyy::updateX(int delta){
    if(currentIndex==-1){
        return true;
    }
    offx->setValue(offx->value()+delta);
    return false;
}
bool Fzyy::updateY(int delta){
    if(currentIndex==-1){
        return true;
    }
    offy->setValue(offy->value()+delta);
    return false;
}
int Fzyy::getHeight()
{
    return fzyyHeight;
}

ToolButtonEx::ToolButtonEx(QWidget *parent, int vpianyi, QColor vwenziColor, GLYFTable *vGlyf):
    QToolButton(parent),suofang(1.0),winAscent(0),winDscent(0),pianyi(vpianyi),wenziColor(vwenziColor),pGlyf(vGlyf)
{
    //QToolButton::setal
}
void ToolButtonEx::jisuanSuofang(uint16 vwinAscent, uint16 vwinDscent){
    winAscent=vwinAscent;
    winDscent=vwinDscent;
    suofang=1.0*this->height()/(winAscent+winDscent);
}
void ToolButtonEx::clear(){
    zfpp.clear();
    repaint();
}
void ToolButtonEx::paintEvent(QPaintEvent *event)
{
    //先调用基类函数画按钮。
    QToolButton::paintEvent(event);
    //补画字符
    QPainter hh;
    hh.begin(this);
    //字符轮廓
    QBrush br(wenziColor);
    hh.setBrush(br);
    hh.fillPath(zfpp,br);
    hh.drawText(this->width()/2,this->height()*0.6,btnText);
    hh.end();

}
double ToolButtonEx::toPingmuY(int16 ttfY, int x, int y)
{
    Q_UNUSED(x);
    Q_UNUSED(y);
    return (winAscent-ttfY)*suofang;
}

double ToolButtonEx::toPingmuX(int16 ttfX,int x,int y)
{
    Q_UNUSED(x);
    Q_UNUSED(y);
    return suofang*ttfX+pianyi;
}
void ToolButtonEx::setText(QString x){
    //覆盖原有函数不写原来的text
    btnText=x;
}
void ToolButtonEx::huazifu(int32 glyphIndex){
    typedef double (ToolButtonEx::*toPingmu)(int16,int,int);
    pGlyf->getZifuPaintPath<ToolButtonEx,toPingmu>(glyphIndex,zfpp,this,&ToolButtonEx::toPingmuX,&ToolButtonEx::toPingmuY);
}

void ToolButtonEx::setGlyf(GLYFTable* vglyf){
    this->pGlyf=vglyf;
}
