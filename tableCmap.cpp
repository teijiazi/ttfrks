#include<tableCmap.h>
#include<QRegularExpression>
#include "cmapzifu3500.h"
#include "cmapzifu6500.h"
formatCmap::formatCmap(uint16 vfmt):format(vfmt)
{

}

formatCmap::~formatCmap()
{

}

format0::format0():formatCmap(0){}
void format0::read(QDataStream&dt){
    dt>>length>>language;
    for(uint i=0;i<256;i++){
        dt>>glyphIdArray[i];
    }
}
void format0::show(CMAPTable* pcmap){
    pcmap->datashow(QString("\r\nformat0\r\nlength:%1\r\n"
                            "language:%2\r\n")
                    .arg(length)
                    .arg(language));
    QString info;
    for(uint i=0;i<256;i++){
        info.append(QString("%1 ").arg(glyphIdArray[i]));
    }
    pcmap->datashow(info);
}
void format0::save(QDataStream&dt){
    dt<<format<<length<<language;
    for(uint i=0;i<256;i++){
        dt<<glyphIdArray[i];
    }
}
void format0::jisuan(CMAPTable* pcmap){
    Q_UNUSED(pcmap);
}

void format0::soujizifu(CMAPTable *pcmap)
{
    Q_UNUSED(pcmap);
}


format4::format4():formatCmap(4),reservedPad(0),fzNumGlyphId(0){}
void format4::arr(){
    vendCode.clear();
    vstartCode.clear();
    vidDelta.clear();
    vidRangeOffsets.clear();
    vglyphIdArray.clear();
}
void format4::show(CMAPTable* pcmap){
    pcmap->datashow(QString("\r\nformat4\r\nlength:%1\r\n"
                            "language:%2\r\n"
                            "segCountX2:%3\r\n"
                            "searchRange:%4\r\n"
                            "entrySelector:%5\r\n"
                            "rangeShift:%6\r\n"
                            )
                    .arg(length).arg(language).arg(segCountX2).arg(searchRange).arg(entrySelector)
                    .arg(rangeShift));
    QString info;
    pcmap->datashow(QString("\r\nendCode\r\n"));
    for(uint i=0;i<fzSegCount;i++){
        info.append(QString("%1 ").arg(vendCode[i],4,16,QChar('0')));
    }
    pcmap->datashow(info);
    info.clear();
    pcmap->datashow(QString("\r\nstartCode\r\n"));
    for(uint i=0;i<fzSegCount;i++){
        info.append(QString("%1 ").arg(vstartCode[i],4,16,QChar('0')));
    }
    pcmap->datashow(info);
    info.clear();
    pcmap->datashow(QString("\r\nidDelta\r\n"));
    for(uint i=0;i<fzSegCount;i++){
        info.append(QString("%1 ").arg(vidDelta[i]));
    }
    pcmap->datashow(info);
    info.clear();
    pcmap->datashow(QString("\r\nidRangeOffsets\r\n"));
    for(uint i=0;i<fzSegCount;i++){
        info.append(QString("%1 ").arg(vidRangeOffsets[i]));
    }
    pcmap->datashow(info);
    info.clear();
    pcmap->datashow(QString("\r\nglyphIdArray\r\n"));
    for(uint i=0;i<fzNumGlyphId;i++){
        info.append(QString("%1 ").arg(vglyphIdArray[i]));
    }
    pcmap->datashow(info);
}
void format4::read(QDataStream&dt){
    dt>>length>>language>>segCountX2>>searchRange>>entrySelector
            >>rangeShift;
    fzSegCount=segCountX2/2;
    arr();
    for(uint i=0;i<fzSegCount;i++){
        uint16 x=0;
        dt>>x;
        vendCode.append(x);
    }
    dt>>reservedPad;
    for(uint i=0;i<fzSegCount;i++){
        uint16 x=0;
        dt>>x;
        vstartCode.append(x);
    }
    for(uint i=0;i<fzSegCount;i++){
        int16 x=0;
        dt>>x;
        vidDelta.append(x);
    }
    for(uint i=0;i<fzSegCount;i++){
        uint16 x=0;
        dt>>x;
        vidRangeOffsets.append(x);
    }
    fzNumGlyphId=(length-(8*sizeof(uint16)+fzSegCount*4*sizeof(uint16)))/sizeof(uint16);
    for(uint i=0;i<fzNumGlyphId;i++){
        uint16 x=0;
        dt>>x;
        vglyphIdArray.append(x);
    }
}
void format4::setZifuList(CMAPTable *pcmap, uint16 zifuBianma, uint16 glyphIdx, int from){
    if(glyphIdx==UINT16_MAX){
        snd<<"from="<<from<<",dlzf=0x"<<QString::number(zifuBianma,16)<<","<<glyphIdx;
        return;
    }
    if(pcmap->zifuOrderByGlyphId[glyphIdx]==0){                   //zifuOrderByGlyphId中glyphIndex的位置还没有填写字符
        pcmap->zifuOrderByGlyphId[glyphIdx]=zifuBianma;                    //填入此字符
    }else{                                                        //glyphIndex处已经填写了字符，说明是一个共用图形的字符
        uint16 zhubianma=pcmap->zifuOrderByGlyphId[glyphIdx];
        auto fnd=pcmap->shareZifu.find(zhubianma);                //在共用图形库shareZifu中查找这个key=glyphIndex
        if(fnd!=pcmap->shareZifu.end()){                          //找到了
            fnd.value().append(zifuBianma);                                //在共用库的这个位置补充这个字符c
        }else{                                                    //没有找到
            u32vector sz;                                         //建立一个共用节点
            sz.append(zifuBianma);                                         //填写共用的字符c
            pcmap->shareZifu.insert(zhubianma,sz);                //把此节点插入共用库中
        }
    }
}
void format4::soujizifu(CMAPTable *pcmap){
    //获取字符数组，按照glyph id的顺序排列===>zifuXulieGlyphId
    for(uint i=0;i+1<fzSegCount;i++){//不搜集0xffff
        for(uint16 c=vstartCode[i];c<=vendCode[i];c++){
            uint16 glyphIdx=0;
            if(vidRangeOffsets[i]==0){
                glyphIdx=(c+(vidDelta[i]+65536)%65536);                                     //根据ttf文件记录计算出glyphIndex
                setZifuList(pcmap,c,glyphIdx,1);
            }else{
                int xi=c-vstartCode[i]+vidRangeOffsets[i]/2-(fzSegCount-i);
                int xxi=vglyphIdArray[xi];
                if(xxi!=0){
                    glyphIdx=(xxi+vidDelta[i]);
                    setZifuList(pcmap,c,glyphIdx,2);
                }else{
                    snd<<"xx="<<c;
                }
            }
            if(vendCode[i]==vstartCode[i]){
                break;
            }
        }
    }
}

void format4::jisuan(CMAPTable* pcmap){
    //分段
    arr();
    int16 firstOffset=-1;//第一个不使用idDelta的分段。
    //
    u16vector jianceIndex;
    jianceIndex.clear();
    //0用来做notdef
    //先开一段
    vstartCode.append(pcmap->suoyinPaixu[pcmap->sypxKaishiIdx].bianma);
    vendCode.append(pcmap->suoyinPaixu[pcmap->sypxKaishiIdx].bianma);
    jianceIndex.clear();
    jianceIndex.append(pcmap->suoyinPaixu[pcmap->sypxKaishiIdx].idx);
    for(int i=pcmap->sypxKaishiIdx+1;i<pcmap->suoyinPaixu.length();i++){
        //分段
        if(pcmap->suoyinPaixu[i].bianma==pcmap->suoyinPaixu[i-1].bianma+1&&
                pcmap->suoyinPaixu[i].bianma<0xffff){
            //查找一个连续的区域
            vendCode.last()=pcmap->suoyinPaixu[i].bianma;
            jianceIndex.append(pcmap->suoyinPaixu[i].idx);
        }else{
            //填写delta offset
            if(jianceIndex.length()>0){
                bool useDelta=true;
                for(int ji=0;ji<jianceIndex.length()-1;ji++){
                    //检测收集到的索引是不是也连续
                    if(jianceIndex[ji]+1!=jianceIndex[ji+1]){
                        useDelta=false;
                        break;
                    }
                }
                if(useDelta){
                    //索引也连续则使用delta
                    vidDelta.append(jianceIndex[0]-vstartCode.last());
                    vidRangeOffsets.append(0);
                }else{
                    //索引不连续则使用offset，不用delta
                    if(firstOffset==-1){
                        firstOffset=vidDelta.length();
                    }
                    vidDelta.append(0);
                    vidRangeOffsets.append(vglyphIdArray.length());//offset=2*(offset剩余的长度+vglyphIdArray现有的长度)
                    vglyphIdArray.append(jianceIndex);
                }
            }
            //新开一段
            vstartCode.append(pcmap->suoyinPaixu[i].bianma);
            vendCode.append(pcmap->suoyinPaixu[i].bianma);
            if(pcmap->suoyinPaixu[i].bianma>=0xffff){
                if(pcmap->suoyinPaixu[i].bianma>0xffff){
                    vstartCode.last()=0xffff;
                    vendCode.last()=0xffff;
                }
                vidRangeOffsets.append(0);
                vidDelta.append(1);
                break;
            }else{
                jianceIndex.clear();
                jianceIndex.append(pcmap->suoyinPaixu[i].idx);//收集此连续编码对应的索引glyphIndex
            }
        }
    }
    //
    fzSegCount=vstartCode.length();
    for(int i=0;i<vidRangeOffsets.length();i++){
        if(i==firstOffset||vidRangeOffsets[i]!=0){
            //更新offset
            vidRangeOffsets[i]=2*(fzSegCount-i+vidRangeOffsets[i]);
        }
    }
    segCountX2=fzSegCount*2;
    entrySelector=floor(log2(fzSegCount));
    searchRange=pow(2,entrySelector)*2;
    rangeShift=segCountX2-searchRange;
    length=sizeof(uint16)*(8+fzSegCount*4+vglyphIdArray.length());
    language=0;//platformid=1才需要设置语言，其他platid,语音都是0，
}

