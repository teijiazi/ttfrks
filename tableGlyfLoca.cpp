#include <tableGlyfLoca.h>

HMTXTable::HMTXTable(QObject *parent):TableTTF(parent){}

void HMTXTable::read(QDataStream& dt, uint32 offset,uint16 numberOfHMetrics,uint16 numGlyphs){
    dt.device()->seek(offset);
    for(uint i=0;i<numberOfHMetrics;i++){
        LongHorMetric lhm;
        dt>>lhm.advanceWidth>>lhm.lsb;
        fzhMetricsAll.append(lhm);
    }
    hMetrics=fzhMetricsAll;
    leftSideBearings.clear();
    if(numGlyphs - numberOfHMetrics>0){
        uint16 aw=fzhMetricsAll[numberOfHMetrics-1].advanceWidth;
        for(uint i=0;i+numberOfHMetrics<numGlyphs;i++){
            LongHorMetric lhm;
            lhm.advanceWidth=aw;
            dt>>lhm.lsb;
            leftSideBearings.append(lhm.lsb);
            fzhMetricsAll.append(lhm);
        }
    }
};
void HMTXTable::jisuan(UFWORD& maxAw,uint16&vNumberOfHMetrics){
    //
    maxAw=fzhMetricsAll[0].advanceWidth;
    for(int i=0;i<fzhMetricsAll.length();i++){
        if(fzhMetricsAll[i].advanceWidth>maxAw){
            maxAw=fzhMetricsAll[i].advanceWidth;
        }
    }
    //压缩存储
    leftSideBearings.clear();
    int removeBegin=-1;
    for(int i=fzhMetricsAll.length()-1;i>0;i--){
        if(fzhMetricsAll[i].advanceWidth==fzhMetricsAll[i-1].advanceWidth){
            removeBegin=i;
            leftSideBearings.prepend(fzhMetricsAll[i].lsb);
        }else{
            break;
        }
    }
    hMetrics.clear();
    if(removeBegin!=-1){
        hMetrics=QList<LongHorMetric>(fzhMetricsAll.begin(),fzhMetricsAll.begin()+removeBegin);
    }else{
        hMetrics=fzhMetricsAll;
    }
    //
    vNumberOfHMetrics=hMetrics.length();
}
void HMTXTable::save(QDataStream& dt){
    for(int i=0;i<hMetrics.length();i++){
        dt<<hMetrics[i].advanceWidth<<hMetrics[i].lsb;
    }

    for(int i=0;i<leftSideBearings.length();i++){
        dt<<leftSideBearings[i];
    }
}
void HMTXTable::show(){
    QString hm("advanceWidth\tlsb\r\n");
    for(int i=0;i<hMetrics.length();i++){
        hm.append(QString("%3:%1\t%2\r\n").arg(hMetrics[i].advanceWidth).arg(hMetrics[i].lsb).arg(i));
    }
    datashow(hm);

    hm="leftSideBearings\r\n";
    for(int i=0;i<leftSideBearings.length();i++){
        hm.append(QString("%2:%1\r\n").arg(leftSideBearings[i]).arg(i));
    }
    datashow(hm);
}

void HMTXTable::xinjian(uint16 tuxingzongshu)
{
    leftSideBearings.clear();
    for(uint i=0;i<tuxingzongshu;i++){
        hMetrics.append(LongHorMetric());
    }
    fzhMetricsAll=hMetrics;
}

void HMTXTable::deleteWenzi(const s32vector &glyphIdxSel)
{
    for(int i=glyphIdxSel.length()-1;i>=0;i--){
        fzhMetricsAll.removeAt(glyphIdxSel[i]);
    }
}

void HMTXTable::deleteTuxing(s32vector &glyphIdxSel){
    //删除图形以后，lsb等于0.aw不动。
    for(int i=glyphIdxSel.length()-1;i>=0;i--){
        fzhMetricsAll[glyphIdxSel[i]].lsb=0;
    }
}

void HMTXTable::appendBianma(int bccnt){
    //补充空的字符
    for(int i=0;i<bccnt;i++){
        LongHorMetric x;
        fzhMetricsAll.append(x);
    }
}

uint16 HMTXTable::getAw(int32 glyphIdx) const
{
    return fzhMetricsAll[glyphIdx].advanceWidth;
}

int16 HMTXTable::getLsb(int32 glyphIdx)
{
    return fzhMetricsAll[glyphIdx].lsb;
}

void HMTXTable::setLsbAw(int32 glyphIdx, int16 lsb, uint16 aw){
    fzhMetricsAll[glyphIdx].lsb=lsb;
    fzhMetricsAll[glyphIdx].advanceWidth=aw;
}

void HMTXTable::setLsb(int32 glyphIdx, int16 lsb)
{
    fzhMetricsAll[glyphIdx].lsb=lsb;
}


LOCATable::LOCATable(QObject *parent):TableTTF(parent){}
LOCATable::~LOCATable(){

}
void LOCATable::read(QDataStream& dt, uint32 offset,uint16 cd,uint16 numGlyphs){
    dt.device()->seek(offset);
    if(cd==0){
        for(int i=0; i<numGlyphs+1;i++){
            Offset16 x;
            dt>>x;
            offsets.append(x*2);
        }
    }else{
        for(int i=0; i<numGlyphs+1;i++){
            Offset32 x;
            dt>>x;
            offsets.append(x);
        }
    }
}
void LOCATable::save(QDataStream& dt){
    //都存储为32位
    for(uint16 i=0; i<offsets.length();i++){
        dt<<offsets[i];
    }
}

void LOCATable::show()
{
    QString info;
    for(uint16 i=0; i<offsets.length();i++){
        info.append(QString("%1:%2\r\n").arg(i).arg(offsets[i]));
    }
    datashow(info);
}

Glyph::Glyph():numberOfContours(0),xMin(0),yMin(0),xMax(0),yMax(0)
{

}

Glyph::~Glyph()
{

}

