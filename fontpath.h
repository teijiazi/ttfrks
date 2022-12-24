#ifndef FONTPATH_H
#define FONTPATH_H

#include <QObject>
#include "ttcfile.h"
enum CodePage{
    cp936,//!< code page 936
    cp950,//!< code page 950
    cpQita,//!< code page 936 950以外的代码页/字符集
    cpStrCount
};

static const QString codePageStr[cpStrCount]={
    "中文GBK","繁体950","其他"
};

class KeyString{
public:
    KeyString(const QString&n,const QString&p, Offset32 of,CodePage vdmy):
        name(n),path(p),tableOffset(of),dmy(vdmy){}
    QString name;
    QString path;
    Offset32 tableOffset;
    CodePage dmy;
};

typedef QList<KeyString> KeyStringList;



class FontPath : public QObject
{
    Q_OBJECT
public:
    explicit FontPath(KeyStringList& vnamelist, QObject *parent = nullptr);
    static QString showVersion();
    void getFullFontName();
    static void getWPSexe(QString &wpsCmdline);
    /// 修改记事本的字体。win11无效
    static void setNotepadFont(const QString &ziti);
    void getNameFromFile(const QString &fontfilePath, bool paixu=false);
private:
    void getAllFontPathFromReg(int from);
    void getNameFromTable(QDataStream &dt, QString& fontname, CodePage &dmy);
    KeyStringList &namelist;
signals:

};

#endif // FONTPATH_H
