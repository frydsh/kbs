#include "bbslib.h"

int main() {
   	int i;

   	init_all();
	printf("<style type=text/css>A {color: #0000f0}</style>");
	printf("<center>\n");
   	printf("%s -- ���������� <hr color=green>\n", BBSNAME);
   	printf("<table>\n");
   	printf("<tr><td>����<td>���<td>����\n");
   	for(i=0; i<SECNUM; i++) {
      		printf("<tr><td>%d<td><a href=bbsboa?%d>%s</a>", i, i, secname[i][0]);
      		printf("<td><a href=bbsboa?%d>%s</a>\n", i, secname[i][1]);
   	}
   	printf("</table><hr>\n");
   	printf("[<a href=bbsall>ȫ��������</a>]");
   	printf("[<a href=bbs0an>����������</a>]");
   	printf("</center>\n");
	http_quit();
}
