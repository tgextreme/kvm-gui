/****************************************************************************
** Meta object code from reading C++ file 'PreferencesDialog.h'
**
** Created by: The Qt Meta Object Compiler version 68 (Qt 6.4.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include <memory>
#include "../../../src/ui/PreferencesDialog.h"
#include <QtGui/qtextcursor.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'PreferencesDialog.h' doesn't include <QObject>."
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
struct qt_meta_stringdata_PreferencesDialog_t {
    uint offsetsAndSizes[24];
    char stringdata0[18];
    char stringdata1[22];
    char stringdata2[1];
    char stringdata3[21];
    char stringdata4[18];
    char stringdata5[21];
    char stringdata6[14];
    char stringdata7[17];
    char stringdata8[15];
    char stringdata9[19];
    char stringdata10[22];
    char stringdata11[20];
};
#define QT_MOC_LITERAL(ofs, len) \
    uint(sizeof(qt_meta_stringdata_PreferencesDialog_t::offsetsAndSizes) + ofs), len 
Q_CONSTINIT static const qt_meta_stringdata_PreferencesDialog_t qt_meta_stringdata_PreferencesDialog = {
    {
        QT_MOC_LITERAL(0, 17),  // "PreferencesDialog"
        QT_MOC_LITERAL(18, 21),  // "selectDefaultVMFolder"
        QT_MOC_LITERAL(40, 0),  // ""
        QT_MOC_LITERAL(41, 20),  // "selectSnapshotFolder"
        QT_MOC_LITERAL(62, 17),  // "onLanguageChanged"
        QT_MOC_LITERAL(80, 20),  // "onUpdateCheckChanged"
        QT_MOC_LITERAL(101, 13),  // "addNATNetwork"
        QT_MOC_LITERAL(115, 16),  // "removeNATNetwork"
        QT_MOC_LITERAL(132, 14),  // "editNATNetwork"
        QT_MOC_LITERAL(147, 18),  // "addHostOnlyNetwork"
        QT_MOC_LITERAL(166, 21),  // "removeHostOnlyNetwork"
        QT_MOC_LITERAL(188, 19)   // "editHostOnlyNetwork"
    },
    "PreferencesDialog",
    "selectDefaultVMFolder",
    "",
    "selectSnapshotFolder",
    "onLanguageChanged",
    "onUpdateCheckChanged",
    "addNATNetwork",
    "removeNATNetwork",
    "editNATNetwork",
    "addHostOnlyNetwork",
    "removeHostOnlyNetwork",
    "editHostOnlyNetwork"
};
#undef QT_MOC_LITERAL
} // unnamed namespace

Q_CONSTINIT static const uint qt_meta_data_PreferencesDialog[] = {

 // content:
      10,       // revision
       0,       // classname
       0,    0, // classinfo
      10,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags, initial metatype offsets
       1,    0,   74,    2, 0x08,    1 /* Private */,
       3,    0,   75,    2, 0x08,    2 /* Private */,
       4,    0,   76,    2, 0x08,    3 /* Private */,
       5,    0,   77,    2, 0x08,    4 /* Private */,
       6,    0,   78,    2, 0x08,    5 /* Private */,
       7,    0,   79,    2, 0x08,    6 /* Private */,
       8,    0,   80,    2, 0x08,    7 /* Private */,
       9,    0,   81,    2, 0x08,    8 /* Private */,
      10,    0,   82,    2, 0x08,    9 /* Private */,
      11,    0,   83,    2, 0x08,   10 /* Private */,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,

       0        // eod
};

Q_CONSTINIT const QMetaObject PreferencesDialog::staticMetaObject = { {
    QMetaObject::SuperData::link<QDialog::staticMetaObject>(),
    qt_meta_stringdata_PreferencesDialog.offsetsAndSizes,
    qt_meta_data_PreferencesDialog,
    qt_static_metacall,
    nullptr,
    qt_incomplete_metaTypeArray<qt_meta_stringdata_PreferencesDialog_t,
        // Q_OBJECT / Q_GADGET
        QtPrivate::TypeAndForceComplete<PreferencesDialog, std::true_type>,
        // method 'selectDefaultVMFolder'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'selectSnapshotFolder'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'onLanguageChanged'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'onUpdateCheckChanged'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'addNATNetwork'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'removeNATNetwork'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'editNATNetwork'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'addHostOnlyNetwork'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'removeHostOnlyNetwork'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'editHostOnlyNetwork'
        QtPrivate::TypeAndForceComplete<void, std::false_type>
    >,
    nullptr
} };

void PreferencesDialog::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<PreferencesDialog *>(_o);
        (void)_t;
        switch (_id) {
        case 0: _t->selectDefaultVMFolder(); break;
        case 1: _t->selectSnapshotFolder(); break;
        case 2: _t->onLanguageChanged(); break;
        case 3: _t->onUpdateCheckChanged(); break;
        case 4: _t->addNATNetwork(); break;
        case 5: _t->removeNATNetwork(); break;
        case 6: _t->editNATNetwork(); break;
        case 7: _t->addHostOnlyNetwork(); break;
        case 8: _t->removeHostOnlyNetwork(); break;
        case 9: _t->editHostOnlyNetwork(); break;
        default: ;
        }
    }
    (void)_a;
}

const QMetaObject *PreferencesDialog::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *PreferencesDialog::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_PreferencesDialog.stringdata0))
        return static_cast<void*>(this);
    return QDialog::qt_metacast(_clname);
}

int PreferencesDialog::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QDialog::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 10)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 10;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 10)
            *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType();
        _id -= 10;
    }
    return _id;
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
