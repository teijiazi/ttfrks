#include "tableComm.h"

////////////////////////////////////////////////
///  LangSysTable
///
///

LangSysTable::LangSysTable():
    lookupOrderOffset(0),
    requiredFeatureIndex(0xFFFF),
    featureIndexCount(0){}
LangSysTable::~LangSysTable(){

}
void LangSysTable::show(QString&info){
    info.append(QString("    [2B]lookupOrderOffset:%1\r\n"
                        "    [2B]requiredFeatureIndex=%2\r\n"
                        "    [2B]featureIndexCount=%3\r\n")
                .arg(lookupOrderOffset)
                .arg(requiredFeatureIndex)
                .arg(featureIndexCount));
    if(featureIndexCount>0){
        info.append(QString("    [%1B]pFeatureIndices:")
                    .arg(sizeof(uint16)*featureIndexCount));
        for(int i=0;i<featureIndexCount;i++){
            info.append(QString("%1 ").arg(featureIndices[i]));
        }
        info.append("\r\n");
    }
}

void LangSysTable::addFeatureIndex(int featureIndexArr){
    featureIndexCount+=1;
    featureIndices.append(featureIndexArr);
    sort(featureIndices.begin(),featureIndices.end());
}

void LangSysTable::gengxinFeatrueIndex(int featureIndexBegin)
{
    for(int i=0;i<this->featureIndices.length();i++){
        if(featureIndices[i]>=featureIndexBegin){
            featureIndices[i]+=1;
        }
    }
}

void LangSysTable::delFeatureIndex(const u16vector &featureIndexDeleted){
    //由于功能不存在而删除功能索引
    for(int i=0;i<featureIndexDeleted.length();i++){
        bool sc=featureIndices.removeOne(featureIndexDeleted[i]);
        if(sc){
            featureIndexCount-=1;
        }
    }
    //全部删除完成后，后续递减
    for(int fi=0;fi<featureIndices.length();fi++){
        for(int i=0;i<featureIndexDeleted.length();i++){
            if(featureIndices[fi]>featureIndexDeleted[i]){
                featureIndices[fi]-=1;
            }
        }
    }
}

int LangSysTable::byteLength()
{
    return sizeof(uint16)*(3+featureIndexCount);
}
void LangSysTable::read(QDataStream&dt){
    dt>>lookupOrderOffset>>requiredFeatureIndex>>featureIndexCount;
    if(featureIndexCount>0){
        featureIndices.fill(0,featureIndexCount);
        for(int i=0;i<featureIndexCount;i++){
            dt>>featureIndices[i];
        }
    }
}
void LangSysTable::save(QDataStream&dt){
    dt<<lookupOrderOffset<<requiredFeatureIndex<<featureIndexCount;
    for(int i=0;i<featureIndexCount;i++){
        dt<<featureIndices[i];
    }
}

void LangSysRecord::show(QString&info){
    QString langTagStr=[](const QString& langTag)->QString{
        QList<pair<QString,QString> > ydy={
            {"ZHH ","繁体中文(香港)"},
            {"ZHP ","中文语音"},
            {"ZHS ","简体中文"},
            {"ZHT ","繁体中文"},
            {"ZHTM","繁体中文(澳门)"},
        };
        for(auto i=ydy.begin();i!=ydy.end();i++){
            if(i->first==langTag){
                return i->second;
            }
        }
        return "";
    }(langSysTag);
    info.append(QString("    [4B]langSysTag:%1\t\t%3\r\n"
                        "    [2B]langSysOffset=%2\r\n")
                .arg(langSysTag)
                .arg(langSysOffset)
                .arg(langTagStr));
}

int LangSysRecord::byteLength()
{
    return 4+sizeof(uint16);
}
void LangSysRecord::read(QDataStream&dt){
    dt>>langSysTag.data>>langSysOffset;
}
void LangSysRecord::save(QDataStream&dt){
    dt<<langSysTag.data<<langSysOffset;
}

ScriptTable::ScriptTable():defaultLangSysOffset(0),langSysCount(0),fzDefChongfu(false){}
ScriptTable::~ScriptTable(){

}

