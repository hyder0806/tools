
#ifndef __OUR_ITEM_H__
#define __OUR_ITEM_H__

#include <gtk/gtk.h>
//定义类型宏和转换宏
#define TYPE_OUR_ITEM	(our_item_get_type())
#define OUR_ITEM(obj)	(GTK_CHECK_CAST(obj,TYPE_OUR_ITEM,OurItem))
//定义实例结构和类结构
typedef struct _OurItem OurItem;
typedef struct _OurItemClass OurItemClass;

struct _OurItem {
	GtkHBox parent;	//父控件为横向盒状容器
	GtkWidget *entry;	//单行文本录入
};

struct _OurItemClass {
	GtkHBoxClass parent_class;
	//下面定义函数指针，为所有OurItem实例所使用，即所有的OurItem控件在输入信息点击回车键后都执行此函数
	void (*enter_ok)(void);
};

GtkType our_item_get_type(void);
GtkWidget *our_item_new(void);
GtkWidget *self_item_new(void);
void on_item_ok(GtkWidget *widget, GtkWidget *entry);

#endif //__OUR_ITEM_H__