/// ttf文件,DataStream：bigendian,指针操作：littleEndian
int16 Glyph::fz16(uint16 v){
    /// int16 x=ufz16(value);
    /// uint16 xx=ufz16(value);
    /// 以上都不影响结果
    return ((v&0xff)<<8)|(v>>8);
}
/// ttf文件,DataStream：bigendian,指针操作：littleEndian
uint16 Glyph::ufz16(uint16 v){
    /// int16 x=ufz16(value);
    /// uint16 xx=ufz16(value);
    /// 以上都不影响结果
    return ((v&0xff)<<8)|(v>>8);
}
Simple::Simple():instructionLength(0){}
Simple::~Simple(){

}
void Simple::yasuo(QVector<char>& ysData){
    Biji fuzhiBzr=bzr2List;
    QList<char> fzflags;
    int lki=numberOfContours-1;
    endPtsOfContours.clear();
    endPtsOfContours.fill(0,numberOfContours);
    for(int i=bzr2List.length()-1;i>0;i--){
        //轮廓统计
        if(bzr2List[i].lkEnd){
            endPtsOfContours[lki--]=i;
        }
        //改成相对坐标
        fuzhiBzr[i].x=bzr2List[i].x-bzr2List[i-1].x;
        fuzhiBzr[i].y=bzr2List[i].y-bzr2List[i-1].y;
        //修改标记
        uint8 flag=0;
        flag|=!bzr2List[i].isControl;
        if(fuzhiBzr[i].y==0){
            flag|=Y_IS_SAME;
        }else if((fuzhiBzr[i].y & 0xff00)==0){
            flag|=POSITIVE_Y;
        }else if(((-fuzhiBzr[i].y) & 0xff00)==0){
            flag|=fuY;
            fuzhiBzr[i].y*=-1;
        }
        if(fuzhiBzr[i].x==0){
            flag|=X_IS_SAME;
        }else if((fuzhiBzr[i].x&0xff00)==0){
            flag|=POSITIVE_X;
        }else if(((-fuzhiBzr[i].x) & 0xff00)==0){
            flag|=fuX;
            fuzhiBzr[i].x*=-1;
        }
        fzflags.prepend(flag);
    }
    //第一个点的flag
    uint8 flag=0;
    flag|=!bzr2List[0].isControl;
    if((fuzhiBzr[0].x&0xff00)==0){
        flag|=POSITIVE_X;
    }else if(((-fuzhiBzr[0].x) & 0xff00)==0){
        flag|=fuX;
        fuzhiBzr[0].x*=-1;
    }
    if((fuzhiBzr[0].y&0xff00)==0){
        flag|=POSITIVE_Y;
    }else if(((-fuzhiBzr[0].y) & 0xff00)==0){
        flag|=fuY;
        fuzhiBzr[0].y*=-1;
    }
    fzflags.prepend(flag);
    //压缩flag
    int i=0;
    uint8 jishu=0;
    int writePos=-1;
    while (i<fzflags.length()-1) {
        if(fzflags[i]==fzflags[i+1]){
            jishu+=1;
            if(writePos==-1){
                fzflags[i]|=REPEAT_FLAG;
                writePos=i+1;
            }
        }else{
            if(writePos!=-1){
                fzflags[writePos]=jishu;
                fzflags.erase(fzflags.begin()+writePos+1,fzflags.begin()+i+1);
                i=writePos+1;
                jishu=0;
                writePos=-1;
                continue;
            }
        }
        i++;
    }
    if(writePos!=-1){
        fzflags[writePos]=jishu;
        fzflags.erase(fzflags.begin()+writePos+1,fzflags.begin()+i+1);
    }
    //写入ysData,bigEndian
    ysData.clear();
    ysData<<(numberOfContours>>8)<<(numberOfContours&0xff);
    ysData<<(xMin>>8)<<(xMin&0xff);
    ysData<<(yMin>>8)<<(yMin&0xff);
    ysData<<(xMax>>8)<<(xMax&0xff);
    ysData<<(yMax>>8)<<(yMax&0xff);
    for(int ki=0;ki<numberOfContours;ki++){
        ysData<<(endPtsOfContours[ki]>>8)<<(endPtsOfContours[ki]&0xff);
    }
    ysData<<0<<0;
    ysData<<fzflags.toVector();
    QVector<char> yzb;
    //第一个点
    if(fuzhiBzr[0].x&0xff00){
        ysData<<(fuzhiBzr[0].x>>8)<<(fuzhiBzr[0].x&0xff);
    }else{
        ysData<<(fuzhiBzr[0].x&0xff);
    }
    if(fuzhiBzr[0].y&0xff00){
        yzb<<(fuzhiBzr[0].y>>8)<<(fuzhiBzr[0].y&0xff);
    }else{
        yzb<<(fuzhiBzr[0].y&0xff);
    }
    //剩余点
    for(int fi=1;fi<fuzhiBzr.length();fi++){
        if(fuzhiBzr[fi].x!=0){
            if(fuzhiBzr[fi].x&0xff00){
                ysData<<(fuzhiBzr[fi].x>>8)<<(fuzhiBzr[fi].x&0xff);
            }else{
                ysData<<(fuzhiBzr[fi].x&0xff);
            }
        }
        if(fuzhiBzr[fi].y!=0){
            if(fuzhiBzr[fi].y&0xff00){
                yzb<<(fuzhiBzr[fi].y>>8)<<(fuzhiBzr[fi].y&0xff);
            }else{
                yzb<<(fuzhiBzr[fi].y&0xff);
            }
        }
    }
    //y
    ysData<<yzb;
}

Biji &Simple::getBzrList(GLYFTable*pgl)
{
    Q_UNUSED(pgl);
    return this->bzr2List;
}

void Simple::appendBzr(const Biji &bl, const lunkuoFanwei &lfw, int lks)
{
    bzr2List.append(bl);
    if(numberOfContours==0){
        xMax=lfw.xMax;
        xMin=lfw.xMin;
        yMax=lfw.yMax;
        yMin=lfw.yMin;
    }else{
        if(xMax<lfw.xMax)xMax=lfw.xMax;
        if(xMin>lfw.xMin)xMin=lfw.xMin;
        if(yMax<lfw.yMax)yMax=lfw.yMax;
        if(yMin>lfw.yMin)yMin=lfw.yMin;
    }
    numberOfContours+=lks;
}

void Simple::tsx(int sxDelta)
{
    for(int i=0;i<bzr2List.length();i++){
        bzr2List[i].y+=sxDelta;
    }
}

void Simple::zuoyouYidong(int zyDelta)
{
    for(int i=0;i<bzr2List.length();i++){
        bzr2List[i].x+=zyDelta;
    }
}

void Simple::tsxzy(int sxDelta,int zyDelta)
{
    for(int i=0;i<bzr2List.length();i++){
        bzr2List[i].y+=sxDelta;
        bzr2List[i].x+=zyDelta;
    }
}

void Simple::suofang(bool fangda)
{
    double suofangBili=fangda?1.05:0.952;
    suofang(suofangBili);
}

void Simple::suofang(double suofangBili)
{
    for(int i=0;i<bzr2List.length();i++){
        bzr2List[i].y*=suofangBili;
        bzr2List[i].x*=suofangBili;
    }
    this->xMax*=suofangBili;
    this->yMax*=suofangBili;
    this->xMin*=suofangBili;
    this->yMin*=suofangBili;
}

