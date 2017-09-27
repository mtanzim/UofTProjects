/*************************************************************************
    CSC418/2504, Winter 20l5
    Assignment 1

    Instructions:
        Please read the assignment page to determine exactly what needs to be
        implemented.  Then read over this file and the other .cpp and .h files
        and become acquainted with the design of this program.

        Add source code where it appears appropriate. In particular, see lines
        marked 'TODO'.  Most of what you will need to change is in the GLWidget
	class.

        You should not need to change the overall structure of the program.
        However it should be clear what your changes do, and you should use
        sufficient comments to explain your code.

        Note that the following packages need to be installed on Linux for
        this program to properly compile and link:
        - libqt4-dev
        - libqt4-opengl-dev

**************************************************************************/

// The include_gl.h header includes all the necessary OpenGL headers that we'll
// need.
#include "include_gl.h"

#include <QtGui/QApplication>

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cmath>
#include <vector>

// Include the classes specific to our application:
#include "MainWindow.h"
#include "GLWidget.h"

int main(int argc, char** argv)
{
    int width = 600;
    int height = 900;

    // See if the user passed command-line arguments for the window size.
    if (argc != 3)
    {
	std::cout << "Usage: demo [width] [height]\n";
	std::cout << "Using " << width << "x" << height
	    << " window by default...\n";
    }
    else
    {
	width = atoi(argv[1]);
	height = atoi(argv[2]);
    }

    // Create the Qt application and the main window, and show the main window.
    QApplication app(argc, argv);
    MainWindow main_window;
    main_window.resize(QSize(width, height));
    main_window.show();
    return app.exec();
}