void ScriptTable::show(QString&info){
    info.append(QString("  [2B]defaultLangSysOffset:%1\t\t[0=没有]\r\n"
                        "  [2B]langSysCount=%2\r\n")
                .arg(defaultLangSysOffset)
                .arg(langSysCount));
    info.append("  LangSysRecord\r\n");
    for(int i=0;i<langSysCount;i++){
        lstLangSysRecord[i].show(info);
    }
    if(defaultLangSysOffset){
        info.append(QString("  LangSysTable Default\r\n"));
        defLangSysTable.show(info);
    }
    for(uint i=0;i<langSysCount;i++){
        info.append(QString("  LangSysTable%1\r\n").arg(i));
        lstLangSysTable[i].show(info);
    }
}

void ScriptTable::addDefYuyan(int featureIndexArr){
    //增加默认语言，不增加其他语言
    fzDefChongfu=false;
    defaultLangSysOffset=4;
    langSysCount=0;
    defLangSysTable.addFeatureIndex(featureIndexArr);
}

int ScriptTable::byteLength()
{
    //语言记录的长度
    int len=2*sizeof(uint16)+langSysCount*LangSysRecord::byteLength();
    //默认语言表的长度  read✓ 标记是否重复，adddef✓,add× 标记是否重复 删×改× 标记是否重复,save× 有且重复，不写。
    if(defaultLangSysOffset&& (!fzDefChongfu)){
        len+=defLangSysTable.byteLength();
    }
    //语言表的长度
    for(int i=0;i<langSysCount;i++){
        len+=lstLangSysTable[i].byteLength();
    }
    return len;
}
void ScriptTable::read(QDataStream&dt){
    uint64 posScriptT=dt.device()->pos();
    dt>>defaultLangSysOffset>>langSysCount;

    for(int i=0;i<langSysCount;i++){
        LangSysRecord lsr;
        lsr.read(dt);
        if(defaultLangSysOffset==lsr.langSysOffset){
            this->fzDefChongfu=true;
        }
        lstLangSysRecord.append(lsr);
    }
    if(defaultLangSysOffset){
        dt.device()->seek(posScriptT+defaultLangSysOffset);
        defLangSysTable.read(dt);
    }

    for(uint i=0;i<langSysCount;i++){
        dt.device()->seek(posScriptT+lstLangSysRecord[i].langSysOffset);
        LangSysTable lst;
        lst.read(dt);
        lstLangSysTable.append(lst);
    }
}
void ScriptTable::save(QDataStream&dt){
    dt<<defaultLangSysOffset<<langSysCount;
    for(int i=0;i<langSysCount;i++){
        lstLangSysRecord[i].save(dt);
    }
    if(defaultLangSysOffset && (!fzDefChongfu)){
        //如果默认语言表与lstLangSysTable中的表相同，则不存储。
        defLangSysTable.save(dt);
    }
    for(uint i=0;i<langSysCount;i++){
        lstLangSysTable[i].save(dt);
    }
}

void ScriptRecord::show(QString&info){
    QString scriptTagStr=[](const QString& scriptTag)->QString{
        QList<pair<QString,QString> > ydy={
            {"latn","拉丁语"},
            {"hani","CJK 表意文字"},
        };
        for(auto i=ydy.begin();i!=ydy.end();i++){
            if(i->first==scriptTag){
                return i->second;
            }
        }
        return "";
    }(scriptTag);
    info.append(QString("  [4B]scriptTag:%1\t\t%3\r\n"
                        "  [2B]scriptOffset=%2\r\n")
                .arg(scriptTag)
                .arg(scriptOffset)
                .arg(scriptTagStr));
}

int ScriptRecord::byteLength()
{
    return 4+sizeof(Offset16);
}
void ScriptRecord::read(QDataStream&dt){
    dt>>scriptTag.data>>scriptOffset;
}
void ScriptRecord::save(QDataStream&dt){
    dt<<scriptTag.data<<scriptOffset;
}


ScriptListTable::ScriptListTable():scriptCount(0){}
ScriptListTable::~ScriptListTable(){
    for(int i=0;i<lstScriptTable.length();i++){
        delete lstScriptTable[i];
        delete lstScriptRecord[i];
    }
}
void ScriptListTable::show(QString&info,TableTTF* ptb){
    info.append(QString("[2B]scriptCount:%1\r\n").arg(scriptCount));
    if(scriptCount>0){
        info.append("ScriptRecord:\r\n");
        for(int i=0;i<scriptCount;i++){
            lstScriptRecord[i]->show(info);
        }
        //根据ScriptRecord，读ScriptTable
        for(int i=0;i<scriptCount;i++){
            info.append(QString("ScriptTable %1:\r\n").arg(i+1));
            lstScriptTable[i]->show(info);
            ptb->jindu(i/scriptCount);
        }
    }
}
void ScriptListTable::getJiaoben(QStringList &jbLst){
    //列出脚本
    jbLst.clear();
    for(int i=0;i<scriptCount;i++){
        jbLst.append(lstScriptRecord[i]->scriptTag);
    }
}
void ScriptListTable::getYuyan(QStringList&yyLst, int i, bool &youDef){
    //列出脚本对应的语言
    //i 脚本索引
    youDef=lstScriptTable[i]->defaultLangSysOffset;
    ListLangSysRecord& pLsr=lstScriptTable[i]->lstLangSysRecord;
    for(uint li=0;li<lstScriptTable[i]->langSysCount;li++){
        yyLst.append(pLsr[li].langSysTag);
    }
}

