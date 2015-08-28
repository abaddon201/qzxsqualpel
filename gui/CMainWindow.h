#ifndef CMAINWINDOW_H
#define CMAINWINDOW_H

#include <QMainWindow>
#include <QListWidget>

#include "CDisassemblerWidget.h"
#include "CLabelsWidget.h"


class CMainWindow : public QMainWindow, public IGUIUpdater {
  Q_OBJECT
public:
  CMainWindow();
  CLabelsWidget* labelsWidget() {
    return m_LabelsWidget;
  }

  void updateWidgets() override;
private:
  //Ui::CMainWindow ui;
  QListWidget* navigationStack;
//  Highlighter* m_Highlighter;
  CDisassemblerWidget* m_DisassemblerWidget;
  CLabelsWidget* m_LabelsWidget;

  void setupFileMenu();
private slots:
  void openFile();
  void saveFile();
};

#endif
