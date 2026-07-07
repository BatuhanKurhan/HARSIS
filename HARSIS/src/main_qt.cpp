/**
 * main_qt.cpp -- HARSIS Qt6 Uygulaması Başlangıç Noktası
 *
 * Hastane Randevu Sistemi - Faz 2: Qt6 GUI
 *
 * Derleme:
 *   mkdir build && cd build
 *   cmake .. -DCMAKE_PREFIX_PATH=/opt/homebrew
 *   cmake --build .
 *   ./harsis_qt
 */

#include "../include/main_window.h"

#include <QApplication>
#include <QLocale>
#include <QTranslator>

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    
    // Uygulama bilgileri
    app.setApplicationName("HARSIS");
    app.setApplicationDisplayName("Hastane Randevu Sistemi");
    app.setOrganizationName("HARSIS");
    app.setOrganizationDomain("harsis.local");
    
    // Ana pencereyi oluştur ve göster
    MainWindow mainWindow;
    mainWindow.show();
    
    return app.exec();
}
