#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <config.h>
#include <draw.h>
#include <encoding_manager.h>
#include <fonts_manager.h>
#include <disp_manager.h>
#include <string.h>



typedef struct page_desc {
	int ipage;
	unsigned char *puc_lcd_first_pos_file;
	unsigned char *puc_lcd_next_page_first_pos_file;
	struct page_desc *pt_next_page;
	struct page_desc *pt_pre_page;
} T_page_desc, *PT_page_desc;

static int g_ifd_text_file;
static unsigned char *g_puc_text_file_mem;
static unsigned char *g_puc_text_file_mem_end;
static PT_encoding_opr g_pt_encoding_opr_for_file;

static PT_disp_opr g_pt_disp_opr;

static unsigned char *g_puc_lcd_first_posat_file;
static unsigned char *g_puc_lcd_next_possat_file;

static int g_dwfont_size;

static PT_page_desc g_pt_pages = NULL;
static PT_page_desc g_pt_cur_page = NULL;

int open_text_file(char *pc_file_name)
{
	struct stat t_stat;

	g_ifd_text_file = open(pc_file_name, O_RDONLY);
	if(g_ifd_text_file < 0)
	{
		DBG_PRINTF("cant open text file %s\n", pc_file_name);
		return -1;
	}

	if(fstat(g_ifd_text_file, &t_stat))
	{
		DBG_PRINTF("cant get fstat\n");
		return -1;
	}

	g_puc_text_file_mem = (unsigned char *)mmap(NULL, t_stat.st_size, PROT_READ, MAP_SHARED, g_ifd_text_file, 0);
	if(g_puc_text_file_mem == (unsigned char *)-1)
	{
		DBG_PRINTF("cant mmap for text file\n");
		return -1;
	}

	g_puc_text_file_mem_end = g_puc_text_file_mem + t_stat.st_size;
	g_pt_encoding_opr_for_file = select_encoding_opr_for_file(g_puc_text_file_mem);

	if(g_pt_encoding_opr_for_file)
	{
		g_puc_lcd_first_posat_file = g_puc_text_file_mem + g_pt_encoding_opr_for_file->ihead_len;
		return 0;
	}
	else
	{
		return -1;
	}
}

int set_text_detail(char *pc_hzk_file, char *pc_file_freetype, unsigned int dw_font_size)
{
	int ierror = 0;
	PT_font_opr pt_font_opr;
	PT_font_opr pt_tmp;
	int iret = -1;

	g_dwfont_size = dw_font_size;
	
	pt_font_opr = g_pt_encoding_opr_for_file->pt_font_opr_supported_head;
	while(pt_font_opr)
	{
		if(strcmp(pt_font_opr->name, "ascii") == 0)
		{
			ierror = pt_font_opr->font_init(NULL, dw_font_size);
		}
		else if(strcmp(pt_font_opr->name, "gbk")==0)
		{
			ierror = pt_font_opr->font_init(pc_hzk_file, dw_font_size);
		}
		else
		{
			ierror = pt_font_opr->font_init(pc_file_freetype, dw_font_size);
		}
		DBG_PRINTF("%s, %d\n", pt_font_opr->name, ierror);

		pt_tmp = pt_font_opr->pt_next;

		if(ierror == 0)
		{
			/* 比如对于ascii编码的文件, 可能用ascii字体也可能用gbk字体, 
			 * 所以只要有一个FontInit成功, SetTextDetail最终就返回成功
			 */
			iret = 0;
		}
		else
		{
			del_font_opr_frm_dncoding(g_pt_encoding_opr_for_file, pt_font_opr);
		}
		pt_font_opr = pt_tmp;
	}
	return iret;
}


int select_and_init_display(char *pc_name)
{
	int i_error;
	g_pt_disp_opr = get_disp_opr(pc_name);
	if(!g_pt_disp_opr)
	{
		return -1;
	}

	i_error = g_pt_disp_opr->device_init();
	return i_error;
}

int get_disp_resolution(int *pi_xres, int *pi_yres)
{
	if(g_pt_disp_opr)
	{
		*pi_xres = g_pt_disp_opr->ixres;
		*pi_yres = g_pt_disp_opr->iyres;
		return 0;
	}
	else
	{
		return -1;
	}
}

int intc_lcdx(int ix)
{
	if (ix+1 < g_pt_disp_opr->ixres)
		return (ix+1);
	else
		return 0;
}

