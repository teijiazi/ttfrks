#include "gpossubtable.h"

PairPosFormat2::PairPosFormat2():posFormat(2),pClass1Record(nullptr),pCover(nullptr),
    pClassDef1_format1(nullptr),pClassDef1_format2(nullptr),
    pClassDef2_format1(nullptr),pClassDef2_format2(nullptr)
{

}

PairPosFormat2::~PairPosFormat2()
{
    if(pClass1Record){
        delete []pClass1Record;
    }
    if(pCover){
        delete pCover;
    }
    if(pClassDef1_format1){
        delete pClassDef1_format1;
    }
    if(pClassDef1_format2){
        delete pClassDef1_format2;
    }
    if(pClassDef2_format1){
        delete pClassDef2_format1;
    }
    if(pClassDef2_format2){
        delete pClassDef2_format2;
    }
}

void PairPosFormat2::read(QDataStream&dt){
    dt>>coverageOffset>>valueFormat1>>valueFormat2
            >>classDef1Offset>>classDef2Offset>>class1Count>>class2Count;
    if(class1Count>0){
        pClass1Record=new Class1Record[class1Count];
        for(int i=0;i<class1Count;i++){
            pClass1Record[i].read(dt,class2Count,valueFormat1, valueFormat2);
        }
    }

    uint16	coverageFormat;
    dt>>coverageFormat;
    if(coverageFormat==1){
        pCover=new CoverageFormat1;
    }else if(coverageFormat==2){
        pCover=new CoverageFormat2;
    }
    pCover->read(dt);

    uint16	format;
    dt>>format;
    if(format==1){
        pClassDef1_format1=new ClassDefFormat1;
        pClassDef1_format1->read(dt,true);
    }else{
        pClassDef1_format2=new ClassDefFormat2;
        pClassDef1_format2->read(dt,true);
    }

    dt>>format;
    if(format==1){
        pClassDef2_format1=new ClassDefFormat1;
        pClassDef2_format1->read(dt,true);
    }else{
        pClassDef2_format2=new ClassDefFormat2;
        pClassDef2_format2->read(dt,true);
    }
}
void PairPosFormat2::show(QString&info){
    info.append(QString("PairPosFormat2\r\n  posFormat:%1\r\n  coverageOffset:%2\r\n  valueFormat1:%3\r\n  valueFormat2:%4"
                        "\r\n  classDef1Offset:%5\r\n  classDef2Offset:%6\r\n  class1Count:%7\r\n  class2Count:%8\r\n")
                .arg(posFormat).arg(coverageOffset).arg(valueFormat1)
                .arg(valueFormat2).arg(classDef1Offset).arg(classDef2Offset).arg(class1Count).arg(class2Count));
    if(class1Count>0){
        if(pClass1Record){
            for(int i=0;i<class1Count;i++){
                pClass1Record[i].show(i,info,class2Count,valueFormat1, valueFormat2);
            }
        }
    }

    if(pCover){
        pCover->show(info);
    }

    if(pClassDef1_format1){
        pClassDef1_format1->show(info);
    }else if(pClassDef1_format2){
        pClassDef1_format2->show(info);
    }

    if(pClassDef2_format1){
        pClassDef2_format1->show(info);
    }else if(pClassDef2_format2){
        pClassDef2_format2->show(info);
    }
}

int PairPosFormat2::byteLength()
{
    return 0;
}
void PairPosFormat2::save(QDataStream&dt){
    dt<<posFormat<<coverageOffset<<valueFormat1<<valueFormat2
     <<classDef1Offset<<classDef2Offset<<class1Count<<class2Count;
    if(class1Count>0){
        for(int i=0;i<class1Count;i++){
            pClass1Record[i].save(dt,class2Count,valueFormat1, valueFormat2);
        }
    }
    if(pCover){
        pCover->save(dt);
    }
    if(pClassDef1_format1){
        pClassDef1_format1->save(dt);
    }
    if(pClassDef1_format2){
        pClassDef1_format2->save(dt);
    }
    if(pClassDef2_format1){
        pClassDef2_format1->save(dt);
    }
    if(pClassDef2_format2){
        pClassDef2_format2->save(dt);
    }
}

Class1Record::Class1Record():pClass2Record(nullptr)
{

}

