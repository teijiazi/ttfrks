#ifndef TABLECOMM_H
#define TABLECOMM_H
#include "types.h"
#include "gsubsubtable.h"
#include "subtablecomm.h"
#include "gpossubtable.h"

///----------------------------------------
///       ScriptListTable
///              |-----scriptCount
///              |-----ScriptRecord
///              |         |-----scriptTag
///              |         |-----scriptOffset
///              |-----ScriptRecord
///              |-----ScriptRecord
///              |-----ScriptRecord
///              |-----ScriptTable[]
///              |       |----defaultLangSysOffset
///              |       |----langSysCount
///              |       |----LangSysRecord[]
///              |       |         |-----langSysTag
///              |       |         |-----langSysOffset
///              |       |----LangSysTable[]
///              |       |       |----lookupOrderOffset
///              |       |       |----requiredFeatureIndex
///              |       |       |----featureIndexCount
///              |       |       |----featureIndices[uint16]
///
///       FeatureListTable
///              |----featureCount
///              |----FeatureRecord
///              |----FeatureRecord
///              |         |-----featureTag
///              |         |-----featureOffset
///              |----FeatureRecord
///              |----FeatureRecord
///              |----FeatureRecord
///              |----FeatureTable[]
///              |       |----featureParamsOffset
///              |       |----lookupIndexCount
///              |       |----lookupListIndices[uint16]
///
///       LookupListTable
///              |----lookupCount
///              |----lookupOffsets
///              |----LookupTable[]
///              |        |----lookupType
///              |        |----lookupFlag
///              |        |----subTableCount
///              |        |----subtableOffsets[Offset16]
///              |        |----markFilteringSet---USE_MARK_FILTERING_SET
///
///

class LangSysTable{
public:
    LangSysTable();
    ~LangSysTable();
    Offset16	lookupOrderOffset;//!<	= NULL (reserved for an offset to a reordering table)
    uint16	requiredFeatureIndex;//!<	Index of a feature required for this language system; if no required features = 0xFFFF
    uint16	featureIndexCount;//!<	Number of feature index values for this language system — excludes the required feature
    ///uint16	featureIndices[featureIndexCount]	Array of indices into the FeatureList, in arbitrary order
    u16vector featureIndices;
    void read(QDataStream&dt);
    void save(QDataStream&dt);
    void show(QString &info);
    int byteLength();
    void addFeatureIndex(int featureIndexArr);
    void gengxinFeatrueIndex(int featureIndexBegin);
    void delFeatureIndex(const u16vector &featureIndexDeleted);
};

class LangSysRecord{
public:
    Tag	langSysTag;//!<	4-byte LangSysTag identifier
    Offset16	langSysOffset;//!<	Offset to LangSys table, from beginning of Script table
    void read(QDataStream&dt);
    void save(QDataStream&dt);
    void show(QString &info);
    static int byteLength();
};
LIST(LangSysRecord);
LIST(LangSysTable);
class ScriptTable{
public:
    ScriptTable();
    ~ScriptTable();
    Offset16	defaultLangSysOffset;//!<	Offset to default LangSys table, from beginning of Script table — may be NULL
    uint16	langSysCount;//!<	Number of LangSysRecords for this script — excluding the default LangSys
    ///LangSysRecord	langSysRecords[langSysCount]	Array of LangSysRecords, listed alphabetically by LangSys tag
    ListLangSysRecord lstLangSysRecord;
    ListLangSysTable  lstLangSysTable;
    LangSysTable defLangSysTable;
    bool fzDefChongfu;//!<默认语言表是否共享语言表.true=共享
    void read(QDataStream&dt);
    void save(QDataStream&dt);
    void show(QString &info);
    int byteLength();
    void addDefYuyan(int featureIndexArr);
};
class ScriptRecord{
public:
    Tag	scriptTag;//!<	4-byte script tag identifier
    Offset16	scriptOffset;//!<	Offset to Script table, from beginning of ScriptList
    void read(QDataStream&dt);
    void save(QDataStream&dt);
    void show(QString &info);
    static int byteLength();
};
LIST(ScriptRecord);
LIST(ScriptTable);

class ScriptListTable{
public:
    ScriptListTable();
    ~ScriptListTable();
    void read(QDataStream& dt, TableTTF *ptb);
    void save(QDataStream& dt);
    uint16	scriptCount;//!<	Number of ScriptRecords
    ///ScriptRecord	scriptRecords[scriptCount]	Array of ScriptRecords, listed alphabetically by script tag
    ListPScriptRecord lstScriptRecord;
    ListPScriptTable lstScriptTable;
    void xinjian1(QString tag);
    void show(QString &info, TableTTF *ptb);
    ///列出现有脚本，存入jbLst-----ScriptRecord.scriptTag
    void getJiaoben(QStringList &jbLst);
    void getYuyan(QStringList &yyLst, int i, bool &youDef);
    ///根据脚本和语言，返回功能的索引列表
    void getFeatureArr(u16vector &featureIndices, int i, int yuyanIndex);
    int addJiaobenYuyanDef(const QString &jiaoben, int featureIndex);
    bool jiaobenYouxiao(const QString &jiaoben);
    int byteLength();
    void gengxinFeatrueIndex(int featureIndexBegin);
    void delFeatureIndex(const u16vector &featureIndexDeleted);
    void insertFeatureIndex(int jiaobenIndex, int yuyanIndex, int featureIndex);
private:
    void xiufuPianyi();
};

