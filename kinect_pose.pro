QT       += core
TEMPLATE = app
CONFIG += console
CONFIG -= app_bundle
CONFIG -= qt

HEADERS += \
    Viewer.h \
    NiteSampleUtilities.h \
    predictor.h \
    actionmodel.h \
    gaussianofjoint.h \
    settings.h \
    factor.h \
    inference.h \
    cliquetree.h \
    utils.h \
    testpredict.h


SOURCES += main.cpp \
    Viewer.cpp \
    predictor.cpp \
    actionmodel.cpp \
    gaussianofjoint.cpp \
    factor.cpp \
    inference.cpp \
    cliquetree.cpp \
    utils.cpp \
    testpredict.cpp

INCLUDEPATH += $$PWD


# armadillo
unix|win32: LIBS += -L$$PWD/../../../devel/armadillo-4.000.3/examples/lib_win64/ -lblas_win64_MT -llapack_win64_MT

INCLUDEPATH += $$PWD/../../../devel/armadillo-4.000.3/include
DEPENDPATH += $$PWD/../../../devel/armadillo-4.000.3/include

# openni
unix|win32: LIBS += -L"$$PWD/../../../../../MProgram Files/OpenNI2/Lib/" -lOpenNI2

INCLUDEPATH += "$$PWD/../../../../../MProgram Files/OpenNI2/Include"
DEPENDPATH += "$$PWD/../../../../../MProgram Files/OpenNI2/Include"

# Nite
unix|win32: LIBS += -L"$$PWD/../../../../../MProgram Files/PrimeSense/NiTE2/Lib/" -lNiTE2

INCLUDEPATH += "$$PWD/../../../../../MProgram Files/PrimeSense/NiTE2/Include"
DEPENDPATH += "$$PWD/../../../../../MProgram Files/PrimeSense/NiTE2/Include"

# opengl include
unix|win32: LIBS += -lopengl32
INCLUDEPATH += $$PWD/include

# glut lib
unix|win32: LIBS += -L"$$PWD/lib" -lglut64




