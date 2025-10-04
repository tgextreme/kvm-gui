/****************************************************************************
** Meta object code from reading C++ file 'VMConfigDialog.h'
**
** Created by: The Qt Meta Object Compiler version 68 (Qt 6.8.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../src/ui/VMConfigDialog.h"
#include <QtGui/qtextcursor.h>
#include <QtCore/qmetatype.h>

#include <QtCore/qtmochelpers.h>

#include <memory>


#include <QtCore/qxptype_traits.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'VMConfigDialog.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 68
#error "This file was generated using the moc from 6.8.2. It"
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
struct qt_meta_tag_ZN14VMConfigDialogE_t {};
} // unnamed namespace


#ifdef QT_MOC_HAS_STRINGDATA
static constexpr auto qt_meta_stringdata_ZN14VMConfigDialogE = QtMocHelpers::stringData(
    "VMConfigDialog",
    "accept",
    "",
    "onMemorySliderChanged",
    "value",
    "onCPUCountChanged",
    "addHardDisk",
    "removeHardDisk",
    "addCDDVD",
    "selectBootISO",
    "addNetworkAdapter",
    "removeNetworkAdapter",
    "addSharedFolder",
    "removeSharedFolder",
    "selectSharedFolderPath",
    "enableEncryption",
    "enabled",
    "testAudioDevice"
);
#else  // !QT_MOC_HAS_STRINGDATA
#error "qtmochelpers.h not found or too old."
#endif // !QT_MOC_HAS_STRINGDATA

Q_CONSTINIT static const uint qt_meta_data_ZN14VMConfigDialogE[] = {

 // content:
      12,       // revision
       0,       // classname
       0,    0, // classinfo
      14,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags, initial metatype offsets
       1,    0,   98,    2, 0x0a,    1 /* Public */,
       3,    1,   99,    2, 0x08,    2 /* Private */,
       5,    1,  102,    2, 0x08,    4 /* Private */,
       6,    0,  105,    2, 0x08,    6 /* Private */,
       7,    0,  106,    2, 0x08,    7 /* Private */,
       8,    0,  107,    2, 0x08,    8 /* Private */,
       9,    0,  108,    2, 0x08,    9 /* Private */,
      10,    0,  109,    2, 0x08,   10 /* Private */,
      11,    0,  110,    2, 0x08,   11 /* Private */,
      12,    0,  111,    2, 0x08,   12 /* Private */,
      13,    0,  112,    2, 0x08,   13 /* Private */,
      14,    0,  113,    2, 0x08,   14 /* Private */,
      15,    1,  114,    2, 0x08,   15 /* Private */,
      17,    0,  117,    2, 0x08,   17 /* Private */,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void, QMetaType::Int,    4,
    QMetaType::Void, QMetaType::Int,    4,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Bool,   16,
    QMetaType::Void,

       0        // eod
};

Q_CONSTINIT const QMetaObject VMConfigDialog::staticMetaObject = { {
    QMetaObject::SuperData::link<QDialog::staticMetaObject>(),
    qt_meta_stringdata_ZN14VMConfigDialogE.offsetsAndSizes,
    qt_meta_data_ZN14VMConfigDialogE,
    qt_static_metacall,
    nullptr,
    qt_incomplete_metaTypeArray<qt_meta_tag_ZN14VMConfigDialogE_t,
        // Q_OBJECT / Q_GADGET
        QtPrivate::TypeAndForceComplete<VMConfigDialog, std::true_type>,
        // method 'accept'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'onMemorySliderChanged'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<int, std::false_type>,
        // method 'onCPUCountChanged'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<int, std::false_type>,
        // method 'addHardDisk'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'removeHardDisk'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'addCDDVD'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'selectBootISO'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'addNetworkAdapter'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'removeNetworkAdapter'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'addSharedFolder'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'removeSharedFolder'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'selectSharedFolderPath'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'enableEncryption'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<bool, std::false_type>,
        // method 'testAudioDevice'
        QtPrivate::TypeAndForceComplete<void, std::false_type>
    >,
    nullptr
} };

void VMConfigDialog::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    auto *_t = static_cast<VMConfigDialog *>(_o);
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: _t->accept(); break;
        case 1: _t->onMemorySliderChanged((*reinterpret_cast< std::add_pointer_t<int>>(_a[1]))); break;
        case 2: _t->onCPUCountChanged((*reinterpret_cast< std::add_pointer_t<int>>(_a[1]))); break;
        case 3: _t->addHardDisk(); break;
        case 4: _t->removeHardDisk(); break;
        case 5: _t->addCDDVD(); break;
        case 6: _t->selectBootISO(); break;
        case 7: _t->addNetworkAdapter(); break;
        case 8: _t->removeNetworkAdapter(); break;
        case 9: _t->addSharedFolder(); break;
        case 10: _t->removeSharedFolder(); break;
        case 11: _t->selectSharedFolderPath(); break;
        case 12: _t->enableEncryption((*reinterpret_cast< std::add_pointer_t<bool>>(_a[1]))); break;
        case 13: _t->testAudioDevice(); break;
        default: ;
        }
    }
}

const QMetaObject *VMConfigDialog::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *VMConfigDialog::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_ZN14VMConfigDialogE.stringdata0))
        return static_cast<void*>(this);
    return QDialog::qt_metacast(_clname);
}

int VMConfigDialog::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QDialog::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 14)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 14;
    }
    if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 14)
            *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType();
        _id -= 14;
    }
    return _id;
}
QT_WARNING_POP
