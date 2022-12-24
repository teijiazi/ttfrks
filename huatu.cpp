#include "huatu.h"

huatu::huatu(QWidget *parent) : QWidget(parent),isTTFok(false),huaZuobiao(true),
    lksel(-1),xianshiDian(false),
    kjx(200),kjy(50),ox(10),oy(10),
    pmWinAscent(0),     pmWinDescent(0),     pmxHeight(0),     pmCapHeight(0),
    pmzx(0),    pmBaseline(0),    pmOx(0),    pmOy(0),
    pmxMin(0),    pmxMax(0),    pmyMin(0),    pmyMax(0),    pmPP1(0),    pmPP2(0),
    zfFzx(nullptr),
    suofangBeishu(1),shouciSuofang(0.2),sa(nullptr),
    xianshiLunkuohao(false),zaihuaxian(false),pGlyf(nullptr)
{
    setMouseTracking(true);
    setCursor(QCursor(Qt::CrossCursor));
    setFocusPolicy(Qt::StrongFocus);
}
void huatu::setScroll(QScrollArea *xsa){
    sa=xsa;
}

void huatu::showPoint(bool isShowPoint)
{
    if(!isTTFok){
        return;
    }
    xianshiDian=isShowPoint;
    setBhZifu();
    repaint();
}

void huatu::showLunkuoIndex(bool isShowLunkuoIdx)
{
    if(!isTTFok){
        return;
    }
    xianshiLunkuohao=isShowLunkuoIdx;
    setBhZifu();
    repaint();
}
void huatu::huajizhunxian(QPainter& hh,QPen&hb){
    hb.setStyle(Qt::DotLine);
    hh.setPen(hb);
    hh.drawText(ox,pmWinAscent-2,QString("%1 上边界").arg(usWinAscent));
    hh.drawLine(QLineF(ox,pmWinAscent, pmPP1,pmWinAscent));
    hh.drawText(ox,pmWinDescent-2,QString("%1 下边界").arg(sWinDescent));
    hh.drawLine(ox,pmWinDescent,pmPP1,pmWinDescent);
    if(sxHeight!=0){
        hh.drawText(ox,pmxHeight-2,"小写字母");
        hh.drawLine(ox,pmxHeight,    pmPP2,pmxHeight);
    }
    if(sCapHeight!=0){
        hh.drawText(ox,pmCapHeight-2,"大写字母");
        hh.drawLine(ox,pmCapHeight,  pmPP2,pmCapHeight);
    }
    hb.setStyle(Qt::SolidLine);
    hh.setPen(hb);
    hh.drawLine(pmPP1,pmWinAscent, pmPP2,pmWinAscent);//上边界2
    hh.drawLine(pmPP1,pmWinDescent,pmPP2,pmWinDescent);//下边界2
    hh.drawText(ox,pmzx-2,"边界中线");
    hb.setColor(QColor(210,93,92,200));
    hh.setPen(hb);
    hh.drawText(pmOx-10,oy+12,"Y");
    hh.drawText(ox,pmBaseline-2,"基线X");
    hh.drawLine(pmOx,oy,pmOx,this->height()-oy);
    hh.drawLine(ox,pmBaseline,this->width()-ox,pmBaseline);
    //中线
    hb.setColor(Qt::red);
    hb.setStyle(Qt::SolidLine);
    hh.setPen(hb);
    hh.drawLine(pmPP1,pmzx,pmPP2,pmzx);
    hb.setStyle(Qt::DotLine);
    hh.setPen(hb);
    hh.drawLine(ox,pmzx,pmPP1,pmzx);
}
void huatu::huaZuoyoujianju(QPainter& hh,QPen&hb){
    hb.setStyle(Qt::DashLine);
    hb.setColor(Qt::blue);
    hh.setPen(hb);
    //advancedWidth pp2
    hb.setStyle(Qt::DotLine);
    hh.setPen(hb);
    hh.drawText(pmPP2,oy+12,QString::number(ttfxMin-ttflsb+ttfaw));
    hh.drawLine(QLineF(pmPP2,oy,pmPP2,pmWinAscent));
    //lsb pp1
    hh.drawText(pmPP1,oy,QString::number(ttfxMin-ttflsb));
    hh.drawLine(QLineF(pmPP1,oy,pmPP1,pmWinAscent));
    hb.setStyle(Qt::SolidLine);
    hh.setPen(hb);
    hh.drawLine(QLineF(pmPP2,pmWinAscent,pmPP2,pmWinDescent));//pp2
    hh.drawLine(QLineF(pmPP1,pmWinAscent,pmPP1,pmWinDescent));//pp1
    if(zfpp.length()==0){
        return;
    }
    if(lkppList.length()==0){
        return;
    }
    //记录间距
    hh.drawText(pmxMin-60,pmyMax+12,QString("左间距  %1").arg(ttflsb));
    hh.drawText(pmxMax+30,pmyMin,QString("%1 右间距").arg(ttfaw-(ttfxMax-ttfxMin)-ttflsb));
}
void huatu::paintEvent(QPaintEvent *event){
    Q_UNUSED(event);
    QPainter hh;
    hh.begin(this);
    QPen hb;
    hh.setFont(QFont("微软雅黑"));
    hb.setColor(QColor(131,27,207));
    hh.setPen(hb);
    hh.drawRect(ox,oy,this->width()-20,this->height()-20);
    hb.setColor(Qt::blue);
    hh.setPen(hb);
    if(isTTFok){
        if(huaZuobiao){
            huajizhunxian(hh,hb);
            huaZuoyoujianju(hh,hb);
        }
        hb.setColor(Qt::blue);
        hh.setPen(hb);
        huazifu(hh,hb);
        if(zaihuaxian){
            hb.setColor(QColor(240,177,0));
            hh.setPen(hb);
            hh.setBrush(QBrush());
            hh.drawRect(QRectF(xuanzheBegin,xuanzeSize));
        }
    }

    hh.end();
}

