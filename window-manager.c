#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <time.h>

#include <sys/wait.h>

#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/cursorfont.h>

#include "window-manager.h"

void setup_window_manager		(struct window_manager *self);
void start_window_manager		(struct window_manager *self);
void close_window_manager		(struct window_manager *self);

void on_create_notify			(struct window_manager *self, XEvent *e);
void on_destroy_notify			(struct window_manager *self, XEvent *e);
void on_reparent_notify			(struct window_manager *self, XEvent *e);
void on_map_notify				(struct window_manager *self, XEvent *e);
void on_mapping_notify			(struct window_manager *self, XEvent *e);
void on_unmap_notify				(struct window_manager *self, XEvent *e);
void on_configure_notify		(struct window_manager *self, XEvent *e);
void on_map_request				(struct window_manager *self, XEvent *e);
void on_configure_request		(struct window_manager *self, XEvent *e);
void on_button_press				(struct window_manager *self, XEvent *e);
void on_button_release			(struct window_manager *self, XEvent *e);
void on_motion_notify			(struct window_manager *self, XEvent *e);
void on_key_press					(struct window_manager *self, XEvent *e);
void on_key_release				(struct window_manager *self, XEvent *e);

void decorate_window				(struct window_manager *self, Window w);
void undecorate_window			(struct window_manager *self, Window w);
void kill_client					(struct window_manager *self, Window w);
void spawn_client					(struct window_manager *self, char *bin, char *argv[]);

struct window_manager *new_window_manager()
{
	struct window_manager *wm = malloc(sizeof(struct window_manager));;

	wm->running = 0;
	wm->display = XOpenDisplay(NULL); if (wm->display == NULL) { free(wm); return NULL; }
	wm->root = DefaultRootWindow(wm->display);

