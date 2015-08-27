#include "CMainWindow.h"

#include <QDockWidget>
#include <QFileDialog>
#include <QMenu>
#include <QMenuBar>
#include <QApplication>

#include "CDisassemblerCoreZX.h"

CMainWindow::CMainWindow() {
  //ui.setupUi(this);

  setupFileMenu();

  m_Disassembler=new CDisassembler(this);

  IDisassemblerCore* core=new CDisassemblerCoreZX();
  m_Disassembler->setCore(core);
  QDockWidget* dock=new QDockWidget(tr("Navigation Stack"), this);
  dock->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
  navigationStack = new QListWidget(dock);
  dock->setWidget(navigationStack);
  addDockWidget(Qt::RightDockWidgetArea, dock);

  dock=new QDockWidget(tr("Labels list"), this);
  dock->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
  m_LabelsWidget = new CLabelsWidget(dock);
  dock->setWidget(m_LabelsWidget);
  addDockWidget(Qt::RightDockWidgetArea, dock);

  //m_Highlighter=new Highlighter(m_Disassembler->document());
  setCentralWidget(m_Disassembler);
}

void CMainWindow::setupFileMenu() {
  QMenu* fileMenu = new QMenu(tr("&File"), this);
  menuBar()->addMenu(fileMenu);

  fileMenu->addAction(tr("&Open..."), this, SLOT(openFile()),
                      QKeySequence(tr("Ctrl+O",
                                      "File|Open")));
  fileMenu->addAction(tr("&Save..."), this, SLOT(saveFile()),
                      QKeySequence(tr("Ctrl+S",
                                      "File|Save")));
  fileMenu->addAction(tr("E&xit"), qApp, SLOT(quit()),
                      QKeySequence(tr("Ctrl+Q",
                                      "File|Exit")));
}

void CMainWindow::openFile() {
  QString fileName = QFileDialog::getOpenFileName(this, tr("Open File"), "", "RAW ZX Files (*.raw)");
  if (!fileName.isEmpty()) {
    m_Disassembler->openRAWFile(fileName);
  }
}

void CMainWindow::saveFile() {
  QString fileName = QFileDialog::getSaveFileName(this, tr("Save File"), "", "ASM Files (*.asm)");
  if (!fileName.isEmpty()) {
    m_Disassembler->saveASMFile(fileName);
  }
}