void format4::save(QDataStream&dt){
    dt<<format<<length<<language<<segCountX2<<searchRange<<entrySelector
     <<rangeShift;
    for(uint i=0;i<fzSegCount;i++){
        dt<<vendCode[i];
    }
    dt<<reservedPad;
    for(uint i=0;i<fzSegCount;i++){
        dt<<vstartCode[i];
    }
    for(uint i=0;i<fzSegCount;i++){
        dt<<vidDelta[i];
    }
    for(uint i=0;i<fzSegCount;i++){
        dt<<vidRangeOffsets[i];
    }
    for(int i=0;i<vglyphIdArray.length();i++){
        dt<<vglyphIdArray[i];
    }
}

format12::format12():formatCmap(12),reserved(0){}
format12::~format12(){
}
void format12::show(CMAPTable* pcmap){
    pcmap->datashow(QString("\r\nformat12\r\nreserved:%1\r\n"
                            "length:%2\r\n"
                            "language:%3\r\n"
                            "numGroups:%4\r\n"
                            )
                    .arg(reserved).arg(length).arg(language).arg(numGroups));
    QString info;
    for(uint i=0;i<numGroups;i++){
        info.append(QString("%4\r\n"
                            "startCharCode:%1\r\n"
                            "endCharCode:%2\r\n"
                            "startGlyphID:%3\r\n"
                            )
                    .arg(groups[i].startCharCode,4,16,QChar('0'))
                    .arg(groups[i].endCharCode,4,16,QChar('0')).arg(groups[i].startGlyphID).arg(i));
    }
    pcmap->datashow(info);
}
void format12::read(QDataStream&dt){
    dt>>reserved>>length>>language>>numGroups;

    for(uint i=0;i<numGroups;i++){
        SequentialMapGroup smg;
        dt>>smg.startCharCode
                >>smg.endCharCode
                >>smg.startGlyphID;
        groups.append(smg);
    }
}
void format12::save(QDataStream&dt){
    dt<<format<<reserved<<length<<language<<numGroups;
    for(uint i=0;i<numGroups;i++){
        dt<<groups[i].startCharCode
         <<groups[i].endCharCode
        <<groups[i].startGlyphID;
    }
}
void format12::jisuan(CMAPTable* pcmap){
    this->groups.clear();
    u16vector jianceIndex;
    jianceIndex.clear();
    //0用来做notdef，从1开始检查，
    int sylen=pcmap->suoyinPaixu.length();
    if(pcmap->suoyinPaixu.last().bianma==0xffff){
        sylen-=1;
    }
    //
    SequentialMapGroup smg;
    smg.startCharCode=pcmap->suoyinPaixu[pcmap->sypxKaishiIdx].bianma;
    smg.endCharCode=smg.startCharCode;
    smg.startGlyphID=pcmap->suoyinPaixu[pcmap->sypxKaishiIdx].idx;
    groups.append(smg);
    //分段
    for(int i=pcmap->sypxKaishiIdx+1;i<sylen;i++){
        if(pcmap->suoyinPaixu[i].bianma==pcmap->suoyinPaixu[i-1].bianma+1 &&
                pcmap->suoyinPaixu[i].idx==pcmap->suoyinPaixu[i-1].idx+1     ){//paixu[0]=0
            //查找一个连续的区域
            groups.last().endCharCode=pcmap->suoyinPaixu[i].bianma;
        }else{
            //新开一段
            SequentialMapGroup smgx;
            smgx.startCharCode=pcmap->suoyinPaixu[i].bianma;
            smgx.endCharCode=smgx.startCharCode;
            smgx.startGlyphID=pcmap->suoyinPaixu[i].idx;
            groups.append(smgx);
        }
    }
    //
    reserved=0;
    language=0;
    numGroups=this->groups.length();
    length=sizeof(uint32)*(4+numGroups*3);
}

void format12::soujizifu(CMAPTable *pcmap)
{
    //获取字符数组，按照glyph id的顺序排列===>zifuXulieGlyphId
    for(uint i=0;i<numGroups;i++){
        for(uint32 c=groups[i].startCharCode;c<=groups[i].endCharCode;c++){
            if(groups[i].startGlyphID==UINT32_MAX){
                continue;
            }
            int32 glyphIdx=c-groups[i].startCharCode+groups[i].startGlyphID;//根据ttf文件记录计算出glyphIndex
            if(pcmap->zifuOrderByGlyphId[glyphIdx]==0){                   //zifuOrderByGlyphId中glyphIndex的位置还没有填写字符
                pcmap->zifuOrderByGlyphId[glyphIdx]=c;                    //填入此字符
            }else{                                                        //glyphIndex处已经填写了字符，说明是一个共用图形的字符
                uint32 zhubianma=pcmap->zifuOrderByGlyphId[glyphIdx];
                auto fnd=pcmap->shareZifu.find(zhubianma);                //在共用图形库shareZifu中查找这个key=glyphIndex
                if(fnd!=pcmap->shareZifu.end()){                          //找到了
                    fnd.value().append(c);                                //在共用库的这个位置补充这个字符c
                }else{                                                    //没有找到
                    u32vector sz;                                         //建立一个共用节点
                    sz.append(c);                                         //填写共用的字符c
                    pcmap->shareZifu.insert(zhubianma,sz);                //把此节点插入共用库中
                }
            }
            if(groups[i].startCharCode==groups[i].endCharCode){
                break;
            }
        }
    }
}


EncodingRecord::EncodingRecord(uint16 platid, uint16 encodid, uint16 fmtId):
    platformID(platid),encodingID(encodid),subtableOffset(0),fzFormatPoint(nullptr),fzFmtid(fmtId){}
EncodingRecord::~EncodingRecord(){
    if(fzFormatPoint){
        delete fzFormatPoint;
    }
}

void EncodingRecord::read(QDataStream&dt,uint16 fmtId, TableTTF* ptb){
    if(fmtId==4){
        fzFormatPoint=new format4;
        fzFormatPoint->read(dt);
    }else if(fmtId==12){
        fzFormatPoint=new format12;
        fzFormatPoint->read(dt);
    }else if(fmtId==0){
        fzFormatPoint=new format0;
        fzFormatPoint->read(dt);
    }else{
        ptb->datashow(QString("没有定义表，格式id=%1").arg(fmtId),true);
    }
}
void EncodingRecord::save(QDataStream&dt){
    if(fzFormatPoint){
        fzFormatPoint->save(dt);
    }
}
void EncodingRecord::show(CMAPTable* pcmap){
    if(fzFormatPoint){
        fzFormatPoint->show(pcmap);
    }
}

CMAPTable::CMAPTable(QObject *parent):TableTTF(parent){

}
void CMAPTable::read(QDataStream& dt, uint32 offset, uint16 tuxingZongshu){
    shareZifu.clear();
    zifuOrderByGlyphId.fill(0,tuxingZongshu);
    //
    dt.device()->seek(offset);
    dt>>version>>numTables;
    //
    for(int i=0;i<numTables;i++){
        EncodingRecord ex;
        dt>>ex.platformID>>ex.encodingID>>ex.subtableOffset;
        pEncodingRecord.append(ex);
    }
    int fmt12Idx=-1,fmt4Idx=-1;
    for(int i=0;i<numTables;i++){
        //如果subtableOffset 已存在
        bool ycz=false;
        for(int ix=0;ix<i;ix++){
            if(pEncodingRecord[i].subtableOffset==pEncodingRecord[ix].subtableOffset){
                pEncodingRecord[i].fzFmtid=pEncodingRecord[ix].fzFmtid;
                ycz=true;
                break;
            }
        }
        if(ycz){
            continue;
        }
        dt.device()->seek(offset+pEncodingRecord[i].subtableOffset);
        uint16 fmtId;
        dt>>fmtId;
        if(fmtId==12){
            fmt12Idx=i;
        }else if(fmtId==4){
            fmt4Idx=i;
        }
        pEncodingRecord[i].fzFmtid=fmtId;
        pEncodingRecord[i].read(dt,fmtId,this);
    }
    //可以单独试验两种格式
    if(fmt12Idx!=-1){
        pEncodingRecord[fmt12Idx].fzFormatPoint->soujizifu(this);
        zifuReady(this);
    }else if(fmt4Idx!=-1){
        pEncodingRecord[fmt4Idx].fzFormatPoint->soujizifu(this);
        zifuReady(this);
    }else{
        datashow("cmap format error");
    }
}
void CMAPTable::save(QDataStream& dt){
    dt<<version<<numTables;
    for(int i=0;i<numTables;i++){
        dt<<pEncodingRecord[i].platformID
         <<pEncodingRecord[i].encodingID
        <<pEncodingRecord[i].subtableOffset;
    }
    for(int i=0;i<numTables;i++){
        if(pEncodingRecord[i].fzFormatPoint){
            pEncodingRecord[i].save(dt);
        }
    }
}

