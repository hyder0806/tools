#include "xtcp.h"
#include "buff.h"
#include "uart.h"
#include "callbacks.h"
#include "self_item.h"
#include "res_manager.h"
#include "task.h"
#include "ttask.h"
#include "atk_dl.h"

extern GtkWidget *dnl_label;
extern GtkWidget *dnl_comboBox;
extern GtkWidget *TextArea;
extern GtkWidget *dnl_path;
extern GtkTextBuffer *g_buffer;
extern GtkWidget *Kitem;
extern GtkWidget *prcss_bar;	//process_bar
GtkTextIter Tstart, Tend;
GtkTextMark *Imark;

void deal_textarea(int signo){
	g_print("Catch a signal -- SIGALRM /n");
}

int received;
int screen_update = 1;
int dnl_update_system = 0;
static int hfc_sys_out = 0;
static int ms5_flag, ucTick10ms, ucTick100ms, ucTick1s, ucTick10s;
int dbg_uart_fd = 0;
double p_value = 0;
gchar *filename = NULL;
extern int security;
extern int atk_download_time_out; 

GMainLoop* loop;
GIOChannel* channel;
GSource* source;
GMainContext *context;


void uart_thread(void);
void dnl_process_update(void);


#if 0
{ 
	int ret;
	int len;
	int dWriteCnt,dReadLen;

	ret = atk_connect_board();
	if(ret)
	{
		g_print("Could not connect CPU\n");
		return;
	}

	dnl_fd = open(filename,O_RDONLY);
	{
		g_print("Could not open the file\n");
		return;
	}

	//send command addr


	dWriteCnt = 0;
	do
	{
		dReadLen = fread(aucReadBuff,1,sizeof(aucReadBuff),fp);
		/*
		while(serial_GetRcvBuffFreeSize() <= sizeof(aucReadBuff))
		{
			Sleep(100);
		}
		*/
		serial_write(aucReadBuff, dReadLen);
		dWriteCnt += dReadLen;

		dCurPercent = (dWriteCnt*100/dFileLen);
		if( dCurPercent > dPrevPercent )
		{
			dPrevPercent =  dCurPercent;
			printf("\r");
			printf("%d%%",dCurPercent);
		}
	}while(dReadLen == sizeof(aucReadBuff));
	len = read(dnl_fd,);


#if 0
	GString *text;

	p_value += 0.01;
	
	text = g_string_new("");
	g_string_sprintf(text, "%d %%", (int)(p_value * 100));
	
	
	gtk_progress_bar_set_text(GTK_PROGRESS_BAR(prcss_bar),text->str);
	gtk_progress_bar_set_fraction(GTK_PROGRESS_BAR(prcss_bar),p_value);
#endif
}
#endif

void scroll_mark_onscreen(void)
{
	Imark = gtk_text_buffer_get_insert(g_buffer);
	gtk_text_view_scroll_mark_onscreen(GTK_TEXT_VIEW(TextArea), Imark);
}


gboolean callback(GIOChannel *channel, GIOCondition condition)
{
    gchar ch;
    gsize length;
	GError *error=NULL;
    
	g_io_channel_read_chars(channel, &ch, 1, &length, &error);
	if(length > 0)
	{
		buff_inb(&cbuff,ch);
		//g_print("%x ",ch);
	}
   
	return TRUE;
}

static void sys_clk(void)   //5ms cycle
{
    ms5_flag = 1;
    if(ucTick10ms==0)
    {
      ucTick10ms=2;
      //tcp.tick();
      if(ucTick100ms==0)
      {
        ucTick100ms=9;
        if(ucTick1s==0)
        {
            ucTick1s=9;
			atk_download_task_clk();
			if(ucTick10s==0)
            {
                ucTick10s=9;              
            }else{
              ucTick10s--;
            }
        }else{
          ucTick1s--;
        }
      }else{
        ucTick100ms--;
      }
    }else{
      ucTick10ms--;
    }
}

unsigned char display_update(void)
{
    gchar buff[256];

	gsize length;

    length = buff_dlen(&cbuff);
    if(!length)
		return 1;

	gdk_threads_enter();
	memset(buff,'\0',strlen(buff));
	buff_out(&cbuff, buff, length);

	gtk_text_buffer_get_end_iter(g_buffer, &Tend);
	gtk_text_buffer_insert(g_buffer, &Tend, buff, length);
	scroll_mark_onscreen();
	gdk_threads_leave();

	return 0;
}

