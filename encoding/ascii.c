
#include <config.h>
#include <encoding_manager.h>
#include <string.h>

static int is_ascii_support(unsigned char *puc_buf_head);
static int ascii_get_code_frmbuf(unsigned char *puc_buf_start, unsigned char *puc_buf_end, unsigned int *pdw_code);


static T_encoding_opr g_t_ascii_encoding_opr = {
	.name				= "ascii",
	.ihead_len			= 0,
	.is_spuuort			= is_ascii_support,
	.get_code_frmbuf 	= ascii_get_code_frmbuf,

};

static int is_ascii_support(unsigned char *puc_buf_head)
{
	const char a_str_utf8[] = {0xef, 0xbb, 0xbf,0};
	const char a_str_utf16le[] = {0xff, 0xfe, 0};
	const char a_str_utf16be[] = {0xfe, 0xff, 0};

	if(strncmp((const char *) puc_buf_head, a_str_utf8, 3) == 0)
	{
		return 0;
	}
	else if(strncmp((const char *)puc_buf_head, a_str_utf16le, 2)== 0)
	{
		return 0;
	}
	else if(strncmp((const char *)puc_buf_head, a_str_utf16be, 2) == 0)
	{
		return 0;
	}
	else
	{
		return 1;
	}
}

static int ascii_get_code_frmbuf(unsigned char *puc_buf_start, unsigned char *puc_buf_end, unsigned int *pdw_code)
{
	u
}

