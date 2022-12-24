#include "cfftable.h"
#include "cffString.h"

CFFTable::CFFTable(QObject *parent) : TableTTF(parent),TopDICTINDEX(),pCharsetsFormat(nullptr)
{

}

CFFTable::~CFFTable()
{
    if(pCharsetsFormat){
        delete pCharsetsFormat;
    }
}

bool CFFTable::read(QDataStream &dt, uint32 offset,uint16 maxpNumGlyphs)
{
    //true 格式正确 false 格式错误
    dt.device()->seek(offset);
    dt>>major>>minor>>hdrSize>>offSize;
    NameINDEX.read(dt);//count==1,otf要求cff表中的fontset中只有一个font
    if(NameINDEX.count!=1){
        return false;
    }
    TopDICTINDEX.readDICT(dt);//charStringType==2
    if(TopDICTINDEX.CharstringType!=2){
        return false;
    }
    StringINDEX.read(dt);
    GlobalsubrsINDEX.read(dt);
    //以上顺序存储
    //以下按照offset存储
    //
    dt.device()->seek(offset+TopDICTINDEX.charStringOffset);
    CharStringsINDEX.read(dt);
    uint16 zifuCount=CharStringsINDEX.count;//zifuCount==maxp.numGlyphs
    if(zifuCount!=maxpNumGlyphs){
        return false;
    }
    //
    dt.device()->seek(offset+TopDICTINDEX.charsetOffset);
    Card8 fmt;
    dt>>fmt;
    switch(fmt){
    case 0:pCharsetsFormat=new CharsetsFormat0(zifuCount);break;
    case 1:pCharsetsFormat=new CharsetsFormat1;break;
    case 2:pCharsetsFormat=new CharsetsFormat2;break;
    }
    pCharsetsFormat->read(dt);
    return true;
}

void CFFTable::save(QDataStream &dt)
{
    Q_UNUSED(dt);
}

void CFFTable::show()
{
    datashow(QString("major:%1\t\t\r\n"
                     "minor:%2\r\n"
                     "hdrSize:%3\r\n"
                     "offSize:%4\r\n")
             .arg(major).arg(minor).arg(hdrSize).arg(offSize));
    QString info;
    info.append("=====Name Index\r\n");
    NameINDEX.showChar(info);
    info.append("=====Top DICT INDEX\r\n");
    TopDICTINDEX.showDICT(info);
    info.append("=====String INDEX\r\n");
    StringINDEX.showChar(info,nStdStrings);
    info.append("=====Global subrs INDEX\r\n");
    GlobalsubrsINDEX.showChar(info);
    info.append("=====Char Strings INDEX\r\n");
    CharStringsINDEX.showCharStrings(info);
    if(pCharsetsFormat){
        info.append("=====Charsets\r\n");
        pCharsetsFormat->show(info);
    }
    datashow(info);
}

void INDEXFormat::read(QDataStream &dt)
{
    dt>>count;
    if(count==0){
        return;
    }
    dt>>offSize;
    if(offSize==1){
        uint8 x;
        for(int i=0;i<=count;i++){
            dt>>x;
            offset.append(x);
        }
    }else if(offSize==2){
        uint16 x;
        for(int i=0;i<=count;i++){
            dt>>x;
            offset.append(x);
        }
    }else if(offSize==4){
        uint32 x;
        for(int i=0;i<=count;i++){
            dt>>x;
            offset.append(x);
        }
    }else{//offSize=3
        uint8 x,y,z;
        for(int i=0;i<=count;i++){
            dt>>x>>y>>z;
            offset.append(x<<16|y<<8|z);
        }
    }
    //
    for(int i=0;i<count;i++){
        QByteArray ba;
        ba.fill(0,offset[i+1]-offset[i]);
        dt.readRawData(ba.data(),offset[i+1]-offset[i]);
        data.append(ba);
    }
}

