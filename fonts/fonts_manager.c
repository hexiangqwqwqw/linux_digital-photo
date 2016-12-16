#include <fonts_manager.h>
#include <config.h>
#include <string.h>
#include <stdio.h>

static PT_font_opr g_pt_font_opr_head;
int register_font_opr(PT_font_opr pt_font_opr)
{
	PT_font_opr pt_tmp;

	if(!g_pt_font_opr_head)
	{
		g_pt_font_opr_head = pt_font_opr;
		pt_font_opr->pt_next = NULL;
	}
	else
	{
		pt_tmp = g_pt_font_opr_head;
		while(pt_tmp->pt_next)
		{
			pt_tmp = pt_tmp->pt_next;
		}
		pt_tmp->pt_next = pt_font_opr;
		pt_font_opr->pt_next = NULL;
		
	}

	return 0;
}

void show_font_opr(void)
{
	int i = 0;
	PT_font_opr pt_tmp = g_pt_font_opr_head;
	while(pt_tmp)
	{
		printf("%02d %s\n", i++, pt_tmp->name);
		pt_tmp = pt_tmp->pt_next;
	}
}

PT_font_opr get_font_opr(char *puc_name)
{
	PT_font_opr pt_tmp = g_pt_font_opr_head;

	while(pt_tmp)
	{
		if(strcmp(pt_tmp->name, puc_name)==0)
		{
			return pt_tmp;
		}
		pt_tmp = pt_tmp->pt_next;
	}
	return NULL;
}

int fonts_init(void)
{
	int ierror;

	ierror = ascll_init();
	if(ierror)
	{
		DBG_PRINTF("ascll_init error!\n");
		return -1;
	}

	ierror = gbk_init();
	if(ierror)
	{
		DBG_PRINTF("gbk_init error!\n");
		return -1;
	}

	ierror = freetype_init();
	if(ierror)
	{
		DBG_PRINTF("freetype_init error!\n");
		return -1;
	}

	return 0;
}