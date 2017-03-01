
#include <input_manager.h>
#include <termios.h>
#include <unistd.h>
#include <stdio.h>



static int stdin_device_init(void)
{
	struct termios t_tty_state;
	 
	//get the terminal state
	tcgetattr(STDIN_FILENO, &t_tty_state);
	 
	//turn off canonical mode
	t_tty_state.c_lflag &= ~ICANON;
	//minimum of number input read.
	t_tty_state.c_cc[VMIN] = 1;	/* 有一个数据时就立刻返回 */
	
	//set the terminal attributes.
	tcsetattr(STDIN_FILENO, TCSANOW, &t_tty_state);
	
	return 0;

}

static int stdin_device_exit(void)
{
    struct termios t_tty_state;
 
    //get the terminal state
    tcgetattr(STDIN_FILENO, &t_tty_state);
 
    //turn on canonical mode
    t_tty_state.c_lflag |= ICANON;
	
    //set the terminal attributes.
    tcsetattr(STDIN_FILENO, TCSANOW, &t_tty_state);	

	return 0;
}

static int stdin_get_input_event(PT_input_event pt_input_event)
{
	/*如果有数据就读取 处理 返回
	*如果没有数据，就立刻返回，不等待
	*/
	/*select poll */

	struct timeval t_tv;
	char c;

	/*处理数据*/
	pt_input_event->i_type = INPUT_TYPE_STDIN;


	c = fgetc(stdin);	/*如果没有数据，会休眠	*/
	gettimeofday(&pt_input_event->t_time, NULL);
	if(c == 'u')
	{
		pt_input_event->i_val = INPUT_VALUE_UP;
	}
	else if(c == 'n')
	{
		pt_input_event->i_val = INPUT_VALUE_DOWN;
	}
	else if (c == 'q')
	{
		pt_input_event->i_val = INPUT_VALUE_EXIT;
	}
	else
	{
		pt_input_event->i_val = INPUT_VALUE_UNKNOWN;
	}
	return 0;
	
}

static T_input_opr g_t_input_opr = {
	.name 				= "stdin",
	.device_init		= stdin_device_init,
	.device_exit		= stdin_device_exit,
	.get_input_event	= stdin_get_input_event,
};

int stdin_init(void)
{
	return register_input_opr(&g_t_input_opr);
}

