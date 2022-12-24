#include "subtablecomm.h"

void ClassDefFormat1::show(QString&info){
    info.append(QString("ClassDefFormat1\r\n  classFormat:%1,"
                        "startGlyphID=%2,"
                        "glyphCount=%3\r\n").arg(classFormat).arg(startGlyphID).arg(glyphCount));
    if(glyphCount>0){
        info.append("  classValueArray:");
        if(classValueArray){
            for(int i=0;i<glyphCount;i++){
                info.append(QString("%1 ").arg(classValueArray[i]));
            }
        }
        info.append("\r\n");
    }
}
void ClassDefFormat1::read(QDataStream&dt,bool formatYidu){
    if(!formatYidu){
        dt>>classFormat;
    }
    dt>>startGlyphID>>glyphCount;
    if(glyphCount>0){
        classValueArray=new uint16[glyphCount];
        for(int i=0;i<glyphCount;i++){
            dt>>classValueArray[i];
        }
    }
}
void ClassDefFormat1::save(QDataStream&dt){
    dt<<classFormat<<startGlyphID<<glyphCount;
    if(classValueArray){
        for(int i=0;i<glyphCount;i++){
            dt<<classValueArray[i];
        }
    }
}


ClassDefFormat2::ClassDefFormat2():
    classFormat(2),
    classRangeCount(0),
    pClassRangeRecord(nullptr){}
ClassDefFormat2::~ClassDefFormat2(){
    if(pClassRangeRecord){
        delete [] pClassRangeRecord;
    }
}
void ClassDefFormat2::arr(){
    pClassRangeRecord=new ClassRangeRecord[classRangeCount];
}
void ClassDefFormat2::show(QString&info){
    info.append(QString("ClassDefFormat2\r\n  classFormat:%1,"
                        "classRangeCount=%2\r\n").arg(classFormat).arg(classRangeCount));
    if(classRangeCount>0){
        if(pClassRangeRecord){
            for(int i=0;i<classRangeCount;i++){
                info.append(QString("[%4]\r\n  startGlyphID:%1,"
                                    "endGlyphID=%2,"
                                    "classValue=%3\r\n")
                            .arg(pClassRangeRecord[i].startGlyphID)
                            .arg(pClassRangeRecord[i].endGlyphID)
                            .arg(pClassRangeRecord[i].classValue)
                            .arg(i));
            }
        }
    }
}
void ClassDefFormat2::read(QDataStream& dt, bool formatYidu){
    if(!formatYidu){
        dt>>classFormat;
    }
    dt>>classRangeCount;
    if(classRangeCount>0){
        arr();
        for(int i=0;i<classRangeCount;i++){
            dt>>pClassRangeRecord[i].startGlyphID
                    >>pClassRangeRecord[i].endGlyphID
                    >>pClassRangeRecord[i].classValue;
        }
    }
}
void ClassDefFormat2::save(QDataStream&dt){
    dt<<classFormat<<classRangeCount;
    if(pClassRangeRecord){
        for(int i=0;i<classRangeCount;i++){
            dt<<pClassRangeRecord[i].startGlyphID
             <<pClassRangeRecord[i].endGlyphID
            <<pClassRangeRecord[i].classValue;
        }
    }
}

void RangeRecord::show(int i,QString &info)
{
    info.append(QString("  %4:%1,%2,%3\r\n")
                .arg(startGlyphID).arg(endGlyphID).arg(startCoverageIndex).arg(i+1));
}
void RangeRecord::read(QDataStream &dt)
{
    dt>>startGlyphID>>endGlyphID>>startCoverageIndex;
}
void RangeRecord::save(QDataStream &dt)
{
    dt<<startGlyphID<<endGlyphID<<startCoverageIndex;
}

CoverageFormat::CoverageFormat(uint16 fmt):coverageFormat(fmt)
{

}

CoverageFormat::~CoverageFormat()
{

}

CoverageFormat1::CoverageFormat1():
    CoverageFormat(1),glyphCount(0)
{

}

CoverageFormat1::~CoverageFormat1()
{
}

void CoverageFormat1::show(QString &info)
{
    info.append(QString("  [2B]coverageFormat:%1\r\n").arg(coverageFormat));
    info.append(QString("  [2B]glyphCount:%1\r\n").arg(glyphCount));
    if(glyphCount>0){
        info.append(QString("  [%1B]glyphArray:").arg(glyphCount*sizeof(uint16)));
        for(int i=0;i<glyphCount;i++){
            info.append(QString("%1 ").arg(glyphArray[i]));
        }
        info.append("\r\n");
    }
}

