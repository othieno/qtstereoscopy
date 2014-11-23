include(../../../install/oculusvr.pri)
include(../benchmark.pri)

TARGET = oculusvr_benchmarks

HEADERS +=\
   qoculusrift_benchmark.h\
   qoculusriftstereorenderer_benchmark.h

SOURCES +=\
   qoculusrift_benchmark.cpp\
   qoculusriftstereorenderer_benchmark.cpp\
   oculusvr_benchmarks.cpp
