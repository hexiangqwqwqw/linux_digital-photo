#include <config.h>
#include <input_manager.h>
#include <string.h>

static PT_input_opr g_pt_input_opr_head;

int register_input_opr(PT_input_opr pt_input_opr)
{
	PT_input_opr pt_tmp;

	if(!g_pt_input_opr_head)
	{
		g_pt_input_opr_head = pt_input_opr;
		pt_input_opr->pt_next = NULL;
	}
	else
	{
		pt_tmp = g_pt_input_opr_head;
		while(pt_tmp->pt_next)
		{
			pt_tmp= pt_tmp->pt_next;
		}
		pt_tmp->pt_next = pt_input_opr;
		pt_input_opr->pt_next = NULL;
	}

	return 0;
	
}

void show_input_opr(void)
{
	int i = 0;
	PT_input_opr pt_tmp = g_pt_input_opr_head;
	while(pt_tmp)
	{
		printf("%02d %s\n", i++, pt_tmp->name);
		pt_tmp = pt_tmp->pt_next;
	}
}

int all_input_devices_init(void)
{
	PT_input_opr pt_tmp = g_pt_input_opr_head;
	int i_error = -1;
	
	while(pt_tmp)
	{
		if(pt_tmp->device_init() == 0)
		{
			i_error = 0;
		}
		pt_tmp = pt_tmp->pt_next;
	}
	return i_error;
}

int get_input_event(PT_input_event pt_input_event)
{
	PT_input_opr pt_tmp = g_pt_input_opr_head;

	while(pt_tmp)
	{
		if(pt_tmp->get_input_event(pt_input_event) == 0)
		{
			return 0;
		}
		pt_tmp = pt_tmp->pt_next;
	}
	return -1;
}

int input_init(void)
{
	int i_error;
	i_error = stdin_init();
	i_error |= touch_screen_init();
	return i_error;
}