void Simple::updateBindBox(GLYFTable *pgl)
{
    Q_UNUSED(pgl);
    bool getFirst=false;
    Biji &xds=bzr2List;
    for (int i = 0; i < xds.length(); i++)
    {
        //计算包围矩形
        if(!getFirst){
            getFirst=true;
            xMin=xds[i].x;
            yMin=xds[i].y;
            xMax=xds[i].x;
            yMax=xds[i].y;
        }else {
            if(xds[i].x<xMin)xMin=xds[i].x;
            if(xds[i].y<yMin)yMin=xds[i].y;
            if(xds[i].x>xMax)xMax=xds[i].x;
            if(xds[i].y>yMax)yMax=xds[i].y;
        }
    }
}

void Simple::read(uint16* p){
    numberOfContours=ufz16(p[0]);
    xMin=ufz16(p[1]),yMin=ufz16(p[2]),xMax=ufz16(p[3]),yMax=ufz16(p[4]);
    uint16* pEnd=&p[5];
    for(int i=0;i<numberOfContours;i++,pEnd++){
        endPtsOfContours.append(ufz16(*pEnd));
    }
    instructionLength=ufz16(*pEnd++);
    uint8 * p8=(uint8*)pEnd;
    if(instructionLength>0){
        for(int i=0;i<instructionLength;i++){
            instructions.append(*p8++);
        }
    }
    uint16 numPoint=endPtsOfContours[numberOfContours-1]+1;
    QVector<char> fzflags;
    fzflags.fill(0,numPoint);
    //找到y坐标数组的开始位置，扩展flag数组中的repeat标记
    int i=0;
    uint16 xlen=0;
    while(i<numPoint){
        fzflags[i]=p8[0];
        int xCoord=0;
        switch(fzflags[i]&xTest)
        {
        case POSITIVE_X:
        case fuX:
            xCoord=1;
            break;
        case is16b:
            xCoord=2;
        }
        if(p8[0] &REPEAT_FLAG)
        {
            xlen+=xCoord+ xCoord*p8[1];
            for(int fi=0;fi<p8[1];fi++){
                fzflags[++i]=p8[0];
            }
            p8++;
        }else{
            xlen+=xCoord;
        }
        i++;
        p8++;
    }
    //按标记填写x y坐标数组
    uint8 *px=p8;
    uint8 *py=p8+xlen;
    i=0;
    while(i<numPoint){
        bzr2 bz;
        bz.isControl=!(fzflags[i]&curveTest);
        switch(fzflags[i]&xTest)
        {
        case POSITIVE_X:
            bz.x=px[0];px++;
            break;
        case fuX:
            bz.x=-px[0];px++;
            break;
        case is16b:
            bz.x=ufz16(*((uint16*)px));px+=2;
            break;
        case X_IS_SAME:
            bz.x=0;
            break;
        }
        switch(fzflags[i]&yTest)
        {
        case POSITIVE_Y:
            bz.y=py[0];py++;
            break;
        case fuY:
            bz.y=-py[0];py++;
            break;
        case is16b:
            bz.y=ufz16(*((uint16*)py));py+=2;
            break;
        case Y_IS_SAME:
            bz.y=0;
            break;
        }
        //修改成绝对坐标
        if(i!=0){
            bz.x+=bzr2List[i-1].x;
            bz.y+=bzr2List[i-1].y;
        }
        bzr2List.push_back(bz);
        i++;
    }
    //把一个数组标记为多个轮廓。不能与插点放在一起循环，插点会改变原来的索引
    for(int lki=0;lki<endPtsOfContours.length();lki++){
        bzr2List[endPtsOfContours[lki]].lkEnd=true;
    }
}

void Simple::getPointCount(GLYFTable*pgl, uint16 &pointC, uint16 &lunkuoC)
{
    //累加
    Q_UNUSED(pgl);
    pointC+=bzr2List.length();
    lunkuoC+=this->numberOfContours;
}

Composite::Composite(){}

Composite::~Composite()
{

}
void Composite::read(uint16* p){
    numberOfContours=ufz16(p[0]);
    xMin=ufz16(p[1]),yMin=ufz16(p[2]),xMax=ufz16(p[3]),yMax=ufz16(p[4]);
    //
    uint16* pFlag=&p[5];
    do {
        BujianCanshu bc;
        bc.flags=ufz16(*pFlag++);
        if(bc.flags & WE_HAVE_INSTRUCTIONS){
            //snd<<"yzl";
        }
        bc.glyphIndex=ufz16(*pFlag++);
        if ( bc.flags & ARG_1_AND_2_ARE_WORDS) {
            bc.offsetX=ufz16(*pFlag++);
            bc.offsetY=ufz16(*pFlag++);
        } else {
            int8* p8=(int8*)pFlag++;
            bc.offsetX=*p8++;
            bc.offsetY=*p8++;
        }
        if ( bc.flags & WE_HAVE_A_SCALE ) {
            bc.scalex=fz16(*pFlag++);
            bc.scaley=bc.scalex;
        } else if ( bc.flags & WE_HAVE_AN_X_AND_Y_SCALE ) {
            bc.scalex=fz16(*pFlag++);
            bc.scaley=fz16(*pFlag++);
        } else if ( bc.flags & WE_HAVE_A_TWO_BY_TWO ) {
            bc.scalex=fz16(*pFlag++);
            bc.scale01=fz16(*pFlag++);
            bc.scale10=fz16(*pFlag++);
            bc.scaley=fz16(*pFlag++);
        }
        csList.append(bc);
    } while ( csList.last().flags & MORE_COMPONENTS );
    //if (csList[0].flags & WE_HAVE_INSTRUCTIONS){
    //    uint16 numInstr;
    //    uint8 instr[numInstr];
    //}
}

