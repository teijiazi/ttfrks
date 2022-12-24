#include "gsubsubtable.h"

SingleSubstFormat1::~SingleSubstFormat1()
{
    if(pCover){
        delete pCover;
        pCover=nullptr;
    }
}

void SingleSubstFormat1::read(QDataStream &dt)
{
    uint64 posSub=dt.device()->pos()-sizeof(substFormat);//format已经读取
    dt>>coverageOffset>>deltaGlyphID;
    dt.device()->seek(posSub+coverageOffset);
    uint16	coverageFormat;
    dt>>coverageFormat;
    if(coverageFormat==1){
        pCover=new CoverageFormat1;
    }else if(coverageFormat==2){
        pCover=new CoverageFormat2;
    }
    if(pCover){
        pCover->read(dt);
    }
}

void SingleSubstFormat1::show(QString &info)
{
    info.append(QString("  substFormat:%1\r\n  coverageOffset:%2\r\n").arg(substFormat).arg(coverageOffset));
    info.append(QString("  deltaGlyphID:%1\r\n").arg(deltaGlyphID));
}

void SingleSubstFormat1::save(QDataStream &dt)
{
    dt<<substFormat<<coverageOffset<<deltaGlyphID;
    if(pCover){
        pCover->save(dt);
    }
}

int SingleSubstFormat1::byteLength()
{
    int len=3*sizeof(uint16);
    if(pCover){
        len+=pCover->byteLength();
    }
    return len;
}

SingleSubstFormat2::~SingleSubstFormat2(){
    if(pCover){
        delete pCover;
        pCover=nullptr;
    }
}

void SingleSubstFormat2::read(QDataStream &dt)
{
    uint64 posSub=dt.device()->pos()-sizeof(substFormat);//format已经读取
    dt>>coverageOffset>>glyphCount;
    substituteGlyphIDs.fill(0,glyphCount);
    for(int i=0;i<glyphCount;i++){
        dt>>substituteGlyphIDs[i];
    }
    dt.device()->seek(posSub+coverageOffset);
    uint16	coverageFormat;
    dt>>coverageFormat;
    if(coverageFormat==1){
        pCover=new CoverageFormat1;
    }else if(coverageFormat==2){
        pCover=new CoverageFormat2;
    }
    if(pCover){
        pCover->read(dt);
    }
}

void SingleSubstFormat2::show(QString &info)
{
    info.append(QString("  [2B]substFormat:%1\r\n  [2B]coverageOffset:%2\r\n").arg(substFormat).arg(coverageOffset));
    info.append(QString("  [2B]glyphCount:%1\r\n  [%2B]").arg(glyphCount).arg(glyphCount*sizeof(uint16)));
    for(int i=0;i<glyphCount;i++){
        info.append(QString("%1 ").arg(substituteGlyphIDs[i]));
    }
    info.append("\r\n");
    if(pCover){
        pCover->show(info);
    }
}

void SingleSubstFormat2::save(QDataStream &dt)
{
    dt<<substFormat<<coverageOffset<<glyphCount;
    for(int i=0;i<glyphCount;i++){
        dt<<substituteGlyphIDs[i];
    }
    if(pCover){
        pCover->save(dt);
    }
}

int SingleSubstFormat2::byteLength()
{
    int len=sizeof(uint16)*(3+glyphCount);
    if(pCover){
        len+=pCover->byteLength();
    }
    return len;
}

void SingleSubstFormat2::setVert(const u16vector &heng, const u16vector &shu)
{
    set(heng,shu);
}

void SingleSubstFormat2::set(const u16vector &coverGIDs, const u16vector &tihuanGIDs)
{
    glyphCount=tihuanGIDs.length();
    substituteGlyphIDs=tihuanGIDs;
    if(!pCover){
        pCover=new CoverageFormat1;
    }
    pCover->quanbuTihuan(coverGIDs);
    xiufuPianyi();
}

void SingleSubstFormat2::xiufuPianyi()
{
    int offset=sizeof(uint16)*(3+glyphCount);
    coverageOffset=offset;
}

