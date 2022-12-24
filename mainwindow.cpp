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
#include <QSet>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow),ttfFile(nullptr),scene(nullptr),yt(nullptr),pte(nullptr),pte2(nullptr),
      jincheng(nullptr),keyiBaocun(false),saveLiebiaoPos(0)
{
    XIANSHIhanshuming;
    ui->setupUi(this);
    //
    QMenu *xjCaidan=new QMenu("xx",this);//新建
    xjCaidan->addAction("3500",this,&MainWindow::on_xinjian_clicked);
    xjCaidan->addAction("6500",this,&MainWindow::on_xinjian_clicked);
    ui->xinjian->setMenu(xjCaidan);
    //
    QMenu *zzCaidan=new QMenu("zz",this);//造字
    zzCaidan->addAction("CAD字体转入",this,&MainWindow::on_zaozi_clicked);
    zzCaidan->addAction("AI设计转入",this,&MainWindow::on_zaozi_clicked);
    ui->zaozi->setMenu(zzCaidan);
    //存储
    QMenu *ccCaidan=new QMenu("cc",this);//存储
    ccCaidan->addAction("另存为",this,&MainWindow::on_save_clicked,QKeySequence(tr("Ctrl+Shift+S")));
    ui->save->setMenu(ccCaidan);
    //
    tishi=new TishiChuangkou(this);
    pte=new PlainTextEx(ui->insertBox,QRect(10,30,351,131));
    connect(pte,&PlainTextEx::enterPressed,this,&MainWindow::on_charu_clicked);
    pte2=new PlainTextEx(ui->bianzuheBox,QRect(10,90,351,101));
    connect(pte2,&PlainTextEx::enterPressed,this,&MainWindow::on_charu_2_clicked);
    //
    ht=new huatu;
    connect(ht,&huatu::showMousePos,this,&MainWindow::showMousePos);
    connect(ht,&huatu::updateBfb,ui->bfb,&QToolButton::setText);
    connect(ht,&huatu::tsx,this,&MainWindow::duiqifuzhu);
    connect(ht,&huatu::updataMaodianUI,this,&MainWindow::updateMaodianUI);
    ui->htGundong->setWidget(ht);
    ht->setScroll(ui->htGundong);
    qb=new quanbu;
    connect(qb,&quanbu::chaziOk,this,&MainWindow::showDanzi);
    connect(qb,&quanbu::kanlunkuo,this,&MainWindow::kanlunkuo);
    connect(qb,&quanbu::bujianXuanhao,this,&MainWindow::xuanzeBujian);
    connect(qb,&quanbu::xuanhaoLianbizi,this,&MainWindow::xuanhaoLianbizi);
    connect(qb,&quanbu::errorshow,tishi,&TishiChuangkou::showinfo);
    connect(qb,&quanbu::jizhuWeizhi,this,&MainWindow::jizhuWeizhi);
    ui->gundong->setWidget(qb);
    qb->setScroll(ui->gundong);
    //editName
    int width=300,height=50,wl=110;
    QWidget * rq=new QWidget(this);
    rq->setFixedHeight(height*YudingyiNameCount);
    rq->setFixedWidth(width*2+wl+20);
    int top=10;
    for(int i=0;i<YudingyiNameCount;i++){
        QLabel* l=new QLabel(rq);
        int xingao=((i>=1&&i<=6)||i==15)?30:height;
        l->setGeometry(10,top+5,wl-10,20);
        l->setText(NameRecord::yudingyiStr[i]);
        QPlainTextEdit * pt=new QPlainTextEdit(rq);
        pt->setGeometry(wl,top,width,xingao);
        QPlainTextEdit * ptY=new QPlainTextEdit(rq);
        ptY->setGeometry(width+wl+20,top,width,xingao);
        //
        top+=xingao;
        //
        if((i>=1&&i<=6)||i==15){//1~6自动填写
            pt->setEnabled(false);
            ptY->setEnabled(false);
        }
        namePte.append(pt);
        namePteY.append(ptY);
    }
    ui->nameGundong->setWidget(rq);
    //
    QMenu *fzCaidan=new QMenu("fz",this);//复制
    fzCaidan->addAction("复制图形",qb,&quanbu::fuzhiLunkuoDian,QKeySequence(tr("Ctrl+D")));
    fzCaidan->addAction("复制文字",qb,&quanbu::fuzhiWenzi,QKeySequence(tr("Ctrl+C")));
    fzCaidan->addAction("复制全部编码",qb,&quanbu::fuzhiBianmaAll);
    ui->fuzhi->setMenu(fzCaidan);
    connect(ui->fuzhi,&QToolButton::clicked,qb,&quanbu::fuzhiBianma);
    //
    zzhb=new ToolButtonEx(ui->xieziBan,0,QColor(0,0,0),&ttfFile->glyf);
    zzhb->setGeometry(0,0,ui->xieziBan->width(),ui->xieziBan->height());
    lbhb=new ToolButtonEx(ui->lbRQ,0,QColor(0,0,0),&ttfFile->glyf);
    lbhb->setGeometry(0,0,ui->lbRQ->width(),ui->lbRQ->height());
    dlg=new QDialog(this);

    ui->tabWidget->setFont(QFont("微软雅黑"));
    //
    uilsb = new SpinEditEx(ui->lsbstr,this,&MainWindow::lsbValueChanged);
    uiaw  = new SpinEditEx(ui->awstr,this, &MainWindow::awValueChanged);
    uisbj = new SpinEditEx(ui->sbjstr,this,&MainWindow::sbjValueChanged);
    uixbj = new SpinEditEx(ui->xbjstr,this,&MainWindow::xbjValueChanged);
    uitsx = new SpinEditEx(ui->tsxstr,this,&MainWindow::tsxValueChanged);
    //
    connect(ui->shangyige,&QToolButton::clicked,this,&MainWindow::xiayige);
    connect(ui->xiayige,&QToolButton::clicked,this,&MainWindow::xiayige);
    connect(ui->dqjixian,&QToolButton::clicked,this,&MainWindow::tsxYushe);
    connect(ui->dqdaxiexian,&QToolButton::clicked,this,&MainWindow::tsxYushe);
    connect(ui->dqzhongxian,&QToolButton::clicked,this,&MainWindow::tsxYushe);
    connect(ui->toolButton_14,&QToolButton::clicked,ht,&huatu::showPoint);//显示控制点
    connect(ui->toolButton_22,&QToolButton::clicked,ht,&huatu::showLunkuoIndex);//显示轮廓序号
    connect(ui->xsAll,&QToolButton::clicked,this,&MainWindow::xsBd);//分组显示
    connect(ui->xsZuhe,&QToolButton::clicked,this,&MainWindow::xsBd);//分组显示
    connect(ui->xsPutong,&QToolButton::clicked,this,&MainWindow::xsBd);//分组显示
    connect(ui->xsZuhetu,&QToolButton::clicked,this,&MainWindow::xsBd);//分组显示
    connect(ui->xsPutongtu,&QToolButton::clicked,this,&MainWindow::xsBd);//分组显示
    connect(ui->xsQita,&QToolButton::clicked,this,&MainWindow::xsBd);//分组显示
    connect(ui->xsBiaodian,&QToolButton::clicked,this,&MainWindow::xsBd);//分组显示
    connect(ui->xsShuzi,&QToolButton::clicked,this,&MainWindow::xsBd);//分组显示
    connect(ui->xsYingwen,&QToolButton::clicked,this,&MainWindow::xsBd);//分组显示
    connect(ui->xsMeixie,&QToolButton::clicked,this,&MainWindow::xsBd);//分组显示
    connect(ui->xsZhongwen,&QToolButton::clicked,this,&MainWindow::xsBd);//分组显示
    connect(ui->xsWeibianma,&QToolButton::clicked,this,&MainWindow::xsBd);//分组显示
    connect(ui->xsPianpang,&QToolButton::clicked,this,&MainWindow::xsBd);//分组显示
    connect(ui->xsSiyou,&QToolButton::clicked,this,&MainWindow::xsBd);//分组显示
    connect(ui->xsXila,&QToolButton::clicked,this,&MainWindow::xsBd);//分组显示
    connect(ui->xsPinyin,&QToolButton::clicked,this,&MainWindow::xsBd);//分组显示
    connect(ui->xsZhuyin,&QToolButton::clicked,this,&MainWindow::xsBd);//分组显示
    connect(ui->xsShuxue,&QToolButton::clicked,this,&MainWindow::xsBd);//分组显示
    //
    ui->delTTF->setParent(ui->listWidget);
    ui->listWidget->setParent(this);
    winit();
    //
    this->setWindowTitle(FontPath::showVersion());
    jincheng=new QProcess;
}

