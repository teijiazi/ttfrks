#ifndef TABLEGSUB_H
#define TABLEGSUB_H
#include<tableComm.h>

class GSUBTable : public TableTTF
{
    Q_OBJECT
    void reset();
public:
    GSUBTable(QObject *parent = nullptr);
    ~GSUBTable();
    uint16	majorVersion;//	Major version of the GSUB table, = 1
    uint16	minorVersion;//	Minor version of the GSUB table, = 0
    Offset16	scriptListOffset;//	Offset to ScriptList table, from beginning of GSUB table
    Offset16	featureListOffset;//	Offset to FeatureList table, from beginning of GSUB table
    Offset16	lookupListOffset;//	Offset to LookupList table, from beginning of GSUB table
    //ver1.1
    Offset32	featureVariationsOffset;
    ScriptListTable* scriptListTable;
    FeatureListTable* featureListTable;
    LookupListTable* lookupListTable;
    QString info;
    void read(QDataStream& dt, uint32 offset);
    void save(QDataStream& dt);
    void show();
    void jisuan();
    ///列出现有脚本，存入jbLst-----ScriptRecord.scriptTag
    void getJiaoben(QStringList &jbLst);
    void getYuyan(QStringList &yyLst, int jiaobenIndex, bool &youDef);
    bool insertLiga(int jiaobenIndex, int yuyanIndex, int zibiaoIndex, const u16vector &shuru,
                    int &jgCoverIdx, int &jgShuruIdx);
    void gengxinLiga(int jiaobenIndex, int yuyanIndex, int zibiaoIndex, int coverIndex, int li, int16 shuchuGlyphIndex);
    void getLigaZifu(QList<u16vector> &shuruGlyList, u16vector &shuchuGlyphIdx,
                     int jiaobenIndex, int yuyanIndex, int zibiaoIndex, int coverIndex);
    void getLigaCover(u16vector &cover, int jiaobenIndex, int yuyanIndex, int zibiaoIndex);
    void getLigaZibiao(int jiaobenIndex, int yuyanIndex, uint16 &zibiaoShu);
    int addLoopupLiga(const QString &jiaoben, const u16vector &shuru, int& jgCoverIdx, int& jgShuruIdx);
    void shanchuLiga(int &ji, int &yi, int &zi, int &ci, int &si);
    int addLoopupVert(const QString &jiaoben, const u16vector &heng, const u16vector &shu);
    void tihuanVert(int jiaobenIndex, const u16vector &heng, const u16vector &shu);
};

#endif // TABLEGSUB_H
