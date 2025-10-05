/****************************************************************************
** Meta object code from reading C++ file 'DiskManagerDialog.h'
**
** Created by: The Qt Meta Object Compiler version 68 (Qt 6.4.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include <memory>
#include "../../../src/ui/DiskManagerDialog.h"
#include <QtGui/qtextcursor.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'DiskManagerDialog.h' doesn't include <QObject>."
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
struct qt_meta_stringdata_DiskManagerDialog_t {
    uint offsetsAndSizes[22];
    char stringdata0[18];
    char stringdata1[13];
    char stringdata2[1];
    char stringdata3[11];
    char stringdata4[13];
    char stringdata5[13];
    char stringdata6[14];
    char stringdata7[14];
    char stringdata8[23];
    char stringdata9[13];
    char stringdata10[6];
};
#define QT_MOC_LITERAL(ofs, len) \
    uint(sizeof(qt_meta_stringdata_DiskManagerDialog_t::offsetsAndSizes) + ofs), len 
Q_CONSTINIT static const qt_meta_stringdata_DiskManagerDialog_t qt_meta_stringdata_DiskManagerDialog = {
    {
        QT_MOC_LITERAL(0, 17),  // "DiskManagerDialog"
        QT_MOC_LITERAL(18, 12),  // "onCreateDisk"
        QT_MOC_LITERAL(31, 0),  // ""
        QT_MOC_LITERAL(32, 10),  // "onCopyDisk"
        QT_MOC_LITERAL(43, 12),  // "onModifyDisk"
        QT_MOC_LITERAL(56, 12),  // "onRemoveDisk"
        QT_MOC_LITERAL(69, 13),  // "onReleaseDisk"
        QT_MOC_LITERAL(83, 13),  // "onRefreshList"
        QT_MOC_LITERAL(97, 22),  // "onDiskSelectionChanged"
        QT_MOC_LITERAL(120, 12),  // "onTabChanged"
        QT_MOC_LITERAL(133, 5)   // "index"
    },
    "DiskManagerDialog",
    "onCreateDisk",
    "",
    "onCopyDisk",
    "onModifyDisk",
    "onRemoveDisk",
    "onReleaseDisk",
    "onRefreshList",
    "onDiskSelectionChanged",
    "onTabChanged",
    "index"
};
#undef QT_MOC_LITERAL
} // unnamed namespace

Q_CONSTINIT static const uint qt_meta_data_DiskManagerDialog[] = {

 // content:
      10,       // revision
       0,       // classname
       0,    0, // classinfo
       8,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags, initial metatype offsets
       1,    0,   62,    2, 0x08,    1 /* Private */,
       3,    0,   63,    2, 0x08,    2 /* Private */,
       4,    0,   64,    2, 0x08,    3 /* Private */,
       5,    0,   65,    2, 0x08,    4 /* Private */,
       6,    0,   66,    2, 0x08,    5 /* Private */,
       7,    0,   67,    2, 0x08,    6 /* Private */,
       8,    0,   68,    2, 0x08,    7 /* Private */,
       9,    1,   69,    2, 0x08,    8 /* Private */,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Int,   10,

       0        // eod
};

Q_CONSTINIT const QMetaObject DiskManagerDialog::staticMetaObject = { {
    QMetaObject::SuperData::link<QDialog::staticMetaObject>(),
    qt_meta_stringdata_DiskManagerDialog.offsetsAndSizes,
    qt_meta_data_DiskManagerDialog,
    qt_static_metacall,
    nullptr,
    qt_incomplete_metaTypeArray<qt_meta_stringdata_DiskManagerDialog_t,
        // Q_OBJECT / Q_GADGET
        QtPrivate::TypeAndForceComplete<DiskManagerDialog, std::true_type>,
        // method 'onCreateDisk'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'onCopyDisk'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'onModifyDisk'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'onRemoveDisk'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'onReleaseDisk'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'onRefreshList'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'onDiskSelectionChanged'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'onTabChanged'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<int, std::false_type>
    >,
    nullptr
} };

void DiskManagerDialog::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<DiskManagerDialog *>(_o);
        (void)_t;
        switch (_id) {
        case 0: _t->onCreateDisk(); break;
        case 1: _t->onCopyDisk(); break;
        case 2: _t->onModifyDisk(); break;
        case 3: _t->onRemoveDisk(); break;
        case 4: _t->onReleaseDisk(); break;
        case 5: _t->onRefreshList(); break;
        case 6: _t->onDiskSelectionChanged(); break;
        case 7: _t->onTabChanged((*reinterpret_cast< std::add_pointer_t<int>>(_a[1]))); break;
        default: ;
        }
    }
}

const QMetaObject *DiskManagerDialog::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *DiskManagerDialog::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_DiskManagerDialog.stringdata0))
        return static_cast<void*>(this);
    return QDialog::qt_metacast(_clname);
}

