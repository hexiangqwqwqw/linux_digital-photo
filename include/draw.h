#ifndef _DRAW_H
#define _DRAW_H

int open_text_file(char *pc_file_name);
int set_text_detail(char *pc_hzk_file, char *pc_file_free_type, unsigned int dw_font_size);
int select_and_init_display(char *pc_name);
int show_next_page(void);
int show_pre_page(void);

#endif /*_DRAW_H*/
