#ifndef GPOSSUBTABLE_H
#define GPOSSUBTABLE_H

#include <QObject>
#include "types.h"
#include<subtablecomm.h>

static const QString gposSubtableType[]={
    "",//对应lookuptype，1开始
    "Single adjustment",//	Adjust position of a single glyph
    "Pair adjustment",	//Adjust position of a pair of glyphs
    "Cursive attachment",//	Attach cursive glyphs
    "MarkToBase attachment",//	Attach a combining mark to a base glyph
    "MarkToLigature attachment",//	Attach a combining mark to a ligature
    "MarkToMark attachment",	//Attach a combining mark to another mark
    "Context positioning",	//Position one or more glyphs in context
    "Chained Context positioning",	//Position one or more glyphs in chained context
    "Extension positioning",
};

class PairPosFormat1:public Subtable{
public:
    uint16	posFormat;//	Format identifier: format = 1
    Offset16	coverageOffset;//	Offset to Coverage table, from beginning of PairPos subtable.
    uint16	valueFormat1;//	Defines the types of data in valueRecord1 — for the first glyph in the pair (may be zero).
    uint16	valueFormat2;//	Defines the types of data in valueRecord2 — for the second glyph in the pair (may be zero).
    uint16	pairSetCount;//	Number of PairSet tables
    //Offset16	pairSetOffsets[pairSetCount]	Array of offsets to PairSet tables. Offsets are from beginning of PairPos subtable, ordered by Coverage Index.
    void read(QDataStream&dt)override;
    void show(QString &info)override;
    void save(QDataStream &dt) override;
    PairPosFormat1():posFormat(1){}
    int byteLength() override;
};
class ValueRecord{
public:
    enum{
        X_PLACEMENT    	=0x0001,//Includes horizontal adjustment for placement
        Y_PLACEMENT        =0x0002,//Includes vertical adjustment for placement
        X_ADVANCE          =0x0004,//Includes horizontal adjustment for advance
        Y_ADVANCE          =0x0008,//Includes vertical adjustment for advance
        X_PLACEMENT_DEVICE	=0x0010,//Includes Device table (non-variable font) / VariationIndex table (variable font) for horizontal placement
        Y_PLACEMENT_DEVICE	=0x0020,//Includes Device table (non-variable font) / VariationIndex table (variable font) for vertical placement
        X_ADVANCE_DEVICE	=0x0040,//Includes Device table (non-variable font) / VariationIndex table (variable font) for horizontal advance
        Y_ADVANCE_DEVICE	=0x0080,//Includes Device table (non-variable font) / VariationIndex table (variable font) for vertical advance
        Reserved           =0xFF00,//For future use (set to zero)
    };
    int16	xPlacement;//	Horizontal adjustment for placement, in design units.
    int16	yPlacement;//	Vertical adjustment for placement, in design units.
    int16	xAdvance;//	Horizontal adjustment for advance, in design units — only used for horizontal layout.
    int16	yAdvance;//	Vertical adjustment for advance, in design units — only used for vertical layout.
    Offset16	xPlaDeviceOffset;//	Offset to Device table (non-variable font) / VariationIndex table (variable font) for horizontal placement, from beginning of the immediate parent table (SinglePos or PairPosFormat2 lookup subtable, PairSet table within a PairPosFormat1 lookup subtable) — may be NULL.
    Offset16	yPlaDeviceOffset;//	Offset to Device table (non-variable font) / VariationIndex table (variable font) for vertical placement, from beginning of the immediate parent table (SinglePos or PairPosFormat2 lookup subtable, PairSet table within a PairPosFormat1 lookup subtable) — may be NULL.
    Offset16	xAdvDeviceOffset;//	Offset to Device table (non-variable font) / VariationIndex table (variable font) for horizontal advance, from beginning of the immediate parent table (SinglePos or PairPosFormat2 lookup subtable, PairSet table within a PairPosFormat1 lookup subtable) — may be NULL.
    Offset16	yAdvDeviceOffset;//	Offset to Device table (non-variable font) / VariationIndex table (variable font) for vertical advance, from beginning of the immediate parent table (SinglePos or PairPosFormat2 lookup subtable, PairSet table within a PairPosFormat1 lookup subtable) — may be NULL.
    void read(QDataStream&dt,uint16 valueFormat);
    void save(QDataStream&dt,uint16 valueFormat);
    void show(QString &info, uint16 valueFormat);
};

class Class2Record{
public:
    ValueRecord	valueRecord1;//	Positioning for first glyph — empty if valueFormat1 = 0.
    ValueRecord	valueRecord2;//	Positioning for second glyph — empty if valueFormat2 = 0.
    void read(QDataStream&dt, uint16 valueFormat1, uint16 valueFormat2);
    void save(QDataStream&dt,uint16 valueFormat1, uint16 valueFormat2);
    void show(int i, QString &info, uint16 valueFormat1, uint16 valueFormat2);
};
class Class1Record{
public:
    Class1Record();
    ~Class1Record();
    //Class2Record	class2Records[class2Count]	Array of Class2 records, ordered by classes in classDef2.
    Class2Record* pClass2Record;
    void read(QDataStream&dt, uint16 class2Count, uint16 valueFormat1, uint16 valueFormat2);
    void save(QDataStream&dt,uint16 class2Count,uint16 valueFormat1, uint16 valueFormat2);
    void show(int i, QString &info, uint16 class2Count, uint16 valueFormat1, uint16 valueFormat2);
};
class PairPosFormat2:public Subtable{
public:
    uint16	posFormat;//	Format identifier: format = 2
    Offset16	coverageOffset;//	Offset to Coverage table, from beginning of PairPos subtable.
    uint16	valueFormat1;//	ValueRecord definition — for the first glyph of the pair (may be zero).
    uint16	valueFormat2;//	ValueRecord definition — for the second glyph of the pair (may be zero).
    Offset16	classDef1Offset;//	Offset to ClassDef table, from beginning of PairPos subtable — for the first glyph of the pair.
    Offset16	classDef2Offset;//	Offset to ClassDef table, from beginning of PairPos subtable — for the second glyph of the pair.
    uint16	class1Count;//	Number of classes in classDef1 table — includes Class 0.
    uint16	class2Count;//	Number of classes in classDef2 table — includes Class 0.
    //Class1Record	class1Records[class1Count]	Array of Class1 records, ordered by classes in classDef1.
    Class1Record* pClass1Record;
    CoverageFormat* pCover;
    ClassDefFormat1 *pClassDef1_format1;
    ClassDefFormat2 *pClassDef1_format2;
    ClassDefFormat1 *pClassDef2_format1;
    ClassDefFormat2 *pClassDef2_format2;
    PairPosFormat2();
    ~PairPosFormat2();
    void read(QDataStream&dt)override;
    void save(QDataStream&dt)override;
    void show(QString &info)override;
    int byteLength() override;
};