void INDEXFormat::show(QString &info)
{
    info.append(QString("count:%1\t\t\r\n")
                .arg(count));
    if(count==0){
        return;
    }
    info.append(QString("offSize:%1\t\t\r\n")
                .arg(offSize));
    info.append("offset:\r\n");
    for(int i=0;i<offset.length();i++){
        info.append(QString("%1 ")
                    .arg(offset[i]));
    }
    info.append("\r\n");
    info.append("data:\r\n");
}
void INDEXFormat::showChar(QString &info, int indexBegin)
{
    show(info);
    if(count==0){
        return;
    }
    for(int i=0;i<count;i++){
        info.append(QString("  %1:").arg(i+indexBegin));
        for(int bi=0;bi<data[i].length();bi++){
            info.append(QString("%1").arg(data[i][bi]));
        }
        info.append("\r\n");
    }
}
void INDEXFormat::showCharStrings(QString&info){
    show(info);
    if(count==0){
        return;
    }
    //charStringType==2,不等于2时，read终止
}
void TopDictINDEXFormat::showDICT(QString &info)
{
    show(info);
    if(count==0){
        return;
    }
    for(int i=0;i<count;i++){
        info.append(QString("  %1:\r\n").arg(i));
        DICTData dd(data[i]);
        dd.showTopDict(info);
        info.append("\r\n");
    }
}