int intc_lcdy(int iy)
{
	if(iy+g_dwfont_size < g_pt_disp_opr->iyres)
		return (iy+g_dwfont_size);
	else
		return 0;
}

int relocate_fontpos(PT_font_bit_map pt_font_bitmap)
{
	int i_lcdy;
	int i_deltax;
	int i_deltay;

	if(pt_font_bitmap->iy_max > g_pt_disp_opr->iyres)
	{
		return -1; /*page full*/
	}

	if(pt_font_bitmap->ix_max > g_pt_disp_opr->ixres)
	{
		i_lcdy = intc_lcdy(pt_font_bitmap->icur_originy);
		if(i_lcdy == 0)
		{
			return -1;
		}
		else
		{
			i_deltax = 0-pt_font_bitmap->icur_originx;
			i_deltay = i_lcdy - pt_font_bitmap->icur_originy;

			pt_font_bitmap->icur_originx += i_deltax;
			pt_font_bitmap->icur_originy += i_deltay;

			pt_font_bitmap->inext_originx += i_deltax;
			pt_font_bitmap->inext_originy += i_deltay;

			pt_font_bitmap->ix_left += i_deltax;
			pt_font_bitmap->ix_max += i_deltax;

			pt_font_bitmap->iy_top += i_deltay;
			pt_font_bitmap->iy_max += i_deltay;

			return 0;
		}
	}
	return 0;
}

int show_onefont(PT_font_bit_map pt_font_bitmap)
{
	int ix;
	int iy;
	unsigned char  uc_byte = 0;
	int i = 0;
	int i_bit;

	if (pt_font_bitmap->ibpp == 1)
	{
		for(iy=pt_font_bitmap->iy_top; iy < pt_font_bitmap->iy_max; iy++)
		{
			i = (iy - pt_font_bitmap->iy_top) * pt_font_bitmap->ipitch;
			for(ix = pt_font_bitmap->ix_left, i_bit = 7; ix < pt_font_bitmap->ix_max; ix++)
			{
				if(i_bit == 7)
				{
					uc_byte = pt_font_bitmap->puc_buffer[i++];
				}
				if(uc_byte & (1 << i_bit))
				{
					g_pt_disp_opr->show_pixel(ix, iy, COLOR_FOREGROUND);
				}
				else
				{
					
				}
				i_bit--;
				if(i_bit == -1)
				{
					i_bit = 7;
				}
			}
		}
	}
	else if(pt_font_bitmap->ibpp == 8)
	{
		for(iy = pt_font_bitmap->iy_top; iy < pt_font_bitmap->iy_max; iy++)
			for(ix = pt_font_bitmap->ix_left; ix < pt_font_bitmap->ix_max; ix++)
			{
				if(pt_font_bitmap->puc_buffer[i++])
					g_pt_disp_opr->show_pixel(ix, iy, COLOR_FOREGROUND);
			}
	}
	else
	{
		DBG_PRINTF("show one font error ,cant support %d bpp \n", pt_font_bitmap->ibpp);
		return -1;
	}
	return 0;
	
}

