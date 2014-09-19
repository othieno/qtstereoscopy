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
QTSTEREOSCOPY_BASEDIR = "$$PWD/.."
QTSTEREOSCOPY_SRC = "$$QTSTEREOSCOPY_BASEDIR/src"

# Add QtStereoscopy's source directory to the include path.
INCLUDEPATH += "$$QTSTEREOSCOPY_SRC"

# Add C++11 support.
CONFIG += c++11

# Add QtStereoscopy's general header and source files.
HEADERS +=\
   "$$QTSTEREOSCOPY_SRC/qabstractstereodisplay.h"\
   "$$QTSTEREOSCOPY_SRC/qabstractstereorenderer.h"\
   "$$QTSTEREOSCOPY_SRC/qstereocamera.h"\
   "$$QTSTEREOSCOPY_SRC/qstereowindow.h"

SOURCES +=\
   "$$QTSTEREOSCOPY_SRC/qabstractstereodisplay.cpp"\
   "$$QTSTEREOSCOPY_SRC/qabstractstereorenderer.cpp"\
   "$$QTSTEREOSCOPY_SRC/qstereocamera.cpp"
