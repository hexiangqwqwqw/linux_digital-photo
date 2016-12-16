#ifndef _DISP_MANAGER_H
#define _DISP_MANAGER_H

typedef struct disp_opr {
	char *name;
	int ixres;
	int iyres;
	int ibpp;
	int (*device_init)(void);
	int (*show_pixel)(int ipnex, int ipeny, unsigned int dw_color);
	int (*clean_screen)(unsigned int dw_back_color);
	struct disp_opr *pt_next;
}T_disp_opr, *PT_disp_opr;

int register_dispopr(PT_disp_opr pt_dispopr);
void show_disp_opr(void);
int display_init(void);
int fb_init(void);

#endif	/* _DISP_MANAGER_H */