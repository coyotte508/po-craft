DESTDIR = ../bin
LIBS += -lopengl32 -lglu32 -L"libs/windows/" -lglut32

SOURCES += \
    main.cpp \
    terrain.cpp \
    vec3f.cpp

HEADERS += \
    terrain.h \
    terrain-inl.h \
    vec3f.h \
    vec3f-inl.h

RESOURCES += \
    ../res/res.qrc

OTHER_FILES += \
    ../res/heightmap.bmp
