QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

LIBS += -lVersion
LIBS +=D:/test/opencv4/buildmgw/install/x64/mingw/lib/libopencv_imgproc455.dll.a
LIBS +=D:/test/opencv4/buildmgw/install/x64/mingw/lib/libopencv_imgcodecs455.dll.a
LIBS +=D:/test/opencv4/buildmgw/install/x64/mingw/lib/libopencv_core455.dll.a
INCLUDEPATH+=../opencv4/buildmgw/install/include

# The following define makes your compiler emit warnings if you use
# any Qt feature that has been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    ../rizhi/rizhi.cpp \
    arctobazer.cpp \
    cfftable.cpp \
    fontpath.cpp \
    fzyy.cpp \
    gpossubtable.cpp \
    gsubsubtable.cpp \
    huatu.cpp \
    image2bzr2.cpp \
    kanquantu.cpp \
    lianbiziui.cpp \
    main.cpp \
    mainwindow.cpp \
    mainwindow2.cpp \
    quanbu.cpp \
    subtablecomm.cpp \
    table2.cpp \
    tableCmap.cpp \
    tableComm.cpp \
    tableDSIG.cpp \
    tableGDEF.cpp \
    tableGPOS.cpp \
    tableGasp.cpp \
    tableGlyfLoca.cpp \
    tableName.cpp \
    tablePost.cpp \
    tablegsub.cpp \
    tables1.cpp \
    tablevhea.cpp \
    ttcfile.cpp \
    ttffile.cpp \
    ttfhead.cpp \
    types.cpp

HEADERS += \
    ../qtJiemianKuozhan/PlainTextEx.h \
    ../qtJiemianKuozhan/spineditex.h \
    ../rizhi/rizhi.h \
    arctobazer.h \
    cffString.h \
    cfftable.h \
    cmapzifu3500.h \
    cmapzifu6500.h \
    fontpath.h \
    fzyy.h \
    gpossubtable.h \
    gsubsubtable.h \
    huatu.h \
    image2bzr2.h \
    kanquantu.h \
    mainwindow.h \
    pinyinku.h \
    quanbu.h \
    subtablecomm.h \
    table2.h \
    tableCmap.h \
    tableComm.h \
    tableDSIG.h \
    tableGDEF.h \
    tableGPOS.h \
    tableGasp.h \
    tableGlyfLoca.h \
    tableName.h \
    tablePost.h \
    tablegsub.h \
    tables1.h \
    tablevhea.h \
    tishichuangkou.h \
    ttcfile.h \
    ttffile.h \
    ttfhead.h \
    types.h

FORMS += \
    mainwindow.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RC_ICONS+=tb.ico

VERSION = 1.0.0.1471

