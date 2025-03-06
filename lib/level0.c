#include <zephyr/zbus/zbus.h>

struct level0_msg{
	int a;
	int b;
};



ZBUS_CHAN_DEFINE(meshtastic_level0_chan,       /* Name */
		 struct level0_msg, /* Message type */

		 NULL,                 /* Validator */
		 NULL,                 /* User data */
		 ZBUS_OBSERVERS_EMPTY, /* observers */
		 ZBUS_MSG_INIT(.a = 0, .b = 1)
);
