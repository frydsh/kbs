#include "bbslib.h"

int main() {
   	FILE *fp;
   	int type=0, i, num;
	char userid[80], buf[512], path[512], file[512], board[512], title[80]="";
   	bcache_t *brd;
   	struct fileheader  *x;

   	init_all();
	if(!loginok) http_fatal("�Ҵҹ��Ͳ����޸����£����ȵ�¼");
	strsncpy(board, getparm("board"), 20);
	type=atoi(getparm("type"));
	brd=getbcache(board);
	if(brd==0) http_fatal("�����������");
	strcpy(board, brd->filename);
	strsncpy(file, getparm("file"), 20);
	if(!haspostperm(currentuser, board))
		http_fatal("�������������������Ȩ�ڴ���������������");
	if (valid_filename(file) < 0)
		http_fatal("����Ĳ���");
   	x=get_file_ent(board, file);
	/*if(strstr(file, "..") || strstr(file, "/")) http_fatal("����Ĳ���");*/
	if(x==0) http_fatal("����Ĳ���");
	/* �˴���δ���վ��Ͱ��� */
	if(strcmp(x->owner, currentuser->userid)) http_fatal("����Ȩ�޸Ĵ�����");
	/* added by flyriver, 2001.12.10 */
	/* ͬ��ID�����޸���ID������ */
#ifdef HAPPY_BBS
	if (x->posttime < currentuser->firstlogin)
		http_fatal("����Ȩ�޸Ĵ�����");
#else
	if (file_time(file) < currentuser->firstlogin)
		http_fatal("����Ȩ�޸Ĵ�����");
#endif /* HAPPY_BBS */
	printf("<center>%s -- �޸����� [ʹ����: %s]<hr color=green>\n", BBSNAME, currentuser->userid);
	if(type!=0) return update_form(board, file);
   	printf("<table border=1>\n");
	printf("<tr><td>");
	printf("<tr><td><form method=post action=bbsedit>\n");
   	printf("ʹ�ñ���: %s ������: %s<br>\n", nohtml(x->title), board);
   	printf("�������ߣ�%s<br>\n", currentuser->userid);
   	printf("<textarea name=text rows=20 cols=80 wrap=physicle>");
	sprintf(path, "boards/%s/%s", board, file);
	fp=fopen(path, "r");
	if(fp==0) http_fatal("�ļ���ʧ");
	/* ���ﻹ��Ҫ��Ӵ��룬�Է�ֹ�û��޸��ļ�ͷ����Դ */
	while(1) {
		if(fgets(buf, 500, fp)==0) break;
		if(!strcasestr(buf, "</textarea>")) printf("%s", buf);
	}
	fclose(fp);
   	printf("</textarea>\n");
   	printf("<tr><td class=post align=center>\n");
	printf("<input type=hidden name=type value=1>\n");
	printf("<input type=hidden name=board value=%s>\n", board);
	printf("<input type=hidden name=file value=%s>\n", file);
	printf("<input type=submit value=����> \n");
   	printf("<input type=reset value=����></form>\n");
	printf("</table>");
	http_quit();
}

int update_form(char *board, char *file) {
	FILE *fp;
	char *buf=getparm("text"), path[80];
	sprintf(path, "boards/%s/%s", board, file);
	fp=fopen(path, "w");
	if(fp==0) http_fatal("�޷�����");
	fprintf(fp, "%s", buf);
	fprintf(fp, "\n�� �޸�:��%s � %s �޸ı��ģ�[FROM: %s] ", currentuser->userid, wwwCTime(time(0))+4, fromhost);
	fclose(fp);
	printf("�޸����³ɹ�.<br><a href=bbsdoc?board=%s>���ر�������</a>", board);
}