void MainWindow::winit(){
    XIANSHIhanshuming;
    ui->wjm->setText("");
    ui->ztmz->setText("");
    ui->banben->setValue(1.0);
    ui->riqi->setDateTime(QDateTime::currentDateTime());
    ui->tbl->clear();
    ui->tabWidget->setCurrentIndex(0);
    ui->qitaxinxi->clear();
    ui->danziXinxi->clear();
    ui->ttfxy->clear();
    ui->bfb->setText("100%");
    ui->sfda->hide();
    ui->sfxiao->hide();
    ui->shangyige->hide();
    ui->xiayige->hide();
    ui->lianbiCK->hide();
    ui->bianzuheBox->hide();
    ui->ppx->setText("0");
    ui->ppy->setText("0");
    ui->ppxs->setText("1.0");
    ui->ppys->setText("1.0");
    ui->syx->setText("0");
    ui->syy->setText("0");
    ui->syxs->setText("0.8");
    ui->syys->setText("0.8");
    dlg->hide();
    ui->xssjck->hide();
    ui->zzsetck->hide();
    ui->piliangCk->hide();
    ui->insertBox->hide();
    ui->lkhedian->clear();
    ui->xsAll->setChecked(true);
    SaveFenzuBtn=ui->xsAll;
    ui->toolButton_14->setChecked(false);
    ui->toolButton_22->setChecked(false);
    ui->tabWidget->removeTab(tiLunkuoxiugai);
    ui->tabWidget->setCurrentIndex(0);
    ui->tabWidget->setTabText(tiWenziliebiao,"全部文字");
    ui->info->clear();
    for(int i=0;i<fzyyList.length();i++){
        delete fzyyList[i];
    }
    fzyyList.clear();
    ui->listWidget->hide();
    qb->init();
    for(int i=0;i<namePte.length();i++){
        namePte[i]->clear();
        namePteY[i]->clear();
    }
}

MainWindow::~MainWindow()
{
    XIANSHIhanshuming;
    delete ui;
}

void MainWindow::dropEvent(QDropEvent *event)
{
    //接收拖放文件，并打开第一个。
    XIANSHIhanshuming;
    if(tishi->isVisible()){
        return;
    }
    QList<QUrl> urlList=event->mimeData()->urls();
    if(urlList.length()>0){
        QString fi=urlList[0].toLocalFile();
        JianceTTF(fi);
    }
}
///新建--------keyiBaocun=false--------------------------------------------------------------------|
///                ____________                                                                    |
/// 打开文件按钮-----| JianceTTF |-|---单字体-keyiBaocun=true----------------------------|            |
///                ----↑-------  |                                              ______↓_______     _|____
///                    |         |                                              | DakaiTTFfile |--| winit |
/// 拖放--dropEvent-----          |                                               -------↑-------  -------
///                              |---字体集: showFontNameSelect--keyiBaocun=false-------------|
///                                                 ↑
/// 显示本机字体--------------------------------------|
///
void MainWindow::dragEnterEvent(QDragEnterEvent *event){
    XIANSHIhanshuming;
    if (event->mimeData()->hasUrls()){
        event->acceptProposedAction();
    }
}

void MainWindow::kanlunkuo(bool fromDataChange,bool fromBujianDelete){
    XIANSHIhanshuming;
    //fromDataChange true，由于其他操作引起的数据改变，而调用本函数刷新图形
    //fromDataChange=false,qb双击查看，下一个上一个查看，等直接指定showidx查看。
    if(!fromBujianDelete){
        if(!fromDataChange){
            xiugaiShowindex=qb->currentSelidx();
            xiugaiGlyphIndex=qb->getGlyphIndex(xiugaiShowindex);
        }
    }
    int16 lsb=ttfFile->hmtx.getLsb(xiugaiGlyphIndex);
    uint16 aw=ttfFile->hmtx.getAw(xiugaiGlyphIndex);
    lunkuoFanwei lfw;
    bool youtu=ttfFile->glyf.getLunkuofanwei(xiugaiGlyphIndex,lfw);
    //
    //可以查看lsb不在y轴的字符，暂时不能单独修改lsb。只能修改lsb=xmin的字符
    //不允许修改无图的lsb
    uilsb->setEnabled((lfw.xMin-lsb==0)&&youtu);
    if(!fromDataChange){
        if(ui->tabWidget->count()==3){
            ht->init();
            ui->tabWidget->insertTab(tiLunkuoxiugai,ui->tab3,"xx");
        }

        QString wz=qb->getZifuFirst(xiugaiShowindex);
        if(wz=="&"){
            wz="&&";
        }
        QString bmstr;
        ui->tabWidget->setTabText(tiLunkuoxiugai,QString("%1：%2（%3）").arg(xiugaiGlyphIndex)
                                  .arg(wz)
                                  .arg(qb->getBianmaAll(xiugaiShowindex,bmstr)));
        //新字符首次填写数据
        uilsb->setValue(lsb);
        uiaw->setValue(aw);
        uitsx->setValue(lfw.yMax);
        ui->lsbY->setChecked(lfw.xMin-lsb==0);
        //
        uisbj->setValue(ttfFile->os2.usWinAscent);
        uixbj->setValue(-ttfFile->os2.usWinDescent);
        QString tj;
        PBjCanshuList cl=ttfFile->glyf.tongjiLunkuoDian(xiugaiGlyphIndex,tj);
        ui->fzgd->setVisible(cl);
        if(cl){
            ui->scrollAreaWidgetContents_3->setFixedHeight(cl->length()*Fzyy::getHeight());
            for(int i=0;i<cl->length();i++){
                if(i>=fzyyList.length()){
                    Fzyy* yy1=new Fzyy(ui->scrollAreaWidgetContents_3,i,
                                       &ttfFile->glyf,ttfFile->os2.usWinAscent,ttfFile->os2.usWinDescent);
                    fzyyList.append(yy1);
                    yy1->tb(cl->at(i),i);
                    connect(yy1,&Fzyy::bujianGaibian,this,&MainWindow::xuanzeBujian);
                }else{
                    fzyyList[i]->tb(cl->at(i),i);
                }
                if(i>0){
                    fzyyList[i-1]->hidebtn();
                }
            }
            for(int i=cl->length();i<fzyyList.length();i++){
                fzyyList[i]->ychm();
            }
        }
        ui->fzbj->setChecked(cl);
        int fzgdx=cl?240:100;
        ui->htGundong->setGeometry(fzgdx,0,ui->tab3->width()-fzgdx,ui->tab3->height());
        ui->lkhedian->setText(tj);
    }
    ui->tabWidget->setCurrentIndex(tiLunkuoxiugai);
    ht->kanlunkuo(lfw,lsb,aw,xiugaiGlyphIndex,
                  ui->tab3->height(),&ttfFile->glyf.fzxList);
    ht->repaint();
}
void MainWindow::xuanzeBujian(int bujianIdx, BujianGaibian bg, int xy, double scale){
    if(bg==bgHuanyige){
        qb->saveFzPara(bujianIdx,xiugaiGlyphIndex);
        ui->tabWidget->setCurrentIndex(tiWenziliebiao);
        ui->findTxt->selectAll();
        ui->findTxt->setFocus();
    }else if(bg==bgAppend){
        jiaYizu(bujianIdx);
    }else if(bg==bgDelete){
        shanYizu(bujianIdx);
    }else{//修改偏移bgX bgY 缩放bgScale 部件gid修改bgXuanhao
        if(bg==bgXuanhao){
            //组合字部件参数表更新
            fzyyList[bujianIdx]->updateBujianIdx(xy);
        }
        int newYmax=ttfFile->glyf.fzgbXY(xiugaiGlyphIndex,bg,bujianIdx,xy,scale);
        uilsb->setValue(ttfFile->hmtx.getLsb(xiugaiGlyphIndex));
        uiaw->setValue(ttfFile->hmtx.getAw(xiugaiGlyphIndex));
        uitsx->setValue(newYmax);
        kanlunkuo(true);
    }
}

