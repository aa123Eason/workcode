#include "mainwindow.h"

#include <QApplication>
#include <QSharedMemory>
#include <QProcess>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

//    static QSharedMemory *sharMemory = new QSharedMemory("demo");   //新建共享内存，内存key为"Single_myexe_App"，不同exe程序可以设置不同名称，用以区分运行程序。
//    if(sharMemory->attach())  //判断是否有同名称"Single_myexe_App"的共享内存空间被占用。
//    {
////        QProcess pro;
////        pro.startDetached("pkill demo");
////        pro.close();
//        //        QMessageBox::information(NULL,QStringLiteral("Warning"),QStringLiteral(" 应用程序已运行！ "));

//          //如果没有相同名称的共享内存被占用，则新建名称为"Single_myexe_App"的共享内存，大小最小为1，不涉及共享内存存储或内容传递，建立最小共享内存即可。
//        return 0;  //如果有相同名称的共享内存占用，说明该程序已启动，弹出提醒框，返回值0，退出程序。
//    }

//    sharMemory->create(1);


    MainWindow *w = new MainWindow;
    w->show();
    return a.exec();
}
