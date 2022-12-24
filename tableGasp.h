#ifndef TABLEGASP_H
#define TABLEGASP_H
#include<types.h>
class GaspRange{
public:
    uint16	rangeMaxPPEM;//	Upper limit of range, in PPEM
    uint16	rangeGaspBehavior;//	Flags describing desired rasterizer behavior.
    void read(QDataStream&dt);
    void save(QDataStream&dt);
};
class GASPTable: public TableTTF{//ttf optional
    Q_OBJECT
public:
    GASPTable(QObject *parent=nullptr);
    ~GASPTable();
    void read(QDataStream& dt, uint32 offset);
    void save(QDataStream& dt);
    void show();
    uint16	version;//	Version number (set to 1)
    uint16	numRanges;//	Number of records to follow
    //GaspRange	gaspRanges[numRanges]	Sorted by ppem
    GaspRange *gaspRanges;
    void jisuan();
};
#endif // TABLEGASP_H