void MainWindow::jiaYizu(int fzyyCurrentIndex){
    //fzyyCurrentIndex 增加按钮所在的组件的序号，0开始。
    if(fzyyCurrentIndex==fzyyList.length()-1){
        Fzyy* yy1=new Fzyy(ui->scrollAreaWidgetContents_3,fzyyCurrentIndex+1,
                           &ttfFile->glyf,ttfFile->os2.usWinAscent,ttfFile->os2.usWinDescent);
        fzyyList.append(yy1);
        connect(yy1,&Fzyy::bujianGaibian,this,&MainWindow::xuanzeBujian);
        yy1->show();
    }else{
        fzyyList[fzyyCurrentIndex+1]->tb(BujianCanshu(),fzyyCurrentIndex+1);
    }
    ui->scrollAreaWidgetContents_3->setFixedHeight((fzyyCurrentIndex+2)*Fzyy::getHeight());
    //
    ttfFile->glyf.jiayizu(xiugaiGlyphIndex);
}

void MainWindow::shanYizu(int fzyyCurrentIndex){
    bool sccg=ttfFile->glyf.shanyizu(xiugaiGlyphIndex,fzyyCurrentIndex);
    if(sccg){
        kanlunkuo(false,true);
    }else{
        tishi->showinfo("组合字至少留一组");
    }
}

void MainWindow::on_dakaiTtf_clicked()
{
    XIANSHIhanshuming;
    QString x;
#ifdef QT_DEBUG
    if(ui->findTxt->text()==""){
        x="E:\\文字\\Candara.ttf";
    }else if(ui->findTxt->text()=="1"){
        x="E:\\文字\\shiyanheiti.ttf";
    }else{
#endif
        x=QFileDialog::getOpenFileName(nullptr,
                                       "打开字体文件",
                                       "E:/文字",
                                       "所有字体文件 (*.ttf *.otf *.ttc *.otc);;"
                                       "truetype 字体文件 (*.ttf);;"
                                       "CFF 字体文件 (*.otf);;"
                                       "混合字体文件 (*.ttc *.otc);;"
                                       "所有文件 (*.*)"
                                       );

#ifdef QT_DEBUG
    }
#endif
    if(x==""){
        tishi->showinfo("没有打开字体文件");
        return;
    }
    JianceTTF(x);
}
void MainWindow::on_dakaiTtf_2_clicked()
{
    //显示本机字体
    XIANSHIhanshuming;
    FontPath fp(namelist);
    fp.getFullFontName();
    showFontNameSelect(false);
}
void MainWindow::showFontNameSelect(bool fromFile){
    // 显示搜到的所有字体。 fromFile=true:一个字体集中的字体。false:已安装的字体
    XIANSHIhanshuming;
    keyiBaocun=false;
    ui->listWidget->clear();
    CodePage dmy=cpQita;
    for(int i=0;i<namelist.length();i++){
        if(namelist[i].dmy!=dmy){
            dmy=namelist[i].dmy;
            ui->listWidget->addItem("------"+codePageStr[namelist[i].dmy]+"------");
        }
        ui->listWidget->addItem(QString("%1：%2").arg(i+1).arg(namelist[i].name));
    }
    int xgd=351;
    ui->listWidget->setFixedHeight(xgd);
    ui->delTTF->move(ui->listWidget->width()-ui->delTTF->width()-50,
                     0);
    setDialog(ui->listWidget,fromFile?"选择字体集中的字体":"本机字体");
}
void MainWindow::JianceTTF(const QString& ttfFileNameFullPath){
    XIANSHIhanshuming;
    //检测文件
    QFile file(ttfFileNameFullPath);
    if (!file.open(QIODevice::ReadOnly)){
        tishi->showinfo("字体文件不存在 "+ttfFileNameFullPath);
        return;
    }
    QDataStream dt(&file);
    Tag ttcbiaoji;
    dt>>ttcbiaoji.data;
    file.close();
    if(ttcbiaoji=="ttcf"){//ttc otc 标记相同
        FontPath fp(namelist);
        fp.getNameFromFile(ttfFileNameFullPath,true);
        showFontNameSelect(true);
    }else{//单字体
        keyiBaocun=true;
        DakaiTTFfile(ttfFileNameFullPath);
    }
}
void MainWindow::DakaiTTFfile(const QString& ttfFileNameFullPath,Offset32 ttfOffset){
    XIANSHIhanshuming;
    if(ttfFile){
        ui->tabWidget->setCurrentIndex(0);
        delete ttfFile;
        ttfFile=nullptr;
    }
    winit();
    //
    QFontMetrics fm(ui->wjm->font());
    QString xx=fm.elidedText("数据来源："+ttfFileNameFullPath,Qt::ElideMiddle,ui->wjm->width());
    ui->wjm->setText(xx);
    ui->wjm->setToolTip(ttfFileNameFullPath);

    QFile file(ttfFileNameFullPath);
    if (!file.open(QIODevice::ReadOnly)){
        tishi->showinfo("字体文件不存在 "+ttfFileNameFullPath);
        return;
    }
    QDataStream dt(&file);
    dt.device()->seek(ttfOffset);
    //按照ttf格式读取数据
    newTTF();
    TTFLeibie jg=ttfFile->readFrom(dt);
    file.close();
    if(jg==tlERR){
        tishi->showinfo("不是TTF/OTF 文件 "+ttfFileNameFullPath);
        return;
    }
    setHtQbUiFromTTF();
}
void MainWindow::setHtQbUiFromTTF(){
    XIANSHIhanshuming;
    ht->setZuobiao(ttfFile->os2.usWinAscent,ttfFile->os2.usWinDescent,
                   ttfFile->os2.sxHeight,ttfFile->os2.sCapHeight,
                   ttfFile->os2.fzZhongxian(),&ttfFile->glyf);
    zzhb->setGlyf(&ttfFile->glyf);
    lbhb->setGlyf(&ttfFile->glyf);
    uiaw->setMinimum(0);
    //
    ui->p1->setValue(ttfFile->os2.panose[0]);
    ui->p2->setValue(ttfFile->os2.panose[1]);
    ui->p3->setValue(ttfFile->os2.panose[2]);
    ui->p4->setValue(ttfFile->os2.panose[3]);
    ui->p5->setValue(ttfFile->os2.panose[4]);
    ui->p6->setValue(ttfFile->os2.panose[5]);
    ui->p7->setValue(ttfFile->os2.panose[6]);
    ui->p8->setValue(ttfFile->os2.panose[7]);
    ui->p9->setValue(ttfFile->os2.panose[8]);
    ui->p10->setValue(ttfFile->os2.panose[9]);
    //
    qb->setWenzi(&ttfFile->glyf,&ttfFile->hmtx, ttfFile->os2.usWinAscent,ttfFile->os2.usWinDescent);
}