void huatu::setBhZuobiao(){
    pmWinAscent=toPingmuY(usWinAscent);
    pmWinDescent=toPingmuY(sWinDescent);
    pmxHeight=toPingmuY(sxHeight);
    pmCapHeight=toPingmuY(sCapHeight);
    pmzx=toPingmuY(zx);
    pmBaseline=toPingmuY(0);
    pmOx=toPingmuX(0);
    pmOy=pmBaseline;
}

void huatu::setZuobiao(uint16 WinAscent, uint16 WinDescent, int16 xHeight, int16 CapHeight,
                       int fzzx,GLYFTable* vglyf)
{
    usWinAscent=WinAscent;
    sWinDescent=-WinDescent;
    sxHeight=xHeight;
    sCapHeight=CapHeight;
    zx=fzzx;
    if(vglyf){
        pGlyf=vglyf;
    }
    setBhZuobiao();
}
void huatu::huazifu(QPainter& hh,QPen&hb){
    const int maxColor=15;
    QColor zfdColor[maxColor]={
        QColor(255,128,0),
        QColor(136,0,21),
        QColor(255,128,128),
        QColor(0,0,0),
        QColor(255,201,14),
        QColor(131,27,207),
        QColor(128,255,0),
        QColor(0,128,128),
        QColor(237,28,36),
        QColor(255,242,0),
        QColor(163,73,164),
        QColor(63,72,204),
        QColor(0,162,232),
        QColor(255,0,255),
        QColor(112,146,190)
    };

    if(zfpp.length()>0&&huaZuobiao){
        //笔迹边界
        hb.setStyle(Qt::DashLine);
        hb.setColor(QColor(163,73,164,200));
        hh.setPen(hb);
        hh.drawRect(pmxMin,pmyMax,pmxMax-pmxMin,pmyMin-pmyMax);
        //标记字符上下方向的中点
        hb.setColor(Qt::blue);
        hb.setStyle(Qt::SolidLine);
        hh.setPen(hb);
        int16 bhzdY=pmyMax+(pmyMin-pmyMax)/2;
        hh.drawLine(pmxMin,bhzdY-5,pmxMin-5,bhzdY);
        hh.drawLine(pmxMin,bhzdY+5,pmxMin-5,bhzdY);
        hh.drawLine(pmxMax,bhzdY-5,pmxMax+5,bhzdY);
        hh.drawLine(pmxMax,bhzdY+5,pmxMax+5,bhzdY);
    }

    //字符轮廓
    QBrush br(QColor(100,123,32));
    hh.setBrush(br);
    hh.fillPath(zfpp,br);
    //标记单个轮廓
    if(lksel>=0 && lksel<lkppList.length()){
        hb.setColor(zfdColor[0]);
        hb.setWidth(2);
        hh.strokePath(lkppList[lksel],hb);
    }
    //字符点
    hb.setWidth(2);
    hb.setStyle(Qt::SolidLine);
    for(int i=0;i<zfddList.length();i++){
        hb.setColor(zfdColor[i%maxColor]);
        hh.strokePath(zfddList[i],hb);
    }
    //轮廓序号
    for(int i=0;i<lkhPPList.length();i++){
        hb.setColor(zfdColor[i%maxColor]);
        hh.strokePath(lkhPPList[i],hb);
    }
    //辅助线
    hb.setWidth(1);
    hb.setColor(zfdColor[0]);
    hh.strokePath(fzxpp,hb);
}
void huatu::kanlunkuo(lunkuoFanwei lfw, int16 lsb, uint16 aw, int32 vglyphIndex,
                      int xsHeight,QList<fuzhuxian>* fzx){
    //xsHeight=显示高度，用来调整字符大小
    zfFzx=fzx;
    ttfxMin=lfw.xMin;
    ttfxMax=lfw.xMax;
    ttfyMin=lfw.yMin;
    ttfyMax=lfw.yMax;
    ttflsb =lsb;
    ttfaw  =aw;
    glyphIndex=vglyphIndex;
    isTTFok=true;
    lksel=-1;//每次画图前清除选择。
    shouciSuofang=(1.0*xsHeight-2*kjy)/(usWinAscent-sWinDescent);
    setBhZifu();
    setBhZuobiao();
}
void huatu::updateFuzhuxian(){
    setBhZifu();
    repaint();
}

