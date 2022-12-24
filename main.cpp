#include "mainwindow.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.show();
    return a.exec();
    /// 轮廓优化
	/// 图片导入，整体导入与切割导入，自动切割
    /// 图像识别区分轮廓识别与像素识别
    /// 轮廓，变粗/变细/
}
