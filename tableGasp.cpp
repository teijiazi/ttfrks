#include<tableGasp.h>
void GaspRange::read(QDataStream&dt){
    dt>>rangeMaxPPEM>>rangeGaspBehavior;
}
void GaspRange::save(QDataStream&dt){
    dt<<rangeMaxPPEM<<rangeGaspBehavior;
}


GASPTable::GASPTable(QObject *parent):TableTTF(parent),gaspRanges(nullptr){}
GASPTable::~GASPTable(){
    if(gaspRanges){
        delete [] gaspRanges;
    }
}
void GASPTable::read(QDataStream& dt, uint32 offset){
    dt.device()->seek(offset);
    dt>>version>>numRanges;
    if(numRanges>0){
        gaspRanges=new GaspRange[numRanges];
        for(int i=0;i<numRanges;i++){
            gaspRanges[i].read(dt);
        }
    }
};
void GASPTable::save(QDataStream& dt){
    dt<<version<<numRanges;
    for(int i=0;i<numRanges;i++){
        gaspRanges[i].save(dt);
    }
    //不需要补零
}
void GASPTable::jisuan(){
    if(gaspRanges){
        delete [] gaspRanges;
    }
    version=1;
    numRanges=4;
    gaspRanges=new GaspRange[numRanges];
    uint16 dt[4][2]={
        {7,10},
        {17,5},
        {59,15},
        {65535,15}
    };
    for(int i=0;i<numRanges;i++){
        gaspRanges[i].rangeMaxPPEM=dt[i][0];
        gaspRanges[i].rangeGaspBehavior=dt[i][1];
    }
}

void GASPTable::show()
{
    datashow(QString("version:%1\r\n"
                      "numRanges:%2\r\n").arg(version).arg(numRanges));
    for(int i=0;i<numRanges;i++){
        datashow(QString("rangeMaxPPEM:%1\t\t\t\r\n").arg(gaspRanges[i].rangeMaxPPEM)
                 +QString("rangeGaspBehavior:%1\t\t\r\n").arg(gaspRanges[i].rangeGaspBehavior)
                +QString("    网格拟合\t\t%1\r\n").arg(gaspRanges[i].rangeGaspBehavior&0x01?"✓":"×")
                +QString("    灰度渲染\t\t%1\r\n").arg(gaspRanges[i].rangeGaspBehavior&0x02?"✓":"×")
                +QString("    网格拟合和ClearType对称平滑\t%1\r\n").arg(gaspRanges[i].rangeGaspBehavior&0x04?"✓":"×")
                +QString("    ClearType®多轴平滑\t\t%1\r\n").arg(gaspRanges[i].rangeGaspBehavior&0x08?"✓":"×"));

    }
}

