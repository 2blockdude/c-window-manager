#include <stdio.h>
#include "wm.h"

int main()
{
	WindowManager *wm = new_window_manager();

	if (wm == NULL) return 1;

	wm->run(wm);
	wm->close(wm);

	return 0;
}
