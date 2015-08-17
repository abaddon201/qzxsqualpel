#ifndef CMAINWINDOW_H
#define CMAINWINDOW_H

#include <QMainWindow>
#include <QListWidget>

//#include "ui_CMainWindow.h"

//#include "CHighlighter.h"
#include "CDisassembler.h"
#include "CLabelsWidget.h"


class CMainWindow : public QMainWindow {
    Q_OBJECT
public:
    CMainWindow();
    CLabelsWidget* labelsWidget() {
        return m_LabelsWidget;
    }
private:
    //Ui::CMainWindow ui;
    QListWidget *navigationStack;
//  Highlighter* m_Highlighter;
    CDisassembler* m_Disassembler;
    CLabelsWidget* m_LabelsWidget;

    void setupFileMenu();
private slots:
    void openFile();
    void saveFile();
};

#endif
