#include<tableGDEF.h>

GDEFTable::GDEFTable(QObject *parent):TableTTF(parent),
    majorVersion(1),
    minorVersion(0),
    glyphClassDefOffset(0),
    attachListOffset(0),
    ligCaretListOffset(0),
    markAttachClassDefOffset(0),
    markGlyphSetsDefOffset(0),
    itemVarStoreOffset(0),
    pGlyphClassDef(nullptr){}
GDEFTable::~GDEFTable(){
    if(pGlyphClassDef){
        delete pGlyphClassDef;
    }
}
void GDEFTable::read(QDataStream& dt, uint32 offset){
    dt.device()->seek(offset);
    dt>>majorVersion>>minorVersion
            >>glyphClassDefOffset
            >>attachListOffset//0
            >>ligCaretListOffset//0
            >>markAttachClassDefOffset;//0
    if(majorVersion==1 && minorVersion==2){
        dt>>markGlyphSetsDefOffset;
    }
    if(majorVersion==1 && minorVersion==3){
        dt>>markGlyphSetsDefOffset>>itemVarStoreOffset;
    }
    if(glyphClassDefOffset){
        pGlyphClassDef=new ClassDefFormat2;
        pGlyphClassDef->read(dt,false);
    }
    if(attachListOffset){
        //
    }
    if(ligCaretListOffset){
        //
    }
    if(markAttachClassDefOffset){
        //
    }
}
void GDEFTable::jisuan(uint16 vEndGlyphID){
    if(pGlyphClassDef){
        delete pGlyphClassDef;
    }

    //1.0
    majorVersion=1;
    minorVersion=0;
    glyphClassDefOffset=12;
    pGlyphClassDef=new ClassDefFormat2;
    pGlyphClassDef->classRangeCount=1;
    pGlyphClassDef->arr();
    pGlyphClassDef->pClassRangeRecord[0].startGlyphID=0;
    pGlyphClassDef->pClassRangeRecord[0].endGlyphID=vEndGlyphID;
    pGlyphClassDef->pClassRangeRecord[0].classValue=1;
}
void GDEFTable::save(QDataStream &dt){
    dt<<majorVersion<<minorVersion
     <<glyphClassDefOffset
    <<attachListOffset//0
    <<ligCaretListOffset//0
    <<markAttachClassDefOffset;//0
    if(pGlyphClassDef){
        pGlyphClassDef->save(dt);
    }
}

void GDEFTable::show()
{
    datashow(QString("majorVersion:%1\r\nminorVersion:%2\r\n"
                      "glyphClassDefOffset:%3\r\nattachListOffset:%4\r\n"
                      "ligCaretListOffset:%5\r\n"
                      "markAttachClassDefOffset:%6\r\n"
                      ).arg(majorVersion).arg(minorVersion)
              .arg(glyphClassDefOffset).arg(attachListOffset).arg(ligCaretListOffset)
              .arg(markAttachClassDefOffset));
    if(majorVersion==1 && minorVersion==2){
        datashow(QString("markGlyphSetsDefOffset:%1\r\n").arg(markGlyphSetsDefOffset));
    }
    if(majorVersion==1 && minorVersion==3){
        datashow(QString("markGlyphSetsDefOffset:%1\r\n"
                          "itemVarStoreOffset:%2\r\n").arg(markGlyphSetsDefOffset).arg(itemVarStoreOffset));
    }
    datashow(info);
}
