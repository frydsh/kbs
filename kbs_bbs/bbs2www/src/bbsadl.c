#include "bbslib.h"

int main() {
	int i, no=0;
	char brd[100], buf[256];
	bcache_t *bc;

	init_all();
	bc = getbcacheaddr();
	printf("<nobr><center>%s -- ���������ط���<hr color=green>\n", BBSNAME);
	printf("<table>\n");
	printf("<tr><td>���<td>����<td>��С(�ֽ�)<td>����ʱ��\n");
	for(i=0; i<MAXBOARD; i++) {
		strsncpy(brd, bc[i].filename, 60);
		sprintf(buf, "www/an/%s.tgz");
		if(!file_exist(buf)) continue;
		no++;
		printf("<tr><td>%d<td><a href=/an/%s.tgz>%s.tgz</a><td>%d<td>%s\n", 
			no, brd, brd, file_size(buf), wwwCTime(file_time(buf))+4);
	}
	printf("</table>");
}
