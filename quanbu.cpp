#include "quanbu.h"
#include <QApplication>
#include <QRegularExpression>
#include <QClipboard>

quanbu::quanbu(QWidget *parent) : QWidget(parent),isTTFok(false),zifuKuan(80),biaoqian(20),qbzifu(nullptr),
    shareZifu(nullptr),sa(nullptr),ox(1),oy(1),zaiXuanLianbizi(false)
{
    setFocusPolicy(Qt::StrongFocus);
    init();
}

void quanbu::setScroll(QScrollArea *xsa){
    sa=xsa;
}

void quanbu::zifuReady(CMAPTable*pcmap){
    qbzifu=&pcmap->zifuOrderByGlyphId;
    shareZifu=&pcmap->shareZifu;
    xianshiZifu.fill(0,qbzifu->length());
}

uint32 quanbu::getBianmaFirst(int32 showIdx){
    //根据索引返回编码--不包含合用图形的编码
    return (*qbzifu)[xianshiZifu[showIdx]==0?showIdx:xianshiZifu[showIdx]];
}

QString quanbu::getZifuFirst(int32 showIdx){
    //根据索引返回编码--不包含合用图形的编码
    uint32 bm=getBianmaFirst(showIdx);
    return QString::fromUcs4(&bm,1);
}

QString& quanbu::getBianmaAll(int32 showIdx, QString& bmall){
    //根据索引找到全部编码,空格分割0x1111 0x3333，字符串格式，存入bmall，并返回bmall的引用。
    int32 glyidx=getGlyphIndex(showIdx);
    bmall+=QString("0x%1").arg((*qbzifu)[glyidx],2,16,QChar('0')).toUpper();
    auto fnd=shareZifu->find((*qbzifu)[glyidx]);
    if(fnd!=shareZifu->end()){
        for(auto it=fnd.value().begin();it!=fnd.value().end();it++){
            bmall+=QString(" 0x%1").arg(*it,2,16,QChar('0')).toUpper();
        }
    }
    return bmall;
}

int32 quanbu::getBianmaAll(int32 showIdx, u32vector& bmall){
    //根据索引找到全部编码，存入bmall,u32vector,返回编码的gid。
    int32 glyidx=getGlyphIndex(showIdx);
    bmall.append((*qbzifu)[glyidx]);
    auto fnd=shareZifu->find((*qbzifu)[glyidx]);
    if(fnd!=shareZifu->end()){
        for(auto it=fnd.value().begin();it!=fnd.value().end();it++){
            bmall.append(*it);
        }
    }
    return glyidx;
}

int32 quanbu::getGlyphIndex(int32 showIdx){
    //二次索引
    //xianshiZifu[]={glyidx}
    //qbzifu[]={bianma}
    return xianshiZifu[showIdx]==0?showIdx:xianshiZifu[showIdx];
}

