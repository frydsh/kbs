#include "bbslib.h"

char *eff_size();
int my_t_lines;

int main() {
	FILE *fp;
	char board[80], dir[80], *ptr;
	bcache_t *x1;
	struct fileheader x;
	int i, start, total;

 	init_all();
	strsncpy(board, getparm("board"), 32);
	x1=getbcache(board);
	if(x1==0) http_fatal("�����������");
	strcpy(board, x1->filename);
	if(!has_read_perm(currentuser, board)) http_fatal("�����������");
	getcwd(dir, sizeof(dir)-1);
	sprintf(dir, "boards/%s/.DIR", board);
        if ((fp=fopen(dir, "r")) == NULL)
			http_fatal("Open .DIR failed.");
        total=file_size(dir)/sizeof(struct fileheader);
	start=atoi(getparm("start"));
	my_t_lines=atoi(getparm("my_t_lines"));
	if(my_t_lines<10 || my_t_lines>40) my_t_lines=20;
        if(strlen(getparm("start"))==0 || start>total-my_t_lines) start=total-my_t_lines;
  	if(start<0) start=0;
	brc_initial(currentuser->userid, board);
	printf("<nobr><center>\n");
	printf("%s -- [������: %s] ����[%s] ������[%d] <a href=\"bbsbrdadd?board=%s\">Ԥ������</a><hr color=\"green\">\n", 
		BBSNAME, board, userid_str(x1->BM), total, board);
	if(total<=0) http_fatal("��������Ŀǰû������");
      	printf("<table width=\"613\">\n");
      	printf("<tr><td>���</td><td>״̬</td><td>����</td><td>����</td><td>����</td></tr>\n");
	if(fp) {
	fseek(fp, start*sizeof(struct fileheader), SEEK_SET);
      	for(i=0; i<my_t_lines; i++) {
		char filename[80];
		char *ptr, *font1="", *font2="";
		if(fread(&x, sizeof(x), 1, fp)<=0) break;
		ptr=flag_str2(x.accessed[0], brc_has_read(x.filename));
		if(ptr[0]=='N') {
			font1="<font color=\"#909090\">";
			font2="</font>";
		}
		sprintf(filename, "boards/%s/%s", board, x.filename);
		printf("<tr><td>%d</td><td>%s%s%s</td><td>%s</td>",
			start+i+1, font1, ptr, font2, userid_str(x.owner));
			/* ֻ��ʾ���� */
         	printf("<td>%6.6s</td>", wwwCTime(atoi(x.filename+2))+4);
			/* ȥ��ͳ���ֽ����Ĺ���, �˷�ϵͳ��Դ */
         	printf("<td><a href=\"bbscon?board=%s&file=%s&num=%d\">%s%36.36s </a></td>",
			board, x.filename, start+i,
			strncmp(x.title, "Re: ", 4) ? "�� " : "",
			void1(nohtml(x.title)));
		/* ȥ������ֵ���� */
		printf("</tr>\n");
      	}
      	printf("</table><hr>\n");
	}
	/*printf("<a href=bbsfdoc?board=%s>�ļ�����</a> ", board);*/
	printf("<a href=\"bbspst?board=%s\">��������</a> \n", board);
	printf("<a href=\"javascript:location=location\">ˢ��</a> ");
	if(start>0)
		printf("<a href=\"bbsdoc?board=%s&start=%d\">��һҳ</a> ", board, start-my_t_lines);
	if(start<total-20)
		printf("<a href=\"bbsdoc?board=%s&start=%d\">��һҳ</a> ", board, start+my_t_lines);
	printf("<a href=\"bbstdoc?board=%s\">����ģʽ</a> ", board);
	if(has_BM_perm(currentuser, board)) 
		printf("<a href=\"bbsmdoc?board=%s\">����ģʽ</a> ", board);
	printf("<a href=\"bbsnot?board=%s\">���滭��</a> ", board);
	printf("<a href=\"bbsgdoc?board=%s\">��ժ��</a> ", board);
	printf("<a href=\"bbs0an?path=%s\">������</a> ", anno_path_of(board));
	printf("<a href=\"/an/%s.tgz\">���ؾ�����</a> ", board);
	printf("<a href=\"bbsbfind?board=%s\">���ڲ�ѯ </a>\n", board);
	if(loginok) 
		printf("<a href=\"bbsclear?board=%s&start=%d\">���δ��</a>\n", board, start);
	fclose(fp);
	printf("<form name=\"form1\" action=\"bbsdoc?board=%s\" method=\"post\">\n", board);
	printf("<input type=\"submit\" value=\"��ת��\"> �� <input type=\"text\" name=\"start\" size=\"4\"> ƪ");
	printf("</form>\n");
	http_quit();
}

char *eff_size(char *file) {
	FILE *fp;
	static char buf[256];
	int i, size, size2=0;
	size=file_size(file);
	if(size>3000|| size==0) goto E;
	size=0;
	fp=fopen(file, "r");
	if(fp==0) return "-";
	for(i=0; i<3; i++)
		if(fgets(buf, 255, fp)==0) break;
	while(1) {
		if(fgets(buf, 255, fp)==0) break;
		if(!strcmp(buf, "--\n")) break;
		if(!strncmp(buf, ": ", 2)) continue;
		if(!strncmp(buf, "�� �� ", 4)) continue;
		if(strstr(buf, "�� ��Դ:��")) continue;
		for(i=0; buf[i]; i++) if(buf[i]<0) size2++;
		size+=strlen(trim(buf));
	}	
	fclose(fp);
E:	
	if(size<2048) {
		sprintf(buf, "(<font style='font-size:12px; color:#008080'>%d��</font>)", size-size2/2);
	} else {
		sprintf(buf, "(<font style='font-size:12px; color:#f00000'>%d.%dǧ��</font>)", size/1000, (size/100)%10);
	}
	return buf;
}