int show_onepage(unsigned char *puc_textfile_memcur_pos)
{
	int i_len;
	int i_error;
	unsigned char *puc_buf_start;
	unsigned int  dw_code;
	PT_font_opr	pt_font_opr;
	T_font_bit_map t_font_bitmap;

	int i_has_not_clrscreen = 1;
	int i_has_get_code =0;

	t_font_bitmap.icur_originx = 0;
	t_font_bitmap.icur_originy = g_dwfont_size;
	puc_buf_start = puc_textfile_memcur_pos;

	while(1)
	{
		i_len = g_pt_encoding_opr_for_file->get_code_frmbuf(puc_buf_start, g_puc_text_file_mem_end, &dw_code);
		if(i_len == 0)
		{
			if(!i_has_get_code)
			{
				return -1;
			}
			else
			{
				return 0;
			}
		}

		i_has_get_code = 1;

		puc_buf_start += i_len;

		if(dw_code == '\n')
		{
			g_puc_lcd_next_possat_file = puc_buf_start;

			t_font_bitmap.icur_originx = 0;
			t_font_bitmap.icur_originy = intc_lcdy(t_font_bitmap.icur_originy);
			if(t_font_bitmap.icur_originy == 0)
			{
				return 0;
			}
			else
			{
				continue;
			}
		}
		else if (dw_code == '\r')
		{
				continue;
		}
		else if(dw_code == '\t')
		{
			dw_code = ' ';
		}

		DBG_PRINTF("dw_code = 0x%x\n", dw_code);

		pt_font_opr = g_pt_encoding_opr_for_file->pt_font_opr_supported_head;
		while(pt_font_opr)
		{
			DBG_PRINTF("%s %s %d\n", __FILE__, __FUNCTION__, __LINE__);
			i_error = pt_font_opr->get_font_bit_map(dw_code, &t_font_bitmap);
			DBG_PRINTF("%s %s %d, pt_font_opr->name = %s. %d\n", __FILE__, __FUNCTION__, __LINE__, pt_font_opr->name, i_error);

			if(i_error == 0)
			{
				DBG_PRINTF("%s %s %d\n", __FILE__, FUNCTION__, __LINE__);
				if(relocate_fontpos(&t_font_bitmap))
				{
					return 0;
				}
				DBG_PRINTF("%s %s %d\n", __FILE__, __FUNCTION__, __LINE__);

				if(i_has_not_clrscreen)
				{
					g_pt_disp_opr->clean_screen(COLOR_BACKGROUND);
					i_has_not_clrscreen = 0;
				}
				DBG_PRINTF("%s %s %d\n", __FILE__, __FUNCTION__, __LINE__);

				if(show_onefont(&t_font_bitmap))
				{
					return -1;
				}

				t_font_bitmap.icur_originx = t_font_bitmap.inext_originx;
				t_font_bitmap.icur_originx = t_font_bitmap.inext_originy;
				g_puc_lcd_next_possat_file = puc_buf_start;

				break;
			}
			pt_font_opr = pt_font_opr->pt_next;
		}
		
	}
	return 0;
}

static void record_page(PT_page_desc pt_page_new)
{
	PT_page_desc pt_page;

	if(!g_pt_pages)
	{
		g_pt_pages = pt_page_new;
	}
	else
	{
		pt_page = g_pt_pages;
		while(pt_page->pt_next_page)
		{
			pt_page = pt_page->pt_next_page;
		}
		pt_page->pt_next_page = pt_page_new;
		pt_page_new->pt_pre_page = pt_page;
	}
}

int show_next_page(void)
{
	int i_error;
	PT_page_desc pt_page;
	unsigned char *puc_text_file_mem_curpos;
	if(g_pt_cur_page)
	{
		puc_text_file_mem_curpos = g_pt_cur_page->puc_lcd_next_page_first_pos_file;
	}
	else
	{
		puc_text_file_mem_curpos = g_puc_lcd_first_posat_file;
	}

	i_error = show_onepage(puc_text_file_mem_curpos);
	DBG_PRINTF("%s %d %d\n", __FUNCTION__, __LINE__, i_error);
	if(i_error == 0)
	{
		if(g_pt_cur_page && g_pt_cur_page->pt_next_page)
		{
			g_pt_cur_page = g_pt_cur_page->pt_next_page;
			return 0;
		}
	}

	pt_page = malloc(sizeof(T_page_desc));
	if(pt_page)
	{
		pt_page->puc_lcd_first_pos_file		        = puc_text_file_mem_curpos;
		pt_page->puc_lcd_next_page_first_pos_file	= g_puc_lcd_next_possat_file;
		pt_page->pt_pre_page						= NULL;
		pt_page->pt_next_page						= NULL;
		g_pt_cur_page = pt_page;
		DBG_PRINTF("%s %d, pos = 0x%x\n", __FUNCTION__, __LINE__, (unsigned int)pt_page->puc_lcd_first_pos_file);
		record_page(pt_page);
		return 0;
	}
	else
	{
		return -1;
	}

	return i_error;
}

int show_pre_page(void)
{
	int i_error;

	DBG_PRINTF("%s %d\n", __FUNCTION__, __LINE__);
	if(!g_pt_cur_page || !g_pt_cur_page->pt_next_page)
	{
		return -1;
	}

	DBG_PRINTF("%s %d, pos = 0x%x\n", __FUNCTION__, __LINE__, (unsigned int)g_pt_cur_page->pt_pre_page->puc_lcd_first_pos_file);
	i_error = show_onepage(g_pt_cur_page->pt_pre_page->puc_lcd_first_pos_file);
	if(i_error == 0)
	{
		DBG_PRINTF("%s %d\n", __FUNCTION__, __LINE__);
		g_pt_cur_page = g_pt_cur_page->pt_pre_page;
	}
	return i_error;
}

