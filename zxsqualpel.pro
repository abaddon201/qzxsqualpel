######################################################################
# Automatically generated by qmake (3.0) ?? 19. ??? 23:09:33 2013
######################################################################

QT += core gui widgets
TEMPLATE = app
TARGET = zxsqualpel
INCLUDEPATH += .
CONFIG += c++14
QMAKE_CXXFLAGS += -std=c++14


# Input
HEADERS += \
           gui/CWidgetChangeLabel.h \
           zx/disassemble.h \
           gui/CLabelsWidget.h \
           gui/CMainWindow.h \
    core/utils.h \
    core/debug_printers.h \
    gui/guichunk.h \
    gui/guichunklist.h \
    core/segment.h \
    core/memory.h \
    gui/helper_widgets.h \
    gui/qt_debug_printers.h \
    core/arg.h \
    core/addr.h \
    core/byte.h \
    core/chunk.h \
    core/chunk_list.h \
    core/command.h \
    core/label.h \
    core/labels.h \
    core/reference.h \
    core/i_disassembler_core.h \
    zx/disassembler_core_ZX.h \
    gui/disassembler_widget.h
SOURCES += \
           zx/disassemble.cpp \
           gui/CWidgetChangeLabel.cpp \
           gui/CLabelsWidget.cpp \
           gui/CMainWindow.cpp \
           main.cpp \
    gui/guichunklist.cpp \
    core/segment.cpp \
    core/memory.cpp \
    core/utils.cpp \
    zx/zx_autocommenter.cpp \
    core/addr.cpp \
    core/byte.cpp \
    core/chunk.cpp \
    core/chunk_list.cpp \
    core/command.cpp \
    core/label.cpp \
    core/labels.cpp \
    core/reference.cpp \
    core/i_disassembler_core.cpp \
    zx/disassembler_core_ZX.cpp \
    gui/disassembler_widget.cpp

#win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../../Qt5_0_1/5.0.1/msvc2010/lib/ -lQt5Widgets
#else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../../Qt5_0_1/5.0.1/msvc2010/lib/ -lQt5Widgetsd
#else:unix: LIBS += -L$$PWD/../../Qt5_0_1/5.0.1/msvc2010/lib/ -lQt5Widgets

#INCLUDEPATH += $$PWD/../../Qt5_0_1/5.0.1/msvc2010/include
#DEPENDPATH += $$PWD/../../Qt5_0_1/5.0.1/msvc2010/include

DISTFILES += \
    .astylerc

FORMS += \
    gui/goto_address.ui
