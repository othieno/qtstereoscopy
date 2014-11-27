include(../../../install/oculusvr.pri)
include(../unit.pri)

TARGET = oculusvr_testsuite

HEADERS +=\
   qoculusrift_test.h\
   qoculusriftrenderer_test.h

SOURCES +=\
   qoculusrift_test.cpp\
   qoculusriftrenderer_test.cpp\
   oculusvr_testsuite.cpp
