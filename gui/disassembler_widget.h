#ifndef CDISASSEMBLER_H
#define CDISASSEMBLER_H

#include <QString>
#include <QByteArray>
#include <QList>
#include <QPlainTextEdit>

#include "core/label.h"
#include "core/labels.h"
#include "core/disassembler_core.h"

#include "guiblocklist.h"

namespace dasm {
namespace gui {

class MainWindow;

class DisassemblerWidget : public QPlainTextEdit {
public:
  DisassemblerWidget(MainWindow* mwnd);

  void openRAWFile(const QString& fileName);
  void saveProjectFile(const QString& fileName);
  void openProjectFile(const QString& fileName);
  void saveASMFile(const QString& fileName);

  ///@brief Показать на экране код с меткой под номером
  void navigateToAddress(uint16_t from_addr, uint16_t addr);
  void navigateToAddress(uint16_t addr);
  void navigateToReference();

  void makeArray(int size, bool clearMem);

  void paintEvent(QPaintEvent* event);
  void refreshView();

  core::CommandPtr getCmdUnderCursor();
  GUIBlockPtr getGuiCmdUnderCursor();

  void keyPressEvent(QKeyEvent*);
  void setCursorPosition(int position);

public slots:
  void changeNameUnderCursor();
  void makeCodeUnderCursor();
  void makeArrayUnderCursor();
  void commentCommandUnderCursor();
  void blockCommentUnderCursor();

private:
  DisassemblerWidget();
  void init();

  void navigateToAddrDlg();

  int _references_on_line;

  MainWindow* _main_window;

  GUIBlockList _commands;
  Q_OBJECT
};

}
}
#endif