	wm->client_count = 0;
	//memcpy(wm->clients, 0, 100);

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

void setup_window_manager(struct window_manager *self)
{
	// note: for SubstructreRedirectMask | SubstructureNotifyMask I think we are
	// combining the two masks for some reason.
	// Another note: "|" is the bitwise "or" operator.
	// note: these masks apply some sort of listener to that window.
	XSelectInput(self->display, self->root, SubstructureRedirectMask | SubstructureNotifyMask);
	XSync(self->display, 0);
	//PointerMotionMask |  |  | ButtonPressMask | KeyPressMask | ButtonPressMask | KeyPressMask

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
	
	XGrabKey(self->display, XKeysymToKeycode(self->display, XK_F4), Mod1Mask, self->root, 0, GrabModeAsync, GrabModeAsync);

	start_window_manager(self);
}

void start_window_manager(struct window_manager *self)
{
	self->running = 1;

	// main loop
	XEvent e;
	XSync(self->display, 0);
	while (self->running && !XNextEvent(self->display, &e))
		if (self->handler[e.type])
			self->handler[e.type](self, &e);
}

void close_window_manager(struct window_manager *self)
{
	// break connection to xserver
	XCloseDisplay(self->display);
	self->display = NULL;
}

void on_create_notify(struct window_manager *self, XEvent *e) {}
void on_destroy_notify(struct window_manager *self, XEvent *e) {}
void on_reparent_notify(struct window_manager *self, XEvent *e) {}
void on_map_notify(struct window_manager *self, XEvent *e) {}

void on_mapping_notify(struct window_manager *self, XEvent *e) {}

void on_unmap_notify(struct window_manager *self, XEvent *e)
{
	XUnmapEvent *ev = &e->xunmap;

	if (ev->event == self->root)
		return;
	//undecorate_window(self, ev->window);
}

void on_configure_notify(struct window_manager *self, XEvent *e) {}

void on_map_request(struct window_manager *self, XEvent *e)
{
	XMapRequestEvent *ev = &e->xmaprequest;

	// frame window with window decorations.
	//decorate_window(self, ev->window);
	//XGrabButton(self->display, Button1, 0, ev->window, 0, ButtonPressMask | ButtonReleaseMask, GrabModeAsync, GrabModeAsync, None, None);
	//XGrabButton(self->display, Button1, Mod1Mask, ev->window, 0, ButtonMotionMask, GrabModeAsync, GrabModeAsync, None, None);
	//XGrabButton(self->display, Button3, Mod1Mask, ev->window, 0, ButtonPressMask | ButtonReleaseMask | ButtonMotionMask, GrabModeAsync, GrabModeAsync, None, None);
	//XGrabKey(self->display, XKeysymToKeycode(self->display, XK_F4), Mod1Mask, ev->window, 0, GrabModeAsync, GrabModeAsync);
	//XGrabKey(self->display, XKeysymToKeycode(self->display, XK_Tab), Mod1Mask, ev->window, 0, GrabModeAsync, GrabModeAsync);

	XMapWindow(self->display, ev->window);
}

void on_configure_request(struct window_manager *self, XEvent *e)
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

void on_button_press(struct window_manager *self, XEvent *e)
{
	XButtonPressedEvent *ev = &e->xbutton;

	XRaiseWindow(self->display, ev->window);
}

void on_button_release(struct window_manager *self, XEvent *e)
{
	XButtonEvent *ev = &e->xbutton;

}

void on_motion_notify(struct window_manager *self, XEvent *e)
{
	XMotionEvent *ev = &e->xmotion;

	if (ev->state & Button1Mask)
	{
		XMoveWindow(self->display, ev->window, ev->x, ev->y);
	}
}

void on_key_press(struct window_manager *self, XEvent *e)
{
	XKeyEvent *ev = &e->xkey;

	if ((ev->state & Mod1Mask) && (ev->keycode == XKeysymToKeycode(self->display, XK_s)))
	{
		spawn_client(self, "dmenu_run", NULL);
	}
	else if ((ev->state & Mod1Mask) && (ev->keycode == XKeysymToKeycode(self->display, XK_Escape)))
	{
		self->running = 0;
	}
	else if ((ev->state & Mod1Mask) && (ev->keycode == XKeysymToKeycode(self->display, XK_F4)))
	{
		Window win;
		kill_client(self, ev->window);
	}
}

void on_key_release(struct window_manager *self, XEvent *e) {}

void decorate_window(struct window_manager *self, Window w)
{
	// get attributes of the window sending the map request
	XWindowAttributes windowAttributes;
	XGetWindowAttributes(self->display, w, &windowAttributes);

	// create the frame window for the decorations
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

	// reparent request window
	XReparentWindow(
			self->display,
			w,			// window to get reparented
			frame,	// parent window
			0, 0);	// offset of client window within the frame window

	XMapWindow(self->display, frame);

	// make frame listen for these key events
	XGrabButton(self->display, Button1, Mod1Mask, w, 0, ButtonPressMask | ButtonReleaseMask | ButtonMotionMask, GrabModeAsync, GrabModeAsync, None, None);
	XGrabButton(self->display, Button3, Mod1Mask, w, 0, ButtonPressMask | ButtonReleaseMask | ButtonMotionMask, GrabModeAsync, GrabModeAsync, None, None);
	XGrabKey(self->display, XKeysymToKeycode(self->display, XK_F4), Mod1Mask, w, 0, GrabModeAsync, GrabModeAsync);
	XGrabKey(self->display, XKeysymToKeycode(self->display, XK_Tab), Mod1Mask, w, 0, GrabModeAsync, GrabModeAsync);
}

void undecorate_window(struct window_manager *self, Window w)
{
	// reverse of frame
	Window frame = self->clients[w % 100];

	XUnmapWindow(self->display, frame);
	XDestroyWindow(self->display, frame);
}

void kill_client(struct window_manager *self, Window w)
{
	Atom *supportedProtocols;
	int numSupportedProtocols;

	if (XGetWMProtocols(self->display, w, &supportedProtocols, &numSupportedProtocols))
	{
		// gracefully close
		XEvent msg;
		msg.xclient.type = ClientMessage;
		msg.xclient.message_type =	XInternAtom(self->display, "struct window_manager_PROTOCOLS", 0);
		msg.xclient.window = w;
		msg.xclient.format = 32; // unsure what 32 means
		msg.xclient.data.l[0] = XInternAtom(self->display, "struct window_manager_DELETE_WINDOW", 0);
		XSendEvent(self->display, w, 0, 0, &msg);
	}
	else
	{
		// forcefully close
		XGrabServer(self->display);
		XSetCloseDownMode(self->display, DestroyAll);
		XKillClient(self->display, w);
		XSync(self->display, 0);
		XUngrabServer(self->display);
	}
}

void spawn_client(struct window_manager *self, char *bin, char *argv[])
{
	// note: fork comes from unistd.h and when called starts a child process for the next functions

	// if fork returns 0 it means we are in the child proccess
	// if fork returns -1 it means the fork failed
	// anything else is the proccess id of the parent proccess
	if (fork() == 0)
	{
		if (self->display)
			close(ConnectionNumber(self->display));
		setsid();

		execvp(bin, argv);

		fprintf(stderr, "wm: execvp %s", "dmenu");
		perror(" failed");
		exit(0);
	}
}
