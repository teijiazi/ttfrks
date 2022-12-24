#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QFile>
#include <QFileDialog>
#include <QMainWindow>
#include <QScrollBar>
#include "tableGlyfLoca.h"
#include <QClipboard>
#include <QMenu>
#include <QMimeData>
#include "ttcfile.h"
#ifdef opencvsrc
#include "imgproc.hpp"
#endif

void MainWindow::setDialog(QWidget* neibuchuangkou, const QString &title, bool modal,
                           const QtPrivate::FunctionPointer<voidInt>::Object *receiver,
                           voidInt funcDialogCloseSlot){
    XIANSHIhanshuming;
    dlg->setModal(modal);
    dlg->setGeometry(neibuchuangkou->geometry());
    if(dlg->children().length()>0){
        static_cast<QWidget*>(dlg->children().first())->hide();
        dlg->children().first()->setParent(this);
    }
    neibuchuangkou->setParent(dlg);
    neibuchuangkou->move(0,0);
    dlg->move((this->width()-dlg->width())/2+this->x(),
              this->height()/5+this->y());
    dlg->setWindowTitle(title);
    neibuchuangkou->show();
    disconnect(dlg,&QDialog::finished,nullptr,nullptr);
    if(funcDialogCloseSlot){
        connect(dlg,&QDialog::finished,receiver,funcDialogCloseSlot);
    }
    dlg->show();
}

void MainWindow::on_zaozi_clicked()
{
    XIANSHIhanshuming;
    if(!qb->isTTFok){
        return;
    }
    if(qb->currentSelidx()==-1){
        return;
    }
    //临时造字 按图片造一个字
#ifdef QT_DEBUG
    QString x="E:/文字/xxx.jpg";
#else
    QString x=QFileDialog::getOpenFileName(nullptr,
                                           "选一张图片",
                                           ib.bc.imgpath,"JPG (*.jpg);;PNG图片 (*.png);;其他图片 (*.gif *.bmp)");
#endif
    if(x==""){
        return;
    }
    QRegularExpression reg("(.*)/.*");
    auto mt=reg.match(x);
    ib.bc.imgpath=mt.captured(1);
    //Graphisview 显示图片
    if(scene){
        delete scene;
        scene=nullptr;
    }
    scene= new QGraphicsScene(this);
    scene->addPixmap(QPixmap::fromImage(QImage(x)));
    if(yt){
        delete yt;
        yt=nullptr;
    }
    yt=new Kanquantu(ui->yuantuRQ);
    yt->setScene(scene);
    yt->show();
    //数值更新
    ui->menValue->setText(QString::number(ib.bc.menxian));
    ui->guaidianValue->setText(QString::number(ib.bc.guaidian));
    ui->zhixianValue->setText(QString::number(ib.bc.zhixian));
    ui->lvbosize->setText(QString::number(ib.bc.lvbo));
    //传递os参数
    zzhb->jisuanSuofang(ttfFile->os2.usWinAscent,ttfFile->os2.usWinDescent);
    //打开对话框
    setDialog(ui->zzsetck,"造字效果",false);
    //获取文字笔迹
    ib.getLKfrom(x, ttfFile->os2.usWinAscent);
    updateZaoziHuabu();
}

void MainWindow::updateZaoziHuabu(int nihe){
    //按图片造字，更新图形
    //mtx.lock();
    XIANSHIhanshuming;
    Biji biji;
    int lks;
    ib.xiugaiCanshu(biji,lks,
                    ui->gaosi->isChecked(),ui->fanse->isChecked(),
                    ui->menxian->value(),ui->zhixian->value()/100.0,
                    ui->lvbo->value(),ui->guaidian->value()/100.0,ui->fenduan->isChecked(),
                    nihe);

    int32 idx=qb->tihuanZifu(biji,lks,ui->baochi->isChecked(),ui->shangxiaBuyuejie->isChecked());
    zzhb->huazifu(idx);
    zzhb->repaint();
    if(ui->tabWidget->currentIndex()==tiLunkuoxiugai){
        kanlunkuo();
    }
    //mtx.unlock();
}



