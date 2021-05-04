#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "wm.h"

WindowManager *new_window_manager()
{
	WindowManager *wm = malloc(sizeof(WindowManager));;

	wm->display = XOpenDisplay(NULL);
	wm->root = DefaultRootWindow(wm->display);
	if (wm->display == NULL) return NULL;

	wm->run = run_window_manager;
	wm->close = close_window_manager;

	return wm;
}

static void run_window_manager(WindowManager *wm)
{
	Display *display = wm->display;
	Window *root = &wm->root;

	XSelectInput(display, *root, SubstructureRedirectMask | SubstructureNotifyMask);
	XSync(display, 0);

	// main loop
	while (1)
	{
		XEvent e;
		XNextEvent(display, &e);

		// dispatch events
		switch (e.type)
		{
			case CreateNotify:
				on_create_notify(&e.xcreatewindow);
				break;
			case DestroyNotify:
				on_destroy_notify(&e.xdestroywindow);
				break;
			case ReparentNotify:
				on_reparent_notify(&e.xreparent);
				break;
			case MapNotify:
				break;
			case UnmapNotify:
				break;
			case ConfigureNotify:
				break;
			case MapRequest:
				break;
			case ConfigureRequest:
				break;
			case ButtonPress:
				break;
			case ButtonRelease:
				break;
			case MotionNotify:
				// Skip any already pending motion events.
				while (XCheckTypedWindowEvent(display, e.xmotion.window, MotionNotify, &e)) {}
				break;
			case KeyPress:
				break;
			case KeyRelease:
				break;
			default:
				printf("Ignored event");
		}
	}
}

static void close_window_manager(WindowManager *wm)
{
	XCloseDisplay(wm->display);
	wm->display = NULL;
}

static void on_create_notify		(XCreateWindowEvent *e)		{}
static void on_destroy_notify		(XDestroyWindowEvent *e)	{}
static void on_reparent_notify	(XReparentEvent *e)			{}
static void on_map_notify			(XMapEvent *e)					{}
static void on_unmap_notify		(XUnmapEvent *e)				{}
static void on_configure_notify	(XConfigureEvent *e)			{}
static void on_map_request			(XMapRequestEvent *e)		{}
static void on_configure_request	(XConfigureRequestEvent *e){}
static void on_button_press		(XButtonEvent *e)				{}
static void on_button_release		(XButtonEvent *e)				{}
static void on_motion_notify		(XMotionEvent *e)				{}
static void on_key_press			(XKeyEvent *e)					{}
static void on_key_release			(XKeyEvent *e)					{}
