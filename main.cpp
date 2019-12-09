#include <QApplication>

#include "gui/main_window.h"
#include "utils/utils.h"

#include "utils/plog/Init.h"
#include "utils/plog/Severity.h"
#include "utils/plog/Appenders/ConsoleAppender.h"
#include "utils/plog/Formatters/TxtFormatter.h"

static plog::ConsoleAppender<plog::TxtFormatter> consoleAppender;

int main(int argc, char** argv) {
  QApplication app(argc, argv);

  //plog::init(plog::debug, &consoleAppender);
  plog::init(plog::error, &consoleAppender);
  //plog::init(plog::debug, "log.txt");

  dasm::utils::initHexes();
  auto mainwnd = new dasm::gui::MainWindow();
  mainwnd->show();
  return app.exec();
}
