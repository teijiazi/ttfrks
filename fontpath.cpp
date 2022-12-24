#include "fontpath.h"
#include <windows.h>
#include "../rizhi/rizhi.h"
#include <QRegularExpression>
#include <QFile>
#include <QDataStream>
#include "types.h"
#include "ttfhead.h"

FontPath::FontPath(KeyStringList& vnamelist,QObject *parent) : QObject(parent),namelist(vnamelist)
{
    namelist.clear();
}

QString FontPath::showVersion(){
    //LIBS += -lVersion
    WCHAR szPath[MAX_PATH];
    GetModuleFileName(NULL, szPath, MAX_PATH);
    //
    DWORD fish=0;
    DWORD verInfoSize=GetFileVersionInfoSizeW(szPath,&fish);
    if(verInfoSize>0){
        PWCHAR pVerinfo=new WCHAR[verInfoSize+1]();
        BOOL isverInfosuc=GetFileVersionInfoW(szPath,0,verInfoSize,pVerinfo);
        if(isverInfosuc){
            VS_FIXEDFILEINFO * pver;
            UINT versize=0;
            BOOL isversuc=VerQueryValueW(pVerinfo,L"\\",(LPVOID*)&pver,&versize);
            if(isversuc){
                return QString("做字体 %1.%2.%3.%4").arg(HIWORD(pver->dwFileVersionMS))
                        .arg(LOWORD(pver->dwFileVersionMS))
                        .arg(HIWORD(pver->dwFileVersionLS))
                        .arg(LOWORD(pver->dwFileVersionLS));
            }
        }else{
            snd<<"GetFileVersionInfoW err="<<GetLastError();
            return "";
        }
        delete []pVerinfo;
    }else{
        snd<<"versize err="<<GetLastError();
        return "";
    }
    return "";
}

void FontPath::setNotepadFont(const QString &ziti){
    //win11 无效
    if(ziti==""){
        return;
    }
    WCHAR wpsPath[]= L"SOFTWARE\\Microsoft\\Notepad";
    HKEY hKey;
    LONG err = RegOpenKeyExW(HKEY_CURRENT_USER,
                             wpsPath, 0, KEY_WRITE, &hKey);
    if (err) {
        return;
    }
    err=RegSetValueExW(hKey,L"lfFaceName",0,REG_SZ,(BYTE *)(ziti.toStdWString().c_str()),(ziti.length()+1)*sizeof (wchar_t));
    if(err){
        snd<<err;
    }
    RegCloseKey(hKey);
}

void FontPath::getWPSexe(QString& wpsCmdline){
    WCHAR wpsPath[]= L"Software\\Classes\\ksowps\\shell\\open\\command";
    DWORD shujuByte=MAX_PATH;
    DWORD ty=0;
    LPBYTE shuju = new BYTE[MAX_PATH];
    LONG err = RegGetValueW(HKEY_CURRENT_USER,wpsPath, L"",RRF_RT_REG_SZ, &ty,(LPVOID)shuju,&shujuByte);
    if (err != ERROR_SUCCESS) {
        return;
    }
    wpsCmdline=QString::fromWCharArray((LPWSTR)shuju)
            .remove("\"%1\"").remove("/wps").remove("\"").trimmed();
    delete[] shuju;
}

void FontPath::getAllFontPathFromReg(int from){
    static const QString fontRegistryPath= "Software\\Microsoft\\Windows NT\\CurrentVersion\\Fonts";
    HKEY hKey;
    LONG err = RegOpenKeyExW((from==1)?HKEY_LOCAL_MACHINE:HKEY_CURRENT_USER,
                             fontRegistryPath.toStdWString().c_str(), 0, KEY_READ, &hKey);
    if (err) {
        return;
    }
    DWORD mingchengZifuMax, //最大的value字符数，不包括末尾的0
            shujuByteMax;//最大数据的字节数，已经包含两字节的0
    err = RegQueryInfoKeyW(hKey, 0, 0, 0, 0, 0, 0, 0, &mingchengZifuMax, &shujuByteMax, 0, 0);
    if (err) {
        return;
    }
    DWORD valueIndex = 0;
    LPWSTR mingcheng = new WCHAR[mingchengZifuMax+1]();//要预留0结束符
    LPBYTE shuju = new BYTE[shujuByteMax]();
    DWORD mingchengZifu, shujuByte, leixing;
    do {
        mingchengZifu = mingchengZifuMax+1;//指出mingcheng的实际长度
        shujuByte = shujuByteMax;
        err = RegEnumValueW(hKey, valueIndex, mingcheng, &mingchengZifu, 0, &leixing, shuju, &shujuByte);
        valueIndex++;
        if (err != ERROR_SUCCESS) {
            continue;
        }
        if (leixing != REG_SZ) {
            snd<<"d";
            continue;
        }
        QString ftpath=QString::fromWCharArray((LPWSTR)shuju);
        if(from==1 && ftpath.indexOf(":")==-1){
            WCHAR winDir[MAX_PATH];
            GetWindowsDirectoryW(winDir, MAX_PATH);
            ftpath=QString::fromWCharArray(winDir)+"\\Fonts\\" +ftpath;
        }
        getNameFromFile(ftpath);
    } while (err != ERROR_NO_MORE_ITEMS);
    delete[] mingcheng;
    delete[] shuju;
    RegCloseKey(hKey);
}

