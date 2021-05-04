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

	wm->on_create = on_create_notify;
	wm->on_destroy = on_destroy_notify;
	wm->on_reparent = on_reparent_notify;
	wm->on_map = on_map_notify;
	wm->on_unmap = on_unmap_notify;
	wm->on_configure = on_configure_notify;
	wm->on_map_request = on_map_request;
	wm->on_configure_request = on_configure_request;
	wm->on_button_press = on_button_press;
	wm->on_button_release = on_button_release;
	wm->on_motion = on_motion_notify;
	wm->on_key_press = on_key_press;
	wm->on_key_release = on_key_release;

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
				wm->on_create(&e.xcreatewindow);
				break;
			case DestroyNotify:
				wm->on_destroy(&e.xdestroywindow);
				break;
			case ReparentNotify:
				wm->on_reparent(&e.xreparent);
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
