#ifndef WINDOW_MANGER
#define WINDOW_MANGER

#include <X11/Xlib.h>

typedef struct WindowManager
{
	// wm stuff
	Display *display;		// handle Xlib display struct
	Window root;			// handle to root window

	// wm functions
	void (*run)								(struct WindowManager *wm);
	void (*close)							(struct WindowManager *wm);
	
	// notify function pointers
	void (*on_create)						(XCreateWindowEvent *e);
	void (*on_destroy)					(XDestroyWindowEvent *e);
	void (*on_reparent)					(XReparentEvent *e);
	void (*on_map)							(XMapEvent *e);
	void (*on_unmap)						(XUnmapEvent *e);
	void (*on_configure)					(XConfigureEvent *e);
	void (*on_map_request)				(XMapRequestEvent *e);
	void (*on_configure_request)		(XConfigureRequestEvent *e);
	void (*on_button_press)				(XButtonEvent *e);
	void (*on_button_release)			(XButtonEvent *e);
	void (*on_motion)						(XMotionEvent *e);
	void (*on_key_press)					(XKeyEvent *e);
	void (*on_key_release)				(XKeyEvent *e);
}WindowManager;

WindowManager *new_window_manager	();
static void run_window_manager		(WindowManager *wm);
static void close_window_manager		(WindowManager *wm);

static void on_create_notify			(XCreateWindowEvent *e);
static void on_destroy_notify			(XDestroyWindowEvent *e);
static void on_reparent_notify		(XReparentEvent *e);
static void on_map_notify				(XMapEvent *e);
static void on_unmap_notify			(XUnmapEvent *e);
static void on_configure_notify		(XConfigureEvent *e);
static void on_map_request				(XMapRequestEvent *e);
static void on_configure_request		(XConfigureRequestEvent *e);
static void on_button_press			(XButtonEvent *e);
static void on_button_release			(XButtonEvent *e);
static void on_motion_notify			(XMotionEvent *e);
static void on_key_press				(XKeyEvent *e);
static void on_key_release				(XKeyEvent *e);

#endif
