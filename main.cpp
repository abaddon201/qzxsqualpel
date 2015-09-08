#include <QApplication>

#include "gui/main_window.h"
#include "core/utils.h"

int main(int argc, char** argv) {
  QApplication app(argc, argv);

  initHexes();
  MainWindow* mainwnd=new MainWindow();
  mainwnd->show();
  return app.exec();
}
