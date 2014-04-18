QT += core widgets opengl

TARGET = 277starter3d
TEMPLATE = app

INCLUDEPATH += ./include

SOURCES += main.cpp \
    myglwidget.cpp \
    mymainwindow.cpp \
    camera.cpp \
    face.cpp \
    halfedge.cpp \
    myfacewidget.cpp \
    myhalfedgewidget.cpp \
    myvertexwidget.cpp \
    vertex.cpp

HEADERS += \
    myglwidget.h \
    mymainwindow.h \
    camera.h \
    face.h \
    halfedge.h \
    myfacewidget.h \
    myhalfedgewidget.h \
    myvertexwidget.h \
    vertex.h

FORMS += \
    mymainwindow.ui

RESOURCES += \
    shaders.qrc

OTHER_FILES += \
    diffuse.frag.glsl \
    wire.frag.glsl \
    diff.frag.glsl \
    diff.vert.glsl \
    wire.vert.glsl \
    277starter3d.pro.user