void MainWindow::newTTF(){
    XIANSHIhanshuming;
    ttfFile=new TtfFile;
    QObject::connect(ttfFile, &TtfFile::appendTxt, this, &MainWindow::qitaxinxiAppend);
    QObject::connect(ttfFile, &TtfFile::appendTable, this, &MainWindow::tableAppend);
    QObject::connect(ttfFile, &TtfFile::jindu, tishi, &TishiChuangkou::showProgress);
    QObject::connect(ttfFile, &TtfFile::showFontinfo, this, &MainWindow::showFontinfo);
    QObject::connect(ttfFile, &TtfFile::setFontDate, this, &MainWindow::setFontDate);
    QObject::connect(&ttfFile->cmap, &CMAPTable::zifuReady, qb, &quanbu::zifuReady);
}

void MainWindow::qitaxinxiAppend(QString xx,bool isError){
    XIANSHIhanshuming;
    if(isError){
        tishi->showinfo(xx);
    }else{
        ui->qitaxinxi->append(xx);
        ui->qitaxinxi->repaint();
    }
}
void MainWindow::tableAppend(QString xx){
    XIANSHIhanshuming;
    ui->tbl->addItem(xx);
}
void MainWindow::showFontinfo(const QList<NameRecord> &str){
    //
    QString bb;
    for(uint16 i=0;i<str.length();i++){
        if(str[i].platformID==piWindows){
            if(str[i].nameID==nxMingcheng){
                if(str[i].languageID==ybZhong){
                    ui->ztmz->setText(str[i].recStr);
                }
            }else if(str[i].nameID==nxBanben){
                bb=str[i].recStr;
            }
            //
            if(str[i].languageID==ybZhong)
                namePte[str[i].nameID]->setPlainText(str[i].recStr);
            else
                namePteY[str[i].nameID]->setPlainText(str[i].recStr);
        }
    }
    //找到版本
    QRegularExpression reg("Version (\\d\\.\\d+)");
    auto match=reg.match(bb);
    double bbvalue=1.0;
    if(match.hasMatch()){
        bbvalue=match.captured(1).toDouble();
    }
    ui->banben->setValue(bbvalue);
}
void MainWindow::setFontDate(LONGDATETIME create1904, uint16 unitsPerEm){
    XIANSHIhanshuming;
    ui->riqi->setDateTime(QDateTime::fromSecsSinceEpoch(create1904-sec19040101to19700101));
    ui->emlog2->setValue(log2(unitsPerEm));
    ui->emstr->setText("单字分辨率："+QString::number(unitsPerEm));
}

void MainWindow::on_save_clicked()
{
    //存储
    if(!ttfFile){
        return;
    }
    if(ui->ztmz->text().trimmed().length()<2){
        tishi->showinfo("没有写字体名字");
        return;
    }
    //
    ttfFile->os2.panose[0]=ui->p1->value();
    ttfFile->os2.panose[1]=ui->p2->value();
    ttfFile->os2.panose[2]=ui->p3->value();
    ttfFile->os2.panose[3]=ui->p4->value();
    ttfFile->os2.panose[4]=ui->p5->value();
    ttfFile->os2.panose[5]=ui->p6->value();
    ttfFile->os2.panose[6]=ui->p7->value();
    ttfFile->os2.panose[7]=ui->p8->value();
    ttfFile->os2.panose[8]=ui->p9->value();
    ttfFile->os2.panose[9]=ui->p10->value();
    QStringList sm,smY;
    for(int i=0;i<namePte.length();i++){
        QString x=namePte[i]->toPlainText().trimmed();
        QString y=namePteY[i]->toPlainText().trimmed();
        if(y!="" && x==""){
            namePte[i]->setPlainText(y);
        }
        sm.append(x);\
        smY.append(y);
    }
    ttfFile->jisuan(ui->ztmz->text().trimmed(),ui->banben->text(),
                    sm,smY,
                    ui->riqi->dateTime().toSecsSinceEpoch(),
                    pow(2,ui->emlog2->value()),qb->chazhao(0x5f));
    //
    QString x;
#ifdef QT_DEBUG
    x="E:\\文字\\shiyanheiti.ttf";
#else
    if(sender()->objectName()=="save" && keyiBaocun){
        //直接存储：存储在原路径。已安装字体除外。
        x=ui->wjm->toolTip();
    }else{
        //另存为
        x=QFileDialog::getSaveFileName(nullptr,
                                       "保存字体文件","E:/文字","*.ttf");
        if(x==""){
            tishi->showinfo("没有存储！！");
            return;
        }
    }
#endif
    QFile file(x);
    if (!file.open(QIODevice::ReadWrite))
    {
        tishi->showinfo("没有存储！！");
        return;
    }
    file.resize(0);//删除重写

    QDataStream dt(&file);
    //按照ttf格式读取数据
    ttfFile->saveTo(dt);
    file.close();
    //
    QFontMetrics fm(ui->wjm->font());
    QString xx=fm.elidedText("存入文件："+x,Qt::ElideMiddle,ui->wjm->width());
    ui->wjm->setText(xx);
    ui->wjm->setToolTip(x);

    //保存完毕以后，各种情况都可以使用直接保存功能
    keyiBaocun=true;

    tishi->showinfo("保存完毕");
    qb->showAll(true);
    ui->tabWidget->setTabText(tiWenziliebiao,"全部文字");
}

void MainWindow::on_xinjian_clicked()
{
    XIANSHIhanshuming;
    if(ttfFile){
        ui->tabWidget->setCurrentIndex(0);
        delete ttfFile;
        ttfFile=nullptr;
    }
    keyiBaocun=false;
    winit();
    newTTF();
    xinjianMuban xm=xmKongge;
    QAction *p =dynamic_cast<QAction *>(sender());
    if(p){
        if(p->text()=="3500"){xm=xm3500;}
        else if(p->text()=="6500"){xm=xm6500;}
    }
    ttfFile->xinjian1(xm);
    //弹窗提示
    ui->ztmz->setText("新字体");
    setDialog(ui->jiben,"设置新字体",true, this,&MainWindow::fanghuiqu);
}

