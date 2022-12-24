#ifndef TISHICHUANGKOU_H
#define TISHICHUANGKOU_H

#include <QObject>
#include <QFrame>
#include <QLabel>
#include <QPushButton>
#include <QProgressBar>
#include <QTimer>
#include "../rizhi/rizhi.h"
#include <QApplication>

enum infoHeight{
    ihContinue=81,
    ihInfo=50
};

class TishiChuangkou : public QFrame
{
    Q_OBJECT
public:
    explicit TishiChuangkou(QWidget *parent=nullptr) : QFrame(parent)
    {
        setObjectName(QString::fromUtf8("tishichuangkou"));
        setStyleSheet(QString::fromUtf8(
                          "background-color: rgb(170, 0, 80);\n"
                          "color: rgb(255, 255, 0);"));
        tishi = new QLabel(this);
        tishi->setObjectName(QString::fromUtf8("tishixx"));
        tishi->setGeometry(QRect(20, -1, 361, 41));
        tishi->setAlignment(Qt::AlignCenter);
        tishi->setText("sdf");
        //
        okBtn = new QPushButton(this);
        okBtn->setObjectName(QString::fromUtf8("okBtn"));
        okBtn->setGeometry(QRect(240, 50, 75, 31));
        okBtn->setStyleSheet(QString::fromUtf8("border-color: rgb(125, 125, 125);\n"
                                               "color: rgb(255, 255, 255);\n"
                                               "background-color: rgb(14, 149, 117);"));
        cancelBtn = new QPushButton(this);
        cancelBtn->setObjectName(QString::fromUtf8("cancelBtn"));
        cancelBtn->setGeometry(QRect(320, 50, 75, 31));
        cancelBtn->setStyleSheet(QString::fromUtf8("border-color: rgb(125, 125, 125);\n"
                                                   "color: rgb(255, 255, 255);\n"
                                                   "background-color: rgb(14, 149, 117);"));
        connect(okBtn,&QPushButton::clicked,this,&TishiChuangkou::okClicked);
        connect(cancelBtn,&QPushButton::clicked,this,&TishiChuangkou::cancelClicked);
        //
        jdt = new QProgressBar(this);
        jdt->setObjectName(QString::fromUtf8("jdt"));
        jdt->setGeometry(QRect(20, 13, 361, 23));
        jdt->setValue(0);
        jdt->setInvertedAppearance(false);
        setWindowModality(Qt::ApplicationModal);
        setWindowFlags(Qt::Dialog|Qt::FramelessWindowHint);
        hide();
    }
    void showinfo(QString info){
        movetoCenter(ihInfo);
        QFontMetrics fm(tishi->font());
        QString xx=fm.elidedText(info,Qt::ElideMiddle,tishi->width());
        tishi->setText(xx);
        tishi->show();
        jdt->hide();
        show();
        QTimer::singleShot(2000, this, &TishiChuangkou::hideinfo);
    }

    template <typename Func3>
    void showinfoContinue(QString okStr, QString cancelStr, QString info,
                          const typename QtPrivate::FunctionPointer<Func3>::Object *receiver,
                          Func3 slot){
        okBtn->setText(okStr);
        cancelBtn->setText(cancelStr);
        movetoCenter(ihContinue);
        tishi->setText(info);
        disconnect(this,&TishiChuangkou::okClick,nullptr,nullptr);
        connect(this,&TishiChuangkou::okClick,receiver,slot);
        tishi->show();
        jdt->hide();
        show();
    }
    void showProgress(int bfb1to100){
        if(bfb1to100>=100){
            hide();
            return;
        }
        tishi->hide();
        if(!jdt->isVisible()){
            movetoCenter(ihInfo);
            jdt->show();
            show();
        }
        jdt->setValue(bfb1to100);
        QApplication::processEvents();
    }
public slots:
    void hideinfo(){
        hide();
    }
private:
    QLabel *tishi;
    QPushButton *okBtn;
    QPushButton *cancelBtn;
    QProgressBar *jdt;
    QString btnBiaojiStr;
    void movetoCenter(int height){
        int width=401;
        //屏幕坐标，原点在屏幕左上角。
        //调整弹窗位置：基于父窗口的左上角，绝对坐标。
        setGeometry((this->parentWidget()->width()-width)/2+parentWidget()->x(),
                    this->parentWidget()->height()/5+parentWidget()->y(),
                    width,height);
    }

signals:
    void cancelClick(QString bj);
    void okClick();
private slots:
    void okClicked(){
        hide();
        okClick();
        btnBiaojiStr="";
    }
    void cancelClicked(){
        hide();
        cancelClick(btnBiaojiStr);
        btnBiaojiStr="";
    }
};

#endif // TISHICHUANGKOU_H