void ScriptListTable::getFeatureArr(u16vector &featureIndices,
                                    int ji,int li){
    //ji 脚本索引 li 语言索引
    //根据脚本和语言，返回功能列表
    if(li!=-1){//指定语言
        ListLangSysTable &pLst=lstScriptTable[ji]->lstLangSysTable;
        featureIndices=pLst[li].featureIndices;
    }else{//默认语言
        LangSysTable& pLst=lstScriptTable[ji]->defLangSysTable;
        featureIndices=pLst.featureIndices;
    }
}

void ScriptListTable::read(QDataStream& dt, TableTTF* ptb){
    uint64 posSL=dt.device()->pos();
    dt>>scriptCount;
    if(scriptCount>0){
        for(int i=0;i<scriptCount;i++){
            ScriptRecord *sr=new ScriptRecord;
            sr->read(dt);
            lstScriptRecord.append(sr);
        }
        //根据ScriptRecord，读ScriptTable
        for(int i=0;i<scriptCount;i++){
            dt.device()->seek(posSL+lstScriptRecord[i]->scriptOffset);
            ScriptTable *st=new ScriptTable;
            st->read(dt);
            lstScriptTable.append(st);
            ptb->jindu(i/scriptCount);
        }
    }
}
void ScriptListTable::save(QDataStream& dt){
    dt<<scriptCount;
    for(int i=0;i<scriptCount;i++){
        lstScriptRecord[i]->save(dt);
    }
    for(int i=0;i<scriptCount;i++){
        lstScriptTable[i]->save(dt);
    }
}

void ScriptListTable::xinjian1(QString tag)
{
    Q_UNUSED(tag);
}

bool ScriptListTable::jiaobenYouxiao(const QString& jiaoben){
    if(jiaoben==""){
        return false;
    }
    //防止重复
    for(int i=0;i<lstScriptRecord.length();i++){
        if(jiaoben==lstScriptRecord[i]->scriptTag){
            return false;
        }
    }
    return true;
}

int ScriptListTable::byteLength()
{
    if(scriptCount==0){
        return sizeof(uint16);
    }else{
        return lstScriptRecord.last()->scriptOffset+lstScriptTable.last()->byteLength();
    }
}

void ScriptListTable::delFeatureIndex(const u16vector &featureIndexDeleted){
    //脚本表中删除无效的功能索引，更新其他功能索引，删除无功能的语言表，删除无语言表的脚本。
    for(int i=0;i<lstScriptTable.length();i++){
        for(int fi=0;i<lstScriptTable[i]->lstLangSysTable.length();fi++){
            lstScriptTable[i]->lstLangSysTable[fi].delFeatureIndex(featureIndexDeleted);
            //删除无功能的语言表
            if(lstScriptTable[i]->lstLangSysTable[fi].featureIndexCount==0){
                lstScriptTable[i]->lstLangSysTable.removeAt(fi);
                lstScriptTable[i]->lstLangSysRecord.removeAt(fi);
                lstScriptTable[i]->langSysCount-=1;
                fi--;
            }
        }
        {
            lstScriptTable[i]->defLangSysTable.delFeatureIndex(featureIndexDeleted);
            //删除无功能的语言表
            if(lstScriptTable[i]->defLangSysTable.featureIndexCount==0){
                lstScriptTable[i]->defaultLangSysOffset=0;//无默认语言
            }
        }
        //删除无语言表的脚本。
        if(lstScriptTable[i]->defaultLangSysOffset==0 &&
                lstScriptTable[i]->langSysCount==0){
            lstScriptTable.removeAt(i);
            lstScriptRecord.removeAt(i);
            scriptCount-=1;
            i--;
        }
    }

    //修复偏移
    xiufuPianyi();
}

