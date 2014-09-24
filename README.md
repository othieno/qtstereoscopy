#QtStereoscopy

[![Build Status][travis_badge]][travis]
[![Stories in Ready][waffle_badge]][waffle]
[![Code Climate][codeclimate_badge]][codeclimate]
[![Code Coverage][coveralls_badge]][coveralls]

The QtStereoscopy module aims to provide support for stereoscopic  display devices
to Qt applications. The full reference documentation, which includes a much more
detailed description, can be found [here][doc]


Requirements
------------

QtStereoscopy extends functionality added to the revised [QtGui][qtgui] module introduced in Qt5.
As such, prior versions of the Qt library are not supported. While the new QtGui module
provides a modern way of using the OpenGL API, it is still relatively new (as of September 2014)
and a few features that exist in prior versions of Qt are still not fully implemented.
One such feature is a set of API calls that allows for access to the native display
device (required by the Oculus SDK, for example). The current workaround is to use a part
of the [Qt Platform Abstraction][qpa] (QPA) which is currently strongly-coupled to the
version of Qt it is built against. Ergo, this is not a portable solution but will very likely
become one as the QPA API matures.

Additionally, the C++11 standard is heavily used. It stands to reason that a compiler that
implements the said standard, at least, is necessary.


Installing QtStereoscopy
------------------------

QtStereoscopy's build process is still not tied to Qt which means the usual way a Qt module
is integrated into a project will not work. Instead, it relies on a set of project include
files (*.pri), each of which specifies the build configuration for a specific set of
stereoscopic display devices.

Start by cloning the qtstereoscopy project then include one or more project include files (found in
the `install` directory) to a project file (*.pro). For example, to add support for Oculus VR devices,
prepend `include(</path/to/qtstereoscopy>/install/oculusvr.pri)` where `</path/to/qtstereoscopy>` is
qtstereoscopy's location.

Once this is done, simply include the header files (see [documentation][doc]) into your project,
like you would a regular Qt module; the build process will take care of the rest.

Check out the examples to see QtStereoscopy in action.

[doc]:https://supranove.github.io/qtstereoscopy
[qpa]:http://qt-project.org/wiki/Qt-Platform-Abstraction
[qtgui]:http://qt-project.org/doc/qt-5/qtgui-index.html
[travis]:https://travis-ci.org/supranove/qtstereoscopy
[travis_badge]:https://travis-ci.org/supranove/qtstereoscopy.svg?branch=master
[waffle]:http://waffle.io/supranove/qtstereoscopy
[waffle_badge]:https://badge.waffle.io/supranove/qtstereoscopy.png?label=ready&title=Ready
[coveralls]:https://coveralls.io/r/supranove/qtstereoscopy
[coveralls_badge]:https://coveralls.io/repos/supranove/qtstereoscopy/badge.png
[codeclimate]:https://codeclimate.com/github/supranove/qtstereoscopy
[codeclimate_badge]:https://codeclimate.com/github/supranove/qtstereoscopy/badges/gpa.svg
