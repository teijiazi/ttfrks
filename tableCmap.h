#ifndef TABLECMAP_H
#define TABLECMAP_H
#include "types.h"

class CMAPTable;

enum xinjianMuban{
    xmKongge,//只有空格
    xm3500,//3500常用字和空格
    xm6500,//6500常用字和空格
};

typedef uint8 zidongZifu;
//保存的时候检测，如果缺少必须的字符则自动补充
const zidongZifu zzOk=0;//不需要补充
const zidongZifu zzNotdef=0x01;//第一个空字符，无图，编码0.
const zidongZifu zzKongge=0x02;//空格
const zidongZifu zzDiukey=0x04;//zifuOrderByGlyphId中没有找到共享字符的key


class suoyinValue{
public:
    suoyinValue(int32 vidx,uint32 vbianma):idx(vidx),bianma(vbianma){}
    int32 idx;    //!<glyphIndex
    uint32 bianma;//!<编码
};

class formatCmap{
public:
    explicit formatCmap(uint16 vfmt);
    virtual ~formatCmap();
    uint16	format;
    virtual void show(CMAPTable *pcmap)=0;
    virtual void read(QDataStream&dt)=0;
    virtual void save(QDataStream&dt)=0;
    virtual void jisuan(CMAPTable *pcmap)=0;
    virtual void soujizifu(CMAPTable *pcmap)=0;
    virtual uint32 getLength()=0;
};

class format0:public formatCmap{
public:
    format0();
    //uint16	format;//	Format number is set to 0.
    uint16	length;//	This is the length in bytes of the subtable.
    uint16	language;//	For requirements on use of the language field, see “Use of the language field in 'cmap' subtables” in this document.
    uint8	glyphIdArray[256];//	An array that maps character codes to glyph index values.
    void read(QDataStream&dt)override;
    void save(QDataStream&dt)override;
    void show(CMAPTable *pcmap)override;
    void jisuan(CMAPTable *pcmap)override;
    void soujizifu(CMAPTable *pcmap)override;
    uint32 getLength()override{return length;}
};

class format4:public formatCmap{
public:
    format4();
    void arr();
    void read(QDataStream&dt)override;
    void save(QDataStream&dt)override;
    //uint16	format;//	Format number is set to 4.
    uint16	length;//	This is the length in bytes of the subtable.
    uint16	language;//	For requirements on use of the language field, see “Use of the language field in 'cmap' subtables” in this document.
    uint16	segCountX2;//	2 × segCount.
    uint16	searchRange;//	Maximum power of 2 less than or equal to segCount, times 2 ((2**floor(log2(segCount))) * 2, where “**” is an exponentiation operator)
    uint16	entrySelector;//	Log2 of the maximum power of 2 less than or equal to numTables (log2(searchRange/2), which is equal to floor(log2(numTables)))
    uint16	rangeShift;//	segCount times 2, minus searchRange ((segCount * 2) - searchRange)
    //uint16	endCode[segCount];//	End characterCode for each segment, last=0xFFFF.
    u16vector vendCode;
    uint16	reservedPad;//	Set to 0.
    //uint16	startCode[segCount]	Start character code for each segment.
    //int16	idDelta[segCount]	Delta for all character codes in segment.
    //uint16	idRangeOffsets[segCount]	Offsets into glyphIdArray or 0
    //uint16	glyphIdArray[ ]	Glyph index array (arbitrary length)
    u16vector vstartCode;
    s16vector vidDelta;
    u16vector vidRangeOffsets;
    u16vector vglyphIdArray;
    uint16 fzSegCount;
    uint32 fzNumGlyphId;
    void jisuan(CMAPTable *pcmap)override;
    void show(CMAPTable *pcmap)override;
    void soujizifu(CMAPTable *pcmap)override;
    uint32 getLength()override{return length;}
private:
    void setZifuList(CMAPTable *pcmap, uint16 zifuBianma, uint16 glyphIdx, int from);
};
class SequentialMapGroup{
public:
    uint32	startCharCode;//!<	First character code in this group
    uint32	endCharCode;  //!<	Last character code in this group
    uint32	startGlyphID; //!<	Glyph index corresponding to the starting character code
};
class format12:public formatCmap{
public:
    format12();
    ~format12();
    void read(QDataStream&dt)override;
    void save(QDataStream&dt)override;
    //uint16	format;//	Subtable format; set to 12.
    uint16	reserved;//	Reserved; set to 0
    uint32	length;//	Byte length of this subtable (including the header)
    uint32	language;//	For requirements on use of the language field, see “Use of the language field in 'cmap' subtables” in this document.
    uint32	numGroups;//	Number of groupings which follow
    //SequentialMapGroup	groups[numGroups]	Array of SequentialMapGroup records.
    QVector<SequentialMapGroup> groups;
    void show(CMAPTable *pcmap)override;
    void jisuan(CMAPTable *pcmap)override;
    void soujizifu(CMAPTable *pcmap)override;
    uint32 getLength()override{return length;}
};

class EncodingRecord{
public:
    explicit EncodingRecord(uint16 platid=0, uint16 encodid=0, uint16 fmtId=0);
    ~EncodingRecord();
    uint16	platformID;//	Platform ID.
    uint16	encodingID;//	Platform-specific encoding ID.
    Offset32	subtableOffset;//	Byte offset from beginning of table to the subtable for this encoding.
    formatCmap* fzFormatPoint;//指向不同格式的指针。
    int fzFmtid;
    void read(QDataStream&dt, uint16 fmtId, TableTTF *ptb);
    void save(QDataStream&dt);
    void show(CMAPTable *pcmap);
};

class CMAPTable:public TableTTF{//必须
    Q_OBJECT
    void getUnicode(uint32 &u1, uint32 &u2, uint32 &u3, uint32 &u4);
public:
    CMAPTable(QObject *parent=nullptr);
    void read(QDataStream& dt, uint32 offset,uint16 tuxingZongshu);
    void save(QDataStream& dt);
    void show();
    void xinjian(xinjianMuban xm);
    uint16	version;//	Table version number (0).
    uint16	numTables;//	Number of encoding tables that follow.
    //EncodingRecord	encodingRecords[numTables]
    //
    QVector<EncodingRecord> pEncodingRecord;
    //
    QString info;
    u32vector zifuOrderByGlyphId;   //!<按glyphID顺序存储字符编码。无编码的字符存为0.
    QMap<uint32,u32vector>shareZifu;//!<key=主编码，value=其他编码
    QVector<suoyinValue> suoyinPaixu;//带着索引排序
    zidongZifu jisuan(uint32 &u1, uint32 &u2, uint32 &u3, uint32 &u4,
                      uint16 &cKaishi, uint16 &cJieshu, s32vector &azidx);
    void deleteWenzi(const s32vector &glyphIdxSel);
    ///补充字符，如果ipt是编码则补充编码，否则按照文字转换编码
    ///返回0----------没有合适的文字或编码
    ///返回>0，可以补充到glyf、hmtx的图形数量
    ///返回＜0，输入编码无效
    int appendBianma(QString &ipt);
    uint32 sypxKaishiIdx;
    static void getUnicodeMiaoshu(u32vector bm, QString &uniFenlei);
    QString showAll(QString fenleiName, s32vector &xianshiZifu);
    ///对应编码改为0，保留图形
    void setBianma0(const s32vector &glyphIdxSel);
signals:
    void zifuReady(CMAPTable*pcmap);
};

#endif // TABLECMAP_H