void MainWindow::fanghuiqu(int doneResult){
    Q_UNUSED(doneResult);
    namePte[nxSheji]->setPlainText("qmgj");
    ui->jiben->setParent(ui->tab);
    ui->jiben->move(10,7);
    ui->jiben->show();
    //
    int v=ui->emlog2->value();
    uint16 emsize=pow(2,v);
    ttfFile->xinjian2(emsize);
    setHtQbUiFromTTF();
    //
    ui->tabWidget->setCurrentIndex(tiWenziliebiao);
    qb->setSel(1);
}
void MainWindow::on_tbl_currentTextChanged(const QString &currentText)
{
    XIANSHIhanshuming;
    ui->qitaxinxi->clear();
    ttfFile->showTable(currentText);
}
void MainWindow::resizeEvent(QResizeEvent* se){
    XIANSHIhanshuming;
    ui->tabWidget->resize(se->size().width()-20,se->size().height()-10-ui->tabWidget->y());
    ui->qitaxinxi->resize(ui->tabWidget->width()-ui->qitaxinxi->x()-20,ui->tab->height()-20);
    ui->tbl->resize(ui->tbl->width(),ui->qitaxinxi->height());
    ui->gundong->resize(ui->tab_2->width()-ui->gundong->x(),ui->tab_2->height()-ui->info->height());
    ui->htGundong->resize(ui->tab3->width()-ui->htGundong->x(),ui->tab3->height());
    ui->nameGundong->resize(ui->tab_Fujia->width()-ui->nameGundong->x(),ui->tab_Fujia->height());
    ui->fzgd->resize(ui->fzgd->width(),ui->tab3->height());
    ui->info->setGeometry(ui->gundong->x(),ui->tab_2->height()-ui->info->height(), ui->tab_2->width(),ui->info->height());
}

void MainWindow::showEvent(QShowEvent *event){
    XIANSHIhanshuming;
    Q_UNUSED(event);
    ui->tabWidget->resize(this->width()-20,this->height()-10-ui->tabWidget->y());
    ui->qitaxinxi->resize(ui->tabWidget->width()-ui->qitaxinxi->x()-20,ui->tab->height()-20);
    ui->tbl->resize(ui->tbl->width(),ui->qitaxinxi->height());
}

void MainWindow::on_tabWidget_currentChanged(int index)
{
    XIANSHIhanshuming;
    if(index==tiLunkuoxiugai){
        ui->htGundong->resize(ui->tab3->width()-ui->htGundong->x(),ui->tab3->height());
        ui->fzgd->resize(ui->fzgd->width(),ui->tab3->height());
    }else if(index==tiWenziliebiao){
        ui->info->setGeometry(ui->gundong->x(),ui->tab_2->height()-ui->info->height(), ui->tab_2->width(),ui->info->height());
        ui->gundong->resize(ui->tab_2->width()-ui->gundong->x(),ui->tab_2->height()-ui->info->height());
    }else if(index==tiJibenxinxi){
        ui->qitaxinxi->resize(ui->tabWidget->width()-ui->qitaxinxi->x()-20,ui->tab->height()-20);
        ui->tbl->resize(ui->tbl->width(),ui->qitaxinxi->height());
    }else if(index==tiFujiaziliao){
        ui->nameGundong->resize(ui->tab_Fujia->width()-ui->nameGundong->x(),ui->tab_Fujia->height());
    }
    ui->sfda->setVisible(index==tiLunkuoxiugai);
    ui->sfxiao->setVisible(index==tiLunkuoxiugai);
    ui->ttfxy->clear();
    ui->shangyige->setVisible(index == tiWenziliebiao||index==tiLunkuoxiugai);
    ui->xiayige->setVisible(ui->shangyige->isVisible());
}

void MainWindow::on_find_clicked()
{
    if(ui->tabWidget->currentIndex()!=tiWenziliebiao){
        ui->tabWidget->setCurrentIndex(tiWenziliebiao);
    }
    //查找
    QString x=ui->findTxt->text();
    if(x.length()>=1){
        bool ok;
        uint32 bm=0;
        if(x.length()>=4){//查编码 4位16进制
            bm=x.toUInt(&ok,16);
            if(ok==false){//查编码失败则查首字
                bm=x.toUcs4()[0];
            }
        }else{//查首字
            bm=x.toUcs4()[0];
        }

        if(!qb->btnChazhao(bm)){
            ui->danziXinxi->setPlainText("没有找到");
        }else{
            ui->xsAll->setChecked(true);
            ui->gundong->setFocus();
        }
    }
}

void MainWindow::showMousePos(int ttfX, int ttfY){
    ui->ttfxy->setText(QString("x:%1,y:%2").arg(ttfX).arg(ttfY));
}

void MainWindow::showDanzi(QString& bianma,QString& ms){
    XIANSHIhanshuming;
    //查字，鼠标点击
    ui->danziXinxi->clear();
    ui->danziXinxi->setPlainText(QString("编码：\r\n%1").arg(bianma));
    ui->info->setText(ms);
}

void MainWindow::on_insert_3_clicked()
{
    //删除文字
    if(qb->bunengXiugai()){
        tishi->showinfo("正在修改其他字符，不能删除");
        return;
    }
    shanchuXuanze sx=qb->deleteWenziJiance(ttfFile->toDelete);
    if(sx==sxOK){
        tishi->showinfoContinue<DATAFUNC>("直接删除","先做备份","删除无法撤销，要先做备份",
                                          this,&MainWindow::delteWenzi);
    }else{
        tishi->showinfo(shanchuXuanzeStr[sx]);
    }
}

void MainWindow::delteWenzi(){//确认以后删除文字
    XIANSHIhanshuming;
    ttfFile->deleteWenzi();
    int saveSel=qb->currentSelidx();
    SaveFenzuBtn->click();
    qb->setSel(saveSel);
    qb->deleteWenziRepaint();
    ui->tabWidget->removeTab(tiLunkuoxiugai);
}

void MainWindow::on_insert_5_clicked()
{
    //合并文字
    xiugaiGongxiang jg=qb->gongxiangGlyph(ui->danziXinxi->toPlainText());
    if(jg!=xgOk){
        tishi->showinfo(xiugaiGongxiangStr[jg]);
    }
}

void MainWindow::xiayige(){
    XIANSHIhanshuming;
    if(!qb->isTTFok){
        return;
    }

    if(ui->tabWidget->currentIndex()==tiLunkuoxiugai){
        qb->clearFzPara();//左右切换终止组合字的修改
        sender()->objectName()=="xiayige"?qb->next():qb->pre();
        kanlunkuo(false);
    }else if(ui->tabWidget->currentIndex()==tiWenziliebiao){
        sender()->objectName()=="xiayige"?qb->next():qb->pre();
        qb->repaint();
    }
}

void MainWindow::awValueChanged(int arg1)
{
    ttfFile->moveRsb(xiugaiGlyphIndex,arg1);
    this->kanlunkuo(true);
}

void MainWindow::lsbValueChanged(int arg1)
{
    XIANSHIhanshuming;
    //左右移动
    int delta=ttfFile->zuoyouyidong(xiugaiGlyphIndex,arg1);
    uiaw->setValue(ttfFile->hmtx.getAw(xiugaiGlyphIndex));
    fzyyUpdateX(delta);
    this->kanlunkuo(true);
}

void MainWindow::fzyyUpdateX(int delta){
    XIANSHIhanshuming;
    if(delta==0 || !ui->fzgd->isVisible())return;
    for(int i=0;i<fzyyList.length();i++){
        if(fzyyList[i]->updateX(delta)){
            break;
        }
    }
}
void MainWindow::fzyyUpdateY(int delta){
    XIANSHIhanshuming;
    if(delta==0 || !ui->fzgd->isVisible())return;
    for(int i=0;i<fzyyList.length();i++){
        if(fzyyList[i]->updateY(delta)){
            break;
        }
    }
}

void MainWindow::tsxValueChanged(int arg1)
{//上下移动
    XIANSHIhanshuming;
    int delta=ttfFile->glyf.tsx(xiugaiGlyphIndex,arg1);
    fzyyUpdateY(delta);
    this->kanlunkuo(true);
}

