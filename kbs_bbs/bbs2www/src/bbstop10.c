/*#include "bbslib.h"*/
#include "bbslib.h"

int main() {
	FILE *fp;
	int n;
	char s1[256], s2[256], s3[256], s4[256],s5;
	char brd[256], id[256], title[256], num[100];
	init_all();
	printf("<center>%s -- ����ʮ�����Ż���\n<hr>\n", BBSNAME);
	fp=fopen("etc/posts/day", "r");
	if(fp==0) http_fatal("can't read data");
	fgets(s1, 255, fp);
	fgets(s1, 255, fp);
	printf("<table border=\"1\" width=\"610\">\n");
	printf("<tr><td>����<td>������<td>����<td>����<td>����\n");
	for(n=1; n<=10; n++)
       	{
		if(fgets(s1, 255, fp) == NULL)
		       	break;
		sscanf(s1+41, "%s", brd);
		sscanf(s1+120, "%s", id);
		sscanf(s1+99, "%s", num);
		if(fgets(s1, 255, fp) == NULL)
		       	break;
		strsncpy(title, s1+27, 60);
		printf("<tr><td>�� %d ��<td><a href=\"bbsdoc?board=%s\">%s</a><td><a href='bbstfind?board=%s&title=%s'>%42.42s</a><td><a href=bbsqry?userid=%s>%12s</a><td>%s\n",
   n, brd, brd, brd, nohtml(title), nohtml(title), id, id, num);
	}
		printf("</table><center>");
		http_quit();
}
