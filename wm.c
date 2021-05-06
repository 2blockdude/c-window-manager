#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>

#include "wm.h"

WM *new_window_manager()
{
	WM *wm = malloc(sizeof(WM));;

	wm->display = XOpenDisplay(NULL);
	if (wm->display == NULL) return NULL;
	wm->root = DefaultRootWindow(wm->display);
	wm->numClients = 0;

	wm->run = run_window_manager;
	wm->close = close_window_manager;

	wm->handler[CreateNotify] = on_create_notify;
	wm->handler[DestroyNotify] = on_destroy_notify;
	wm->handler[ReparentNotify] = on_reparent_notify;
	wm->handler[MapNotify] = on_map_notify;
	wm->handler[UnmapNotify] = on_unmap_notify;
	wm->handler[ConfigureNotify] = on_configure_notify;
	wm->handler[MapRequest] = on_map_request;
	wm->handler[ConfigureRequest] = on_configure_request;
	wm->handler[ButtonPress] = on_button_press;
	wm->handler[ButtonRelease] = on_button_release;
	wm->handler[MotionNotify] = on_motion_notify;
	wm->handler[KeyPress] = on_key_press;
	wm->handler[KeyRelease] = on_key_release;

	return wm;
}

static void run_window_manager(WM *self)
{
	// note: for SubstructreRedirectMask | SubstructureNotifyMask I think we are
	// combining the two masks for some reason.
	// Another note: "|" is the bitwise "or" operator.
	XSelectInput(self->display, self->root, SubstructureRedirectMask | SubstructureNotifyMask);
	XSync(self->display, 0);
	//PointerMotionMask |  |  | ButtonPressMask | KeyPressMask

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

	Cursor c = XCreateFontCursor(self->display, XC_left_ptr);
	XDefineCursor(self->display, self->root, c);
	XSync(self->display, 0);

	// main loop
	while (1)
	{
		XEvent e;
		XNextEvent(self->display, &e);

		//dispatch event
		if (self->handler[e.type])
			self->handler[e.type](self, &e);

	}
}

static void close_window_manager(WM *self)
{
	// break connection to xserver
	XCloseDisplay(self->display);
	self->display = NULL;
}

static void on_create_notify(WM *self, XEvent *e) {}
static void on_destroy_notify(WM *self, XEvent *e) {}
static void on_reparent_notify(WM *self, XEvent *e) {}
static void on_map_notify(WM *self, XEvent *e) {}
static void on_unmap_notify(WM *self, XEvent *e) {}
static void on_configure_notify(WM *self, XEvent *e) {}

static void on_map_request(WM *self, XEvent *e)
{
	XMapRequestEvent *ev = &e->xmaprequest;
	// note: frame is a custom function
	// frame window with window decorations.
	// frame e->window to self
	frame(self, ev->window);
	// map window i guess to main display
	XMapWindow(self->display, ev->window);
}

static void on_configure_request(WM *self, XEvent *e)
{
	XConfigureRequestEvent *ev = &e->xconfigurerequest;
	XWindowChanges changes;

	// copy stuff from e to changes for some reason
	changes.x = ev->x;
	changes.y = ev->y;
	changes.width = ev->width;
	changes.height = ev->height;
	changes.border_width = ev->border_width;
	changes.sibling = ev->above;
	changes.stack_mode = ev->detail;

	// grant request
	XConfigureWindow(self->display, ev->window, ev->value_mask, &changes);
}

static void on_button_press(WM *self, XEvent *e)
{
	XGrabServer(self->display);
	XSetCloseDownMode(self->display, DestroyAll);
	XKillClient(self->display, self->root);
	XSync(self->display, False);
	XUngrabServer(self->display);
}

static void on_button_release(WM *self, XEvent *e)
{
}

static void on_motion_notify(WM *self, XEvent *e)
{
}

static void on_key_press(WM *self, XEvent *e)
{
}

static void on_key_release(WM *self, XEvent *e)
{
}

static void frame(WM *self, Window w)
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
	self->clients[self->numClients++] = frame;

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
