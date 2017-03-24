SOURCES += \
    main.cpp \
    glwidget.cpp

HEADERS += \
    glwidget.h

QT += opengl widgets

CONFIG += debug

win32{
    LIBS += -lglu32
    LIBS += -lopengl32
}

unix{
    LIBS += -lGLU
}