void Composite::yasuo(QVector<char> &ysData)
{
    //写入ysData,bigEndian
    ysData.clear();
    ysData<<(numberOfContours>>8)<<(numberOfContours&0xff);
    ysData<<(xMin>>8)<<(xMin&0xff);
    ysData<<(yMin>>8)<<(yMin&0xff);
    ysData<<(xMax>>8)<<(xMax&0xff);
    ysData<<(yMax>>8)<<(yMax&0xff);
    for(int i=0;i<this->csList.length();i++){
        csList[i].flags=ARGS_ARE_XY_VALUES;
        if(i<csList.length()-1){
            csList[i].flags|=MORE_COMPONENTS;
        }
        bool is16=false;
        if((csList[i].offsetX & 0xff80) || (csList[i].offsetY & 0xff80)){
            csList[i].flags|=ARG_1_AND_2_ARE_WORDS;
            is16=true;
        }
        if((csList[i].scalex !=F2DOT14::MOREN) || (csList[i].scaley !=F2DOT14::MOREN)){
            csList[i].flags|=(csList[i].scalex==csList[i].scaley?WE_HAVE_A_SCALE:WE_HAVE_AN_X_AND_Y_SCALE);
        }
        ysData<<(csList[i].flags>>8)<<(csList[i].flags&0xff);
        ysData<<(csList[i].glyphIndex>>8)<<(csList[i].glyphIndex&0xff);
        if (is16) {
            ysData<<(csList[i].offsetX>>8)<<(csList[i].offsetX&0xff);
            ysData<<(csList[i].offsetY>>8)<<(csList[i].offsetY&0xff);
        }else{
            ysData<<(csList[i].offsetX&0xff);
            ysData<<(csList[i].offsetY&0xff);
        }
        //写入scale
        if ( csList[i].flags & WE_HAVE_A_SCALE ) {
            ysData<<(csList[i].scalex.value>>8)<<(csList[i].scalex.value&0xff);
        } else if ( csList[i].flags & WE_HAVE_AN_X_AND_Y_SCALE ) {
            ysData<<(csList[i].scalex.value>>8)<<(csList[i].scalex.value&0xff);
            ysData<<(csList[i].scaley.value>>8)<<(csList[i].scaley.value&0xff);
        }
    }
}

void Composite::getPointCount(GLYFTable*pgl, uint16 &pointC, uint16 &lunkuoC)
{
    if(!pgl){
        return;
    }
    for(int i=0;i<this->csList.length();i++){
        pgl->getPointCount(csList[i].glyphIndex,pointC, lunkuoC);
    }
}

Biji &Composite::getBzrList(GLYFTable*pgl)
{
    bzr2List.clear();
    if(pgl){
        for(int i=0;i<this->csList.length();i++){
            PBiji bl=pgl->getBzrList(csList[i].glyphIndex);
            if(bl){
                Biji copybl=*bl;
                if(csList[i].flags&ARGS_ARE_XY_VALUES){
                    for(int bi=0;bi<copybl.length();bi++){
                        //先缩放
                        ///
                        ///         | scalex   0 |
                        /// (x,y) x |            | = (nx,ny)
                        ///         | 0    scaley|
                        ///
                        ///         | scalex   scale01 |
                        /// (x,y) x |                  | = (nx,ny)
                        ///         | scale10  scaley  |
                        ///
                        int16 tx=copybl[bi].x*csList[i].scalex+
                                copybl[bi].y*csList[i].scale10;
                        copybl[bi].y=copybl[bi].x*csList[i].scale01+
                                copybl[bi].y*csList[i].scaley;
                        copybl[bi].x=tx;
                        //再平移
                        copybl[bi].x+=csList[i].offsetX;
                        copybl[bi].y+=csList[i].offsetY;
                    }
                }
                bzr2List.append(copybl);
            }
        }
    }
    return this->bzr2List;
}

void Composite::appendBzr(const Biji &bl,const lunkuoFanwei& lfw, int lks)
{
    Q_UNUSED(bl);
    Q_UNUSED(lfw);
    Q_UNUSED(lks);
}

void Composite::tsx(int sxDelta)
{
    for(int i=0;i<this->csList.length();i++){
        if(csList[i].flags&ARGS_ARE_XY_VALUES){
            csList[i].offsetY+=sxDelta;
        }
    }
}

void Composite::zuoyouYidong(int zyDelta)
{
    for(int i=0;i<this->csList.length();i++){
        if(csList[i].flags&ARGS_ARE_XY_VALUES){
            csList[i].offsetX+=zyDelta;
        }
    }
}

void Composite::updateBindBox(GLYFTable*pgl){
    for(int i=0;i<this->csList.length();i++){
        lunkuoFanwei lfw;
        pgl->getLunkuofanwei(csList[i].glyphIndex,lfw,
                             csList[i].offsetX,csList[i].offsetY,
                             csList[i].scalex,csList[i].scaley);
        if(i==0){
            xMax=lfw.xMax;
            xMin=lfw.xMin;
            yMax=lfw.yMax;
            yMin=lfw.yMin;
        }else{
            if(xMax<lfw.xMax)xMax=lfw.xMax;
            if(xMin>lfw.xMin)xMin=lfw.xMin;
            if(yMax<lfw.yMax)yMax=lfw.yMax;
            if(yMin>lfw.yMin)yMin=lfw.yMin;
        }
    }
}

void Composite::suofang(bool fangda)
{
    Q_UNUSED(fangda);
}


glyfLink::glyfLink():pg(nullptr),isSimple(true){

}

glyfLink::~glyfLink()
{
    if(pg){
        delete pg;
    }
}


GLYFTable::GLYFTable(QObject *parent):TableTTF(parent),hmtx(nullptr){}

GLYFTable::~GLYFTable()
{
    for(int i=0;i<gl.length();i++){
        if(gl[i].pg){
            delete gl[i].pg;
            gl[i].pg=nullptr;
        }
        if(i%30==0){
            jindu(i*100/(gl.length()));
        }
    }
    jindu(100);
    gl.clear();
}
void GLYFTable::xinjian(uint16 tuxingzongshu){
    for(uint i=0;i<tuxingzongshu;i++){
        gl.append(glyfLink());
    }
}
void GLYFTable::read(QDataStream& dt, uint32 offset, LOCATable& lct,uint16 numGlyphs){
    dt.device()->seek(offset);
    for(uint i=0;i<numGlyphs;i++){
        uint16 len=lct.offsets[i+1]-lct.offsets[i];
        glyfLink x;
        gl.append(x);
        if(len==0)continue;
        gl[i].ysdata.fill(0,len);
        dt.readRawData(gl[i].ysdata.data(),len);
        uint16* pNofc=(uint16*)gl[i].ysdata.data();
        int16 check=Glyph::ufz16(pNofc[0]);
        if(check>=0){
            gl[i].pg=new Simple;
        }else{
            gl[i].pg=new Composite;
        }
        gl[i].pg->read(pNofc);
        if(i%30==0){
            jindu(i*100/(numGlyphs-1));
        }
    }
};
void GLYFTable::save(QDataStream& dt){
    for(int i=0;i<gl.length();i++){
        if(gl[i].pg){
            dt.writeRawData(gl[i].ysdata.data(),gl[i].ysdata.length());
        }
    }
}

void GLYFTable::show()
{
    QString info;
    for(int i=0;i<gl.length();i++){
        Glyph*sps=gl[i].pg;
        if(sps){
            info.append(QString("s%1:%2,xmin=%3,ymin=%4,xmax=%5,ymax=%6\r\n")
                        .arg(i).arg(sps->numberOfContours)
                        .arg(sps->xMin).arg(sps->yMin).arg(sps->xMax).arg(sps->yMax));
        }else{
            info.append(QString("x%1:无图\r\n").arg(i));
        }
    }
    datashow(info);
}

