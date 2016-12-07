
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
	struct encoding_opr pt_next;
}T_encoding_opr, *PT_encoding_opr;

#endif