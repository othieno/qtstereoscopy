# Try to adhere to the Qt coding conventions for public header files.
QMAKE_CXXFLAGS += -Wall -Wextra -Werror -fno-rtti -Wfloat-equal\
                  -Woverloaded-virtual -Wundef
                 #-Wold-style-cast -Wshadow

# Common configurations.
DESTDIR = build
MOC_DIR = $$DESTDIR
OBJECTS_DIR = $$DESTDIR
