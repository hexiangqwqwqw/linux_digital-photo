#include <config.h>
#include <encoding_manager.h>
#include <string.h>

static int is_utf8_encoding(unsigned char *puc_buf_head);
static int utf8_get_code_frmbuf(unsigned char *puc_buf_start, unsigned char * puc_buf_end, unsigned int *pdw_code);

static T_encoding_opr g_t_utf8_encoding_opr = {
	.name				= "utf8",
	.ihead_len			= 3,
	.is_support			= is_utf8_encoding,
	.get_code_frmbuf	= utf8_get_code_frmbuf,
};

static int is_utf8_encoding(unsigned char *puc_buf_head)
{
	const char a_str_utf8[] = {0xef, 0xbb 0xbf, 0};
	if(strncmp((const char *)puc_buf_head, a_str_utf8, 3) == 0)
	{
		return 1;
	}
	else
	{
		return 0;
	}
}


static int utf8_get_code_frmbuf(unsigned char *puc_buf_start, unsigned char * puc_buf_end, unsigned int *pdw_code)
{
	int i;
	int inum;
	unsigned char uc_val;
	unsigned int dw_sum;

	if(puc_buf_start >= puc_buf_end)
	{
		return 0;
	}

	if(inum == 0)
	{
		*pdw_code = puc_buf_start[0];
		return 1;
	}
	else
	{
		uc_val = uc_val << inum;
		uc_val = uc_val >> inum;
		dw_sum += uc_val;
		for(i = 1; i < inum; i++)
		{
			uc_val = puc_buf_start[i] & 0x3f;
			dw_sum = dw_sum << 6;
			dw_sum += uc_val;
		}
		*pdw_code = dw_sum;
		return inum;
	}

}

int utf8_encoding_init(void)
{
	add_font_opr_for_encoding(&g_t_utf8_encoding_opr, get_font_opr("freetype"));
	add_font_opr_for_encoding(&g_t_utf8_encoding_opr,get_font_opr("ascii"));
	return register_encoding_opr(&g_t_utf8_encoding_opr);
	
}

