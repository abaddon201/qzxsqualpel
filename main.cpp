#include <QApplication>

#include "gui/main_window.h"
#include "utils/utils.h"

int main(int argc, char** argv) {
  QApplication app(argc, argv);

  dasm::utils::initHexes();
  auto mainwnd = new dasm::gui::MainWindow();
  mainwnd->show();
  return app.exec();
}
