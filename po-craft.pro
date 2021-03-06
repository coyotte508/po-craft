DESTDIR = $$PWD/bin

INCLUDEPATH += include/

CONFIG(windows) {
    LIBS += -L"$$PWD/lib/windows/"
    LIBS += -lopengl32 -lglu32
}
!CONFIG(windows) {
    LIBS += -lGL -lGLU -lX11 -lm
}

CONFIG += c++11

#adds debug suffix to libraries when compiled
windows:CONFIG(debug, debug|release) {
    exesuffix=-d
} else {
    exesuffix=
}
unix:!mac {
    QMAKE_LFLAGS += "-Wl,-rpath,\'\$$ORIGIN'"
}


LIBS += -lsfml-graphics$$exesuffix -lsfml-window$$exesuffix -lsfml-system$$exesuffix

SOURCES += \
    src/main.cpp \
    src/terrain.cpp \
    src/vec3f.cpp \
    src/ball.cpp \
    src/game.cpp \
    src/camera.cpp \
    src/menu.cpp \
    src/controller.cpp \
    src/debug-val.cpp \
    src/engine.cpp \
    src/resources.cpp

HEADERS += \
    src/terrain.h \
    src/terrain-inl.h \
    src/vec3f.h \
    src/vec3f-inl.h \
    src/ball.h \
    src/game.h \
    src/dirs.h \
    src/mathconst.h \
    src/camera.h \
    src/macro.h \
    src/menu.h \
    src/controller.h \
    src/debug-val.h \
    src/engine.h \
    src/glu.h \
    src/gl.h \
    src/resources.h
