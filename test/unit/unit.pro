TEMPLATE = app
TARGET = testqtstereoscopy

CONFIG += c++11 qtestlib

DESTDIR = build
MOC_DIR = $$DESTDIR
OBJECTS_DIR = $$DESTDIR

INCLUDEPATH += .

HEADERS +=\
   testqoculusrift.h\
   testqoculusriftstereorenderer.h

SOURCES +=\
   testqoculusrift.cpp\
   testqoculusriftstereorenderer.cpp\
   testqtstereoscopy.cpp
