#include<tableName.h>
#include<QRegularExpression>
#include "pinyinku.h"

const QString NameRecord::yudingyiStr[YudingyiNameCount]={
    /*0*/"版权",
    /*1*/"字体族名称",//
    /*2*/"字体子族名称",//
    /*3*/"唯一字体id",//
    /*4*/"字体全名",//
    /*5*/"版本",//固定格式：“Version <number>.<number>”
    /*6*/"PostScript",// 不超过63字符，仅限[33 to 126],不能用这10个： '[', ']', '(', ')', '{', '}', '<', '>', '/', '%'.
    /*7*/"商标",//
    /*8*/"制造者",//
    /*9*/"设计者",//
    /*10*/"描述",//
    /*11*/"字体供应商的网址",//
    /*12*/"设计者网址",//
    /*13*/"许可证描述",//
    /*14*/"许可证网址",//
    /*15*/"预留",//.
    /*16*/"印刷字体族名称",//
    /*17*/"印刷字体子族名称",//
    /*18*/"完全兼容",
    /*19*/"样本文本",//
    /*20*/"PostScript CID findfont name",//; Its presence in a font means that the nameID 6 holds a PostScript font name that is meant to be used with the “composefont” invocation in order to invoke the font in a PostScript interpreter. See the definition of name ID 6.
    /*21*/"WWS字体族名称",//. Used to provide a WWS-conformant family name in case the entries for IDs 16 and 17 do not conform to the WWS model. (That is, in case the entry for ID 17 includes qualifiers for some attribute other than weight, width or slope.) If bit 8 of the fsSelection field is set, a WWS Family Name entry should not be needed and should not be included. Conversely, if an entry for this ID is included, bit 8 should not be set. (See OS/2.fsSelection field for details.) Examples of name ID 21: “Minion Pro Caption” and “Minion Pro Display”. (Name ID 16 would be “Minion Pro” for these examples.)
    /*22*/"WWS字体子族名称",//. Used in conjunction with ID 21, this ID provides a WWS-conformant subfamily name (reflecting only weight, width and slope attributes) in case the entries for IDs 16 and 17 do not conform to the WWS model. As in the case of ID 21, use of this ID should correlate inversely with bit 8 of the fsSelection field being set. Examples of name ID 22: “Semibold Italic”, “Bold Condensed”. (Name ID 17 could be “Semibold Italic Caption”, or “Bold Condensed Display”, for example.)
    /*23*/"浅色背景调色板",//. This ID, if used in the CPAL table’s Palette Labels Array, specifies that the corresponding color palette in the CPAL table is appropriate to use with the font when displaying it on a light background such as white. Strings for this ID are for use as user interface strings associated with this palette.
    /*24*/"深色背景调色板",// This ID, if used in the CPAL table’s Palette Labels Array, specifies that the corresponding color palette in the CPAL table is appropriate to use with the font when displaying it on a dark background such as black. Strings for this ID are for use as user interface strings associated with this palette
    /*25*/"变体PostScript名称前缀"
};

NameRecord::NameRecord(uint16 vplatformID, uint16 vencodingID, uint16 vlanguageID,
                       uint16 vnameID, uint16 vlength, Offset16 vstringOffset):
    platformID(vplatformID),encodingID(vencodingID),languageID(vlanguageID),
    nameID(vnameID), length(vlength), stringOffset(vstringOffset)
{

}

void NameRecord::set(uint16 vplatformID, uint16 vencodingID, uint16 vlanguageID,
                     uint16 vnameID, uint16 vlength, Offset16 vstringOffset)
{
    platformID=vplatformID;
    encodingID=vencodingID;
    languageID=vlanguageID;
    nameID=vnameID;
    length=vlength;
    stringOffset=vstringOffset;
}

void NameRecord::readFromDt(QDataStream&dt){
    dt>>platformID>>encodingID>>languageID>>nameID>>length>>stringOffset;
}
void NameRecord::save(QDataStream&dt){
    dt<<platformID<<encodingID<<languageID<<nameID<<length<<stringOffset;
}