void MainWindow::on_shangxiaBuyuejie_clicked()
{
    updateZaoziHuabu();
}

void MainWindow::on_guaidian_valueChanged(int value)
{
    XIANSHIhanshuming;
    ui->guaidianValue->setText(QString::number(value/100.0));
    updateZaoziHuabu();
}

void MainWindow::on_fenduan_clicked()
{
    XIANSHIhanshuming;
    updateZaoziHuabu();
}

void MainWindow::on_zhixian_valueChanged(int value)
{
    XIANSHIhanshuming;
    ui->zhixianValue->setText(QString::number(value/100.0));
    updateZaoziHuabu();
}

void MainWindow::on_menxian_valueChanged(int value)
{
    XIANSHIhanshuming;
    ui->menValue->setText(QString::number(value));
    updateZaoziHuabu();
}

void MainWindow::on_gaosi_clicked(bool checked)
{
    XIANSHIhanshuming;
    Q_UNUSED(checked);
    updateZaoziHuabu();
}

void MainWindow::on_fanse_clicked(bool checked)
{
    XIANSHIhanshuming;
    Q_UNUSED(checked);
    updateZaoziHuabu();
}

void MainWindow::on_gaosi_5_clicked()
{
    XIANSHIhanshuming;
    updateZaoziHuabu(CHAIN_APPROX_TC89_KCOS);
}

void MainWindow::on_gaosi_2_clicked()
{
    XIANSHIhanshuming;
    updateZaoziHuabu(CHAIN_APPROX_NONE);
}

void MainWindow::on_gaosi_4_clicked()
{
    XIANSHIhanshuming;
    updateZaoziHuabu(CHAIN_APPROX_TC89_L1);
}

void MainWindow::on_gaosi_3_clicked()
{
    XIANSHIhanshuming;
    updateZaoziHuabu(CHAIN_APPROX_SIMPLE);
}

void MainWindow::on_baochi_clicked()
{
    XIANSHIhanshuming;
    updateZaoziHuabu();
}
void MainWindow::on_lvbo_valueChanged(int value)
{
    XIANSHIhanshuming;
    ui->lvbosize->setText(QString::number(value));
    updateZaoziHuabu();
}

void MainWindow::on_save_2_clicked()
{
    if(ui->wjm->toolTip().trimmed()=="")return;
    //安装看效果
    system("start \"\" \""+ui->wjm->toolTip().toLocal8Bit()+"\" /MIN");
}

void MainWindow::on_delTTF_clicked()
{
    XIANSHIhanshuming;
    //打开字体文件夹
    system("explorer c:\\windows\\fonts");
}


void MainWindow::on_save_4_clicked()
{
    //打开记事本
    //先设置记事本的字体
    FontPath::setNotepadFont(ui->ztmz->text());
    jincheng->terminate();
    jincheng->waitForFinished(3000);
    jincheng->start("notepad.exe", QStringList());
}

void MainWindow::on_save_3_clicked()
{
    //打开wps
    QString wpspath;
    FontPath::getWPSexe(wpspath);
    QStringList cs;
    cs<<"/wps";
    jincheng->terminate();
    jincheng->waitForFinished(3000);
    jincheng->start(wpspath, cs);
}

void MainWindow::on_toolButton_17_clicked()
{
    //放大
    ttfFile->glyf.suofang(xiugaiGlyphIndex,true);
    kanlunkuo();
}

void MainWindow::on_toolButton_16_clicked()
{
    //缩小
    ttfFile->glyf.suofang(xiugaiGlyphIndex,false);
    kanlunkuo();
}

void MainWindow::on_insert_6_clicked()
{
    //批量造字--按钮点击，填写设置
    if(!qb->isTTFok){
        return;
    }
    ui->imgpath->setText(ib.bc.imgpath);
    ui->piliangCk->show();
    setDialog(ui->piliangCk,"批量造字",false);
}