void CoverageFormat1::getCoverGlyphIdx(u16vector &glyphIndexVec)
{
    glyphIndexVec=glyphArray;
}
uint16 CoverageFormat1::coverIndex2GlyphIndex(int coverIndex){
    return glyphArray[coverIndex];
}
int CoverageFormat1::indexOf(int16 gid){
    return glyphArray.indexOf(gid);
}

int CoverageFormat1::byteLength()
{
    return (2+glyphCount)*sizeof(uint16);
}

void CoverageFormat1::shanchu(int &coverIdx)
{
    glyphCount-=1;
    glyphArray.remove(coverIdx);
    coverIdx=-1;
}
void CoverageFormat1::quanbuTihuan(const u16vector& ids)
{
    glyphArray=ids;
    sort(glyphArray.begin(),glyphArray.end());
    glyphCount=glyphArray.length();
}
void CoverageFormat1::set(uint16 id)
{
    //coverage 增加一项--要排序
    glyphArray.append(id);
    sort(glyphArray.begin(),glyphArray.end());
    glyphCount=glyphArray.length();
}
void CoverageFormat1::read(QDataStream &dt)
{
    dt>>glyphCount;
    if(glyphCount>0){
        glyphArray.fill(0,glyphCount);
        for(int i=0;i<glyphCount;i++){
            dt>>glyphArray[i];
        }
    }
}

void CoverageFormat1::save(QDataStream &dt)
{
    dt<<coverageFormat<<glyphCount;
    for(int i=0;i<glyphCount;i++){
        dt<<glyphArray[i];
    }
}

CoverageFormat2::CoverageFormat2():
    CoverageFormat(2),rangeCount(0)
{

}

CoverageFormat2::~CoverageFormat2()
{
}

void CoverageFormat2::show(QString &info)
{
    info.append(QString("  [2B]coverageFormat:%1\r\n").arg(coverageFormat));
    info.append(QString("  [2B]rangeCount:%1\r\n").arg(rangeCount));
    if(rangeCount>0){
        info.append("  startGlyphID,endGlyphID,startCoverageIndex\r\n");
        for(int i=0;i<rangeCount;i++){
            lstRangeRecord[i].show(i,info);
        }
    }
}

void CoverageFormat2::getCoverGlyphIdx(u16vector &glyphIndexVec)
{
    for(int i=0;i<lstRangeRecord.length();i++){
        if(lstRangeRecord[i].startGlyphID==lstRangeRecord[i].endGlyphID){
            glyphIndexVec.append(lstRangeRecord[i].startGlyphID);
        }else{
            for(int gi=lstRangeRecord[i].startGlyphID;gi<=lstRangeRecord[i].endGlyphID;gi++){
                glyphIndexVec.append(gi);
            }
        }
    }
}

uint16 CoverageFormat2::coverIndex2GlyphIndex(int coverIndex)
{
    for(int i=lstRangeRecord.length()-1;i>=0;i--){
        if(coverIndex>=lstRangeRecord[i].startCoverageIndex){
            return lstRangeRecord[i].startGlyphID+coverIndex-lstRangeRecord[i].startCoverageIndex;
        }
    }
    return 0;
}

int CoverageFormat2::indexOf(int16 gid)
{
    for(int i=lstRangeRecord.length()-1;i>=0;i--){
        if(gid>=lstRangeRecord[i].startGlyphID){
            return gid-lstRangeRecord[i].startGlyphID+lstRangeRecord[i].startCoverageIndex;
        }
    }
    return -1;
}

int CoverageFormat2::byteLength()
{
    return (2+3*rangeCount)*sizeof (uint16);
}

void CoverageFormat2::shanchu(int &coverIdx)
{
    for(int i=0;i<lstRangeRecord.length();i++){
        if(coverIdx>=lstRangeRecord[i].startCoverageIndex&&
                coverIdx<=lstRangeRecord[i].startCoverageIndex
                +lstRangeRecord[i].endGlyphID-lstRangeRecord[i].startGlyphID){
            //uint16 delID=coverIdx-lstRangeRecord[i].startCoverageIndex;
            //RangeRecord rr;
            //rr.startGlyphID=id;
            //rr.endGlyphID=id;
            //rr.startCoverageIndex=0;
            break;
        }
    }
}

