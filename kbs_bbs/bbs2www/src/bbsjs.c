#include "bbslib.h"

int main()
{
  	FILE *fp;
    	static char s[300];
	init_all();
	printf("<font style=\"font-size:12px\">\n");
  	printf("<center>��ӭ����[%s], Ŀǰ��������(www/all) [<font color=\"green\">%d/%d</font>]", 
    		count_www(), count_online());
	printf("</font>");
}