bool LigatureSetTable::kaoqian(const LigatureTable&a,const LigatureTable&b){
    //长度较长的靠前。长度相同，glyphIndex大的靠前
    if(a.componentGlyphIDs.length()==b.componentGlyphIDs.length()){
        for(int i=0;i<a.componentGlyphIDs.length();i++){
            if(a.componentGlyphIDs[i]==b.componentGlyphIDs[i]){
                continue;
            }else{
                return a.componentGlyphIDs[i]>b.componentGlyphIDs[i];
            }
        }
    }else{
        return a.componentGlyphIDs.length()>b.componentGlyphIDs.length();
    }
    return true;
}

int LigatureSetTable::set(const u16vector& shuru){
    //LigatureSetTable里面新增一个LigatureTable,返回插入的位置
    //排序 重写偏移
    //fft
    //ffl
    //ffk
    //ffb
    //fl
    //fi
    //ff
    ligatureCount+=1;
    LigatureTable lt;
    lt.ligatureGlyph=0;
    lt.componentCount=shuru.length();
    lt.componentGlyphIDs=u16vector(shuru.begin()+1,shuru.end());
    int crPos=-1;
    for(int i=0;i<lstLigatureTable.length();i++){
        if(kaoqian(lt,lstLigatureTable[i])){
            crPos=i;
            break;
        }
    }
    if(crPos==-1){
        //补在最后
        lstLigatureTable.append(lt);
        crPos=lstLigatureTable.length()-1;
    }else{
        lstLigatureTable.insert(crPos,lt);
    }
    xiufuPianyi();
    return crPos;
}

int LigatureSetTable::byteLength()
{
    int len=sizeof (uint16)*(1+ligatureCount);
    for(int i=0;i<ligatureCount;i++){
        len+=lstLigatureTable[i].byteLength();
    }
    return len;
}

void LigatureSetTable::xiufuPianyi(){
    int off=sizeof (uint16)*(1+ligatureCount);
    ligatureOffsets.clear();
    for(int i=0;i<lstLigatureTable.length();i++){
        ligatureOffsets.append(off);
        off+=lstLigatureTable[i].byteLength();
    }
}

void LigatureSetTable::shanchu(int &shuruIdx){
    ligatureCount--;
    lstLigatureTable.removeAt(shuruIdx);
    xiufuPianyi();
    shuruIdx=-1;
}

void LigatureSetTable::read(QDataStream &dt)
{
    uint64 posL=dt.device()->pos();
    dt>>ligatureCount;
    ligatureOffsets.fill(0,ligatureCount);
    for(int gi=0;gi<ligatureCount;gi++){
        dt>>ligatureOffsets[gi];
    }
    for(int gi=0;gi<ligatureCount;gi++){
        dt.device()->seek(posL+ligatureOffsets[gi]);
        LigatureTable lt;
        dt>>lt.ligatureGlyph>>lt.componentCount;
        lt.componentGlyphIDs.fill(0,lt.componentCount-1);
        for(int i=0;i<lt.componentCount-1;i++){
            dt>>lt.componentGlyphIDs[i];
        }
        lstLigatureTable.append(lt);
    }
}

void LigatureSetTable::show(QString &info)
{
    info.append(QString("  [2B]ligatureCount:%1\r\n").arg(ligatureCount));
    info.append(QString("  [%1B]ligatureOffsets:").arg(ligatureCount*sizeof(Offset16)));
    for(int i=0;i<ligatureCount;i++){
        info.append(QString("%1 ").arg(ligatureOffsets[i]));
    }
    info.append("\r\n");
    info.append("  LigatureTable:\r\n");
    for(int gi=0;gi<ligatureCount;gi++){
        info.append(QString("    lt %1:\r\n    [2B]输出字符ligatureGlyph:%2\r\n    [2B]componentCount:%3\r\n")
                    .arg(gi+1).arg(lstLigatureTable[gi].ligatureGlyph).arg(lstLigatureTable[gi].componentCount));
        info.append(QString("    [%1B]componentGlyphIDs:")
                    .arg((lstLigatureTable[gi].componentCount-1)*sizeof(uint16)));
        for(int i=0;i<lstLigatureTable[gi].componentCount-1;i++){
            info.append(QString("%1 ").arg(lstLigatureTable[gi].componentGlyphIDs[i]));
        }
        info.append("\r\n");
    }
}

