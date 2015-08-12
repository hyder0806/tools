
#include "main.h"
#include "callbacks.h"
#include "appearance.h"
#include "res_manager.h"

int main(int argc , char *argv[])
{
	if(g_thread_supported() == 0)
	{
		g_thread_init(NULL);
	}
	
	gdk_threads_init();
	
	gtk_init(&argc,&argv);
	
	main_window = create_window();
	sub_window = create_sub_window();
	dbg_window = create_dbg_sys_window();
	
	g_signal_connect(G_OBJECT(main_window),"delete_event",
					G_CALLBACK(on_close_win_event),NULL);
	
	g_signal_connect(G_OBJECT(sub_window),"delete_event",
					G_CALLBACK(on_sub_win_close_event),NULL);
	
	g_signal_connect(G_OBJECT(dbg_window),"delete_event",
					G_CALLBACK(on_dbg_win_close_event),NULL);
	
	gtk_widget_show_all(main_window);

	gdk_threads_enter();
	
	gtk_main();
	
	gdk_threads_leave();

	return FALSE;
}
