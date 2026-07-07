cmake_minimum_required(VERSION 3.20)

project(harsis
    VERSION 2.0.0
    DESCRIPTION "HARSIS - Hastane Randevu Sistemi"
    LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

include_directories(include)

# Faz 1: Sadece STL (Qt yok)
add_executable(harsis src/main.cpp)

if(MSVC)
    target_compile_options(harsis PRIVATE /W4 /permissive-)
else()
    target_compile_options(harsis PRIVATE -Wall -Wextra -Wpedantic)
endif()

# Faz 2+: Qt6 GUI Uygulaması
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

find_package(Qt6 COMPONENTS Widgets Network Charts PrintSupport QUIET)

if(Qt6_FOUND)
    message(STATUS "Qt6 bulundu, GUI uygulaması derleniyor...")
    
    add_executable(harsis_qt
        # Faz 2: Temel GUI
        src/main_qt.cpp
        src/main_window.cpp
        src/hasta_dialog.cpp
        src/doktor_dialog.cpp
        src/randevu_dialog.cpp
        src/hasta_model.cpp
        src/doktor_model.cpp
        src/randevu_model.cpp
        # Faz 3: Çok Kanallı Programlama
        src/hatirlatma_servisi.cpp
        src/rapor_uretici.cpp
        src/csv_aktarici.cpp
        # Faz 4: Ağ Programlama
        src/sunucu.cpp
        src/istemci.cpp
    )
    
    target_link_libraries(harsis_qt PRIVATE 
        Qt6::Widgets
        Qt6::Network
    )
    
    # Charts ve PrintSupport opsiyonel
    if(TARGET Qt6::Charts)
        target_link_libraries(harsis_qt PRIVATE Qt6::Charts)
        target_compile_definitions(harsis_qt PRIVATE HAVE_QTCHARTS)
    endif()
    
    if(TARGET Qt6::PrintSupport)
        target_link_libraries(harsis_qt PRIVATE Qt6::PrintSupport)
    endif()
    
    if(MSVC)
        target_compile_options(harsis_qt PRIVATE /W4 /permissive-)
    else()
        target_compile_options(harsis_qt PRIVATE -Wall -Wextra -Wpedantic)
    endif()
    
    message(STATUS "GUI uygulaması hedefi: harsis_qt")
else()
    message(WARNING "Qt6 bulunamadı. Sadece konsol uygulaması (harsis) derlenecek.")
    message(STATUS "Qt6 kurmak için: brew install qt6")
endif()