void LigatureSetTable::save(QDataStream &dt)
{
    dt<<ligatureCount;
    for(int gi=0;gi<ligatureCount;gi++){
        dt<<ligatureOffsets[gi];
    }
    for(int gi=0;gi<ligatureCount;gi++){
        lstLigatureTable[gi].save(dt);
    }
}


LigatureSubstFormat1::LigatureSubstFormat1(QObject *parent):
    Subtable(parent),substFormat(1),ligatureSetCount(0),pCover(nullptr)
{
}

LigatureSubstFormat1::~LigatureSubstFormat1()
{
    if(pCover){
        delete pCover;
        pCover=nullptr;
    }
}

void LigatureSubstFormat1::set(const u16vector&shuru,int& jgCoverIdx,int& jgShuruIdx){
    //新增一个替换。
    //1 已有Cover，增加ligaTable
    //2 已有cover,扩充cover，扩充ligaSetTable,再增加LigaTable
    //3 什么都没有 新建Cover新建ligaSetTable
    /// substFormat
    /// coverageOffset
    /// ligatureSetCount
    /// ligatureSetOffsets
    /// ligatureSet
    ///   ligatrue
    ///   ......
    ///   ligatrue
    /// ligatureSet
    /// ......
    /// ligatureSet
    /// coverage
    int coverIndex=-1;
    if(pCover && (coverIndex=pCover->indexOf(shuru[0]))!=-1){
        //1 首字匹配，补充ligatureTable
        jgShuruIdx=lstLigSet[coverIndex].set(shuru);
    }else{
        //2,3
        ligatureSetCount+=1;
        if(!pCover){
            pCover=new CoverageFormat1;
        }
        pCover->set(shuru[0]);
        coverIndex=pCover->indexOf(shuru[0]);
        LigatureSetTable ls;
        jgShuruIdx=ls.set(shuru);
        lstLigSet.insert(coverIndex,ls);
    }
    //修改ligatureSetOffset--按照coverIndex的顺序插入以后，重新计算偏移
    xiufuPianyi();
    //
    jgCoverIdx=coverIndex;
}

int LigatureSubstFormat1::byteLength()
{
    int len=sizeof(uint16)*(3+ligatureSetCount);
    if(pCover){
        len+=pCover->byteLength();
    }
    for(int i=0;i<this->ligatureSetCount;i++){
        len+=lstLigSet[i].byteLength();
    }
    return len;
}

void LigatureSubstFormat1::xiufuPianyi(){
    ligatureSetOffsets.clear();
    int offset=sizeof(uint16)*(3+ligatureSetCount);
    for(int i=0;i<ligatureSetCount;i++){
        ligatureSetOffsets.append(offset);
        offset+=lstLigSet[i].byteLength();
    }
    coverageOffset=offset;
}

void LigatureSubstFormat1::shanchu(int &coverIdx, int &shuruIdx){
    lstLigSet[coverIdx].shanchu(shuruIdx);
    if(lstLigSet[coverIdx].ligatureCount==0){
        lstLigSet.removeAt(coverIdx);
        pCover->shanchu(coverIdx);
        ligatureSetCount-=1;
    }
    xiufuPianyi();
}

void LigatureSubstFormat1::read(QDataStream &dt)
{
    uint64 posSub=dt.device()->pos()-sizeof(substFormat);//format已经读取
    dt>>coverageOffset>>ligatureSetCount;//	Number of LigatureSet tables
    //Offset16	ligatureSetOffsets[ligatureSetCount]	Array of offsets to LigatureSet tables. Offsets are from beginning of substitution subtable, ordered by Coverage index
    ligatureSetOffsets.fill(0,ligatureSetCount);
    for(int i=0;i<ligatureSetCount;i++){
        dt>>ligatureSetOffsets[i];
    }
    if(ligatureSetCount>0){
        for(int i=0;i<ligatureSetCount;i++){
            LigatureSetTable ls;
            dt.device()->seek(posSub+ligatureSetOffsets[i]);
            ls.read(dt);
            lstLigSet.append(ls);
        }
    }
    dt.device()->seek(posSub+coverageOffset);
    uint16	coverageFormat;
    dt>>coverageFormat;
    if(coverageFormat==1){
        pCover=new CoverageFormat1;
    }else if(coverageFormat==2){
        pCover=new CoverageFormat2;
    }
    if(pCover){
        pCover->read(dt);
    }
}

