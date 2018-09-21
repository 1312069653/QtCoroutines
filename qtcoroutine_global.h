#ifndef QTCOROUTINE_GLOBAL_H
#define QTCOROUTINE_GLOBAL_H

#include <QtCore/qglobal.h>

#ifdef QTCOROUTINE_EXPORTED
#  if defined(QTCOROUTINE_LIBRARY)
#    define QTCOROUTINE_EXPORT Q_DECL_EXPORT
#  else
#    define QTCOROUTINE_EXPORT Q_DECL_IMPORT
#  endif
#else
#  define QTCOROUTINE_EXPORT
#endif

#endif // QTCOROUTINE_GLOBAL_H
