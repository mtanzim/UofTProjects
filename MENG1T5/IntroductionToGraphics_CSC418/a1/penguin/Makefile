# Define name of target executable
PROGRAM       = main

# Define the default rule when make is run without arguments
all: $(PROGRAM)

# Define all C++ source files here
CPPSRCS       = main.cpp \
                GLState.cpp \
                GLWidget.cpp

# Define all Qt headers here
QTHEADERS     = MainWindow.h \
                GLWidget.h

# Dependencies on header files:
main.o:     include_gl.h MainWindow.h GLWidget.h GLTransformStack.h GLState.h \
            GLShape.h Matrix3x3.h Matrix4x4.h
GLWidget.o: include_gl.h GLWidget.h GLTransformStack.h GLState.h GLShape.h \
            Matrix3x3.h Matrix4x4.h
GLState.o:  include_gl.h GLTransformStack.h GLState.h GLShape.h Matrix3x3.h \
            Matrix4x4.h

# If Qt isn't installed on CDF machines then look in the course directory.
ifeq ($(wildcard /usr/include/qt4),)
    QTROOT    = /u/csc418h/winter/pub/root
else
    QTROOT    =
endif

##############################################################################

OSNAME        = $(shell uname -s)

# Define C++ compiler
CXX           = /usr/bin/g++

# Define C compiler options
CFLAGS        = -Wall -c -g

# Define C/C++ pre-processor options
CPPFLAGS      = -W -Wall -g -c -I./ -I$(QTROOT)/usr/include/qt4

# Define libraries to link against
ifeq ($(OSNAME),Linux)
    LIBS      = -L$(QTROOT)/usr/lib/x86_64-linux-gnu \
                -lQtGui -lQtOpenGL -lQtCore -lGLU -lGL -lm -ldl
endif
ifeq ($(OSNAME),Darwin)
    # TODO: Set MacOS libraries appropriately.
    LIBS      = -dylib_file /System/Library/Frameworks/OpenGL.framework/Versions/A/Libraries/libGL.dylib:/System/Library/Frameworks/OpenGL.framework/Versions/A/Libraries/libGL.dylib -L/opt/local/lib -lGLU -lGL -lm -ldl
endif

# Define the location of the destination directory for the executable file
DEST          = .

# Define linker
LINKER        = g++

MOCSRCS       = $(QTHEADERS:.h=_moc.cpp)

# Define all object files to be the same as CPPSRCS but with all the .cpp
# suffixes replaced with .o
OBJ           = $(CPPSRCS:.cpp=.o) $(MOCSRCS:.cpp=.o)

##############################################################################

# Define a rule to compiling all C++ files
%.o : %.cpp
	$(CXX) $(CPPFLAGS) $*.cpp

# Define a rule to create
%_moc.cpp: %.h
	$(QTROOT)/usr/share/qt4/bin/moc $*.h > $*_moc.cpp

# Define rule for creating executable
$(PROGRAM): $(OBJ)
	$(LINKER) $(OBJ) $(LIBS) -o $(PROGRAM)

# Define rule to clean up directory by removing all object, temp and core
# files along with the executable
clean :
	@rm -f $(PROGRAM) $(OBJ) $(MOCSRCS) *~ core