void ScriptListTable::gengxinFeatrueIndex(int featureIndexBegin){
    //featureListTable 中插入新功能后，调整脚本列表中已经登记的功能索引
    //>=featureIndexBegin,+=1
    for(int i=0;i<lstScriptTable.length();i++){
        for(int fi=0;i<lstScriptTable[i]->lstLangSysTable.length();fi++){
            lstScriptTable[i]->lstLangSysTable[fi].gengxinFeatrueIndex(featureIndexBegin);
        }
        if(lstScriptTable[i]->defaultLangSysOffset){
            lstScriptTable[i]->defLangSysTable.gengxinFeatrueIndex(featureIndexBegin);
        }
    }
}

void ScriptListTable::insertFeatureIndex(int jiaobenIndex, int yuyanIndex, int featureIndex){
    //指定语言 指定脚本 ，把功能index插入对应的语言表中
    //假设默认语言表存在。
    if(yuyanIndex==-1){
        lstScriptTable[jiaobenIndex]->defLangSysTable.addFeatureIndex(featureIndex);
    }else{
        lstScriptTable[jiaobenIndex]->lstLangSysTable[yuyanIndex].addFeatureIndex(featureIndex);
    }
    xiufuPianyi();
}

int ScriptListTable::addJiaobenYuyanDef(const QString& jiaoben, int featureIndex){
    //新增脚本+默认语言，脚本不能重复
    //返回新增脚本的插入位置
    ScriptRecord *sr=new ScriptRecord;
    sr->scriptTag=jiaoben;
    sr->scriptOffset=0;//重新计算前设置为0.
    ScriptTable *st=new ScriptTable;
    st->addDefYuyan(featureIndex);
    //record插入--按顺序
    int frPos=-1;
    for(int i=0;i<lstScriptRecord.length();i++){
        if(jiaoben<lstScriptRecord[i]->scriptTag){
            frPos=i;
            lstScriptTable.insert(frPos,st); //table插入同样的位置
            lstScriptRecord.insert(frPos,sr);
            break;
        }
    }
    if(frPos==-1){
        lstScriptRecord.append(sr);
        lstScriptTable.append(st);
        frPos=lstScriptRecord.length()-1;
    }
    scriptCount+=1;
    //重写偏移
    xiufuPianyi();
    return frPos;
}

void ScriptListTable::xiufuPianyi(){
    int off=sizeof(uint16)+scriptCount*ScriptRecord::byteLength();
    for(int i=0;i<lstScriptRecord.length();i++){
        lstScriptRecord[i]->scriptOffset=off;
        off+=lstScriptTable[i]->byteLength();
    }
}
///////////////////////////////////////////////////////
///
///
///

FeatureTable::FeatureTable():featureParamsOffset(0){}
FeatureTable::~FeatureTable(){

}
void FeatureTable::show(QString&info){
    info.append(QString("[2B]featureParamsOffset:%1,[2B]lookupIndexCount=%2")
                .arg(featureParamsOffset==0?"NULL":QString::number(featureParamsOffset)).arg(lookupIndexCount));
    if(lookupIndexCount>0){
        info.append(QString("  [%1B]pLookupListIndices:").arg(lookupIndexCount*2));
        for(int i=0;i<lookupIndexCount;i++){
            info.append(QString("%1 ").arg(lookupListIndices[i]));
        }
        info.append("\r\n");
    }
}

int FeatureTable::byteLength()
{
    return (lookupIndexCount+2)*sizeof(uint16);
}
void FeatureTable::read(QDataStream& dt){
    dt>>featureParamsOffset>>lookupIndexCount;
    if(lookupIndexCount>0){
        lookupListIndices.fill(0,lookupIndexCount);
        for(int i=0;i<lookupIndexCount;i++){
            dt>>lookupListIndices[i];
        }
    }
}
void FeatureTable::save(QDataStream& dt){
    dt<<featureParamsOffset<<lookupIndexCount;
    for(int i=0;i<lookupIndexCount;i++){
        dt<<lookupListIndices[i];
    }
}

void FeatureTable::delLookupIndex(int lookupIndex){
    //删除lookup索引lookupIndex，大于lookupIndex的其他lookup索引递减
    bool sc=lookupListIndices.removeOne(lookupIndex);
    if(sc){
        lookupIndexCount-=1;
        for(int i=0;i<lookupListIndices.length();i++){
            if(lookupListIndices[i]>lookupIndex){
                lookupListIndices[i]-=1;
            }
        }
    }
}

