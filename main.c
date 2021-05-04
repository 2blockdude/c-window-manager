#include <stdio.h>
#include <stdlib.h>
#include <X11/Xlib.h>

// global stuff
Display *display;		// handle Xlib display struct
Window root;			// handle to root window

void windowManagerCreate();
void windowManagerDestroy();
void windowManagerRun();

int main()
{

	windowManagerCreate();

	windowManagerRun();

	windowManagerDestroy();

	return 0;
}

void windowManagerCreate()
{
	// if it could not connect to x server exit with 1
	if (!(display = XOpenDisplay(NULL))) exit(1);
	root = DefaultRootWindow(display);
}

void windowManagerDestroy()
{
	XCloseDisplay(display);
}

void windowManagerRun()
{
	// note: error code for an already running window manager is BadAccess provided by XErrorEvent I think
	XSelectInput(display, root, SubstructureRedirectMask | SubstructureNotifyMask);
	//XSync(display, 0);

	// main loop
	while (1)
	{
		XEvent e;
		XNextEvent(display, &e);

		// dispatch events
		switch (e.type)
		{
			case CreateNotify:
				e.xcreatewindow;
				break;
			case DestroyNotify:
				e.xdestroywindow;
				break;
			case ReparentNotify:
				e.xreparent;
				break;
			default:
				printf("Ignored event");
		}
	}
}
