#include<ttffile.h>

TtfFile::TtfFile(QObject *parent):QObject(parent),
    tuxingZongshu(0){
    QObject::connect(&post, &TableTTF::datashow, this, &TtfFile::appendTxt);
    QObject::connect(&vhea, &TableTTF::datashow, this, &TtfFile::appendTxt);
    QObject::connect(&vmtx, &TableTTF::datashow, this, &TtfFile::appendTxt);
    QObject::connect(&name, &TableTTF::datashow, this, &TtfFile::appendTxt);
    QObject::connect(&name, &NAMETable::showFontinfo, this, &TtfFile::showFontinfo);
    QObject::connect(&dsig, &TableTTF::datashow, this, &TtfFile::appendTxt);
    QObject::connect(&gpos, &TableTTF::datashow, this, &TtfFile::appendTxt);
    QObject::connect(&gpos, &TableTTF::jindu, this, &TtfFile::jindu);
    QObject::connect(&gdef, &TableTTF::datashow, this, &TtfFile::appendTxt);
    QObject::connect(&gsub, &TableTTF::datashow, this, &TtfFile::appendTxt);
    QObject::connect(&gsub, &TableTTF::jindu, this, &TtfFile::jindu);
    QObject::connect(&head, &HEADTable::setFontDate, this, &TtfFile::setFontDate);
    QObject::connect(&head, &TableTTF::datashow, this, &TtfFile::appendTxt);
    QObject::connect(&hhea, &TableTTF::datashow, this, &TtfFile::appendTxt);
    QObject::connect(&maxp, &TableTTF::datashow, this, &TtfFile::appendTxt);
    QObject::connect(&os2, &TableTTF::datashow, this, &TtfFile::appendTxt);
    QObject::connect(&hmtx, &TableTTF::datashow, this, &TtfFile::appendTxt);
    QObject::connect(&cmap, &TableTTF::datashow, this, &TtfFile::appendTxt);
    QObject::connect(&gasp, &TableTTF::datashow, this, &TtfFile::appendTxt);
    QObject::connect(&loca, &TableTTF::datashow, this, &TtfFile::appendTxt);
    QObject::connect(&glyf, &TableTTF::datashow, this, &TtfFile::appendTxt);
    QObject::connect(&glyf, &TableTTF::jindu, this, &TtfFile::jindu);
    QObject::connect(&meta, &TableTTF::datashow, this, &TtfFile::appendTxt);
    QObject::connect(&fpgm, &TableTTF::datashow, this, &TtfFile::appendTxt);
    QObject::connect(&cvt, &TableTTF::datashow, this, &TtfFile::appendTxt);
    QObject::connect(&prep, &TableTTF::datashow, this, &TtfFile::appendTxt);
    QObject::connect(&cff, &TableTTF::datashow, this, &TtfFile::appendTxt);
    glyf.setHmtx(&hmtx);
};

void TtfFile::deleteWenzi(){
    //glyphIdxSel从小到大排序。
    hmtx.deleteWenzi(toDelete);
    glyf.deleteWenzi(toDelete);
    cmap.deleteWenzi(toDelete);
}

void TtfFile::deleteTuxing(s32vector &glyphIdxSel){
    //删除图形
    hmtx.deleteTuxing(glyphIdxSel);
    glyf.deleteTuxing(glyphIdxSel);
}

void TtfFile::shanchuBianma(const s32vector&glyphIdxSel){
    cmap.setBianma0(glyphIdxSel);
}

void TtfFile::chaYinyong(int32 wmgid,QString& jg){
    s32vector gidv;
    glyf.chaYinyong(wmgid,gidv);
    QString zf;
    for(int32 gid: gidv){
        uint32 bm=cmap.zifuOrderByGlyphId[gid];
        if(bm==0){
            zf+=QString("[%1]").arg(gid);
        }else{
            zf+=QString::fromUcs4(&bm,1);
        }
    }
    jg=QString("%1个字：%2").arg(gidv.length()).arg(zf);
}

