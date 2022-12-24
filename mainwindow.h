#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <ttffile.h>
#include <QListWidgetItem>
#include <QResizeEvent>
#include <huatu.h>
#include <quanbu.h>
#include "tishichuangkou.h"
#include "fzyy.h"
#include "../qtJiemianKuozhan/spineditex.h"
#include "fontpath.h"
#include <QGraphicsScene>
#include <QProcess>
#include "image2bzr2.h"
#include "kanquantu.h"
#include "../qtJiemianKuozhan/PlainTextEx.h"

#ifdef XIANSHIhanshuming
#undef XIANSHIhanshuming
#define XIANSHIhanshuming
#endif

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow;
typedef void (MainWindow::*DATAFUNC)();
typedef void (MainWindow::*voidInt)(int);
typedef double (MainWindow::*toPingmu)(int16);

/// tabWidget 页面索引。
enum tabIndex{
    tiJibenxinxi,  //!<基本信息 0
    tiFujiaziliao, //!<附加资料 1
    tiWenziliebiao,//!<文字列表 2
    tiLunkuoxiugai,//!<轮廓修改 3
};

typedef QList<QPlainTextEdit *> PteList;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    void newTTF();
    virtual void resizeEvent(QResizeEvent *se) override;
    void showEvent(QShowEvent *event)override;
    void dropEvent(QDropEvent *event)override;
    void dragEnterEvent(QDragEnterEvent *event)override;
    void fzyyUpdateY(int delta);
    void fzyyUpdateX(int delta);
    void tsxYushe();
    void showVersion();
    /// @brief 显示搜到的所有字体。
    /// @param fromFile true:一个字体集中的字体。false:已安装的字体
    void showFontNameSelect(bool fromFile);
    void JianceTTF(const QString &ttfFileNameFullPath);
    void updateZaoziHuabu(int nihe=-1);
    void setDialog(QWidget *neibuchuangkou, const QString &title, bool modal=true,
                   const QtPrivate::FunctionPointer<voidInt>::Object *receiver=nullptr,
                   voidInt funcDialogCloseSlot=nullptr);
    void fanghuiqu(int doneResult);
    void yuyan2Liga(int yuyanIndex=-1);
    void shanYizu(int fzyyCurrentIndex);
    void jiaYizu(int fzyyCurrentIndex);
public slots:
    void qitaxinxiAppend(QString xx, bool isError=false);
    void tableAppend(QString xx);
    void showFontinfo(const QList<NameRecord> &str);
    void setFontDate(LONGDATETIME create1904,uint16 unitsPerEm);
    void showDanzi(QString &bianma, QString &ms);
    void kanlunkuo(bool fromDataChange=false, bool fromBujianDelete=false);
    void showMousePos(int ttfX,int ttfY);
    void duiqifuzhu(int fzidx);
    void xuanzeBujian(int bujianIdx, BujianGaibian bg, int xy=0, double scale=0);
    void xuanhaoLianbizi(uint32 lbzGlyphIndex);
    void xinzengJiaoben(const u16vector &shuru);
    /// 记住滚动条位置
    void jizhuWeizhi(int v);
    void updateMaodianUI(bool xianshidian);