//--------------------------------------------------------------------
class FeatureTable{
public:
    FeatureTable();
    ~FeatureTable();
    Offset16	featureParamsOffset;//!<	Offset from start of Feature table to FeatureParams table,if defined for the feature and present, else NULL
    uint16	lookupIndexCount;//!<	Number of LookupList indices for this feature
    ///uint16	lookupListIndices[lookupIndexCount]	Array of indices into the LookupList — zero-based (first lookup is LookupListIndex = 0)
    u16vector lookupListIndices;
    void read(QDataStream& dt);
    void save(QDataStream& dt);
    void show(QString &info);
    int byteLength();
    void delLookupIndex(int lookupIndex);
};
class FeatureRecord{
public:
    Tag	featureTag;//!<	4-byte feature identification tag
    Offset16	featureOffset;//!<	Offset to Feature table, from beginning of FeatureList
    void read(QDataStream&dt);
    void save(QDataStream&dt);
    void show(QString &info);
    static int byteLength();
};
LIST(FeatureTable);
LIST(FeatureRecord);
class FeatureListTable{
public:
    FeatureListTable();
    ~FeatureListTable();
    uint16	featureCount;//!<	Number of FeatureRecords in this table
    ///FeatureRecord	featureRecords[featureCount]	Array of FeatureRecords — zero-based (first feature has FeatureIndex = 0), listed alphabetically by feature tag
    ListFeatureRecord  lstFeatureRecord;
    ListFeatureTable   lstFeatureTable;
    void read(QDataStream& dt);
    void save(QDataStream&dt);
    void show(QString &info);
    int addFeature(const QString &tag, int lookupIndex);
    int byteLength();
    void delLookupIndex(int lookupIndex, u16vector &featureIndexDeleted);
private:
    void xiufuPianyi();
};
//-----------------------------------------------
//lookup
enum{USE_MARK_FILTERING_SET=0x10};
class LookupTable{
public:
    LookupTable();
    ~LookupTable();
    void read(QDataStream&dt, tbType tbt, TableTTF *ptb);
    void save(QDataStream&dt);
    uint16	lookupType;//!<	Different enumerations for GSUB and GPOS
    uint16	lookupFlag;//!<	Lookup qualifiers
    uint16	subTableCount;//!<	Number of subtables for this lookup
    ///Offset16	subtableOffsets[subTableCount]	Array of offsets to lookup subtables,
    ///from beginning of Lookup table
    u16vector subtableOffsets;
    uint16	markFilteringSet;    //!<	Index (base 0) into GDEF mark glyph sets structure.
    //!<This field is only present if the USE_MARK_FILTERING_SET lookup flag is set.
    void show(QString &info, tbType tbt);
    ListPSubtable lstPSubtable;
    int byteLength();
};
LIST(LookupTable);
class LookupListTable{
public:
    LookupListTable();
    ~LookupListTable();
    uint16	lookupCount;//!<	Number of lookups in this table
    ///Offset16	lookupOffsets[lookupCount]	Array of offsets to Lookup tables,
    ///from beginning of LookupList — zero based (first lookup is Lookup index = 0)
    u16vector lookupOffsets;
    ListPLookupTable lstPLookupTable;
    void read(QDataStream&dt, tbType tbt, TableTTF *ptb);
    void save(QDataStream&dt);
    void show(QString &info, tbType tbt, TableTTF *ptb);
    void insertLiga(int lookupTableIndex, int zibiaoIndex, const u16vector &shuru, int &jgCoverIdx, int &jgShuruIdx);
    void gengxinLiga(int lookupTableIndex, int zibiaoIndex, int coverIndex, int li, const uint16 shuchu);
    void getLigaZibiao(int16 looklistIndex, uint16 &zibiaoShuliang);
    void getLigaCover(int16 looklistIndex, int16 zibiaoIndex, u16vector &cover);
    void getLigaZifu(int16 looklistIndex, int zibiaoIndex, int coverIndex,
                     QList<u16vector> &shuruGlyList, u16vector &shuchuGlyphIdx);
    int addLiga(const u16vector &shuru,int& jgCoverIdx,int& jgShuruIdx);
    void shanchuLiga(int &lookupTableIndex, int &zibiaoIndex, int &coverIdx, int &shuruIdx);
    int addVert(const u16vector &heng, const u16vector &shu);
    void tihuanVert(int lookupIndex, const u16vector &heng, const u16vector &shu);
private:
    void xiufuPianyi();
};


#endif // TABLECOMM_H
