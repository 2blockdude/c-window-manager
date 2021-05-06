#ifndef WINDOW_MANAGER
#define WINDOW_MANAGER

#include <X11/Xlib.h>
#include <X11/Xutil.h> // add some keys i guess
#include <X11/Xos.h>
#include <X11/cursorfont.h>

typedef struct WindowManager
{
	// wm stuff
	Display *display;		// handle Xlib display struct. establishes a connection to the Xserver
	Window root;			// handle to root window aka the window manager

	// clients
	int numClients;
	Window clients[100];

	// note: Window is a typedef for XID and XID is a typedef for unsigned long.
	// Soooo Window is an unsigned long. It is used as an address for the
	// actual window that the Xserver handles.

	// wm functions
	void (*run)								(struct WindowManager *self);
	void (*close)							(struct WindowManager *self);
}WindowManager;

WindowManager *new_window_manager	();
static void run_window_manager		(WindowManager *self);
static void close_window_manager		(WindowManager *self);

static void on_create_notify			(WindowManager *self, XCreateWindowEvent *e);
static void on_destroy_notify			(WindowManager *self, XDestroyWindowEvent *e);
static void on_reparent_notify		(WindowManager *self, XReparentEvent *e);
static void on_map_notify				(WindowManager *self, XMapEvent *e);
static void on_unmap_notify			(WindowManager *self, XUnmapEvent *e);
static void on_configure_notify		(WindowManager *self, XConfigureEvent *e);
static void on_map_request				(WindowManager *self, XMapRequestEvent *e);
static void on_configure_request		(WindowManager *self, XConfigureRequestEvent *e);
static void on_button_press			(WindowManager *self, XButtonEvent *e);
static void on_button_release			(WindowManager *self, XButtonEvent *e);
static void on_motion_notify			(WindowManager *self, XMotionEvent *e);
static void on_key_press				(WindowManager *self, XKeyEvent *e);
static void on_key_release				(WindowManager *self, XKeyEvent *e);

static void frame							(WindowManager *self, Window w);

static Window win;

#endif