void NameRecord::show(int index,TableTTF *tbl)
{
    QString plat[]={
        "Unicode","Macintosh","xx","Windows"
    };
    QString nameIDStr=[](uint16 nameid)->QString{
        if(nameid<YudingyiNameCount){
            return yudingyiStr[nameid];
        }else if(nameid<=255){
            return "预留";
        }else{//256~32767
            return "特定";
        }
    }(nameID);
    QString encodingIDStr=[](uint16 pid,uint16 cid)->QString{
        if(pid==piWindows){//Windows
            return (cid==1)?"unicodeBMP":(cid==10?"unicodeFull":"其他编码");
        }
        if(pid==piUnicode){//0
            return (cid==3)?"unicodeBMP":(cid==4?"unicodeFull":"其他编码");
        }
        if(pid==piMacintosh){//Macintosh
            const QString macCodeScriptMgr[]={
                "罗马",
                "日语",
                "繁体中文",
                "朝鲜",
                "Arabic",
                "Hebrew",
                "Greek",
                "Russian",
                "RSymbol",
                "Devanagari",
                "Gurmukhi",
                "Gujarati",
                "Oriya",
                "Bengali",
                "Tamil",
                "Telugu",
                "Kannada",
                "Malayalam",
                "Sinhalese",
                "Burmese",
                "Khmer",
                "Thai",
                "Laotian",
                "Georgian",
                "Armenian",
                "简体中文",
                "Tibetan",
                "Mongolian",
                "Geez",
                "Slavic",
                "Vietnamese",
                "Sindhi",
                "Uninterpreted"};
            return macCodeScriptMgr[cid];
        }
        return "未知编码";
    }(platformID,encodingID);
    QString lanStr=[](uint16 pid,uint16 lid)->QString{
        if(pid==piWindows){//Windows
            const uint16 lids[]={0x0406,       0x0409, 0x040b,0x040a,
                                 0x0804,0x0C04,0x1404,0x1004,0x0404,
                                 0x0403,0x0405,0x0407,0x0408,
                                 0x040c,0x040e,0x0410,0x0413,0x0414,
                                 0x0415,0x0416,0x0419,0x041b,0x041d,
                                 0x041f,0x0424,0x042d,0x080a,
                                 0x0816,0x0c0a,0x0c0c};
            const QString lstr[]={"丹麦语","英语（美国）","芬兰语（芬兰）","传统西班牙语（西班牙）",
                                  "中文（中国）","中文（香港）","中文（澳门）","中文（新加坡）","中文（繁体）",
                                  "加泰罗尼亚","捷克","德语","希腊语",
                                  "法语","匈牙利语","意大利语（意大利）","荷兰语（荷兰）","挪威语",
                                  "波兰语","葡萄牙语（巴西）","俄语","斯洛伐克语","瑞典语（瑞典）",
                                  "土耳其语","斯洛文尼亚语","巴斯克语","西班牙语（墨西哥）",
                                  "葡萄牙语（葡萄牙）","现代西班牙语（西班牙）","法语（加拿大）"};
            for(uint i=0;i<sizeof(lids)/sizeof(uint16);i++){
                if(lid==lids[i]){
                    return lstr[i];
                }
            }
            return "其他";
        }
        if(pid==piUnicode){
            return "xxxx";
        }
        if(pid==piMacintosh){//Macintosh
            return (lid==33)?"简体中文":((lid==0)?"英语":"其他");
        }
        return "QString";
    }(platformID,languageID);

    tbl->datashow(QString("%7\r\nplatformID:%1\t%9\r\n"
                          "encodingID:%2\t%10\r\n"
                          "languageID:%3\t%11\r\n"
                          "nameID:%4\t\t%8 -- %12\r\n"
                          "length:%5\r\n"
                          "stringOffset:%6\r\n")
                  .arg(platformID).arg(encodingID).arg(languageID,4,16,QChar('0'))
                  .arg(nameID).arg(length).arg(stringOffset).arg(index+1)
                  .arg(nameIDStr).arg(plat[platformID]).arg(encodingIDStr).arg(lanStr).arg(recStr));
}


