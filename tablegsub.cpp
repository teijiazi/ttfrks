#include "tablegsub.h"

GSUBTable::GSUBTable(QObject *parent) :
    TableTTF(parent),majorVersion(1),minorVersion(0),
    scriptListOffset(0),featureListOffset(0),lookupListOffset(0),
    scriptListTable(nullptr),featureListTable(nullptr),lookupListTable(nullptr)
{

}
GSUBTable::~GSUBTable(){
    reset();
}
void GSUBTable::reset(){
    if(scriptListTable){
        delete scriptListTable;
    }
    if(featureListTable){
        delete featureListTable;
    }
    if(lookupListTable){
        delete lookupListTable;
    }
}
void GSUBTable::read(QDataStream& dt, uint32 offset){
    dt.device()->seek(offset);
    dt>>majorVersion>>minorVersion>>scriptListOffset>>featureListOffset>>lookupListOffset;
    if(majorVersion==1&&minorVersion==1){
        dt>>featureVariationsOffset;
    }
    dt.device()->seek(offset+scriptListOffset);
    scriptListTable=new ScriptListTable;
    scriptListTable->read(dt,this);
    dt.device()->seek(offset+featureListOffset);
    featureListTable=new FeatureListTable;
    featureListTable->read(dt);
    dt.device()->seek(offset+lookupListOffset);
    lookupListTable=new LookupListTable;
    lookupListTable->read(dt,GSUB,this);
    ////ver 1.1
    ////FeatureVariations

};

void GSUBTable::jisuan(){
    //
    if(!scriptListTable){
        scriptListTable=new ScriptListTable;
    }
    if(!featureListTable){
        featureListTable=new FeatureListTable;
    }
    if(!lookupListTable){
        lookupListTable=new LookupListTable;
    }
    if(scriptListTable && featureListTable && lookupListTable){
        scriptListOffset=10;
        featureListOffset=scriptListOffset+scriptListTable->byteLength();
        lookupListOffset=featureListOffset+featureListTable->byteLength();
    }
}

void GSUBTable::save(QDataStream& dt){
    if(scriptListTable && featureListTable && lookupListTable){
        dt<<majorVersion<<minorVersion<<scriptListOffset<<featureListOffset<<lookupListOffset;
        if(majorVersion==1&&minorVersion==1){
            dt<<featureVariationsOffset;
        }
        scriptListTable->save(dt);
        featureListTable->save(dt);
        lookupListTable->save(dt);
    }
}
int GSUBTable::addLoopupVert(const QString& jiaoben,const u16vector&heng,const u16vector &shu){
    //返回新增脚本的插入位置
    //新增一组(脚本+语言+功能+查询)
    if(!scriptListTable){
        scriptListTable=new ScriptListTable;
    }
    if(!featureListTable){
        featureListTable=new FeatureListTable;
    }
    if(!lookupListTable){
        lookupListTable=new LookupListTable;
    }
    int lookupIndex=lookupListTable->addVert(heng,shu);
    int featureIndex=featureListTable->addFeature("vert",lookupIndex);
    //插入功能以后，已经存储在脚本中的功能索引需要更新
    //featureIndex以后的都需要加1
    scriptListTable->gengxinFeatrueIndex(featureIndex);
    int jiaobenIdx=scriptListTable->addJiaobenYuyanDef(jiaoben,featureIndex);
    return jiaobenIdx;
}
void GSUBTable::tihuanVert(int jiaobenIndex, const u16vector&heng,const u16vector &shu){
    //指定脚本和语言default，如果没有vert,则增加vert。如果已有vert,则替换。
    ///调用前提：
    ///脚本+语言确定的时候，如果有vert，则只有一个vert功能，对应只有一个查询表。
    /// 一个查询表有多个子表（目前只有一个），一个子表有一个（替换+cover）
    const ListFeatureRecord& pfr=featureListTable->lstFeatureRecord;
    const ListFeatureTable& pft=featureListTable->lstFeatureTable;
    u16vector fi;
    scriptListTable->getFeatureArr(fi,jiaobenIndex, -1);
    bool insertSuc=false;
    //先查（指定脚本+指定语言）有没有vert功能
    for(int i=0;i<fi.length();i++){
        if(pfr[fi[i]].featureTag=="vert"){//已有vert，插入到现有liga中
            //一个feature
            lookupListTable->tihuanVert(pft[fi[i]].lookupListIndices[0],heng,shu);
            insertSuc=true;
            break;
        }
    }
    //没有vert功能，新建lookup+子表
    if(!insertSuc){
        int lookupIndex=lookupListTable->addVert(heng,shu);
        int featureIndex=featureListTable->addFeature("vert",lookupIndex);
        scriptListTable->gengxinFeatrueIndex(featureIndex);
        scriptListTable->insertFeatureIndex(jiaobenIndex, -1,featureIndex);
    }
}
int GSUBTable::addLoopupLiga(const QString& jiaoben,const u16vector& shuru,int& jgCoverIdx,int& jgShuruIdx){
    //返回新增脚本的插入位置
    //新增一组(脚本+语言+功能+查询)
    if(!scriptListTable){
        scriptListTable=new ScriptListTable;
    }
    //先查脚本是否重复
    if(!scriptListTable->jiaobenYouxiao(jiaoben)){
        return -1;
    }
    if(!featureListTable){
        featureListTable=new FeatureListTable;
    }
    if(!lookupListTable){
        lookupListTable=new LookupListTable;
    }
    int lookupIndex=lookupListTable->addLiga(shuru,jgCoverIdx,jgShuruIdx);
    int featureIndex=featureListTable->addFeature("liga",lookupIndex);
    //插入功能以后，已经存储在脚本中的功能索引需要更新
    //featureIndex以后的都需要加1
    scriptListTable->gengxinFeatrueIndex(featureIndex);
    int jiaobenIdx=scriptListTable->addJiaobenYuyanDef(jiaoben,featureIndex);
    return jiaobenIdx;
}