void LigatureSubstFormat1::show(QString &info)
{
    info.append(QString("  [2B]substFormat:%1\r\n  [2B]coverageOffset:%2\r\n  [2B]ligatureSetCount:%3\r\n")
                .arg(substFormat).arg(coverageOffset).arg(ligatureSetCount));
    info.append(QString("  [%1B]ligatureSetOffsets:").arg(ligatureSetCount*sizeof(uint16)));
    for(int i=0;i<ligatureSetCount;i++){
        info.append(QString("%1 ").arg(ligatureSetOffsets[i]));
    }
    info.append("\r\n");
    info.append("LigatureSetTable\r\n");
    for(int i=0;i<ligatureSetCount;i++){
        info.append(QString("  LS %1:\r\n").arg(i+1));
        lstLigSet[i].show(info);
    }
    if(pCover){
        info.append("CoverageTable\r\n");
        pCover->show(info);
    }
}

void LigatureSubstFormat1::save(QDataStream &dt)
{
    dt<<substFormat<<coverageOffset<<ligatureSetCount;
    for(int i=0;i<ligatureSetCount;i++){
        dt<<ligatureSetOffsets[i];
    }
    if(ligatureSetCount>0){
        for(int i=0;i<ligatureSetCount;i++){
            lstLigSet[i].save(dt);
        }
    }
    if(pCover){
        pCover->save(dt);
    }
}

AlternateSubstFormat1::~AlternateSubstFormat1()
{
    if(pCover){
        delete pCover;
        pCover=nullptr;
    }
    if(pAlt){
        delete[] pAlt;
    }
}

void AlternateSubstFormat1::read(QDataStream &dt)
{
    uint64 posSub=dt.device()->pos()-sizeof(substFormat);//format已经读取
    dt>>coverageOffset>>alternateSetCount;//	Number of LigatureSet tables
    alternateSetOffsets.fill(0,alternateSetCount);
    if(alternateSetCount>0){
        pAlt=new AlternateSetTable[alternateSetCount];
        for(int i=0;i<alternateSetCount;i++){
            dt>>alternateSetOffsets[i];
        }
        for(int i=0;i<alternateSetCount;i++){
            dt.device()->seek(posSub+alternateSetOffsets[i]);
            dt>>pAlt[i].glyphCount;
            pAlt[i].alternateGlyphIDs.fill(0,pAlt[i].glyphCount);
            for(int gi=0;gi<pAlt[i].glyphCount;gi++){
                dt>>pAlt[i].alternateGlyphIDs[gi];
            }
        }
    }
    dt.device()->seek(posSub+coverageOffset);
    uint16	coverageFormat;
    dt>>coverageFormat;
    if(coverageFormat==1){
        pCover=new CoverageFormat1;
    }else if(coverageFormat==2){
        pCover=new CoverageFormat2;
    }
    if(pCover){
        pCover->read(dt);
    }
}

void AlternateSubstFormat1::show(QString &info)
{
    info.append(QString("  [2B]substFormat:%1\r\n  [2B]coverageOffset:%2\r\n  [2B]alternateSetCount:%3\r\n")
                .arg(substFormat).arg(coverageOffset).arg(alternateSetCount));
    info.append(QString("  [%1B]alternateSetOffsets:").arg(alternateSetCount*sizeof(Offset16)));
    for(int i=0;i<alternateSetCount;i++){
        info.append(QString("%1 ").arg(alternateSetOffsets[i]));
    }
    info.append("\r\n");
    if(pCover){
        pCover->show(info);
    }
    if(pAlt){
        info.append("AlternateSetTable\r\n");
        for(int i=0;i<alternateSetCount;i++){
            info.append(QString("  %1:\r\n  [2B]glyphCount:%2\r\n")
                        .arg(i+1).arg(pAlt[i].glyphCount));
            info.append(QString("  [%1B]alternateGlyphIDs:").arg(pAlt[i].glyphCount*sizeof(Offset16)));
            for(int gi=0;gi<pAlt[i].glyphCount;gi++){
                info.append(QString("%1 ").arg(pAlt[i].alternateGlyphIDs[gi]));;
            }
            info.append("\r\n");
        }
    }
}

