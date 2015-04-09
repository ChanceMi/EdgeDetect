/****************************************************************************
** Meta object code from reading C++ file 'steeldetect.h'
**
** Created by: The Qt Meta Object Compiler version 63 (Qt 4.8.5)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../steeldetect.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'steeldetect.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.5. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_SteelDetect[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
       7,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: signature, parameters, type, tag, flags
      13,   12,   12,   12, 0x0a,
      29,   12,   12,   12, 0x0a,
      43,   12,   12,   12, 0x0a,
      57,   12,   12,   12, 0x0a,
      70,   12,   12,   12, 0x0a,
      84,   12,   12,   12, 0x0a,
      96,   12,   12,   12, 0x0a,

       0        // eod
};

static const char qt_meta_stringdata_SteelDetect[] = {
    "SteelDetect\0\0slotOpenVideo()\0slotOpenImg()\0"
    "slotSaveImg()\0slotDetect()\0slotOpenCam()\0"
    "nextFrame()\0slotDetectGpu()\0"
};

void SteelDetect::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        SteelDetect *_t = static_cast<SteelDetect *>(_o);
        switch (_id) {
        case 0: _t->slotOpenVideo(); break;
        case 1: _t->slotOpenImg(); break;
        case 2: _t->slotSaveImg(); break;
        case 3: _t->slotDetect(); break;
        case 4: _t->slotOpenCam(); break;
        case 5: _t->nextFrame(); break;
        case 6: _t->slotDetectGpu(); break;
        default: ;
        }
    }
    Q_UNUSED(_a);
}

const QMetaObjectExtraData SteelDetect::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject SteelDetect::staticMetaObject = {
    { &QMainWindow::staticMetaObject, qt_meta_stringdata_SteelDetect,
      qt_meta_data_SteelDetect, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &SteelDetect::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *SteelDetect::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *SteelDetect::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_SteelDetect))
        return static_cast<void*>(const_cast< SteelDetect*>(this));
    return QMainWindow::qt_metacast(_clname);
}

int SteelDetect::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QMainWindow::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 7)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 7;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
