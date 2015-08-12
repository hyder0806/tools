
#include "main.h"
#include "callbacks.h"
#include "self_item.h"
#include "res_manager.h"

#define BORDER_WIDTH 2
#define DEFAULT_DOWN_FILE "./boot_csf.bin"

const gchar *comm[] = {"UART", "USB"}; 


#define COMM_LEN		sizeof(comm) / sizeof(comm[0])

gint count;
GtkWidget *TextArea;
GtkTextBuffer *g_buffer;
GtkWidget *Kitem;
GtkWidget *dnl_label;
GtkWidget *dnl_comboBox;
GtkWidget *prcss_bar;	//process_bar
GtkWidget *dnl_path;



GtkWidget *uart_comboBox_port;
GtkWidget *uart_comboBox_baudrate;
GtkWidget *uart_comboBox_bit;
GtkWidget *uart_comboBox_stop;
GtkWidget *uart_comboBox_hd_ctl;

const gchar *comboBox_port[] = {"USB0", "USB1", "TTY0", "TTY1"}; 
const gchar *comboBox_baudrate[] = {"115200", "38400", "9600"}; 
const gchar *comboBox_bit[] = {"8", "7"}; 
const gchar *comboBox_stop[] = {"1", "0"}; 
const gchar *comboBox_hd_ctl[] = {"none", "hard", "soft"}; 

#define COMB_PORT_LEN		sizeof(comboBox_port) / sizeof(comboBox_port[0])
#define COMB_BAUD_LEN		sizeof(comboBox_baudrate) / sizeof(comboBox_baudrate[0])
#define COMB_BIT_LEN		sizeof(comboBox_bit) / sizeof(comboBox_bit[0])
#define COMB_STOP_LEN		sizeof(comboBox_stop) / sizeof(comboBox_stop[0])
#define COMB_HD_LEN		sizeof(comboBox_hd_ctl) / sizeof(comboBox_hd_ctl[0])

GtkTreeModel *create_combo_Model(const gchar *BoxText[],gint size)
{
    GtkTreeIter iter;
    GtkListStore *store;
    gint i;
    
    store = gtk_list_store_new(1, G_TYPE_STRING);
   
    for(i = 0; i < size; i++)
    {
        gtk_list_store_append(store, &iter);
        gtk_list_store_set(store, &iter, 0, BoxText[i], -1);
    }
    
    return GTK_TREE_MODEL(store);
}  

