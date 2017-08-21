/*
 *  Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.
 */

#ifndef _FABRICUI_UTIL_QT_SIGNALS_SLOTS_H
#define _FABRICUI_UTIL_QT_SIGNALS_SLOTS_H

#include <QObject>

#if QT_VERSION >= QT_VERSION_CHECK(5,0,0)

#define QOBJECT_CONNECT( objA, TA, ClassA, methodA, ArgsA, objB, TB, ClassB, methodB, ArgsB ) \
    QObject::connect( objA, & ClassA :: methodA, objB, & ClassB :: methodB )

// when several signals/slots have the same name (overloading), we must use this
// macro (const qualifiers must also be specified, if any)
#define QOBJECT_CONNECT_OVERLOADED( objA, TA, ClassA, methodA, ArgsA, constA, objB, TB, ClassB, methodB, ArgsB, constB ) \
    QObject::connect( \
      objA, static_cast<void( ClassA ::*) ArgsA constA >( & ClassA :: methodA ), \
      objB, static_cast<void( ClassB ::*) ArgsB constB >( & ClassB :: methodB ) \
    )

#else

#define QOBJECT_CONNECT( objA, TA, ClassA, methodA, ArgsA, objB, TB, ClassB, methodB, ArgsB ) \
    QObject::connect( objA, TA ( methodA ArgsA ), objB, TB ( methodB ArgsB ) )

#define QOBJECT_CONNECT_OVERLOADED( objA, TA, ClassA, methodA, ArgsA, constA, objB, TB, ClassB, methodB, ArgsB, constB ) \
    QOBJECT_CONNECT( objA, TA, ClassA, methodA, ArgsA, objB, TB, ClassB, methodB, ArgsB )

#endif

#endif //_FABRICUI_UTIL_QT_SIGNALS_SLOTS_H
