#ifndef TTFFILE_H
#define TTFFILE_H
#include <ttfhead.h>
#include <tables1.h>
#include <tableCmap.h>
#include <table2.h>
#include <tableName.h>
#include <tablePost.h>
#include <tableGasp.h>
#include <tableGlyfLoca.h>
#include <tableDSIG.h>
#include <tableGDEF.h>
#include <tableGPOS.h>
#include <tablegsub.h>
#include <cfftable.h>
#include <tablevhea.h>

class TtfFile: public QObject
{
    Q_OBJECT
signals:
    void appendTable(QString tbname);
    void appendTxt(QString xxx,bool isError=false);
    void showFontinfo(const QList<NameRecord> & str);
    void setFontDate(LONGDATETIME create, uint16 unitsPerEm);
public:
    explicit TtfFile(QObject *parent=nullptr);
    void jisuan(const QString &ztmz, const QString &banben, const QStringList &sm,
                const QStringList &smY,
                qint64 riqi1970, uint16 emsize, int xiahuaxianGlyphIndex);
    TTFLeibie readFrom(QDataStream &dt);
    void saveTo(QDataStream &dt);
    void showTable(QString tbnameMix);
    void CalcTableChecksum(QDataStream &dt, int64 &pos, int tbidx);
    bool chabiao(QDataStream &dt, uint32 offset, QString tbname, uint32 tblLen);
    void deleteWenzi();
    void zidongduiqi();
    int zuoyouyidong(int32 index, int newLsb);
    void moveRsb(int32 glyphIndex, int newAw);
    int daxieduiqi(int32 glyphIdx);
    ///字库增加编码，返回0----------没有合适的文字或编码
    ///返回>0，可以补充到glyf hmtx的图形数量
    ///返回＜0，输入编码无效
    int appendBianma(QString &ipt);
    void deleteTuxing(s32vector &glyphIdxSel);
    void xinjian1(xinjianMuban xm);
    void xinjian2(uint16 emsize);
    OffsetTable ot;
    QList<TableRecord> pTableRecord;
    HEADTable head;
    HHEATable hhea;
    MAXPTable maxp;
    OS2Table  os2;
    HMTXTable hmtx;
    CMAPTable cmap;
    FPGMTable fpgm;
    PREPTable prep;
    CVTTable  cvt;
    LOCATable loca;
    GLYFTable glyf;
    NAMETable name;
    POSTTable post;
    GASPTable gasp;
    DSIGTable dsig;
    GDEFTable gdef;
    GPOSTable gpos;
    GSUBTable gsub;
    METATable meta;
    VHEATable vhea;
    VMTXTable vmtx;
    CFFTable  cff;
    uint16 tuxingZongshu;//!<图形总数，包括.notdef等不显示图形
    s32vector toDelete;  //!<点击删除按钮，收集准备删除的字符gid
    void appendZixing(uint16 jigeweizhi);
    /// 只删除编码，不删除图形
    void shanchuBianma(const s32vector &glyphIdxSel);
    /// 查引用。检查当前选择的字符被哪些组合字引用。
    void chaYinyong(int32 wmgid, QString &jg);
    ///分别显示无编码的组合字 普通字
    void getWubianma(s32vector &xianshiZifu, bool xianshiZuhe);
private:
    void checksumAdjustment(QDataStream &dt, int64 headTableOffset);
signals:
    void jindu(int x1to100);
};

#endif // TTFFILE_H