int TtfFile::appendBianma(QString& ipt){
    int bccnt=cmap.appendBianma(ipt);
    if(bccnt>0){
        hmtx.appendBianma(bccnt);
        glyf.appendBianma(bccnt);
    }
    return bccnt;
}
void TtfFile::appendZixing(uint16 jigeweizhi){
    u32vector weibianma(jigeweizhi,0);
    cmap.zifuOrderByGlyphId.append(weibianma);
    hmtx.appendBianma(jigeweizhi);
    glyf.appendBianma(jigeweizhi);
}

void TtfFile::zidongduiqi()
{
    glyf.zidongDuiqi(os2.fzZhongxian(),os2.sCapHeight);
}

int TtfFile::daxieduiqi(int32 glyphIdx)
{
    return glyf.tsx(glyphIdx,os2.sCapHeight,cdYmax);
}

int TtfFile::zuoyouyidong(int32 index, int newLsb){
    return glyf.zuoyouYidong(index,newLsb);
}

void TtfFile::moveRsb(int32 glyphIndex, int newAw){
    hmtx.fzhMetricsAll[glyphIndex].advanceWidth=newAw;
}

void TtfFile::xinjian1(xinjianMuban xm){
    cmap.xinjian(xm);
    tuxingZongshu=cmap.zifuOrderByGlyphId.length();
    hmtx.xinjian(tuxingZongshu);
    glyf.xinjian(tuxingZongshu);
}
void TtfFile::xinjian2(uint16 emsize){
    os2.xinjian(emsize);
}

void TtfFile::jisuan(const QString &ztmz, const QString &banben,
                     const QStringList&sm,const QStringList&smY,
                     qint64 riqi1970,uint16 emsize,int xiahuaxianGlyphIndex){
    if(emsize!=256&&emsize!=512&&emsize!=1024&&emsize!=2048){
        emsize=512;
    }
    //填写每个表
    uint32 u1,u2,u3,u4;
    uint16 cKaishi,cJieshu;
    s32vector azidx;//!<a-z字符的gid
    zidongZifu zz=cmap.jisuan(u1,u2,u3,u4,cKaishi,cJieshu,azidx);
    glyf.jisuan(loca,zz);
    tuxingZongshu=cmap.zifuOrderByGlyphId.length();
    lunkuoFanwei lfw;
    FWORD vMinRsb,vMaxExt,vMinlsb;
    uint16 jdLk,jdDian,fzLk,fzDian,avgAW;
    //glyf.jisuan以后才能计算glyf.bindbox,为了更新压缩数据。
    bool lsbAtY=glyf.bindBox(lfw,vMinlsb,vMinRsb,vMaxExt,
                             jdLk,jdDian,fzLk,fzDian,avgAW,azidx);
    head.jisuan(riqi1970,emsize,banben,lfw.xMin,lfw.yMin,lfw.xMax,lfw.yMax,lsbAtY,os2.panose[3]);
    maxp.jisuan(tuxingZongshu,jdLk,jdDian,fzLk,fzDian);
    UFWORD maxAw;
    uint16 vNumberOfHMetrics;
    //glyf.bindbox计算以后，才能hmtx.jisuan
    hmtx.jisuan(maxAw,vNumberOfHMetrics);
    lunkuoFanwei lfw_;
    bool yt=glyf.getLunkuofanwei(xiahuaxianGlyphIndex,lfw_);
    int16 houdu_=yt?(lfw_.yMax-lfw_.yMin):12;
    os2.jisuan(emsize,u1,u2,u3,u4,cKaishi,cJieshu,houdu_,avgAW);
    post.jisuan(os2.panose[3]==9,houdu_,yt?lfw_.yMax:0);
    hhea.jisuan(maxAw,
                vNumberOfHMetrics,vMaxExt,vMinlsb,vMinRsb,
                os2.sTypoAscender,os2.sTypoDescender,os2.sTypoLineGap);
    gdef.jisuan(tuxingZongshu-1);//字符数改变则重写为1.0。
    gpos.jisuan();
    meta.jisuan();
    name.jisuan(ztmz, banben,sm,smY);//按字体名字和版本，重写数据
    dsig.jisuan();
    gasp.jisuan();
    gsub.jisuan();
}
TTFLeibie TtfFile::readFrom(QDataStream &dt){
    TTFLeibie jg=ot.read(dt);
    if(jg==tlERR){
        return jg;
    }
    for(int i=0;i<ot.numTables;i++){
        TableRecord tr;
        tr.read(dt);
        pTableRecord.append(tr);
        appendTxt(QString("表%1：%2").arg(i+1).arg(tr.tableTag));
    }


    const int maxTableCountT=27;//可识别的表数量。按照写表顺序读。有则读取
    const QString ttfBodyStr[maxTableCountT]={"head","hhea","maxp","OS/2","hmtx",
                                              "LTSH", "VDMX", "hdmx",
                                              "cmap","fpgm","prep","cvt ",
                                              "loca","glyf","kern", "name","post","gasp","PCLT",
                                              "DSIG","GDEF","GPOS","GSUB","MERG","meta","vhea","vmtx"
                                             };
    const int maxTableCountO=16;//可识别的表数量。按照写表顺序读。有则读取
    const QString otfBodyStr[maxTableCountO]={"head","hhea","maxp","OS/2","name","cmap",
                                              "post","CFF ","FFTM","feat",
                                              "bsln","hmtx","morx","prop","vhea","vmtx"
                                             };
    appendTable(QString("0:表目录"));
    int showidx=1;
    int maxTableCount=jg==tlTTF?maxTableCountT:maxTableCountO;
    bool chabiaook=true;
    for(int bi=0;bi<maxTableCount;bi++){
        for(int i=0;i<ot.numTables;i++){
            QString tbname=pTableRecord[i].tableTag;
            if((jg==tlTTF&&tbname==ttfBodyStr[bi])||(jg==tlOTF&&tbname==otfBodyStr[bi])){
                uint32 offset=pTableRecord[i].offset;
                uint32 tblLen=pTableRecord[i].length;//一个表的长度
                chabiaook&=chabiao(dt,offset,tbname,tblLen);
                appendTable(QString("%1:%2").arg(showidx++).arg(tbname));
                break;
            }
        }
        jindu(100*bi/maxTableCount);
    }
    jindu(100);

    return chabiaook?jg:tlERR;
}

