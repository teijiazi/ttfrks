#ifndef TABLEGLYF_H
#define TABLEGLYF_H
#include <types.h>
#include <tableCmap.h>

class LongHorMetric{
public:
    LongHorMetric(uint vaw=100,int vlsb=0);
    uint16	advanceWidth;//	Advance width, in font design units.
    int16	lsb;//	Glyph left side bearing, in font design units.
};
class HMTXTable:public TableTTF{//必须
    Q_OBJECT
public:
    HMTXTable(QObject *parent=nullptr);
    void read(QDataStream& dt, uint32 offset,uint16 numberOfHMetrics,uint16 numGlyphs);
    void save(QDataStream& dt);
    void show();
    void xinjian(uint16 tuxingzongshu);
    void deleteWenzi(const s32vector &glyphIdxSel);
    uint16 getAw(int32 glyphIdx) const;
    int16 getLsb(int32 glyphIdx);
    void jisuan(UFWORD &maxAw, uint16 &vNumberOfHMetrics);
    FWORD minLsb();
    void appendBianma(int bccnt);
    void deleteTuxing(s32vector &glyphIdxSel);
    QList<LongHorMetric> fzhMetricsAll;
    void setLsbAw(int32 glyphIdx, int16 lsb, uint16 aw);
    void setLsb(int32 glyphIdx, int16 lsb);
private:
    //longHorMetric	hMetrics[numberOfHMetrics]	Paired advance width and left side bearing values for each glyph. Records are indexed by glyph ID.
    QList<LongHorMetric> hMetrics;
    //int16	leftSideBearings[numGlyphs - numberOfHMetrics]	Left side bearings for glyph IDs greater than or equal to numberOfHMetrics.
    QList<int16> leftSideBearings;
};

class LOCATable:public TableTTF{//ttfbx
    Q_OBJECT
public:
    LOCATable(QObject *parent=nullptr);
    ~LOCATable();
    void read(QDataStream& dt, uint32 offset,uint16 cd,uint16 numGlyphs);
    void save(QDataStream& dt);
    void show();
    //Offset16	offsets[n]	The actual local offset divided by 2 is stored. The value of n is numGlyphs + 1. The value for numGlyphs is found in the 'maxp' table.
    //Offset32	offsets[n]	The actual local offset is stored. The value of n is numGlyphs + 1. The value for numGlyphs is found in the 'maxp' table.
    QVector<Offset32> offsets;
signals:
};

/** @brief 二阶Bezer曲线上的一个点
@param strModel The string variable containing the model you want to load.
@param objname The optional name of the node to read (if empty, the first top-level node will be used)
This is static template method of Algorithm. It's usage is following (in the case of SVM):
@code
Ptr<SVM> svm = Algorithm::loadFromString<SVM>(myStringModel);
@endcode
*/
class bzr2{
public:
    bzr2():isControl(false),lkEnd(false),wojia(false){}
    int16 x;//!<x,y=绝对坐标
    int16 y;
    bool isControl;//!<true=不在曲线上，是控制点，offCurve，\nfalse=在曲线上,onCurve
    bool lkEnd;    //!<轮廓结束
    bool wojia;    //!<增补的onCurve点
};

typedef QList<bzr2> Biji,*PBiji;

