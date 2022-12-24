#include "table2.h"

PREPTable::PREPTable(QObject *parent):TableTTF(parent){}
void PREPTable::read(QDataStream& dt, uint32 offset, uint32 tblen){
    dt.device()->seek(offset);
    for(uint i=0;i<tblen;i++){
        uint8 x;
        dt>>x;
        values.append(x);
    }
};
void PREPTable::save(QDataStream& dt){
    Q_UNUSED(dt);
}

void PREPTable::show()
{
    QString info;
    info+=QString("%1个:\r\n").arg(values.length());
    for(int i=0;i<values.length();i++){
        info+=(QString("%1 ").arg(values[i]));
    }
    datashow(info);
}


FPGMTable::FPGMTable(QObject *parent):PREPTable(parent){

}

CVTTable::CVTTable(QObject *parent):TableTTF(parent){}
void CVTTable::read(QDataStream& dt, uint32 offset,uint32 tblen){
    dt.device()->seek(offset);
    for(uint i=0;i<tblen;i++){
        FWORD x;
        dt>>x;
        values.append(x);
    }
};
void CVTTable::save(QDataStream& dt){
    Q_UNUSED(dt);
}

void CVTTable::show()
{
    QString info;
    info+=QString("%1个:\r\n").arg(values.length());
    for(int i=0;i<values.length();i++){
        info+=(QString("%1 ").arg(values[i]));
    }
    datashow(info);
}

DataMap::DataMap():pdata(nullptr)
{

}

DataMap::~DataMap()
{
    if(pdata)delete[]pdata;
}

void DataMap::read(QDataStream &dt)
{
    dt>>tag.data>>dataOffset>>dataLength;
}

void DataMap::readData(QDataStream&dt){
    pdata=new char[dataLength];
    dt.readRawData(pdata,dataLength);
}

void DataMap::saveData(QDataStream&dt){
    if(pdata){
        dt.writeRawData(pdata, dataLength);
    }
}
void DataMap::save(QDataStream &dt)
{
    dt<<tag.data<<dataOffset<<dataLength;
}

METATable::METATable(QObject *parent):TableTTF(parent),dataMaps(nullptr){}

METATable::~METATable()
{
    if(dataMaps)delete [] dataMaps;
}
void METATable::read(QDataStream& dt, uint32 offset){
    dt.device()->seek(offset);
    dt>>version>>flags>>reserved>>dataMapsCount;
    if(dataMapsCount>0){
        dataMaps=new DataMap[dataMapsCount];
        for(uint i=0;i<dataMapsCount;i++){
            dataMaps[i].read(dt);
        }
        for(uint i=0;i<dataMapsCount;i++){
            dataMaps[i].readData(dt);
        }
    }
};
void METATable::save(QDataStream& dt){
    dt<<version<<flags<<reserved<<dataMapsCount;
    if(dataMaps){
        for(uint i=0;i<dataMapsCount;i++){
            dataMaps[i].save(dt);
        }
        for(uint i=0;i<dataMapsCount;i++){
            dataMaps[i].saveData(dt);
        }
    }
}

void METATable::jisuan(){
    version=1;
    flags=0;
    reserved=40;//meta头+2个dataMaps记录=40
    dataMapsCount=2;//dlng slng
    if(dataMaps){
        delete [] dataMaps;
    }
    dataMaps=new DataMap[dataMapsCount];

    dataMaps[0].tag="dlng";
    dataMaps[0].dataOffset=40;
    dataMaps[0].dataLength=4;
    dataMaps[0].pdata=new char[dataMaps[0].dataLength];
    QString x="Hans";
    memcpy(dataMaps[0].pdata,x.toStdString().c_str(),dataMaps[0].dataLength);
    dataMaps[1].tag="slng";
    dataMaps[1].dataOffset=44;//40+4
    x="Hans, Latn";
    dataMaps[1].dataLength=x.length();
    dataMaps[1].pdata=new char[dataMaps[1].dataLength];
    memcpy(dataMaps[1].pdata,x.toStdString().c_str(),dataMaps[1].dataLength);
}

void METATable::show()
{
    datashow(QString("version:%1\t\t[1]\r\n"
                     "flags:%2\t\t[0]\r\n"
                     "reserved:%3\t\t[0]\r\n"
                     "dataMapsCount:%4\t\r\n")
             .arg(version).arg(flags).arg(reserved).arg(dataMapsCount));
    if(dataMapsCount>0){
        for(uint i=0;i<dataMapsCount;i++){
            datashow(QString("tag:%1\t\t\r\n"
                             "dataOffset:%2\t\t\r\n"
                             "dataLength:%3\t\r\n"
                             "data:%4\r\n")
                     .arg(dataMaps[i].tag).arg(dataMaps[i].dataOffset).arg(dataMaps[i].dataLength)
                     .arg(QString::fromLocal8Bit((char*)dataMaps[i].pdata,dataMaps[i].dataLength)));
        }
    }
}

