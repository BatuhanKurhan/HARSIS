QT += core gui widgets network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++20

TARGET = harsis_qt
TEMPLATE = app

INCLUDEPATH += include

SOURCES += \
    src/main_qt.cpp \
    src/main_window.cpp \
    src/hasta_dialog.cpp \
    src/doktor_dialog.cpp \
    src/randevu_dialog.cpp \
    src/hasta_model.cpp \
    src/doktor_model.cpp \
    src/randevu_model.cpp \
    src/hatirlatma_servisi.cpp \
    src/rapor_uretici.cpp \
    src/csv_aktarici.cpp \
    src/sunucu.cpp \
    src/istemci.cpp

HEADERS += \
    include/varliklar.h \
    include/depo.h \
    include/main_window.h \
    include/hasta_dialog.h \
    include/doktor_dialog.h \
    include/randevu_dialog.h \
    include/hasta_model.h \
    include/doktor_model.h \
    include/randevu_model.h \
    include/hatirlatma_servisi.h \
    include/rapor_uretici.h \
    include/csv_aktarici.h \
    include/sunucu.h \
    include/istemci.h

# Derleme seçenekleri
QMAKE_CXXFLAGS += -std=c++20

# macOS için
macx {
    QMAKE_MACOSX_DEPLOYMENT_TARGET = 11.0
}
