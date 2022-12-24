#include "mainwindow.h"
#include "../rizhi/rizhi.h"
#include "ui_mainwindow.h"

void MainWindow::on_xsSiyou_2_clicked()
{
    //查看 编辑连笔字
    if(!qb->isTTFok){
        return;
    }
    qb->xuanLianbizi(true);
    lbhb->jisuanSuofang(ttfFile->os2.usWinAscent,ttfFile->os2.usWinDescent);
    lbhb->clear();
    QStringList jbLst;
    ui->jiaoben->clear();
    ttfFile->gsub.getJiaoben(jbLst);
    ui->jiaoben->addItems(jbLst);
    setDialog(ui->lianbiCK,"连笔字",false,this,&MainWindow::lianbiziOver);
    ui->lianbiShuru->setFocus();
}

void MainWindow::lianbiziOver(int x)
{
    //连笔字窗口关闭
    Q_UNUSED(x);
    qb->xuanLianbizi(false);
    ui->jiaoben->clear();
    ui->yuyan->clear();
    ui->lbShuru->clear();
    ui->lbZibiao->clear();
    ui->yuyandefault->setChecked(false);
    ui->yuyandefault->setEnabled(false);
}
void MainWindow::xuanhaoLianbizi(uint32 lbzGlyphIndex){
    //选好连笔字
    int ji=ui->jiaoben->currentRow();
    int yi=ui->yuyan->currentRow();
    int zi=ui->lbZibiao->currentRow();
    int ci=ui->lbCover->currentRow();
    int li=ui->lbShuru->currentRow();
    bool you=ui->yuyandefault->isEnabled();
    bool xuan=ui->yuyandefault->isChecked();
    if(ji==-1||li==-1||zi==-1||ci==-1){
        return;
    }
    if(you){
        if((!xuan) && yi==-1){
            return;
        }
    }else{
        if(yi==-1){
            return;
        }
    }
    //
    lbhb->huazifu(lbzGlyphIndex);
    lbhb->repaint();
    //数据更新
    ttfFile->gsub.gengxinLiga(ji,yi,zi,ci,li,lbzGlyphIndex);
    //
    this->shuchuGlyphIdx[li]=lbzGlyphIndex;
}
void MainWindow::on_sfda_2_clicked()
{
    //加一组连笔字
    QString jcsr=ui->lianbiShuru->text().trimmed();
    if(jcsr.length()==0){
        tishi->showinfo("1 没有字");
        return;
    }
    QVector<uint> shuru=jcsr.toUcs4();
    u16vector shuruGlyphIndex;
    for(int i=0;i<shuru.length();i++){
        int32 glyphIndex=qb->chazhao(shuru[i]);
        if(glyphIndex!=-1){
            shuruGlyphIndex.append(glyphIndex);
        }else{
            tishi->showinfo("字库中的字不全");
            return;
        }
    }
    if(ui->jiaoben->currentRow()<0){
        xinzengJiaoben(shuruGlyphIndex);
        return;
    }
    if(!ui->yuyandefault->isChecked()&&ui->yuyan->currentRow()<0){
        snd<<ui->jiaoben->currentRow();
        tishi->showinfo("没有选语言");
        return;
    }

    auto fd=ui->lbShuru->findItems(jcsr,Qt::MatchExactly);
    if(fd.length()>0){
        tishi->showinfo("已存在 "+jcsr);
        return;
    }
    //
    //补充一个lookup--对应的语言如果存在liga则补充，没有则新建---不操作。
    int jgCoverIdx=0, jgShuruIdx=0;
    bool insertSuc=ttfFile->gsub.insertLiga(ui->jiaoben->currentRow(),
                                            ui->yuyan->currentRow(),
                                            ui->lbZibiao->currentRow(),
                                            shuruGlyphIndex,
                                            jgCoverIdx, jgShuruIdx);
    if(insertSuc){
        int zbIdx=ui->lbZibiao->currentRow();
        ui->lbZibiao->setCurrentRow(-1);
        ui->lbZibiao->setCurrentRow(zbIdx);
    }else{
        on_yuyandefault_clicked(true);
    }
    ui->lbCover->setCurrentRow(jgCoverIdx);
    ui->lbShuru->setCurrentRow(jgShuruIdx);
}

void MainWindow::xinzengJiaoben(const u16vector& shuru){
    ////新增一组(脚本+语言+功能+查询)
    int jgCoverIdx=0, jgShuruIdx=0;
    int jgIndex=ttfFile->gsub.addLoopupLiga(ui->jiaobenHani->isChecked()?"hani":"latn",
                                            shuru,jgCoverIdx, jgShuruIdx);
    if(jgIndex==-1){
        tishi->showinfo(QString("先在列表中选 ")+(ui->jiaobenHani->isChecked()?"hani":"latn"));
        return;
    }
    //更新脚本
    ui->jiaoben->clear();
    QStringList jbLst;
    ttfFile->gsub.getJiaoben(jbLst);
    ui->jiaoben->addItems(jbLst);
    //更新
    ui->jiaoben->setCurrentRow(jgIndex);
    ui->yuyandefault->setChecked(true);
    on_yuyandefault_clicked(true);
    ui->lbCover->setCurrentRow(jgCoverIdx);
    ui->lbShuru->setCurrentRow(jgShuruIdx);
}