void CMAPTable::show()
{
    datashow(QString("version:%1\t\t[0]\r\n"
                     "numTables:%2\r\n").arg(version).arg(numTables));
    for(int i=0;i<numTables;i++){
        QString fmtid;
        if(pEncodingRecord[i].fzFormatPoint){
            fmtid=QString::number(pEncodingRecord[i].fzFormatPoint->format);
        }
        datashow(QString("platformID:%1\r\n"
                         "encodingID:%2\r\n"
                         "subtableOffset:%3\r\n"
                         "格式ID:%4\r\n")
                 .arg(pEncodingRecord[i].platformID)
                 .arg(pEncodingRecord[i].encodingID)
                 .arg(pEncodingRecord[i].subtableOffset)
                 .arg(fmtid));
    }
    for(int i=0;i<numTables;i++){
        pEncodingRecord[i].show(this);
    }
}

void CMAPTable::xinjian(xinjianMuban xm)
{
    zifuOrderByGlyphId.clear();
    suoyinPaixu.clear();
    shareZifu.clear();
    //
    zifuOrderByGlyphId.append(0);
    zifuOrderByGlyphId.append(0x20);
    if(xm==xm3500){
        zifuOrderByGlyphId.append(zf3500.toUcs4());
    }else if(xm==xm6500){
        zifuOrderByGlyphId.append(zf6500.toUcs4());
    }
    zifuReady(this);
}

zidongZifu CMAPTable::jisuan(uint32 &u1, uint32 &u2, uint32 &u3, uint32 &u4,
                             uint16 &cKaishi, uint16 &cJieshu, s32vector& azidx){
    //azidx:a-z的glyphIndex
    //返回缺少的字符，如notdef 空格
    zidongZifu jg=zzOk;
    //准备索引排序
    //第一个不是0，则把0插入第一
    if(zifuOrderByGlyphId[0]!=0){
        zifuOrderByGlyphId.prepend(0);
        jg|=zzNotdef;
    }
    //空格不存在，则插入第二
    if(zifuOrderByGlyphId.indexOf(0x20)==-1){
        zifuOrderByGlyphId.insert(1,0x20);
        jg|=zzKongge;
    }
    //排序
    suoyinPaixu.clear();
    for(int i=0;i<zifuOrderByGlyphId.length();i++){
        suoyinPaixu.append(suoyinValue(i,zifuOrderByGlyphId[i]));
    }
    for(auto it=shareZifu.begin();it!=shareZifu.end();it++){
        //key=主编码 value=共享编码
        int32 idx=zifuOrderByGlyphId.indexOf(it.key());
        if(idx==-1){//排除共享字符中的无关项目---如果存在则是之前存在错误
            jg|=zzDiukey;
            continue;
        }
        //空格与其他字符共享笔迹只能在zifuOrderByGlyphId
        for(auto itv=it.value().begin();itv!=it.value().end();itv++){
            if(*itv!=0x20){
                suoyinPaixu.append(suoyinValue(idx,*itv));
            }
        }
    }
    sort(suoyinPaixu.begin(),suoyinPaixu.end(),[](const suoyinValue&a,const suoyinValue&b){
        //从小到大
        return a.bianma<b.bianma;
    });
    //
    //查找字母a-z和空格的glyphidx
    azidx.fill(0,27);
    int chaxunTongji=0;
    for(int i=0;i<suoyinPaixu.length();i++){
        switch(suoyinPaixu[i].bianma){
        case 0x20:
            azidx.last()=suoyinPaixu[i].idx;
            chaxunTongji++;
            break;
        case 0x61 ... 0x7a://a-z
            azidx[suoyinPaixu[i].bianma-0x61]=suoyinPaixu[i].idx;
            chaxunTongji++;
            break;
        }
        if(chaxunTongji>=27){
            break;
        }
    }
    //计算开始字符结束字符
    //开始字符，只找空格以后，空格以前的不记录--->存储后变成0---xxxxxx
    for(int i=1;i<suoyinPaixu.length();i++){
        if(suoyinPaixu[i].bianma>0x0){
            cKaishi=suoyinPaixu[i].bianma<0xffff?suoyinPaixu[i].bianma:0xffff;
            sypxKaishiIdx=i;
            break;
        }
    }
    cJieshu=suoyinPaixu.last().bianma<0xffff?suoyinPaixu.last().bianma:0xffff;
    getUnicode(u1, u2, u3, u4);
    //补充额外编码ffff
    if(suoyinPaixu.last().bianma<0xffff){
        suoyinPaixu.append(suoyinValue(1,0xffff));//末尾补充ffff
    }
    //0 3 0 4 3 1 3 10
    //删除全部数据重建
    pEncodingRecord.clear();
    version=0;
    // For current Apple platforms, use of platform ID 1 is discouraged.
    //pEncodingRecord.append(EncodingRecord(0,3,4));
    //pEncodingRecord.append(EncodingRecord(0,4,12));
    pEncodingRecord.append(EncodingRecord(3,1,4));
    pEncodingRecord.append(EncodingRecord(3,10,4));
    numTables=pEncodingRecord.length();
    //压缩数据
    //第一个表的偏移
    Offset32 pianyi=4+numTables*2*sizeof(Offset32);
    pEncodingRecord[0].subtableOffset=pianyi;
    pEncodingRecord[0].fzFormatPoint=new format4;
    pEncodingRecord[0].fzFormatPoint->jisuan(this);
    pEncodingRecord[1].subtableOffset=pianyi+pEncodingRecord[0].fzFormatPoint->getLength();
    pEncodingRecord[1].fzFormatPoint=new format12;
    pEncodingRecord[1].fzFormatPoint->jisuan(this);
    //pEncodingRecord[2].subtableOffset=pEncodingRecord[0].subtableOffset;
    //pEncodingRecord[3].subtableOffset=pEncodingRecord[1].subtableOffset;
    return jg;
}
void CMAPTable::setBianma0(const s32vector& glyphIdxSel){
    //编码改为0 删除对应的共享字符
    for(int i=glyphIdxSel.length()-1;i>=0;i--){
        shareZifu.remove(zifuOrderByGlyphId[glyphIdxSel[i]]);
        zifuOrderByGlyphId[glyphIdxSel[i]]=0;
    }
}
void CMAPTable::deleteWenzi(const s32vector &glyphIdxSel)
{
    //删除图形原始数据
    for(int i=glyphIdxSel.length()-1;i>=0;i--){
        shareZifu.remove(zifuOrderByGlyphId[glyphIdxSel[i]]);
        zifuOrderByGlyphId.removeAt(glyphIdxSel[i]);
    }
}
int CMAPTable::appendBianma(QString& ipt){
    //补充字符
    //如果是编码则补充编码
    //否则按照文字转换编码
    //返回0----------没有合适的文字或编码
    //返回>0，可以补充到glyf hmtx的图形数量
    //返回＜0，输入编码无效
    QRegularExpression reg("(?:\\$|0[xX])([0-9a-fA-F]+)[;,]");
    auto x=reg.globalMatch(ipt);
    QSet<uint32> tstbm;
    u32vector bianmaArr;
    while (x.hasNext()) {
        QRegularExpressionMatch match = x.next();
        uint32 bm = match.captured(1).toUInt(nullptr,16);
        tstbm.insert(bm);
        bianmaArr.append(bm);
    }
    if(tstbm.empty()){//没有检测到编码
        u32vector iptzifu=ipt.toUcs4();
        tstbm=QSet<uint32>(iptzifu.begin(),iptzifu.end());
    }else{
        //检测到编码--------直接复制不排序
        zifuOrderByGlyphId.append(bianmaArr);
        return bianmaArr.length();
    }
    //去除0x20前的字符
    for(int i=0;i<0x20;i++){
        tstbm.remove(i);
    }
    if(tstbm.empty()){
        return -1;
    }
    //搜集现有的字符防止重复。
    QSet<uint32> ycz(zifuOrderByGlyphId.begin(),zifuOrderByGlyphId.end());
    for(auto it=shareZifu.begin();it!=shareZifu.end();it++){
        QSet<uint32> yyg(it.value().begin(),it.value().end());
        ycz.unite(yyg);
    }
    tstbm.subtract(ycz);//输入字符减去已存在字符
    if(tstbm.empty()){
        return 0;
    }else{
        u32vector bc(tstbm.begin(),tstbm.end());
        sort(bc.begin(),bc.end(),[](const uint32&a,const uint32&b){
            //从小到大 标点在前

            //与网页保持排序一致
            //半角在前--方便田字格排版
            //标点集中在前--全角提出来

            //(0x21, 0x128),//半角英文
            //(0x2000<=a&&a<=0x206f),//标点,包含—‘’“”…
            //(0x3000<=a&&a<=0x30ff),//中日朝符号和标点，包含、。《》【】
            //(0xff00<=a&&a<=0xffef));//全角，包含！，：；？￥（）
            bool azai=(a>=0x21&&a<=0x128)||(0x2000<=a&&a<=0x206f)||(0x3000<=a&&a<=0x30ff)||(0xff00<=a&&a<=0xffef);
            bool bzai=(b>=0x21&&b<=0x128)||(0x2000<=b&&b<=0x206f)||(0x3000<=b&&b<=0x30ff)||(0xff00<=b&&b<=0xffef);
            bool aZaiqian=(azai^bzai)?azai:a<b;
            return aZaiqian;
        });
        zifuOrderByGlyphId.append(bc);
        return bc.length();
    }
}