GtkWidget *create_window(void)
{
	GtkWidget *window;
	GtkWidget *box,*box2;
	GtkWidget *scrolledwin;
	GtkCellRenderer *renderer;
	GtkWidget *Table,*Table2;
	GtkWidget *IO_frame,*B_frame,*C_frame,*Key_frame;
	GtkWidget *button,*open_button,*cfg_button,*add_button,*clear_button,*manager_button;
	GtkWidget *a_button,*b_button,*c_button;
	gchar *title = "Elinux debug tools";
	
	window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	
	gtk_window_set_title(GTK_WINDOW(window),title);
	
	//set windows width and hight
	gtk_widget_set_size_request(window,800,600);
	
	//set windows position
	gtk_window_set_position(GTK_WINDOW(window),GTK_WIN_POS_CENTER);
	
	//set border width	
	gtk_container_set_border_width(GTK_CONTAINER(window),BORDER_WIDTH);

	Table = gtk_table_new(10,10,FALSE);
	gtk_container_set_border_width(GTK_CONTAINER(Table),BORDER_WIDTH);
	gtk_table_set_row_spacings(GTK_TABLE(Table),BORDER_WIDTH);
	gtk_table_set_col_spacings(GTK_TABLE(Table),BORDER_WIDTH);
	gtk_container_add(GTK_CONTAINER(window),Table);

	/***********************Left Area********************************/
	IO_frame = gtk_frame_new("Output/input");
	gtk_table_attach(GTK_TABLE(Table), IO_frame, 
			0, 8, 0, 8, 
			GTK_FILL | GTK_EXPAND, 
			GTK_FILL | GTK_EXPAND, 
			1, 
			1);
	
	box = gtk_vbox_new(FALSE, 0);
	gtk_container_set_border_width (GTK_CONTAINER (box), BORDER_WIDTH);
	gtk_container_add (GTK_CONTAINER (IO_frame), box);

	TextArea = gtk_text_view_new();
	gtk_text_view_set_buffer(GTK_TEXT_VIEW(TextArea),g_buffer);
	gtk_text_view_set_editable(GTK_TEXT_VIEW(TextArea), FALSE);
	gtk_text_view_set_cursor_visible(GTK_TEXT_VIEW(TextArea),TRUE);
	g_buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW (TextArea));

	scrolledwin = gtk_scrolled_window_new(NULL,NULL);
	gtk_scrolled_window_set_policy(
			GTK_SCROLLED_WINDOW(scrolledwin),
		    GTK_POLICY_AUTOMATIC,
			GTK_POLICY_ALWAYS);
	
	gtk_box_pack_start(GTK_BOX(box),scrolledwin, TRUE, TRUE,0);
	gtk_container_add(GTK_CONTAINER(scrolledwin),TextArea);

	/********************Left Area end********************************/

	/**********************Right Area*********************************/
	C_frame = gtk_frame_new("Control");
	gtk_table_attach(GTK_TABLE(Table), C_frame, 
			8, 10, 0, 9, 
			GTK_FILL | GTK_EXPAND, 
			GTK_FILL | GTK_EXPAND, 
			1, 
			1);
	
	Table2 = gtk_table_new(8,2,FALSE);
	//gtk_container_set_border_width(GTK_CONTAINER(Table2),BORDER_WIDTH);
	//gtk_table_set_row_spacings(GTK_TABLE(Table2),BORDER_WIDTH);
	//gtk_table_set_col_spacings(GTK_TABLE(Table2),BORDER_WIDTH);
	gtk_container_add(GTK_CONTAINER(C_frame),Table2);



	uart_comboBox_port = gtk_combo_box_new_with_model(create_combo_Model(comboBox_port,COMB_PORT_LEN));
	gtk_combo_box_set_active(GTK_COMBO_BOX(uart_comboBox_port), 0);


	renderer = gtk_cell_renderer_text_new();
	gtk_cell_layout_pack_start(GTK_CELL_LAYOUT(uart_comboBox_port), renderer, FALSE);
	gtk_cell_layout_set_attributes(GTK_CELL_LAYOUT(uart_comboBox_port), renderer, "text", 0, NULL);    
	//g_signal_connect(GTK_OBJECT(dnl_comboBox), "changed",
	//			             G_CALLBACK(combo_changed), dnl_label);


	gtk_table_attach(GTK_TABLE(Table2), uart_comboBox_port, 
			0, 1, 0, 1, 
			GTK_FILL | GTK_EXPAND, 
			GTK_FILL, 
			1, 
			1);

	cfg_button = gtk_button_new_with_label("config");
	gtk_table_attach(GTK_TABLE(Table2), cfg_button, 
			1, 2, 0, 1, 
			GTK_FILL | GTK_EXPAND, 
			GTK_FILL, 
			1, 
			1);

	/********************************************************/

	uart_comboBox_baudrate = gtk_combo_box_new_with_model(create_combo_Model(comboBox_baudrate,COMB_BAUD_LEN));
	gtk_combo_box_set_active(GTK_COMBO_BOX(uart_comboBox_baudrate), 0);

	gtk_cell_layout_pack_start(GTK_CELL_LAYOUT(uart_comboBox_baudrate), renderer, FALSE);
	gtk_cell_layout_set_attributes(GTK_CELL_LAYOUT(uart_comboBox_baudrate), renderer, "text", 0, NULL);    
	//g_signal_connect(GTK_OBJECT(dnl_comboBox), "changed",
	//			             G_CALLBACK(combo_changed), dnl_label);
	gtk_table_attach(GTK_TABLE(Table2), uart_comboBox_baudrate, 
			0, 1, 1, 2, 
			GTK_FILL | GTK_EXPAND, 
			GTK_FILL, 
			1, 
			1);

	cfg_button = gtk_button_new_with_label("config");
	gtk_table_attach(GTK_TABLE(Table2), cfg_button, 
			1, 2, 1, 2, 
			GTK_FILL | GTK_EXPAND, 
			GTK_FILL, 
			1, 
			1);

	/********************************************************/
	uart_comboBox_bit = gtk_combo_box_new_with_model(create_combo_Model(comboBox_bit,COMB_BIT_LEN));
	gtk_combo_box_set_active(GTK_COMBO_BOX(uart_comboBox_bit), 0);

	gtk_cell_layout_pack_start(GTK_CELL_LAYOUT(uart_comboBox_bit), renderer, FALSE);
	gtk_cell_layout_set_attributes(GTK_CELL_LAYOUT(uart_comboBox_bit), renderer, "text", 0, NULL);    
	//g_signal_connect(GTK_OBJECT(dnl_comboBox), "changed",
	//			             G_CALLBACK(combo_changed), dnl_label);

	gtk_table_attach(GTK_TABLE(Table2), uart_comboBox_bit, 
			0, 1, 2, 3, 
			GTK_FILL | GTK_EXPAND, 
			GTK_FILL, 
			1, 
			1);

	cfg_button = gtk_button_new_with_label("config");
	gtk_table_attach(GTK_TABLE(Table2), cfg_button, 
			1, 2, 2, 3, 
			GTK_FILL | GTK_EXPAND, 
			GTK_FILL, 
			1, 
			1);

	/********************************************************/
	uart_comboBox_stop = gtk_combo_box_new_with_model(create_combo_Model(comboBox_stop,COMB_STOP_LEN));
	gtk_combo_box_set_active(GTK_COMBO_BOX(uart_comboBox_stop), 0);

	gtk_cell_layout_pack_start(GTK_CELL_LAYOUT(uart_comboBox_stop), renderer, FALSE);
	gtk_cell_layout_set_attributes(GTK_CELL_LAYOUT(uart_comboBox_stop), renderer, "text", 0, NULL);    
	//g_signal_connect(GTK_OBJECT(dnl_comboBox), "changed",
	//			             G_CALLBACK(combo_changed), dnl_label);
	gtk_table_attach(GTK_TABLE(Table2), uart_comboBox_stop, 
			0, 1, 3, 4, 
			GTK_FILL | GTK_EXPAND, 
			GTK_FILL, 
			1, 
			1);

	cfg_button = gtk_button_new_with_label("config");
	gtk_table_attach(GTK_TABLE(Table2), cfg_button, 
			1, 2, 3, 4, 
			GTK_FILL | GTK_EXPAND, 
			GTK_FILL, 
			1, 
			1);

	/********************************************************/
	uart_comboBox_hd_ctl = gtk_combo_box_new_with_model(create_combo_Model(comboBox_hd_ctl,COMB_HD_LEN));
	gtk_combo_box_set_active(GTK_COMBO_BOX(uart_comboBox_hd_ctl), 0);

	gtk_cell_layout_pack_start(GTK_CELL_LAYOUT(uart_comboBox_hd_ctl), renderer, FALSE);
	gtk_cell_layout_set_attributes(GTK_CELL_LAYOUT(uart_comboBox_hd_ctl), renderer, "text", 0, NULL);    
	//g_signal_connect(GTK_OBJECT(dnl_comboBox), "changed",
	//			             G_CALLBACK(combo_changed), dnl_label);
	gtk_table_attach(GTK_TABLE(Table2), uart_comboBox_hd_ctl, 
			0, 1, 4, 5, 
			GTK_FILL | GTK_EXPAND, 
			GTK_FILL, 
			1, 
			1);

	cfg_button = gtk_button_new_with_label("config");
	gtk_table_attach(GTK_TABLE(Table2), cfg_button, 
			1, 2, 4, 5, 
			GTK_FILL | GTK_EXPAND, 
			GTK_FILL, 
			1, 
			1);