bool GLYFTable::bindBox(lunkuoFanwei &lfw,
                        FWORD& vMinlsb, FWORD& vMinRsb, FWORD&vMaxExt,
                        uint16 &jdLk, uint16 &jdDian, uint16 &fzLk, uint16 &fzDian,
                        uint16& avgWidth, const s32vector &azidx){
    //根据azidx计算avgWidth
    const int xishu[27]={
        //a-z,kongge
        64,14,27,35,100,20,14,42,63,3,6,35,20,56,56,17,4,49,56,71,31,10,18,3,18,2,166
    };
    uint64 sunaw=0;
    //uint32 numGlyphNotNull=0;
    for(int i=0;i<azidx.length();i++){
        Glyph*p=gl[azidx[i] ].pg;
        if(p){
            sunaw+=(p->xMax-p->xMin)*xishu[i];
        }
    }
    avgWidth=sunaw/1000;
    //
    //返回lsb在不在x=0
    // vMinRsb,vMaxExt vMinlsb不计算空字符
    //lfw 不计算空字符
    //计算包含的轮廓数量，点数量
    bool getFirst=false;
    vMinRsb=0;
    vMinlsb=0;
    vMaxExt=0;
    jdLk=0;
    jdDian=0;
    fzLk=0;
    fzDian=0;
    bool lsbAtY=true;
    for(int i=0;i<gl.length();i++){
        Glyph*p=gl[i].pg;
        if(p){
            uint16 ds=0,lks=0;
            p->getPointCount(this,ds,lks);
            if(p->numberOfContours>0){
                if(lks>jdLk)jdLk=lks;
                if(ds>jdDian)jdDian=ds;
            }else{
                if(lks>fzLk)fzLk=lks;
                if(ds>fzDian)fzDian=ds;
            }
            //
            //sunaw+=p->xMax-p->xMin;
            //numGlyphNotNull+=1;
            //
            if(!getFirst){
                getFirst=true;
                lfw.xMin=p->xMin,lfw.yMin=p->yMin,lfw.xMax=p->xMax,lfw.yMax=p->yMax;
                vMinRsb=hmtx->getAw(i)-hmtx->fzhMetricsAll[i].lsb
                        -(p->xMax-p->xMin);
                vMaxExt=hmtx->fzhMetricsAll[i].lsb+(p->xMax-p->xMin);
                vMinlsb=hmtx->fzhMetricsAll[i].lsb;
                if(p->xMin!=hmtx->fzhMetricsAll[i].lsb){
                    lsbAtY=false;
                }
            }else {
                if(p->xMin<lfw.xMin)lfw.xMin=p->xMin;
                if(p->yMin<lfw.yMin)lfw.yMin=p->yMin;
                if(p->xMax>lfw.xMax)lfw.xMax=p->xMax;
                if(p->yMax>lfw.yMax)lfw.yMax=p->yMax;
                int16 rsb=hmtx->getAw(i)-hmtx->fzhMetricsAll[i].lsb
                        -(p->xMax-p->xMin);
                if(rsb<vMinRsb)vMinRsb=rsb;
                int16 maxExt=hmtx->fzhMetricsAll[i].lsb+(p->xMax-p->xMin);
                if(maxExt>vMaxExt)vMaxExt=maxExt;
                int16 lsb=hmtx->fzhMetricsAll[i].lsb;
                if(lsb<vMinlsb)vMinlsb=lsb;
                if(p->xMin!=hmtx->fzhMetricsAll[i].lsb){
                    lsbAtY=false;
                }
            }
        }
    }
    //avgWidth=sunaw/numGlyphNotNull;
    return lsbAtY;
}

void GLYFTable::setHmtx(HMTXTable* vhmtx){
    hmtx=vhmtx;
}

void GLYFTable::jisuan(LOCATable& lct, zidongZifu zz){
    if(zz&zzNotdef){
        glyfLink glnotdef;
        gl.prepend(glnotdef);
        hmtx->fzhMetricsAll.prepend(LongHorMetric());
    }
    if(zz&zzKongge){
        glyfLink glkg;
        gl.insert(1,glkg);
        hmtx->fzhMetricsAll.insert(1,LongHorMetric());
    }
    lct.offsets.clear();
    lct.offsets.append(0);
    Offset32 begin=0;
    for(int i=0;i<gl.length();i++){
        if(gl[i].pg){
            //压缩图形数据
            gl[i].pg->yasuo(gl[i].ysdata);
            begin+=gl[i].ysdata.length();
        }
        lct.offsets.append(begin);
    }
}

void GLYFTable::deleteWenzi(s32vector &glyphIdxSel){
    //更新组合字的引用  删除则清零，其他则移位
    sort(glyphIdxSel.begin(),glyphIdxSel.end());
    for(int i=0;i<gl.length();i++){                               //遍历全部图形
        if(gl[i].pg && gl[i].pg->numberOfContours<0){             //找到组合字
            Composite*fz=dynamic_cast<Composite*>(gl[i].pg);
            if(fz){
                for(int fi=0;fi<fz->csList.length();fi++){        //遍历组合字符的组成部分
                    int js=0;
                    for(int di=0;di<glyphIdxSel.length();di++){   //遍历待删除的图形，确定组成部件的新图索引
                        if(fz->csList[fi].glyphIndex==(uint16)glyphIdxSel[di]){//已删除则清零
                            fz->csList[fi].glyphIndex=0;
                            js=0;
                            break;
                        }else if(fz->csList[fi].glyphIndex>(uint16)glyphIdxSel[di]){//需要移位。
                            js+=1;
                        }
                    }
                    if(js!=0){
                        fz->csList[fi].glyphIndex-=js;               //更新：新图索引
                    }
                }
            }
        }
    }
    //删除图形原始数据
    for(int i=glyphIdxSel.length()-1;i>=0;i--){
        gl.removeAt(glyphIdxSel[i]);
    }
}

void GLYFTable::deleteTuxing(s32vector &glyphIdxSel){
    //删除图形以后，pg释放。
    for(int i=glyphIdxSel.length()-1;i>=0;i--){
        int gidx=glyphIdxSel[i];
        if(gl[gidx].pg){
            delete gl[gidx].pg;
            gl[gidx].pg=nullptr;
        }
    }
}

void GLYFTable::appendBianma(int bccnt){
    //补充空的字符
    for(int i=0;i<bccnt;i++){
        glyfLink x;
        gl.append(x);
    }
}

