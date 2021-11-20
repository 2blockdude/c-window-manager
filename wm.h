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

	// clients todo: replace with binary tree or hash table
	int numClients;
	Window clients[100];

	// note: Window is a typedef for XID and XID is a typedef for unsigned long.
	// Soooo Window is an unsigned long. It is used as an address for the
	// actual window that the Xserver handles.

	// function pointers to start wm
	void (*run)							(WM *self);
	void (*close)						(WM *self);

	// function pointers to wm requests/notfity
	void (*handler[LASTEvent])			(WM *self, XEvent *e);
};

WM *new_window_manager					();

#endif
