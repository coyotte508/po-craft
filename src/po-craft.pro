DESTDIR = ../bin

CONFIG(windows) {
    LIBS += -L"../libs/windows/"
    LIBS += -lopengl32 -lglu32 -lglut32
}
!CONFIG(windows) {
    LIBS += -lglut -lGL -lGLU -lX11 -lm
}

SOURCES += \
    main.cpp \
    terrain.cpp \
    vec3f.cpp

HEADERS += \
    terrain.h \
    terrain-inl.h \
    vec3f.h \
    vec3f-inl.h