void TtfFile::saveTo(QDataStream &dt){
    //表头顺序 保存计算校验和
    enum tbHead{
        tbdsig,
        tbgdef,
#ifdef BAOCUN_GPOS
        tbgpos,
#endif
        tbgsub,
        tbos2,
        tbcmap,
        //tbcvt,
        //tbfpgm,
        tbgasp,
        tbglyf,
        tbhead,
        tbhhea,
        tbhmtx,
        tbloca,
        tbmaxp,
#ifdef BAOCUN_META
        tbmeta,
#endif
        tbname,
        tbpost,
        //tbprep
#ifdef BAOCUN_VHEA
        tbvhea,
#endif
#ifdef BAOCUN_VMTX
        tbvmtx,
#endif
        tableCount//目前需要存储的表的数量
    };

    //存储用表头顺序
    const QString tbHeadStr[tableCount]={
        "DSIG","GDEF",
    #ifdef BAOCUN_GPOS
        "GPOS",
    #endif
        "GSUB","OS/2","cmap",
        //"cvt ","fpgm",
        "gasp","glyf","head","hhea","hmtx","loca","maxp",
    #ifdef BAOCUN_META
        "meta",
    #endif
        "name","post",
        //"prep",
    #ifdef BAOCUN_VHEA
        "vhea",
    #endif
    #ifdef BAOCUN_VMTX
        "vmtx",
    #endif
    };

    ot.jisuan(tableCount);
    pTableRecord.clear();
    for(int i=0;i<tableCount;i++){
        TableRecord tr;
        tr.tableTag=tbHeadStr[i];
        pTableRecord.append(tr);
    }
    ot.save(dt);
    for(int i=0;i<ot.numTables;i++){
        pTableRecord[i].save(dt);
    }
    //按照写表顺序，存储指定表-------TTF
    int64 pos=dt.device()->pos();
    head.save(dt);
    CalcTableChecksum(dt,pos,tbhead);
    hhea.save(dt);
    CalcTableChecksum(dt,pos,tbhhea);
    maxp.save(dt);
    CalcTableChecksum(dt,pos,tbmaxp);
    os2.save(dt);
    CalcTableChecksum(dt,pos,tbos2);
    hmtx.save(dt);
    CalcTableChecksum(dt,pos,tbhmtx);
    cmap.save(dt);
    CalcTableChecksum(dt,pos,tbcmap);
    //fpgm.save(dt);
    //prep.save(dt);
    //cvt.save(dt);
    loca.save(dt);
    CalcTableChecksum(dt,pos,tbloca);
    glyf.save(dt);
    CalcTableChecksum(dt,pos,tbglyf);
    name.save(dt);
    CalcTableChecksum(dt,pos,tbname);
    post.save(dt);
    CalcTableChecksum(dt,pos,tbpost);
    gasp.save(dt);
    CalcTableChecksum(dt,pos,tbgasp);
    dsig.save(dt);
    CalcTableChecksum(dt,pos,tbdsig);
    gdef.save(dt);
    CalcTableChecksum(dt,pos,tbgdef);
#ifdef BAOCUN_GPOS
    gpos.save(dt);
    CalcTableChecksum(dt,pos,tbgpos);
#endif
    gsub.save(dt);
    CalcTableChecksum(dt,pos,tbgsub);
#ifdef BAOCUN_META
    meta.save(dt);
    CalcTableChecksum(dt,pos,tbmeta);
#endif
#ifdef BAOCUN_VHEA
    vhea.save(dt);
    CalcTableChecksum(dt,pos,tbvhea);
#endif
#ifdef BAOCUN_VMTX
    vmtx.save(dt);
    CalcTableChecksum(dt,pos,tbvmtx);
#endif

    //修改head校验和
    checksumAdjustment(dt,pTableRecord[tbhead].offset+8);//checksumAdjustment在head表的位置

}
void TtfFile::checksumAdjustment(QDataStream&dt,int64 headTableOffset)
{
    uint32 jyh=0;
    int64 toubuChangdu=12+ot.numTables*16;//偏移表长12，记录表一个16
    uint32 x;
    dt.device()->seek(0);
    for(int64 i=0;i<toubuChangdu;i+=4){
        dt>>x;
        jyh+=x;
    }
    for(int i=0;i<ot.numTables;i++){
        jyh+=pTableRecord[i].checkSum;
    }
    head.checksumAdjustment=0xB1B0AFBA-jyh;
    dt.device()->seek(headTableOffset);
    dt<<head.checksumAdjustment;
}