class MarkRecord{
public:
    uint16	markClass;//	Class defined for the associated mark.
    Offset16	markAnchorOffset;//	Offset to Anchor table, from beginning of MarkArray table.
};

class MarkArrayTable{
public:
    MarkArrayTable():pMarkRecords(nullptr){};
    ~MarkArrayTable(){
        if(pMarkRecords){
            delete [] pMarkRecords;
        }
    }
    uint16	markCount;//	Number of MarkRecords
    //MarkRecord	markRecords[markCount]	Array of MarkRecords, ordered by corresponding glyphs in the associated mark Coverage table.
    MarkRecord* pMarkRecords;
    void read(QDataStream&dt);
    void show(QString&info);
};

class BaseRecord{
public:
    //Offset16	baseAnchorOffsets[markClassCount]	Array of offsets (one per mark class) to Anchor tables. Offsets are from beginning of BaseArray table, ordered by class (offsets may be NULL).
    u16vector baseAnchorOffsets;
};
class BaseArrayTable{
public:
    BaseArrayTable():pBaseRecord(nullptr){}
    ~BaseArrayTable(){
        if(pBaseRecord){
            delete []pBaseRecord;
        }
    }
    uint16	baseCount;//	Number of BaseRecords
    //BaseRecord	baseRecords[baseCount]	Array of BaseRecords, in order of baseCoverage Index.
    BaseRecord* pBaseRecord;
    void read(QDataStream&dt);
    void show(QString&info);
    uint16 fzmarkClassCount;
};

//lookup type=4
class MarkBasePosFormat1:public Subtable{//只有格式1
public:
    MarkBasePosFormat1():posFormat(1),markCover(nullptr),baseCover(nullptr){}
    ~MarkBasePosFormat1();
    void read(QDataStream&dt)override;
    void show(QString &info)override;
    void save(QDataStream &dt) override;
    uint16      posFormat;//	Format identifier: format = 1
    Offset16	markCoverageOffset;//	Offset to markCoverage table, from beginning of MarkBasePos subtable.
    Offset16	baseCoverageOffset;//	Offset to baseCoverage table, from beginning of MarkBasePos subtable.
    uint16      markClassCount;//	Number of classes defined for marks
    Offset16	markArrayOffset;//	Offset to MarkArray table, from beginning of MarkBasePos subtable.
    Offset16	baseArrayOffset;//	Offset to BaseArray table, from beginning of MarkBasePos subtable.
    CoverageFormat *markCover;
    CoverageFormat *baseCover;
    MarkArrayTable mat;
    BaseArrayTable bat;
    int byteLength() override;
};

//lookup type=8
//Simple Glyph Contexts
class ChainedContextsPositioningFormat1: public ChainedSequenceContextFormat1, public Subtable{
public:
    ChainedContextsPositioningFormat1(){}
    ~ChainedContextsPositioningFormat1(){};
    void read(QDataStream&dt)override;
    void show(QString &info)override;
    void save(QDataStream &dt) override;
    int byteLength() override;
};

//Class-based Glyph Contexts
class ChainedContextsPositioningFormat2: public ChainedSequenceContextFormat2, public Subtable{
public:
    ChainedContextsPositioningFormat2(){}
    ~ChainedContextsPositioningFormat2(){};
    void read(QDataStream&dt)override;
    void show(QString &info)override;
    void save(QDataStream &dt) override;
    int byteLength() override;
};

//Coverage-based Glyph Contexts
class ChainedContextsPositioningFormat3: public ChainedSequenceContextFormat3, public Subtable{
public:
    ChainedContextsPositioningFormat3(){}
    ~ChainedContextsPositioningFormat3(){};
    void read(QDataStream&dt)override;
    void show(QString &info)override;
    void save(QDataStream &dt) override;
    int byteLength() override;
};

//lookuptype=9
class ExtensionPosFormat1:public Subtable{
    Q_OBJECT
public:
    explicit ExtensionPosFormat1(QObject *parent = nullptr):
        Subtable(parent),posFormat(1){};
    ~ExtensionPosFormat1(){};
    void read(QDataStream&dt)override;
    void show(QString&info)override;
    void save(QDataStream &dt) override;
    uint16	posFormat;//	Format identifier. Set to 1.
    uint16	extensionLookupType;//	Lookup type of subtable referenced by extensionOffset (i.e. the extension subtable).
    Offset32	extensionOffset;//	Offset to the extension subtable, of lookup type extensionLookupType,
    //relative to the start of the ExtensionPosFormat1 subtable.
    int byteLength() override;
};
#endif // GPOSSUBTABLE_H
