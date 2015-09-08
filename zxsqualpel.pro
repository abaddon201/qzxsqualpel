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
           core/CByte.h \
           core/CChunk.h \
           core/CChunkList.h \
           core/CCommand.h \
           core/IDisassemblerCore.h \
           core/CLabel.h \
           core/CReference.h \
           core/CLabels.h \
           gui/CWidgetChangeLabel.h \
           zx/CDisassemblerCoreZX.h \
           zx/disassemble.h \
           gui/CLabelsWidget.h \
           gui/CMainWindow.h \
           gui/CDisassemblerWidget.h \
    core/utils.h \
    core/debug_printers.h \
    gui/guichunk.h \
    gui/guichunklist.h \
    core/segment.h \
    core/memory.h \
    gui/helper_widgets.h \
    gui/qt_debug_printers.h \
    core/arg.h \
    core/addr.h
SOURCES += \
           core/CByte.cpp \
           core/CChunk.cpp \
           core/CLabel.cpp \
           core/CCommand.cpp \
           core/CReference.cpp \
           core/CChunkList.cpp \
           core/CLabels.cpp \
          core/IDisassemblerCore.cpp \
           zx/CDisassemblerCoreZX.cpp \
           zx/disassemble.cpp \
           gui/CWidgetChangeLabel.cpp \
           gui/CLabelsWidget.cpp \
           gui/CMainWindow.cpp \
           gui/CDisassemblerWidget.cpp \
           main.cpp \
    gui/guichunklist.cpp \
    core/segment.cpp \
    core/memory.cpp \
    core/utils.cpp \
    zx/zx_autocommenter.cpp \
    core/addr.cpp

#win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../../Qt5_0_1/5.0.1/msvc2010/lib/ -lQt5Widgets
#else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../../Qt5_0_1/5.0.1/msvc2010/lib/ -lQt5Widgetsd
#else:unix: LIBS += -L$$PWD/../../Qt5_0_1/5.0.1/msvc2010/lib/ -lQt5Widgets

#INCLUDEPATH += $$PWD/../../Qt5_0_1/5.0.1/msvc2010/include
#DEPENDPATH += $$PWD/../../Qt5_0_1/5.0.1/msvc2010/include

DISTFILES += \
    .astylerc

FORMS += \
    gui/goto_address.ui