version0::version0():version(0),count(0),pData(nullptr){}
version0::~version0(){
    if(pData){
        delete []pData;
    }
}
void version0::chaijieString(uint16 platid,char*p,int len,QString &jg){
    if(platid==piMacintosh){
        //mac，char字符
        jg=QString::fromLocal8Bit(p,len);
        return;
    }else{
        //platid=windows，bigendian wchar编码。2字节。
        char*pDst=new char[len];
        for(int i=0;i<len;i+=2){
            pDst[i]=p[i+1];
            pDst[i+1]=p[i];
        }
        jg=QString::fromWCharArray((wchar_t*)pDst,len/sizeof(wchar_t));
        delete[]pDst;
    }
}
void version0::readFromDt(QDataStream&dt,uint32 nameLen,NAMETable* nt){
    dt>>count>>storageOffset;
    nameRecord.clear();
    for(int i=0;i<count;i++){
        NameRecord nr;
        nr.readFromDt(dt);
        nameRecord.push_back(nr);
    }
    fzDatalen=nameLen-sizeof(uint16)*(3+count*6);
    pData=new char[fzDatalen];
    dt.readRawData(pData,fzDatalen);
    //拆解
    for(uint16 i=0;i<count;i++){
        chaijieString(
                    nameRecord[i].platformID,
                    &pData[nameRecord[i].stringOffset],
                nameRecord[i].length,
                nameRecord[i].recStr);
    }

    //界面更新
    nt->showFontinfo(nameRecord);
}
void version0::save(QDataStream&dt){
    dt<<version<<count<<storageOffset;
    for(int i=0;i<count;i++){
        nameRecord[i].save(dt);
    }
    dt.writeRawData(pData,fzDatalen);
}