void MainWindow::on_piliangZaoziKaishi_clicked()
{
    //开始批量造字
    bool ok=false;
    uint qishi=ui->fnQishi->text().toUInt(&ok);
    if(!ok){
        tishi->showinfo("起始和终止都是数字");
        return;
    }
    uint zhongzhi=ui->fnZhongzhi->text().toUInt(&ok);
    if(!ok){
        tishi->showinfo("起始和终止都是数字");
        return;
    }
    dlg->accept();
    QString imgExt=ui->ibJpg->isChecked()?"jpg":(ui->ibPng->isChecked()?"png":"gif");
    for(uint i=qishi;i<=zhongzhi;i++){
        QString fn=ib.bc.imgpath+"/"+ui->fnPre->text().trimmed()+" ("+QString::number(i)+")."+imgExt;
        ib.getLKfrom(fn, ttfFile->os2.usWinAscent);
        Biji biji;
        int lks;
        ib.piliangShiyong(biji,lks);
        qb->tihuanZifu(biji,lks,ui->baochi->isChecked(),ui->shangxiaBuyuejie->isChecked());
        qb->next();
    }
}


void MainWindow::on_insert_7_clicked()
{
    //添加没有编码的字
    XIANSHIhanshuming;
    if(!qb->isTTFok){
        return;
    }
    if(qb->bunengXiugai()){
        tishi->showinfo("不能修改");
        return;
    }
    ttfFile->appendZixing(10);
    qb->showAll(true);
    ui->tabWidget->setTabText(tiWenziliebiao,"全部文字");
}

void MainWindow::on_insert_clicked()
{
    //插入文字到字库末尾
    if(qb->bunengXiugai()){
        tishi->showinfo("不能修改");
        return;
    }
    //插入文字到最后
    if(!qb->isTTFok){
        return;
    }

    pte->clear();
    setDialog(ui->insertBox,"增加文字或编码");
    pte->setFocus();
}

void MainWindow::on_charu_clicked()
{
    //插入文字到最后
    QString ipt=pte->toPlainText();
    int bccnt=ttfFile->appendBianma(ipt);
    if(bccnt>0){
        qb->showAll(true);
        ui->danziXinxi->clear();
        ui->tabWidget->removeTab(tiLunkuoxiugai);
        ui->tabWidget->setTabText(tiWenziliebiao,"全部文字");
        dlg->accept();
    }else{
        tishi->showinfo(bccnt==0?"文字或编码已存在":"输入字符无效");
    }
}
void MainWindow::on_charu_2_clicked()
{
    //修改为组合字
    //"3521z53s0.8你他 偏旁gid 剩余在右 剩余左移53
    QString ipt=pte2->toPlainText().trimmed();//
    QRegularExpression reg("(\\d+)(.*)");    //找到开头的数字---偏旁的glyphIndex
    auto x=reg.match(ipt);
    int pianpangGid=0;
    QString ptz=ipt;
    if(x.hasMatch()){
        pianpangGid=x.captured(1).toUInt();
        ptz=x.captured(2);
    }
    u32vector uni=ptz.toUcs4();              //普通字改为组合字
    for(int i=0;i<uni.length();i++){
        int gid=qb->chazhao(uni[i]);
        if(gid!=-1){
            ttfFile->glyf.gaiweiZuhezi(gid,pianpangGid,
                                       ui->ppx->text().trimmed().toInt(),ui->ppy->text().trimmed().toInt(),
                                       ui->ppxs->text().trimmed().toDouble(),ui->ppys->text().trimmed().toDouble(),
                                       ui->syx->text().trimmed().toInt(),ui->syy->text().trimmed().toInt(),
                                       ui->syxs->text().trimmed().toDouble(),ui->syys->text().trimmed().toDouble());
        }
    }
    dlg->accept();
    qb->repaint();
}
void MainWindow::on_plzh_clicked()
{
    //3521z53s0.8你他 偏旁gid 剩余在右 剩余左移53
    if(qb->bunengXiugai()){
        tishi->showinfo("不能修改");
        return;
    }
    //插入文字到最后
    if(!qb->isTTFok){
        return;
    }
    pte2->clear();
    setDialog(ui->bianzuheBox,"修改为组合字",false);
    pte2->setFocus();
}
