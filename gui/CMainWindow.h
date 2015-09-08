#ifndef CMAINWINDOW_H
#define CMAINWINDOW_H

#include <QMainWindow>
#include <QListWidget>

#include "disassembler_widget.h"
#include "CLabelsWidget.h"


class MainWindow : public QMainWindow, public IGUIUpdater {
  Q_OBJECT
public:
  MainWindow();
  LabelsWidget* labelsWidget() {
    return _labels_widget;
  }

  void updateWidgets() override;

private:
  //Ui::CMainWindow ui;
  QListWidget* _navigation_stack;
//  Highlighter* m_Highlighter;
  DisassemblerWidget* _disassembler_widget;
  LabelsWidget* _labels_widget;

  void setupFileMenu();

private slots:
  void loadGuesses();

  void openFile();
  void saveFile();
};

#endif
