#include "bbslib.h"

int main() {
	FILE *fp;
	struct fileheader x;
	int filetime, i, total=0, total2=0, type;
	char *ptr, buf[512], path[80], dir[80];
   	init_all();
	if(!loginok) http_fatal("����δ��¼, ���ȵ�¼");
   	printf("<center>\n");
   	printf("%s -- ���ʼ��б� [ʹ����: %s]<hr color=green>\n", BBSNAME, currentuser->userid);
   	sprintf(dir, "mail/%c/%s/.DIR", toupper(currentuser->userid[0]), currentuser->userid);
	fp=fopen(dir, "r");
	if(fp==0) http_fatal("Ŀǰ��������û���κ��ż�");
      	printf("<table width=610 border=1>\n");
      	printf("<tr><td>���<td>״̬<td>������<td>����<td>�ż�����\n");
      	while(1) {
		if(fread(&x, sizeof(x), 1, fp)<=0) break;
		total++;
		if(x.accessed[0] & FILE_READ) continue;
		printf("<tr><td>%d<td>N", total);
 		ptr=strtok(x.owner, " (");
		if(ptr==0) ptr=" ";
		ptr=nohtml(ptr);
		printf("<td><a href=bbsqry?userid=%s>%13.13s</a>", ptr, ptr);
         	printf("<td>%6.6s", wwwCTime(atoi(x.filename+2))+4);
         	printf("<td><a href=bbsmailcon?file=%s&num=%d>", x.filename, total-1);
	 	if(strncmp("Re: ", x.title, 4)) printf("�� ");
         	hprintf("%42.42s", void1(x.title));
	 	printf(" </a>\n");
		total2++;
      	}
      	printf("</table><hr>\n");
	printf("�������乲��%d���ż�, ��������%d��.", total, total2);
	http_quit();
}