int GLYFTable::zuoyouYidong(int32 idx,int newXmin){
    //idx glyphIndex
    if(!gl[idx].pg){
        return 0;
    }
    int zyDelta=newXmin-gl[idx].pg->xMin;
    if(zyDelta!=0){
        gl[idx].pg->xMax+=zyDelta;
        gl[idx].pg->xMin=newXmin;
        gl[idx].pg->zuoyouYidong(zyDelta);
        //
        hmtx->fzhMetricsAll[idx].lsb=newXmin;
        if(-zyDelta>=hmtx->fzhMetricsAll[idx].advanceWidth){
            hmtx->fzhMetricsAll[idx].advanceWidth=0;
        }else{
            hmtx->fzhMetricsAll[idx].advanceWidth+=zyDelta;
        }
    }
    return zyDelta;
}

void GLYFTable::zidongDuiqi(int zx,int capx)
{
    Q_UNUSED(zx);
    Q_UNUSED(capx);
    //上下：全部居中对齐，第一个不处理，空格（20 a0 3000)不处理，标点（,.，。、_）在基线，引号在大写线（‘’“”'"）
    //左右：前后间距10，空格宽一半，3000全宽，
    for(int i=1;i<gl.length();i++){

    }
}
void GLYFTable::clearFuzhuxian(){
    fzxList.clear();
}
bool GLYFTable::fuzhuxianYicunzai(int y){
    //true=辅助线已存在
    for(int i=0;i<fzxList.length();i++){
        if(fzxList[i].y==y){
            return true;
        }
    }
    return false;
}

void GLYFTable::addFuzhuxian(chuizhiDuiqi dqfs, int32 glyphIndex){
    //增加辅助线
    lunkuoFanwei lfw;
    bool yt=getLunkuofanwei(glyphIndex,lfw);
    if(!yt){
        datashow("没有笔迹",true);
        return;
    }

    if(dqfs==cdYmax){
        if(fuzhuxianYicunzai(lfw.yMax)){
            datashow("辅助线已存在",true);
            return;
        }
        fzxList.append(fuzhuxian(cdYmax,lfw.yMax));
    }else if(dqfs==cdYmin){
        if(fuzhuxianYicunzai(lfw.yMin)){
            datashow("辅助线已存在",true);
            return;
        }
        fzxList.append(fuzhuxian(cdYmin,lfw.yMin));
    }
    //排序
    sort(fzxList.begin(),fzxList.end(),[](const fuzhuxian&a,const fuzhuxian&b){
        //从大到小
        return a.y>b.y;
    });
}

int GLYFTable::tsxFromFzx(int32 glyphIdx, int fzxidx){
    if(!gl[glyphIdx].pg){
        return 0;
    }
    //指定新的yMax
    int oldymax=gl[glyphIdx].pg->yMax;
    if(fzxidx<0||fzxidx>=fzxList.length()){
        return oldymax;
    }
    return tsx(glyphIdx,fzxList[fzxidx].y,fzxList[fzxidx].dqfs);
}

int GLYFTable::tsx(int32 idx, int newMax, chuizhiDuiqi dqfs){
    //返回deltay
    //无图的按0处理
    if(!gl[idx].pg){
        return 0;
    }
    //指定新的yMax
    int oldymax=gl[idx].pg->yMax;
    int sxDelta=newMax-oldymax;
    if(dqfs==cdJuzhong){
        //居中对齐
        int newZx=newMax;
        int oldZx=oldymax-(oldymax-gl[idx].pg->yMin)/2;
        sxDelta=newZx-oldZx;
    }else if(dqfs==cdYmin){
        //指定新的Ymin
        sxDelta=newMax-gl[idx].pg->yMin;
    }
    if(sxDelta!=0){
        gl[idx].pg->yMax+=sxDelta;
        gl[idx].pg->yMin+=sxDelta;
        gl[idx].pg->tsx(sxDelta);
    }
    return sxDelta;
}

void GLYFTable::getKongzifu(s32vector& xszf){
    //获取空字符。没有笔迹的字符。
    xszf.clear();
    for(int i=0;i<gl.length();i++){
        if(!gl[i].pg){
            xszf.append(i);
        }
    }
}

void GLYFTable::getZuhezi(s32vector& xszf){
    xszf.clear();
    for(int i=0;i<gl.length();i++){
        if(gl[i].pg && gl[i].pg->numberOfContours<0){
            xszf.append(i);
        }
    }
}
void GLYFTable::getPutongzi(s32vector& xszf){
    xszf.clear();
    for(int i=0;i<gl.length();i++){
        if(gl[i].pg && gl[i].pg->numberOfContours>0){
            xszf.append(i);
        }
    }
}

int GLYFTable::lsbDengyuXMin(int32 glyphIndex){
    //lsb==xmin,left side bearing at x=0
    //勾选，保证lsb=xmin。无图形的，lsb=0.
    //返回新的lsb
    if(!gl[glyphIndex].pg){
        return 0;
    }
    //δ=xMin-lsb
    int delta=gl[glyphIndex].pg->xMin-hmtx->fzhMetricsAll[glyphIndex].lsb;
    if(delta!=0){
        //lsb=xMin
        hmtx->fzhMetricsAll[glyphIndex].lsb=gl[glyphIndex].pg->xMin;
        if(-delta>=hmtx->fzhMetricsAll[glyphIndex].advanceWidth){
            hmtx->fzhMetricsAll[glyphIndex].advanceWidth=0;
        }else{
            hmtx->fzhMetricsAll[glyphIndex].advanceWidth+=delta;
        }
    }
    return hmtx->fzhMetricsAll[glyphIndex].lsb;
}

bool GLYFTable::getLunkuofanwei(int32 glyphIndex, lunkuoFanwei& lfw,
                                int offx,int offy,double scalex,double scaley){
    //返回是否有图。true=有图
    //lfw:返回轮廓范围
    if(glyphIndex<0){
        return false;
    }
    if(gl[glyphIndex].pg){
        lfw=lunkuoFanwei(gl[glyphIndex].pg->xMin*scalex+offx,gl[glyphIndex].pg->yMin*scaley+offy,
                         gl[glyphIndex].pg->xMax*scalex+offx,gl[glyphIndex].pg->yMax*scaley+offy);
        return true;
    }else{
        return false;
    }
}

PBiji GLYFTable::getBzrList(int32 glyphIndex){
    if(gl[glyphIndex].pg){
        return &(gl[glyphIndex].pg->getBzrList(this));
    }else{
        return nullptr;
    }
}


void GLYFTable::getPointCount(int32 glyphIndex, uint16 &ds, uint16&lks){
    if(gl[glyphIndex].pg){
        gl[glyphIndex].pg->getPointCount(this,ds,lks);
    }
}

