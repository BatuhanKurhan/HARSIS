/****************************************************************************
** Meta object code from reading C++ file 'rapor_uretici.h'
**
** Created by: The Qt Meta Object Compiler version 69 (Qt 6.11.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "include/rapor_uretici.h"
#include <QtCore/qmetatype.h>

#include <QtCore/qtmochelpers.h>

#include <memory>


#include <QtCore/qxptype_traits.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'rapor_uretici.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 69
#error "This file was generated using the moc from 6.11.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

#ifndef Q_CONSTINIT
#define Q_CONSTINIT
#endif

QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
QT_WARNING_DISABLE_GCC("-Wuseless-cast")
namespace {
struct qt_meta_tag_ZN12RaporUreticiE_t {};
} // unnamed namespace

template <> constexpr inline auto RaporUretici::qt_create_metaobjectdata<qt_meta_tag_ZN12RaporUreticiE_t>()
{
    namespace QMC = QtMocConstants;
    QtMocHelpers::StringRefStorage qt_stringData {
        "RaporUretici",
        "ilerlemeDegisti",
        "",
        "yuzde",
        "raporHazir",
        "RaporSonucu",
        "sonuc",
        "hataOlustu",
        "mesaj"
    };

    QtMocHelpers::UintData qt_methods {
        // Signal 'ilerlemeDegisti'
        QtMocHelpers::SignalData<void(int)>(1, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::Int, 3 },
        }}),
        // Signal 'raporHazir'
        QtMocHelpers::SignalData<void(const RaporSonucu &)>(4, 2, QMC::AccessPublic, QMetaType::Void, {{
            { 0x80000000 | 5, 6 },
        }}),
        // Signal 'hataOlustu'
        QtMocHelpers::SignalData<void(const QString &)>(7, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::QString, 8 },
        }}),
    };
    QtMocHelpers::UintData qt_properties {
    };
    QtMocHelpers::UintData qt_enums {
    };
    return QtMocHelpers::metaObjectData<RaporUretici, qt_meta_tag_ZN12RaporUreticiE_t>(QMC::MetaObjectFlag{}, qt_stringData,
            qt_methods, qt_properties, qt_enums);
}
Q_CONSTINIT const QMetaObject RaporUretici::staticMetaObject = { {
    QMetaObject::SuperData::link<QThread::staticMetaObject>(),
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN12RaporUreticiE_t>.stringdata,
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN12RaporUreticiE_t>.data,
    qt_static_metacall,
    nullptr,
    qt_staticMetaObjectRelocatingContent<qt_meta_tag_ZN12RaporUreticiE_t>.metaTypes,
    nullptr
} };

void RaporUretici::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    auto *_t = static_cast<RaporUretici *>(_o);
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: _t->ilerlemeDegisti((*reinterpret_cast<std::add_pointer_t<int>>(_a[1]))); break;
        case 1: _t->raporHazir((*reinterpret_cast<std::add_pointer_t<RaporSonucu>>(_a[1]))); break;
        case 2: _t->hataOlustu((*reinterpret_cast<std::add_pointer_t<QString>>(_a[1]))); break;
        default: ;
        }
    }
    if (_c == QMetaObject::IndexOfMethod) {
        if (QtMocHelpers::indexOfMethod<void (RaporUretici::*)(int )>(_a, &RaporUretici::ilerlemeDegisti, 0))
            return;
        if (QtMocHelpers::indexOfMethod<void (RaporUretici::*)(const RaporSonucu & )>(_a, &RaporUretici::raporHazir, 1))
            return;
        if (QtMocHelpers::indexOfMethod<void (RaporUretici::*)(const QString & )>(_a, &RaporUretici::hataOlustu, 2))
            return;
    }
}

const QMetaObject *RaporUretici::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *RaporUretici::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_staticMetaObjectStaticContent<qt_meta_tag_ZN12RaporUreticiE_t>.strings))
        return static_cast<void*>(this);
    return QThread::qt_metacast(_clname);
}

int RaporUretici::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QThread::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 3)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 3;
    }
    if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 3)
            *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType();
        _id -= 3;
    }
    return _id;
}

// SIGNAL 0
void RaporUretici::ilerlemeDegisti(int _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 0, nullptr, _t1);
}

// SIGNAL 1
void RaporUretici::raporHazir(const RaporSonucu & _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 1, nullptr, _t1);
}

// SIGNAL 2
void RaporUretici::hataOlustu(const QString & _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 2, nullptr, _t1);
}
QT_WARNING_POP