class lunkuoFanwei{
public:
    lunkuoFanwei(int16 vxMin=0, int16 vyMin=0, int16 vxMax=0, int16	vyMax=0);
    int16	xMin;//!<	Minimum x for coordinate data.
    int16	yMin;//!<	Minimum y for coordinate data.
    int16	xMax;//!<	Maximum x for coordinate data.
    int16	yMax;//!<	Maximum y for coordinate data.
};
class GLYFTable;
class Glyph{
public:
    Glyph();
    virtual ~Glyph();
    int16	numberOfContours;//!<If the number of contours is greater than or equal to zero, this is a simple glyph. If negative, this is a composite glyph — the value -1 should be used for composite glyphs.
    int16	xMin;//	Minimum x for coordinate data.
    int16	yMin;//	Minimum y for coordinate data.
    int16	xMax;//	Maximum x for coordinate data.
    int16	yMax;//	Maximum y for coordinate data.
    virtual void read(uint16 *p)=0;
    virtual void yasuo(QVector<char> &ysData)=0;
    virtual void getPointCount(GLYFTable*pgl, uint16 &pointC, uint16 &lunkuoC)=0;
    virtual Biji& getBzrList(GLYFTable*pgl)=0;
    virtual void tsx(int sxDelta)=0;
    virtual void zuoyouYidong(int zyDelta)=0;
    virtual void appendBzr(const Biji& bl,const lunkuoFanwei& lfw, int lks)=0;
    static uint16 ufz16(uint16 v);
    int16 fz16(uint16 v);
    virtual void suofang(bool fangda)=0;
    Biji bzr2List;          //!<一个字符的全部点
    virtual void updateBindBox(GLYFTable *pgl)=0;
};

class Simple:public Glyph{
public:
    Simple();
    ~Simple();
    void read(uint16 *p)override;
    enum{
        curveTest=0x01,
        REPEAT_FLAG=0x08,
        xTest=0x12,     //00010010
        POSITIVE_X=0x12,
        fuX=0x02,
        X_IS_SAME=0x10,
        is16b=0x00,
        yTest=0x24,     //00100100
        POSITIVE_Y=0x24,     //00100100
        fuY=0x04,     //00000100
        Y_IS_SAME=0x20,     //00100000
    };
    //simple
    //uint16  endPtsOfContours[numberOfContours]	Array of point indices for the last point of each contour, in increasing numeric order.
    u16vector endPtsOfContours;//!<存储-压缩的时候要clear然后再写入。
    uint16	instructionLength;//Total number of bytes for instructions. If instructionLength is zero, no instructions are present for this glyph, and this field is followed directly by the flags field.
    //uint8	instructions[instructionLength]	Array of instruction byte code for the glyph.
    QVector<uint8> instructions;//
    //uint8	flags[variable]	Array of flag elements. See below for details regarding the number of flag array elements.
    //uint8 or int16	xCoordinates[variable]	Contour point x-coordinates. See below for details regarding the number of coordinate array elements. Coordinate for the first point is relative to (0,0); others are relative to previous point.
    //uint8 or int16	yCoordinates[variable]	Contour point y-coordinates. See below for details regarding the number of coordinate array elements. Coordinate for the first point is relative to (0,0); others are relative to previous point.
    //
    void getPointCount(GLYFTable*pgl, uint16 &pointC, uint16 &lunkuoC)override;
    void yasuo(QVector<char> &ysData)override;
    Biji& getBzrList(GLYFTable*pgl)override;
    void appendBzr(const Biji& bl,const lunkuoFanwei& lfw, int lks)override;
    void tsx(int sxDelta)override;
    void zuoyouYidong(int zyDelta)override;
    void tsxzy(int sxDelta, int zyDelta);
    void suofang(bool fangda)override;
    void suofang(double suofangBili);
    void updateBindBox(GLYFTable *pg) override;
};
/// 组合字的部件参数
class BujianCanshu{
public:
    BujianCanshu(uint16 vglyphIndex=0,int offX=0,int offY=0, double xscale=1.0,double yscale=1.0):flags(0x0002),
        glyphIndex(vglyphIndex),offsetX(offX),offsetY(offY),
        scalex(xscale),scaley(yscale),
        scale01(0.0),scale10(0.0){}
    uint16	flags;     //!<	component flag
    uint16	glyphIndex;//!<	glyph index of component
    //uint8, int8, uint16 or int16	argument1;//	x-offset for component or point number; type depends on bits 0 and 1 in component flags
    //uint8, int8, uint16 or int16	argument2;//	y-offset for component or point number; type depends on bits 0 and 1 in component flags
    int16 offsetX;//存储时检测是16bit还是8bit。pointnumber转为offset存储。
    int16 offsetY;
    F2DOT14  scalex;//WE_HAVE_A_SCALE WE_HAVE_AN_X_AND_Y_SCALE  WE_HAVE_A_TWO_BY_TWO
    F2DOT14  scaley;//                WE_HAVE_AN_X_AND_Y_SCALE  WE_HAVE_A_TWO_BY_TWO
    F2DOT14  scale01;//                                         WE_HAVE_A_TWO_BY_TWO
    F2DOT14  scale10;//                                         WE_HAVE_A_TWO_BY_TWO
};
typedef QList<BujianCanshu> BujianCanshuList, *PBjCanshuList;