void quanbu::paintEvent(QPaintEvent *event){
    Q_UNUSED(event);
    if(!qbzifu)return;
    if(pGlyf && pGlyf->gl.length()==0){
        return;
    }
    QPainter hh;
    //
    int zifushu=xianshiZifu.length();
    int lie=(this->width()-2*ox)/zifuKuan;
    int hang=1+zifushu/(lie);
    int huabuGao=hang*(zifuKuan+biaoqian)+oy*2;
    setFixedHeight(huabuGao);

    //
    int scrollBarHeight=sa->verticalScrollBar()->height();
    int scrollPos=sa->verticalScrollBar()->value();
    int hangPerPage=1+scrollBarHeight/(zifuKuan+biaoqian);
    int xianshiHangKaishi= (hang-hangPerPage)*scrollPos/(huabuGao-scrollBarHeight);
    xianshiHangKaishi-=1;
    if(xianshiHangKaishi<0){
        xianshiHangKaishi=0;
    }
    //滑动范围：0~huabuGao-scrollBarHeight
    //起始行号：0~hang-hangPerPage
    //                      scrollPos               开始显示行号
    //               huabuGao-scrollBarHeight   hang-hangPerPage
    int ow=this->width()-ox,oh=this->height()-oy;
    hh.begin(this);
    QPen hb;
    hb.setColor(Qt::blue);
    hh.setPen(hb);
    hh.drawRect(ox,oy,ow-ox,oh-oy);
    //写字
    int32 idx=xianshiHangKaishi*(lie);
    //eg lie=3,画数线3次，显示3列。---一行显示lie个字，不完整的列不显示文字。

    QPainterPath zfpp;
    for(int r=xianshiHangKaishi;r<xianshiHangKaishi+hangPerPage+2;r++){
        //画横线
        int h1=oy+biaoqian+r*(biaoqian+zifuKuan),
                h2=oy+(biaoqian+zifuKuan)+r*(biaoqian+zifuKuan);
        hb.setColor(Qt::blue);
        hb.setStyle(Qt::DotLine);
        hh.setPen(hb);
        hh.drawLine(ox,h1,  ow,h1);
        hb.setStyle(Qt::DashLine);
        hh.setPen(hb);
        hh.drawLine(ox,h2,  ow,h2);
        //写字
        for(int c=0;c<lie&&idx<zifushu;c++){
            //文字区域左上角的坐标点
            int wzx=ox+c*zifuKuan;
            int wzy=oy+biaoqian+r*(biaoqian+zifuKuan);
            //画竖线
            hb.setColor(Qt::blue);
            hb.setStyle(Qt::SolidLine);
            hh.setPen(hb);
            hh.drawLine(wzx+zifuKuan,wzy-biaoqian, wzx+zifuKuan,wzy+zifuKuan);
            //序号
            hh.setFont(QFont("微软雅黑"));
            hh.setPen(hb);
            int32 glyphIndex=getGlyphIndex(idx);
            hh.drawText(wzx+10,wzy-5,QString("%1").arg(glyphIndex));
            typedef double (quanbu::*toPingmu)(int16,int,int);
            PBiji  pb=pGlyf->getBzrList(glyphIndex);
            if(pb){
                lunkuoFanwei lfw;
                pGlyf->getLunkuofanwei(glyphIndex,lfw);
                zfpp.clear();
                pGlyf->getZifuPaintPath<quanbu,toPingmu>(glyphIndex,zfpp,this,&quanbu::toPingmuX,&quanbu::toPingmuY,
                                                         wzx,wzy,
                                                         lfw.xMin-pHmtx->getLsb(glyphIndex)+pHmtx->getAw(glyphIndex));
                QBrush br(QColor(100,123,32,200));
                hh.setBrush(br);
                hh.fillPath(zfpp,br);
            }else{
                //大字
                hh.setFont(QFont("微软雅黑",50));
                hb.setColor(Qt::lightGray);
                hh.setPen(hb);
                hh.drawText(wzx,wzy,zifuKuan,zifuKuan,
                            Qt::AlignCenter,getZifuFirst(idx));
            }
            //选中
            if(idx>=selIdx&&idx<=selIdxShift){
                hh.fillRect(wzx,wzy-biaoqian,zifuKuan,zifuKuan+biaoqian,QBrush(QColor(255,0,0,100)));
            }
            //
            idx++;
        }
    }
    //
    hh.end();
}
void quanbu::init(){
    isTTFok=false;
    qbzifu=nullptr;
    shareZifu=nullptr;
    selIdx=-1;
    selIdxShift=-1;
    isShowall=true;
    fzpara.xuanBujian=false;
    zaiXuanLianbizi=false;
    if(sa){
        sa->ensureVisible(0,0,0,biaoqian+zifuKuan);
    }
}
bool quanbu::mousePosToGlyphId(const QPoint &mousePos, bool& inShift){
    if(!qbzifu)return false;

    inShift=(QApplication::keyboardModifiers()==Qt::ShiftModifier);

    int lie=(mousePos.x()-ox)/zifuKuan;
    int yihangZishu=(this->width()-2*ox)/zifuKuan;
    if(lie>=yihangZishu){
        return false;
    }
    int hang=(mousePos.y()-oy)/(zifuKuan+biaoqian);
    int idx=hang*(yihangZishu)+lie;
    if(idx>=xianshiZifu.length()){
        return false;
    }
    if(inShift){
        selIdxShift=idx;
    }else{
        selIdx=idx;
        selIdxShift=idx;
    }
    return true;
}
void quanbu::mousePressEvent(QMouseEvent *event)
{
    if(event->button()==Qt::LeftButton){
        bool inShift=false;
        if(!mousePosToGlyphId(event->pos(),inShift)){
            return;
        }
        //
        this->repaint();
        if(!inShift){
            updateBianma();
        }
    }
}