Class1Record::~Class1Record()
{
    if(pClass2Record){
        delete []pClass2Record;
    }
}

void Class1Record::read(QDataStream &dt, uint16 class2Count,uint16 valueFormat1, uint16 valueFormat2)
{
    pClass2Record=new Class2Record[class2Count];
    for(int c2i=0;c2i<class2Count;c2i++){
        pClass2Record[c2i].read(dt,valueFormat1,valueFormat2);
    }
}
void Class1Record::show(int i,QString &info,uint16 class2Count,uint16 valueFormat1, uint16 valueFormat2)
{
    info.append(QString("  [%1]pClass1Record\r\n").arg(i));
    if(pClass2Record){
        for(int c2i=0;c2i<class2Count;c2i++){
            pClass2Record[c2i].show(c2i,info,valueFormat1,valueFormat2);
        }
    }
}
void Class1Record::save(QDataStream &dt,uint16 class2Count,uint16 valueFormat1, uint16 valueFormat2)
{
    if(pClass2Record){
        for(int i=0;i<class2Count;i++){
            pClass2Record[i].save(dt,valueFormat1,valueFormat2);
        }
    }
}

void Class2Record::read(QDataStream &dt, uint16 valueFormat1, uint16 valueFormat2)
{
    if(valueFormat1){
        valueRecord1.read(dt,valueFormat1);
    }
    if(valueFormat2){
        valueRecord2.read(dt,valueFormat2);
    }
}
void Class2Record::show(int i, QString &info, uint16 valueFormat1, uint16 valueFormat2)
{
    info.append(QString("    [%1]pClass2Record\r\n").arg(i));
    if(valueFormat1){
        info.append(QString("      valueRecord1\r\n"));
        valueRecord1.show(info,valueFormat1);
    }
    if(valueFormat2){
        info.append(QString("      valueRecord2\r\n"));
        valueRecord2.show(info,valueFormat2);
    }
}
void Class2Record::save(QDataStream &dt,uint16 valueFormat1,uint16 valueFormat2)
{
    if(valueFormat1){
        valueRecord1.save(dt,valueFormat1);
    }
    if(valueFormat2){
        valueRecord2.save(dt,valueFormat2);
    }
}

void ValueRecord::show(QString &info,uint16 valueFormat)
{
    info.append("        ");
    if(valueFormat&X_PLACEMENT){
        info.append(QString("xPlacement:%1\r\n").arg(xPlacement));
    }
    if(valueFormat&Y_PLACEMENT){
        info.append(QString("yPlacement:%1\r\n").arg(yPlacement));
    }
    if(valueFormat&X_ADVANCE){
        info.append(QString("xAdvance:%1\r\n").arg(xAdvance));
    }
    if(valueFormat&Y_ADVANCE){
        info.append(QString("yAdvance:%1\r\n").arg(yAdvance));
    }
    if(valueFormat&X_PLACEMENT_DEVICE){
        info.append(QString("xPlaDeviceOffset:%1\r\n").arg(xPlaDeviceOffset));
    }
    if(valueFormat&Y_PLACEMENT_DEVICE){
        info.append(QString("yPlaDeviceOffset:%1\r\n").arg(yPlaDeviceOffset));
    }
    if(valueFormat&X_ADVANCE_DEVICE){
        info.append(QString("xAdvDeviceOffset:%1\r\n").arg(xAdvDeviceOffset));
    }
    if(valueFormat&Y_ADVANCE_DEVICE){
        info.append(QString("yAdvDeviceOffset:%1\r\n").arg(yAdvDeviceOffset));
    }
}
void ValueRecord::read(QDataStream &dt, uint16 valueFormat)
{
    //X_PLACEMENT    	=0x0001,//Includes horizontal adjustment for placement
    //Y_PLACEMENT        =0x0002,//Includes vertical adjustment for placement
    //X_ADVANCE          =0x0004,//Includes horizontal adjustment for advance
    //Y_ADVANCE          =0x0008,//Includes vertical adjustment for advance
    //X_PLACEMENT_DEVICE	=0x0010,//Includes Device table (non-variable font) / VariationIndex table (variable font) for horizontal placement
    //Y_PLACEMENT_DEVICE	=0x0020,//Includes Device table (non-variable font) / VariationIndex table (variable font) for vertical placement
    //X_ADVANCE_DEVICE	=0x0040,//Includes Device table (non-variable font) / VariationIndex table (variable font) for horizontal advance
    //Y_ADVANCE_DEVICE	=0x0080,//Includes Device table (non-variable font) / VariationIndex table (variable font) for vertical advance
    //Reserved           =0xFF00,
    if(valueFormat&X_PLACEMENT){
        dt>>xPlacement;
    }
    if(valueFormat&Y_PLACEMENT){
        dt>>yPlacement;
    }
    if(valueFormat&X_ADVANCE){
        dt>>xAdvance;
    }
    if(valueFormat&Y_ADVANCE){
        dt>>yAdvance;
    }
    if(valueFormat&X_PLACEMENT_DEVICE){
        dt>>xPlaDeviceOffset;
    }
    if(valueFormat&Y_PLACEMENT_DEVICE){
        dt>>yPlaDeviceOffset;
    }
    if(valueFormat&X_ADVANCE_DEVICE){
        dt>>xAdvDeviceOffset;
    }
    if(valueFormat&Y_ADVANCE_DEVICE){
        dt>>yAdvDeviceOffset;
    }
}
void ValueRecord::save(QDataStream &dt,uint16 valueFormat)
{
    if(valueFormat&X_PLACEMENT){
        dt<<xPlacement;
    }
    if(valueFormat&Y_PLACEMENT){
        dt<<yPlacement;
    }
    if(valueFormat&X_ADVANCE){
        dt<<xAdvance;
    }
    if(valueFormat&Y_ADVANCE){
        dt<<yAdvance;
    }
    if(valueFormat&X_PLACEMENT_DEVICE){
        dt<<xPlaDeviceOffset;
    }
    if(valueFormat&Y_PLACEMENT_DEVICE){
        dt<<yPlaDeviceOffset;
    }
    if(valueFormat&X_ADVANCE_DEVICE){
        dt<<xAdvDeviceOffset;
    }
    if(valueFormat&Y_ADVANCE_DEVICE){
        dt<<yAdvDeviceOffset;
    }
}

