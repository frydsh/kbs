#include "bbslib.h"
char *stat1();

int main() {
	FILE *fp;
	char board[80], dir[80], *ptr;
	bcache_t *x1;
	struct fileheader *data;
	int i, start, total2=0, total, sum=0;

 	init_all();
	strsncpy(board, getparm("board"), 32);
	x1=getbcache(board);
	if(x1==0) http_fatal("�����������");
	strcpy(board, x1->filename);
	if(!has_read_perm(currentuser, board)) http_fatal("�����������");
	sprintf(dir, "boards/%s/.DIR", board);
        fp=fopen(dir, "r");
        if(fp==0) http_fatal("�����������Ŀ¼");
	total=file_size(dir)/sizeof(struct fileheader);
	data=calloc(sizeof(struct fileheader), total);
	if(data==0) http_fatal("�ڴ����");
	total=fread(data, sizeof(struct fileheader), total, fp);
	fclose(fp);
	for(i=0; i<total; i++) if(strncmp(data[i].title, "Re:",3)) total2++;
	start=atoi(getparm("start"));
        if(strlen(getparm("start"))==0 || start>total2-19) start=total2-19;
  	if(start<0) start=0;
	printf("<nobr><center>\n");
	printf("%s -- �����Ķ�: [������: %s] ����[%s] ����%d, ����%d��<hr color=green>\n", 
		BBSNAME, board, userid_str(x1->BM), total, total2);
	if(total<=0) http_fatal("��������Ŀǰû������");
	printf("<table width=610>\n");
      	printf("<tr><td>���<td>״̬<td>����<td>����<td>����<td>����/����\n");
	for(i=0; i<total; i++) {
		if(!strncmp(data[i].title, "Re:", 3)) continue;
		sum++;
		if(sum-1<start) continue;
		if(sum-1>start+19) break;
		printf("<tr><td>%d<td>%s<td>%s",
			sum+1, flag_str(data[i].accessed[0]), userid_str(data[i].owner));
         	printf("<td>%6.6s", wwwCTime(atoi(data[i].filename+2))+4);
        	printf("<td><a href=bbstcon?board=%s&file=%s>�� %38.38s </a><td>%s",
			board, data[i].filename,
			nohtml(data[i].title), stat1(data, i, total));
      	}
      	printf("</table><hr>\n");
	if(start>0)
		printf("<a href=bbstdoc?board=%s&start=%d>��һҳ</a> ", board, start-19);
	if(start<total2-19)
		printf("<a href=bbstdoc?board=%s&start=%d>��һҳ</a> ", board, start+19);
	printf("<a href=bbsnot?board=%s>���滭��</a> ", board);
	printf("<a href=bbsdoc?board=%s>һ��ģʽ</a> ", board);
	printf("<a href=bbsgdoc?board=%s>��ժ��</a> ", board);
	printf("<a href=bbs0an?path=%s>������</a> ", anno_path_of(board));
	printf("<a href=/an/%s.tgz>���ؾ�����</a> ", board);
	printf("<a href=bbspst?board=%s>��������</a> <br>\n", board);
	free(data);
        printf("<form action=bbstdoc?board=%s method=post>\n", board);
 	printf("<input type=submit value=��ת��> �� <input type=text name=start size=4> ƪ");
 	printf("</form>\n");
	http_quit();
}

char *stat1(struct fileheader *data, int from, int total) {
	static char buf[256];
	char *ptr=data[from].title;
	int i, re=0, click=*(int*)(data[from].title+73);
	for(i=from; i<total; i++) {
		if(!strncmp(ptr, data[i].title+4, 40)) {
			re++;
			click+=*(int*)(data[i].title+73);
		}
	}
	sprintf(buf, "<font color=%s>%d</font>/<font color=%s>%d</font>", 
		re>9 ? "red" : "black", re, click>499 ? "red" : "black", click);
	return buf;
}
