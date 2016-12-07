#include <disp_manager.h>

static PT_DispOpr g_pt_dispopr_head;

int register_dispopr(PT_DispOpr pt_dispopr)
{
	PT_DispOpr pt_tmp;

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

void show_dispopr(void)
{
	int i = 0;
	PT_DispOpr pt_tmp = g_pt_dispopr_head;

	while(pt_tmp)
	{
		
	}
}

