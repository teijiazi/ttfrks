#ifndef TABLEGPOS_H
#define TABLEGPOS_H
#include<tableComm.h>

class GPOSTable:public TableTTF{
    //新建version=1.0，不使用1.1
    //新建依赖：
    //需要数据:vEndGlyphID
    Q_OBJECT
public:
    GPOSTable(QObject *parent=nullptr);
    ~GPOSTable();
    void read(QDataStream& dt, uint32 offset);
    void xinjian();
    void save(QDataStream& dt);
    void show();
    bool cunzai;
    //header
    uint16 majorVersion;//	Major version of the GPOS table, = 1
    uint16 minorVersion;//	Minor version of the GPOS table, = 0
    Offset16 scriptListOffset;//	Offset to ScriptList table, from beginning of GPOS table
    Offset16 featureListOffset;//	Offset to FeatureList table, from beginning of GPOS table
    Offset16 lookupListOffset;//	Offset to LookupList table, from beginning of GPOS table
    //ver1.1
    Offset32	featureVariationsOffset;//	Offset to FeatureVariations table, from beginning of GPOS table (may be NULL)
    ScriptListTable* scriptListTable;
    FeatureListTable* featureListTable;
    LookupListTable* lookupListTable;
    //
    QString info;
    void jisuan();
};
#endif // TABLEGPOS_H
