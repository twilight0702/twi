QT       += core gui
QT+=sql
QT+=core gui sql

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    courses.cpp \
    login.cpp \
    main.cpp \
    student.cpp \
    tip.cpp \
    widget.cpp

HEADERS += \
    courses.h \
    login.h \
    student.h \
    tip.h \
    widget.h

FORMS += \
    courses.ui \
    login.ui \
    student.ui \
    tip.ui \
    widget.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
