#include <stdio.h>
#include "window-manager.h"

int main()
{
	WM *wm = new_window_manager();

	if (wm == NULL) return 1;

	wm->run(wm);
	wm->close(wm);

	return 0;
}
