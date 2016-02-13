CONFIG += c++11
QT += opengl
QT += core
QT += gui
QT += widgets


INCLUDEPATH += "$$PWD/external/OpenCL/include"
LIBS += "-L$$PWD\external\OpenCL\lib\x86" -lOpenCL


# Copy CL Kernels to Output
PWD_WIN = $${PWD}
DESTDIR_WIN = $${OUT_PWD}
PWD_WIN ~= s,/,\\,g
DESTDIR_WIN ~= s,/,\\,g

win32 {
    copyfiles.commands = $$quote(cmd /c xcopy /S /Y /I $${PWD_WIN}\\render\\cl_sources $${DESTDIR_WIN}\\cl_sources)
}
!win32 {
#    copyfiles.commands = $$quote(cp $${PWD_WIN}\\render\\cl_sources $${DESTDIR_WIN}\\cl_sources)
}

QMAKE_EXTRA_TARGETS += copyfiles
POST_TARGETDEPS += copyfiles



# Copy Sample to Output
win32 {
    copysample.commands = $$quote(cmd /c xcopy /S /Y /I $${PWD_WIN}\\i3c_sample $${DESTDIR_WIN}\\)
}
!win32 {
    #copysample.commands = $$quote(cp $${PWD_WIN}\\i3c_sample $${DESTDIR_WIN}\\)
}

QMAKE_EXTRA_TARGETS += copysample
POST_TARGETDEPS += copysample

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
    tests/render/basic_gl_i3c_rendertest.cpp \
    render/i3c_transform.cpp \
    io/Images/image_v2.cpp \
    utils/i3c_converter.cpp \
    editing/i3c_cube.cpp \
    editing/i3c_editingcube.cpp \
    tests/editing/testediting.cpp

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
    tests/render/basic_gl_i3c_rendertest.h \
    render/i3c_transform.h \
    utils/logs.h \
    io/Images/image_v2.h \
    utils/i3c_converter.h \
    editing/i3c_cube.h \
    editing/i3c_editingcube.h \
    tests/editing/testediting.h

DISTFILES += \
    render/cl_sources/video.cl\
    render/cl_sources/clear.cl\
    render/cl_sources/rendering.cl

OTHER_FILES += \
    render/cl_sources/video.cl \
    render/cl_sources/rendering.cl \
    render/cl_sources/clear.cl