void quanbu::updateBianma(){
    //获取指定图形对应的全部编码--一个图形对应多个编码
    //idx:glyph id,begin=0
    QString gxbm;
    getBianmaAll(selIdx,gxbm);
    u32vector bmv;
    getBianmaAll(selIdx,bmv);
    QString bmms;//!<编码描述，unicode分类名称
    CMAPTable::getUnicodeMiaoshu(bmv,bmms);
    chaziOk(gxbm,bmms);
}

int quanbu::next(){
    selIdx+=1;
    if(selIdx>=xianshiZifu.length()){
        selIdx=xianshiZifu.length()-1;
    }
    selIdxShift=selIdx;
    updateBianma();
    return selIdx;
}
int quanbu::pre(){
    selIdx-=1;
    if(selIdx<0){
        selIdx=0;
    }
    selIdxShift=selIdx;
    updateBianma();
    return selIdx;
}
int quanbu::currentSelidx(){
    //返回xianshiZifu中的当前选择的索引
    return selIdx;
}
void quanbu::setSel(int32 selidx){
    selIdx=selidx;
    selIdxShift=selidx;
}

void quanbu::showAll(bool visShowall){
    //分类显示
    if(!isTTFok){
        return;
    }
    isShowall=visShowall;
    if(visShowall){
        xianshiZifu.clear();
        xianshiZifu.fill(0,qbzifu->length());
    }
    selIdx=-1;
    selIdxShift=-1;
    this->repaint();
}

bool quanbu::bunengXiugai(){
    return fzpara.xuanBujian;
}

void quanbu::saveFzPara(int vbujianIdx, int32 zhGlyphIdx){
    if(fzpara.xuanBujian){
        fzpara.bujianIdx=vbujianIdx;
    }else{
        fzpara.xuanBujian=true;
        fzpara.bujianIdx=vbujianIdx;
        fzpara.zhGlyphIdx=zhGlyphIdx;
    }
}
void quanbu::clearFzPara(){
    if(fzpara.xuanBujian){
        fzpara.xuanBujian=false;
    }
}
void quanbu::xuanLianbizi(bool zaiXuan){
    zaiXuanLianbizi=zaiXuan;
}

