#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <time.h>

#include <sys/wait.h>

#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/cursorfont.h>

#include "wm.h"

WM *new_window_manager()
{
	WM *wm = malloc(sizeof(WM));;

	wm->running = 0;
	wm->display = XOpenDisplay(NULL); if (wm->display == NULL) { free(wm); return NULL; }
	wm->root = DefaultRootWindow(wm->display);
	wm->numClients = 0;

	wm->run = setup_window_manager;
	wm->close = close_window_manager;

	wm->handler[CreateNotify] = on_create_notify;
	wm->handler[DestroyNotify] = on_destroy_notify;
	wm->handler[ReparentNotify] = on_reparent_notify;
	wm->handler[MapNotify] = on_map_notify;
	wm->handler[MappingNotify] = on_mapping_notify;
	wm->handler[MapRequest] = on_map_request;
	wm->handler[UnmapNotify] = on_unmap_notify;
	wm->handler[ConfigureNotify] = on_configure_notify;
	wm->handler[ConfigureRequest] = on_configure_request;
	wm->handler[MotionNotify] = on_motion_notify;
	wm->handler[ButtonPress] = on_button_press;
	wm->handler[ButtonRelease] = on_button_release;
	wm->handler[KeyPress] = on_key_press;
	wm->handler[KeyRelease] = on_key_release;

	return wm;
}

static void setup_window_manager(WM *self)
{
	// note: for SubstructreRedirectMask | SubstructureNotifyMask I think we are
	// combining the two masks for some reason.
	// Another note: "|" is the bitwise "or" operator.
	// note: these masks apply some sort of listener to that window.
	XSelectInput(self->display, self->root, SubstructureRedirectMask | SubstructureNotifyMask);
	XSync(self->display, 0);
	//PointerMotionMask |  |  | ButtonPressMask | KeyPressMask | ButtonPressMask | KeyPressMask

	//XGrabServer(self->display);
	//Window returned_root, returned_parent;
	//Window* top_level_windows;
	//unsigned int num_top_level_windows;
	//XQueryTree(
	//		self->display,
	//		self->root,
	//		&returned_root,
	//		&returned_parent,
	//		&top_level_windows,
	//		&num_top_level_windows);
	////     ii. Frame each top-level window.
	//for (unsigned int i = 0; i < num_top_level_windows; ++i)
	//	decorate_window(self, top_level_windows[i]);
	////     iii. Free top-level window array.
	//XFree(top_level_windows);
	////   e. Ungrab X server.
	//XUngrabServer(self->display);


	// create cursor
	int snum = DefaultScreen(self->display);
	Cursor c = XCreateFontCursor(self->display, XC_left_ptr);
	XDefineCursor(self->display, self->root, c);
	XWarpPointer(self->display, None, self->root, 0, 0, 0, 0, DisplayWidth(self->display, snum) / 2, DisplayHeight(self->display, snum) / 2);

	//XSetWindowAttributes wa;
	//wa.cursor = c;
	//wa.event_mask = SubstructureRedirectMask | SubstructureNotifyMask;
	////|ButtonPressMask|PointerMotionMask|EnterWindowMask|LeaveWindowMask|StructureNotifyMask|PropertyChangeMask;
	//XChangeWindowAttributes(self->display, self->root, CWEventMask | CWCursor, &wa);
	//XSelectInput(self->display, self->root, wa.event_mask);

	XUngrabKey(self->display, AnyKey, AnyModifier, self->root);
	XGrabKey(self->display, XKeysymToKeycode(self->display, XK_s), Mod1Mask, self->root, 1, GrabModeAsync, GrabModeAsync);
	XGrabKey(self->display, XKeysymToKeycode(self->display, XK_Escape), Mod1Mask, self->root, 1, GrabModeAsync, GrabModeAsync);

	start_window_manager(self);
}

