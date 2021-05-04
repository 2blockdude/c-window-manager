#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "wm.h"

WindowManager *new_window_manager()
{
	WindowManager *wm = malloc(sizeof(WindowManager));;

	wm->display = XOpenDisplay(NULL);
	if (wm->display == NULL) return NULL;
	wm->root = DefaultRootWindow(wm->display);
	wm->run = run_window_manager;
	wm->close = close_window_manager;

	return wm;
}

static void run_window_manager(WindowManager *self)
{
	XSelectInput(self->display, self->root, SubstructureRedirectMask | SubstructureNotifyMask);
	XSync(self->display, 0);

	// main loop
	while (1)
	{
		XEvent e;
		XNextEvent(self->display, &e);

		// dispatch events
		switch (e.type)
		{
			case CreateNotify:
				on_create_notify(self, &e.xcreatewindow);
				break;
			case DestroyNotify:
				on_destroy_notify(self, &e.xdestroywindow);
				break;
			case ReparentNotify:
				on_reparent_notify(self, &e.xreparent);
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
				on_configure_request(self, &e.xconfigurerequest);
				break;
			case ButtonPress:
				break;
			case ButtonRelease:
				break;
			case MotionNotify:
				// Skip any already pending motion events.
				while (XCheckTypedWindowEvent(self->display, e.xmotion.window, MotionNotify, &e)) {}
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

static void close_window_manager(WindowManager *self)
{
	XCloseDisplay(self->display);
	self->display = NULL;
}

static void on_create_notify(WindowManager *self, XCreateWindowEvent *e) {}
static void on_destroy_notify(WindowManager *self, XDestroyWindowEvent *e) {}
static void on_reparent_notify(WindowManager *self, XReparentEvent *e) {}
static void on_map_notify(WindowManager *self, XMapEvent *e) {}
static void on_unmap_notify(WindowManager *self, XUnmapEvent *e) {}
static void on_configure_notify(WindowManager *self, XConfigureEvent *e) {}

static void on_map_request(WindowManager *self, XMapRequestEvent *e)
{
	// note: frame is a custom function
	// frame window with window decorations.
	// frame e->window to self
	frame(self, e->window);
	// map window i guess to main display
	XMapWindow(self->display, e->window);
}

static void frame(WindowManager *self, Window w)
{
	XWindowAttributes windowAttributes;
	XGetWindowAttributes(self->display, w, &windowAttributes);
}

static void on_configure_request(WindowManager *self, XConfigureRequestEvent *e)
{
	XWindowChanges changes;

	// copy stuff from e to changes for some reason
	changes.x = e->x;
	changes.y = e->y;
	changes.width = e->width;
	changes.height = e->height;
	changes.border_width = e->border_width;
	changes.sibling = e->above;
	changes.stack_mode = e->detail;

	// grant request
	XConfigureWindow(self->display, e->window, e->value_mask, &changes);

	printf("resized");
}

static void on_button_press		(WindowManager *self, XButtonEvent *e)				{}
static void on_button_release		(WindowManager *self, XButtonEvent *e)				{}
static void on_motion_notify		(WindowManager *self, XMotionEvent *e)				{}
static void on_key_press			(WindowManager *self, XKeyEvent *e)					{}
static void on_key_release			(WindowManager *self, XKeyEvent *e)					{}