void PairPosFormat1::read(QDataStream &dt)
{
    Q_UNUSED(dt);
}

void PairPosFormat1::show(QString &info)
{
    Q_UNUSED(info);
}

void PairPosFormat1::save(QDataStream &dt)
{
    Q_UNUSED(dt);
}

int PairPosFormat1::byteLength()
{
    return 0;
}

MarkBasePosFormat1::~MarkBasePosFormat1()
{
    if(markCover){
        delete markCover;
    }
    if(baseCover){
        delete baseCover;
    }
}

void MarkBasePosFormat1::read(QDataStream &dt)
{
    uint64 mbpPos=dt.device()->pos();
    mbpPos-=sizeof(posFormat);
    dt>>markCoverageOffset>>baseCoverageOffset
            >>markClassCount>>markArrayOffset>>baseArrayOffset;
    //先定位
    dt.device()->seek(mbpPos+markCoverageOffset);
    uint16 coverFmt;
    dt>>coverFmt;
    if(coverFmt==1){
        markCover=new CoverageFormat1;
    }else{
        markCover=new CoverageFormat2;
    }
    markCover->read(dt);
    //先定位
    dt.device()->seek(mbpPos+baseCoverageOffset);
    dt>>coverFmt;
    if(coverFmt==1){
        baseCover=new CoverageFormat1;
    }else{
        baseCover=new CoverageFormat2;
    }
    baseCover->read(dt);
    //先定位
    dt.device()->seek(mbpPos+markArrayOffset);
    mat.read(dt);
    //先定位
    dt.device()->seek(mbpPos+baseArrayOffset);
    bat.fzmarkClassCount=markClassCount;
    bat.read(dt);
}

void MarkBasePosFormat1::show(QString &info)
{
    info.append(QString("  [2B]posFormat:%1\r\n  [2B]markCoverageOffset:%2\r\n  [2B]baseCoverageOffset:%3\r\n  "
                        "[2B]markClassCount:%4\r\n  [2B]markArrayOffset:%5\r\n  [2B]baseArrayOffset:%6\r\n")
                .arg(posFormat).arg(markCoverageOffset).arg(baseCoverageOffset)
                .arg(markClassCount).arg(markArrayOffset).arg(baseArrayOffset));
    if(markCover){
        info.append("  [markCoverage]:\r\n");
        markCover->show(info);
    }
    if(baseCover){
        info.append("  [baseCoverage]:\r\n");
        baseCover->show(info);
    }
    info.append("  [MarkArrayTable]:\r\n");
    mat.show(info);
    info.append("  [BaseArrayTable]:\r\n");
    bat.show(info);
}

