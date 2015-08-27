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
HEADERS += CAddr.h \
           CByte.h \
           CChunk.h \
           CChunkList.h \
           CCommand.h \
           CDisassembler.h \
           CDisassemblerCoreZX.h \
           CLabel.h \
           CLabels.h \
           CLabelsWidget.h \
           CMainWindow.h \
           CReference.h \
           CWidgetChangeLabel.h \
           disassemble.h \
           IDisassemblerCore.h
SOURCES += CAddr.cpp \
           CByte.cpp \
           CChunk.cpp \
           CChunkList.cpp \
           CCommand.cpp \
           CDisassembler.cpp \
           CDisassemblerCoreZX.cpp \
           CLabel.cpp \
           CLabels.cpp \
           CLabelsWidget.cpp \
           CMainWindow.cpp \
           CReference.cpp \
           CWidgetChangeLabel.cpp \
           disassemble.cpp \
           main.cpp

#win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../../Qt5_0_1/5.0.1/msvc2010/lib/ -lQt5Widgets
#else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../../Qt5_0_1/5.0.1/msvc2010/lib/ -lQt5Widgetsd
#else:unix: LIBS += -L$$PWD/../../Qt5_0_1/5.0.1/msvc2010/lib/ -lQt5Widgets

#INCLUDEPATH += $$PWD/../../Qt5_0_1/5.0.1/msvc2010/include
#DEPENDPATH += $$PWD/../../Qt5_0_1/5.0.1/msvc2010/include

DISTFILES += \
    .astylerc