void TtfFile::CalcTableChecksum(QDataStream&dt, int64 &pos, int tbidx)
{
    //写入各表校验和
    //实际长度
    int64 pos2=dt.device()->pos();
    int64 tableLength=pos2-pos;
    //补零
    int bl=((tableLength+3) & ~3)-tableLength;
    for(int i=0;i<bl;i++){
        dt<<pad1;
    }
    //补零后总长度
    int64 pos3=pos2+bl;//dt.device()->pos();
    //计算校验和
    dt.device()->seek(pos);
    uint32 checksum=0;
    uint32 x;
    for(int64 i=pos;i<pos3;i+=4){
        dt>>x;
        checksum+=x;
    }
    //填表
    pTableRecord[tbidx].checkSum=checksum;
    pTableRecord[tbidx].length=tableLength;
    pTableRecord[tbidx].offset=pos;
    //更新表 os长度12，一个tableRecord长度16
    int64 tbPos=12+tbidx*16;
    dt.device()->seek(tbPos);
    pTableRecord[tbidx].save(dt);
    //更新pos
    pos=pos3;
    dt.device()->seek(pos);
}
void TtfFile::showTable(QString tbnameMix)
{
    if(tbnameMix==""){
        return;
    }
    QStringList tblist=tbnameMix.split(":");
    QString tbname=tblist[1];
    if(tblist[0]=="0"){
        QString tb;
        for(int i=0;i<pTableRecord.length();i++){
            tb+=QString("名称：%4 偏移:%1 长度:%2 校验和：%3\r\n")
                    .arg(pTableRecord[i].offset,8,16,QChar('0'))
                    .arg(pTableRecord[i].length,8,16,QChar('0'))
                    .arg(pTableRecord[i].checkSum,8,16,QChar('0'))
                    .arg(pTableRecord[i].tableTag);
        }
        appendTxt(tb);
        return;
    }


    if(tbname=="head"){
        head.show();
    }else if(tbname=="hhea"){
        hhea.show();
    }else if(tbname=="maxp"){
        maxp.show();
    }else if(tbname=="OS/2"){
        os2.show();
    }else if(tbname=="hmtx"){
        hmtx.show();
    }else if(tbname=="cmap"){
        cmap.show();
    }else if(tbname=="fpgm"){
        fpgm.show();
    }else if(tbname=="prep"){
        prep.show();
    }else if(tbname=="cvt "){
        cvt.show();
    }else if(tbname=="loca"){
        loca.show();
    }else if(tbname=="glyf"){
        glyf.show();
    }else if(tbname=="name"){
        name.show();
    }else if(tbname=="post"){
        post.show();
    }else if(tbname=="gasp"){
        gasp.show();
    }else if(tbname=="DSIG"){
        dsig.show();
    }else if(tbname=="GDEF"){
        gdef.show();
    }else if(tbname=="GPOS"){
        gpos.show();
    }else if(tbname=="GSUB"){
        gsub.show();
    }else if(tbname=="meta"){
        meta.show();
    }else if(tbname=="vhea"){
        vhea.show();
    }else if(tbname=="vmtx"){
        vmtx.show();
    }else if(tbname=="CFF "){
        cff.show();
    }
}

