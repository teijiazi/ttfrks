#ifndef SUBTABLECOMM_H
#define SUBTABLECOMM_H

#include <QObject>
#include "types.h"

#define LIST(rec) typedef QList<rec> List##rec;\
    typedef QList<rec *> ListP##rec

class Subtable : public QObject
{
    Q_OBJECT
public:
    explicit Subtable(QObject *parent = nullptr):QObject(parent){};
    virtual ~Subtable(){};
    virtual void read(QDataStream&dt)=0;
    virtual void show(QString&info)=0;
    virtual void save(QDataStream&dt)=0;
    virtual int byteLength()=0;
signals:

};
LIST(Subtable);

class ClassDefFormat1{
public:
    ClassDefFormat1():classFormat(1),classValueArray(nullptr){}
    ~ClassDefFormat1(){
        if(classValueArray){
            delete [] classValueArray;
        }
    }
    uint16	classFormat;//	Format identifier — format = 1
    uint16	startGlyphID;//	First glyph ID of the classValueArray
    uint16	glyphCount;//	Size of the classValueArray
    //uint16	classValueArray[glyphCount]	Array of Class Values — one per glyph ID
    uint16* classValueArray;
    void read(QDataStream&dt, bool formatYidu);
    void save(QDataStream&dt);
    void show(QString &info);
};

class ClassRangeRecord{
public:
    uint16	startGlyphID;//	First glyph ID in the range
    uint16	endGlyphID;//	Last glyph ID in the range
    uint16	classValue;//	Applied to all glyphs in the range
};
class ClassDefFormat2{
public:
    ClassDefFormat2();
    ~ClassDefFormat2();
    void arr();
    void read(QDataStream& dt,bool formatYidu);
    void save(QDataStream&dt);
    uint16	classFormat;//	Format identifier — format = 2
    uint16	classRangeCount;//	Number of ClassRangeRecords
    //ClassRangeRecord classRangeRecords[classRangeCount];//	Array of ClassRangeRecords — ordered by startGlyphID
    ClassRangeRecord* pClassRangeRecord;
    void show(QString &info);
};

class RangeRecord{
public:
    uint16	startGlyphID;//	First glyph ID in the range
    uint16	endGlyphID;//	Last glyph ID in the range
    uint16	startCoverageIndex;//	Coverage Index of first glyph ID in range
    void read(QDataStream& dt);
    void save(QDataStream&dt);
    void show(int i, QString &info);
};
LIST(RangeRecord);

class CoverageFormat{
public:
    uint16	coverageFormat;
    explicit CoverageFormat(uint16 fmt);
    virtual ~CoverageFormat();
    virtual void read(QDataStream& dt)=0;
    virtual void save(QDataStream&dt)=0;
    virtual void show(QString &info)=0;
    virtual void getCoverGlyphIdx(u16vector &glyphIndexVec)=0;
    virtual uint16 coverIndex2GlyphIndex(int coverIndex)=0;
    virtual void set(uint16 id)=0;
    virtual int indexOf(int16 gid)=0;
    virtual int byteLength()=0;
    virtual void shanchu(int &coverIdx)=0;
    virtual void quanbuTihuan(const u16vector &ids)=0;
};

class CoverageFormat1:public CoverageFormat{
public:
    uint16	glyphCount;//	Number of glyphs in the glyph array
    //uint16	glyphArray[glyphCount]	Array of glyph IDs — in numerical order
    u16vector glyphArray;
    explicit CoverageFormat1();
    ~CoverageFormat1();
    void read(QDataStream& dt)override;
    void save(QDataStream&dt)override;
    void show(QString &info)override;
    void getCoverGlyphIdx(u16vector &glyphIndexVec)override;
    uint16 coverIndex2GlyphIndex(int coverIndex)override;
    void set(uint16 id)override;
    int indexOf(int16 gid)override;
    int byteLength() override;
    void shanchu(int &coverIdx)override;
    void quanbuTihuan(const u16vector &ids)override;
};