static gboolean timeout_func(gpointer data)
{
	sys_clk();

	if(screen_update)
	{
		//display_update();
	}
		
	dnl_process_update();

	task_queue_schedule();
	
	if(hfc_sys_out)
	{
		return FALSE;
	}
	else{
		return TRUE;
	}
}


void uart_thread(void)
{
	int ret;

		g_print("Uart initilize..\n");
		ret = dev_uart_open(DBG_PORT);
    	if(ret != 0)
    	{
    	    g_print("open uart error:%d\n",ret);
    	    return;
    	}

		dbg_uart_fd = dev_uart_ioctl(DBG_PORT,NULL,GET_UART_FD);
		if(dbg_uart_fd > 0)
		{
			channel = g_io_channel_unix_new(dbg_uart_fd);
			//g_io_channel_init(channel);
			source = g_io_create_watch(channel, G_IO_IN);
			g_io_channel_set_encoding (channel, NULL, NULL);
			g_io_channel_unref(channel);
			g_source_set_callback(source, (GSourceFunc)callback, channel, NULL);

			context = g_main_context_new();
			g_source_attach(source, context);
			g_source_unref(source);
		
			loop = g_main_loop_new(context, FALSE);
			
			g_timeout_add(5, timeout_func, loop);
			
			g_main_loop_run(loop);
			g_main_context_unref(context);
		}
}

void open_button_clicked(GtkWidget *button,gpointer userdata)
{
	int ret;
	const gchar *button_label;

	button_label = gtk_button_get_label(GTK_BUTTON(button));
	if(strncmp("Open",button_label,4) == 0)
	{
		buff_init(&cbuff,0);	//init the buf and clear it 
		//task_queue_init();

		gtk_button_set_label(GTK_BUTTON(button),"Close");
		g_thread_create((GThreadFunc)uart_thread,NULL,FALSE,NULL);
	}
	else if(strncmp("Close",button_label,5) == 0)
	{
		received = FALSE;	
		dbg_uart_fd = dev_uart_ioctl(DBG_PORT,NULL,GET_UART_FD);
		if(dbg_uart_fd > 0)
		{
			dev_uart_close(DBG_PORT);
			g_print("Close Uart\n");
			gtk_button_set_label(GTK_BUTTON(button),"Open");
		}
	}
}

void connect_usb_button_clicked(GtkWidget *button,gpointer userdata)
{
	GtkTreeIter iter;
	gchar *com_active;
	GtkTreeModel *model;

	model = gtk_combo_box_get_model(GTK_COMBO_BOX(dnl_comboBox));
	gtk_combo_box_get_active_iter(GTK_COMBO_BOX(dnl_comboBox), &iter);
	gtk_tree_model_get(model, &iter, 0, &com_active, -1);

	g_print("select:%s\n",com_active);

	if(strncmp(com_active,UART_PORT,UART_PORT_LEN))
	{
	}
	else if(strncmp(com_active,USB_PORT,USB_PORT_LEN))
	{
	}
		
}

void connect_uart_button_clicked(GtkWidget *button,gpointer userdata)
{
	GtkTreeIter iter;
	gchar *com_active;
	GtkTreeModel *model;

	model = gtk_combo_box_get_model(GTK_COMBO_BOX(dnl_comboBox));
	gtk_combo_box_get_active_iter(GTK_COMBO_BOX(dnl_comboBox), &iter);
	gtk_tree_model_get(model, &iter, 0, &com_active, -1);

	g_print("select:%s\n",com_active);

	if(0 == security)
	{
		if(!strncmp(com_active,UART_PORT,UART_PORT_LEN))
		{
			atk_download_task_init();
		}
		else if(!strncmp(com_active,USB_PORT,USB_PORT_LEN))
		{
			;
		}
	}
	else
	{
		gtk_label_set_text(GTK_LABEL(dnl_label), "connected");
	}
}

gboolean combo_changed(GtkComboBox *comboBox, GtkLabel *dnl_label)
{
	GtkTreeModel *model = gtk_combo_box_get_model(comboBox);
	GtkTreeIter iter;
	gchar *active;

	g_print("change\n");
	gtk_combo_box_get_active_iter(comboBox, &iter);
	gtk_tree_model_get(model, &iter, 0, &active, -1);
	gtk_label_set_text(dnl_label, active);
}