typedef double (huatu::*toPingmu)(int16,int,int);
void huatu::setBhZifu(){
    pmxMin=toPingmuX(ttfxMin);
    pmxMax=toPingmuX(ttfxMax);
    pmyMin=toPingmuY(ttfyMin);
    pmyMax=toPingmuY(ttfyMax);
    pmPP1 =toPingmuX(ttfxMin-ttflsb);
    pmPP2 =toPingmuX(ttfxMin-ttflsb+ttfaw);

    //准备辅助线
    fzxpp.clear();
    for(int i=0;i<zfFzx->length();i++){
        double pmy=toPingmuY((*zfFzx)[i].y);
        fzxpp.moveTo(ox,pmy);
        fzxpp.lineTo(pmPP2,pmy);
        fzxpp.addText(ox+(i%2==0?60:90),pmy-2,QFont("微软雅黑",9),QString::number(i+1));
    }
    //
    zfddList.clear();
    lkhPPList.clear();
    pGlyf->getZifuPaintPath<huatu,toPingmu>(glyphIndex,zfpp,this,&huatu::toPingmuX,&huatu::toPingmuY,
                                            0,0,0,
                                            xianshiDian?&zfddList:nullptr,
                                            xianshiLunkuohao?&lkhPPList:nullptr,
                                            &lkppList);
}