#if 0	
	
	open_button = gtk_button_new_with_label("Open");
	gtk_table_attach(GTK_TABLE(Table2), open_button, 
			0, 1, 0, 1, 
			GTK_FILL | GTK_EXPAND, 
			GTK_FILL, 
			1, 
			1);
	
	cfg_button = gtk_button_new_with_label("configure");
	gtk_table_attach(GTK_TABLE(Table2), cfg_button, 
			1, 2, 0, 1, 
			GTK_FILL | GTK_EXPAND, 
			GTK_FILL, 
			1, 
			1);
#endif
#if 0
	box = gtk_vbox_new(FALSE, 0);
	gtk_container_set_border_width (GTK_CONTAINER (box), BORDER_WIDTH);
	gtk_container_add (GTK_CONTAINER (C_frame), box);

	open_button = gtk_button_new_with_label("Open");
	gtk_box_pack_start(GTK_BOX(box),open_button,FALSE,FALSE,3);
	g_signal_connect(G_OBJECT(open_button),"clicked",
			G_CALLBACK(open_button_clicked),(gpointer)count);

	cfg_button = gtk_button_new_with_label("configure");
	gtk_box_pack_start(GTK_BOX(box),cfg_button,FALSE,FALSE,3);
	g_signal_connect(G_OBJECT(cfg_button),"clicked",
			G_CALLBACK(cfg_button_clicked),(gpointer)count);

	add_button = gtk_button_new_with_label("add");
	gtk_box_pack_start(GTK_BOX(box),add_button,FALSE,FALSE,3);
	g_signal_connect(G_OBJECT(add_button),"clicked",
			G_CALLBACK(add_button_clicked),(gpointer)count);

	clear_button = gtk_button_new_with_label("clear");
	gtk_box_pack_start(GTK_BOX(box),clear_button,FALSE,FALSE,3);
	g_signal_connect(G_OBJECT(clear_button),"clicked",
			G_CALLBACK(clear_button_clicked),(gpointer)count);

	manager_button = gtk_button_new_with_label("manager");
	gtk_box_pack_start(GTK_BOX(box),manager_button,FALSE,FALSE,3);
	g_signal_connect(G_OBJECT(manager_button),"clicked",
			G_CALLBACK(manager_button_clicked),(gpointer)count);
	/********************Right Area1 End********************************/
	box2 = gtk_vbox_new(FALSE, 0);
	gtk_container_set_border_width (GTK_CONTAINER (box2), BORDER_WIDTH);
	gtk_container_add (GTK_CONTAINER (C_frame), box2);

	a_button = gtk_button_new_with_label("Open1");
	gtk_box_pack_start(GTK_BOX(box2),open_button,FALSE,FALSE,3);

	b_button = gtk_button_new_with_label("confi1");
	gtk_box_pack_start(GTK_BOX(box2),cfg_button,FALSE,FALSE,3);

	b_button = gtk_button_new_with_label("add1");
	gtk_box_pack_start(GTK_BOX(box2),add_button,FALSE,FALSE,3);

