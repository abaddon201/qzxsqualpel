#include "main_window.h"

#include <QDockWidget>
#include <QFileDialog>
#include <QMenu>
#include <QMenuBar>
#include <QApplication>

#include "zx/disassembler_core_ZX.h"

void MainWindow::updateWidgets() {
  _disassembler_widget->refreshView();
}

MainWindow::MainWindow() {
  //ui.setupUi(this);

  _disassembler_widget = new DisassemblerWidget(this);

  IDisassemblerCore *core = new DisassemblerCoreZX(this);
  core->init();
  _disassembler_widget->setCore(core);
  QDockWidget *dock = new QDockWidget(tr("Navigation Stack"), this);
  dock->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
  _navigation_stack = new QListWidget(dock);
  dock->setWidget(_navigation_stack);
  addDockWidget(Qt::RightDockWidgetArea, dock);

  dock = new QDockWidget(tr("Labels list"), this);
  dock->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
  _labels_widget = new LabelsWidget(dock, _disassembler_widget);
  dock->setWidget(_labels_widget);
  addDockWidget(Qt::RightDockWidgetArea, dock);

  //m_Highlighter=new Highlighter(m_Disassembler->document());
  setCentralWidget(_disassembler_widget);

  setupMenu();
}

void MainWindow::setupMenu() {
  QMenu *fileMenu = new QMenu(tr("&File"), this);
  menuBar()->addMenu(fileMenu);

  fileMenu->addAction(tr("&Open..."), this, SLOT(openFile()), QKeySequence(tr("Ctrl+O", "File|Open")));
  fileMenu->addAction(tr("&Save ASM..."), this, SLOT(saveFile()), QKeySequence(tr("Ctrl+S", "File|Save ASM")));
  fileMenu->addAction(tr("E&xit"), qApp, SLOT(quit()), QKeySequence(tr("Ctrl+Q", "File|Exit")));

  QMenu *disassmMenu = new QMenu(tr("&Disassm"), this);
  menuBar()->addMenu(disassmMenu);

  disassmMenu->addAction(tr("&Code"), _disassembler_widget, SLOT(makeCodeUnderCursor()),
                         QKeySequence(tr("C", "Disassm|Code")));
  disassmMenu->addSeparator();
  disassmMenu->addAction(tr("Comment command..."), _disassembler_widget, SLOT(commentCommandUnderCursor()),
                         QKeySequence(tr(";", "Disassm|Comment command")));
  disassmMenu->addAction(tr("change &Name..."), _disassembler_widget, SLOT(changeNameUnderCursor()),
                         QKeySequence(tr("N", "Disassm|change Name")));

  QMenu *toolsMenu = new QMenu(tr("&Tools"), this);
  menuBar()->addMenu(toolsMenu);

  toolsMenu->addAction(tr("&Load guesses..."), this, SLOT(loadGuesses()), QKeySequence(tr("Ctrl+L", "File|Load")));
}

void MainWindow::openFile() {
  QString fileName = QFileDialog::getOpenFileName(this, tr("Open File"), "", "RAW ZX Files (*.raw)");
  if (!fileName.isEmpty()) {
    _disassembler_widget->openRAWFile(fileName);
  }
}

void MainWindow::loadGuesses() {
  QString fileName = QFileDialog::getOpenFileName(this, tr("Load Guesses file"), "", "QSqualpel Guess Files (*.qzg)");
  if (!fileName.isEmpty()) {
    IDisassemblerCore::inst()->loadGuessFile(fileName.toStdString());
  }
}

void MainWindow::saveFile() {
  QString fileName = QFileDialog::getSaveFileName(this, tr("Save File"), "", "ASM Files (*.asm)");
  if (!fileName.isEmpty()) {
    _disassembler_widget->saveASMFile(fileName);
  }
}
