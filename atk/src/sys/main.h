#ifndef __MAIN_H__
#define __MAIN_H__

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <signal.h>
#include <time.h>
#include <memory.h>
#include <sys/time.h>
#include <gtk/gtk.h>
#include <gdk/gdkkeysyms.h>

enum{
	COLUMN = 0,
	NUM_COLS,
};

GtkWidget *main_window;
GtkWidget *sub_window;
GtkWidget *dbg_window;

#endif

