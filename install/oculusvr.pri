#
# The MIT License (MIT)
#
# Copyright (c) 2014 Jeremy Othieno.
#
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:
#
# The above copyright notice and this permission notice shall be included in
# all copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
# THE SOFTWARE.
#
# Common build configurations.
include("$$PWD/common.pri")

# Make sure the OVRSDK environment variable is defined and contains a path to an existing directory.
OVRSDK = "$$(OVRSDK)"
isEmpty(OVRSDK):error(The OVRSDK environment variable is not defined.)
!exists($$OVRSDK):error("The OVRSDK environment variable is defined, but the directory \'$$OVRSDK\' does not exist.")

# If the directory exists, make sure it contains a valid Oculus SDK installation. The following
# subdirectories are listed in chapter 4.3 of the Oculus Developer Guide.
LIBOVR = $$OVRSDK/LibOVR
LIBOVR_INCLUDE = $$LIBOVR/Include
LIBOVR_LIB = $$LIBOVR/Lib
LIBOVR_SRC = $$LIBOVR/Src

# Make sure the directory contains a valid Oculus SDK installation.
exists($$LIBOVR):exists($$LIBOVR_INCLUDE):exists($$LIBOVR_LIB):exists($$LIBOVR_SRC){

# Use gui-private module to gain low-level features that allow us to access the platform's native
# display, as required by the Oculus SDK.
QT += gui-private

# Oculus SDK build configurations.
INCLUDEPATH += $$LIBOVR_INCLUDE $$LIBOVR_SRC
LIBS += -lovr

# GNU/Linux build configuration.
unix:!macx {
                                   LIBS += -lX11 -lXinerama -lXrandr -ludev
   eval(QMAKE_HOST.arch = x86_64): LIBS += -L$$LIBOVR_LIB/Linux/Release/x86_64
   else:                           LIBS += -L$$LIBOVR_LIB/Linux/Release/i386
}

# Mac OS build configuration.
macx {
}

# Windows build configuration.
win32 {
}

# Add header and source files specific to the Oculus Rift implementation.
INCLUDEPATH += "$$QTSTEREOSCOPY_SRC/oculusvr/"

HEADERS +=\
   "$$QTSTEREOSCOPY_SRC/oculusvr/qoculusrift.h"\
   "$$QTSTEREOSCOPY_SRC/oculusvr/qoculusriftstereorenderer.h"

SOURCES +=\
   "$$QTSTEREOSCOPY_SRC/oculusvr/qoculusrift.cpp"\
   "$$QTSTEREOSCOPY_SRC/oculusvr/qoculusriftstereorenderer.cpp"
}
else:error("The OVRSDK environment variable is defined, but the directory \'$$OVRSDK\' does not contain a valid Oculus SDK installation.")