void quanbu::xuanbujianZidongDianji(){
    int32 bjGlyphIdx=getGlyphIndex(selIdx);
    if(bjGlyphIdx==fzpara.zhGlyphIdx){
        errorshow("不能选自己");
        return;
    }
    if(bjGlyphIdx==0){
        errorshow("不能选notdef");
        return;
    }
    if(!pGlyf->gl[bjGlyphIdx].pg){
        errorshow("此处没有图形");
        return;
    }
    this->pGlyf->xiugaiBujianGlyphIndex(bjGlyphIdx,fzpara.bujianIdx,fzpara.zhGlyphIdx);
    bujianXuanhao(fzpara.bujianIdx,bgXuanhao,bjGlyphIdx);
    fzpara.xuanBujian=false;
}
void quanbu::mouseDoubleClickEvent(QMouseEvent *event)
{
    bool inShift=false;
    if(!mousePosToGlyphId(event->pos(),inShift)){
        return;
    }
    if(!inShift){
        if(fzpara.xuanBujian){
            xuanbujianZidongDianji();
        }else if(zaiXuanLianbizi){
            xuanhaoLianbizi(getGlyphIndex(selIdx));
        }else{
            jizhuWeizhi(sa->verticalScrollBar()->value());
            kanlunkuo(false);
        }
    }
}
void quanbu::fuzhiBianmaAll(){
    if(selIdx==-1){
        return;
    }
    QString wb;
    for(int i=selIdx;i<=selIdxShift;i++){
        getBianmaAll(i,wb);
    }
    QClipboard* jtb=QApplication::clipboard();
    jtb->setText(wb);
}
void quanbu::fuzhiBianma(){
    //不复制共享编码
    if(selIdx==-1){
        return;
    }
    QString wb;
    for(int i=selIdx;i<=selIdxShift;i++){
        wb+=QString("0x%1;").arg(getBianmaFirst(i),4,16,QChar('0')).toUpper();
    }
    QClipboard* jtb=QApplication::clipboard();
    jtb->setText(wb);
}
void quanbu::fuzhiLunkuoDian(){
    //复制轮廓数据 用于 zhantieBiji
    if(selIdx==-1){
        return;
    }
    QClipboard* jtb=QApplication::clipboard();
    QString bzrStr;
    for(int gi=selIdx;gi<=selIdxShift;gi++){
        int32 glyidx=getGlyphIndex(gi);
        PBiji  pb=pGlyf->getBzrList(glyidx);
        if(pb){
            int16 lsb=pHmtx->getLsb(glyidx);
            uint16 aw=pHmtx->getAw(glyidx);
            lunkuoFanwei lfw;
            pGlyf->getLunkuofanwei(glyidx,lfw);
            uint16 lks=0,ds=0;
            pGlyf->getPointCount(glyidx,ds,lks);
            bzrStr+=QString("%1%2%3%4%5%6%7%8")
                    .arg(lks,4,16,QChar('0'))
                    .arg((uint16)(lfw.xMax),4,16,QChar('0'))
                    .arg((uint16)(lfw.yMax),4,16,QChar('0'))
                    .arg((uint16)(lfw.xMin),4,16,QChar('0'))
                    .arg((uint16)(lfw.yMin),4,16,QChar('0'))
                    .arg((uint16)(pb->length()),4,16,QChar('0'))
                    .arg((uint16)(lsb),4,16,QChar('0'))
                    .arg(aw,4,16,QChar('0'));
            for (int i=0;i<pb->length();i++) {
                bzrStr+=QString("%1%2%3%4%5")
                        .arg((uint16)((*pb)[i].x),4,16,QChar('0'))
                        .arg((uint16)((*pb)[i].y),4,16,QChar('0'))
                        .arg((*pb)[i].lkEnd?1:0,1,16)
                        .arg((*pb)[i].wojia?1:0,1,16)
                        .arg((*pb)[i].isControl?1:0,1,16);
            }
        }else{
            bzrStr+="G";
        }
    }
    jtb->setText(bzrStr);
}

void quanbu::zhantieBiji(){
    if(selIdx==-1){
        return;
    }
    QClipboard* jtb=QApplication::clipboard();
    QString bj=jtb->text();
    int i=0;
    for(int gi=selIdx;i<bj.length()&& gi<xianshiZifu.length();gi++){
        if(bj[i]=="G"){
            i++;
            continue;
        }
        int32 glyidx=getGlyphIndex(gi);
        Biji bl;
        lunkuoFanwei lfw;
        int16 lks=bj.mid(i,4).toInt(nullptr,16);//轮廓数量
        lfw.xMax=bj.mid(i+4,4).toInt(nullptr,16);
        lfw.yMax=bj.mid(i+8,4).toInt(nullptr,16);
        lfw.xMin=bj.mid(i+12,4).toInt(nullptr,16);
        lfw.yMin=bj.mid(i+16,4).toInt(nullptr,16);
        int16 bjs=bj.mid(i+20,4).toInt(nullptr,16);//点的数量
        int16 lsb=bj.mid(i+24,4).toInt(nullptr,16);
        int16 aw=bj.mid(i+28,4).toInt(nullptr,16);
        pHmtx->setLsbAw(glyidx,lsb,aw);
        //snd<<aw;
        int bji=0;
        i+=32;
        while(bji<bjs){
            bzr2 blx;
            blx.x=bj.mid(i,4).toInt(nullptr,16);
            blx.y=bj.mid(i+4,4).toInt(nullptr,16);
            blx.lkEnd=bj.mid(i+8,1).toShort(nullptr,16);
            blx.wojia=bj.mid(i+9,1).toShort(nullptr,16);
            blx.isControl=bj.mid(i+10,1).toShort(nullptr,16);
            bl.append(blx);
            i+=11;
            bji++;
        }
        pGlyf->appendBzr(glyidx,bl,lfw,lks);
    }
    repaint();
}
void quanbu::fuzhiWenzi(){
    if(selIdx==-1){
        return;
    }
    QString wb;
    for(int i=selIdx;i<=selIdxShift;i++){
        wb+=getZifuFirst(i);
    }
    QClipboard* jtb=QApplication::clipboard();
    jtb->setText(wb);
}