void dnl_process_update(void)
{
	GString *text;

	if(dnl_update_system)
	{
		//p_value += 0.01;
		if(1 == p_value)
		{
			dnl_update_system = 0;
		}
		
		text = g_string_new("");
		g_string_sprintf(text, "%d %%", (int)(p_value * 100));
		
		gtk_progress_bar_set_text(GTK_PROGRESS_BAR(prcss_bar),text->str);
		gtk_progress_bar_set_fraction(GTK_PROGRESS_BAR(prcss_bar),p_value);
	}
}

void start_dnl_button_clicked(GtkWidget *button,gpointer userdata)
{
	if(security)
	{
		atk_download_task_start(security);	
		dnl_update_system = 1;
		security = 0;
	}
}


void file_open_button_clicked(GtkWidget *button,gpointer userdata)
{
	GtkWidget *dialog;
	gchar *content;
	gsize bytes;
	GError *error = NULL;
	GtkTextBuffer *buffer;
	
	dialog = gtk_file_chooser_dialog_new(
			"Open File",
			NULL,
			GTK_FILE_CHOOSER_ACTION_OPEN,
			GTK_STOCK_CANCEL,
			GTK_RESPONSE_CANCEL,
			GTK_STOCK_OPEN,
			GTK_RESPONSE_ACCEPT,
			NULL);

	if (gtk_dialog_run(GTK_DIALOG(dialog)) == GTK_RESPONSE_ACCEPT)
	{
		filename = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(dialog));
		g_file_get_contents(filename, &content, &bytes, &error);
		if (error != NULL) {
			g_printf(error->message);
			g_clear_error(&error);
		}
		gtk_entry_set_text(GTK_ENTRY(dnl_path),filename);
		g_free(filename);
	}
	gtk_widget_destroy(dialog);
}

int dis = 1;
void cfg_button_clicked(GtkWidget *button,gpointer userdata)
{
//	g_buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW (TextArea));
//	gtk_text_buffer_set_text(g_buffer, "Hello Text View!", -1);
	if(dis == 1)
	{
		dis = 0;
		task_queue_init();
	//	dev_uart_open(DBG_PORT);
	}
	else
	{
		task_queue_display();
	}
}

extern unsigned char atk_connect_status(TASK *task);
extern int atk_download_file_init(void);
void add_button_clicked(GtkWidget *button,gpointer userdata)
{
	atk_download_task_init();
}

void clear_button_clicked(GtkWidget *button,gpointer userdata)
{
}

void manager_button_clicked(GtkWidget *button,gpointer userdata)
{
}


void debug_system_button_clicked(GtkWidget *button,gpointer userdata)
{
	if(dbg_ishide == FALSE)
	{
		gtk_widget_show_all(dbg_window);
		dbg_ishide = TRUE;
	}else{
		gtk_widget_hide(dbg_window);
		dbg_ishide = FALSE;
	}
}

//以下函数为自定义控件ouritem的"ouritem_ok"信号的回调函数
void on_item_ok(GtkWidget *widget, GtkWidget *entry)
{
	unsigned char *enter_string = NULL;
	int ret;

	gtk_text_buffer_get_end_iter(g_buffer, &Tend);

	enter_string = gtk_entry_get_text(GTK_ENTRY(entry));

	enter_string = g_strconcat(enter_string, "\n", NULL);

	ret = dev_uart_ioctl(DBG_PORT,NULL,GET_UART_FD);
	if(ret > 0)
	{
		dev_uart_write(DBG_PORT,enter_string,strlen(enter_string));
	}

	gtk_entry_set_text(GTK_ENTRY(entry),"");
	scroll_mark_onscreen();
}


void on_close_win_event(GtkWidget *widget,GdkEvent *event,gpointer data)
{
	int ret;

	hfc_sys_out = 1;
	ret = dev_uart_ioctl(DBG_PORT,NULL,GET_UART_FD);
	if(ret > 0)
	{
		received = FALSE;	
		dev_uart_close(DBG_PORT);
		g_print("Close Uart\n");
	}
	task_queue_destroy();
	gtk_main_quit();
}

	
