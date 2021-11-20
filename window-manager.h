#ifndef WINDOW_MANAGER
#define WINDOW_MANAGER

#include <X11/Xlib.h>

typedef struct window_manager WM;
struct window_manager
{
	// wm stuff
	int running;
	Display *display;		// handle Xlib display struct. establishes a connection to the Xserver
	Window root;			// handle to root window aka the window manager

	// clients todo: replace with binary tree or hash table
	int client_count;
	Window clients[100];

	// note: Window is a typedef for XID and XID is a typedef for unsigned long.
	// Soooo Window is an unsigned long. It is used as an address for the
	// actual window that the Xserver handles.

	// function pointers to start wm
	void (*run)							(struct window_manager *self);
	void (*close)						(struct window_manager *self);

	// function pointers to wm requests/notfity
	void (*handler[LASTEvent])			(struct window_manager *self, XEvent *e);
};

struct window_manager *new_window_manager();

#endif
