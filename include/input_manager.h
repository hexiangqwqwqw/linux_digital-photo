#ifndef _INPUT_MANAGER_H
#define _INPUT_MANAGER_H
#include <sys/time.h>


#define INPUT_TYPE_STDIN		0
#define INPUT_TYPE_TOUCHSCREEN	1

#define INPUT_VALUE_UP			0
#define INPUT_VALUE_DOWN		1
#define INPUT_VALUE_EXIT		2
#define INPUT_VALUE_UNKNOWN		-1

typedef struct input_event {
	struct timeval t_time;
	int i_type;
	int i_val;
}T_input_event, *PT_input_event;

typedef struct input_opr {
	char  *name;
	int (*device_init)(void);
	int (*device_exit)(void);
	int (*get_input_event)(PT_input_event pt_input_event);
	struct input_opr *pt_next;
}T_input_opr, *PT_input_opr;

int input_init(void);
int register_input_opr(PT_input_opr pt_input_opr);
void show_input_opr(void);
int all_input_devices_init(void);
int get_input_event(PT_input_event pt_input_event);
int stdin_init(void);
int touch_screen_init(void);


#endif