private slots:
    void delteWenzi();
    void on_dakaiTtf_clicked();
    //void gundong_valueChanged(int value);
    void on_save_clicked();
    void on_jiaobenHani_clicked();
    void on_jiaobenLatn_clicked();
    void on_xinjian_clicked();
    void on_insert_clicked();
    void on_tabWidget_currentChanged(int index);

    void on_find_clicked();
    void on_insert_3_clicked();
    void on_insert_5_clicked();
    void awValueChanged(int arg1);
    void lsbValueChanged(int arg1);
    void tsxValueChanged(int arg1);

    void on_toolButton_19_clicked();

    void xsBd();

    void on_sfxiao_clicked();
    void on_sfda_clicked();

    void on_lsbY_clicked(bool checked);

    void on_toolButton_18_clicked();

    void on_toolButton_15_clicked();

    void on_toolButton_20_clicked();
    void on_toolButton_21_clicked();

    void on_insert_2_clicked();

    void on_p10_valueChanged(int value);

    void on_p1_valueChanged(int value);
    void on_p2_valueChanged(int value);
    void on_p3_valueChanged(int value);
    void on_p4_valueChanged(int value);
    void on_p5_valueChanged(int value);
    void on_p6_valueChanged(int value);
    void on_p7_valueChanged(int value);
    void on_p8_valueChanged(int value);
    void on_p9_valueChanged(int value);
    void on_toolButton_9_clicked();

    void on_toolButton_23_clicked();

    void on_toolButton_24_clicked();
    void on_emlog2_valueChanged(int value);
    void sbjValueChanged(int arg1);
    void xbjValueChanged(int arg1);
    void on_dakaiTtf_2_clicked();

    void on_listWidget_itemDoubleClicked(QListWidgetItem *item);

    void on_zaozi_clicked();

    void on_menxian_valueChanged(int value);

    void on_gaosi_clicked(bool checked);

    void on_fanse_clicked(bool checked);
    void on_gaosi_5_clicked();

    void on_gaosi_2_clicked();

    void on_gaosi_4_clicked();

    void on_gaosi_3_clicked();

    void on_baochi_clicked();

    void on_save_2_clicked();

    void on_delTTF_clicked();
    void on_save_4_clicked();

    void on_save_3_clicked();
    void on_lvbo_valueChanged(int value);
    void on_charu_clicked();

    void on_zuobiao_clicked(bool checked);
    void on_fenduan_clicked();

    void on_guaidian_valueChanged(int value);

    void on_zhixian_valueChanged(int value);

    void on_shangxiaBuyuejie_clicked();

    void on_toolButton_17_clicked();

    void on_toolButton_16_clicked();

    void on_insert_6_clicked();
    void on_sfda_2_clicked();

    void on_sfxiao_2_clicked();

    void on_xsSiyou_2_clicked();

    void lianbiziOver(int x);

    void on_insert_7_clicked();
    void on_yuyan_currentRowChanged(int currentRow);
    void on_yuyandefault_clicked(bool checked);
    void on_jiaoben_currentRowChanged(int currentRow);
    void on_lbShuru_currentRowChanged(int currentRow);
    void on_lbZibiao_currentRowChanged(int currentRow);
    void on_lbCover_currentRowChanged(int currentRow);
    void on_tbl_currentTextChanged(const QString &currentText);

    void on_shupai_clicked();
    void on_piliangZaoziKaishi_clicked();

    void on_shanchuBiji_clicked();
    void on_ycCtrlF_clicked();

    void on_plzh_clicked();
    void on_xiayige_2_clicked();
    void on_insert_4_clicked();    
    void on_charu_2_clicked();
    void on_fzbj_clicked(bool checked);
    void on_insert_8_clicked();

private:
    void winit();
    void saveTTFfile();
    /// 打开ttf/ttc文件.offset=0：打开ttf文件，offset>0:打开ttc中的其中一个ttf
    void DakaiTTFfile(const QString &ttfFileNameFullPath, Offset32 ttfOffset=0);
    void setHtQbUiFromTTF();
    void xiayige();
    Ui::MainWindow *ui;
    TtfFile *ttfFile;
    huatu * ht;
    quanbu* qb;
    QList<Fzyy*> fzyyList;//!<用来填写组合字的部件参数，每个组合字共用此列表
    int xiugaiShowindex;//修改用
    int32 xiugaiGlyphIndex;//!<正在修改的字符的glyphIndex
    SpinEditEx*uilsb;
    SpinEditEx*uiaw;
    SpinEditEx*uisbj;
    SpinEditEx*uixbj;
    SpinEditEx*uitsx;
    QToolButton* SaveFenzuBtn;
    QDialog *dlg;
    ToolButtonEx* zzhb;
    ToolButtonEx* lbhb;
    Image2Bzr2 ib;
    QGraphicsScene* scene;
    //std::mutex mtx;
    KeyStringList namelist;
    Kanquantu* yt;
    u16vector shuchuGlyphIdx;
    TishiChuangkou *tishi;
    PlainTextEx* pte;
    PlainTextEx* pte2;
    QProcess* jincheng;
    PteList namePte;
    PteList namePteY;
    bool keyiBaocun;//!< true:可以使用保存功能。false:不能直接保存要使用另存为功能
    int saveLiebiaoPos;
};
#endif // MAINWINDOW_H