void FeatureRecord::read(QDataStream&dt){
    dt>>featureTag.data>>featureOffset;
}
void FeatureRecord::show(QString&info){
    info.append(QString("%1,%2\r\n").arg(featureTag).arg(featureOffset));
}

int FeatureRecord::byteLength()
{
    return 4+sizeof (Offset16);
}
void FeatureRecord::save(QDataStream&dt){
    dt<<featureTag.data<<featureOffset;
}


FeatureListTable::FeatureListTable():featureCount(0){}
FeatureListTable::~FeatureListTable(){

}
void FeatureListTable::show(QString&info){
    info.append(QString("[2B]featureCount:%1\r\n").arg(featureCount));
    if(featureCount>0){
        info.append(QString("[%1B]FeatureRecord[featureTag featureOffset]\r\n")
                    .arg(featureCount*FeatureRecord::byteLength()));
        for(int i=0;i<featureCount;i++){
            info.append(QString("  %1:").arg(i));
            lstFeatureRecord[i].show(info);
        }
        info.append("FeatureTable\r\n");
        for(int i=0;i<featureCount;i++){
            info.append(QString("  %1:").arg(i));
            lstFeatureTable[i].show(info);
        }
    }
}

int FeatureListTable::addFeature(const QString& tag,int lookupIndex){
    //返回新增功能在功能列表中的索引
    featureCount+=1;
    //新纪录
    FeatureRecord fr;
    fr.featureTag=tag;
    fr.featureOffset=0;//重新计算前设置为0.
    FeatureTable ft;
    ft.lookupIndexCount=1;
    ft.lookupListIndices.append(lookupIndex);
    //record插入--按顺序
    int frPos=-1;
    for(int i=0;i<lstFeatureRecord.length();i++){
        if(tag<lstFeatureRecord[i].featureTag){
            frPos=i;
            lstFeatureRecord.insert(frPos,fr);
            lstFeatureTable.insert(frPos,ft); //table插入同样的位置
            break;
        }
    }
    if(frPos==-1){
        lstFeatureRecord.append(fr);
        lstFeatureTable.append(ft);
        //更新索引用于返回数据
        frPos=lstFeatureRecord.length()-1;
    }
    //重写偏移
    xiufuPianyi();
    return frPos;
}

void FeatureListTable::xiufuPianyi(){
    int off=sizeof(uint16)+featureCount*FeatureRecord::byteLength();
    for(int i=0;i<featureCount;i++){
        lstFeatureRecord[i].featureOffset=off;
        off+=lstFeatureTable[i].byteLength();
    }
}

int FeatureListTable::byteLength()
{
    if(featureCount==0){
        return sizeof(uint16);
    }else{
        return lstFeatureRecord.last().featureOffset+lstFeatureTable.last().byteLength();
    }
}

void FeatureListTable::delLookupIndex(int lookupIndex, u16vector&featureIndexDeleted){
    //遍历功能表删除lookup索引lookupIndex
    for(int i=0;i<lstFeatureTable.length();i++){
        lstFeatureTable[i].delLookupIndex(lookupIndex);
        //删除后，功能里面无lookup索引：把此功能的索引存入featureIndexDeleted
        if(lstFeatureTable[i].lookupIndexCount==0){
            featureIndexDeleted.append(i);
        }
    }
    //遍历结束后，删除featureIndexDeleted中的功能表,修复偏移
    for(int i=0;i<featureIndexDeleted.length();i++){
        lstFeatureTable.removeAt(featureIndexDeleted[i]);
        lstFeatureRecord.removeAt(featureIndexDeleted[i]);
        this->featureCount-=1;
    }
    xiufuPianyi();
}

void FeatureListTable::read(QDataStream& dt){
    uint64 posFL=dt.device()->pos();
    dt>>featureCount;
    if(featureCount>0){
        for(int i=0;i<featureCount;i++){
            FeatureRecord fr;
            fr.read(dt);
            lstFeatureRecord.append(fr);
        }
        for(int i=0;i<featureCount;i++){
            dt.device()->seek(posFL+lstFeatureRecord[i].featureOffset);
            FeatureTable ft;
            ft.read(dt);
            lstFeatureTable.append(ft);
        }
    }
}
void FeatureListTable::save(QDataStream&dt){
    dt<<featureCount;
    for(int i=0;i<featureCount;i++){
        lstFeatureRecord[i].save(dt);
    }
    for(int i=0;i<featureCount;i++){
        lstFeatureTable[i].save(dt);
    }
}