bool GSUBTable::insertLiga(int jiaobenIndex, int yuyanIndex, int zibiaoIndex,const u16vector& shuru,
                              int& jgCoverIdx,int& jgShuruIdx){
    //在已经存在的cover-liga中增加ligatrueTable
    //返回true:脚本+语言指定时，有liga功能。
    //返回false:脚本+语言指定时，没有liga功能。
    ///调用前提：
    ///脚本+语言确定的时候，如果有liga，则只有一个liga功能，对应只有一个查询表。
    /// 一个查询表有多个子表（目前只有一个），一个子表有多个（coverage+ligaSet）,一个ligaSet有多个liga表
    const ListFeatureRecord& pfr=featureListTable->lstFeatureRecord;
    const ListFeatureTable& pft=featureListTable->lstFeatureTable;
    u16vector fi;
    scriptListTable->getFeatureArr(fi,jiaobenIndex, yuyanIndex);
    bool insertSuc=false;
    //先查指定脚本指定语言有没有liga功能
    //已有liga，插入到现有liga中
    for(int i=0;i<fi.length();i++){
        if(pfr[fi[i]].featureTag=="liga"){
            //一个feature
            lookupListTable->insertLiga(pft[fi[i]].lookupListIndices[0],zibiaoIndex,
                    shuru,jgCoverIdx,jgShuruIdx);
            insertSuc=true;
            break;
        }
    }
    //没有liga功能，新建lookup+子表
    if(!insertSuc){
        int lookupIndex=lookupListTable->addLiga(shuru,jgCoverIdx,jgShuruIdx);
        int featureIndex=featureListTable->addFeature("liga",lookupIndex);
        scriptListTable->gengxinFeatrueIndex(featureIndex);
        scriptListTable->insertFeatureIndex(jiaobenIndex, yuyanIndex,featureIndex);
    }
    return insertSuc;
}


void GSUBTable::shanchuLiga(int &ji,int&yi,int& zi,int&ci,int&si){
    //删除一组。
    //每一项只要删除，原始索引修改为-1
    //没有发生删除行为的，原始索引不变。
    const ListFeatureRecord& pfr=featureListTable->lstFeatureRecord;
    const ListFeatureTable& pft=featureListTable->lstFeatureTable;
    u16vector fi;
    scriptListTable->getFeatureArr(fi,ji, yi);
    for(int i=0;i<fi.length();i++){
        if(pfr[fi[i]].featureTag=="liga"){
            //一个feature
            int li=pft[fi[i]].lookupListIndices[0];//对应只有一个查询表。
            int sli=li;
            lookupListTable->shanchuLiga(li,zi,ci,si);
            if(li==-1){
                //lookup table已删除,遍历功能表删除lookup索引+更新其他lookup索引+删除无索引的功能
                u16vector featureIndexDeleted;//收集被删除的功能索引
                featureListTable->delLookupIndex(sli,featureIndexDeleted);
                //脚本表中删除无效的功能索引，更新其他功能索引，删除无功能的语言表，删除无语言表的脚本。
                scriptListTable->delFeatureIndex(featureIndexDeleted);
                ji=-1;
                yi=-1;
            }
            break;
        }
    }
}