void huatu::mouseMoveEvent(QMouseEvent *event)
{
    showMousePos(toTTFx(event->pos().x()),toTTFy(event->pos().y()));
    if((event->buttons()==Qt::LeftButton)){//拖动左键画图
        zaihuaxian=true;
        xuanzeSize=QSizeF(event->x()-xuanzheBegin.x(),event->y()-xuanzheBegin.y());
        if(xuanzeSize.isNull()){
            return;//第一次启动，点击的时候会出发mouseMove----实际没有移动，不需要重画，防止擦掉已经选择的轮廓
        }
        lksel=-1;
        repaint();
    }
}

void huatu::mousePressEvent(QMouseEvent *event)
{
    //snd<<"mousePressEvent";
    xuanzheBegin=event->localPos();
    //检查是否点中轮廓内部---找到所有包含的轮廓，只记录最里面的轮廓。
    lksel=-1;//点击没有选中轮廓--预设，或清除之前的选择
    for(int i=0;i<lkppList.length();i++){
        if(lkppList[i].contains(xuanzheBegin)){
            if(lksel<0 || lkppList[lksel].contains(lkppList[i].elementAt(0))){//首次记录，或当前轮廓更小
                lksel=i;//记录一个轮廓
            }
        }
    }
    repaint();
}

void huatu::mouseReleaseEvent(QMouseEvent *event)
{
    //snd<<"release";
    if(zaihuaxian){
        xuanzeBianjie.xMax=toTTFx(xuanzheBegin.x());
        xuanzeBianjie.xMin=toTTFx(event->pos().x());
        if(xuanzeBianjie.xMin>xuanzeBianjie.xMax){
            short x=xuanzeBianjie.xMin;xuanzeBianjie.xMin=xuanzeBianjie.xMax;xuanzeBianjie.xMax=x;
        }
        xuanzeBianjie.yMax=toTTFy(xuanzheBegin.y());
        xuanzeBianjie.yMin=toTTFy(event->pos().y());
        if(xuanzeBianjie.yMin>xuanzeBianjie.yMax){
            short x=xuanzeBianjie.yMin;xuanzeBianjie.yMin=xuanzeBianjie.yMax;xuanzeBianjie.yMax=x;
        }
    }
    zaihuaxian=false;
}

void huatu::keyPressEvent(QKeyEvent *event)
{
    if(event->modifiers()!=Qt::ControlModifier){
        return;
    }
    //ctrl+1 ... ctrl+9 字符对齐辅助线
    switch (event->key()) {
    case Qt::Key_1 ... Qt::Key_9:
        tsx(event->key()-Qt::Key_1);
        break;
    default:
        QWidget::keyPressEvent(event);
    }
}

void huatu::wheelEvent(QWheelEvent *event)
{
    int delta=event->angleDelta().y();
    if(delta>=120){
        suofang(true);
        event->accept();
    }else if(delta<=-120){
        suofang(false);
        event->accept();
    }else{
        event->ignore();
    }
}
void huatu::mouseDoubleClickEvent(QMouseEvent *event){
    Q_UNUSED(event);
    showPoint(!xianshiDian);
    updataMaodianUI(xianshiDian);
}

void huatu::suofang(bool isFangda){
    if(isFangda){
        suofangBeishu*=1.25;
    }else{
        suofangBeishu*=0.8;
    }
    setBhZuobiao();
    setBhZifu();
    int sfh=this->pmWinDescent+suofangBeishu*kjy;
    int sfw=this->pmPP2+suofangBeishu*kjx;
    if(sfh<sa->height())sfh=sa->height();
    if(sfw<sa->width())sfw=sa->width();
    setFixedHeight(sfh);
    setFixedWidth(sfw);
    repaint();
    updateBfb(QString("%1%").arg(suofangBeishu*100,0,'f',2));
}
void huatu::init()
{
    isTTFok=false;
    suofangBeishu=1;
    shouciSuofang=0.2;
    xianshiDian=false;
    xianshiLunkuohao=false;
    zaihuaxian=false;
    lksel=-1;
    repaint();
}
