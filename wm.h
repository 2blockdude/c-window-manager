#ifndef WINDOW_MANAGER
#define WINDOW_MANAGER

#include <X11/Xlib.h>

typedef struct WindowManager WM;
struct WindowManager
{
	// wm stuff
	int running;
	Display *display;		// handle Xlib display struct. establishes a connection to the Xserver
	Window root;			// handle to root window aka the window manager

	// clients
	int numClients;
	Window clients[100];

	// note: Window is a typedef for XID and XID is a typedef for unsigned long.
	// Soooo Window is an unsigned long. It is used as an address for the
	// actual window that the Xserver handles.

	// function pointers to start wm
	void (*run)								(WM *self);
	void (*close)							(WM *self);

	// function pointers to wm requests/notfity
	void (*handler[LASTEvent])			(WM *self, XEvent *e);
};

WM *new_window_manager	();
static void setup_window_manager		(WM *self);
static void start_window_manager		(WM *self);
static void close_window_manager		(WM *self);

static void on_create_notify			(WM *self, XEvent *e);
static void on_destroy_notify			(WM *self, XEvent *e);
static void on_reparent_notify		(WM *self, XEvent *e);
static void on_map_notify				(WM *self, XEvent *e);
static void on_mapping_notify			(WM *self, XEvent *e);
static void on_unmap_notify			(WM *self, XEvent *e);
static void on_configure_notify		(WM *self, XEvent *e);
static void on_map_request				(WM *self, XEvent *e);
static void on_configure_request		(WM *self, XEvent *e);
static void on_button_press			(WM *self, XEvent *e);
static void on_button_release			(WM *self, XEvent *e);
static void on_motion_notify			(WM *self, XEvent *e);
static void on_key_press				(WM *self, XEvent *e);
static void on_key_release				(WM *self, XEvent *e);

static void decorate_window			(WM *self, Window w);

#endif
