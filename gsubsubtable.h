#ifndef GSUBSUBTABLE_H
#define GSUBSUBTABLE_H

#include <QObject>
#include "types.h"
#include "subtablecomm.h"

static const QString gsubSubtableType[]={
    "",//对应lookuptype，1开始
    "Single",
    "Multiple",
    "Alternate",
    "Ligature",
    "Context",
    "Chaining Context",
    "Extension Substitution",
    "Reverse chaining context single"
};

class SingleSubstFormat1:public Subtable{
    Q_OBJECT
public:
    explicit SingleSubstFormat1(QObject *parent = nullptr):
        Subtable(parent),substFormat(1),pCover(nullptr){};
    ~SingleSubstFormat1();
    void read(QDataStream&dt)override;
    void show(QString&info)override;
    void save(QDataStream&dt)override;
    uint16	substFormat;//	Format identifier: format = 1
    Offset16	coverageOffset;//	Offset to Coverage table, from beginning of substitution subtable
    int16	deltaGlyphID;//	Add to original glyph ID to get substitute glyph ID
    int byteLength() override;
    CoverageFormat* pCover;
};
class SingleSubstFormat2:public Subtable{
    Q_OBJECT
public:
    explicit SingleSubstFormat2(QObject *parent = nullptr):
        Subtable(parent),substFormat(2),pCover(nullptr){};
    ~SingleSubstFormat2();
    void read(QDataStream&dt)override;
    void show(QString&info)override;
    void save(QDataStream&dt)override;
    uint16	substFormat;//	Format identifier: format = 1
    Offset16	coverageOffset;//	Offset to Coverage table, from beginning of substitution subtable
    uint16	glyphCount;//	Number of glyph IDs in the substituteGlyphIDs array
    //uint16	substituteGlyphIDs[glyphCount]	Array of substitute glyph IDs — ordered by Coverage index
    u16vector substituteGlyphIDs;
    CoverageFormat* pCover;
    int byteLength() override;
    void setVert(const u16vector &heng, const u16vector &shu);
private:
    void xiufuPianyi();
    void set(const u16vector &coverGIDs, const u16vector &tihuanGIDs);
};

//loopup type=2 Multiple output glyphs
class SequenceTable{
public:
    void read(QDataStream&dt);
    void show(QString&info);
    void save(QDataStream&dt);
    uint16	glyphCount;//	Number of glyph IDs in the substituteGlyphIDs array. This must always be greater than 0.
    //uint16	substituteGlyphIDs[glyphCount]	String of glyph IDs to substitute
    u16vector substituteGlyphIDs;
};

class MultipleSubstFormat1:public Subtable{
public:
    explicit MultipleSubstFormat1(QObject *parent = nullptr):
        Subtable(parent),substFormat(1),pCover(nullptr),pSeq(nullptr){};
    ~MultipleSubstFormat1();
    void read(QDataStream&dt)override;
    void show(QString&info)override;
    void save(QDataStream&dt)override;
    uint16	substFormat;//	Format identifier: format = 1
    Offset16	coverageOffset;//	Offset to Coverage table, from beginning of substitution subtable
    uint16	sequenceCount;//	Number of Sequence table offsets in the sequenceOffsets array
    //Offset16	sequenceOffsets[sequenceCount]	Array of offsets to Sequence tables.
    //Offsets are from beginning of substitution subtable, ordered by Coverage index
    u16vector sequenceOffsets;
    CoverageFormat *pCover;
    SequenceTable *pSeq;
    int byteLength() override;
};

//lookuptype=3
/// 参考结构：
/// substFormat
/// coverageOffset
/// alternateSetCount
/// alternateSetOffsets
/// ---AlternateSetTable
/// ---...
/// ---AlternateSetTable
/// ---CoverageFormat
class AlternateSetTable{
public:
    uint16	glyphCount;//	Number of glyph IDs in the alternateGlyphIDs array
    //uint16	alternateGlyphIDs[glyphCount]	Array of alternate glyph IDs, in arbitrary order
    u16vector alternateGlyphIDs;
};

class AlternateSubstFormat1:public Subtable{
    Q_OBJECT
public:
    explicit AlternateSubstFormat1(QObject *parent = nullptr):
        Subtable(parent),substFormat(1),pCover(nullptr),pAlt(nullptr){};
    ~AlternateSubstFormat1();
    void read(QDataStream&dt)override;
    void show(QString&info)override;
    void save(QDataStream&dt)override;
    uint16	substFormat;//	Format identifier: format = 1
    Offset16	coverageOffset;//	Offset to Coverage table, from beginning of substitution subtable
    uint16	alternateSetCount;//	Number of AlternateSet tables
    //Offset16	alternateSetOffsets[alternateSetCount]	Array of offsets to AlternateSet tables.
    //          Offsets are from beginning of substitution subtable, ordered by Coverage index
    u16vector alternateSetOffsets;
    CoverageFormat* pCover;
    AlternateSetTable* pAlt;
    int byteLength() override;
};