void CoverageFormat2::quanbuTihuan(const u16vector &ids)
{
    Q_UNUSED(ids);
}

void CoverageFormat2::set(uint16 id)
{
    //coverage 增加一项--要排序
    int insertPos=-1;
    for(int i=0;i<lstRangeRecord.length();i++){
        if(id<lstRangeRecord[i].startGlyphID){
            insertPos=i;
            break;
        }
    }
    RangeRecord rr;
    rr.startGlyphID=id;
    rr.endGlyphID=id;
    rr.startCoverageIndex=0;
    if(insertPos==-1){
        lstRangeRecord.append(rr);
    }else{
        lstRangeRecord.insert(insertPos,rr);
    }
    //压缩
    for(int i=0;i<lstRangeRecord.length()-1;i++){
        if(lstRangeRecord[i].endGlyphID+1==lstRangeRecord[i+1].startGlyphID){
            lstRangeRecord[i].endGlyphID=lstRangeRecord[i+1].endGlyphID;
            lstRangeRecord.removeAt(i+1);
            i--;
        }
    }
    //更新rangeCount startCoverageIndex
    rangeCount=lstRangeRecord.length();
    for(int i=0;i<lstRangeRecord.length();i++){
        if(i==0){
            lstRangeRecord[i].startCoverageIndex=0;
        }else{
            lstRangeRecord[i].startCoverageIndex=
                    lstRangeRecord[i-1].startCoverageIndex+
                    lstRangeRecord[i].endGlyphID-lstRangeRecord[i].startGlyphID+1;
        }
    }
}


void CoverageFormat2::read(QDataStream &dt)
{
    dt>>rangeCount;
    if(rangeCount>0){
        for(int i=0;i<rangeCount;i++){
            RangeRecord rr;
            rr.read(dt);
            lstRangeRecord.append(rr);
        }
    }
}

void CoverageFormat2::save(QDataStream &dt)
{
    dt<<coverageFormat<<rangeCount;
    for(int i=0;i<rangeCount;i++){
        lstRangeRecord[i].save(dt);
    }
}



void ChainedSequenceContextFormat3::read(QDataStream &dt)
{
    dt>>backtrackGlyphCount;
    if(backtrackGlyphCount>0){
        backtrackCoverageOffsets.fill(0,backtrackGlyphCount);
        for(int i=0;i<backtrackGlyphCount;i++){
            dt>>backtrackCoverageOffsets[i];
        }
    }
    dt>>inputGlyphCount;
    if(inputGlyphCount>0){
        inputCoverageOffsets.fill(0,inputGlyphCount);
        for(int i=0;i<inputGlyphCount;i++){
            dt>>inputCoverageOffsets[i];
        }
    }
    dt>>lookaheadGlyphCount;
    if(lookaheadGlyphCount>0){
        lookaheadCoverageOffsets.fill(0,lookaheadGlyphCount);
        for(int i=0;i<lookaheadGlyphCount;i++){
            dt>>lookaheadCoverageOffsets[i];
        }
    }
    dt>>seqLookupCount;
    for(int i=0;i<seqLookupCount;i++){
        SequenceLookupRecord x;
        x.read(dt);
        seqLookupRecords.append(x);
    }
}

void ChainedSequenceContextFormat3::show(QString &info)
{
    info.append(QString("  [2B]format:%1\r\n"
                        "  [2B]backtrackGlyphCount:%2\r\n"
                        "  [%3B]backtrackCoverageOffsets:")
                .arg(format).arg(backtrackGlyphCount).arg(2*backtrackGlyphCount));
    for(int i=0;i<backtrackGlyphCount;i++){
        info.append(QString("%1 ").arg(backtrackCoverageOffsets[i]));
    }
    info.append("\r\n");
    info.append(QString("  [2B]inputGlyphCount:%2\r\n"
                        "  [%3B]inputCoverageOffsets:")
                .arg(inputGlyphCount).arg(2*inputGlyphCount));
    for(int i=0;i<inputGlyphCount;i++){
        info.append(QString("%1 ").arg(inputCoverageOffsets[i]));
    }
    info.append("\r\n");
}

void SequenceLookupRecord::read(QDataStream &dt)
{
    dt>>sequenceIndex>>this->lookupListIndex;
}

void SequenceLookupRecord::show(QString &info)
{
    info.append(QString("  sequenceIndex:%1\r\n  lookupListIndex:%2\r\n")
                .arg(sequenceIndex).arg(lookupListIndex));
}
