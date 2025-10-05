/****************************************************************************
** Meta object code from reading C++ file 'VMConfigDialog.h'
**
** Created by: The Qt Meta Object Compiler version 68 (Qt 6.4.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include <memory>
#include "../../../src/ui/VMConfigDialog.h"
#include <QtGui/qtextcursor.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'VMConfigDialog.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 68
#error "This file was generated using the moc from 6.4.2. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

#ifndef Q_CONSTINIT
#define Q_CONSTINIT
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
namespace {
struct qt_meta_stringdata_VMConfigDialog_t {
    uint offsetsAndSizes[36];
    char stringdata0[15];
    char stringdata1[7];
    char stringdata2[1];
    char stringdata3[22];
    char stringdata4[6];
    char stringdata5[18];
    char stringdata6[12];
    char stringdata7[15];
    char stringdata8[9];
    char stringdata9[14];
    char stringdata10[18];
    char stringdata11[21];
    char stringdata12[16];
    char stringdata13[19];
    char stringdata14[23];
    char stringdata15[17];
    char stringdata16[8];
    char stringdata17[16];
};
#define QT_MOC_LITERAL(ofs, len) \
    uint(sizeof(qt_meta_stringdata_VMConfigDialog_t::offsetsAndSizes) + ofs), len 
Q_CONSTINIT static const qt_meta_stringdata_VMConfigDialog_t qt_meta_stringdata_VMConfigDialog = {
    {
        QT_MOC_LITERAL(0, 14),  // "VMConfigDialog"
        QT_MOC_LITERAL(15, 6),  // "accept"
        QT_MOC_LITERAL(22, 0),  // ""
        QT_MOC_LITERAL(23, 21),  // "onMemorySliderChanged"
        QT_MOC_LITERAL(45, 5),  // "value"
        QT_MOC_LITERAL(51, 17),  // "onCPUCountChanged"
        QT_MOC_LITERAL(69, 11),  // "addHardDisk"
        QT_MOC_LITERAL(81, 14),  // "removeHardDisk"
        QT_MOC_LITERAL(96, 8),  // "addCDDVD"
        QT_MOC_LITERAL(105, 13),  // "selectBootISO"
        QT_MOC_LITERAL(119, 17),  // "addNetworkAdapter"
        QT_MOC_LITERAL(137, 20),  // "removeNetworkAdapter"
        QT_MOC_LITERAL(158, 15),  // "addSharedFolder"
        QT_MOC_LITERAL(174, 18),  // "removeSharedFolder"
        QT_MOC_LITERAL(193, 22),  // "selectSharedFolderPath"
        QT_MOC_LITERAL(216, 16),  // "enableEncryption"
        QT_MOC_LITERAL(233, 7),  // "enabled"
        QT_MOC_LITERAL(241, 15)   // "testAudioDevice"
    },
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
};
#undef QT_MOC_LITERAL
} // unnamed namespace

Q_CONSTINIT static const uint qt_meta_data_VMConfigDialog[] = {

 // content:
      10,       // revision
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
    qt_meta_stringdata_VMConfigDialog.offsetsAndSizes,
    qt_meta_data_VMConfigDialog,
    qt_static_metacall,
    nullptr,
    qt_incomplete_metaTypeArray<qt_meta_stringdata_VMConfigDialog_t,
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
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<VMConfigDialog *>(_o);
        (void)_t;
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
    if (!strcmp(_clname, qt_meta_stringdata_VMConfigDialog.stringdata0))
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
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 14)
            *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType();
        _id -= 14;
    }
    return _id;
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