//lookuptype=4
class LigatureTable{
public:
    uint16	ligatureGlyph;//	glyph ID of ligature to substitute
    uint16	componentCount;//	Number of components in the ligature
    //uint16	componentGlyphIDs[componentCount - 1]	Array of component glyph IDs — start with the second component, ordered in writing direction
    u16vector componentGlyphIDs;
    int byteLength();
    void save(QDataStream&dt);
};
LIST(LigatureTable);

class LigatureSetTable{
public:
    LigatureSetTable():ligatureCount(0){}
    ~LigatureSetTable(){

    }
    void read(QDataStream&dt);
    void show(QString&info);
    void save(QDataStream&dt);
    uint16	ligatureCount;//	Number of Ligature tables
    //Offset16	ligatureOffsets[LigatureCount]	Array of offsets to Ligature tables.
    //          Offsets are from beginning of LigatureSet table, ordered by preference.
    u16vector ligatureOffsets;
    ListLigatureTable lstLigatureTable;
    int set(const u16vector &shuru);
    int byteLength();
    void shanchu(int &shuruIdx);
    void xiufuPianyi();
private:
    bool kaoqian(const LigatureTable &a, const LigatureTable &b);
};
LIST(LigatureSetTable);
class LigatureSubstFormat1:public Subtable{
    Q_OBJECT
public:
    explicit LigatureSubstFormat1(QObject *parent = nullptr);
    ~LigatureSubstFormat1();
    void read(QDataStream&dt)override;
    void show(QString&info)override;
    void save(QDataStream&dt)override;
    uint16	substFormat;//	Format identifier: format = 1
    Offset16	coverageOffset;//	Offset to Coverage table,
    //from beginning of substitution subtable
    uint16	ligatureSetCount;//	Number of LigatureSet tables
    //Offset16	ligatureSetOffsets[ligatureSetCount]	Array of offsets to LigatureSet tables.
    //Offsets are from beginning of substitution subtable, ordered by Coverage index
    u16vector ligatureSetOffsets;
    ListLigatureSetTable lstLigSet;
    CoverageFormat* pCover;
    void set(const u16vector &shuru, int &jgCoverIdx, int &jgShuruIdx);
    int byteLength() override;
    void shanchu(int&coverIdx,int&shuruIdx);
private:
    void xiufuPianyi();
};

//lookuptype=6
//class LigatureSubstFormat1:public Subtable{
//    Q_OBJECT
//public:
//    explicit LigatureSubstFormat1(QObject *parent = nullptr):
//        Subtable(parent),substFormat(1),pCover(nullptr),pLig(nullptr){};
//    ~LigatureSubstFormat1();
//    void read(QDataStream&dt)override;
//    void show(QString&info)override;
//    uint16	substFormat;//	Format identifier: format = 1
//    Offset16	coverageOffset;//	Offset to Coverage table, from beginning of substitution subtable
//    uint16	ligatureSetCount;//	Number of LigatureSet tables
//    //Offset16	ligatureSetOffsets[ligatureSetCount]	Array of offsets to LigatureSet tables. Offsets are from beginning of substitution subtable, ordered by Coverage index
//    u16vector ligatureSetOffsets;
//    CoverageFormat* pCover;
//    LigatureSetTable* pLig;
//};

//lookuptype=7
class ExtensionSubstFormat1:public Subtable{
    Q_OBJECT
public:
    explicit ExtensionSubstFormat1(QObject *parent = nullptr):
        Subtable(parent),substFormat(1){};
    ~ExtensionSubstFormat1(){};
    void read(QDataStream&dt)override;
    void show(QString&info)override;
    void save(QDataStream&dt)override;
    int byteLength() override;
    uint16	substFormat;//	Format identifier. Set to 1.
    uint16	extensionLookupType;//	Lookup type of subtable referenced by extensionOffset (that is, the extension subtable).
    Offset32	extensionOffset;//	Offset to the extension subtable, of lookup type extensionLookupType, relative to the start of the ExtensionSubstFormat1 subtable.
};
#endif // GSUBSUBTABLE_H
