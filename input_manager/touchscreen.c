#include <config.h>
#include <input_manager.h>
#include <stdlib.h>
#include <draw.h>
#include <tslib.h>

/* 参考tslib里的ts_print.c */

static struct tsdev *g_t_tsdev;
static int gi_xres;
static int gi_yres;

/* 注意: 由于要用到LCD的分辨率, 此函数要在SelectAndInitDisplay之后调用 */
static int touch_screendev_init(void)
{
	char *pc_tsname = NULL;
	if((pc_tsname = getenv("TSLIB_TSDEVICE")) != NULL)
	{
		g_t_tsdev = ts_open(pc_tsname, 0);		/*以阻塞的方式打开*/
	}
	else
	{
		g_t_tsdev = ts_open("/dev/event0", 1);
	}
	if(!g_t_tsdev)
	{
		DBG_PRINTF("ts_open error!\n");
		return -1;
	}

	if(ts_config(g_t_tsdev))
	{
		DBG_PRINTF("ts_config error!\n");
		return -1;
	}

	if(get_disp_resolution(&gi_xres, &gi_yres))
	{
		return -1;
	}

	return 0;
}

static int touch_screendev_exit()
{
	return 0;
}

static int is_outof_500ms(struct timeval *pt_pre_time, struct timeval *pt_now_time)
{
	int i_pre_ms;
	int i_now_ms;

	i_pre_ms = pt_pre_time->tv_sec*1000 + pt_pre_time->tv_usec/1000;
	i_now_ms = pt_now_time->tv_sec*1000 + pt_now_time->tv_usec/1000;

	return (i_now_ms > i_pre_ms + 500);
}

static int touch_screendev_get_inputevent(PT_input_event pt_input_event)
{
	struct ts_sample t_samp;
	int i_ret;

	static struct timeval t_pre_time;
	i_ret = ts_read(g_t_tsdev, &t_samp, 1); 	/*无数据，休眠*/

	if(i_ret < 0)
	{
		return -1;
	}

	/*数据处理*/
	if(is_outof_500ms(&t_pre_time, &t_samp.tv))
	{
		t_pre_time = t_samp.tv;
		pt_input_event->t_time = t_samp.tv;
		pt_input_event->i_type = INPUT_TYPE_TOUCHSCREEN;

		if(t_samp.y < gi_yres/3)
		{
			pt_input_event->i_val = INPUT_VALUE_UP;
		}
		else if(t_samp.y > 2*gi_yres/3)
		{
			pt_input_event->i_val = INPUT_VALUE_DOWN;
		}
		else
		{
			pt_input_event->i_val = INPUT_VALUE_UNKNOWN;
		}
		return 0;
	}
	else
	{
		return -1;
	}
	return 0;
	
}

static T_input_opr g_t_touchscreen_opr = {
	.name				= "touchscreen",
	.device_init		= touch_screendev_init,
	.device_exit		= touch_screendev_exit,
	.get_input_event	= touch_screendev_get_inputevent,
};

int touch_screen_init(void)
{
	return register_input_opr(&g_t_touchscreen_opr);
}

