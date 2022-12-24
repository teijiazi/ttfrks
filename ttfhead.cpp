#include <ttfhead.h>

TTFLeibie OffsetTable::read(QDataStream& dt){
    //set 读取dt，给成员赋值 文件错误返回false
    dt>>sfntVersion>>numTables>>searchRange>>entrySelector>>rangeShift;
    if(searchRange==pow(2,entrySelector)*16
            && rangeShift==numTables*16-searchRange){
        if(sfntVersion==0x00010000){
            return tlTTF;
        }else if(sfntVersion==0x4F54544F){
            return tlOTF;
        }
    }
    return tlERR;
}
void OffsetTable::save(QDataStream& dt){
    //把成员的值存储到dt
    dt<<sfntVersion<<numTables<<searchRange<<entrySelector<<rangeShift;
}
void OffsetTable::jisuan(uint16 vNumTable){
    numTables=vNumTable;
    sfntVersion=0x00010000;//truetype
    double x=numTables;
    double ex=log10(x)/log10(2);
    entrySelector=(uint16)floor(ex);
    searchRange=pow(2,entrySelector)*16;
    rangeShift=numTables*16-searchRange;
}


TableRecord::TableRecord():checkSum(0),offset(0),length(0){

}
void TableRecord::read(QDataStream &dt){
    dt>>tableTag.data>>checkSum>>offset>>length;
}
void TableRecord::save(QDataStream& dt){
    dt<<tableTag.data<<checkSum<<offset<<length;
}

