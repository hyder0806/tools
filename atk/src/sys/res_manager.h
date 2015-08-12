#ifndef __RES_MANAGER_H__
#define __RES_MANAGER_H__

#include "main.h"

extern gboolean cfg_ishide;
extern gboolean dbg_ishide;

void on_sub_win_close_event(GtkWidget *window,GdkEvent *event,gpointer data);
void on_dbg_win_close_event(GtkWidget *window,GdkEvent *event,gpointer data);
void on_changed(GtkWidget *widget, gpointer statusbar);
void view_onRowActivated(GtkTreeView *treeview, GtkTreePath *path, GtkTreeViewColumn *col, gpointer userdata);

gboolean view_onPopupMenu (GtkWidget *treeview, gpointer userdata);
gboolean view_onButtonPressed (GtkWidget *treeview, GdkEventButton *event, gpointer userdata);

#endif
