#include "tablevhea.h"

VHEATable::VHEATable(QObject *parent):TableTTF(parent),
    version(0x00010000),lineGap(0),reserved1(0),reserved2(0),reserved3(0),reserved4(0),
    metricDataFormat(0)
{

}

void VHEATable::read(QDataStream &dt, uint32 offset)
{
    dt.device()->seek(offset);
    dt>>version>>ascent>>descent>>lineGap>>advanceHeightMax>>minTopSideBearing
            >>minBottomSideBearing>>yMaxExtent>>caretSlopeRise>>caretSlopeRun>>
            caretOffset>>reserved1>>reserved2>>reserved3>>reserved4
            >>metricDataFormat>>numOfLongVerMetrics;
}

void VHEATable::save(QDataStream &dt)
{
    Q_UNUSED(dt);
}

void VHEATable::show()
{
    datashow(QString("version:%1\t\t[1.0]\r\n"
                     "%18:%2\r\n"
                     "%19:%3\r\n"
                     "%20:%4\t\t[0]\r\n"
                     "advanceHeightMax:%5\r\n"
                     "minTopSideBearing:%6\r\n"
                     "minBottomSideBearing:%7\r\n"
                     "yMaxExtent:%8\r\n"
                     "caretSlopeRise:%9\r\n"
                     "caretSlopeRun:%10\r\n"
                     "caretOffset:%11\r\n"
                     "reserved1:%12\t\t[0]\r\n"
                     "reserved2:%13\t\t[0]\r\n"
                     "reserved3:%14\t\t[0]\r\n"
                     "reserved4:%15\t\t[0]\r\n"
                     "metricDataFormat:%16\t\t[0]\r\n"
                     "numOfLongVerMetrics:%17\r\n")
             .arg(version.operator QString()).arg(ascent).arg(descent).arg(lineGap).arg(advanceHeightMax)
             .arg(minTopSideBearing).arg(minBottomSideBearing).arg(yMaxExtent).arg(caretSlopeRise)
             .arg(caretSlopeRun).arg(caretOffset)
             .arg(reserved1).arg(reserved2).arg(reserved3).arg(reserved4)
             .arg(metricDataFormat).arg(numOfLongVerMetrics)
             .arg(version==0x00010000?"ascent":"vertTypoAscender")
             .arg(version==0x00010000?"descent":"vertTypoDescender")
             .arg(version==0x00010000?"lineGap":"vertTypoLineGap")
             );
}

VMTXTable::VMTXTable(QObject *parent):TableTTF(parent){}
void VMTXTable::read(QDataStream& dt, uint32 offset){
    dt.device()->seek(offset);
};
void VMTXTable::save(QDataStream& dt){
    Q_UNUSED(dt);
}

void VMTXTable::show()
{

}