///标记组合字的其中一个部件的参数发生了改变
enum BujianGaibian{
    bgHuanyige,//!<替换部件--开始选部件
    bgAppend,  //!<增加部件
    bgDelete,  //!<删除部件
    bgX,
    bgY,
    bgXScale,
    bgYScale,
    bgScale01,
    bgScale10,
    bgXuanhao,//!<替换部件--部件已经选好
};

class Composite:public Glyph{
public:
    BujianCanshuList csList;//!<一个字符的全部组件
    Composite();
    ~Composite();
    enum{
        ARG_1_AND_2_ARE_WORDS=0x0001,
        ARGS_ARE_XY_VALUES=0x0002,
        ROUND_XY_TO_GRID=0x0004,
        WE_HAVE_A_SCALE=0x0008,
        MORE_COMPONENTS=0x0020,
        WE_HAVE_AN_X_AND_Y_SCALE=0x0040,
        WE_HAVE_A_TWO_BY_TWO=0x0080,
        WE_HAVE_INSTRUCTIONS=0x0100,
        USE_MY_METRICS=0x0200,
        OVERLAP_COMPOUND=0x0400,
        SCALED_COMPONENT_OFFSET=0x0800,
        UNSCALED_COMPONENT_OFFSET=0x1000
    };
    void read(uint16 *p)override;
    void yasuo(QVector<char> &ysData)override;
    void getPointCount(GLYFTable*pgl, uint16 &pointC, uint16 &lunkuoC)override;
    Biji& getBzrList(GLYFTable*pgl)override;
    void appendBzr(const Biji &bl, const lunkuoFanwei &lfw, int lks)override;
    void tsx(int sxDelta)override;
    void zuoyouYidong(int zyDelta)override;
    void updateBindBox(GLYFTable*pgl)override;
    void suofang(bool fangda)override;
};

const int biaodianXiaLen=6;
const int biaodianShangLen=6;
const uint16 biaodianXia[biaodianXiaLen]={
    //，。,.、_
    0xff0c,0x3002,0x2c,0x2e,0x3001,0x5f};
const uint16 biaodianShang[biaodianShangLen]={
    //‘’“”'"
    0x2018,0x2019,0x201c,0x201d,0x27,0x22
};

class glyfLink{
public:
    glyfLink();
    ~glyfLink();
    Glyph* pg;
    QVector<char> ysdata;
    bool isSimple;
};

enum chuizhiDuiqi{
    cdYmax,//调整ymax,指定新的ymax
    cdJuzhong,//指定新的中线，ymax与ymin的中线与新的中线对齐
    cdYmin,//指定新的yMin
};

class fuzhuxian{
public:
    fuzhuxian(chuizhiDuiqi vdqfs,int vy):dqfs(vdqfs),y(vy){}
    chuizhiDuiqi dqfs;
    int y;
};

