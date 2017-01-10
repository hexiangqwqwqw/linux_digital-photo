#include <config.h>
#include <disp_manager.h>
#include <linux/fb.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <sys/mman.h>

#include <string.h>

static int fb_device_init(void);
static int fb_show_pixel(int ix, int iy, unsigned int dw_color);
static int fb_clean_screen(unsigned int dw_back_color);

static int g_fd;
static struct fb_var_screeninfo g_t_fb_var;
static struct fb_fix_screeninfo g_t_fb_fix;
static unsigned int g_dw_screen_size;
static unsigned char *g_puc_fbmem;

static unsigned int g_dw_linewidth;
static unsigned int g_dw_pixelwidth;

/*
*分配，设置，注册一个结构体
*/
static T_disp_opr g_t_fbopr = {
	.name			= "fb",
	.device_init	=fb_device_init,
	.show_pixel		=fb_show_pixel,
	.clean_screen	= fb_clean_screen,
};

static int fb_device_init(void)
{
	int ret;

	g_fd = open(FB_DEVICE_NAME, O_RDWR);
	if(g_fd < 0)
	{
		DBG_PRINTF("cant open %s\n", FB_DEVICE_NAME);
	}

	ret = ioctl(g_fd, FBIOGET_VSCREENINFO, &g_t_fb_var);
	if(ret < 0)
	{
		DBG_PRINTF("cant get fb's var\n");
		return -1;
	}

	ret = ioctl(g_fd, FBIOGET_FSCREENINFO, &g_t_fb_fix);
	if(ret < 0)
	{
		DBG_PRINTF("cant get fb's fix\n");
		return -1;
	}

	g_dw_screen_size = g_t_fb_var.xres * g_t_fb_var.yres * g_t_fb_var.bits_per_pixel / 8;
	g_puc_fbmem = (unsigned char *)mmap(NULL, g_dw_screen_size, PROT_READ | PROT_WRITE, MAP_SHARED, g_fd,0);
	if(g_puc_fbmem < 0)
	{
		DBG_PRINTF("cant mmap\n");
		return -1;
	}

	g_t_fbopr.ixres = g_t_fb_var.xres;
	g_t_fbopr.iyres = g_t_fb_var.yres;
	g_t_fbopr.ibpp = g_t_fb_var.bits_per_pixel;

	g_dw_linewidth = g_t_fb_var.xres * g_t_fb_var.bits_per_pixel / 8;
	g_dw_pixelwidth = g_t_fb_var.bits_per_pixel / 8;

	return 0;
}

static int fb_show_pixel(int ix, int iy, unsigned int dw_color)
{
	unsigned char *puc_fb;
	unsigned short *pw_fb16bpp;
	unsigned int *pdw_fb32bpp;
	unsigned short w_color_16bpp; /*565*/
	int ired;
	int igreen;
	int iblue;
	
	if((ix >= g_t_fb_var.xres) || (iy >= g_t_fb_var.yres))
	{
		DBG_PRINTF("out of  region\n");
		return -1;
	}

	puc_fb = g_puc_fbmem + g_dw_linewidth *iy + g_dw_pixelwidth * ix;
	pw_fb16bpp = (unsigned short *)puc_fb;
	pdw_fb32bpp = (unsigned int *)puc_fb;

	switch(g_t_fb_var.bits_per_pixel)
	{
		case 8:
		{
			*puc_fb = (unsigned char)dw_color;
			break;
		}
		case 16:
		{
			ired = (dw_color >> (16+3)) & 0x1f;
			igreen = (dw_color >> (8+2)) & 0x3f;
			iblue = (dw_color >>3) & 0x1f;
			w_color_16bpp = (ired << 11) | (igreen << 5) | iblue;
			*pw_fb16bpp = w_color_16bpp;
			break;
		}
		case 32:
		{
				*pdw_fb32bpp = dw_color;
				break;
			
		}
		default :
		{
			DBG_PRINTF("cant support %d bpp\n", g_t_fb_var.bits_per_pixel);
			return -1;
		}
	}

	return 0;
	
}

static int fb_clean_screen(unsigned int dw_back_color)
{
	unsigned short w_color_16bpp; /* 565 */
	unsigned char *puc_fb;
	unsigned short *pw_fb16bpp;
	unsigned int *pdw_fb32bpp;
	
	int ired;
	int igreen;
	int iblue;
	int i = 0;

	puc_fb = g_puc_fbmem;
	pw_fb16bpp = (unsigned short *)puc_fb;
	pdw_fb32bpp = (unsigned int *)puc_fb;
	
	switch(g_t_fb_var.bits_per_pixel)
	{
		case 8:
		{
			memset(g_puc_fbmem, dw_back_color, g_dw_screen_size);
			break;
		}
		case 16:
		{
			ired = (dw_back_color >>(16+3)) & 0x1f;
			igreen = (dw_back_color >> (8+2)) & 0x3f;
			iblue = (dw_back_color >> 3) & 0x1f;
			w_color_16bpp = (ired << 11) | (igreen << 5) | iblue;
			while(i < g_dw_screen_size)
			{
				*pw_fb16bpp = w_color_16bpp;
				pw_fb16bpp++;
				i +=2;
			}
			break;
		}
		case 32:
		{
			while(i < g_dw_screen_size)
			{
				*pdw_fb32bpp = dw_back_color;
				pdw_fb32bpp++;
				i +=4;
			}
			break;
		}
		default :
		{
			DBG_PRINTF("cant support %d bpp\n", g_t_fb_var.bits_per_pixel);
			return -1;
		}
		
	}
	return 0;
}

int fb_init(void)
{
	return register_dispopr(&g_t_fbopr);
}