void TopDictINDEXFormat::readDICT(QDataStream &dt)
{
    read(dt);
    if(count==0){
        return;
    }
    for(int i=0;i<count;i++){
        DICTData dd(data[i]);
        dd.readTopDict(charStringOffset,charsetOffset,CharstringType);
    }
}
DICTData::DICTData(QByteArray& ba)
{
    int i=0;
    QByteArray czs;
    bool czsy=false;
    while(i<ba.length()){
        uint8 firstByte=ba[i];
        switch(firstByte){
        case 0 ... 21:
        {
            if(czsy){
                ddOperand.append(czs);
                czs.clear();
                czsy=false;
            }
            uint16 czf=ba[i++];
            if(czf==12){
                czf=(12<<8)|ba[i++];
            }
            ddOperator.append(czf);
            break;
        }
        case 28:
            czsy=true;
            czs.append(ba.mid(i,3));
            i+=3;
            break;
        case 29:
            czsy=true;
            czs.append(ba.mid(i,5));
            i+=5;
            break;
        case 30:
        {
            czsy=true;
            int ri=i+1;
            while(ri<ba.length()){
                if((ba[ri]&0x0f)==0x0f){
                    break;
                }
                ri++;
            }
            czs.append(ba.mid(i,ri-i+1));
            i=ri+1;
            break;
        }
        case 32 ... 246:
            czsy=true;
            czs.append(ba.mid(i,1));
            i+=1;
            break;
        case 247 ... 254:
            czsy=true;
            czs.append(ba.mid(i,2));
            i+=2;
            break;
        }
    }
}
void DICTData::showTopDict(QString &info)
{
    for(int bi=0;bi<ddOperand.length();bi++){
        info.append(QString("    %1 %2:%3\r\n")
                    .arg(topOperatorDec(ddOperator[bi]))
                    .arg(topOperator(ddOperator[bi]),-19,QChar(' '))
                    .arg(topOperand(ddOperator[bi],ddOperand[bi])));
    }
}
void DICTData::readTopDict(int32& charStringOffset, int32& charsetOffset, int &CharstringType)
{
    CharstringType=2;//default
    for(int bi=0;bi<ddOperand.length();bi++){
        if(ddOperator[bi]==15){
            charsetOffset=toNumber(ddOperand[bi]).toInt();
        }else if(ddOperator[bi]==17){
            charStringOffset=toNumber(ddOperand[bi]).toInt();
        }else if(ddOperator[bi]==0x0c06){
            CharstringType=toNumber(ddOperand[bi]).toInt();
        }
    }
}
QString DICTData::topOperatorDec(uint16 czf){
    if(czf&0x0c00){
        return QString("12 %1").arg(czf&0xff,2,10,QChar(' '));
    }else{
        return QString("   %1").arg(czf,2,10,QChar(' '));
    }
}
QString DICTData::topOperator(uint16 czf)
{
    const QString TopDICTCzs2[]={
        "Copyright",         // 12 0 SID –, FontInfo
        "isFixedPitch",      // 12 1 boolean 0 (false), FontInfo
        "ItalicAngle",       // 12 2 number 0, FontInfo
        "UnderlinePosition", // 12 3 number –100, FontInfo
        "UnderlineThickness",// 12 4 number 50, FontInfo
        "PaintType",         // 12 5 number 0
        "CharstringType",    // 12 6 number 2
        "FontMatrix",        // 12 7 array 0.001 0 0 0.001 0 0
        "StrokeWidth",       // 12 8 number 0
        "","","","","","","","","","","",//9-19
        "SyntheticBase",     // 12 20 number –, synthetic base font index
        "PostScript",        // 12 21 SID –, embedded PostScript language code
        "BaseFontName",      // 12 22 SID –, (added as needed by Adobe-based technology)
        "BaseFontBlend",     // 12 23 delta –, (added as needed by Adobe-based technology)
        "","","","","","",//24-29
        //CIDFont Operator Extensions
        "ROS(SID SID number)",               //12 30 SID SID number   –, Registry Ordering Supplement
        "CIDFontVersion",    //12 31 number 0
        "CIDFontRevision",   //12 32 number 0
        "CIDFontType",       //12 33 number 0
        "CIDCount",          //12 34 number 8720
        "UIDBase",           //12 35 number –
        "FDArray",           //12 36 number –, Font DICT (FD) INDEX offset (0)
        "FDSelect",          //12 37 number –, FDSelect offset (0)
        "FontName",          //12 38 SID –, FD FontName
    };
    const QString TopDICTCzs[]={
        "version",    // 0 SID –, FontInfo
        "Notice",     // 1 SID –, FontInfo
        "FullName",   // 2 SID –, FontInfo
        "FamilyName", // 3 SID –, FontInfo
        "Weight",     // 4 SID –, FontInfo
        "FontBBox",   // 5 array 0 0 0 0
        "","","","","","","",//6~12
        "UniqueID",   // 13 number –
        "XUID",       // 14 array –
        "charset",    // 15 number 0, charset offset (0)
        "Encoding",   // 16 number 0, encoding offset (0)
        "CharStrings",// 17 number –, CharStrings offset (0)
        "Private",    // 18 number number –, Private DICT size and offset (0)
    };
    if(czf&0x0c00){
        return TopDICTCzs2[czf-0x0c00];
    }else{
        return TopDICTCzs[czf];
    }
}
QString DICTData::toDelta(const QByteArray &czs){
    for(int i=0;i<czs.length();i++){
        if(czs[i]){
            return "true";
        }
    }
    return "false";
}
QString DICTData::toArray(const QByteArray &czs){
    QString jg;
    int yiduchangdu=0;
    QByteArray jcsz(czs);
    int i=0;
    while(i<czs.length()){
        jg+=toNumber(jcsz,&yiduchangdu)+" ";
        i+=yiduchangdu;
        jcsz=czs.right(czs.length()-i);
    }
    return jg;
}

QString DICTData::toBoolean(const QByteArray &czs){
    QString jg=toNumber(czs);
    return jg=="1"?"true":"false";
}


QString DICTData::banzijie(uint8 bzj,bool* jieshu){
    switch(bzj){
    case 0 ... 9:return QString::number(bzj);
    case 0xa:return ".";
    case 0xb:return "E";
    case 0xc:return "E–";
    case 0xd:return "";
    case 0xe:return "–";
    default:
        if(jieshu){
            *jieshu=true;
        }
        return "";
    }
}