void MainWindow::on_sfxiao_2_clicked()
{
    /// 删一组连笔字
    /// 删除选中的ligaTable.
    ///        当ligaTable全部删除时，删除ligaSet
    ///        当ligaSet全部删除时，删除lookupTable
    /// 删除lookupTable后，feature中剔除对应的索引。
    /// feature中没有索引时，删除功能，脚本中剔除对应的索引。
    /// 脚本中没有功能时，删除脚本。

    int si=ui->lbShuru->currentRow();
    if(si==-1){
        return;
    }
    int ji=ui->jiaoben->currentRow();
    int yi=ui->yuyan->currentRow();
    int zi=ui->lbZibiao->currentRow();
    int ci=ui->lbCover->currentRow();
    ttfFile->gsub.shanchuLiga(ji,yi,zi,ci,si);
    ui->jiaoben->setCurrentRow(-1);
    if(ji==-1){
        ui->jiaoben->clear();
        QStringList jbLst;
        ttfFile->gsub.getJiaoben(jbLst);
        ui->jiaoben->addItems(jbLst);
    }else{
        ui->jiaoben->setCurrentRow(ji);
        if(yi==-1){
            ui->yuyandefault->setChecked(true);
            on_yuyandefault_clicked(true);
        }else{
            ui->yuyan->setCurrentRow(yi);
        }
        ui->lbZibiao->setCurrentRow(zi);
        ui->lbCover->setCurrentRow(ci);
        ui->lbShuru->setCurrentRow(si);
    }
}
void MainWindow::on_jiaobenHani_clicked()
{
    ui->jiaoben->setCurrentRow(-1);
}

void MainWindow::on_jiaobenLatn_clicked()
{
    ui->jiaoben->setCurrentRow(-1);
}


void MainWindow::on_jiaoben_currentRowChanged(int currentRow)
{
    //点击脚本，显示语言
    ui->yuyan->clear();
    ui->yuyandefault->setChecked(false);
    ui->yuyandefault->setEnabled(false);
    ui->lbShuru->clear();
    ui->lbZibiao->clear();
    ui->lbCover->clear();
    lbhb->clear();
    if(currentRow==-1){
        return;
    }
    QStringList yyLst;
    bool youDef=false;
    ttfFile->gsub.getYuyan(yyLst,currentRow,youDef);
    ui->yuyan->addItems(yyLst);
    ui->yuyandefault->setEnabled(youDef);
    ui->yuyandefault->setChecked(false);
}

void MainWindow::on_yuyandefault_clicked(bool checked)
{
    if(!checked){
        return;
    }
    //查看默认语言的liga
    ui->yuyan->setCurrentRow(-1);
    yuyan2Liga(-1);
}
void MainWindow::on_yuyan_currentRowChanged(int currentRow)
{
    //查看指定语言的liga
    if(currentRow==-1){
        return;
    }
    if(ui->yuyandefault->isEnabled()){
        ui->yuyandefault->setChecked(false);
    }
    yuyan2Liga(currentRow);
}

void MainWindow::yuyan2Liga(int yuyanIndex){
    //点击语言，显示子表
    QStringList lgdiyi;
    ui->lbShuru->clear();
    ui->lbZibiao->clear();
    ui->lbCover->clear();
    lbhb->clear();
    uint16 zibiao=0;
    ttfFile->gsub.getLigaZibiao(ui->jiaoben->currentRow(),
                                yuyanIndex,zibiao);
    for(int i=0;i<zibiao;i++){
        lgdiyi.append(QString::number(i+1));
    }
    ui->lbZibiao->addItems(lgdiyi);
    if(lgdiyi.length()>0){
        ui->lbZibiao->setCurrentRow(0);
    }
}

void MainWindow::on_lbZibiao_currentRowChanged(int currentRow)
{
    //点击子表，显示Cover
    if(currentRow==-1){
        return;
    }
    ui->lbShuru->clear();
    ui->lbCover->clear();
    lbhb->clear();
    u16vector cover;
    ttfFile->gsub.getLigaCover(cover,
                               ui->jiaoben->currentRow(),
                               ui->yuyan->currentRow(),
                               currentRow);
    QStringList lgdiyi;
    for(int i=0;i<cover.length();i++){
        lgdiyi.append(QString::fromUcs4(&ttfFile->cmap.zifuOrderByGlyphId[cover[i]],1));
    }
    ui->lbCover->addItems(lgdiyi);
    if(lgdiyi.length()>0){
        ui->lbCover->setCurrentRow(0);
    }
}
void MainWindow::on_lbCover_currentRowChanged(int currentRow)
{
    //点击Cover显示liga
    if(currentRow==-1){
        return;
    }
    ui->lbShuru->clear();
    lbhb->clear();
    QStringList lgzf;
    QList<u16vector> shuruGlyList;
    u16vector zibiao;
    shuchuGlyphIdx.clear();
    ttfFile->gsub.getLigaZifu(shuruGlyList,
                              shuchuGlyphIdx,
                              ui->jiaoben->currentRow(),
                              ui->yuyan->currentRow(),
                              ui->lbZibiao->currentRow(),
                              currentRow);
    for(int i=0;i<shuruGlyList.length();i++){
        u16vector& zf=shuruGlyList[i];
        QString x;
        for(int vi=0;vi<zf.length();vi++){
            x+=QString::fromUcs4(&ttfFile->cmap.zifuOrderByGlyphId[zf[vi]],1);
        }
        lgzf.append(x);
    }
    ui->lbShuru->addItems(lgzf);
}
void MainWindow::on_lbShuru_currentRowChanged(int currentRow)
{
    //点击liga输入，查看不同的连笔字
    if(currentRow<0 || currentRow>=shuchuGlyphIdx.length()){
        return;
    }
    ui->lianbiShuru->setText(ui->lbShuru->currentItem()->text());
    //
    lbhb->huazifu(shuchuGlyphIdx[currentRow]);
    lbhb->repaint();
}