void AlternateSubstFormat1::save(QDataStream &dt)
{
    Q_UNUSED(dt);
}

int AlternateSubstFormat1::byteLength()
{
    return 0;
}

void ExtensionSubstFormat1::read(QDataStream &dt)
{
    dt>>extensionLookupType>>extensionOffset;
}

void ExtensionSubstFormat1::show(QString &info)
{
    info.append(QString("  [2B]substFormat:%1\r\n  [2B]extensionLookupType:%2\r\n  [4B]extensionOffset:%3\r\n")
                .arg(substFormat).arg(extensionLookupType).arg(extensionOffset));
}

void ExtensionSubstFormat1::save(QDataStream &dt)
{
    Q_UNUSED(dt);
}

int ExtensionSubstFormat1::byteLength()
{
    return 0;
}

MultipleSubstFormat1::~MultipleSubstFormat1()
{
    if(pCover){
        delete pCover;
    }
    if(pSeq){
        delete []pSeq;
    }
}

void MultipleSubstFormat1::read(QDataStream &dt)
{
    uint64 posSub=dt.device()->pos()-sizeof(substFormat);//format已经读取
    dt>>coverageOffset>>sequenceCount;
    sequenceOffsets.fill(0,sequenceCount);
    for(int i=0;i<sequenceCount;i++){
        dt>>sequenceOffsets[i];
    }
    if(sequenceCount>0){
        pSeq=new SequenceTable[sequenceCount];
    }
    for(int i=0;i<sequenceCount;i++){
        dt.device()->seek(posSub+sequenceOffsets[i]);
        pSeq[i].read(dt);
    }
    dt.device()->seek(posSub+coverageOffset);
    uint16	coverageFormat;
    dt>>coverageFormat;
    if(coverageFormat==1){
        pCover=new CoverageFormat1;
    }else if(coverageFormat==2){
        pCover=new CoverageFormat2;
    }
    if(pCover){
        pCover->read(dt);
    }
}

void MultipleSubstFormat1::show(QString &info)
{
    info.append(QString("  [2B]substFormat:%1\r\n  [2B]coverageOffset:%2\r\n  [2B]sequenceCount:%3\r\n")
                .arg(substFormat).arg(coverageOffset).arg(sequenceCount));
    info.append(QString("  [%1B]sequenceOffsets:").arg(sequenceCount*sizeof(uint16)));
    for(int i=0;i<sequenceCount;i++){
        info.append(QString("%1 ").arg(sequenceOffsets[i]));
    }
    info.append("\r\n");
    if(pCover){
        info.append("输入字形索引(Coverage):\r\n");
        pCover->show(info);
    }
    if(pSeq){
        info.append("输出字形索引(SequenceTable)\r\n");
        for(int i=0;i<sequenceCount;i++){
            info.append(QString("  %1:\r\n").arg(i));
            pSeq[i].show(info);
        }
    }
}

void MultipleSubstFormat1::save(QDataStream &dt)
{
    Q_UNUSED(dt);
}

int MultipleSubstFormat1::byteLength()
{
    return 0;
}

void SequenceTable::read(QDataStream &dt)
{
    dt>>glyphCount;
    this->substituteGlyphIDs.fill(0,glyphCount);
    for(int i=0;i<glyphCount;i++){
        dt>>substituteGlyphIDs[i];
    }
}

void SequenceTable::show(QString &info)
{
    info.append(QString("  [2B]glyphCount:%1\r\n").arg(glyphCount));
    info.append(QString("  [%1B]substituteGlyphIDs:").arg(glyphCount*sizeof(uint16)));
    for(int i=0;i<glyphCount;i++){
        info.append(QString("%1 ").arg(substituteGlyphIDs[i]));
    }
    info.append("\r\n");
}

void SequenceTable::save(QDataStream &dt)
{
    Q_UNUSED(dt);
}

int LigatureTable::byteLength()
{
    return sizeof(uint16)*(this->componentCount+1);
}

void LigatureTable::save(QDataStream &dt)
{
    dt<<ligatureGlyph<<componentCount;
    for(int i=0;i<componentGlyphIDs.length();i++){
        dt<<componentGlyphIDs[i];
    }
}