void FontPath::getFullFontName(){
    //列举全部字体
    getAllFontPathFromReg(1);
    getAllFontPathFromReg(2);
    sort(namelist.begin(),namelist.end(),[](const KeyString &a, const KeyString &b){
        //代码页排序：简体中文 繁体中文 其他
        return (a.dmy==b.dmy&&a.name>b.name)||(a.dmy<b.dmy);
    });
    for(int i=0;i<namelist.length()-1;i++){//去掉重复的注册表项目
        if(namelist[i].name==namelist[i+1].name && namelist[i].path==namelist[i+1].path){
            namelist.removeAt(i+1);
            i--;
        }
    }
}

void FontPath::getNameFromFile(const QString& fontfilePath,bool paixu){
    //列举指定文件的字体
    QFile file(fontfilePath);
    if (!file.open(QIODevice::ReadOnly)){
        return;
    }
    //读文件头
    QDataStream dt(&file);
    uint32 sfntVersion;// 0x00010000 or 0x4F54544F ('OTTO')  — see below.
    u32vector ttcOffsetVector;
    dt>>sfntVersion;
    Tag ttftag;
    ttftag.data=sfntVersion;
    if(ttftag=="ttcf"){
        TTCHeader th;
        th.getNameFromTTC(dt,ttcOffsetVector);
    }else{
        ttcOffsetVector.append(0);
    }
    for(Offset32 py:ttcOffsetVector){
        dt.device()->seek(py);
        QString fontname;
        CodePage dmy=cpQita;
        getNameFromTable(dt,fontname,dmy);
        if(fontname!=""){
            namelist.append(KeyString(fontname,fontfilePath,py,dmy));
        }
    }
    file.close();
    if(paixu){
        sort(namelist.begin(),namelist.end(),[](const KeyString &a, const KeyString &b){
            //代码页排序：简体中文 繁体中文 其他
            return (a.dmy==b.dmy&&a.name>b.name)||(a.dmy<b.dmy);
        });
    }
}

void FontPath::getNameFromTable(QDataStream &dt,QString& fontname,CodePage& dmy){
    //读TTF，找到字体的名字和代码页
    //读ttf表头
    OffsetTable ot;
    TTFLeibie jy=ot.read(dt);
    if(jy==tlERR){
        return;
    }
    //遍历表目录，找到name表和OS/2表
    TableRecord trName;
    TableRecord trOS2;
    for(int i=0;i<ot.numTables;i++){
        TableRecord tr;
        tr.read(dt);
        if(tr.tableTag=="name"){
            trName=tr;
        }else if(tr.tableTag=="OS/2"){
            trOS2=tr;
        }
    }

    class jl{
    public:
        jl(uint16 len=0,Offset16 so=0,int i=-1):length(len),stringOffset(so),index(i){}
        uint16 length;
        Offset16 stringOffset;
        int index;
    };
    static const int shibieYuyan=6;
    jl mz[shibieYuyan];
    if(trName.length!=0){
        //读取字体名字，在windows+Full font name中查找，优先顺序：中文>英文
        dt.device()->seek(trName.offset);
        uint64 posname=dt.device()->pos();
        uint16	version;//	Table version number (=0).
        uint16	count;//	Number of name records.
        Offset16	storageOffset;//
        dt>>version>>count>>storageOffset;
        for(int i=0;i<count;i++){
            uint16	platformID;//	Platform ID.
            uint16	encodingID;//	Platform-specific encoding ID.
            uint16	languageID;//	Language ID.
            uint16	nameID;//	Name ID.
            uint16	length;//	String length (in bytes).
            Offset16	stringOffset;
            dt>>platformID>>encodingID>>languageID>>nameID>>length>>stringOffset;
            if(nameID==4&&platformID==3){//full font name
                int idx=0;
                switch(languageID){
                case 0x0804:idx=0;break;
                case 0x0C04:idx=1;break;
                case 0x1404:idx=2;break;
                case 0x1004:idx=3;break;
                case 0x0404:idx=4;break;
                case 0x0409:idx=5;break;
                default:
                    idx=-1;
                }
                if(idx!=-1){
                    mz[idx]=jl(length,stringOffset,i);
                }
            }
        }
        int youxiao=-1;
        for(int i=0;i<shibieYuyan;i++){
            if(mz[i].index!=-1){
                youxiao=i;
                break;
            }
        }
        if(youxiao==-1){
            return;//没有名字，终止处理
        }
        //找到存储区
        dt.device()->seek(posname+storageOffset);
        int fzDatalen=trName.length-sizeof(uint16)*(3+count*6);
        char*pData=new char[fzDatalen];
        dt.readRawData(pData,fzDatalen);
        //拆解
        int len=mz[youxiao].length;
        char*p=&pData[mz[youxiao].stringOffset];
        char*pDst=new char[len];
        for(int i=0;i<len;i+=2){
            pDst[i]=p[i+1];
            pDst[i+1]=p[i];
        }
        fontname=QString::fromWCharArray((wchar_t*)pDst,len/sizeof(wchar_t));
        if(ot.sfntVersion==0x4F54544F){
            fontname="[OTF]"+fontname;
        }
        delete[]pDst;
        delete[]pData;            //
    }

    if(trOS2.length!=0){
        //读出代码页
        dt.device()->seek(trOS2.offset+78);//78字节到codepage1
        uint32 ulCodePageRange1=0;
        dt>>ulCodePageRange1;
        if(ulCodePageRange1&0x00040000){//如果有简体中文则标记为简体中文
            dmy=cp936;
        }else if(ulCodePageRange1&0x00100000){//如果无简体中文有繁体中文则标记为繁体中文
            dmy=cp950;
        }else{
            dmy=cpQita;
        }
    }
}
