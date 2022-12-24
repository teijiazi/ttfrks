#ifndef TABLENAME_H
#define TABLENAME_H
#include "types.h"

enum nameidXuhao{
    nxBanquan,//!<版权
    nxMingcheng,//!<字体族名称
    nxZizu,//!<字体子族名称
    nxWeiyiId,//!<唯一字体id
    nxQuanming,//!<字体全名
    nxBanben,//!<版本//固定格式：“Version <number>.<number>”
    nxPostScript,//!<PostScript// 不超过63字符，仅限[33 to 126],不能用这10个： '[', ']', '(', ')', '{', '}', '<', '>', '/', '%'.
    nxShangbiao,//!<商标
    nxZhizao,//!<制造者
    nxSheji,//!<设计者
    nxMiaoshu,//!<描述
    nxZhizaoUrl,//!<字体供应商的网址
    nxShejiUrl,//!<设计者网址
    nxXukeMiaoshu,//!<许可证描述
    nxXukeUrl,//!<许可证网址
    nxYuliu,//!<预留//.
    nx16,//!<印刷字体族名称
    nx17,//!<印刷字体子族名称
    nx18,//!<完全兼容
    nx19,//!<样本文本
    nx20,//!<PostScript CID findfont name Its presence in a font means that the nameID 6 holds a PostScript font name that is meant to be used with the “composefont” invocation in order to invoke the font in a PostScript interpreter. See the definition of name ID 6.
    nx21,//!<WWS字体族名称. Used to provide a WWS-conformant family name in case the entries for IDs 16 and 17 do not conform to the WWS model. (That is, in case the entry for ID 17 includes qualifiers for some attribute other than weight, width or slope.) If bit 8 of the fsSelection field is set, a WWS Family Name entry should not be needed and should not be included. Conversely, if an entry for this ID is included, bit 8 should not be set. (See OS/2.fsSelection field for details.) Examples of name ID 21: “Minion Pro Caption” and “Minion Pro Display”. (Name ID 16 would be “Minion Pro” for these examples.)
    nx22,//!<WWS字体子族名称. Used in conjunction with ID 21, this ID provides a WWS-conformant subfamily name (reflecting only weight, width and slope attributes) in case the entries for IDs 16 and 17 do not conform to the WWS model. As in the case of ID 21, use of this ID should correlate inversely with bit 8 of the fsSelection field being set. Examples of name ID 22: “Semibold Italic”, “Bold Condensed”. (Name ID 17 could be “Semibold Italic Caption”, or “Bold Condensed Display”, for example.)
    nx23,//!<浅色背景调色板. This ID, if used in the CPAL table’s Palette Labels Array, specifies that the corresponding color palette in the CPAL table is appropriate to use with the font when displaying it on a light background such as white. Strings for this ID are for use as user interface strings associated with this palette.
    nx24,//!<深色背景调色板 This ID, if used in the CPAL table’s Palette Labels Array, specifies that the corresponding color palette in the CPAL table is appropriate to use with the font when displaying it on a dark background such as black. Strings for this ID are for use as user interface strings associated with this palette
    nx25,//!<变体PostScript名称前缀"
    YudingyiNameCount//!<26 预定义name id数量。
};

enum platId{
    piUnicode,
    piMacintosh,
    piBukeyong,
    piWindows
};

enum yuyanBianma{
    ybYing=0x0409,//!<英文 美国
    ybZhong=0x0804,//!<中文 中国
};

class NAMETable;
class NameRecord{
public:
    static const QString yudingyiStr[YudingyiNameCount];
    NameRecord(uint16	vplatformID=piWindows,uint16	vencodingID=1,uint16	vlanguageID=0,
               uint16	vnameID=0, uint16	vlength=0,Offset16	vstringOffset=0);
    uint16	platformID;//	Platform ID.
    uint16	encodingID;//	Platform-specific encoding ID.
    uint16	languageID;//	Language ID.
    uint16	nameID;//	Name ID.
    uint16	length;//	String length (in bytes).
    Offset16	stringOffset;//	String offset from start of storage area (in bytes).
    QString recStr;
    void readFromDt(QDataStream&dt);
    void save(QDataStream&dt);
    void show(int index, TableTTF *tbl);
    void set(uint16	vplatformID=piWindows,uint16	vencodingID=1,uint16	vlanguageID=0,
             uint16	vnameID=0, uint16	vlength=0,Offset16	vstringOffset=0);
};
class version0{
public:
    version0();
    ~version0();
    void readFromDt(QDataStream&dt, uint32 nameLen, NAMETable *nt);
    void save(QDataStream&dt);
    void show(TableTTF *tbl);
    uint16	version;//	Table version number (=0).
    uint16	count;//	Number of name records.
    Offset16	storageOffset;//	Offset to start of string storage (from start of table).
    //NameRecord	nameRecord[count]	The name records where count is the number of records.
    QList<NameRecord> nameRecord;
    //(Variable)		Storage for the actual string data.
    char * pData;
    uint32 fzDatalen;
    void jisuan(const QString &ztmz, const QString &banben, const QStringList &sm,
                const QStringList &smY);
    //true:platid=3 nameid=9 lan=0804 str=qmgj
    bool win09NotQmgj();
private:
    void chaijieString(uint16 platid, char *p, int len, QString &jg);
};
class LangTagRecord{
public:
    uint16	length;//	Language-tag string length (in bytes)
    Offset16	langTagOffset;//	Language-tag string offset from start of storage area (in bytes).
};
class version1{
public:
    version1();
    ~version1();
    void arrNr();
    void arrLTR();
    uint16	version;//	Table version number (=1).
    uint16	count;//	Number of name records.
    Offset16	storageOffset;//	Offset to start of string storage (from start of table).
    //NameRecord	nameRecord[count]	The name records where count is the number of records.
    NameRecord *nameRecord;
    uint16	langTagCount;//	Number of language-tag records.
    //LangTagRecord	langTagRecord[langTagCount]	The language-tag records where langTagCount is the number of records.
    LangTagRecord* langTagRecord;
    //(Variable)		Storage for the actual string data.
    char * pData;
};

class NAMETable: public TableTTF{//必须
    Q_OBJECT
public:
    NAMETable(QObject *parent=nullptr);
    ~NAMETable();
    void read(QDataStream& dt, uint32 offset,uint32 nameLen);
    void save(QDataStream& dt);
    void show();
    void jisuan(const QString &ztmz, const QString &banben, const QStringList &sm,
                const QStringList &smY);
    version0 *nt0;
    version1 *nt1;
    /// 检测字体是不是本程序产生的，返回true---不是，不支持修改竖排版功能。
    bool bukexiugai();
signals:
    ///提供name所有数据用来显示，List以nameid为索引。0~25。
    void showFontinfo(const QList<NameRecord> & str);
};

#endif // TABLENAME_H
