QT       += core gui serialport

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = k226ECCTool
TEMPLATE = app

macx {
    # mac only
}
unix:!macx{
    # linux only
    QMAKE_CXX= color_compile ccache g++
}
win32 {
    # windows only
    QMAKE_CXXFLAGS += -march=i686
}

SOURCES += main.cpp\
    mainwindow.cpp\
	serialworker.cpp \
    xcanframe.cpp \
    xframe.cpp

HEADERS  += mainwindow.h\
	serialworker.h \
    xcanframe.h \
    xframe.h

FORMS    += mainwindow.ui

DESTDIR += ../output/

INCLUDEPATH += ../libUtils ../libQtDbc/inc
LIBS+= -L../output/libQtDbc -lQtDbc
