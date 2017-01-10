
#ifndef  _FONTS_MANAGER_H
#define _FONTS_MANAGER_H

typedef struct font_bit_map {
		int ix_left;
		int iy_top;
		int ix_max;
		int iy_max;
		int ibpp;
		int ipitch;			/* 对于单色位图, 两行象素之间的跨度 */
		int icur_originx;
		int icur_originy;
		int inext_originx;
		int inext_originy;
		unsigned char *puc_buffer;
}T_font_bit_map, *PT_font_bit_map;

typedef struct font_opr{
		char *name;
		int (*font_init)(char *pc_fontfiel, unsigned int dw_fontsize);
		int (*get_font_bit_map)(unsigned int dw_code, PT_font_bit_map pt_font_bitmap);
		struct font_opr *pt_next;
}T_font_opr, *PT_font_opr;

int register_font_opr(PT_font_opr pt_fontopr);
void show_font_opr(void);
int fonts_init(void);
int ascll_init(void);
int gbk_init(void);
int freetype_init(void);
PT_font_opr get_font_opr(char *pc_name);

#endif
