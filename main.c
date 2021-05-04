#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <X11/Xlib.h>

void WindowManagerRun(Display *display, Window *root);

void OnCreateNotify(XCreateWindowEvent *e);
void OnDestroyNotify(XDestroyWindowEvent *e);
void OnReparentNotify(XReparentEvent *e);

int main()
{
	Display *display;		// handle Xlib display struct
	Window root;			// handle to root window

	// init wm
	display = XOpenDisplay(NULL);
	root = DefaultRootWindow(display);
	if (display == NULL) return 0;

	WindowManagerRun(display, &root);
	XCloseDisplay(display);

	return 0;
}

void WindowManagerRun(Display *display, Window *root)
{
	// note: error code for an already running window manager is BadAccess provided by XErrorEvent I think
	XSelectInput(display, *root, SubstructureRedirectMask | SubstructureNotifyMask);
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
				OnCreateNotify(&e.xcreatewindow);
				break;
			case DestroyNotify:
				OnDestroyNotify(&e.xdestroywindow);
				break;
			case ReparentNotify:
				OnReparentNotify(&e.xreparent);
				break;
			default:
				printf("Ignored event");
		}
	}
}

void OnCreateNotify(XCreateWindowEvent *e) {}
void OnDestroyNotify(XDestroyWindowEvent *e) {}
void OnReparentNotify(XReparentEvent *e) {}