QString DICTData::toNumber(const QByteArray &czs, int* yiduchangdu){
    QString czsstr;
    if(czs[0]==0x1e){//小数
        int i=1;
        while(i<czs.length()){
            czsstr+=banzijie(czs[i]>>4);
            bool jieshu=false;
            czsstr+=banzijie(czs[i]&0x0f,&jieshu);
            if(jieshu){
                if(yiduchangdu){
                    *yiduchangdu=i+1;
                }
                break;
            }
            i++;
        }
    }else{
        //整数
        switch(uint8(czs[0])){
        case 28:
            czsstr=QString::number((int16)((czs[1]<<8)|czs[2]));
            if(yiduchangdu){
                *yiduchangdu=3;
            }
            break;
        case 29:
            czsstr=QString::number((int32)(uint8(czs[1])<<24|uint8(czs[2])<<16|uint8(czs[3])<<8|uint8(czs[4])));
            if(yiduchangdu){
                *yiduchangdu=5;
            }
            break;
        case 32 ... 246:
            czsstr=QString::number((int8)(czs[0]-139));
            if(yiduchangdu){
                *yiduchangdu=1;
            }
            break;
        case 247 ... 250:
            czsstr=QString::number((uint16)((czs[0]-247)*256+czs[1]+108));
            if(yiduchangdu){
                *yiduchangdu=2;
            }
            break;
        case 251 ... 254:
            czsstr=QString::number((int16)(-(uint8(czs[0])-251)*256-uint8(czs[1])-108));
            if(yiduchangdu){
                *yiduchangdu=2;
            }
            break;
        default:
            if(yiduchangdu){
                *yiduchangdu=0;
            }
            return "";
        }
    }
    return czsstr;
}
QString DICTData::toSIDStr(const QByteArray &czs){
    QString sid=toNumber(czs);
    uint16 x=sid.toUInt();
    QString jg="sid="+sid;
    if(x<nStdStrings){
        jg+="("+StandardStrings[x]+")";
    }
    return jg;
}
QString DICTData::topOperand(uint16 czf, const QByteArray &czs)
{
    QString czsstr;
    if(czf&0x0c00){
        switch(czf-0x0c00){
        case 0:
        case 21:
        case 22:
        case 38:czsstr=toSIDStr(czs);break;
        case 1:czsstr=toBoolean(czs);break;
        case 2 ... 6:
        case 8:
        case 20:
        case 31 ... 37:czsstr=toNumber(czs);break;
        case 7:czsstr=toArray(czs);break;
        case 23:czsstr=toDelta(czs);break;
        case 30:
            czsstr=toSIDStr(czs.mid(0,2))+" "+
                    toSIDStr(czs.mid(2,2))+" "+
                    toNumber(czs.right(czs.length()-4));
            break;
        }
    }else{
        switch(czf){
        case 0 ... 4:czsstr=toSIDStr(czs);break;
        case 13:
        case 15 ... 18:czsstr=toNumber(czs);break;
        case 5:
        case 14:czsstr=toArray(czs);break;
        }
    }
    return czsstr;
}



void CharsetsFormat0::read(QDataStream &dt)
{
    for(int i=0;i<zifuCount-1;i++){
        CFFSID sid;
        dt>>sid;
        glyph.append(sid);
    }
}

void CharsetsFormat1::read(QDataStream &dt)
{
    Q_UNUSED(dt);
}

void CharsetsFormat2::read(QDataStream &dt)
{
    Q_UNUSED(dt);
}
void CharsetsFormat0::show(QString &info)
{
    info.append(QString("chartset format:%1\r\n"
                        "zifuZongshu:%2\r\n")
                .arg(format).arg(zifuCount));
    for(int i=0;i<glyph.length();i++){
        info.append(QString("%1 ").arg(glyph[i]));
    }
}

void CharsetsFormat1::show(QString &info)
{
    info.append(QString("chartset format:%1\r\n").arg(format));
}

void CharsetsFormat2::show(QString &info)
{
    info.append(QString("chartset format:%1\r\n").arg(format));
}
