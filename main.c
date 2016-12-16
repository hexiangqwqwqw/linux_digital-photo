#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <config.h>
#include <draw.h>
#include <encoding_manager.h>
#include <fonts_manager.h>
#include <disp_manager.h>
#include <input_manager.h>
#include <string.h>

/*./show_file [-s SIZE] [-f freetype_font_file] [-h HZK] <text_file>*/
int main(int argc, char **argv)
{
	int i_error;
	unsigned int dw_fontsize = 16;
	char ac_hzk_file[128];
	char ac_freetypefile[128];
	char ac_textfile[128];

	char ac_display[128];

	char c_opr;
	int b_list = 0;

	T_input_event t_input_event;

	 ac_hzk_file[0] = '\0';
	 ac_freetypefile[0] = '\0';
	 ac_textfile[0] = '\0';

	 strcpy(ac_display, "fb");

	 while((i_error = getopt(argc, argv, "ls:f:h:d:")) != -1)
	 {
		switch(i_error)
		{
			case 'l':
			{
				b_list = 1;
				break;
			}
			case 's':
			{
				dw_fontsize = strtoul(optarg, NULL, 0);
				break;
			}
			case 'f':
			{
				strncpy(ac_freetypefile, optarg, 128);
				ac_freetypefile[127] = '\0';
				break;
			}
			case 'h':
			{
				strncpy(ac_hzk_file, optarg, 128);
				ac_hzk_file[127] = '\0';
				break;
			}
			case 'd':
			{
				strncpy(ac_display, optarg, 128);
				ac_display[127] = '\0';
			}
			default :
			{
				printf("Usage: %s [-s size] [-d display] [-f font_file] [-h hzk] <text_file>\n",argv[0]);
				printf("Usage: %s -l\n", argv[0]);
				return -1;
				break;
			}
		}
	 }
	 if(!b_list && (optind >= argc))
	 {
		printf("Usage: %s [-s size] [-d display] [-f font_file] [-h hzk] <text_file>\n",argv[0]);
		printf("Usage: %s -l\n", argv[0]);
		return -1;
	 }

	 i_error = display_init();
	 if(i_error)
	 {
		printf("display init error!\n");
		return -1;
	 }

	 i_error = fonts_init();
	 if(i_error)
	 {
		printf("fonts init error\n");
		return -1;
	 }

	 i_error = encoding_init();
	 if(i_error)
	 {
		printf("encoding init error\n");
		return -1;
	 }

	 i_error = input_init();
	 if(i_error)
	 {
		printf("input init error\n");
		return -1;
	 }

	 if(b_list)
	 {
		printf("supported display: \n");
		show_disp_opr();

		printf("supported font: \n");
		show_font_opr();

		printf("suported encoding :\n");
		show_encoding_opr();

		printf("supported input: \n");
		show_input_opr();

		return 0;
	 }

	 strncpy(ac_textfile, argv[optind], 128);
	 ac_textfile[127] = '\0';

	 i_error = open_text_file(ac_textfile);
	 if(i_error)
	 {
		printf("open text ifle error!\n");
		return -1;
	 }

	 i_error = set_text_detail(ac_hzk_file,ac_freetypefile, dw_fontsize);
	 if(i_error)
	 {
	 	printf("set text file error!\n");
		return -1;
	 }

	 DBG_PRINTF("%s %s %d\n", __FILE__, __FUNCTION__, __LINE__);

	 i_error = select_and_init_display(ac_display); 
	if(i_error)
	{
		printf("select and  init dispaly error!\n");
		return -1;
	}

	i_error = all_input_devices_init();
	if(i_error)
	{
		DBG_PRINTF("error  all input devices init\n");
		return -1;
	}

	DBG_PRINTF("%s %s %d\n", __FILE__, __FUNCTION__, __LINE__);
	i_error = show_next_page();
	DBG_PRINTF("%s %s %d\n", __FILE__, __FUNCTION__, __LINE__);
	if(i_error)
	{
		printf("error to show first page\n");
		return -1;
	}

	printf("Enter 'n' to show next page, 'u' to show previous page, 'q' to exit :");

	while(1)
	{
		if (get_input_event(&t_input_event) == 0)
		{
			if (t_input_event.i_val == INPUT_VALUE_DOWN)
			{
				show_next_page();
			}
			else if (t_input_event.i_val == INPUT_VALUE_UP)
			{
				show_pre_page();
			}
			else if(t_input_event.i_val == INPUT_VALUE_EXIT)
			{
				return 0;
			}
		}
	}
	return 0;
	
}

