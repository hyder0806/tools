
#ifndef __USB_H__

#define __USB_H__

extern int hfc_usb_close(void);
extern int hfc_usb_init(void);
extern int hfc_usb_send_report_via_control(void);
extern int hfc_usb_send_disconnect_package(void);
extern int hfc_usb_connect(void);
extern int hfc_usb_disconnect(void);

#endif
