PUBLIC_HEADERS += \
	$$PWD/qtcoroutine.h \
	$$PWD/qtcoawaitables.h \
	$$PWD/qtcoqueue.h

HEADERS += $$PUBLIC_HEADERS \
	$$PWD/qtcoroutine_p.h

SOURCES += \
	$$PWD/qtcoroutine.cpp \
	$$PWD/qtcoawaitables.cpp

win32: SOURCES += $$PWD/qtcoroutine_win.cpp
else: SOURCES += $$PWD/qtcoroutine_unix.cpp

INCLUDEPATH += $$PWD
