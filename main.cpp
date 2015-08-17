#include <QApplication>

#include "CMainWindow.h"

int main(int argc, char** argv) {
    QApplication app(argc, argv);

    CMainWindow* mainwnd=new CMainWindow();
    mainwnd->show();
    return app.exec();
}
