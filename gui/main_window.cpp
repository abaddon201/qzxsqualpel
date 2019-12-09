#include "main_window.h"

#include <QDockWidget>
#include <QFileDialog>
#include <QMenu>
#include <QMenuBar>
#include <QApplication>
#include <QMessageBox>
#include <QDesktopWidget>

#include "navigation_stack.h"
#include "core/navigation_stack.h"

namespace dasm {
namespace gui {

void MainWindow::updateWidgets() {
  _disassembler_widget->refreshView();
}

void MainWindow::onAddressUpdated(uint16_t addr, uint16_t bytes) {
  _disassembler_widget->onAddressUpdated(addr, bytes);
}

MainWindow::MainWindow() {
  //ui.setupUi(this);

  _disassembler_widget = new DisassemblerWidget(this);

  dasm::core::DisassemblerCore::inst().init(this);

  QDockWidget* dock = new QDockWidget(tr("Navigation Stack"), this);
  dock->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
  _navigation_stack = new QListWidget(dock);
  dock->setWidget(_navigation_stack);
  addDockWidget(Qt::RightDockWidgetArea, dock);
  dasm::core::NavigationStack::inst().setListener(&dasm::gui::NavigationStack::inst());
  dasm::gui::NavigationStack::inst().addView(_navigation_stack);

  dock = new QDockWidget(tr("Labels list"), this);
  dock->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
  _labels_widget = new LabelsWidget(dock, _disassembler_widget);
  dock->setWidget(_labels_widget);
  addDockWidget(Qt::RightDockWidgetArea, dock);

  //m_Highlighter=new Highlighter(m_Disassembler->document());
  setCentralWidget(_disassembler_widget);
  adjustSize();
  move(QApplication::desktop()->availableGeometry().center() - this->rect().center());
  setupMenu();
}

void MainWindow::setupMenu() {
  QMenu* fileMenu = new QMenu(tr("&File"), this);
  menuBar()->addMenu(fileMenu);

  fileMenu->addAction(tr("Open &Bin..."), this, SLOT(openFile()), QKeySequence(tr("Ctrl+B", "File|Open")));
  fileMenu->addAction(tr("&Open project..."), this, SLOT(openProject()), QKeySequence(tr("Ctrl+O", "File|Open")));
  fileMenu->addAction(tr("&Save project..."), this, SLOT(saveProject()), QKeySequence(tr("Ctrl+S", "File|Save")));
  fileMenu->addAction(tr("&Save ASM..."), this, SLOT(saveAsm()), QKeySequence(tr("Ctrl+D", "File|Save ASM")));
  fileMenu->addAction(tr("E&xit"), qApp, SLOT(quit()), QKeySequence(tr("Ctrl+Q", "File|Exit")));

  QMenu* disassmMenu = new QMenu(tr("&Disassm"), this);
  menuBar()->addMenu(disassmMenu);

  disassmMenu->addAction(tr("&Code"), _disassembler_widget, SLOT(makeCodeUnderCursor()),
                         QKeySequence(tr("C", "Disassm|Code")));
  disassmMenu->addAction(tr("Array"), _disassembler_widget, SLOT(makeArrayUnderCursor()),
                         QKeySequence(tr("*", "Disassm|Code")));
  disassmMenu->addSeparator();
  disassmMenu->addAction(tr("Comment command..."), _disassembler_widget, SLOT(commentCommandUnderCursor()),
                         QKeySequence(tr(";", "Disassm|Comment command")));
  disassmMenu->addAction(tr("change &Name..."), _disassembler_widget, SLOT(changeNameUnderCursor()),
                         QKeySequence(tr("N", "Disassm|change Name")));

  QMenu* toolsMenu = new QMenu(tr("&Tools"), this);
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
    dasm::core::DisassemblerCore::inst().loadGuessFile(fileName.toStdString());
  }
}

void MainWindow::saveAsm() {
  QString fileName = QFileDialog::getSaveFileName(this, tr("Save Asm"), "", "ASM Files (*.asm)");
  if (!fileName.isEmpty()) {
    _disassembler_widget->saveASMFile(fileName);
  }
}

void MainWindow::saveProject() {
  QString fileName = QFileDialog::getSaveFileName(this, tr("Save Project"), "", "Project files (*.qzx)");
  if (!fileName.isEmpty()) {
    _disassembler_widget->saveProjectFile(fileName);
    core::DisassemblerCore::inst().resetModified();
  }
}

void MainWindow::openProject() {
  QString fileName = QFileDialog::getOpenFileName(this, tr("Open Project"), "", "Project Files (*.qzx)");
  if (!fileName.isEmpty()) {
    _disassembler_widget->openProjectFile(fileName);
    core::DisassemblerCore::inst().resetModified();
  }
}

void MainWindow::closeEvent(QCloseEvent* event) {
  if (!core::DisassemblerCore::inst().isModified()) {
    event->accept();
    return;
  }
  QMessageBox::StandardButton resBtn = QMessageBox::question(this, "QZXSqualpel",
                                                             tr("You have unsaved changes.\n"),
                                                             QMessageBox::Cancel | QMessageBox::Save | QMessageBox::Close,
                                                             QMessageBox::Cancel);
  switch (resBtn) {
    case QMessageBox::Close:
      event->accept();
      return;
    case QMessageBox::Cancel:
      event->ignore();
      return;
    case QMessageBox::Save:
      event->ignore();
      saveProject();
      return;
  }
}
}
}
