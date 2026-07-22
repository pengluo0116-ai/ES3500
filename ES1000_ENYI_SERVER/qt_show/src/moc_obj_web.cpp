/****************************************************************************
** Meta object code from reading C++ file 'obj_web.h'
**
** Created by: The Qt Meta Object Compiler version 63 (Qt 4.8.6)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "obj_web.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'obj_web.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.6. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_obj_web[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
       9,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: signature, parameters, type, tag, flags
       8,   29,   29,   29, 0x0a,
      30,   29,   45,   29, 0x0a,
      53,   29,   45,   29, 0x0a,
      68,   29,   45,   29, 0x0a,
      91,   29,  104,   29, 0x0a,
     108,   29,  104,   29, 0x0a,
     122,   29,  135,   29, 0x0a,
     150,   29,  135,   29, 0x0a,
     166,   29,  135,   29, 0x0a,

       0        // eod
};

static const char qt_meta_stringdata_obj_web[] = {
    "obj_web\0web_window_connect()\0\0"
    "get_realData()\0QString\0get_liftReal()\0"
    "checkUpdata_liftInfo()\0open_radio()\0"
    "int\0close_radio()\0get_devAll()\0"
    "unsigned short\0get_devOnline()\0"
    "get_devAlarm()\0"
};

void obj_web::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        obj_web *_t = static_cast<obj_web *>(_o);
        switch (_id) {
        case 0: _t->web_window_connect(); break;
        case 1: { QString _r = _t->get_realData();
            if (_a[0]) *reinterpret_cast< QString*>(_a[0]) = _r; }  break;
        case 2: { QString _r = _t->get_liftReal();
            if (_a[0]) *reinterpret_cast< QString*>(_a[0]) = _r; }  break;
        case 3: { QString _r = _t->checkUpdata_liftInfo();
            if (_a[0]) *reinterpret_cast< QString*>(_a[0]) = _r; }  break;
        case 4: { int _r = _t->open_radio();
            if (_a[0]) *reinterpret_cast< int*>(_a[0]) = _r; }  break;
        case 5: { int _r = _t->close_radio();
            if (_a[0]) *reinterpret_cast< int*>(_a[0]) = _r; }  break;
        case 6: { unsigned short _r = _t->get_devAll();
            if (_a[0]) *reinterpret_cast< unsigned short*>(_a[0]) = _r; }  break;
        case 7: { unsigned short _r = _t->get_devOnline();
            if (_a[0]) *reinterpret_cast< unsigned short*>(_a[0]) = _r; }  break;
        case 8: { unsigned short _r = _t->get_devAlarm();
            if (_a[0]) *reinterpret_cast< unsigned short*>(_a[0]) = _r; }  break;
        default: ;
        }
    }
}

const QMetaObjectExtraData obj_web::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject obj_web::staticMetaObject = {
    { &QWebView::staticMetaObject, qt_meta_stringdata_obj_web,
      qt_meta_data_obj_web, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &obj_web::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *obj_web::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *obj_web::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_obj_web))
        return static_cast<void*>(const_cast< obj_web*>(this));
    return QWebView::qt_metacast(_clname);
}

int obj_web::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWebView::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 9)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 9;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
