#include <QApplication>

#include "gui/CMainWindow.h"
#include "core/utils.h"

int main(int argc, char** argv) {
  QApplication app(argc, argv);

  initHexes();
  CMainWindow* mainwnd=new CMainWindow();
  mainwnd->show();
  return app.exec();
}