bool TtfFile::chabiao(QDataStream &dt,uint32 offset,QString tbname,uint32 tblLen){
    //true 格式正确
    //false 格式错误
    bool jg=true;
    if(tbname=="head"){
        head.read(dt,offset);
    }else if(tbname=="hhea"){
        hhea.read(dt,offset);
    }else if(tbname=="maxp"){
        maxp.read(dt,offset);
        this->tuxingZongshu=maxp.numGlyphs;
    }else if(tbname=="OS/2"){
        os2.read(dt,offset);
    }else if(tbname=="hmtx"){
        hmtx.read(dt,offset,hhea.numberOfHMetrics,maxp.numGlyphs);
    }else if(tbname=="cmap"){
        cmap.read(dt,offset,maxp.numGlyphs);
    }else if(tbname=="fpgm"){
        fpgm.read(dt,offset,tblLen);
    }else if(tbname=="prep"){
        prep.read(dt,offset,tblLen);
    }else if(tbname=="cvt "){
        cvt.read(dt,offset,tblLen);
    }else if(tbname=="loca"){
        loca.read(dt,offset,head.indexToLocFormat,maxp.numGlyphs);
    }else if(tbname=="glyf"){
        glyf.read(dt,offset,loca,maxp.numGlyphs);
    }else if(tbname=="name"){
        name.read(dt,offset,tblLen);
    }else if(tbname=="post"){
        post.read(dt,offset,tblLen);
    }else if(tbname=="gasp"){
        gasp.read(dt,offset);
    }else if(tbname=="DSIG"){
        dsig.read(dt,offset,tblLen);
    }else if(tbname=="GDEF"){
        gdef.read(dt,offset);
    }else if(tbname=="GPOS"){
        gpos.read(dt,offset);
    }else if(tbname=="GSUB"){
        gsub.read(dt,offset);
    }else if(tbname=="meta"){
        meta.read(dt,offset);
    }else if(tbname=="vhea"){
        vhea.read(dt,offset);
    }else if(tbname=="vmtx"){
        vmtx.read(dt,offset);
    }else if(tbname=="CFF "){
        jg=cff.read(dt,offset,maxp.numGlyphs);
    }
    return jg;
}

void TtfFile::getWubianma(s32vector& xianshiZifu,bool xianshiZuhe){
    //分别显示无编码的组合字 普通字
    s32vector x;//保存gid
    cmap.showAll("xsWeibianma",xianshiZifu);
    if(xianshiZuhe){
        glyf.getZuhezi(x);
    }else{
        glyf.getPutongzi(x);
    }
    QSet<int32> setx(x.begin(),x.end());
    setx.intersect(QSet<int32>(xianshiZifu.begin(),xianshiZifu.end()));
    xianshiZifu=QVector<int32>(setx.begin(),setx.end());
    sort(xianshiZifu.begin(),xianshiZifu.end());
}
