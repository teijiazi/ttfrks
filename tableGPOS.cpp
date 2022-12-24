#include<tableGPOS.h>

GPOSTable::GPOSTable(QObject *parent):TableTTF(parent),cunzai(false),majorVersion(1),minorVersion(0),
    scriptListTable(nullptr),featureListTable(nullptr),lookupListTable(nullptr){}
GPOSTable::~GPOSTable(){
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
void GPOSTable::read(QDataStream& dt, uint32 offset){
    cunzai=true;
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
    lookupListTable->read(dt,GPOS,this);
    //ver 1.1
    //FeatureVariations
};
void GPOSTable::xinjian(){

}
void GPOSTable::save(QDataStream& dt){
    if(!cunzai)return;
    dt<<majorVersion<<minorVersion<<scriptListOffset<<featureListOffset<<lookupListOffset;
    if(majorVersion==1&&minorVersion==1){
        dt<<featureVariationsOffset;
    }
    if(scriptListTable){
        scriptListTable->save(dt);
    }
    if(featureListTable){
        featureListTable->save(dt);
    }
    if(lookupListTable){
        lookupListTable->save(dt);
    }
}

void GPOSTable::show()
{
    datashow(QString("majorVersion:%1\t\t[1]\r\n"
                     "minorVersion:%2\t\t%6feature Variations Offset\r\n"
                     "scriptListOffset:%3\r\n"
                     "featureListOffset:%4\r\n"
                     "lookupListOffset:%5").arg(majorVersion).arg(minorVersion)
             .arg(scriptListOffset).arg(featureListOffset).arg(lookupListOffset)
             .arg(minorVersion==1?"有":"没有"));
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
        lookupListTable->show(info,GPOS,this);
    }
    jindu(100);
    datashow(info);
}

void GPOSTable::jisuan(){

}
