#ifndef WINDOW_MANGER
#define WINDOW_MANGER

#include <X11/Xlib.h>

typedef struct WindowManager
{
	// wm stuff
	Display *display;		// handle Xlib display struct
	Window root;			// handle to root window

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

#endif
