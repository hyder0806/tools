
#include "main.h"
#include "self_item.h"


//定义枚举类型，说明信号的名称和次序
enum {
	OURITEM_OK_SIGNAL,
	LAST_SIGNAL
};

gint emit_flag = 0;

static gint ouritem_signals[LAST_SIGNAL] = { 0 };

static void our_item_init(OurItem *ouritem);
static void our_item_class_init(OurItemClass *ouritemclass);

extern GtkWidget *Kitem;

//下面定义函数指针，为所有OurItem实例所使用，即所有的OurItem控件在输入信息点击回车键后都执行此函数,暂时未实现
static void enter_ok(void);

void on_key_release(GtkWidget *entry, GdkEventKey *event, gpointer data);

//注册自定义控件
GtkType	our_item_get_type(void)
{
	static GtkType our_item_type = 0;
	if(!our_item_type)
	{
		GtkTypeInfo our_item_info = {
			"OurItem",	//控件名
			sizeof(OurItem),	//控件实例的尺寸
			sizeof(OurItemClass),	//控件类的尺寸
			(GtkClassInitFunc)our_item_class_init,	//控件类初始化函数
			(GtkObjectInitFunc)our_item_init,	//控件实例初始化函数
			NULL, //
			NULL //
		};
		our_item_type = gtk_type_unique(GTK_TYPE_HBOX, &our_item_info);//注册此控件
	}
	return our_item_type;
}

//初始化实例结构
static void our_item_init(OurItem *ouritem)
{
	ouritem->entry = gtk_entry_new();
	g_signal_connect(G_OBJECT(ouritem->entry),"key_release_event",
			G_CALLBACK(on_key_release),ouritem);
	gtk_box_pack_start(GTK_BOX(ouritem),ouritem->entry,TRUE,TRUE,2);
}

//初始化类结构
static void our_item_class_init(OurItemClass *ouritemclass)
{
	GtkObjectClass *object_class;
	object_class = (GtkObjectClass*)ouritemclass;
	//下面函数创建一个新的信号
	ouritem_signals[OURITEM_OK_SIGNAL] = g_signal_new("ouritem_ok",
					G_TYPE_FROM_CLASS(object_class),
					G_SIGNAL_RUN_FIRST,
					0,//G_STRUCT_OFFSET(OurItemClass, enter_ok),
					NULL,NULL,
					g_cclosure_marshal_VOID__VOID,
					G_TYPE_NONE, 0, NULL);
	ouritemclass->enter_ok = NULL;//enter_ok;
}

//创建新的自定义控件
GtkWidget* our_item_new(void)
{
	return GTK_WIDGET(g_object_new(TYPE_OUR_ITEM,0));
}

//带参数创建自定义控件
GtkWidget* self_item_new(void)
{
	GtkWidget* item;
	item = our_item_new();
	return item;
}

static void signal_install(GtkWidget *entry)
{
	if(!emit_flag)
	{
		g_signal_connect(G_OBJECT(Kitem),"ouritem_ok",G_CALLBACK(on_item_ok),entry);
		emit_flag = 1;		
	}
}

//以下函数捕获键盘输入消息
void on_key_release(GtkWidget *entry, GdkEventKey *event, gpointer data)
{
	signal_install(entry);
	if(event->keyval == GDK_Return)	//当按下回车键后发射自定义的信号
	{
		g_signal_emit(G_OBJECT(data),ouritem_signals[OURITEM_OK_SIGNAL],0);
	}
}
