#include <encoding_manager.h>
#include <string.h>
#include <stdlib.h>


static PT_encoding_opr g_pt_encoding_opr_head;

int register_encoding_opr(PT_encoding_opr  pt_encoding_opr)
{
	PT_encoding_opr pt_tmp;

	if(!g_pt_encoding_opr_head)
	{
		g_pt_encoding_opr_head = pt_encoding_opr;
		pt_encoding_opr->pt_next = NULL;
	}
	else
	{	pt_tmp = g_pt_encoding_opr_head;
		while(pt_tmp->pt_next)
		{
			pt_tmp = pt_tmp->pt_next;
		}
		pt_tmp->pt_next = pt_encoding_opr;
		pt_encoding_opr->pt_next = NULL;
	}
		return 0;
}

void show_encoding_opr(void)
{
	int i = 0;
	PT_encoding_opr pt_tmp = g_pt_encoding_opr_head;

	while(pt_tmp)
	{
		printf("%02d %s\n", i++, pt_tmp->name);
		pt_tmp = pt_tmp->pt_next;
	}

}

PT_encoding_opr select_encoding_opr_for_file(unsigned char *puc_filebuf_head)
{
	PT_encoding_opr pt_tmp = g_pt_encoding_opr_head;

	while(pt_tmp)
	{
		if(pt_tmp->is_support(puc_filebuf_head))
			return pt_tmp;
		else
			pt_tmp = pt_tmp->pt_next;
	}
	return NULL;
}

int add_font_opr_for_encoding(PT_encoding_opr pt_encoding_opr, PT_font_opr pt_font_opr)
{
	PT_font_opr pt_font_opr_cpy;
	if(!pt_encoding_opr || !pt_font_opr)
	{
		return -1;
	}
	else
	{
		pt_font_opr_cpy = malloc(sizeof(T_font_opr));
		if(!pt_font_opr_cpy)
		{
			return -1;
		}
		else
		{
			memcpy(pt_font_opr_cpy, pt_font_opr, sizeof(T_font_opr));
			pt_font_opr_cpy->pt_next = pt_encoding_opr->pt_font_opr_supported_head;
			pt_encoding_opr->pt_font_opr_supported_head = pt_font_opr_cpy;
			return 0;
		}
	}
}
int del_font_opr_frm_dncoding(PT_encoding_opr pt_encoding_opr, PT_font_opr pt_font_opr)
{
	PT_font_opr pt_tmp;
	PT_font_opr pt_pre;

	if(!pt_encoding_opr || !pt_font_opr)
	{
		return -1;
	}
	else
	{
		pt_tmp = pt_encoding_opr->pt_font_opr_supported_heaad;
		if(strcmp(pt_tmp->name, pt_font_opr->name)==0)
		{
			/*删除节点*/
			pt_encoding_opr->pt_font_opr_supported_head = pt_tmp->pt_next;
			free(pt_tmp);
			return 0;
		}
	

		pt_pre = pt_encoding_opr->pt_encoding_opr_supported_head;
		pt_tmp = pt_pre->pt_next;

		while(pt_tmp)
		{
			if(strcmp(pt_tmp->name, pt_font_opr->name) == 0)
			{
				/*从链表里取出，释放*/
				pt_pre->pt_next = pt_tmp->pt_next;
				free(pt_tmp);
				return 0;
			}
			else
			{
				pt_pre = pt_tmp;
				pt_tmp = pt_tmp->pt_next;
			}
		}
		return -1;
	}
	
}

int encoding_init(void)
{
	int ierror;

	ierror = ascii_encoding_init(void);
	if(ierror)
	{
		DGB_PRINTF("ascii encoding init error!\n");
		return -1;
	}

	ierror = utf16le_encoding_init(void);
	if(ierror)
	{
		DGB_PRINTF("utf16le encoding init error!\n");
		return -1;
	}

	ierror = utf16be_encoding_init(void);
	if(ierror)
	{
		DGB_PRINTF("utf16be encoding init error!\n");
		return -1;
	}

	ierror = utf8_encoding_init(void);
	if(ierror)
	{
		DGB_PRINTF("utf8 encoding init error!\n");
		return -1;
	}

	return 0;
}