void MainWindow::tsxYushe(){
    XIANSHIhanshuming;
    if(!ht->isTTFok){
        return;
    }
    int delta=0;
    if(sender()->objectName()=="dqzhongxian"){
        delta=ttfFile->glyf.tsx(xiugaiGlyphIndex,ht->zx,cdJuzhong);
    }else if(sender()->objectName()=="dqjixian"){
        delta=ttfFile->glyf.tsx(xiugaiGlyphIndex,0,cdYmin);
    }else if(sender()->objectName()=="dqdaxiexian"){
        delta=ttfFile->daxieduiqi(xiugaiGlyphIndex);
    }
    this->kanlunkuo(true);
    if(delta!=0){
        uitsx->setValue(uitsx->value()+delta);
        fzyyUpdateY(delta);
    }
}

void MainWindow::on_toolButton_19_clicked()
{
    XIANSHIhanshuming;
    //全部自动调整一次
    if(!ht->isTTFok){
        return;
    }
    int idx=qb->currentSelidx();
    ttfFile->zidongduiqi();
    this->kanlunkuo(idx);
}

void MainWindow::jizhuWeizhi(int v){
    saveLiebiaoPos=v;
}

void MainWindow::xsBd()
{
    //标点符号
    if(!qb->isTTFok){
        return;
    }
    SaveFenzuBtn=dynamic_cast<QToolButton*>(sender());
    if(sender()->objectName()=="xsMeixie"){
        //显示没有笔迹的字
        ttfFile->glyf.getKongzifu(qb->xianshiZifu);
        ui->tabWidget->setTabText(tiWenziliebiao,QString("无笔迹 %1 个").arg(qb->xianshiZifu.length()));
        qb->showAll(false);
    }else if(sender()->objectName()=="xsZuhe"){
        ttfFile->glyf.getZuhezi(qb->xianshiZifu);
        ui->tabWidget->setTabText(tiWenziliebiao,QString("组合字 %1 个").arg(qb->xianshiZifu.length()));
        qb->showAll(false);
        ui->gundong->verticalScrollBar()->setValue(saveLiebiaoPos);
    }else if(sender()->objectName()=="xsPutong"){
        ttfFile->glyf.getPutongzi(qb->xianshiZifu);
        ui->tabWidget->setTabText(tiWenziliebiao,QString("普通字 %1 个").arg(qb->xianshiZifu.length()));
        qb->showAll(false);
    }else if(sender()->objectName()=="xsZuhetu"){
        ttfFile->getWubianma(qb->xianshiZifu,true);
        ui->tabWidget->setTabText(tiWenziliebiao,QString("无编码组合字 %1 个").arg(qb->xianshiZifu.length()));
        qb->showAll(false);
    }else if(sender()->objectName()=="xsPutongtu"){
        ttfFile->getWubianma(qb->xianshiZifu,false);
        ui->tabWidget->setTabText(tiWenziliebiao,QString("无编码普通字 %1 个").arg(qb->xianshiZifu.length()));
        qb->showAll(false);
    }else if(sender()->objectName()=="xsAll"){
        qb->showAll(true);
        ui->tabWidget->setTabText(tiWenziliebiao,QString("全部文字 %1 个").arg(qb->xianshiZifu.length()));
    }else{
        QString bt=ttfFile->cmap.showAll(sender()->objectName(),qb->xianshiZifu);
        ui->tabWidget->setTabText(tiWenziliebiao,QString("%1 %2 个").arg(bt).arg(qb->xianshiZifu.length()));
        qb->showAll(false);
    }
    ui->danziXinxi->clear();
}

void MainWindow::on_sfda_clicked()
{
    //放大
    if(!ht->isTTFok){
        return;
    }
    ht->suofang(true);
}

void MainWindow::on_sfxiao_clicked()
{
    //缩小
    if(!ht->isTTFok){
        return;
    }
    ht->suofang(false);
}

void MainWindow::on_lsbY_clicked(bool checked)
{
    XIANSHIhanshuming;
    if(!ht->isTTFok){
        return;
    }
    if(checked){
        int newLsb=ttfFile->glyf.lsbDengyuXMin(xiugaiGlyphIndex);
        if(uilsb->value()!=newLsb){
            uilsb->setValue(newLsb);
        }
        this->kanlunkuo(true);
    }else{
        uilsb->setEnabled(false);
    }
}


void MainWindow::on_toolButton_18_clicked()
{
    XIANSHIhanshuming;
    //轮廓最多的字
    if(!ht->isTTFok){
        return;
    }
    int maxlks=0;
    int showidx=-1;
    for(int i=0;i<qb->xianshiZifu.length();i++){
        int32 glIdx=qb->getGlyphIndex(i);
        uint16 ds=0,lks=0;
        ttfFile->glyf.getPointCount(glIdx,ds,lks);
        if(lks>maxlks){
            maxlks=lks;
            showidx=i;
        }
    }
    if(showidx!=-1){
        qb->setSel(showidx);
        ui->danziXinxi->clear();
        kanlunkuo(false);
    }else{
        tishi->showinfo("先显示全部文字。");
    }
}

void MainWindow::on_toolButton_15_clicked()
{
    XIANSHIhanshuming;
    //点最多的字
    if(!ht->isTTFok){
        return;
    }
    int dians=0;
    int showidx=-1;
    for(int i=0;i<qb->xianshiZifu.length();i++){
        int32 glIdx=qb->getGlyphIndex(i);
        uint16 ds=0,lks=0;
        ttfFile->glyf.getPointCount(glIdx,ds,lks);
        if(ds>dians){
            dians=ds;
            showidx=i;
        }
    }
    if(showidx!=-1){
        qb->setSel(showidx);
        ui->danziXinxi->clear();
        kanlunkuo(false);
    }else{
        tishi->showinfo("先显示全部文字。");
    }
}

void MainWindow::on_toolButton_20_clicked()
{
    XIANSHIhanshuming;
    //显示点的数据
    if(!ht->isTTFok){
        return;
    }
    PBiji xds=ttfFile->glyf.getBzrList(xiugaiGlyphIndex);
    if(xds){
        QString x;
        for(int i=0;i<xds->length();i++){
            x+=QString("%1:%2,%3 %4%5%6\r\n").arg(i,3).arg((*xds)[i].x).arg((*xds)[i].y)
                    .arg((*xds)[i].isControl?",曲线控制":"")
                    .arg((*xds)[i].lkEnd?",轮廓结束":"")
                    .arg((*xds)[i].wojia?",插点":"");
        }
        ui->xssj->setText(x);
        QClipboard* jtb=QApplication::clipboard();
        jtb->setText(x);
        setDialog(ui->xssjck,"各点数据",false);
    }else{
        tishi->showinfo("此文字没有轮廓数据。");
    }
}
void MainWindow::on_toolButton_21_clicked()
{
    XIANSHIhanshuming;
    //左边界不在y的字
    if(!ht->isTTFok){
        return;
    }
    int showidx=-1;
    for(int i=0;i<qb->xianshiZifu.length();i++){
        int32 glIdx=qb->getGlyphIndex(i);
        if(ttfFile->glyf.gl[glIdx].pg){
            int xmin=ttfFile->glyf.gl[glIdx].pg->xMin;
            int lsb=ttfFile->hmtx.getLsb(glIdx);
            if(xmin!=lsb){
                showidx=i;
                break;
            }
        }
    }
    if(showidx!=-1){
        qb->setSel(showidx);
        ui->danziXinxi->clear();
        kanlunkuo(false);
    }else{
        tishi->showinfo("所有字符lsb都在x=0。");
    }
}

