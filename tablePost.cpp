#include "tablePost.h"

void postAppend::read(QDataStream&dt,uint32 appendLen){
    dt>>numGlyphs;
    glyphNameIndex=new uint16[numGlyphs];
    dt.readRawData((char*)glyphNameIndex,numGlyphs*sizeof (uint16));
    fzStrLen=appendLen-sizeof (uint16)*(1+numGlyphs);
    stringData=new uint8[fzStrLen];
    dt.readRawData((char*)stringData,fzStrLen*sizeof(uint8));
}
void postAppend::save(QDataStream&dt){
    //返回写入的长度
    dt<<numGlyphs;
    dt.writeRawData((char*)glyphNameIndex,numGlyphs*sizeof (uint16));
    dt.writeRawData((char*)stringData,fzStrLen*sizeof(uint8));
}



POSTTable::POSTTable(QObject *parent):TableTTF(parent),
    v2append(nullptr){}
POSTTable::~POSTTable(){
    if(v2append){
        delete v2append;
    }
}
void POSTTable::jisuan(bool isDengkuan, FWORD underlineThick, FWORD underlinePos){
    version.value=0x00030000;
    italicAngle.value=0;
    underlineThickness=underlineThick;
    isFixedPitch=isDengkuan?1:0;//等宽==1，否则等于0.
    minMemType42=0;//这四项已知则填写。未知则写为0。
    maxMemType42=0;
    minMemType1=0;
    maxMemType1=0;
    underlinePosition=underlinePos;
    if(v2append){
        delete v2append;
        v2append=nullptr;
    }
}
void POSTTable::read(QDataStream& dt, uint32 offset, uint32 postLen){
    dt.device()->seek(offset);
    dt>>version.value>>italicAngle.value>>underlinePosition>>underlineThickness
            >>isFixedPitch>>minMemType42>>maxMemType42>>minMemType1>>maxMemType1;
    if(version.value==0x00020000){
        v2append=new postAppend;
        v2append->read(dt,postLen-8*sizeof(uint32));
    }
};
void POSTTable::save(QDataStream& dt){
    dt<<version.value<<italicAngle.value<<underlinePosition<<underlineThickness
     <<isFixedPitch<<minMemType42<<maxMemType42<<minMemType1<<maxMemType1;
    //不需要补零
    if(version.value==0x00020000 && v2append){
        v2append->save(dt);
    }
}

void POSTTable::show()
{
    datashow(QString("version:%1\r\n"
                      "italicAngle:%2\t\t[0=竖直文本]\r\n"
                      "underlinePosition:%3\t\t下划线的位置ymax\r\n"
                      "underlineThickness:%9\t\t也等于os/2删除线厚度\r\n"
                      "isFixedPitch:%4\t\t%10\r\n"
                      "minMemType42:%5\t\t[0表示未知，下三项同]\r\n"
                      "maxMemType42:%6\r\n"
                      "minMemType1:%7\r\n"
                      "maxMemType1:%8\r\n").arg((QString)version).arg(italicAngle)
              .arg(underlinePosition).arg(isFixedPitch).arg(minMemType42)
              .arg(maxMemType42).arg(minMemType1).arg(maxMemType1).arg(underlineThickness)
             .arg(isFixedPitch==0?"按比例（非等宽）":"不按比例（等宽）"));
}