void MarkBasePosFormat1::save(QDataStream &dt)
{
    Q_UNUSED(dt);
}

int MarkBasePosFormat1::byteLength()
{
    return 0;
}

void MarkArrayTable::read(QDataStream &dt)
{
    dt>>markCount;
    pMarkRecords=new MarkRecord[markCount];
    for(int i=0;i<markCount;i++){
        dt>>pMarkRecords[i].markClass
                >>pMarkRecords[i].markAnchorOffset;
    }
}

void MarkArrayTable::show(QString &info)
{
    info.append(QString("  markCount:%1\r\n").arg(markCount));
    if(markCount>0){
        info.append(QString("  markClass markAnchorOffset:\r\n"));
    }
    for(int i=0;i<markCount;i++){
        info.append(QString("  %1: %2,%3").arg(i+1)
                    .arg(pMarkRecords[i].markClass)
                    .arg(pMarkRecords[i].markAnchorOffset));
        info.append("\r\n");
    }
}

void BaseArrayTable::read(QDataStream &dt)
{
    dt>>baseCount;
    pBaseRecord=new BaseRecord[baseCount];
    for(int bi=0;bi<baseCount;bi++){
        pBaseRecord[bi].baseAnchorOffsets.fill(0,fzmarkClassCount);
        for(int i=0;i<fzmarkClassCount;i++){
            dt>>pBaseRecord[bi].baseAnchorOffsets[i];
        }
    }
}

void BaseArrayTable::show(QString &info)
{
    info.append(QString("  baseCount:%1\r\n").arg(baseCount));
    for(int bi=0;bi<baseCount;bi++){
        info.append(QString("  %1: ").arg(bi+1));
        for(int i=0;i<fzmarkClassCount;i++){
            info.append(QString("%1 ")
                        .arg(pBaseRecord[bi].baseAnchorOffsets[i]));
        }
        info.append("\r\n");
    }
}

void ChainedContextsPositioningFormat1::read(QDataStream &dt)
{
    Q_UNUSED(dt);
}

void ChainedContextsPositioningFormat1::show(QString &info)
{
    info.append(QString("  [2B]format:%1\r\n")
                .arg(format));
}

void ChainedContextsPositioningFormat1::save(QDataStream &dt)
{
    Q_UNUSED(dt);
}

int ChainedContextsPositioningFormat1::byteLength()
{
    return 0;
}

void ChainedContextsPositioningFormat2::read(QDataStream &dt)
{
    Q_UNUSED(dt);
}

void ChainedContextsPositioningFormat2::show(QString &info)
{
    info.append(QString("  [2B]format:%1\r\n")
                .arg(format));
}

void ChainedContextsPositioningFormat2::save(QDataStream &dt)
{
    Q_UNUSED(dt);
}

int ChainedContextsPositioningFormat2::byteLength()
{
    return 0;
}

void ChainedContextsPositioningFormat3::read(QDataStream &dt)
{
    Q_UNUSED(dt);
}

void ChainedContextsPositioningFormat3::show(QString &info)
{
    ChainedSequenceContextFormat3::show(info);
}

void ChainedContextsPositioningFormat3::save(QDataStream &dt)
{
    Q_UNUSED(dt);
}

int ChainedContextsPositioningFormat3::byteLength()
{
    return 0;
}

void ExtensionPosFormat1::read(QDataStream &dt)
{
    dt>>extensionLookupType>>extensionOffset;
}

void ExtensionPosFormat1::show(QString &info)
{
    info.append(QString("  [2B]posFormat:%1\r\n  [2B]extensionLookupType:%2\r\n  [4B]extensionOffset:%3\r\n")
                .arg(posFormat).arg(extensionLookupType).arg(extensionOffset));
}

void ExtensionPosFormat1::save(QDataStream &dt)
{
    Q_UNUSED(dt);
}

int ExtensionPosFormat1::byteLength()
{
    return 0;
}

