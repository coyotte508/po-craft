DESTDIR = ../bin

CONFIG(windows) {
    INCLUDEPATH += ../include/windows/
    LIBS += -L"../lib/windows/"
    LIBS += -lopengl32 -lglu32 -lglut32 -lSDL -lSDLmain
}
!CONFIG(windows) {
    LIBS += -lglut -lGL -lGLU -lX11 -lm -lSDL -lSDL_ttf
}

LIBS += -lsfml-graphics -lsfml-window -lsfml-system

DEFINES += _WCHAR_T_DEFINED GLUT_NO_WARNING_DISABLE GLUT_BUILDING_LIB
DEFINES -= QT_NEEDS_QMAIN
SOURCES += \
    main.cpp \
    terrain.cpp \
    vec3f.cpp \
    ball.cpp \
    text3d.cpp \
    game.cpp \
    camera.cpp \
    menu.cpp \
    controller.cpp \
    debug-val.cpp \
    engine.cpp

HEADERS += \
    terrain.h \
    terrain-inl.h \
    vec3f.h \
    vec3f-inl.h \
    ball.h \
    text3d.h \
    game.h \
    dirs.h \
    mathconst.h \
    camera.h \
    macro.h \
    menu.h \
    controller.h \
    debug-val.h \
    engine.h