void GLYFTable::appendBzr(int32 glyphIndex, const Biji& bl, const lunkuoFanwei& lfw, int lks){
    if(glyphIndex>=gl.length()){
        return;
    }
    if(gl[glyphIndex].pg){
        gl[glyphIndex].pg->appendBzr(bl,lfw,lks);
    }else{
        gl[glyphIndex].pg=new Simple;
        gl[glyphIndex].pg->appendBzr(bl,lfw,lks);
    }
}

PBjCanshuList GLYFTable::tongjiLunkuoDian(int32 glyphIndex, QString &tj){
    //统计轮廓的数量 点的数量 补充的点数量
    //如果是组合字，返回组合字的部件列表
    if(gl[glyphIndex].pg){
        uint16 lks=0,ds=0;
        gl[glyphIndex].pg->getPointCount(this,ds,lks);
        tj=QString("轮廓数量：%1\r\n点的数量：%2").arg(lks).arg(ds);
        if(gl[glyphIndex].pg->numberOfContours<0){
            Composite*fz=dynamic_cast<Composite*>(gl[glyphIndex].pg);
            return &fz->csList;
        }
        return nullptr;
    }
    return nullptr;
}

void GLYFTable::xiugaiBujianGlyphIndex(int32 bujianGlyphIdx, int16 bujianIdx, int32 zuheziGlyphIdx){
    //uint32 bujianGlyphIdx,组合字符需要的部件glyphidx
    //int bujianIdx, 组合字符的第几个部件
    //uint32 zuheziGlyphIdx 组合字符的glyphIdx
    Composite*fz=dynamic_cast<Composite*>(gl[zuheziGlyphIdx].pg);
    if(fz){
        if(bujianIdx<fz->csList.length()){
            fz->csList[bujianIdx].glyphIndex=(uint16)bujianGlyphIdx;
        }
    }
}
void GLYFTable::jiayizu(int32 zuheziGlyphIdx){
    //zuheziGlyphIdx 组合字的glyphIdx
    Composite*fz=dynamic_cast<Composite*>(gl[zuheziGlyphIdx].pg);
    if(fz){
        fz->csList.append(BujianCanshu());
    }
}
bool GLYFTable::shanyizu(int32 zuheziGlyphIdx, int bujianIdx){
    //zuheziGlyphIdx 组合字的glyphIdx
    Composite*fz=dynamic_cast<Composite*>(gl[zuheziGlyphIdx].pg);
    if(fz){
        if(fz->csList.length()>1){
            fz->csList.removeAt(bujianIdx);
            return true;
        }
    }
    return false;
}
int GLYFTable::fzgbXY(int32 zuheziGlyphIdx, BujianGaibian bg, int bujianIdx, int xy, double scale){
    //返回新的ymax
    //xy 偏移
    //scale 缩放
    Composite*fz=dynamic_cast<Composite*>(gl[zuheziGlyphIdx].pg);
    if(fz){
        switch(bg){
        case bgX:fz->csList[bujianIdx].offsetX=xy;break;
        case bgY:fz->csList[bujianIdx].offsetY=xy;break;
        case bgXScale:fz->csList[bujianIdx].scalex=  scale;break;
        case bgYScale:fz->csList[bujianIdx].scaley=  scale;break;
        case bgScale01:fz->csList[bujianIdx].scale01=scale;break;
        case bgScale10:fz->csList[bujianIdx].scale10=scale;break;
        default:break;
        }
        fz->updateBindBox(this);
        lsbDengyuXMin(zuheziGlyphIdx);
        //rsb=10
        hmtx->fzhMetricsAll[zuheziGlyphIdx].advanceWidth=fz->xMax+10;
        return fz->yMax;
    }
    return 0;
}

void GLYFTable::chaYinyong(int32 wmgid, s32vector& jg){
    //wmgig:一个字符（可能是组合字的部件）的gid。
    //jg:查询到的结果，搜集使用wmgid的组合字的gid
    jg.clear();
    for(int i=0;i<gl.length();i++){
        if(gl[i].pg && gl[i].pg->numberOfContours<0){
            Composite*zh=dynamic_cast<Composite*>(gl[i].pg);
            for(int ci=0;ci<zh->csList.length();ci++){
                if(zh->csList[ci].glyphIndex==(uint16)wmgid){
                    jg.append(i);
                    break;
                }
            }
        }
    }
}

bool GLYFTable::isSimple(int32 glyphIndex){
    return(gl[glyphIndex].pg && gl[glyphIndex].pg->numberOfContours>0);
}
void GLYFTable::gaiweiZuhezi(int32 glyphIndex, int16 bj1Gid,
                             int16 bj1xOff, int16 bj1yOff, double bj1xScale, double bj1yScale,
                             int16 bj2xOff, int16 bj2yOff, double bj2xScale, double bj2yScale){
    //自动做两个部件，并指定第一个部件 bj1Gid,
    //偏移 缩放
    if(gl[glyphIndex].pg){
        delete gl[glyphIndex].pg;
    }
    Composite * pc=new Composite;
    gl[glyphIndex].pg=pc;
    pc->numberOfContours=-1;
    pc->csList.clear();
    pc->csList.append(BujianCanshu(bj1Gid,bj1xOff,bj1yOff,bj1xScale,bj1yScale));
    pc->csList.append(BujianCanshu(0,     bj2xOff,bj2yOff,bj2xScale,bj2yScale));
}

void GLYFTable::shanchuLunkuo(int32 glyphIndex,int lksel){
    //简单字，删除指定的轮廓
    if(lksel<0)return;
    PBiji xds=getBzrList(glyphIndex);
    if(xds){
        //找到第一点
        int i=0;
        for(int ti=0;ti<lksel;ti++){
            while(xds->at(i++).lkEnd==false);
        }
        int first=i;
        //找到此轮廓的终点
        while(xds->at(i++).lkEnd==false);
        i--;
        //删除
        xds->erase(xds->begin()+first,xds->begin()+i+1);
        //更新包围矩形 轮廓数
        gl[glyphIndex].pg->numberOfContours-=1;
        gl[glyphIndex].pg->updateBindBox(this);
        //修正lsb，删除轮廓不改动lsb
        hmtx->setLsb(glyphIndex, gl[glyphIndex].pg->xMin);
    }
}

