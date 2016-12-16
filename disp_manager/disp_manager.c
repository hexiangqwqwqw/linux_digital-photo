#include <disp_manager.h>
#include <stdio.h>
#include <string.h>

static PT_disp_opr g_pt_dispopr_head;

int register_dispopr(PT_disp_opr pt_dispopr)
{
	PT_disp_opr pt_tmp;

	if(!g_pt_dispopr_head){
		
		g_pt_dispopr_head = pt_dispopr;
		pt_dispopr->pt_next = NULL;
		
	} else{

		pt_tmp = g_pt_dispopr_head;
		while(pt_tmp->pt_next)
		{
			pt_tmp = pt_tmp->pt_next;
		}
		pt_tmp->pt_next = pt_dispopr;
		pt_dispopr->pt_next = NULL;
	}

	return 0;
}

void show_disp_opr(void)
{
	int i = 0;
	PT_disp_opr pt_tmp = g_pt_dispopr_head;

	while(pt_tmp)
	{
		printf("%02d %s\n", i++, pt_tmp->name);
		pt_tmp = pt_tmp->pt_next;
	}
}


PT_disp_opr get_disp_opr(char *name)
{
	PT_disp_opr pt_tmp = g_pt_dispopr_head;

	while(pt_tmp)
	{
		if(strcmp(pt_tmp->name, name) == 0)
		{
			return pt_tmp;
		}
		pt_tmp = pt_tmp->pt_next;
	}
	return NULL;
}

int display_init(void)
{
		int i_error;
		i_error = fb_init();
		return i_error;
}
