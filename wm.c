#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>

#include "wm.h"

WindowManager *new_window_manager()
{
	WindowManager *wm = malloc(sizeof(WindowManager));;

	wm->display = XOpenDisplay(NULL);
	if (wm->display == NULL) return NULL;
	wm->root = DefaultRootWindow(wm->display);
	wm->numClients = 0;

	wm->run = run_window_manager;
	wm->close = close_window_manager;

	return wm;
}

static void run_window_manager(WindowManager *self)
{
	// note: for SubstructreRedirectMask | SubstructureNotifyMask I think we are
	// combining the two masks for some reason.
	// Another note: "|" is the bitwise "or" operator.
	XSelectInput(self->display, self->root, SubstructureRedirectMask);
	XSync(self->display, 0);
	//PointerMotionMask |  | SubstructureNotifyMask | ButtonPressMask | KeyPressMask

	XGrabServer(self->display);

	Window returned_root, returned_parent;
	Window* top_level_windows;
	unsigned int num_top_level_windows;
	XQueryTree(
			self->display,
			self->root,
			&returned_root,
			&returned_parent,
			&top_level_windows,
			&num_top_level_windows);
	//     ii. Frame each top-level window.
	for (unsigned int i = 0; i < num_top_level_windows; ++i)
		frame(self, top_level_windows[i]);
	//     iii. Free top-level window array.
	XFree(top_level_windows);
	//   e. Ungrab X server.
	XUngrabServer(self->display);

	win = XCreateSimpleWindow(
			self->display,
			self->root,
			0,
			0,
			500,
			500,
			0, // border width
			0x0000FF, // boarder color. blue
			0XFF0000); // background color
	XSelectInput(self->display, win, ExposureMask|ButtonPressMask|KeyPressMask);
	//XMapWindow(self->display, win);
	// seems to do the same as above
	XMapRaised(self->display, win);
	//XLowerWindow(self->display, win);

	XTextItem i;
	i.delta = 0;
	i.font = None;

	GC gc = XCreateGC(self->display, win, 0, 0);
	XSync(self->display, 0);

	Cursor c = XCreateFontCursor(self->display, XC_left_ptr);
	XDefineCursor(self->display, self->root, c);
	XSync(self->display, 0);

	// main loop
	while (1)
	{
		XEvent e;
		XNextEvent(self->display, &e);

		XClearWindow(self->display, win);
		char string[100];

		sprintf(string,
				"type: %d " \
				"buttonpress: %d " \
				"keypress: %d " \
				"lasteven: %d ",
				e.type,
				ButtonPress,
				KeyPress,
				LASTEvent);

		i.chars = string;
		i.nchars = strlen(string);

		XDrawText(self->display, win, gc, 100, 100, &i, 1);


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
				on_button_press(self, &e.xbutton);
				break;
			case ButtonRelease:
				break;
			case MotionNotify:
				// Skip any already pending motion events.
				while (XCheckTypedWindowEvent(self->display, e.xmotion.window, MotionNotify, &e)) {}
				on_motion_notify(self, &e.xmotion);
				break;
			case KeyPress:
				on_key_press(self, &e.xkey);
				break;
			case KeyRelease:
				break;
			default:
				break;
		}
	}
}

static void close_window_manager(WindowManager *self)
{
	// break connection to xserver
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
}

static void on_button_press(WindowManager *self, XButtonEvent *e)
{
	XGrabServer(self->display);
	XSetCloseDownMode(self->display, DestroyAll);
	XKillClient(self->display, self->root);
	XSync(self->display, False);
	XUngrabServer(self->display);
}
static void on_button_release		(WindowManager *self, XButtonEvent *e)				{}

static void on_motion_notify(WindowManager *self, XMotionEvent *e)
{
}

static void on_key_press(WindowManager *self, XKeyEvent *e)
{
}

static void on_key_release			(WindowManager *self, XKeyEvent *e)					{}

static void frame(WindowManager *self, Window w)
{
	// get attributes of the window sending the map request
	XWindowAttributes windowAttributes;
	XGetWindowAttributes(self->display, w, &windowAttributes);

	// create the frame window for the decorations
	// this frame window will parent the the map requesting window
	Window frame = XCreateSimpleWindow(
			self->display,
			self->root,
			windowAttributes.x,
			windowAttributes.y,
			windowAttributes.width,
			windowAttributes.height,
			1, // border width
			0x0000FF, // boarder color. i think black
			0X0000FF); // background color

	// select events on frame
	XSelectInput(self->display, frame, SubstructureRedirectMask | SubstructureNotifyMask);

	// add client to save set, so that it will be restored and kept alive if we crash
	XAddToSaveSet(self->display, w);

	// reparent request window
	XReparentWindow(
			self->display,
			w,			// window to get reparented
			frame,	// parent window
			0, 0); // offset of client window within the frame. unsure what that mean

	// map frame for some reason
	XMapWindow(self->display, frame);

	// store the client frame window
	//self->clients[self->numClients++] = frame;

	// not sure how this stuff works
	// 9. Grab universal window management actions on client window.
	//   a. Move windows with alt + left button.
	XGrabButton(
			self->display,
			Button1,
			Mod1Mask,
			w,
			0,
			ButtonPressMask | ButtonReleaseMask | ButtonMotionMask,
			GrabModeAsync,
			GrabModeAsync,
			None,
			None);
	//   b. Resize windows with alt + right button.
	XGrabButton(
			self->display,
			Button3,
			Mod1Mask,
			w,
			0,
			ButtonPressMask | ButtonReleaseMask | ButtonMotionMask,
			GrabModeAsync,
			GrabModeAsync,
			None,
			None);
	//   c. Kill windows with alt + f4.
	XGrabKey(
			self->display,
			XKeysymToKeycode(self->display, XK_F4),
			Mod1Mask,
			w,
			0,
			GrabModeAsync,
			GrabModeAsync);
	//   d. Switch windows with alt + tab.
	XGrabKey(
			self->display,
			XKeysymToKeycode(self->display, XK_Tab),
			Mod1Mask,
			w,
			0,
			GrabModeAsync,
			GrabModeAsync);
}
