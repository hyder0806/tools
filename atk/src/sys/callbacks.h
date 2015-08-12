#ifndef __CALLBACKS_H__
#define __CALLBACKS_H__

#include "main.h"

extern int screen_update;

extern void open_button_clicked(GtkWidget *button,gpointer userdata);
extern void cfg_button_clicked(GtkWidget *button,gpointer userdata);
extern void add_button_clicked(GtkWidget *button,gpointer userdata);
extern void clear_button_clicked(GtkWidget *button,gpointer userdata);
extern void on_close_win_event(GtkWidget *widget,GdkEvent *event, gpointer data);
extern void manager_button_clicked(GtkWidget *button,gpointer userdata);
extern void debug_system_button_clicked(GtkWidget *button,gpointer userdata);
extern void file_open_button_clicked(GtkWidget *button,gpointer userdata);
extern void start_dnl_button_clicked(GtkWidget *button,gpointer userdata);
extern void connect_uart_button_clicked(GtkWidget *button,gpointer userdata);
extern void connect_usb_button_clicked(GtkWidget *button,gpointer userdata);

//comboBox callback
extern gboolean combo_changed(GtkComboBox *comboBox, GtkLabel *label);

extern unsigned char display_update(void);

#endif
