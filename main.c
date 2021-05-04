#include "wm.h"

int main()
{
	WindowManager *wm = new_window_manager();

	wm->run(wm);
	wm->close(wm);

	return 0;
}
