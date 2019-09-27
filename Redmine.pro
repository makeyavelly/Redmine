#-------------------------------------------------
#
# Project created by QtCreator 2019-04-24T10:34:49
#
#-------------------------------------------------

QT       += core gui sql webkit

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = Redmine
TEMPLATE = app


SOURCES += main.cpp\
        MainWindow.cpp \
    ini.cpp \
    debug.cpp \
    Sql.cpp \
    widget/WidgetSettings.cpp \
    widget/Widget.cpp \
    widget/WidgetHistory.cpp \
    MainWindowTester.cpp \
    widget/WidgetSettingsTester.cpp \
    Types.cpp

HEADERS  += MainWindow.h \
    ini.h \
    debug.h \
    Types.h \
    Sql.h \
    widget/WidgetSettings.h \
    widget/Widget.h \
    widget/WidgetHistory.h \
    MainWindowTester.h \
    widget/WidgetSettingsTester.h

FORMS    += MainWindow.ui \
    widget/WidgetSettings.ui \
    widget/WidgetHistory.ui \
    MainWindowTester.ui \
    widget/WidgetSettingsTester.ui

RESOURCES += \
    icons.qrc
