#include "CMainWindow.h"

#include <QDockWidget>
#include <QFileDialog>
#include <QMenu>
#include <QMenuBar>
#include <QApplication>

#include "zx/CDisassemblerCoreZX.h"

void CMainWindow::updateWidgets() {
  m_DisassemblerWidget->refreshView();
}

CMainWindow::CMainWindow() {
  //ui.setupUi(this);

  setupFileMenu();

  m_DisassemblerWidget=new CDisassemblerWidget(this);

  IDisassemblerCore* core=new CDisassemblerCoreZX(this);
  core->init();
  m_DisassemblerWidget->setCore(core);
  QDockWidget* dock=new QDockWidget(tr("Navigation Stack"), this);
  dock->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
  navigationStack = new QListWidget(dock);
  dock->setWidget(navigationStack);
  addDockWidget(Qt::RightDockWidgetArea, dock);

  dock=new QDockWidget(tr("Labels list"), this);
  dock->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
  m_LabelsWidget = new CLabelsWidget(dock, m_DisassemblerWidget);
  dock->setWidget(m_LabelsWidget);
  addDockWidget(Qt::RightDockWidgetArea, dock);

  //m_Highlighter=new Highlighter(m_Disassembler->document());
  setCentralWidget(m_DisassemblerWidget);
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
  QMenu* toolsMenu = new QMenu(tr("&Tools"), this);
  menuBar()->addMenu(toolsMenu);

  toolsMenu->addAction(tr("&Load guesses..."), this, SLOT(loadGuesses()),
                      QKeySequence(tr("Ctrl+L",
                                      "File|Load")));
}

void CMainWindow::openFile() {
  QString fileName = QFileDialog::getOpenFileName(this, tr("Open File"), "", "RAW ZX Files (*.raw)");
  if (!fileName.isEmpty()) {
    m_DisassemblerWidget->openRAWFile(fileName);
  }
}

void CMainWindow::loadGuesses() {
  QString fileName = QFileDialog::getOpenFileName(this, tr("Load Guesses file"), "", "QSqualpel Guess Files (*.qzg)");
  if (!fileName.isEmpty()) {
    IDisassemblerCore::inst()->loadGuessFile(fileName.toStdString());
  }
}

void CMainWindow::saveFile() {
  QString fileName = QFileDialog::getSaveFileName(this, tr("Save File"), "", "ASM Files (*.asm)");
  if (!fileName.isEmpty()) {
    m_DisassemblerWidget->saveASMFile(fileName);
  }
}
