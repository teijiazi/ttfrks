#ifndef TABLEGDEF_H
#define TABLEGDEF_H
#include<tableComm.h>
class GDEFTable:public TableTTF{
    //新建version=1.0，不使用1.2 1.3
    //新建依赖：
    //需要数据:vEndGlyphID
    Q_OBJECT
public:
    GDEFTable(QObject *parent=nullptr);
    ~GDEFTable();
    void read(QDataStream& dt, uint32 offset);
    void jisuan(uint16 vEndGlyphID);
    void save(QDataStream &dt);
    void show();
    QString info;
    uint16	majorVersion;//	Major version of the GDEF table, = 1
    uint16	minorVersion;//	Minor version of the GDEF table, = 0
    Offset16	glyphClassDefOffset;//	Offset to class definition table for glyph type, from beginning of GDEF header (may be NULL)
    Offset16	attachListOffset;//	Offset to attachment point list table, from beginning of GDEF header (may be NULL)
    Offset16	ligCaretListOffset;//	Offset to ligature caret list table, from beginning of GDEF header (may be NULL)
    Offset16	markAttachClassDefOffset;//	Offset to class definition table for mark attachment type, from beginning of GDEF header (may be NULL)
    //ver 1.2
    Offset16	markGlyphSetsDefOffset;//	Offset to the table of mark glyph set definitions, from beginning of GDEF header (may be NULL)
    //ver 1.3
    Offset32	itemVarStoreOffset;//	Offset to the Item Variation Store table, from beginning of GDEF header (may be NULL)
    //
    ClassDefFormat2 *pGlyphClassDef;
};
#endif // TABLEGDEF_H
