#include <config.h>
#include <fonts_manager.h>
#include <ft2build.h>
#include FT_FREETYPE_H
#include FT_GLYPH_H

static int freetype_font_init(char *pc_font_file, unsigned int dw_font_size);
static int freetype_get_font_bitmap(unsigned int dw_code, PT_font_bit_map pt_font_bitmap);

static T_font_opr g_t_freetype_font_opr = {
	.name				= "freetype",
	.font_init			= freetype_font_init,
	.get_font_bit_map	= freetype_get_font_bitmap,
};

static FT_Library g_t_library;
static FT_Face g_t_face;
static FT_GlyphSlot g_t_slot;

static int freetype_font_init(char *pc_font_file, unsigned int dw_font_size)
{
	int i_error;

	/*ÏÔÊ¾Ê¸Á¿×ÖÌå*/
	i_error = FT_Init_FreeType(g_t_library);
	/*error handing omitted*/
	if(i_error)
	{
		DBG_PRINTF("FT_Init_FreeType failed\n");
		return -1;
	}

	i_error = FT_New_Face(g_t_library, pc_font_file, 0, &g_t_face);
	if(i_error)
	{
		DBG_PRINTF("FT_Init_FreeType failed\n");
		return -1;

	}

	g_t_slot = g_t_face->glyph;

	i_error = FT_Set_Pixel_Sizes(g_t_face, dw_font_size, 0);
	if(i_error)
	{
		DBG_PRINTF("FT_Set_Pixel_Sizes failed: %d\n", dw_font_size);
		return -1;
	}

	return 0;
	
}

static int freetype_get_font_bitmap(unsigned int dw_code, PT_font_bit_map pt_font_bitmap)
{
	int i_error;
	int i_penx = pt_font_bitmap->icur_originx;
	int i_peny = pt_font_bitmap->icur_originy;

	i_error = FT_Load_Char(g_t_face, dw_code, FT_LOAD_RENDER | FT_LOAD_MONOCHROME);
	if(i_error)
	{
		DBG_PRINTF("FT_Load_Char error for code : 0x%x\n", dw_code);
		return -1;
	}

	pt_font_bitmap->ix_left	= i_penx + g_t_slot->bitmap_left;
	pt_font_bitmap->iy_top	= i_peny + g_t_slot->bitmap_top;
	pt_font_bitmap->ix_max	= pt_font_bitmap->ix_max + g_t_slot->bitmap.width;
	pt_font_bitmap->iy_max	= pt_font_bitmap->iy_top+ g_t_slot->bitmap.rows;
	pt_font_bitmap->ibpp	= 1;
	pt_font_bitmap->ipitch	= g_t_slot->bitmap.pitch;
	pt_font_bitmap->puc_buffer = g_t_slot->bitmap.buffer;

	pt_font_bitmap->inext_originx = i_penx + g_t_slot->advance.x / 64;
	pt_font_bitmap->inext_originy = i_peny;

	return 0;
}

int freetype_init(void)
{
	return register_font_opr(&g_t_freetype_font_opr);
}