class CoverageFormat2:public CoverageFormat{
public:
    uint16	rangeCount;//	Number of RangeRecords
    //RangeRecord	rangeRecords[rangeCount]	Array of glyph ranges — ordered by startGlyphID.
    ListRangeRecord lstRangeRecord;
    explicit CoverageFormat2();
    ~CoverageFormat2();
    void read(QDataStream& dt)override;
    void save(QDataStream&dt)override;
    void show(QString &info)override;
    void getCoverGlyphIdx(u16vector &glyphIndexVec)override;
    uint16 coverIndex2GlyphIndex(int coverIndex)override;
    void set(uint16 id)override;
    int indexOf(int16 gid)override;
    int byteLength() override;
    void shanchu(int &coverIdx)override;
    void quanbuTihuan(const u16vector &ids)override;
};

class SequenceLookupRecord{
public:
    uint16	sequenceIndex;//	Index (zero-based) into the input glyph sequence
    uint16	lookupListIndex;//	Index (zero-based) into the LookupList
    void read(QDataStream&dt);
    void show(QString&info);
};

class ChainedSequenceContextFormat1{
public:
    ChainedSequenceContextFormat1():format(1){}
    uint16	format;//	Format identifier: format = 1
    Offset16	coverageOffset;//	Offset to Coverage table, from beginning of ChainSequenceContextFormat1 table
    uint16	chainedSeqRuleSetCount;//	Number of ChainedSequenceRuleSet tables
    //Offset16	chainedSeqRuleSetOffsets[chainedSeqRuleSetCount]	Array of offsets to ChainedSeqRuleSet tables, from beginning of ChainedSequenceContextFormat1 table (may be NULL)
    u16vector chainedSeqRuleSetOffsets;
};

class ChainedSequenceContextFormat2{
public:
    ChainedSequenceContextFormat2():format(2){}
    uint16	format;//	Format identifier: format = 2
    Offset16	coverageOffset;//	Offset to Coverage table,
    //from beginning of ChainedSequenceContextFormat2 table
    Offset16	backtrackClassDefOffset	;//Offset to ClassDef table containing backtrack sequence context,
    //from beginning of ChainedSequenceContextFormat2 table
    Offset16	inputClassDefOffset;//	Offset to ClassDef table containing input sequence context,
    //from beginning of ChainedSequenceContextFormat2 table
    Offset16	lookaheadClassDefOffset;//	Offset to ClassDef table containing lookahead sequence context,
    //from beginning of ChainedSequenceContextFormat2 table
    uint16	chainedClassSeqRuleSetCount;//	Number of ChainedClassSequenceRuleSet tables
    //Offset16	chainedClassSeqRuleSetOffsets[chainedClassSeqRuleSetCount]	Array of offsets to ChainedClassSequenceRuleSet tables,
    //from beginning of ChainedSequenceContextFormat2 table (may be NULL)
    u16vector chainedClassSeqRuleSetOffsets;
};

class ChainedSequenceContextFormat3{
public:
    ChainedSequenceContextFormat3():format(3){}
    uint16	format;//	Format identifier: format = 3
    uint16	backtrackGlyphCount;//	Number of glyphs in the backtrack sequence
    //Offset16	backtrackCoverageOffsets[backtrackGlyphCount]	Array of offsets to coverage tables for the backtrack sequence
    u16vector backtrackCoverageOffsets;
    uint16	inputGlyphCount;//	Number of glyphs in the input sequence
    //Offset16	inputCoverageOffsets[inputGlyphCount]	Array of offsets to coverage tables for the input sequence
    u16vector inputCoverageOffsets;
    uint16	lookaheadGlyphCount;//	Number of glyphs in the lookahead sequence
    //Offset16	lookaheadCoverageOffsets[lookaheadGlyphCount]	Array of offsets to coverage tables for the lookahead sequence
    u16vector lookaheadCoverageOffsets;
    uint16	seqLookupCount;//	Number of SequenceLookupRecords
    //SequenceLookupRecord	seqLookupRecords[seqLookupCount]	Array of SequenceLookupRecords
    QList<SequenceLookupRecord> seqLookupRecords;
    void read(QDataStream&dt);
    void show(QString& info);
};

#endif // SUBTABLECOMM_H