void quanbu::keyPressEvent(QKeyEvent *event)
{
    if(event->modifiers()==Qt::CTRL){
        switch (event->key()) {
        case Qt::Key_A:
            selIdx=1;
            selIdxShift=xianshiZifu.length()-1;
            repaint();
            break;
        case Qt::Key_V:
            zhantieBiji();
            break;
        default:
            QWidget::keyPressEvent(event);
        }
    }
}

void quanbu::setWenzi(GLYFTable* glyf,HMTXTable* hmtx,uint16 vwinAscent,uint16 vwinDscent){
    isTTFok=true;
    pGlyf=glyf;
    pHmtx=hmtx;
    suofang=1.0*zifuKuan/(vwinAscent+vwinDscent);
    winAscent=vwinAscent;
    winDscent=vwinDscent;
}

int32 quanbu::chazhao(uint32 uni,int congjikaishi){
    //按照编码查找，找到返回glyphindex
    if(!qbzifu)return -1;

    if(congjikaishi<1){congjikaishi=1;}//从1开始查找。0用来存储notdef，编码存储为占用了编码0
    int idx=qbzifu->indexOf(uni,congjikaishi);
    if(idx==-1){
        //查共享字符
        for(auto i=shareZifu->begin();i!=shareZifu->end();i++){
            if(i.value().indexOf(uni)!=-1){
                idx=qbzifu->indexOf(i.key(),1);
                break;
            }
        }
    }
    return idx;
}

bool quanbu::btnChazhao(uint32 uni){
    //点击查找按钮
    if(!qbzifu)return false;

    int idx=chazhao(uni);
    if(idx==-1){
        selIdx=-1;
        selIdxShift=-1;
        repaint();
        return false;
    }
    //找到了
    if(!isShowall){
        //查找结束要显示全部
        this->showAll(true);
    }
    //标记选择
    selIdx=idx;
    selIdxShift=idx;
    //修改视图
    int zifuPerRow=(this->width()-2*ox)/zifuKuan;
    int hang=idx/zifuPerRow;
    sa->ensureVisible(0,oy+(biaoqian+zifuKuan)*hang+(biaoqian+zifuKuan)/2,
                      0,biaoqian+zifuKuan);
    repaint();
    updateBianma();
    //自动打开
    if(fzpara.xuanBujian){
        xuanbujianZidongDianji();
    }
    return true;
}

shanchuXuanze quanbu::deleteWenziJiance(s32vector &glyphIdxSel){
    //返回能否选中。
    //若能选中sxOK，选择的编码的glyphIndex存入glyphIdxSel，
    //若不能选中sx其他，glyphIdxSel的内容未清空但不可用。
    //检查全部编码能否删除，收集主要编码，不收集共享编码
    if(!qbzifu)return sxFileNotOpen;
    if(selIdx==-1)return sxNoSelect;//没有选中
    glyphIdxSel.clear();
    for(int i=selIdx;i<=selIdxShift;i++){
        u32vector bmv;
        glyphIdxSel.append(getBianmaAll(i,bmv));
        //for (int bi=0;bi<bmv.length();bi++) {
        //    if(bmv[bi]==0x20){
        //        return sxKongge;
        //    }else if(bmv[bi]==0 && i==0){
        //        return sxDiyi;
        //    }
        //}
    }
    //返回OK的时候，数据可以被删除
    return sxOK;
}