int DiskManagerDialog::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QDialog::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 8)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 8;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 8)
            *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType();
        _id -= 8;
    }
    return _id;
}
namespace {
struct qt_meta_stringdata_CreateVirtualDiskDialog_t {
    uint offsetsAndSizes[10];
    char stringdata0[24];
    char stringdata1[13];
    char stringdata2[1];
    char stringdata3[16];
    char stringdata4[14];
};
#define QT_MOC_LITERAL(ofs, len) \
    uint(sizeof(qt_meta_stringdata_CreateVirtualDiskDialog_t::offsetsAndSizes) + ofs), len 
Q_CONSTINIT static const qt_meta_stringdata_CreateVirtualDiskDialog_t qt_meta_stringdata_CreateVirtualDiskDialog = {
    {
        QT_MOC_LITERAL(0, 23),  // "CreateVirtualDiskDialog"
        QT_MOC_LITERAL(24, 12),  // "onBrowsePath"
        QT_MOC_LITERAL(37, 0),  // ""
        QT_MOC_LITERAL(38, 15),  // "onFormatChanged"
        QT_MOC_LITERAL(54, 13)   // "onSizeChanged"
    },
    "CreateVirtualDiskDialog",
    "onBrowsePath",
    "",
    "onFormatChanged",
    "onSizeChanged"
};
#undef QT_MOC_LITERAL
} // unnamed namespace

Q_CONSTINIT static const uint qt_meta_data_CreateVirtualDiskDialog[] = {

 // content:
      10,       // revision
       0,       // classname
       0,    0, // classinfo
       3,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags, initial metatype offsets
       1,    0,   32,    2, 0x08,    1 /* Private */,
       3,    0,   33,    2, 0x08,    2 /* Private */,
       4,    0,   34,    2, 0x08,    3 /* Private */,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,

       0        // eod
};

Q_CONSTINIT const QMetaObject CreateVirtualDiskDialog::staticMetaObject = { {
    QMetaObject::SuperData::link<QDialog::staticMetaObject>(),
    qt_meta_stringdata_CreateVirtualDiskDialog.offsetsAndSizes,
    qt_meta_data_CreateVirtualDiskDialog,
    qt_static_metacall,
    nullptr,
    qt_incomplete_metaTypeArray<qt_meta_stringdata_CreateVirtualDiskDialog_t,
        // Q_OBJECT / Q_GADGET
        QtPrivate::TypeAndForceComplete<CreateVirtualDiskDialog, std::true_type>,
        // method 'onBrowsePath'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'onFormatChanged'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'onSizeChanged'
        QtPrivate::TypeAndForceComplete<void, std::false_type>
    >,
    nullptr
} };

void CreateVirtualDiskDialog::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<CreateVirtualDiskDialog *>(_o);
        (void)_t;
        switch (_id) {
        case 0: _t->onBrowsePath(); break;
        case 1: _t->onFormatChanged(); break;
        case 2: _t->onSizeChanged(); break;
        default: ;
        }
    }
    (void)_a;
}

const QMetaObject *CreateVirtualDiskDialog::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *CreateVirtualDiskDialog::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_CreateVirtualDiskDialog.stringdata0))
        return static_cast<void*>(this);
    return QDialog::qt_metacast(_clname);
}

int CreateVirtualDiskDialog::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QDialog::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 3)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 3;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 3)
            *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType();
        _id -= 3;
    }
    return _id;
}
namespace {
struct qt_meta_stringdata_DiskPropertiesDialog_t {
    uint offsetsAndSizes[2];
    char stringdata0[21];
};
#define QT_MOC_LITERAL(ofs, len) \
    uint(sizeof(qt_meta_stringdata_DiskPropertiesDialog_t::offsetsAndSizes) + ofs), len 
Q_CONSTINIT static const qt_meta_stringdata_DiskPropertiesDialog_t qt_meta_stringdata_DiskPropertiesDialog = {
    {
        QT_MOC_LITERAL(0, 20)   // "DiskPropertiesDialog"
    },
    "DiskPropertiesDialog"
};
#undef QT_MOC_LITERAL
} // unnamed namespace

Q_CONSTINIT static const uint qt_meta_data_DiskPropertiesDialog[] = {

 // content:
      10,       // revision
       0,       // classname
       0,    0, // classinfo
       0,    0, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

       0        // eod
};

Q_CONSTINIT const QMetaObject DiskPropertiesDialog::staticMetaObject = { {
    QMetaObject::SuperData::link<QDialog::staticMetaObject>(),
    qt_meta_stringdata_DiskPropertiesDialog.offsetsAndSizes,
    qt_meta_data_DiskPropertiesDialog,
    qt_static_metacall,
    nullptr,
    qt_incomplete_metaTypeArray<qt_meta_stringdata_DiskPropertiesDialog_t,
        // Q_OBJECT / Q_GADGET
        QtPrivate::TypeAndForceComplete<DiskPropertiesDialog, std::true_type>
    >,
    nullptr
} };

void DiskPropertiesDialog::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    (void)_o;
    (void)_id;
    (void)_c;
    (void)_a;
}

const QMetaObject *DiskPropertiesDialog::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *DiskPropertiesDialog::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_DiskPropertiesDialog.stringdata0))
        return static_cast<void*>(this);
    return QDialog::qt_metacast(_clname);
}

int DiskPropertiesDialog::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QDialog::qt_metacall(_c, _id, _a);
    return _id;
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