void MainWindow::on_insert_2_clicked()
{
    //删除笔迹
    XIANSHIhanshuming;
    if(qb->bunengXiugai()){
        tishi->showinfo("不能修改");
        return;
    }
    //擦除
    if(!qb->isTTFok){
        return;
    }
    s32vector glyphIdxSel;
    qb->getXuanze(glyphIdxSel);
    ttfFile->deleteTuxing(glyphIdxSel);
    qb->repaint();
    ui->tabWidget->removeTab(tiLunkuoxiugai);
}
void MainWindow::on_insert_4_clicked()
{
    //删除编码
    if(qb->bunengXiugai()){
        tishi->showinfo("不能修改");
        return;
    }
    //擦除
    if(!qb->isTTFok){
        return;
    }
    s32vector glyphIdxSel;
    qb->getXuanze(glyphIdxSel);
    ttfFile->shanchuBianma(glyphIdxSel);
    ui->danziXinxi->clear();
    ui->info->clear();
    qb->repaint();
    ui->tabWidget->removeTab(tiLunkuoxiugai);
}

void MainWindow::on_p10_valueChanged(int value)
{
    const QString xx[]={
        "任意","不适合","恒定/小","恒定/标准","恒定/大","回避/小","回避/标准","回避/大"
    };
    ui->ps10->setText(QString::number(value)+"X高度:"+xx[value]);
}

void MainWindow::on_p1_valueChanged(int value)
{
    const QString xx[]={
        "任意","不适合","拉丁文字","拉丁手写","拉丁装饰","拉丁符号"
    };
    ui->ps1->setText(QString::number(value)+"字体分类:"+xx[value]);
}
void MainWindow::on_p2_valueChanged(int value)
{
    const QString xx[]={
        "任意","不适用","凹","钝凹","方凹","钝方凹","方","细","椭圆形","夸张","三角形","正常","钝","垂直","喇叭","圆形"
    };
    ui->ps2->setText(QString::number(value)+"衬线样式:"+xx[value]);
}
void MainWindow::on_p3_valueChanged(int value)
{
    const QString xx[]={
        "任何","不适用","非常轻","轻","薄","书本","中等","半米","粗体","重","黑色","超黑色"
    };
    ui->ps3->setText(QString::number(value)+"重量:"+xx[value]);
}
void MainWindow::on_p4_valueChanged(int value)
{
    const QString xx[]={
        "任意","不适用","旧式","现代","均匀宽度","延伸","紧缩","非常延伸","极紧缩","等宽字体"
    };
    ui->ps4->setText(QString::number(value)+"比例:"+xx[value]);
}void MainWindow::on_p5_valueChanged(int value)
{
    const QString xx[]={
        "任何","不适合","无","非常低","低","中低","中","中高","高","非常高"
    };
    ui->ps5->setText(QString::number(value)+"对比:"+xx[value]);
}void MainWindow::on_p6_valueChanged(int value)
{
    const QString xx[]={
        "任意","不匹配","不变化","渐变/对角","渐变/过渡","渐变/垂直","渐变/水平","快速/垂直","快速/水平","瞬间/垂直","瞬间/水平"
    };
    ui->ps6->setText(QString::number(value)+"笔划变化:"+xx[value]);
}
void MainWindow::on_p7_valueChanged(int value)
{
    const QString xx[]={
        "任意","不适合",
        "直臂/水平","直臂/楔形","直臂/垂直","直臂/单衬线","直臂/双衬线",
        "非直/水平","非直/楔形","非直/垂直","非直/单衬线","非直/双衬线"
    };
    ui->ps7->setText(QString::number(value)+"是否直臂:"+xx[value]);
}
void MainWindow::on_p8_valueChanged(int value)
{
    const QString xx[]={
        "任意","不适合","正常/接触","正常/加权","正常/盒装","正常/扁平","正常/圆形","正常/偏心","正常/方形",
        "斜/接触","斜/加权","斜/盒装","斜/扁平","斜/圆形","斜/偏心","斜/方形"
    };
    ui->ps8->setText(QString::number(value)+"字形:"+xx[value]);
}
void MainWindow::on_p9_valueChanged(int value)
{
    const QString xx[]={
        "任何","不适合","标准/修剪","标准/尖头","标准/衬线",
        "高/修剪","高/尖头","高/衬线",
        "恒定/修剪","恒定/尖头","恒定/衬线",
        "低/修剪","低/尖头","低/衬线"
    };
    ui->ps9->setText(QString::number(value)+"中线:"+xx[value]);
}
void MainWindow::on_toolButton_9_clicked()
{
    XIANSHIhanshuming;
    //增加上辅助线,把当前字符的ymax添加到辅助线列表
    ttfFile->glyf.addFuzhuxian(cdYmax,qb->getGlyphIndex(qb->currentSelidx()));
    ht->updateFuzhuxian();
}

void MainWindow::on_toolButton_23_clicked()
{
    XIANSHIhanshuming;
    //增加下辅助线
    ttfFile->glyf.addFuzhuxian(cdYmin,qb->getGlyphIndex(qb->currentSelidx()));
    ht->updateFuzhuxian();
}

void MainWindow::on_toolButton_24_clicked()
{
    XIANSHIhanshuming;
    //清除辅助线
    ttfFile->glyf.clearFuzhuxian();
    ht->updateFuzhuxian();
}

void MainWindow::duiqifuzhu(int fzidx){
    XIANSHIhanshuming;
    //处理按键对齐辅助线
    if(!ht->isTTFok){
        return;
    }
    int idx=qb->currentSelidx();
    int delta=ttfFile->glyf.tsxFromFzx(qb->getGlyphIndex(idx),fzidx);
    if(delta!=0){
        this->kanlunkuo(true);
        uitsx->setValue(uitsx->value()+delta);
        fzyyUpdateY(delta);
    }
}


void MainWindow::on_emlog2_valueChanged(int value)
{
    uint16 emsize=pow(2,value);
    ui->emstr->setText(QString("单字分辨率:%1").arg(emsize));
    if(!qb->isTTFok){
        return;
    }
    ui->tabWidget->removeTab(tiLunkuoxiugai);
    ttfFile->os2.updateShangxiaBianjie(emsize);
    ht->setZuobiao(ttfFile->os2.usWinAscent,ttfFile->os2.usWinDescent,
                   ttfFile->os2.sxHeight,ttfFile->os2.sCapHeight,
                   ttfFile->os2.fzZhongxian());
    qb->setWenzi(&ttfFile->glyf,&ttfFile->hmtx, ttfFile->os2.usWinAscent,ttfFile->os2.usWinDescent);
    fzyyList.clear();
}

void MainWindow::sbjValueChanged(int arg1)
{
    XIANSHIhanshuming;
    if(!ht->isTTFok){
        return;
    }
    snd<<"sdf";
    ttfFile->os2.usWinAscent=arg1;
    ht->setZuobiao(ttfFile->os2.usWinAscent,ttfFile->os2.usWinDescent,
                   ttfFile->os2.sxHeight,ttfFile->os2.sCapHeight,
                   ttfFile->os2.fzZhongxian());
    qb->setWenzi(&ttfFile->glyf,&ttfFile->hmtx, ttfFile->os2.usWinAscent,ttfFile->os2.usWinDescent);
    this->kanlunkuo(true);
}

void MainWindow::xbjValueChanged(int arg1)
{
    XIANSHIhanshuming;
    if(!ht->isTTFok){
        return;
    }
    snd<<"sdf";
    ttfFile->os2.usWinDescent=-arg1;
    ht->setZuobiao(ttfFile->os2.usWinAscent,ttfFile->os2.usWinDescent,
                   ttfFile->os2.sxHeight,ttfFile->os2.sCapHeight,
                   ttfFile->os2.fzZhongxian());
    qb->setWenzi(&ttfFile->glyf,&ttfFile->hmtx, ttfFile->os2.usWinAscent,ttfFile->os2.usWinDescent);
    ht->repaint();
}