/////////////////////////////////////////////////////////
///
///
LookupTable::LookupTable(){

}
LookupTable::~LookupTable(){
    for(int i=0;i<lstPSubtable.length();i++){
        delete lstPSubtable[i];
    }
}
void LookupTable::read(QDataStream&dt, tbType tbt, TableTTF* ptb){
    //返回子表数量，用来确定后面是否还有子表
    uint64 posLk=dt.device()->pos();
    dt>>lookupType>>lookupFlag>>subTableCount;
    if(subTableCount>0){
        subtableOffsets.fill(0,subTableCount);
        for(int i=0;i<subTableCount;i++){
            dt>>subtableOffsets[i];
        }
    }
    if(lookupFlag&USE_MARK_FILTERING_SET){
        dt>>markFilteringSet;
    }
    if(subTableCount>0){
        for(int i=0;i<subTableCount;i++){
            dt.device()->seek(posLk+subtableOffsets[i]);//多个部分按偏移定位。
            uint16	fmtid;
            dt>>fmtid;
            if(tbt==GPOS){
                Subtable* pGposSub=nullptr;
                switch(lookupType){
                case 2:
                    if(fmtid==1){
                        pGposSub=new PairPosFormat1;
                    }else if(fmtid==2){
                        pGposSub=new PairPosFormat2;
                    }
                    break;
                case 4:
                    pGposSub=new MarkBasePosFormat1;
                    break;
                case 8:
                    if(fmtid==1){
                        pGposSub=new ChainedContextsPositioningFormat1;
                    }else if(fmtid==2){
                        pGposSub=new ChainedContextsPositioningFormat2;
                    }else if(fmtid==3){
                        pGposSub=new ChainedContextsPositioningFormat3;
                    }
                    break;
                case 9:
                    if(fmtid==1){
                        pGposSub=new ExtensionPosFormat1;
                    }
                    break;
                default:
                    ptb->datashow(QString("GPOS 子表缺少定义，子表lookupType=%1").arg(lookupType));
                    break;
                }
                if(pGposSub){
                    pGposSub->read(dt);
                    lstPSubtable.append(pGposSub);
                }
            }else if(tbt==GSUB){
                Subtable* pGsubSub=nullptr;
                switch(lookupType){
                case 1:
                    if(fmtid==1){
                        pGsubSub=new SingleSubstFormat1;
                    }else if(fmtid==2){
                        pGsubSub=new SingleSubstFormat2;
                    }
                    break;
                case 2:pGsubSub=new MultipleSubstFormat1;break;
                case 3:pGsubSub=new AlternateSubstFormat1;break;
                case 4:pGsubSub=new LigatureSubstFormat1;break;
                case 7:pGsubSub=new ExtensionSubstFormat1;break;
                default:
                    ptb->datashow(QString("GSUB 子表缺少定义，子表lookupType=%1").arg(lookupType));
                    break;
                }
                if(pGsubSub){
                    pGsubSub->read(dt);
                    lstPSubtable.append(pGsubSub);
                }
            }
        }
    }
}
void LookupTable::show(QString&info, tbType tbt){
    //返回子表数量，用来确定后面是否还有子表
    info.append(QString("  [2B]lookupType:%1(%4)\r\n  [2B]lookupFlag=%2\r\n  [2B]subTableCount:%3\r\n")
                .arg(lookupType).arg(lookupFlag).arg(subTableCount)
                .arg(tbt==GSUB?gsubSubtableType[lookupType]:gposSubtableType[lookupType]));
    if(subTableCount>0){
        info.append(QString("  [%1B]pSubtableOffsets:").arg(subTableCount*sizeof(Offset16)));
        for(int i=0;i<subTableCount;i++){
            info.append(QString("%1 ").arg(subtableOffsets[i]));
        }
        info.append("\r\n");
    }
    if(lookupFlag&USE_MARK_FILTERING_SET){
        info.append(QString("markFilteringSet:%1\r\n").arg(markFilteringSet));
    }
    if(subTableCount>0){
        for(int i=0;i<this->lstPSubtable.length();i++){
            info.append(QString("subtable %1\r\n").arg(i+1));
            lstPSubtable[i]->show(info);
        }
    }
}