static void start_window_manager(WM *self)
{
	self->running = 1;

	// main loop
	XEvent e;
	XSync(self->display, 0);
	while (self->running && !XNextEvent(self->display, &e))
		if (self->handler[e.type])
			self->handler[e.type](self, &e);
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

static void on_mapping_notify(WM *self, XEvent *e) {}

static void on_unmap_notify(WM *self, XEvent *e)
{
	XUnmapEvent *ev = &e->xunmap;

	if (ev->event == self->root)
		return;
	//undecorate_window(self, ev->window);
}

static void on_configure_notify(WM *self, XEvent *e) {}

static void on_map_request(WM *self, XEvent *e)
{
	XMapRequestEvent *ev = &e->xmaprequest;

	// frame window with window decorations.
	//decorate_window(self, ev->window);
	XGrabButton(self->display, Button1, 0, ev->window, 0, ButtonPressMask | ButtonReleaseMask, GrabModeAsync, GrabModeAsync, None, None);
	XGrabButton(self->display, Button1, Mod1Mask, ev->window, 0, ButtonMotionMask, GrabModeAsync, GrabModeAsync, None, None);
	XGrabButton(self->display, Button3, Mod1Mask, ev->window, 0, ButtonPressMask | ButtonReleaseMask | ButtonMotionMask, GrabModeAsync, GrabModeAsync, None, None);
	XGrabKey(self->display, XKeysymToKeycode(self->display, XK_F4), Mod1Mask, ev->window, 0, GrabModeAsync, GrabModeAsync);
	XGrabKey(self->display, XKeysymToKeycode(self->display, XK_Tab), Mod1Mask, ev->window, 0, GrabModeAsync, GrabModeAsync);

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
	XButtonPressedEvent *ev = &e->xbutton;

	XRaiseWindow(self->display, ev->window);
}

static void on_button_release(WM *self, XEvent *e)
{
	XButtonEvent *ev = &e->xbutton;

}

static void on_motion_notify(WM *self, XEvent *e)
{
	XMotionEvent *ev = &e->xmotion;

	if (ev->state & Button1Mask)
	{
		XMoveWindow(self->display, ev->window, ev->x_root, ev->y_root);
	}
}

static void on_key_press(WM *self, XEvent *e)
{
	XKeyEvent *ev = &e->xkey;

	if ((ev->state & Mod1Mask) && (ev->keycode == XKeysymToKeycode(self->display, XK_s)))
	{
		// note: fork comes from unistd.h and when called starts a child process for the next functions
		if (fork() == 0)
		{
			if (self->display)
				close(ConnectionNumber(self->display));
			setsid();

			execlp("dmenu_run", "");

			fprintf(stderr, "wm: execvp %s", "dmenu");
			perror(" failed");
			exit(0);
		}
	}
	else if ((ev->state & Mod1Mask) && (ev->keycode == XKeysymToKeycode(self->display, XK_Escape)))
	{
		self->running = 0;
	}
	else if ((ev->state & Mod1Mask) && (ev->keycode == XKeysymToKeycode(self->display, XK_F4)))
	{
		Atom *supportedProtocols;
		int numSupportedProtocols;

		if (XGetWMProtocols(self->display, ev->window, &supportedProtocols, &numSupportedProtocols))
		{
			// gracefully close
			XEvent msg;
			msg.xclient.type = ClientMessage;
			msg.xclient.message_type =	XInternAtom(self->display, "WM_PROTOCOLS", 0);
			msg.xclient.window = ev->window;
			msg.xclient.format = 32;
			msg.xclient.data.l[0] = XInternAtom(self->display, "WM_DELETE_WINDOW", 0);
			XSendEvent(self->display, ev->window, 0, 0, &msg);
		}
		else
		{
			// force close
			XGrabServer(self->display);
			XSetCloseDownMode(self->display, DestroyAll);
			XKillClient(self->display, ev->window);
			XSync(self->display, 0);
			XUngrabServer(self->display);
		}
	}
}

static void on_key_release(WM *self, XEvent *e) {}

static void decorate_window(WM *self, Window w)
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
			2, // border width
			0x0000FF, // boarder color. i think black
			0X0000FF); // background color

	// select events on frame
	XSelectInput(self->display, frame, SubstructureRedirectMask | SubstructureNotifyMask);

	// add client to save set, so that it will be restored and kept alive if we crash
	// don't really want for now
	// XAddToSaveSet(self->display, w);

	// reparent request window
	XReparentWindow(
			self->display,
			w,			// window to get reparented
			frame,	// parent window
			0, 0);	// offset of client window within the frame window

	// map frame for some reason
	XMapWindow(self->display, frame);

	// store the client frame window
	// self->clients[w % 100] = frame;


	// Grab universal window management actions on client window.

	// adds a listener that only listens for those spesific keys
	// and if if encounters only those keys are being pressed then it
	// will trigger a KeyPress handler function
	XGrabButton(self->display, Button1, Mod1Mask, w, 0, ButtonPressMask | ButtonReleaseMask | ButtonMotionMask, GrabModeAsync, GrabModeAsync, None, None);
	XGrabButton(self->display, Button3, Mod1Mask, w, 0, ButtonPressMask | ButtonReleaseMask | ButtonMotionMask, GrabModeAsync, GrabModeAsync, None, None);
	XGrabKey(self->display, XKeysymToKeycode(self->display, XK_F4), Mod1Mask, w, 0, GrabModeAsync, GrabModeAsync);
	XGrabKey(self->display, XKeysymToKeycode(self->display, XK_Tab), Mod1Mask, w, 0, GrabModeAsync, GrabModeAsync);
}

static void undecorate_window(WM *self, Window w)
{
	// reverse of frame
	Window frame = self->clients[w % 100];

	XUnmapWindow(self->display, frame);
	XDestroyWindow(self->display, frame);
}