void MainWindow::on_listWidget_itemDoubleClicked(QListWidgetItem *item)
{
    XIANSHIhanshuming;
    dlg->hide();
    int idx=item->text().indexOf("：");
    if(idx==-1){
        return;
    }
    int fpIndex=item->text().left(idx).toInt()-1;
    this->DakaiTTFfile(namelist[fpIndex].path,namelist[fpIndex].tableOffset);
}

void MainWindow::on_zuobiao_clicked(bool checked)
{
    if(!ht->isTTFok){
        return;
    }
    ht->huaZuobiao=checked;
    ht->repaint();
}

//竖排版
class sp{
public:
    sp(uint16 vheng,uint16 vshu):heng(vheng),shu(vshu){}
    uint16 heng;//!<横排标点的glyphIndex
    uint16 shu;//!<竖排标点的glyphIndex
};
typedef QList<sp> spList;
void MainWindow::on_shupai_clicked()
{
    if(!qb->isTTFok){
        return;
    }
    //检查竖排编码，若没有则补齐
    ///横排版标点
    u32vector heng={0X2026,0X2014,0X201C,0X201D,0X2018,0X2019,0XFF01,
                    0XFF08,0XFF09,0XFF0C,0XFF0E,0XFF1A,0XFF1B,0XFF1F,
                    0XFF3B,0XFF3D,0XFF3F,0XFF5B,0XFF5D,0X3001,0X3002,
                    0X3008,0X3009,0X300A,0X300B,0X300C,0X300D,0X300E,
                    0X300F,0X3010,0X3011,0X3014,0X3015,0X3016,0X3017
                   };
    ///竖排版标点
    u32vector shu={0XFE19,0XFE31,0XFE43,0XFE44,0XFE41,0XFE42,0XFE15,
                   0XFE35,0XFE36,0XFE10,0XFE12,0XFE13,0XFE14,0XFE16,
                   0XFE47,0XFE48,0XFE33,0XFE37,0XFE38,0XFE11,0XFE12,
                   0XFE3F,0XFE40,0XFE3D,0XFE3E,0XFE41,0XFE42,0XFE43,
                   0XFE44,0XFE3B,0XFE3C,0XFE39,0XFE3A,0XFE17,0XFE18};
    //QString iptHeng("… — “”‘ ’ ！（），．：；？［］＿｛｝ 、。〈〉《 》「」『』【】〔〕〖〗");
    //QString iptShu ("︙︱﹃﹄﹁﹂︕︵︶︐︒︓︔︖﹇﹈  ︳︷︸︑︒ ︿﹀︽︾ ﹁﹂﹃﹄︻︼︹︺︗︘");
    if(heng.length()!=shu.length()){//确认上面两个数组长度相同。
        tishi->showinfo("数据错误");
        return;
    }
    ///横、竖排版的全部标点
    QString ipt;
    for(int i=0;i<heng.length();i++){
        ipt+=QString::fromUcs4(&heng[i],1);
        ipt+=QString::fromUcs4(&shu[i],1);
    }
    //加入字库，更新显示
    int bccnt=ttfFile->appendBianma(ipt);
    if(bccnt>0){
        qb->showAll(true);
        ui->tabWidget->setTabText(tiWenziliebiao,"全部文字");
        ui->danziXinxi->clear();
        ui->tabWidget->removeTab(tiLunkuoxiugai);
    }
    //查到横竖排标点各自对应的glyphIndex
    spList spb;
    for(int i=0;i<heng.length();i++){
        spb.append(sp(qb->chazhao(heng[i]),qb->chazhao(shu[i])));
    }
    //按照glyphIndex排序
    sort(spb.begin(),spb.end(),[](const sp &a,const sp& b){
        return a.heng<b.heng;
    });
    //分别存储
    u16vector hengIdx,shuIdx;//!<横、竖排版标点的已排序的glyphIndex
    for(int i=0;i<heng.length();i++){
        hengIdx.append(spb[i].heng);
        shuIdx.append(spb[i].shu);
    }
    //重建vert表---脚本 语言 功能（vert）查询
    //检查脚本
    QStringList jbLst;
    ttfFile->gsub.getJiaoben(jbLst);
    int haniIndex=jbLst.indexOf("hani");
    if(haniIndex==-1){
        //脚本为空，或没有hani
        ttfFile->gsub.addLoopupVert("hani",hengIdx,shuIdx);
    }else{
        //已有脚本hani+默认语言---------------不能修改别人的字体---------假设默认语言一定存在。
        if(ttfFile->name.bukexiugai()){
            tishi->showinfo("不可修改别的字体");
            return;
        }
        ttfFile->gsub.tihuanVert(haniIndex,hengIdx,shuIdx);
    }
}

/// 打开一个字，选择删除一部分笔迹
void MainWindow::on_shanchuBiji_clicked()
{ 
    if(!ttfFile->glyf.isSimple(xiugaiGlyphIndex)){
        /// 组合字图形删除：
        /// 字体列表中：全部删除
        /// 轮廓编辑中：从组合字部件参数中点击-按钮
        /// 其他：字体列表中，复制图形--删除全部图形（改为简单字）--粘贴，则可以按照简单字选择删除。
        ht->repaint();
        return;
    }
    if(ht->lksel!=-1){
        //---点击删除
        //点在轮廓内部---删除点击的轮廓
        ttfFile->glyf.shanchuLunkuo(xiugaiGlyphIndex,ht->lksel);
    }else{
        //---框选删除
        if(ht->xuanzeBianjie.xMax==ht->xuanzeBianjie.xMin||
                ht->xuanzeBianjie.yMax==ht->xuanzeBianjie.yMin){
            ht->repaint();
            return;
        }
        if(ht->xianshiDian){//显示点---框选删除点
            ttfFile->glyf.shanchuDian(xiugaiGlyphIndex,ht->xuanzeBianjie);
        }else{
            //不显示点---框线删除完整轮廓。全部包括在框里面的轮廓被删除
            ttfFile->glyf.shanchuLunkuo(xiugaiGlyphIndex,ht->xuanzeBianjie);
        }
    }
    kanlunkuo(false);
}

void MainWindow::on_ycCtrlF_clicked()
{
    ui->findTxt->selectAll();
    ui->findTxt->setFocus();
}


void MainWindow::on_xiayige_2_clicked()
{
    ui->tabWidget->setCurrentIndex(tiWenziliebiao);
    ui->xsZuhe->clicked();
}


void MainWindow::on_fzbj_clicked(bool checked)
{
    //checked=true 点击完成后checked=true 按钮是按下状态
    //组合字简单字切换
    if(checked){//删除笔迹，改成组合字。
        //展开面板
        ui->fzgd->show();
        ui->htGundong->setGeometry(240,0,ui->tab3->width()-240,ui->tab3->height());
        //删除笔迹改成组合字
        ttfFile->glyf.gaiweiZuhezi(xiugaiGlyphIndex);
        //显示出来
        kanlunkuo(false);
    }else{//组合字改成简单字

    }
}

void MainWindow::updateMaodianUI(bool xianshidian)
{
    ui->toolButton_14->setChecked(xianshidian);
}

void MainWindow::on_insert_8_clicked()
{
    //查引用：查询无码图形被组合字引用的情况，显示组合字
    QString yy;
    int32 wmgid=qb->getGlyphIndex(qb->currentSelidx());
    ttfFile->chaYinyong(wmgid,yy);
    ui->info->setText(yy);
}
