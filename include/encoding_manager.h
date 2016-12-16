
#ifndef _ENCODING_MANAGER_H
#define _ENCODING_MANAGER_H

#include <fonts_manager.h>
#include <disp_manager.h>

typedef struct encoding_opr {
	char *name;
	int ihead_len;
	PT_font_opr pt_font_opr_supported_head;
	int (*is_support)(unsigned char *puc_bufhead);
	int (*get_code_frmbuf)(unsigned char *puc_bufstar, unsigned char *puc_bufend, unsigned int *pdw_code);
	struct encoding_opr *pt_next;
}T_encoding_opr, *PT_encoding_opr;

int register_encoding_opr(PT_encoding_opr  pt_encoding_opr);
void show_encoding_opr(void);
PT_disp_opr get_disp_opr(char *name);
PT_encoding_opr select_encoding_opr_for_file(unsigned char *puc_filebuf_head);
int add_font_opr_for_encoding(PT_encoding_opr pt_encoding_opr, PT_font_opr pt_font_opr);
int del_font_opr_frm_dncoding(PT_encoding_opr pt_encoding_opr, PT_font_opr pt_font_opr);
int encoding_init(void);
int ascii_encoding_init(void);
int utf16be_encoding_init(void);
int utf16le_encoding_init(void);
int utf8_encoding_init(void);


#endif