#endif	
	/********************Right Area2 End********************************/

	/**********************Button Area 1 *******************************/	
	Key_frame = gtk_frame_new("Key Input");
	gtk_table_attach(GTK_TABLE(Table), Key_frame, 
			0, 8, 8, 9, 
			GTK_FILL | GTK_EXPAND, 
			GTK_FILL, 
			1, 
			1);
	
	box = gtk_hbox_new(FALSE, 0);
	gtk_container_set_border_width (GTK_CONTAINER (box), BORDER_WIDTH);
	gtk_container_add(GTK_CONTAINER (Key_frame), box);

	Kitem = self_item_new();

	gtk_box_pack_start(GTK_BOX(box),Kitem,TRUE,TRUE,1);
	/*******************Button Area 1 End ******************************/
	
	/**********************Button Area 2 *******************************/
	B_frame = gtk_frame_new("Configure");
	gtk_table_attach(GTK_TABLE(Table), B_frame, 
			0, 10, 9, 10, 
			GTK_FILL | GTK_EXPAND, 
			GTK_FILL, 
			1, 
			1);
	
	box = gtk_hbox_new(FALSE, 0);
	gtk_container_set_border_width (GTK_CONTAINER(box), BORDER_WIDTH);
	gtk_container_add(GTK_CONTAINER (B_frame), box);

	button = gtk_button_new_with_label("ATK Tools");
    g_signal_connect_swapped(G_OBJECT(button), 
			"clicked",
			G_CALLBACK(debug_system_button_clicked),
			NULL);
	gtk_box_pack_start(GTK_BOX(box), button, FALSE, FALSE, 0);


	button = gtk_button_new_with_label("close");
    g_signal_connect_swapped(G_OBJECT(button), 
			"clicked",
			G_CALLBACK(on_close_win_event),
			NULL);
	gtk_box_pack_end(GTK_BOX(box), button, FALSE, FALSE, 0);
	/*******************Button Area 2 End ******************************/

	gtk_widget_show_all(window);

	return window;
}