void quanbu::deleteWenziRepaint(){
    //删除成功以后重画。清除选择
    selIdx-=1;
    if(selIdx<0){
        selIdx=-1;
    }
    selIdxShift=selIdx;
    if(selIdx>=0){
        updateBianma();
    }
    repaint();
}

void quanbu::getXuanze(s32vector &glyphIdxSel){
    //获取选择的图形ID，存储在glyphIdxSel
    if(selIdx==-1){
        return;
    }
    for(int i=selIdx;i<=selIdxShift;i++){
        glyphIdxSel.append(getGlyphIndex(i));
    }
}

xiugaiGongxiang quanbu::gongxiangGlyph(QString bmstr){
    if(selIdx==-1)return xgMeiyouxuan;//没有选
    if(selIdx!=selIdxShift)return xgDuoxuan;//多选

    QRegularExpression reg("0[xX]([0-9a-fA-F]{2,4})");
    u32vector bmlist;
    //读取添加的编码
    auto x=reg.globalMatch(bmstr);
    while (x.hasNext()) {
        QRegularExpressionMatch match = x.next();
        QString word = match.captured(1);
        bmlist.append(word.toUInt(nullptr,16));
    }
    //没有输入有效的新编码
    if(bmlist.size()==0){
        return xgWuxiao;
    }
    uint32 laoBianma=getBianmaFirst(selIdx);
    int32 selGid=getGlyphIndex(selIdx);
    //检查主要编码
    if(bmlist[0]!=laoBianma){
        int32 idxtest=chazhao(bmlist[0]);
        if(idxtest!=-1 && idxtest!=selGid){//不能存在于其他主要编码 全部共享编码
            return xgZhubianma;
        }
        (*qbzifu)[selGid]=bmlist[0];
        shareZifu->remove(laoBianma);//替换了老的主要编码时，删除老编码的共享编码
    }
    uint32 xinBianma=bmlist.takeAt(0);
    //以下检查共享编码
    if(bmlist.size()==0){
        //删除了原有的共享编码。有则删除。没有则不变。
        shareZifu->remove(xinBianma);
    }else{
        //剩余编码若有效，则更改共享编码
        for(int i=0;i<bmlist.length();i++){
            //1 全部主要字符中不存在
            int idxtest=qbzifu->indexOf(bmlist[i]);
            if(idxtest!=-1){
                return xgYicunzai;
            }
            //2 没有参加其他共享
            for(auto it=shareZifu->begin();it!=shareZifu->end();it++){
                if(it.key()!=xinBianma && it.value().indexOf(bmlist[i])!=-1){
                    return xgYicunzai;
                }
            }
        }
        //有则替换，没有则增加
        auto fnd=shareZifu->find(xinBianma);
        if(fnd!=shareZifu->end()){
            fnd.value()=bmlist;
        }else{
            shareZifu->insert(xinBianma,bmlist);
        }
    }
    repaint();
    return xgOk;
}

double quanbu::toPingmuY(int16 ttfY,int wzy,int buyong)
{
    Q_UNUSED(buyong);
    return (winAscent-ttfY)*suofang+wzy;
}

double quanbu::toPingmuX(int16 ttfX,int wzx,int pp2)
{
    return wzx+suofang*((winAscent+winDscent-pp2)/2+ttfX);
}

int32 quanbu::tihuanZifu(const Biji &lkd, int lks, bool baochiWeizhi,bool shangxiaBuyuejie){
    int32 glyphidx=getGlyphIndex(selIdx);
    pGlyf->tihuanZifu(glyphidx,
                      lkd,lks,
                      baochiWeizhi?0:(winAscent-(winAscent+winDscent)/2),
                      shangxiaBuyuejie?(winAscent+winDscent):0);
    return glyphidx;
}
