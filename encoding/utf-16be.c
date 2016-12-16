#include <config.h>
#include <encoding_manager.h>
#include <string.h>

static int is_utf16be_coding(unsigned char *puc_buf_head);
static int utf16be_get_code_frmbuf(unsigned char *puc_buf_start, unsigned char*puc_buf_end, unsigned int *pdw_code);

static T_encoding_opr g_t_utf16be_encoding_opr = {
	.name				= "utf-16be",
	.ihead_len			= 2,
	.is_support			= is_utf16be_coding,
	.get_code_frmbuf	= utf16be_get_code_frmbuf,
};

static int is_utf16be_coding(unsigned char *puc_buf_head)
{
	const char a_str_utf16be[] = {0xfe, 0xff, 0};

	if(strncmp((const char *)puc_buf_head, a_str_utf16be, 2) == 0)
	{
		return 1;
	}
	else
	{
		return 0;
	}
}

static int utf16be_get_code_frmbuf(unsigned char *puc_buf_start, unsigned char*puc_buf_end, unsigned int *pdw_code)
{
	if(puc_buf_start + 1 < puc_buf_end)
	{
		*pdw_code = ((unsigned int)puc_buf_start[0]<<8) + puc_buf_start[1];
		return 2;
	}
	else
	{
		return 0;
	}
}

int utf16be_encoding_init(void)
{
	add_font_opr_for_encoding(&g_t_utf16be_encoding_opr, get_font_opr("freetype"));
	add_font_opr_for_encoding(&g_t_utf16be_encoding_opr, get_font_opr("ascii"));
	return register_encoding_opr(&g_t_utf16be_encoding_opr);
	
}