static GtkTreeModel *create_and_fill_model(void)
{
	GtkTreeStore *treestore;
	GtkTreeIter toplevel, child;
	treestore = gtk_tree_store_new(NUM_COLS, G_TYPE_STRING);

	/* first tree */
	gtk_tree_store_append(treestore, &toplevel, NULL);
	gtk_tree_store_set(treestore, &toplevel,COLUMN, "Scripting languages", -1);
	gtk_tree_store_append(treestore, &child, &toplevel);
	gtk_tree_store_set(treestore, &child, COLUMN, "Python", -1);
	gtk_tree_store_append(treestore, &child, &toplevel);
	gtk_tree_store_set(treestore, &child, COLUMN, "Perl",-1);
	gtk_tree_store_append(treestore, &child, &toplevel);
	gtk_tree_store_set(treestore, &child, COLUMN, "PHP",-1);

	/* second tree*/
	gtk_tree_store_append(treestore, &toplevel, NULL);
	gtk_tree_store_set(treestore, &toplevel, COLUMN, "Compiled languages",-1);
	gtk_tree_store_append(treestore, &child, &toplevel);
	gtk_tree_store_set(treestore, &child, COLUMN, "C",-1);
	gtk_tree_store_append(treestore, &child, &toplevel);
	gtk_tree_store_set(treestore, &child, COLUMN, "C++",-1);
	gtk_tree_store_append(treestore, &child, &toplevel);
	gtk_tree_store_set(treestore, &child, COLUMN, "Java",-1);
	
	return GTK_TREE_MODEL(treestore);
}


static GtkWidget *create_view_and_model (void)
{
	GtkTreeViewColumn *col;
	GtkCellRenderer *renderer;
	GtkWidget *view;
	GtkTreeModel *model;
	view = gtk_tree_view_new();
	col = gtk_tree_view_column_new();
	gtk_tree_view_column_set_title(col, "Programming languages");
	gtk_tree_view_append_column(GTK_TREE_VIEW(view), col);
	renderer = gtk_cell_renderer_text_new();
	gtk_tree_view_column_pack_start(col, renderer, TRUE);
	gtk_tree_view_column_add_attribute(col, renderer, "text", COLUMN);
	model = create_and_fill_model();
	gtk_tree_view_set_model(GTK_TREE_VIEW(view), model);
	g_object_unref(model);

	/* double click event */
	g_signal_connect(view, "row-activated", (GCallback)view_onRowActivated, NULL);

	/* right key press event */	
	g_signal_connect(view, "button-press-event", (GCallback)view_onButtonPressed, NULL);

	/* popup-menu event */	
	g_signal_connect(view, "popup-menu", (GCallback)view_onPopupMenu, NULL);

	return view;
}


GtkWidget *create_sub_window(void)
{
	GtkWidget *window;
	gchar *title = "Elinux Manager tools";
	GtkWidget *view;
	GtkWidget *vbox;
	GtkTreeSelection *selection;
	GtkWidget *statusbar;

	
	window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	
	gtk_window_set_title(GTK_WINDOW(window),title);
	
	//set windows width and hight
	gtk_widget_set_size_request(window, 350, 300);
	
	gtk_window_set_resizable(GTK_WINDOW(window), FALSE);

	//set windows position
	gtk_window_set_position(GTK_WINDOW(window),GTK_WIN_POS_CENTER);
	
	//set border width	
	gtk_container_set_border_width(GTK_CONTAINER(window),BORDER_WIDTH);

	vbox = gtk_vbox_new(FALSE, 2);
	gtk_container_add(GTK_CONTAINER(window), vbox);

	view = create_view_and_model();
	selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(view));
	gtk_box_pack_start(GTK_BOX(vbox), view, TRUE, TRUE, 1);

	statusbar = gtk_statusbar_new();
	gtk_box_pack_start(GTK_BOX(vbox), statusbar, FALSE, TRUE, 1);
	g_signal_connect(selection, "changed", G_CALLBACK(on_changed),statusbar);

	return window;
}


