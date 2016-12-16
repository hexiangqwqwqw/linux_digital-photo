#include <fonts_manager.h>
#include <config.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>

static int gbk_font_init(char *pc_font_file, unsigned int dw_font_size);
static int gbk_get_font_bit_map(unsigned int dw_code, PT_font_bit_map pt_font_bit_map);

static T_font_opr g_t_gpk_font_opr = {
	.name				= "gpk",
	.font_init			= gbk_font_init,
	.get_font_bit_map	= gbk_get_font_bit_map,
};

static int g_ifd_hzk;
static unsigned char *g_puc_hzk_mem;
static unsigned char *g_puc_hzk_mem_end;

static int gbk_font_init(char *pc_font_file, unsigned int dw_font_size)
{
	struct stat t_stat;
	
	if(dw_font_size != 16)
	{
		DBG_PRINTF("gbk cant support %d fontsize\n", pc_font_file);
		return -1;
	}
	 g_ifd_hzk = open(pc_font_file, O_RDONLY);
	if(g_ifd_hzk < 0)
	{
		DBG_PRINTF("cant open %s\n", pc_font_file);
		return -1;
	}
	if(fstat(g_ifd_hzk, &t_stat))
	{
		DBG_PRINTF("cant get fstat\n");
		return -1;
	}

	g_puc_hzk_mem = (unsigned char *)mmap(NULL, t_stat.st_size, PROT_READ, MAP_SHARED, g_ifd_hzk, 0);
	if(g_puc_hzk_mem == (unsigned char *) -1)
	{
		DBG_PRINTF("cant mmap for hzk16\n");
		return -1;
	}

	g_puc_hzk_mem_end = g_puc_hzk_mem + t_stat.st_size;
	return 0;
		
}

static int gbk_get_font_bit_map(unsigned int dw_code, PT_font_bit_map pt_font_bit_map)
{
	int iarea;
	int iwhere;

	int ipenx = pt_font_bit_map->icur_originx;
	int ipeny = pt_font_bit_map->icur_originy;

	DBG_PRINTF("%s %s %d\n", __FILE__, __FUNCTION__, __LINE__);
	if(dw_code & 0xffff0000)
	{
		DBG_PRINTF("dont support this code : 0x %x\n", dw_code);
		return -1;
	}

	iarea = (int)(dw_code & 0xff) - 0xA1;
	iwhere = (int)((dw_code >> 8) & 0xff ) - 0xA1;

	if((iarea < 0) || (iwhere < 0))
	{
		DBG_PRINTF("%s %s %d\n", __FILE__, FUNCTION__, __LINE__);

		return -1;
	}

	pt_font_bit_map->ix_left	= ipenx;
	pt_font_bit_map->iy_top		= ipeny - 16;
	pt_font_bit_map->ix_max		= ipenx + 16;
	pt_font_bit_map->iy_max		= ipeny;
	pt_font_bit_map->ibpp		= 1;
	pt_font_bit_map->ipitch		= 2;
	pt_font_bit_map->puc_buffer = g_puc_hzk_mem + (iarea * 94 + iwhere) * 32;

	if(pt_font_bit_map->puc_buffer >= g_puc_hzk_mem_end)
	{
		return -1;
	}

	pt_font_bit_map->inext_originx = ipenx + 16;
	pt_font_bit_map->inext_originy = ipeny;

	DBG_PRINTF("%s %s %d\n", __FILE__, FUNCTION__, __LINE__);
	return 0;	
}

int gbk_init(void)
{
	return register_font_opr(&g_t_gpk_font_opr);
}