void GLYFTable::shanchuDian(int32 glyphIndex, lunkuoFanwei xuanzeBianjie){
    //简单字，删除全部框选的点
    //一个轮廓剩余不够3个点则全部清除
    /// 查轮廓每个点（本身无插点），发现不在框内的点，立即删除
    /// 一个轮廓查完时检查是否不足3个--全删除，轮廓数量-1；检查end是否存在--不存在要补上。
    /// 巡查结束，重新计算包围矩形，修正lsb以便剩余笔迹位置不变。
    ///
    ///            i=0
    ///          lkb=0
    ///            |
    ///  /---------|
    ///  |         |
    ///  |         |
    ///  ↑         |
    ///  |         |            删除此轮廓全部点 |
    ///  |         |            轮廓数量-1     |
    ///  |         |           i退回到轮廓起始  |
    ///  |         |          ----------------
    ///  |         |
    ///  |       i需要删除------------------------|
    ///  |         y|                           n|
    ///  |          |                            |
    ///  |        是轮廓末尾-------------|       是轮廓末尾--------------|
    ///  |          y|                 n|        y|                 n|
    ///  |           |                  |         |                  |
    ///  |         不够4点-----|         |       不够3点-----|         |
    ///  ↑          y|       n|         |         |       n|         |
    ///  |--←←←-----调        |         |  O←←←---调       |          |
    ///  ↑                    |         |                 |          |
    ///  |      前一点标记为轮廓终点        |     i的下一点作为新轮廓开始    |
    ///  |     当前序号i作为新轮廓开始      |        |                   |
    ///  |           |                  /        |                   |
    ///  |           \                 /         |                   |
    ///  |            \_______________/       ___|_____              |
    ///  |            |  删除i位置的点  |      | 查下一个点|-------------|
    ///  |--←←←-------\______________/       |_________|
    ///  |                                         |
    ///  |--←←←------------------------------------/
    ///
    ///
    PBiji xds=getBzrList(glyphIndex);
    if(xds){
        Glyph* pg = gl[glyphIndex].pg;
        int lkBegin=0;
        int i=0;
        while(i<xds->length()){
            if(((*xds)[i].x<=xuanzeBianjie.xMax) && ((*xds)[i].x>=xuanzeBianjie.xMin)&&
                    ((*xds)[i].y<=xuanzeBianjie.yMax) && ((*xds)[i].y>=xuanzeBianjie.yMin)){
                if(xds->at(i).lkEnd){
                    if(i-lkBegin<3){
                        for(int xi=0;xi<=i-lkBegin;xi++){
                            xds->removeAt(lkBegin);
                        }
                        pg->numberOfContours-=1;
                        i=lkBegin;
                        continue;
                    }else{
                        (*xds)[i-1].lkEnd=true;
                        lkBegin=i;
                    }
                }
                xds->removeAt(i);
                continue;
            }else if(xds->at(i).lkEnd){
                if(i-lkBegin<2){
                    for(int xi=0;xi<=i-lkBegin;xi++){
                        xds->removeAt(lkBegin);
                    }
                    pg->numberOfContours-=1;
                    i=lkBegin;
                    continue;
                }else{
                    lkBegin=i+1;
                }
            }
            ++i;
        }

        pg->updateBindBox(this);
        //修正lsb
        hmtx->setLsb(glyphIndex, pg->xMin);
    }
}
void GLYFTable::shanchuLunkuo(int32 glyphIndex,lunkuoFanwei xuanzeBianjie){
    //简单字，删除全部框选的轮廓
    /// 查轮廓的每个点，发现不在框内的点，跳到下一个轮廓
    /// 遇到轮廓终点时，把这个轮廓标记为删除（记录起点和终点）
    /// 巡查结束，从笔迹中删除标记的全部轮廓，更新轮廓数量，重新计算包围矩形，修正lsb以便剩余笔迹位置不变。
    PBiji xds=getBzrList(glyphIndex);
    if(xds){
        int delBegin=-1;
        vector<pair<int,int> >del;
        for(int i=0;i<xds->length();i++){
            if(((*xds)[i].x<=xuanzeBianjie.xMax) && ((*xds)[i].x>=xuanzeBianjie.xMin)&&
                    ((*xds)[i].y<=xuanzeBianjie.yMax) && ((*xds)[i].y>=xuanzeBianjie.yMin)){
                if(delBegin==-1){
                    delBegin=i;//第一点在框内
                }else if((*xds)[i].lkEnd){
                    //最后一点也在框内。删。
                    del.push_back(pair<int,int>(delBegin,i));
                    delBegin=-1;
                }
            }else{
                //不在框内
                delBegin=-1;
                //找到下一个轮廓
                while((*xds)[i++].lkEnd==false);
                i--;
            }
        }
        if(del.size()>0){
            for(auto i=del.end()-1;i>=del.begin();i--){
                xds->erase(xds->begin()+i->first,xds->begin()+i->second+1);
            }
            //
            Glyph* pg = gl[glyphIndex].pg;
            pg->numberOfContours-=del.size();
            pg->updateBindBox(this);
            //修正lsb
            hmtx->setLsb(glyphIndex, pg->xMin);
        }
    }
}
void GLYFTable::tihuanZifu(int32 glyphIndex, const Biji &biji, int lks, int16 newZx, int16 newHeightMax){
    if(gl[glyphIndex].pg){
        delete gl[glyphIndex].pg;
    }
    Simple* ps=new Simple;
    gl[glyphIndex].pg = ps;
    ps->numberOfContours=lks;
    ps->bzr2List=biji;
    ps->updateBindBox(this);
    //自动缩放
    if(newHeightMax!=0){
        ps->suofang(1.0*newHeightMax/(ps->yMax-ps->yMin));
    }
    //自动调整,0 表示不调整
    const int zidongLSB=5;
    const int zidongRSB=5;
    if(newZx!=0){
        int oldZx=ps->yMax-(ps->yMax-ps->yMin)/2;
        int sxDelta=newZx-oldZx;
        int zyDelta=zidongLSB-ps->xMin;
        ps->yMax+=sxDelta;
        ps->yMin+=sxDelta;
        ps->xMax+=zyDelta;
        ps->xMin+=zyDelta;
        ps->tsxzy(sxDelta,zyDelta);
    }
    hmtx->fzhMetricsAll[glyphIndex].lsb=ps->xMin;
    hmtx->fzhMetricsAll[glyphIndex].advanceWidth=ps->xMax+(newZx!=0?zidongRSB:0);
}


void GLYFTable::suofang(int32 glyphIndex, bool fangda){
    if(gl[glyphIndex].pg){
        gl[glyphIndex].pg->suofang(fangda);
    }
}

lunkuoFanwei::lunkuoFanwei(int16 vxMin, int16 vyMin, int16 vxMax, int16 vyMax)
    :xMin(vxMin),yMin(vyMin),xMax(vxMax),yMax(vyMax)
{

}

LongHorMetric::LongHorMetric(uint vaw, int vlsb):advanceWidth(vaw),lsb(vlsb)
{

}
