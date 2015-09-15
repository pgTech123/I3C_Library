LIBS += "-L/usr/lib/x86_64-linux-gnu/" -lSDL2main -lSDL2    #SDL
LIBS += "-L/usr/lib/x86_64-linux-gnu/" -lGL -lglut -lGLU

SOURCES += \
    main.cpp \
    io/Video/videofile.cpp \
    io/i3c_write.cpp \
    io/i3c_read.cpp \
    i3c.cpp \
    io/Images/image_v1.cpp \
    io/Images/imagefile.cpp \
    utils/binary_func.cpp \
    render/gl_i3c_element.cpp \
    render/gl_i3c_scene.cpp \
    tests/tests.cpp \
    tests/render/gl_window.cpp \
    tests/render/basic_gl_i3c_rendertest.cpp

HEADERS += \
    i3c.h \
    io/Images/imagefile.h \
    io/Video/videofile.h \
    io/i3c_write.h \
    io/i3c_read.h \
    utils/binary_func.h \
    io/Images/image_v1.h \
    render/gl_i3c_element.h \
    render/gl_i3c_scene.h \
    tests/tests.h \
    tests/render/gl_window.h \
    tests/render/basic_gl_i3c_rendertest.h