GtkTreeModel *createModel()
{
    GtkTreeIter iter;
    GtkListStore *store;
    gint i;
    
    store = gtk_list_store_new(1, G_TYPE_STRING);
   
    for(i = 0; i < COMM_LEN; i++)
    {
        gtk_list_store_append(store, &iter);
        gtk_list_store_set(store, &iter, 0, comm[i], -1);
    }
    
    return GTK_TREE_MODEL(store);
}  

GtkWidget *create_dbg_sys_window(void)
{
	GtkWidget *window;
	GtkWidget *vbox;	
	GtkWidget *hbox;
    GtkCellRenderer *renderer;
	GtkWidget *button;
	gchar *title = "HFC Debug System";
	
	window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	
	gtk_window_set_title(GTK_WINDOW(window),title);
	
	//set windows width and hight
	gtk_widget_set_size_request(window, 400, 230);
	
	gtk_window_set_resizable(GTK_WINDOW(window), FALSE);

	//set windows position
	gtk_window_set_position(GTK_WINDOW(window),GTK_WIN_POS_CENTER);
	
	//set border width	
	gtk_container_set_border_width(GTK_CONTAINER(window),BORDER_WIDTH);

	vbox = gtk_vbox_new(FALSE, 0);
	gtk_container_set_border_width(GTK_CONTAINER (vbox), BORDER_WIDTH);
	gtk_container_add(GTK_CONTAINER (window), vbox);

	dnl_label = gtk_label_new("UART: no connect");
	gtk_label_set_justify(GTK_LABEL(dnl_label),GTK_JUSTIFY_CENTER);
	
	gtk_box_pack_start(GTK_BOX(vbox),dnl_label, FALSE,TRUE,3);
	
	hbox = gtk_hbox_new(TRUE, 0);
	gtk_container_set_border_width (GTK_CONTAINER(hbox), BORDER_WIDTH);

    dnl_comboBox = gtk_combo_box_new_with_model(createModel());
    gtk_combo_box_set_active(GTK_COMBO_BOX(dnl_comboBox), 0);

    renderer = gtk_cell_renderer_text_new();
    gtk_cell_layout_pack_start(GTK_CELL_LAYOUT(dnl_comboBox), renderer, FALSE);
    gtk_cell_layout_set_attributes(GTK_CELL_LAYOUT(dnl_comboBox), renderer, "text", 0, NULL);    

	button = gtk_button_new_with_label("Connect");
    
	gtk_box_pack_start(GTK_BOX(hbox),dnl_comboBox, FALSE,TRUE,3);
	gtk_box_pack_start(GTK_BOX(hbox),button,FALSE,TRUE,3);
	
	g_signal_connect(G_OBJECT(button),"clicked",
			G_CALLBACK(connect_uart_button_clicked),(gpointer)count);
    g_signal_connect(GTK_OBJECT(dnl_comboBox), "changed",
			                     G_CALLBACK(combo_changed), dnl_label);


	gtk_box_pack_start(GTK_BOX(vbox),hbox, FALSE,TRUE,3);
	
	dnl_path = gtk_entry_new();
	gtk_entry_set_text(GTK_ENTRY(dnl_path),DEFAULT_DOWN_FILE);
	gtk_box_pack_start(GTK_BOX(vbox),dnl_path,FALSE,FALSE,3);
	
	button = gtk_button_new_with_label("Load");
	gtk_box_pack_start(GTK_BOX(vbox),button,FALSE,FALSE,3);
	g_signal_connect(G_OBJECT(button),"clicked",
			G_CALLBACK(file_open_button_clicked),(gpointer)count);
	
	prcss_bar = gtk_progress_bar_new();
	gtk_progress_bar_set_text(GTK_PROGRESS_BAR(prcss_bar),"Progress");
	gtk_box_pack_start(GTK_BOX(vbox),prcss_bar,FALSE,FALSE,3);

	button = gtk_button_new_with_label("Download");
	gtk_box_pack_start(GTK_BOX(vbox),button,FALSE,FALSE,3);
	g_signal_connect(G_OBJECT(button),"clicked",
			G_CALLBACK(start_dnl_button_clicked),(gpointer)count);

	return window;
}



