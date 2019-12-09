#ifndef CMAINWINDOW_H
#define CMAINWINDOW_H

#include <QMainWindow>
#include <QListWidget>
#include <QCloseEvent>
#include <QStatusBar>

#include "disassembler_widget.h"
#include "labels_widget.h"

namespace dasm {
namespace gui {

class MainWindow : public QMainWindow, public IGUIUpdater {
  Q_OBJECT
public:
  MainWindow();

  LabelsWidget* labelsWidget() {
    return _labels_widget;
  }

  void updateWidgets() override;

  void onAddressUpdated(uint16_t addr, uint16_t bytes) override;

  void onPositionChanged(int pos) override;

  void closeEvent(QCloseEvent* event);

  void showMessage(const std::string& msg, int timeout = 0) {
    statusBar()->showMessage(QString::fromStdString(msg), timeout);
  }

  void clearMessage() {
    statusBar()->clearMessage();
  }

private:
  //Ui::CMainWindow ui;
  QListWidget* _navigation_stack;
  //  Highlighter* m_Highlighter;
  DisassemblerWidget* _disassembler_widget;
  LabelsWidget* _labels_widget;

  void setupMenu();

private slots:

  void loadGuesses();

  void openFile();

  void saveProject();
  void openProject();

  void saveAsm();
};

}
}

#endif
