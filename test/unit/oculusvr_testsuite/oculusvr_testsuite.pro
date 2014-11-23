include(../../../install/oculusvr.pri)
include(../unit.pri)

TARGET = oculusvr_testsuite

HEADERS +=\
   qoculusrift_test.h\
   qoculusriftstereorenderer_test.h

SOURCES +=\
   qoculusrift_test.cpp\
   qoculusriftstereorenderer_test.cpp\
   oculusvr_testsuite.cpp
