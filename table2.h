#ifndef TABLE2_H
#define TABLE2_H
#include "types.h"

class PREPTable:public TableTTF{//ttf optional
    Q_OBJECT
public:
    PREPTable(QObject *parent=nullptr);
    void read(QDataStream& dt, uint32 offset, uint32 tblen);
    void save(QDataStream& dt);
    void show();
    //uint8[ n ]	Set of instructions executed whenever point size or font or transformation change.
    //              n is the number of uint8 items that fit in the size of the table.
    QList<uint8> values;
};
class FPGMTable:public PREPTable{//ttf optional
    Q_OBJECT
public:
    FPGMTable(QObject *parent=nullptr);
};
class CVTTable:public TableTTF{//ttf optional
    Q_OBJECT
public:
    CVTTable(QObject *parent=nullptr);
    void read(QDataStream& dt, uint32 offset, uint32 tblen);
    void save(QDataStream& dt);
    void show();
    //FWORD[ n ]	List of n values referenceable by instructions.
    //              n is the number of FWORD items that fit in the size of the table.
    QList<FWORD> values;
};
class DataMap{
public:
    DataMap();
    ~DataMap();
    Tag	tag;//	A tag indicating the type of metadata.
    Offset32	dataOffset;//	Offset in bytes from the beginning of the metadata table to the data for this tag.
    uint32	dataLength;//	Length of the data, in bytes. The data is not required to be padded to any byte boundary.
    char* pdata;
    void read(QDataStream&dt);
    void save(QDataStream&dt);
    void readData(QDataStream &dt);
    void saveData(QDataStream &dt);
};

class METATable:public TableTTF{
    Q_OBJECT
public:
    METATable(QObject *parent=nullptr);
    ~METATable();
    void read(QDataStream& dt, uint32 offset);
    void save(QDataStream& dt);
    void show();
    //
    uint32	version;//	Version number of the metadata table — set to 1.
    uint32	flags;//	Flags — currently unused; set to 0.
    uint32	reserved;//	Not used; should be set to 0.
    uint32	dataMapsCount;//	The number of data maps in the table.
    //DataMap	dataMaps[dataMapsCount]	Array of data map records.
    DataMap*dataMaps;
    void jisuan();
};



#endif // TABLE2_H
