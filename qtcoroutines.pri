CONFIG += c++14

mac: DEFINES += _XOPEN_SOURCE

PUBLIC_HEADERS += \
	$$PWD/qtcoroutine_global.h \
	$$PWD/qtcoroutine.h \
	$$PWD/qtcoawaitables.h \
	$$PWD/qtcoqueue.h \
	$$PWD/qtcoiterator.h \
	$$PWD/qtcoconcurrent.h

HEADERS += $$PUBLIC_HEADERS \
	$$PWD/qtcoroutine_p.h

SOURCES += \
	$$PWD/qtcoroutine.cpp \
	$$PWD/qtcoawaitables.cpp \
	$$PWD/qtcoiterator.cpp

win32: SOURCES += $$PWD/qtcoroutine_win.cpp
else: SOURCES += $$PWD/qtcoroutine_unix.cpp

INCLUDEPATH += $$PWD

qtcoroutines_exported: DEFINES += QTCOROUTINE_EXPORTED QTCOROUTINE_LIBRARY
