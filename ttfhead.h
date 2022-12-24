#ifndef TTFHEAD_H
#define TTFHEAD_H

#include <QWidget>
#include "types.h"

//#define BAOCUN_GPOS
//#define BAOCUN_META

enum TTFLeibie{
    tlTTF,
    tlOTF,
    tlERR
};

class OffsetTable//1320
{
public:
    uint32 sfntVersion;// 0x00010000 or 0x4F54544F ('OTTO')  — see below.
    uint16 numTables;// Number of tables.
    uint16 searchRange;// (Maximum power of 2 <= numTables) x  16.
    uint16 entrySelector;// Log2(maximum power of 2 <=  numTables).
    uint16 rangeShift;// NumTables x 16-searchRange.
    TTFLeibie read(QDataStream& dt);
    void save(QDataStream& dt);
    void jisuan(uint16 vNumTable);
};

class TableRecord{
public:
    TableRecord();
    Tag tableTag;// Table identifier.
    uint32 checkSum;// CheckSum for this table.
    Offset32 offset;// Offset from beginning of TrueType font file.
    uint32 length;// Length of this table.
    void read(QDataStream &dt);
    void save(QDataStream& dt);
};


#endif // TTFHEAD_H
