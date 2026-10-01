#include "logindialog.h"
#include "mainwindow.h"
#include "style.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    a.setStyleSheet(appStyle());//全局样式

    //登录对话框作为门控: 只有登录成功才进主界面
    LoginDialog dlg;
    if (dlg.exec() != QDialog::Accepted) {
        return 0;
    }

    MainWindow w;
    w.show();
    return QApplication::exec();
}