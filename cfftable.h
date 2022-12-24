#ifndef CFFTABLE_H
#define CFFTABLE_H

#include<types.h>

typedef uint8 Card8,OffSize;
typedef uint16 Card16,CFFSID;
typedef uint32 Offset;

class DICTData{
public:
    explicit DICTData(QByteArray &ba);
    QList<uint16> ddOperator;
    QByteArrayList ddOperand;
    void showTopDict(QString& info);
    void readTopDict(int32 &charStringOffset, int32 &charsetOffset, int& CharstringType);
private:
    QString topOperator(uint16 czf);
    QString topOperand(uint16 czf,const QByteArray &czs);
    QString topOperatorDec(uint16 czf);
    QString toNumber(const QByteArray &czs, int *yiduchangdu=nullptr);
    QString toBoolean(const QByteArray &czs);
    QString toSIDStr(const QByteArray &czs);
    QString toArray(const QByteArray &czs);
    QString toDelta(const QByteArray &czs);
    QString banzijie(uint8 bzj, bool *jieshu=nullptr);
};

class INDEXFormat{
public:
    INDEXFormat():count(0){}
    Card16 count;//Number of objects stored in INDEX
    OffSize offSize;//Offset array element size
    //Offset offset[count+1];//Offset array (from byte preceding object data)
    QList<Offset> offset;
    //Card8 data[<varies>];//Object data
    QList<QByteArray> data;
    void read(QDataStream& dt);
    void show(QString&info);
    void showChar(QString&info, int indexBegin=0);
    void showCharStrings(QString&info);
};
class TopDictINDEXFormat:public INDEXFormat{
public:
    int32 charsetOffset;
    int32 charStringOffset;
    int CharstringType;
    void showDICT(QString&info);
    void readDICT(QDataStream& dt);
};

class CharsetsFormat{
public:
    CharsetsFormat(Card8 fmt):format(fmt){}
    virtual ~CharsetsFormat(){};
    Card8    format;//
    virtual void read(QDataStream&dt)=0;
    virtual void show(QString &info)=0;
};
class CharsetsFormat0:public CharsetsFormat{
public:
    explicit CharsetsFormat0(uint16 xzifuCount):CharsetsFormat(0),zifuCount(xzifuCount){}
    ~CharsetsFormat0(){};
    //CFFSID    glyph[nGlyphs–1]    Glyph name array
    QList<CFFSID> glyph;
    uint16 zifuCount;
    void read(QDataStream &dt) override;
    void show(QString &info)override;
};
class CharsetsFormat1:public CharsetsFormat{
public:
    CharsetsFormat1():CharsetsFormat(1){}
    ~CharsetsFormat1(){};
    void read(QDataStream &dt) override;
    void show(QString &info)override;
};
class CharsetsFormat2:public CharsetsFormat{
public:
    CharsetsFormat2():CharsetsFormat(2){}
    ~CharsetsFormat2(){};
    void read(QDataStream &dt) override;
    void show(QString &info)override;
};


class CFFTable : public TableTTF
{
    Q_OBJECT
public:
    explicit CFFTable(QObject *parent = nullptr);
    ~CFFTable();
    bool read(QDataStream& dt, uint32 offset, uint16 maxpNumGlyphs);
    void save(QDataStream& dt);
    void show();
    Card8    major;//    Format major version (starting at 1)
    Card8    minor;//    Format minor version (starting at 0)
    Card8    hdrSize;//    Header size (bytes)
    OffSize    offSize;//    Absolute offset (0) size
    INDEXFormat NameINDEX;
    TopDictINDEXFormat TopDICTINDEX;
    INDEXFormat StringINDEX;
    INDEXFormat GlobalsubrsINDEX;
    INDEXFormat CharStringsINDEX;
    CharsetsFormat* pCharsetsFormat;
signals:

};

#endif // CFFTABLE_H
