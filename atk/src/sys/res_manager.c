
#include "xtcp.h"
#include "buff.h"
#include "uart.h"
#include "res_manager.h"

gboolean cfg_ishide = FALSE;
gboolean dbg_ishide = FALSE;


void view_onRowActivated(GtkTreeView *treeview, GtkTreePath *path, GtkTreeViewColumn *col, gpointer userdata)
{
	GtkTreeModel *model;
	GtkTreeIter iter;

	g_print ("A row has been double-clicked!\n");
	model = gtk_tree_view_get_model(treeview);

	if (gtk_tree_model_get_iter(model, &iter, path))
	{
		gchar *name;
		gtk_tree_model_get(model, &iter, COLUMN, &name, -1);
		g_print ("Double-clicked row contains name %s\n", name);
		g_free(name);
	}
}


void on_changed(GtkWidget *widget, gpointer statusbar) 
{
	GtkTreeIter iter;
	GtkTreeModel *model;
	char *value;
	if(gtk_tree_selection_get_selected(GTK_TREE_SELECTION(widget), &model, &iter))
	{
		gtk_tree_model_get(model, &iter, COLUMN, &value,-1);
		gtk_statusbar_push(GTK_STATUSBAR(statusbar),
							gtk_statusbar_get_context_id(GTK_STATUSBAR(statusbar),
							value),
							value);
		g_free(value);
	}
}

void view_popup_menu_onDoSomething(GtkWidget *menuitem, gpointer userdata)
{
	/* we passed the view as userdata when we connected the signal */
	GtkTreeView *treeview = GTK_TREE_VIEW(userdata);

	g_print ("Do something!\n");
}

void view_popup_menu(GtkWidget *treeview, GdkEventButton *event, gpointer userdata)
{
	GtkWidget *menu, *menuitem;

	menu = gtk_menu_new();
	menuitem = gtk_menu_item_new_with_label("Do something");

	g_signal_connect(menuitem, "activate",(GCallback) view_popup_menu_onDoSomething, treeview);

	gtk_menu_shell_append(GTK_MENU_SHELL(menu), menuitem);

	gtk_widget_show_all(menu);

	/* Note: event can be NULL here when called from view_onPopupMenu;
	*  gdk_event_get_time() accepts a NULL argument */
	gtk_menu_popup(GTK_MENU(menu), 
					NULL, NULL, NULL, NULL,
					(event != NULL) ? event->button : 0,
					gdk_event_get_time((GdkEvent*)event));
}


gboolean view_onButtonPressed(GtkWidget *treeview, GdkEventButton *event, gpointer userdata)
{
	/* single click with the right mouse button? */
	if (event->type == GDK_BUTTON_PRESS  &&  event->button == 3)
	{
		g_print ("Single right click on the tree view.\n");

		/* optional: select row if no row is selected or only
		*  one other row is selected (will only do something
		*  if you set a tree selection mode as described later
		*  in the tutorial) */
		if (1)
		{
			GtkTreeSelection *selection;

			selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(treeview));

				/* Note: gtk_tree_selection_count_selected_rows() does not
				*   exist in gtk+-2.0, only in gtk+ >= v2.2 ! */
				if (gtk_tree_selection_count_selected_rows(selection)  <= 1)
				{
					GtkTreePath *path;

					/* Get tree path for row that was clicked */
					if (gtk_tree_view_get_path_at_pos(GTK_TREE_VIEW(treeview),
									 (gint) event->x, 
									 (gint) event->y,
									 &path, NULL, NULL, NULL))
					{
						gtk_tree_selection_unselect_all(selection);
						gtk_tree_selection_select_path(selection, path);
						gtk_tree_path_free(path);
					}
				}
		} /* end of optional bit */
		view_popup_menu(treeview, event, userdata);
		return TRUE; /* we handled this */
	}
	return FALSE; /* we did not handle this */
}

gboolean view_onPopupMenu(GtkWidget *treeview, gpointer userdata)
{
	view_popup_menu(treeview, NULL, userdata);
	return TRUE; /* we handled this */
}

void on_sub_win_close_event(GtkWidget *window,GdkEvent *event,gpointer data)
{
	gtk_widget_hide(window);
	cfg_ishide = FALSE;
}

void on_dbg_win_close_event(GtkWidget *window,GdkEvent *event,gpointer data)
{
	gtk_widget_hide(window);
	dbg_ishide = FALSE;
}