void CMAPTable::getUnicodeMiaoshu(u32vector bm,QString &uniFenlei){
    //显示一个glyph对应编码的unicode描述
    uniFenlei="";
    for(int i=0;i<bm.length();i++){
        switch (bm[i]){
        case 0x0000:uniFenlei+="未编码";break;
        case 0x0001 ... 0x007F:uniFenlei+="基础拉丁语";break;
        case 0x0080 ... 0x00FF:uniFenlei+="拉丁语-1 补充";break;
        case 0x0100 ... 0x017F:uniFenlei+="拉丁语扩展-A";break;
        case 0x0180 ... 0x024F:uniFenlei+="拉丁语扩展-B";break;
        case 0x0250 ... 0x02AF:uniFenlei+="国际音标扩展";break;
        case 0x1D00 ... 0x1D7F:uniFenlei+="拼音扩展";break;
        case 0x1D80 ... 0x1DBF:uniFenlei+="语音扩展补充";break;
        case 0x02B0 ... 0x02FF:uniFenlei+="间距修饰符字母";break;
        case 0xA700 ... 0xA71F:uniFenlei+="声调修饰符";break;
        case 0x0300 ... 0x036F:uniFenlei+="组合变音符号";break;
        case 0x1DC0 ... 0x1DFF:uniFenlei+="组合变音符号补充";break;
        case 0x0370 ... 0x03FF:uniFenlei+="希腊语和科普特语";break;
        case 0x2C80 ... 0x2CFF:uniFenlei+="科普特语";break;
        case 0x0400 ... 0x04FF:uniFenlei+="西里尔";break;
        case 0x0500 ... 0x052F:uniFenlei+="西里尔文补充";break;
        case 0x2DE0 ... 0x2DFF:uniFenlei+="西里尔文扩展-A";break;
        case 0xA640 ... 0xA69F:uniFenlei+="西里尔文扩展-B";break;
        case 0x0530 ... 0x058F:uniFenlei+="亚美尼亚语";break;
        case 0x0590 ... 0x05FF:uniFenlei+="希伯来语";break;
        case 0xA500 ... 0xA63F:uniFenlei+="瓦伊语";break;
        case 0x0600 ... 0x06FF:uniFenlei+="阿拉伯语";break;
        case 0x0750 ... 0x077F:uniFenlei+="阿拉伯语补充";break;
        case 0x08a0 ... 0x08ff:uniFenlei+="阿拉伯语扩展A";break;//os2页面没写
        case 0x07C0 ... 0x07FF:uniFenlei+="西非书面文字";break;
        case 0x0900 ... 0x097F:uniFenlei+="梵文";break;
        case 0x0980 ... 0x09FF:uniFenlei+="孟加拉语";break;
        case 0x0A00 ... 0x0A7F:uniFenlei+="果鲁穆奇语";break;
        case 0x0A80 ... 0x0AFF:uniFenlei+="古吉拉特语";break;
        case 0x0B00 ... 0x0B7F:uniFenlei+="奥里亚语";break;
        case 0x0B80 ... 0x0BFF:uniFenlei+="泰米尔语";break;
        case 0x0C00 ... 0x0C7F:uniFenlei+="泰卢固语";break;
        case 0x0C80 ... 0x0CFF:uniFenlei+="卡纳拉语";break;
        case 0x0D00 ... 0x0D7F:uniFenlei+="马拉雅拉姆语";break;
        case 0x0E00 ... 0x0E7F:uniFenlei+="泰语";break;
        case 0x0E80 ... 0x0EFF:uniFenlei+="老挝语";break;
        case 0x10A0 ... 0x10FF:uniFenlei+="格鲁吉亚语";break;
        case 0x2D00 ... 0x2D2F:uniFenlei+="格鲁吉亚补充";break;
        case 0x1B00 ... 0x1B7F:uniFenlei+="巴厘语";break;
        case 0x1100 ... 0x11FF:uniFenlei+="韩文字母";break;
        case 0x1E00 ... 0x1EFF:uniFenlei+="拉丁语扩展附加";break;
        case 0x2C60 ... 0x2C7F:uniFenlei+="拉丁语扩展-C";break;
        case 0xA720 ... 0xA7FF:uniFenlei+="拉丁语扩展-D";break;
        case 0x1F00 ... 0x1FFF:uniFenlei+="希腊语扩展";break;
        case 0x2000 ... 0x206F:uniFenlei+="通用标点符号";break;
        case 0x2E00 ... 0x2E7F:uniFenlei+="补充标点符号";break;
        case 0x2070 ... 0x209F:uniFenlei+="上标和下标";break;
        case 0x20A0 ... 0x20CF:uniFenlei+="货币符号";break;
        case 0x20D0 ... 0x20FF:uniFenlei+="组合符号的变音符号";break;
        case 0x2100 ... 0x214F:uniFenlei+="类字母符号";break;
        case 0x2150 ... 0x218F:uniFenlei+="分数和罗马数字";break;
        case 0x2190 ... 0x21FF:uniFenlei+="箭头";break;
        case 0x27F0 ... 0x27FF:uniFenlei+="补充箭头-A";break;
        case 0x2900 ... 0x297F:uniFenlei+="补充箭头-B";break;
        case 0x2B00 ... 0x2BFF:uniFenlei+="其他符号和箭头";break;
        case 0x2200 ... 0x22FF:uniFenlei+="数学运算符";break;
        case 0x2A00 ... 0x2AFF:uniFenlei+="补充数学运算符";break;
        case 0x27C0 ... 0x27EF:uniFenlei+="杂项数学符号-A";break;
        case 0x2980 ... 0x29FF:uniFenlei+="杂项数学符号-B";break;
        case 0x2300 ... 0x23FF:uniFenlei+="其他技术";break;
        case 0x2400 ... 0x243F:uniFenlei+="控制图片";break;
        case 0x2440 ... 0x245F:uniFenlei+="光学字符识别";break;
        case 0x2460 ... 0x24FF:uniFenlei+="带圈的字母数字";break;
        case 0x2500 ... 0x257F:uniFenlei+="箱体图纸";break;
        case 0x2580 ... 0x259F:uniFenlei+="块元素";break;
        case 0x25A0 ... 0x25FF:uniFenlei+="几何形状";break;
        case 0x2600 ... 0x26FF:uniFenlei+="其他符号";break;
        case 0x2700 ... 0x27BF:uniFenlei+="装饰符号";break;
        case 0x3000 ... 0x303F:uniFenlei+="中日韩符号和标点符号";break;
        case 0x3040 ... 0x309F:uniFenlei+="平假名";break;
        case 0x30A0 ... 0x30FF:uniFenlei+="片假名";break;
        case 0x31F0 ... 0x31FF:uniFenlei+="片假名语音扩展";break;
        case 0x3100 ... 0x312F:uniFenlei+="注音符号BPMF";break;
        case 0x31A0 ... 0x31BF:uniFenlei+="BPMF扩展";break;
        case 0x3130 ... 0x318F:uniFenlei+="韩文兼容字母";break;
        case 0xA840 ... 0xA87F:uniFenlei+="八思巴字";break;
        case 0x3200 ... 0x32FF:uniFenlei+="中日韩带圈字符和月份";break;
        case 0x3300 ... 0x33FF:uniFenlei+="中日韩兼容性";break;
        case 0xAC00 ... 0xD7AF:uniFenlei+="韩文音节";break;
        case 0x10900 ... 0x1091F:uniFenlei+="腓尼基字母";break;
        case 0x4E00 ... 0x9FFF:uniFenlei+="中日韩统一表意文字";break;
        case 0x2E80 ... 0x2EFF:uniFenlei+="中日韩汉字部首补充";break;
        case 0x2F00 ... 0x2FDF:uniFenlei+="康熙部首";break;
        case 0x2FF0 ... 0x2FFF:uniFenlei+="表意文字描述字符-汉字结构";break;
        case 0x3400 ... 0x4DBF:uniFenlei+="中日韩统一表意文字扩展 A";break;
        case 0x20000 ... 0x2A6DF:uniFenlei+="中日韩统一表意文字扩展 B";break;
        case 0x3190 ... 0x319F:uniFenlei+="日文使用解读汉语文言文";break;
        case 0xE000 ... 0xF8FF:uniFenlei+="私人使用区";break;
        case 0x31C0 ... 0x31EF:uniFenlei+="中日韩笔画";break;
        case 0xF900 ... 0xFAFF:uniFenlei+="中日韩兼容性表意文字";break;
        case 0x2F800 ... 0x2FA1F:uniFenlei+="中日韩兼容性表意文字补充";break;
        case 0xFB00 ... 0xFB4F:uniFenlei+="字母连写";break;
        case 0xFB50 ... 0xFDFF:uniFenlei+="阿拉伯语-A";break;
        case 0xFE20 ... 0xFE2F:uniFenlei+="组合半符号";break;
        case 0xFE10 ... 0xFE1F:uniFenlei+="竖排标点";break;
        case 0xFE30 ... 0xFE4F:uniFenlei+="中日韩兼容性竖排标点";break;
        case 0xFE50 ... 0xFE6F:uniFenlei+="小形式变体-标点";break;
        case 0xFE70 ... 0xFEFF:uniFenlei+="阿拉伯语-B";break;
        case 0xFF00 ... 0xFFEF:uniFenlei+="半宽和全宽形式";break;
        case 0xFFF0 ... 0xFFFF:uniFenlei+="特殊字符";break;
        case 0x0F00 ... 0x0FFF:uniFenlei+="藏语";break;
        case 0x0700 ... 0x074F:uniFenlei+="古叙利亚语";break;
        case 0x0780 ... 0x07BF:uniFenlei+="塔纳";break;
        case 0x0D80 ... 0x0DFF:uniFenlei+="僧伽罗语";break;
        case 0x1000 ... 0x109F:uniFenlei+="缅甸";break;
        case 0x1200 ... 0x137F:uniFenlei+="埃塞俄比亚语";break;
        case 0x1380 ... 0x139F:uniFenlei+="埃塞俄比亚补充";break;
        case 0x2D80 ... 0x2DDF:uniFenlei+="埃塞俄比亚扩展";break;
        case 0x13A0 ... 0x13FF:uniFenlei+="切罗基语";break;
        case 0x1400 ... 0x167F:uniFenlei+="统一的加拿大原住民音节";break;
        case 0x1680 ... 0x169F:uniFenlei+="欧甘字母";break;
        case 0x16A0 ... 0x16FF:uniFenlei+="卢恩字母";break;
        case 0x1780 ... 0x17FF:uniFenlei+="高棉语";break;
        case 0x19E0 ... 0x19FF:uniFenlei+="高棉语符号";break;
        case 0x1800 ... 0x18AF:uniFenlei+="蒙古语";break;
        case 0x2800 ... 0x28FF:uniFenlei+="盲文图案";break;
        case 0xA000 ... 0xA48F:uniFenlei+="彝音节";break;
        case 0xA490 ... 0xA4CF:uniFenlei+="彝族部首";break;
        case 0x1700 ... 0x171F:uniFenlei+="塔加洛语";break;
        case 0x1720 ... 0x173F:uniFenlei+="哈努诺文";break;
        case 0x1740 ... 0x175F:uniFenlei+="布希德";break;
        case 0x1760 ... 0x177F:uniFenlei+="塔格巴努亚文";break;
        case 0x10300 ... 0x1032F:uniFenlei+="旧斜体";break;
        case 0x10330 ... 0x1034F:uniFenlei+="哥特式";break;
        case 0x10400 ... 0x1044F:uniFenlei+="德瑟雷特字母";break;
        case 0x1D000 ... 0x1D0FF:uniFenlei+="拜占庭音乐符号";break;
        case 0x1D100 ... 0x1D1FF:uniFenlei+="音乐符号";break;
        case 0x1D200 ... 0x1D24F:uniFenlei+="古希腊乐谱";break;
        case 0x1D400 ... 0x1D7FF:uniFenlei+="数学字母数字符号";break;
        case 0xF0000 ... 0xFFFFD:uniFenlei+="私人使用";break;
        case 0x100000 ... 0x10FFFD:uniFenlei+="私人使用";break;
        case 0xFE00 ... 0xFE0F:uniFenlei+="变体选择器";break;
        case 0xE0000 ... 0xE007F:uniFenlei+="变体选择器补充";break;
        case 0xE0100 ... 0xE01EF:uniFenlei+="标签";break;
        case 0x1900 ... 0x194F:uniFenlei+="林布语";break;
        case 0x1950 ... 0x197F:uniFenlei+="泰乐";break;
        case 0x1980 ... 0x19DF:uniFenlei+="新泰禄";break;
        case 0x1A00 ... 0x1A1F:uniFenlei+="布吉语";break;
        case 0x2C00 ... 0x2C5F:uniFenlei+="格拉哥里语";break;
        case 0x2D30 ... 0x2D7F:uniFenlei+="提菲纳";break;
        case 0x4DC0 ... 0x4DFF:uniFenlei+="易经六十四卦符号";break;
        case 0xA800 ... 0xA82F:uniFenlei+="锡尔赫特文";break;
        case 0x10000 ... 0x1007F:uniFenlei+="线性B 音节";break;
        case 0x10080 ... 0x100FF:uniFenlei+="线性B 表意文字";break;
        case 0x10100 ... 0x1013F:uniFenlei+="爱琴海数字";break;
        case 0x10140 ... 0x1018F:uniFenlei+="古希腊数字";break;
        case 0x10380 ... 0x1039F:uniFenlei+="乌加里特语";break;
        case 0x103A0 ... 0x103DF:uniFenlei+="古波斯语";break;
        case 0x10450 ... 0x1047F:uniFenlei+="沙维安";break;
        case 0x10480 ... 0x104AF:uniFenlei+="奥斯曼亚";break;
        case 0x10800 ... 0x1083F:uniFenlei+="塞浦路斯音节";break;
        case 0x10A00 ... 0x10A5F:uniFenlei+="卡罗什蒂";break;
        case 0x1D300 ... 0x1D35F:uniFenlei+="太玄经符号";break;
        case 0x12000 ... 0x123FF:uniFenlei+="楔形文字";break;
        case 0x12400 ... 0x1247F:uniFenlei+="楔形文字数字和标点符号";break;
        case 0x1D360 ... 0x1D37F:uniFenlei+="算筹";break;
        case 0x1B80 ... 0x1BBF:uniFenlei+="巽他语";break;
        case 0x1C00 ... 0x1C4F:uniFenlei+="绒巴文";break;
        case 0x1C50 ... 0x1C7F:uniFenlei+="桑塔利语字母";break;
        case 0xA880 ... 0xA8DF:uniFenlei+="索拉什特拉语";break;
        case 0xA900 ... 0xA92F:uniFenlei+="克耶字母";break;
        case 0xA930 ... 0xA95F:uniFenlei+="拉让语";break;
        case 0xAA00 ... 0xAA5F:uniFenlei+="卡姆";break;
        case 0x10190 ... 0x101CF:uniFenlei+="古代符号";break;
        case 0x101D0 ... 0x101FF:uniFenlei+="斐斯托斯圆盘古文字";break;
        case 0x102A0 ... 0x102DF:uniFenlei+="卡里亚字母";break;
        case 0x10280 ... 0x1029F:uniFenlei+="利西亚语";break;
        case 0x10920 ... 0x1093F:uniFenlei+="吕底亚";break;
        case 0x1F030 ... 0x1F09F:uniFenlei+="多米诺骨牌";break;
        case 0x1F000 ... 0x1F02F:uniFenlei+="麻将";break;
        case 0x2A700 ... 0x2B73F:uniFenlei+="中日韩统一表意文字扩展 C";break;//不在opentype网页上
        case 0x2B740 ... 0x2B81F:uniFenlei+="中日韩统一表意文字扩展 D";break;//不在opentype网页上
        case 0x2B820 ... 0x2CEAF:uniFenlei+="中日韩统一表意文字扩展 E";break;//不在opentype网页上
        case 0x2CEB0 ... 0x2EBEF:uniFenlei+="中日韩统一表意文字扩展 F";break;//不在opentype网页上
        }
        uniFenlei+="\r\n";
    }
}
QString CMAPTable::showAll(QString fenleiName,s32vector& xianshiZifu){
    //显示特定分类的字符
    if(fenleiName=="xsWeibianma"){
        xianshiZifu.clear();
        for(int i=0;i<zifuOrderByGlyphId.length();i++){
            if(zifuOrderByGlyphId[i]==0&&i!=0){
                xianshiZifu.append(i);
            }
        }
        return "未编码";
    }else if(fenleiName=="xsBiaodian"){
        xianshiZifu.clear();
        for(int i=0;i<zifuOrderByGlyphId.length();i++){
            switch(zifuOrderByGlyphId[i]){
            case 0x20 ... 0x2f://键盘
            case 0x3A ... 0x40://键盘
            case 0x5B ... 0x60://键盘
            case 0x7B ... 0x7E://键盘
            case 0x89://千分号
            case 0x91 ... 0x94://其他语言的单引号双引号
            case 0xA5:
            case 0x2000 ... 0x206F://通用标点
            case 0x2E00 ... 0x2E7F://补充标点
            case 0x3000 ... 0x3006://中日韩标点 去掉〇
            case 0x3008 ... 0x3020://中日韩标点 去掉横竖数字
            case 0x302a ... 0x3037://中日韩标点 去掉十 二十 三十
            case 0x303b ... 0x303F://中日韩标点
            case 0xFE10 ... 0xFE1F://竖排标点
            case 0xFE30 ... 0xFE6F://中日韩竖排标点//小标点
            case 0xff01 ... 0xff0f://全角
            case 0xff1A ... 0xff20://全角
            case 0xff3B ... 0xff40://全角
            case 0xff5B ... 0xff5E://全角
            case 0xffe5:
                xianshiZifu.append(i);
            }
        }
        return "标点";
    }else if(fenleiName=="xsShuzi"){
        xianshiZifu.clear();
        for(int i=0;i<zifuOrderByGlyphId.length();i++){
            switch(zifuOrderByGlyphId[i]){
            case 0x30 ... 0x39://0..9
            case 0xb2 ... 0xb3://上标2 3
            case 0xb9://上标1
            case 0xbc ... 0xbe://1/4 1/2 3/4
            case 0x2070://上标0
            case 0x2074 ... 0x2079://上标4 56789
            case 0x2080 ... 0x2089://下标0..9
            case 0x3021 ... 0x3029://〡〢〣〤〥〦〧〨〩
            case 0x3038 ... 0x303a://十 二十 三十
            case 0x3251 ... 0x325f://㉑。。㉟
            case 0x32b1 ... 0x32bf://㊱..㊿
            case 0x2150 ... 0x218f://分数和罗马数字
            case 0x2460 ... 0x249b://①..⑳⑴..⒇⒈..⒛
            case 0x24ea ... 0x24ff:
            case 0x2776 ... 0x277f://❶..❿
            case 0xff10 ... 0xff19:xianshiZifu.append(i);
            }
        }
        return "数字";
    }else if(fenleiName=="xsYingwen"){
        xianshiZifu.clear();
        for(int i=0;i<zifuOrderByGlyphId.length();i++){
            switch(zifuOrderByGlyphId[i]){
            case 0x41 ... 0x5a:
            case 0x61 ... 0x7a:
            case 0xff21 ... 0xff3a:
            case 0xff41 ... 0xff5a:xianshiZifu.append(i);
            }
        }
        return "英文字母";
    }else if(fenleiName=="xsZhongwen"){
        xianshiZifu.clear();
        for(int i=0;i<zifuOrderByGlyphId.length();i++){
            switch(zifuOrderByGlyphId[i]){
            case 0x2E80 ... 0x2FDF://"中日韩汉字部首补充";"康熙部首";
            case 0x3007://〇
            case 0x31C0 ... 0x31EF://"中日韩笔画";
            case 0x3200 ... 0x3250://去掉带圈的阿拉伯数字
            case 0x3260 ... 0x32b0://去掉带圈的阿拉伯数字
            case 0x32c0 ... 0x4DBF://"中日韩带圈字符和月份";"中日韩兼容性";"中日韩统一表意文字扩展 A";
            case 0x4E00 ... 0x9FFF://"中日韩统一表意文字";
            case 0xF900 ... 0xFAFF://"中日韩兼容性表意文字";
            case 0x20000 ... 0x2A6DF://"中日韩统一表意文字扩展 B";
            case 0x2A700 ... 0x2EBEF://"中日韩统一表意文字扩展 CDEF";
            case 0x2F800 ... 0x2FA1F://"中日韩兼容性表意文字补充"
                xianshiZifu.append(i);
            }
        }
        return "中文";
    }else if(fenleiName=="xsPianpang"){
        xianshiZifu.clear();
        for(int i=0;i<zifuOrderByGlyphId.length();i++){
            switch(zifuOrderByGlyphId[i]){
            case 0x310B:
            case 0x310E:
            case 0x3110:
            case 0x3123:
            case 0x3125:
            case 0x3404 ... 0x3406:
            case 0x34C1:
            case 0x353E ... 0x353F:
            case 0x38FA:
            case 0x39AE:
            case 0x3B30:
            case 0x43CC ... 0x43CD:
            case 0x4491:
            case 0x4E02:
            case 0x4E04 ... 0x4E06:
            case 0x4E0C:
            case 0x4E28 ... 0x4E29:
            case 0x4E2C:
            case 0x4E2F:
            case 0x4E31:
            case 0x4E33:
            case 0x4E35 ... 0x4E37:
            case 0x4E3F:
            case 0x4E40 ... 0x4E42:
            case 0x4E44:
            case 0x4E46 ... 0x4E47:
            case 0x4E4A:
            case 0x4E51:
            case 0x4E5A ... 0x4E5C:
            case 0x4E85:
            case 0x4EA0:
            case 0x4EBB:
            case 0x5182 ... 0x5184:
            case 0x5186 ... 0x5187:
            case 0x518B:
            case 0x518E:
            case 0x5196:
            case 0x51AB:
            case 0x51F5:
            case 0x5202:
            case 0x52F9:
            case 0x531A:
            case 0x5369:
            case 0x536A:
            case 0x536C:
            case 0x5383:
            case 0x5902:
            case 0x590A:
            case 0x5917:
            case 0x5918:
            case 0x592C:
            case 0x5B80:
            case 0x5C22:
            case 0x5C6E:
            case 0x5C75:
            case 0x5DDB:
            case 0x5DDC:
            case 0x5EF4:
            case 0x5EFE:
            case 0x5F0B:
            case 0x5F14:
            case 0x5F1A:
            case 0x5F50:
            case 0x5F51:
            case 0x5F54:
            case 0x5F56:
            case 0x5F61:
            case 0x5F73:
            case 0x5FC4:
            case 0x6209:
            case 0x620B:
            case 0x624C:
            case 0x6534:
            case 0x65E1:
            case 0x66F5:
            case 0x66F7:
            case 0x6B68:
            case 0x6B7A:
            case 0x6BB3:
            case 0x6C35:
            case 0x6C36:
            case 0x6C3A:
            case 0x706C:
            case 0x722B:
            case 0x722D:
            case 0x722F:
            case 0x7230:
            case 0x723F:
            case 0x725C:
            case 0x72AD:
            case 0x72AE:
            case 0x738D:
            case 0x7539:
            case 0x753E:
            case 0x7540:
            case 0x7541:
            case 0x7557:
            case 0x755A:
            case 0x758B ... 0x758D:
            case 0x7590:
            case 0x7592:
            case 0x7679:
            case 0x7680:
            case 0x7681:
            case 0x768B:
            case 0x76CD:
            case 0x7714:
            case 0x7718:
            case 0x77DE:
            case 0x793B:
            case 0x79B8:
            case 0x79BA:
            case 0x7CF8:
            case 0x7E9F:
            case 0x7F36:
            case 0x7F52:
            case 0x7F53:
            case 0x7F8C:
            case 0x8002:
            case 0x8011:
            case 0x8012:
            case 0x807F:
            case 0x8080:
            case 0x808E:
            case 0x8099:
            case 0x81FE ... 0x81FF:
            case 0x8203 ... 0x8204:
            case 0x8279:
            case 0x8864:
            case 0x886E:
            case 0x8901:
            case 0x8980:
            case 0x8BA0:
            case 0x8C55 ... 0x8C56:
            case 0x8C59:
            case 0x8C78:
            case 0x8FB5 ... 0x8FB6:
            case 0x9485:
            case 0x9578:
            case 0x961D:
            case 0x96B9 ... 0x96BD:
            case 0x96CB:
            case 0x97CB:
            case 0x98E0:
            case 0x9963:
            case 0x9B32:
            case 0x9EFD ... 0x9EFE:
            case 0x9FB4 ... 0x9FBB:
                //case 0x2E80 ... 0x2FDF://"中日韩汉字部首补充";"康熙部首";
                //case 0x31C0 ... 0x31EF://"中日韩笔画";
            case 0x2E80 ... 0x2E81:
            case 0x2E84:
            case 0x2E88:
            case 0x2E8C ... 0x2E8D:
            case 0x2E97:
            case 0x2E99:
            case 0x2E9C:
            case 0x2E9E:
            case 0x2EA2:
            case 0x2EA7:
            case 0x2EA9:
            case 0x2EAA:
            case 0x2EAE:
            case 0x2EB4 ... 0x2EB8:
            case 0x2EBB:
            case 0x2ECA://⻊
            case 0x2ED2:
            case 0x2ED7:
            case 0x2EEB:
            case 0x2F1B:
            case 0x2F21 ... 0x2F23:
            case 0x2F68:
            case 0x2F71:
            case 0x2F87:
            case 0x2F8C:
            case 0x31C2:
            case 0x31C4 ... 0x31CF:
            case 0x31D5:
            case 0x31D7 ... 0x31D9:
            case 0x31DB ... 0x31DE:
            case 0x31E1:
                xianshiZifu.append(i);
            }
        }
        return "偏旁部首";
    }else if(fenleiName=="xsSiyou"){
        xianshiZifu.clear();
        for(int i=0;i<zifuOrderByGlyphId.length();i++){
            switch(zifuOrderByGlyphId[i]){
            case 0xE000 ... 0xF8FF:
            case 0xF0000 ... 0xFFFFD:
            case 0x100000 ... 0x10FFFD:
                xianshiZifu.append(i);
            }
        }
        return "私有区域";
    }else if(fenleiName=="xsPinyin"){
        xianshiZifu.clear();
        for(int i=0;i<zifuOrderByGlyphId.length();i++){
            switch(zifuOrderByGlyphId[i]){
            case 0xC0 ... 0xC1:
            case 0xC8 ... 0xC9:
            case 0xCC ... 0xCD:
            case 0xD2 ... 0xD3:
            case 0xD9 ... 0xDA:
            case 0xE0 ... 0xE1:
            case 0xE8 ... 0xE9:
            case 0xEC ... 0xED:
            case 0xF2 ... 0xF3:
            case 0xF9 ... 0xFA:
            case 0xFC:
            case 0x100 ... 0x101:
            case 0x112 ... 0x113:
            case 0x11A ... 0x11B:
            case 0x12B:
            case 0X14C ... 0X14D:
            case 0x16A ... 0x16B:
            case 0x1ce:
            case 0X1D0:
            case 0X1D2:
            case 0X1D4:
            case 0X1D6:
            case 0X1D8:
            case 0X1DA:
            case 0X1DC:
            case 0x2C7:
            case 0x2c9 ... 0x2cb:
                xianshiZifu.append(i);
            }
        }
        return "汉语拼音";
    }else if(fenleiName=="xsZhuyin"){
        xianshiZifu.clear();
        for(int i=0;i<zifuOrderByGlyphId.length();i++){
            switch(zifuOrderByGlyphId[i]){
            case 0x3100 ... 0x312F://注音符号BPMF
            case 0x31A0 ... 0x31BF://BPMF扩展
                xianshiZifu.append(i);
            }
        }
        return "注音符号";
    }else if(fenleiName=="xsShuxue"){
        xianshiZifu.clear();
        for(int i=0;i<zifuOrderByGlyphId.length();i++){
            switch(zifuOrderByGlyphId[i]){
            case 0xb1:
            case 0xd7:
            case 0xF7:
            case 0x2200 ... 0x22FF://数学运算符
            case 0x2A00 ... 0x2AFF://补充数学运算符
            case 0x27C0 ... 0x27EF://杂项数学符号-A
            case 0x2980 ... 0x29FF://杂项数学符号-B
                xianshiZifu.append(i);
            }
        }
        return "数学符号";
    }else if(fenleiName=="xsXila"){
        xianshiZifu.clear();
        for(int i=0;i<zifuOrderByGlyphId.length();i++){
            switch(zifuOrderByGlyphId[i]){
            case 0x0370 ... 0x03FF://希腊语和科普特语
            case 0x1F00 ... 0x1FFF://希腊语扩展
                xianshiZifu.append(i);
            }
        }
        return "希腊字母";
    }else if(fenleiName=="xsQita"){
        xianshiZifu.clear();
        for(int i=0;i<zifuOrderByGlyphId.length();i++){
            switch(zifuOrderByGlyphId[i]){
            case 0x0:
            case 0x20 ... 0x7E:
            case 0x89:
            case 0x91 ... 0x94:
            case 0xa5:
            case 0xb1 ... 0xb3://上标2 3
            case 0xb9://上标1
            case 0xbc ... 0xbe://1/4 1/2 3/4
            case 0xC0 ... 0xC1:
            case 0xC8 ... 0xC9:
            case 0xCC ... 0xCD:
            case 0xD2 ... 0xD3:
            case 0xD7:
            case 0xD9 ... 0xDA:
            case 0xE0 ... 0xE1:
            case 0xE8 ... 0xE9:
            case 0xEC ... 0xED:
            case 0xF2 ... 0xF3:
            case 0xF7:
            case 0xF9 ... 0xFA:
            case 0xFC:
            case 0x100 ... 0x101:
            case 0x112 ... 0x113:
            case 0x11A ... 0x11B:
            case 0x12B:
            case 0X14C ... 0X14D:
            case 0x16A ... 0x16B:
            case 0x1ce:
            case 0X1D0:
            case 0X1D2:
            case 0X1D4:
            case 0X1D6:
            case 0X1D8:
            case 0X1DA:
            case 0X1DC:
            case 0x2C7:
            case 0x2c9 ... 0x2cb:
            case 0x0370 ... 0x03FF://希腊语和科普特语
            case 0x1F00 ... 0x1FFF://希腊语扩展
            case 0x2000 ... 0x2070:
            case 0x2074 ... 0x2079://上标4 5
            case 0x2080 ... 0x2089://下标0..9
            case 0x2150 ... 0x218f:
            case 0x2200 ... 0x22FF://数学运算符
            case 0x2460 ... 0x249b:
            case 0x24ea ... 0x24ff:
            case 0x2776 ... 0x277f:
            case 0x27C0 ... 0x27EF://杂项数学符号-A
            case 0x2980 ... 0x29FF://杂项数学符号-B
            case 0x2A00 ... 0x2AFF://补充数学运算符
            case 0x2E00 ... 0x2FDF:
            case 0x3000 ... 0x303F:
            case 0x3100 ... 0x312F:
            case 0x31A0 ... 0x31EF:
            case 0x3200 ... 0x4DBF://"中日韩统一表意文字扩展 A";
            case 0x4E00 ... 0x9FFF://"中日韩统一表意文字";
            case 0xE000 ... 0xF8FF://私人使用
            case 0xF900 ... 0xFAFF://"中日韩兼容性表意文字";
            case 0xFE10 ... 0xFE1F://竖排标点
            case 0xFE30 ... 0xFE6F:
            case 0xff01 ... 0xff5E:
            case 0xffe5:
            case 0x20000 ... 0x2A6DF://"中日韩统一表意文字扩展 B";
            case 0x2A700 ... 0x2EBEF://"中日韩统一表意文字扩展 CDEF";
            case 0x2F800 ... 0x2FA1F://"中日韩兼容性表意文字补充"
            case 0xF0000 ... 0xFFFFD://私人使用
            case 0x100000 ... 0x10FFFD://私人使用
                break;
            default:
                xianshiZifu.append(i);
            }
        }
        return "其他文字";
    }
    return "------";
}
void CMAPTable::getUnicode(uint32 &u1, uint32 &u2, uint32 &u3, uint32 &u4){
    //统计字体中全部字符的unicode分类
    u1=u2=u3=u4=0;
    if(suoyinPaixu.last().bianma>0xffff){
        //case 0x10000 ... 0x10FFFF:
        u2|=0x02000000;
    }
    for(int i=0;i<suoyinPaixu.length();i++){
        switch (suoyinPaixu[i].bianma){
        case 0x0000 ... 0x007F:u1|=0x1;break;
        case 0x0080 ... 0x00FF:u1|=0x2;break;
        case 0x0100 ... 0x017F:u1|=0x4;break;
        case 0x0180 ... 0x024F:u1|=0x8;break;
        case 0x0250 ... 0x02AF:
        case 0x1D00 ... 0x1D7F:
        case 0x1D80 ... 0x1DBF:u1|=0x10;break;
        case 0x02B0 ... 0x02FF:
        case 0xA700 ... 0xA71F:u1|=0x20;break;
        case 0x0300 ... 0x036F:
        case 0x1DC0 ... 0x1DFF:u1|=0x40;break;
        case 0x0370 ... 0x03FF:u1|=0x80;break;
        case 0x2C80 ... 0x2CFF:u1|=0x0100;break;
            //9
        case 0x0400 ... 0x04FF:
        case 0x0500 ... 0x052F:
        case 0x2DE0 ... 0x2DFF:
        case 0xA640 ... 0xA69F:u1|=0x0200;break;
            //10
        case 0x0530 ... 0x058F:u1|=0x0400;break;
        case 0x0590 ... 0x05FF:u1|=0x0800;break;
        case 0xA500 ... 0xA63F:u1|=0x1000;break;
            //13
        case 0x0600 ... 0x06FF:
        case 0x0750 ... 0x077F:u1|=0x2000;break;
            //14
        case 0x07C0 ... 0x07FF:u1|=0x00004000;break;
        case 0x0900 ... 0x097F:u1|=0x00008000;break;
        case 0x0980 ... 0x09FF:u1|=0x00010000;break;
        case 0x0A00 ... 0x0A7F:u1|=0x00020000;break;
        case 0x0A80 ... 0x0AFF:u1|=0x00040000;break;
        case 0x0B00 ... 0x0B7F:u1|=0x00080000;break;
        case 0x0B80 ... 0x0BFF:u1|=0x00100000;break;
        case 0x0C00 ... 0x0C7F:u1|=0x00200000;break;
        case 0x0C80 ... 0x0CFF:u1|=0x00400000;break;
        case 0x0D00 ... 0x0D7F:u1|=0x00800000;break;
        case 0x0E00 ... 0x0E7F:u1|=0x01000000;break;
        case 0x0E80 ... 0x0EFF:u1|=0x02000000;break;
            //26
        case 0x10A0 ... 0x10FF:
        case 0x2D00 ... 0x2D2F:u1|=0x04000000;break;
            //27
        case 0x1B00 ... 0x1B7F:u1|=0x08000000;break;
        case 0x1100 ... 0x11FF:u1|=0x10000000;break;
            //29
        case 0x1E00 ... 0x1EFF:
        case 0x2C60 ... 0x2C7F:
        case 0xA720 ... 0xA7FF:u1|=0x20000000;break;
            //30
        case 0x1F00 ... 0x1FFF:u1|=0x40000000;break;
            //31
        case 0x2000 ... 0x206F:
        case 0x2E00 ... 0x2E7F:u1|=0x80000000;break;
            //32
        case 0x2070 ... 0x209F:u2|=0x00000001;break;
        case 0x20A0 ... 0x20CF:u2|=0x00000002;break;
        case 0x20D0 ... 0x20FF:u2|=0x00000004;break;
        case 0x2100 ... 0x214F:u2|=0x00000008;break;
        case 0x2150 ... 0x218F:u2|=0x00000010;break;
            //37
        case 0x2190 ... 0x21FF:
        case 0x27F0 ... 0x27FF:
        case 0x2900 ... 0x297F:
        case 0x2B00 ... 0x2BFF:u2|=0x00000020;break;
            //38
        case 0x2200 ... 0x22FF:
        case 0x2A00 ... 0x2AFF:
        case 0x27C0 ... 0x27EF:
        case 0x2980 ... 0x29FF:u2|=0x00000040;break;
            //39-49
        case 0x2300 ... 0x23FF:u2|=0x00000080;break;
        case 0x2400 ... 0x243F:u2|=0x00000100;break;
        case 0x2440 ... 0x245F:u2|=0x00000200;break;
        case 0x2460 ... 0x24FF:u2|=0x00000400;break;
        case 0x2500 ... 0x257F:u2|=0x00000800;break;
        case 0x2580 ... 0x259F:u2|=0x00001000;break;
        case 0x25A0 ... 0x25FF:u2|=0x00002000;break;
        case 0x2600 ... 0x26FF:u2|=0x00004000;break;
        case 0x2700 ... 0x27BF:u2|=0x00008000;break;
        case 0x3000 ... 0x303F:u2|=0x00010000;break;
        case 0x3040 ... 0x309F:u2|=0x00020000;break;
            //50
        case 0x30A0 ... 0x30FF:
        case 0x31F0 ... 0x31FF:u2|=0x00040000;break;
            //51
        case 0x3100 ... 0x312F:
        case 0x31A0 ... 0x31BF:u2|=0x00080000;break;
            //52-56
        case 0x3130 ... 0x318F:u2|=0x00100000;break;
        case 0xA840 ... 0xA87F:u2|=0x00200000;break;
        case 0x3200 ... 0x32FF:u2|=0x00400000;break;
        case 0x3300 ... 0x33FF:u2|=0x00800000;break;
        case 0xAC00 ... 0xD7AF:u2|=0x01000000;break;
            //57
            //case 0x10000 ... 0x10FFFF:u2|=0x02000000;break;
            //58
        case 0x10900 ... 0x1091F:u2|=0x04000000;break;
            //59
        case 0x4E00 ... 0x9FFF:
        case 0x2E80 ... 0x2EFF:
        case 0x2F00 ... 0x2FDF:
        case 0x2FF0 ... 0x2FFF:
        case 0x3400 ... 0x4DBF:
        case 0x20000 ... 0x2A6DF:
        case 0x3190 ... 0x319F:u2|=0x08000000;break;
            //60
        case 0xE000 ... 0xF8FF:u2|=0x10000000;break;
            //61
        case 0x31C0 ... 0x31EF:
        case 0xF900 ... 0xFAFF:
        case 0x2F800 ... 0x2FA1F:u2|=0x20000000;break;
            //62-64
        case 0xFB00 ... 0xFB4F:u2|=0x40000000;break;
        case 0xFB50 ... 0xFDFF:u2|=0x80000000;break;
        case 0xFE20 ... 0xFE2F:u3|=0x00000001;break;
            //65
        case 0xFE10 ... 0xFE1F:
        case 0xFE30 ... 0xFE4F:u3|=0x00000002;break;
            //66-74
        case 0xFE50 ... 0xFE6F:u3|=0x00000004;break;
        case 0xFE70 ... 0xFEFF:u3|=0x00000008;break;
        case 0xFF00 ... 0xFFEF:u3|=0x00000010;break;
        case 0xFFF0 ... 0xFFFF:u3|=0x00000020;break;
        case 0x0F00 ... 0x0FFF:u3|=0x00000040;break;
        case 0x0700 ... 0x074F:u3|=0x00000080;break;
        case 0x0780 ... 0x07BF:u3|=0x00000100;break;
        case 0x0D80 ... 0x0DFF:u3|=0x00000200;break;
        case 0x1000 ... 0x109F:u3|=0x00000400;break;
            //75
        case 0x1200 ... 0x137F:
        case 0x1380 ... 0x139F:
        case 0x2D80 ... 0x2DDF:u3|=0x00000800;break;
            //76-79
        case 0x13A0 ... 0x13FF:u3|=0x00001000;break;
        case 0x1400 ... 0x167F:u3|=0x00002000;break;
        case 0x1680 ... 0x169F:u3|=0x00004000;break;
        case 0x16A0 ... 0x16FF:u3|=0x00008000;break;
            //80
        case 0x1780 ... 0x17FF:
        case 0x19E0 ... 0x19FF:u3|=0x00010000;break;
            //81-82
        case 0x1800 ... 0x18AF:u3|=0x00020000;break;
        case 0x2800 ... 0x28FF:u3|=0x00040000;break;
            //83
        case 0xA000 ... 0xA48F:
        case 0xA490 ... 0xA4CF:u3|=0x00080000;break;
            //84
        case 0x1700 ... 0x171F:
        case 0x1720 ... 0x173F:
        case 0x1740 ... 0x175F:
        case 0x1760 ... 0x177F:u3|=0x00100000;break;
            //85Old Italic
        case 0x10300 ... 0x1032F:u3|=0x00200000;break;
            //86Gothic
        case 0x10330 ... 0x1034F:u3|=0x00400000;break;
            //87Deseret
        case 0x10400 ... 0x1044F:u3|=0x00800000;break;
            //88Byzantine Musical Symbols
        case 0x1D000 ... 0x1D0FF:
        case 0x1D100 ... 0x1D1FF:
        case 0x1D200 ... 0x1D24F:u3|=0x01000000;break;
            //89Mathematical Alphanumeric Symbols
        case 0x1D400 ... 0x1D7FF:u3|=0x02000000;break;
            //90Private Use (plane 15)
        case 0xF0000 ... 0xFFFFD:
        case 0x100000 ... 0x10FFFD:u3|=0x04000000;break;
            //91
        case 0xFE00 ... 0xFE0F:
        case 0xE0000 ... 0xE007F:u3|=0x08000000;break;
            //92
        case 0xE0100 ... 0xE01EF:u3|=0x10000000;break;
            //93-100
        case 0x1900 ... 0x194F:u3|=0x20000000;break;
        case 0x1950 ... 0x197F:u3|=0x40000000;break;
        case 0x1980 ... 0x19DF:u3|=0x80000000;break;
        case 0x1A00 ... 0x1A1F:u4|=0x00000001;break;
        case 0x2C00 ... 0x2C5F:u4|=0x00000002;break;
        case 0x2D30 ... 0x2D7F:u4|=0x00000004;break;
        case 0x4DC0 ... 0x4DFF:u4|=0x00000008;break;
        case 0xA800 ... 0xA82F:u4|=0x00000010;break;
            //101
        case 0x10000 ... 0x1007F:
        case 0x10080 ... 0x100FF:
        case 0x10100 ... 0x1013F:u4|=0x00000020;break;
            //102-109
        case 0x10140 ... 0x1018F:u4|=0x00000040;break;
        case 0x10380 ... 0x1039F:u4|=0x00000080;break;
        case 0x103A0 ... 0x103DF:u4|=0x00000100;break;
        case 0x10450 ... 0x1047F:u4|=0x00000200;break;
        case 0x10480 ... 0x104AF:u4|=0x00000400;break;
        case 0x10800 ... 0x1083F:u4|=0x00000800;break;
        case 0x10A00 ... 0x10A5F:u4|=0x00001000;break;
        case 0x1D300 ... 0x1D35F:u4|=0x00002000;break;
            //110
        case 0x12000 ... 0x123FF:
        case 0x12400 ... 0x1247F:u4|=0x00004000;break;
            //111
        case 0x1D360 ... 0x1D37F:u4|=0x00008000;break;
            //112-118
        case 0x1B80 ... 0x1BBF:u4|=0x00010000;break;
        case 0x1C00 ... 0x1C4F:u4|=0x00020000;break;
        case 0x1C50 ... 0x1C7F:u4|=0x00040000;break;
        case 0xA880 ... 0xA8DF:u4|=0x00080000;break;
        case 0xA900 ... 0xA92F:u4|=0x00100000;break;
        case 0xA930 ... 0xA95F:u4|=0x00200000;break;
        case 0xAA00 ... 0xAA5F:u4|=0x00400000;break;
            //119
        case 0x10190 ... 0x101CF:u4|=0x00800000;break;
            //120
        case 0x101D0 ... 0x101FF:u4|=0x01000000;break;
            //121
        case 0x102A0 ... 0x102DF:
        case 0x10280 ... 0x1029F:
        case 0x10920 ... 0x1093F:u4|=0x02000000;break;
            //122
        case 0x1F030 ... 0x1F09F:
        case 0x1F000 ... 0x1F02F:u4|=0x04000000;break;
        }
    }
}