typedef QList<QPainterPath> LPP, *PLPP;
class GLYFTable:public TableTTF{//ttfbx
    Q_OBJECT
    bool fuzhuxianYicunzai(int y);
public:
    GLYFTable(QObject *parent=nullptr);
    ~GLYFTable();
    void read(QDataStream& dt, uint32 offset, LOCATable& lct,uint16 numGlyphs);
    void save(QDataStream& dt);
    void show();
    bool bindBox(lunkuoFanwei &lfw, FWORD &vMinlsb,
                 FWORD &vMinRsb, FWORD &vMaxExt,
                 uint16 &jdLk, uint16 &jdDian, uint16 &fzLk, uint16 &fzDian,
                 uint16 &avgWidth, const s32vector& azidx);
    void jisuan(LOCATable& lct, zidongZifu zz);
    void deleteWenzi(s32vector &glyphIdxSel);
    int tsx(int32 idx, int newMax, chuizhiDuiqi dqfs=cdYmax);
    int zuoyouYidong(int idx, int newXmin);
    void zidongDuiqi(int zx, int capx);
    ///获取空字符。没有笔迹的字符。
    void getKongzifu(s32vector &xszf);
    int lsbDengyuXMin(int32 glyphIndex);
    /// 获取轮廓范围，返回是否有图。true=有图
    bool getLunkuofanwei(int32 glyphIndex, lunkuoFanwei &lfw,
                         int offx=0, int offy=0, double scalex=1.0, double scaley=1.0);
    void appendBianma(int bccnt);
    void deleteTuxing(s32vector &glyphIdxSel);
    void addFuzhuxian(chuizhiDuiqi dqfs, int32 glyphIndex);
    void clearFuzhuxian();
    int tsxFromFzx(int32 glyphIdx, int fzxidx);
    PBiji getBzrList(int32 glyphIndex);
    void xinjian(uint16 tuxingzongshu);
    void appendBzr(int32 glyphIndex, const Biji &bl, const lunkuoFanwei &lfw, int lks);
    /// 显示组合字
    void getZuhezi(s32vector &xszf);
    /// 显示简单字--普通字
    void getPutongzi(s32vector &xszf);
    void getPointCount(int32 glyphIndex, uint16 &ds, uint16 &lks);
    /// 统计轮廓的数量 点的数量 补充的点数量-->tj
    /// 如果是组合字，返回组合字的部件列表
    PBjCanshuList tongjiLunkuoDian(int32 glyphIndex, QString &tj);
    /// 修改字符 zuheziGlyphIdx 的第bujianIdx个部件的 glyphIndex=bujianGlyphIdx
    void xiugaiBujianGlyphIndex(int32 bujianGlyphIdx, int16 bujianIdx, int32 zuheziGlyphIdx);
    /// 编辑修改组合字的部件参数
    int fzgbXY(int32 zuheziGlyphIdx, BujianGaibian bg, int bujianIdx, int xy=0, double scale=0.0);
    QList<glyfLink> gl;
    QList<fuzhuxian> fzxList;//!<辅助线列表
    HMTXTable*hmtx;
    void setHmtx(HMTXTable *vhmtx);
    //!造字结束后，用新的笔迹替换原有的笔迹
    void tihuanZifu(int32 glyphIndex, const Biji &biji, int lks, int16 newZx, int16 newHeightMax);
    template<class T, typename topingmu>
    void getZifuPaintPath(int32 glyphIndex, QPainterPath &zfpp, T* pLei, topingmu toPingmuX, topingmu toPingmuY,
                          int wzx=0,int wzy=0,int pp2=0,
                          PLPP dPPList=nullptr, PLPP lkhPPList=nullptr,PLPP lkList=nullptr){
        zfpp.clear();
        if(lkList){
            lkList->clear();
        }
        if(lkhPPList){
            lkhPPList->clear();
        }
        if(dPPList){
            dPPList->clear();
        }
        if(!gl[glyphIndex].pg){
            return;
        }
        PBiji zfbl=&(gl[glyphIndex].pg->getBzrList(this));
        Biji xbj=*zfbl;
        int dianshu=xbj.length();
        //为连续的控制点中增加在曲线上的非控制点
        int lkBe=0;//每个轮廓的第一个点
        while(lkBe<dianshu){//每个轮廓都需要检查一次
            int di=lkBe;
            while(!xbj[di].lkEnd){
                if(xbj[di].isControl && xbj[di+1].isControl){//插点
                    bzr2 bz;
                    bz.x=(xbj[di].x+xbj[di+1].x)/2;
                    bz.y=(xbj[di].y+xbj[di+1].y)/2;
                    bz.isControl=false;
                    bz.wojia=true;
                    xbj.insert(di+1,bz);
                    di++;
                    dianshu++;
                }
                di++;
            }
            int lkend=di;//记录每个轮廓的终点
            //首尾如果都是控制点，则把非控制点插入本轮廓的开头
            if(xbj[lkBe].isControl && xbj[lkend].isControl){
                bzr2 bz;
                bz.x=(xbj[lkBe].x+xbj[lkend].x)/2;
                bz.y=(xbj[lkBe].y+xbj[lkend].y)/2;
                bz.isControl=false;
                bz.wojia=true;
                xbj.insert(lkBe,bz);
                lkend+=1;
                dianshu++;
            }
            lkBe=lkend+1;//准备下一个轮廓
        }

        QPainterPath dpp;  //!<临时存储一个轮廓的全部点
        QPainterPath lkhpp;//!<临时存储一个轮廓序号
        QPainterPath lkpp; //!<临时存储一个轮廓
        int lkFirst=0;
        bool saveControl=false;
        const double cBanKuan=3;
        double cGao=1.732*cBanKuan;
        int lkId=0;
        for(int xi=0;xi<xbj.length();xi++){
            double x=(pLei->*toPingmuX)(xbj[xi].x,wzx,pp2),y=(pLei->*toPingmuY)(xbj[xi].y,wzy,pp2);
            //snd<<xi<<",x="<<xbj[xi].x<<",y="<<xbj[xi].y<<",end="<<(xbj[xi].lkEnd?"true":"false")
            //  <<",ctrl="<<(xbj[xi].isControl?"true":"false")<<",wojia="<<(xbj[xi].wojia?"true":"false");
            if(xi==0||xbj[xi-1].lkEnd){
                lkFirst=xi;
                if(lkhPPList){
                    //标记轮廓的第一点
                    lkhpp.addText(x+3,y,QFont("微软雅黑",12),QString::number(++lkId));
                    lkhPPList->append(lkhpp);
                    lkhpp.clear();
                }
                if(xbj[xi].isControl){//第一个点是控制点，要从第二个点开始画。已经补点，不存在连续的控制点。
                    saveControl=true;     //保存起来等最后再画
                    double x1=(pLei->*toPingmuX)(xbj[xi+1].x,wzx,pp2),y1=(pLei->*toPingmuY)(xbj[xi+1].y,wzy,pp2);
                    zfpp.moveTo(x1,y1);//保存第一点，移动到第二个点
                    if(lkList){//复制zfpp
                        lkpp.moveTo(x1,y1);
                    }
                    if(dPPList){
                        //标记第一点---控制点
                        dpp.moveTo(x,y);dpp.lineTo(x-cBanKuan,y-cGao);dpp.lineTo(x+cBanKuan,y-cGao);dpp.closeSubpath();
                        //标记第二个点--在线上的点
                        if(xbj[xi+1].wojia){//在曲线上的点
                            dpp.moveTo(x1,y1);dpp.lineTo(x1+cGao,y1-cBanKuan);
                            dpp.moveTo(x1,y1);dpp.lineTo(x1+cGao,y1+cBanKuan);
                        }else{
                            dpp.addEllipse(QRectF(x1-3,y1-3,6,6));
                        }
                    }
                    xi++;
                }else{
                    saveControl=false;
                    zfpp.moveTo(x,y);
                    if(lkList){//复制zfpp
                        lkpp.moveTo(x,y);
                    }
                    if(dPPList){
                        if(xbj[xi].wojia){
                            dpp.moveTo(x,y);dpp.lineTo(x+cGao,y-cBanKuan);
                            dpp.moveTo(x,y);dpp.lineTo(x+cGao,y+cBanKuan);
                        }else{
                            dpp.addEllipse(QRectF(x-3,y-3,6,6));
                        }
                    }
                }
            }else{
                if(xbj[xi].isControl){
                    if(xbj[xi].lkEnd){//最后一点是控制点，要画曲线回到本轮廓的起点
                        if(dPPList){
                            dpp.moveTo(x,y);dpp.lineTo(x-cBanKuan,y-cGao);dpp.lineTo(x+cBanKuan,y-cGao);dpp.closeSubpath();
                            dPPList->append(dpp);
                            dpp.clear();
                        }
                        zfpp.quadTo(x,y,
                                    (pLei->*toPingmuX)(xbj[lkFirst].x,wzx,pp2),
                                    (pLei->*toPingmuY)(xbj[lkFirst].y,wzy,pp2));
                        if(lkList){//复制zfpp
                            lkpp.quadTo(x,y,
                                        (pLei->*toPingmuX)(xbj[lkFirst].x,wzx,pp2),
                                        (pLei->*toPingmuY)(xbj[lkFirst].y,wzy,pp2));
                            lkList->append(lkpp);
                            lkpp.clear();
                        }
                    }else{
                        //遇到控制点不是最后一点，正常画二阶曲线
                        double x1=(pLei->*toPingmuX)(xbj[xi+1].x,wzx,pp2),y1=(pLei->*toPingmuY)(xbj[xi+1].y,wzy,pp2);
                        zfpp.quadTo(x,y,x1,y1);
                        if(lkList){//复制zfpp
                            lkpp.quadTo(x,y,x1,y1);
                        }
                        if(dPPList){
                            //标记控制点
                            dpp.moveTo(x,y);dpp.lineTo(x-cBanKuan,y-cGao);dpp.lineTo(x+cBanKuan,y-cGao);dpp.closeSubpath();
                            //标记曲线终点
                            if(xbj[xi+1].wojia){
                                dpp.moveTo(x1,y1);dpp.lineTo(x1+cGao,y1-cBanKuan);
                                dpp.moveTo(x1,y1);dpp.lineTo(x1+cGao,y1+cBanKuan);
                            }else{
                                dpp.addEllipse(QRectF(x1-3,y1-3,6,6));
                            }
                        }
                        xi++;
                        //snd<<xi<<",x="<<xbj[xi].x<<",y="<<xbj[xi].y<<",end="<<(xbj[xi].lkEnd?"true":"false")
                        //  <<",ctrl="<<(xbj[xi].isControl?"true":"false")<<",wojia="<<(xbj[xi].wojia?"true":"false");
                        if(xbj[xi].lkEnd){
                            //画了曲线后，如果曲线终点是最后一个点
                            if(dPPList){
                                dPPList->append(dpp);
                                dpp.clear();
                            }
                            if(saveControl){
                                //如果保存了曲线，则画曲线
                                zfpp.quadTo((pLei->*toPingmuX)(xbj[lkFirst].x,wzx,pp2),
                                            (pLei->*toPingmuY)(xbj[lkFirst].y,wzy,pp2),
                                            (pLei->*toPingmuX)(xbj[lkFirst+1].x,wzx,pp2),
                                        (pLei->*toPingmuY)(xbj[lkFirst+1].y,wzy,pp2));
                                if(lkList){//复制zfpp
                                    lkpp.quadTo((pLei->*toPingmuX)(xbj[lkFirst].x,wzx,pp2),
                                                (pLei->*toPingmuY)(xbj[lkFirst].y,wzy,pp2),
                                                (pLei->*toPingmuX)(xbj[lkFirst+1].x,wzx,pp2),
                                            (pLei->*toPingmuY)(xbj[lkFirst+1].y,wzy,pp2));
                                    lkList->append(lkpp);
                                    lkpp.clear();
                                }
                            }else{
                                //如果没有保存的曲线，则画直线封闭。
                                zfpp.closeSubpath();
                                if(lkList){//复制zfpp
                                    lkpp.closeSubpath();
                                    lkList->append(lkpp);
                                    lkpp.clear();
                                }
                            }
                        }
                    }
                }else{
                    //遇到的不是控制点，则画直线
                    zfpp.lineTo(x,y);
                    if(lkList){//复制zfpp
                        lkpp.lineTo(x,y);
                    }
                    if(dPPList){
                        if(xbj[xi].wojia){
                            dpp.moveTo(x,y);dpp.lineTo(x+cGao,y-cBanKuan);
                            dpp.moveTo(x,y);dpp.lineTo(x+cGao,y+cBanKuan);
                        }else{
                            dpp.addEllipse(QRectF(x-3,y-3,6,6));
                        }
                    }
                    if(xbj[xi].lkEnd){
                        //画了直线后如果直线末端点是最后一个点
                        if(dPPList){
                            dPPList->append(dpp);
                            dpp.clear();
                        }
                        if(saveControl){
                            //如果保存了曲线，则画曲线封闭
                            zfpp.quadTo((pLei->*toPingmuX)(xbj[lkFirst].x,wzx,pp2),
                                        (pLei->*toPingmuY)(xbj[lkFirst].y,wzy,pp2),
                                        (pLei->*toPingmuX)(xbj[lkFirst+1].x,wzx,pp2),
                                    (pLei->*toPingmuY)(xbj[lkFirst+1].y,wzy,pp2));
                            if(lkList){//复制zfpp
                                lkpp.quadTo((pLei->*toPingmuX)(xbj[lkFirst].x,wzx,pp2),
                                            (pLei->*toPingmuY)(xbj[lkFirst].y,wzy,pp2),
                                            (pLei->*toPingmuX)(xbj[lkFirst+1].x,wzx,pp2),
                                        (pLei->*toPingmuY)(xbj[lkFirst+1].y,wzy,pp2));
                                lkList->append(lkpp);
                                lkpp.clear();
                            }
                        }else{
                            //如果没有保存的曲线，则画直线封闭。
                            zfpp.closeSubpath();
                            if(lkList){//复制zfpp
                                lkpp.closeSubpath();
                                lkList->append(lkpp);
                                lkpp.clear();
                            }
                        }
                    }
                }
            }
        }
    }
    void suofang(int32 glyphIndex, bool fangda);
    /// 删除全部框选的轮廓,更新包围矩形和轮廓数。
    void shanchuLunkuo(int32 glyphIndex, lunkuoFanwei xuanzeBianjie);
    ///改为组合字，删除原有笔迹。自动做两个部件，并指定部件1gid,和两个部件的偏移 缩放
    void gaiweiZuhezi(int32 glyphIndex, int16 bj1Gid=0,
                      int16 bj1xOff=0, int16 bj1yOff=0, double bj1xScale=1.0, double bj1yScale=1.0,
                      int16 bj2xOff=0, int16 bj2yOff=0, double bj2xScale=1.0, double bj2yScale=1.0);
    ///返回是不是简单字，true=简单字。false=不是简单字，可能是没笔迹或者是组合字。
    bool isSimple(int32 glyphIndex);
    /// 删除一个选中的轮廓，更新包围矩形和轮廓数。
    void shanchuLunkuo(int32 glyphIndex, int lksel);
    /// 删除全部框选的点，更新包围矩形和轮廓数。
    void shanchuDian(int32 glyphIndex, lunkuoFanwei xuanzeBianjie);
    /// 组合字末尾补充一个部件
    void jiayizu(int32 zuheziGlyphIdx);
    /// 删除组合字的一个部件。剩余一个时阻止删除false,其他返回true
    bool shanyizu(int32 zuheziGlyphIdx, int bujianIdx);
    /// 查引用。检查当前选择的字符被哪些组合字引用。
    void chaYinyong(int32 wmgid, s32vector &jg);
};

#endif // TABLEGLYF_H