void GSUBTable::getJiaoben(QStringList&jbLst){
    if(scriptListTable){
        scriptListTable->getJiaoben(jbLst);
    }
}

void GSUBTable::getYuyan(QStringList&yyLst, int jiaobenIndex,bool &youDef){
    yyLst.clear();
    youDef=false;
    if(scriptListTable){
        scriptListTable->getYuyan(yyLst,jiaobenIndex,youDef);
    }
}
void GSUBTable::getLigaZibiao(int jiaobenIndex, int yuyanIndex,uint16& zibiaoShu){
    const ListFeatureRecord& pfr=featureListTable->lstFeatureRecord;
    const ListFeatureTable& pft=featureListTable->lstFeatureTable;
    u16vector fi;
    scriptListTable->getFeatureArr(fi,jiaobenIndex,yuyanIndex);

    for(int i=0;i<fi.length();i++){
        if(pfr[fi[i]].featureTag=="liga"){
            lookupListTable->getLigaZibiao(
                        pft[fi[i]].lookupListIndices[0],zibiaoShu);
            //一个语言只有一个liga功能,一个liga功能只有一个lookup表
            break;
        }
    }
}
void GSUBTable::getLigaCover(u16vector& cover,int jiaobenIndex, int yuyanIndex, int zibiaoIndex){
    const ListFeatureRecord& pfr=featureListTable->lstFeatureRecord;
    const ListFeatureTable& pft=featureListTable->lstFeatureTable;
    u16vector fi;
    scriptListTable->getFeatureArr(fi,jiaobenIndex,yuyanIndex);

    for(int i=0;i<fi.length();i++){
        if(pfr[fi[i]].featureTag=="liga"){
            lookupListTable->getLigaCover(pft[fi[i]].lookupListIndices[0],zibiaoIndex,cover);
            //一个语言只有一个liga
            break;
        }
    }
}
void GSUBTable::getLigaZifu(QList<u16vector>&shuruGlyList,
                            u16vector &shuchuGlyphIdx,
                            int jiaobenIndex, int yuyanIndex,int zibiaoIndex,int coverIndex){
    const ListFeatureRecord& pfr=featureListTable->lstFeatureRecord;
    const ListFeatureTable& pft=featureListTable->lstFeatureTable;
    u16vector fi;
    scriptListTable->getFeatureArr(fi,jiaobenIndex,yuyanIndex);

    for(int i=0;i<fi.length();i++){
        if(pfr[fi[i]].featureTag=="liga"){
            lookupListTable->getLigaZifu(
                        pft[fi[i]].lookupListIndices[0],zibiaoIndex,coverIndex,
                    shuruGlyList,
                    shuchuGlyphIdx);
            //一个语言只有一个liga
            break;
        }
    }
}
void GSUBTable::gengxinLiga(int jiaobenIndex, int yuyanIndex,int zibiaoIndex,
                            int coverIndex,int li,int16 shuchuGlyphIndex){
    const ListFeatureRecord& pfr=featureListTable->lstFeatureRecord;
    const ListFeatureTable& pft=featureListTable->lstFeatureTable;
    u16vector fi;
    scriptListTable->getFeatureArr(fi,jiaobenIndex,yuyanIndex);

    for(int i=0;i<fi.length();i++){
        if(pfr[fi[i]].featureTag=="liga"){
            lookupListTable->gengxinLiga(
                        pft[fi[i]].lookupListIndices[0],
                    zibiaoIndex,coverIndex,li,
                    shuchuGlyphIndex);
            break;
        }
    }
}

void GSUBTable::show()
{
    datashow(QString("[2B]majorVersion:%1\t\t[1]\r\n"
                     "[2B]minorVersion:%2\t\t[0]\r\n"
                     "[2B]scriptListOffset:%3\r\n"
                     "[2B]featureListOffset:%4\r\n"
                     "[2B]lookupListOffset:%5").arg(majorVersion).arg(minorVersion)
             .arg(scriptListOffset).arg(featureListOffset).arg(lookupListOffset));
    info="";
    if(scriptListTable){
        info.append("----scriptListTable-----\r\n");
        scriptListTable->show(info,this);
    }
    if(featureListTable){
        info.append("----featureListTable-----\r\n");
        featureListTable->show(info);
    }
    if(lookupListTable){
        info.append("----lookupListTable-----\r\n");
        lookupListTable->show(info,GSUB,this);
    }
    jindu(100);
    datashow(info);
}