int LookupTable::byteLength()
{
    int len=sizeof(uint16)*(3+subTableCount);
    len+=(lookupFlag&USE_MARK_FILTERING_SET)?2:0;
    for(int i=0;i<lstPSubtable.length();i++){
        len+=lstPSubtable[i]->byteLength();
    }
    return len;
}
void LookupTable::save(QDataStream&dt){
    dt<<lookupType<<lookupFlag<<subTableCount;
    for(int i=0;i<subTableCount;i++){
        dt<<subtableOffsets[i];
    }
    if(lookupFlag&USE_MARK_FILTERING_SET){
        dt<<markFilteringSet;
    }
    for(int i=0;i<subTableCount;i++){
        lstPSubtable[i]->save(dt);
    }
}


LookupListTable::LookupListTable():lookupCount(0){}
LookupListTable::~LookupListTable(){
    for(int i=0;i<lstPLookupTable.length();i++){
        delete lstPLookupTable[i];
    }
}
void LookupListTable::show(QString&info,tbType tbt, TableTTF*ptb){
    Q_UNUSED(ptb);
    info.append(QString("[2B]lookupCount:%1\r\n").arg(lookupCount));
    if(lookupCount>0){
        info.append(QString("[%1B]pLookupOffsets:").arg(lookupCount*sizeof(Offset16)));
        for(int i=0;i<lookupCount;i++){
            info.append(QString("%1 ").arg(lookupOffsets[i]));
        }
        for(int i=0;i<lookupCount;i++){
            info.append(QString("\r\nLookupTable %1\r\n").arg(i));
            lstPLookupTable[i]->show(info,tbt);
        }
    }
}
void LookupListTable::read(QDataStream&dt, tbType tbt, TableTTF*ptb){
    uint64 posLookList=dt.device()->pos();
    dt>>lookupCount;
    if(lookupCount>0){
        lookupOffsets.fill(0,lookupCount);
        for(int i=0;i<lookupCount;i++){
            dt>>lookupOffsets[i];
        }
        //
        for(int i=0;i<lookupCount;i++){
            //先定位在读取
            dt.device()->seek(posLookList+lookupOffsets[i]);
            LookupTable *lt=new LookupTable;
            lt->read(dt,tbt,ptb);
            lstPLookupTable.append(lt);
        }
    }
}
void LookupListTable::save(QDataStream&dt){
    dt<<lookupCount;
    if(lookupCount>0){
        for(int i=0;i<lookupCount;i++){
            dt<<lookupOffsets[i];
        }
        for(int i=0;i<lookupCount;i++){
            lstPLookupTable[i]->save(dt);
        }
    }
}
void LookupListTable::getLigaZibiao(int16 looklistIndex, uint16& zibiaoShuliang){
    LookupTable *plt=lstPLookupTable[looklistIndex];
    //获取子表数量
    zibiaoShuliang=plt->lstPSubtable.length();
}
void LookupListTable::getLigaCover(int16 looklistIndex, int16 zibiaoIndex,u16vector& cover){
    //获取cover
    //looklistIndex 找到lookupTable
    //zibiaoIndex 找到子表
    //cover 返回cover
    LookupTable *plt=lstPLookupTable[looklistIndex];
    cover.clear();
    LigatureSubstFormat1*pLigSub=dynamic_cast<LigatureSubstFormat1*>(plt->lstPSubtable[zibiaoIndex]);
    if(pLigSub && pLigSub->pCover){
        pLigSub->pCover->getCoverGlyphIdx(cover);
    }
}
void LookupListTable::getLigaZifu(int16 looklistIndex, int zibiaoIndex,int coverIndex,
                                  QList<u16vector>&shuruGlyList,u16vector& shuchuGlyphIdx){
    LookupTable *plt=lstPLookupTable[looklistIndex];
    LigatureSubstFormat1*pLigSub=dynamic_cast<LigatureSubstFormat1*>(plt->lstPSubtable[zibiaoIndex]);
    if(pLigSub && pLigSub->pCover){
        uint16 diyizifu=pLigSub->pCover->coverIndex2GlyphIndex(coverIndex);
        LigatureSetTable& lst=pLigSub->lstLigSet[coverIndex];
        for(int ti=0;ti<lst.ligatureCount;ti++){//读出每个ligatureTable
            shuchuGlyphIdx.append(lst.lstLigatureTable[ti].ligatureGlyph);
            u16vector shuru;
            shuru.append(diyizifu);
            shuru.append(lst.lstLigatureTable[ti].componentGlyphIDs);
            shuruGlyList.append(shuru);
        }
    }
}
int LookupListTable::addVert(const u16vector&heng,const u16vector &shu){
    /// 新建lookup表，并新建一个子表为vert。
    /// 用heng shu填充vert子表
    /// 返回lookupTable的索引
    LookupTable *plt=new LookupTable;
    plt->lookupType=1;
    plt->lookupFlag=0;
    plt->subTableCount=1;
    plt->subtableOffsets.append(8);
    Subtable* pGsubSub=new SingleSubstFormat2;
    SingleSubstFormat2*pSingleSub=static_cast<SingleSubstFormat2*>(pGsubSub);
    pSingleSub->setVert(heng,shu);
    plt->lstPSubtable.append(pGsubSub);
    //
    this->lookupCount+=1;
    lstPLookupTable.append(plt);
    //重写偏移
    xiufuPianyi();
    return lookupCount-1;
}
void LookupListTable::tihuanVert(int lookupIndex, const u16vector&heng,const u16vector &shu){
    LookupTable *plt=lstPLookupTable[lookupIndex];
    SingleSubstFormat2*pSub=dynamic_cast<SingleSubstFormat2*>(plt->lstPSubtable[0]);
    if(pSub){
        pSub->setVert(heng,shu);
    }
}
int LookupListTable::addLiga(const u16vector& shuru,int& jgCoverIdx,int& jgShuruIdx){
    /// 新建lookup表，并新建一个子表为liga。
    /// 用输入填充liga子表
    /// 返回lookupTable的索引
    LookupTable *plt=new LookupTable;
    plt->lookupType=4;
    plt->lookupFlag=0;
    plt->subTableCount=1;
    plt->subtableOffsets.append(8);
    Subtable* pGsubSub=new LigatureSubstFormat1;
    LigatureSubstFormat1*pLigSub=static_cast<LigatureSubstFormat1*>(pGsubSub);
    pLigSub->set(shuru,jgCoverIdx,jgShuruIdx);
    plt->lstPSubtable.append(pGsubSub);
    //
    this->lookupCount+=1;
    lstPLookupTable.append(plt);
    //重写偏移
    xiufuPianyi();
    return lookupCount-1;
}
void LookupListTable::insertLiga(int lookupTableIndex,int zibiaoIndex,const u16vector& shuru,
                                 int& jgCoverIdx,int& jgShuruIdx){
    LookupTable *plt=lstPLookupTable[lookupTableIndex];
    LigatureSubstFormat1*pLigSub=dynamic_cast<LigatureSubstFormat1*>(plt->lstPSubtable[zibiaoIndex]);
    if(pLigSub){
        pLigSub->set(shuru,jgCoverIdx,jgShuruIdx);
    }
}
void LookupListTable::xiufuPianyi(){
    lookupOffsets.clear();
    int off=sizeof(uint16)*(1+lookupCount);
    for(int i=0;i<lstPLookupTable.length();i++){
        lookupOffsets.append(off);
        off+=lstPLookupTable[i]->byteLength();
    }
}
void LookupListTable::gengxinLiga(int lookupTableIndex,int zibiaoIndex,
                                  int coverIndex,int li,const uint16 shuchu){
    //li 子表中ligaTable的索引
    //更新输出
    LookupTable *plt=lstPLookupTable[lookupTableIndex];
    LigatureSubstFormat1*pLigSub=dynamic_cast<LigatureSubstFormat1*>(plt->lstPSubtable[zibiaoIndex]);
    if(pLigSub){
        LigatureSetTable& lst=pLigSub->lstLigSet[coverIndex];
        lst.lstLigatureTable[li].ligatureGlyph=shuchu;
    }
}

void LookupListTable::shanchuLiga(int& lookupTableIndex,int& zibiaoIndex,
                                  int& coverIdx,int& shuruIdx){
    LookupTable *plt=lstPLookupTable[lookupTableIndex];
    LigatureSubstFormat1*pLigSub=dynamic_cast<LigatureSubstFormat1*>(plt->lstPSubtable[zibiaoIndex]);
    if(pLigSub){
        pLigSub->shanchu(coverIdx,shuruIdx);
        if(pLigSub->ligatureSetCount==0){
            plt->subTableCount-=1;
            delete plt->lstPSubtable[zibiaoIndex];
            plt->lstPSubtable.removeAt(zibiaoIndex);
            zibiaoIndex=-1;
            //plt->xiufuPianyi();默认只有一个子表
            if(plt->subTableCount==0){
                this->lookupCount-=1;
                delete lstPLookupTable[lookupTableIndex];
                lstPLookupTable.removeAt(lookupTableIndex);
                xiufuPianyi();
                lookupTableIndex=-1;
            }
        }
    }
}