void version0::show(TableTTF *tbl)
{
    tbl->datashow(QString("version:%1\r\ncount:%2\r\nstorageOffset:%3\r\n")
                  .arg(version).arg(count).arg(storageOffset));
    for(int i=0;i<count;i++){
        nameRecord[i].show(i,tbl);
    }
}
//选择共享内容 中文 英文
#define JIA(x,nameid) \
    if(x!=""){\
    nr.set(piWindows,1,ybZhong,nameid,x##Len,x##Off);\
    nameRecord.push_back(nr);\
    if(x##YLen){\
    nr.set(piWindows,1,ybYing,nameid,x##YLen,x##YOff);\
    }else{\
    nr.languageID=ybYing;\
    }\
    nameRecord.push_back(nr);\
    }

//共享内容
#define JIAX(x,nameid) \
    nr.set(piWindows,1,ybYing,nameid,x##Len,x##Off);\
    nameRecord.push_back(nr);\
    nr.languageID=ybZhong;\
    nameRecord.push_back(nr)

#define DINGYI(x,sy)\
    QString x=sm[sy],x##Y=smY[sy]

void version0::jisuan(const QString& ztmz,const QString& banben,
                      const QStringList&sm,const QStringList&smY)
{
    nameRecord.clear();//重建
    if(pData){
        delete []pData;
    }
    //准备name表的各个nameid的数据
    //预定义
    QString subfamily="Regular";
    QString banbenStr="Version "+banben;
    QString ztmzY;//!< 字体名字对应的拼音
    getPinyinWudiao(ztmz,ztmzY);
    //自定义
    DINGYI(banquan,0);
    DINGYI(shangbiao,7);
    //偏移和长度
    uint16 subLen=subfamily.length()*sizeof (wchar_t);//!<subFamily的长度
    uint16 bbLen=banbenStr.length()*sizeof (wchar_t); //!<版本的长度
    uint16 ztYLen=ztmzY.length()*sizeof (wchar_t);    //!<字体英文名称 的长度
    uint16 ztLen=ztmz.length()*sizeof (wchar_t);      //!<字体中文名称 的长度
    uint16 banquanLen=banquan.length()*sizeof (wchar_t);   //!<版权 的长度
    uint16 banquanYLen=banquanY.length()*sizeof (wchar_t);   //!<英文版权 的长度
    if(banquan==banquanY)banquanYLen=0;                    //英文版权如果没有填写，或与中文一致，则与中文共享存储。
    uint16 shangbiaoLen=shangbiao.length()*sizeof (wchar_t); //!<商标 的长度
    uint16 shangbiaoYLen=shangbiaoY.length()*sizeof (wchar_t); //!<英文商标 的长度
    if(shangbiao==shangbiaoY)shangbiaoYLen=0;//英文商标如果没有填写，或与中文一致，则与中文共享存储。
    uint16 fnmLen=ztYLen+subLen+sizeof (wchar_t);     //!<字体英文名称+subfamilyname 的长度，如:DengXian Regular
    uint16 fnmZLen=ztLen+subLen+sizeof (wchar_t);     //!<字体中文名称+subfamilyname 的长度，如:等线 Regular

    //排顺序，写入dataStr
    uint16 banquanOff  =0;
    uint16 ztYOff =banquanOff+banquanLen;
    uint16 subOff =ztYOff+ztYLen+sizeof (wchar_t);
    uint16 fnmOff  =ztYOff;//!<位置: 字体英文名称+subfamilyname
    uint16 bbOff  =fnmOff+fnmLen;
    uint16 shangbiaoOff  =bbOff+bbLen;
    uint16 ztOff  =shangbiaoOff +shangbiaoLen;
    uint16 fnmZOff  =ztOff;//!<位置: 字体中文名称+subfamilyname
    uint16 banquanYOff  =fnmZOff+fnmZLen;
    uint16 shangbiaoYOff  =banquanYOff+banquanYLen;
    uint16 beginOff=shangbiaoYOff+shangbiaoYLen;
    //合在一起
    QString dataStr=banquan+ztmzY+" "+subfamily+banbenStr+shangbiao+ztmz+" "+subfamily;
    //                      DengXian Regular    Version 1.18        等线 Regular
    if(banquanYLen)dataStr+=banquanY;
    if(shangbiaoYLen)dataStr+=shangbiaoY;
    //填写记录，先写完再排序
    //1 字体族名称
    NameRecord nr(piWindows,1,ybYing,nxMingcheng,ztYLen,ztYOff);//Dengxian
    nameRecord.push_back(nr);
    nr.set(piWindows,1,ybZhong,nxMingcheng,ztLen,ztOff);//等线
    nameRecord.push_back(nr);
    //2 Font Subfamily name 中英同 如:Regular
    JIAX(sub,nxZizu);
    //3 Unique font id
    nr.set(piWindows,1,ybYing,nxWeiyiId,fnmLen,fnmOff);//英文 字体名称+subfamilyname 如:DengXian Regular
    nameRecord.push_back(nr);
    nr.set(piWindows,1,ybZhong,nxWeiyiId,fnmZLen,fnmZOff);//中文 字体名称+subfamilyname 如:等线 Regular,不能丢掉regular,安装界面显示用。
    nameRecord.push_back(nr);
    //4 Full font name
    nr.set(piWindows,1,ybYing,nxQuanming,fnmLen,fnmOff);//字体名称+subfamilyname 如:DengXian Regular
    nameRecord.push_back(nr);
    nr.set(piWindows,1,ybZhong,nxQuanming,ztLen,ztOff);//同字体名称 id=1 如：等线
    nameRecord.push_back(nr);
    //5 Version 中英同 如：Version 1.18
    JIAX(bb,nxBanben);
    //6 PostScript 中英同 如：Dengxian
    JIAX(ztY,nxPostScript);
    //有中文，写入。没有英文->共享中文。
    JIA(banquan,nxBanquan);
    JIA(shangbiao,nxShangbiao);
    uint16 offset=beginOff;
    uint16 len=0;
    for(int i=nxZhizao;i<sm.length();i++){//i：nameid 1~6自动填写，0、7 已经填写
        if(sm[i]!=""){
            dataStr+=sm[i];
            len=sm[i].length()*sizeof (wchar_t);
            nr.set(piWindows,1,ybZhong,i,len,offset);
            offset+=len;
            nameRecord.push_back(nr);
            if(smY[i]==""||smY[i]==sm[i]){//英文如果没有填写，或与中文一致，则与中文共享存储。
                nr.languageID=ybYing;
            }else{
                len=smY[i].length()*sizeof (wchar_t);
                nr.set(piWindows,1,ybYing,i,len,offset);
                offset+=len;
                dataStr+=smY[i];
            }
            nameRecord.push_back(nr);
        }
    }
    sort(nameRecord.begin(),nameRecord.end(),[](const NameRecord& a, const NameRecord&b){
        return (a.languageID<b.languageID ||(a.languageID==b.languageID && a.nameID<b.nameID));
    });

    //整理数据
    fzDatalen=dataStr.length()*sizeof (wchar_t);
    pData=new char[fzDatalen];
    dataStr.toWCharArray((wchar_t*)pData);
    //总数
    this->count=nameRecord.length();
    storageOffset=sizeof(uint16)*3+count*6*sizeof(uint16);
    //修改为bigendian
    for(uint32 i=0;i<fzDatalen;i+=2){
        char tst=pData[i];pData[i]=pData[i+1];pData[i+1]=tst;
    }
}

bool version0::win09NotQmgj()
{
    //false:platid=piWindows nameid=nxSheji lan=ybZhong str=qmgj
    for(int i=0;i<nameRecord.length();i++){
        if(nameRecord[i].nameID==nxSheji && nameRecord[i].platformID==piWindows
                && nameRecord[i].languageID==ybZhong
                && nameRecord[i].recStr=="qmgj"){
            return false;
        }
    }
    return true;
}

version1::version1():version(1),count(0),nameRecord(nullptr),langTagCount(0),langTagRecord(nullptr),pData(nullptr){}
version1::~version1(){
    if(nameRecord){
        delete []nameRecord;
    }
    if(langTagRecord){
        delete [] langTagRecord;
    }
    if(pData){
        delete []pData;
    }
}
void version1::arrNr(){
    nameRecord=new NameRecord[count];
}
void version1::arrLTR(){
    langTagRecord=new LangTagRecord[langTagCount];
}



NAMETable::NAMETable(QObject *parent):TableTTF(parent),nt0(nullptr),nt1(nullptr){}
NAMETable::~NAMETable(){
    if(nt0)delete nt0;
    if(nt1)delete nt1;
}
void NAMETable::read(QDataStream& dt, uint32 offset,uint32 nameLen){
    dt.device()->seek(offset);
    uint16 version;
    dt>>version;
    if(version==0){
        nt0=new version0;
        nt0->readFromDt(dt,nameLen,this);
    }else{
        datashow("name table version1");
    }
};
void NAMETable::save(QDataStream& dt){
    if(nt0){
        nt0->save(dt);
    }else if(nt1){
        datashow("name table version1");
    }
}

void NAMETable::show()
{
    if(nt0){
        nt0->show(this);
    }else if(nt1){
        datashow("name table version1");
    }
}

bool NAMETable::bukexiugai(){
    if(nt0){
        return nt0->win09NotQmgj();
    }else if(nt1){
        datashow("name table version1");
    }
    return false;
}

void NAMETable::jisuan(const QString &ztmz, const QString &banben,
                       const QStringList&sm,const QStringList&smY)
{
    if(!nt0){
        nt0=new version0;
    }
    nt0->jisuan(ztmz, banben, sm,smY);
}
