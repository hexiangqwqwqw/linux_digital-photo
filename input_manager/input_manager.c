#include <config.h>
#include <input_manager.h>
#include <string.h>

static PT_input_opr g_pt_input_opr_head;
static T_input_event g_tInputEvent;

static pthread_mutex_t g_tMutex = PTHREAD_MUTEX_INITIALIZER;
static pthread_cond_t g_tConVar  = PTHREAD_COND_INITIALIZER;

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

static void* input_eventThreadFunction(void *pVoid)
{
	T_input_event t_inputEvent;
	/*定义函数指针*/
	int (*get_input_event)(PT_input_event pt_input_event);
	get_input_event = (int (*)(PT_input_event))pVoid;

	while(1)
	{
		if(get_input_event(&t_inputEvent) == 0)
		{
			/*唤醒主线程，把t_inputEvent的值赋值给一个全局变量*/
			pthread_mutex_lock(&g_tMutex);
			g_tInputEvent = t_inputEvent;

			/*唤醒主线程*/
			pthread_cond_signal(&g_tConVar);

			/*释放信号量*/
			pthread_mutex_unlock(&g_tMutex);
			
		}
	}
	return NULL;
}

int all_input_devices_init(void)
{
	PT_input_opr pt_tmp = g_pt_input_opr_head;
	int i_error = -1;
	
	while(pt_tmp)
	{
		if(pt_tmp->device_init() == 0)
		{
			/*创建子线程*/
			
			pthread_create(&pt_tmp->t_threadId, NULL, input_eventThreadFunction, pt_tmp->get_input_event);
			i_error = 0;
		}
		pt_tmp = pt_tmp->pt_next;
	}
	return i_error;
}

int get_input_event(PT_input_event pt_input_event)
{
	/*休眠*/
	pthread_mutex_lock(&g_tMutex);
	pthread_cond_wait(&g_tConVar, &g_tMutex);

	/*唤醒后，返回数据*/
	*pt_input_event = g_tInputEvent;
	pthread_mutex_unlock(&g_tMutex);
	return 0;
}

int input_init(void)
{
	int i_error;
	i_error = stdin_init();
	i_error |= touch_screen_init();
	return i_error;
}



