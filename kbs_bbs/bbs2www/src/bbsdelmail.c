/*
 * $Id$
 */
#include "bbslib.h"
/*
int main() {
	FILE *fp;
	struct fileheader f;
	char path[80], file[80], *id;
	int num=0;
	initwww_all();
	if(loginok == 0) http_fatal("����δ��¼");
	id=getCurrentUser()->userid;
	strsncpy(file, getparm("file"), 20);
	if(strncmp(file, "M.", 2) || strstr(file, "..")) http_fatal("����Ĳ���");
	sprintf(path, "mail/%c/%s/.DIR", toupper(id[0]), id);
	fp=fopen(path, "r");
	if(fp==0) http_fatal("����Ĳ���2");
	while(1) {
		if(fread(&f, sizeof(f), 1, fp)<=0) break;
		num++;
		if(!strcmp(f.filename, file)) {
			fclose(fp);
			delete_record(path, sizeof(struct fileheader), num-1);
			printf("�ż���ɾ��.<br><a href=bbsmail>���������ż��б�</a>\n");
			http_quit();
		}
	}
	fclose(fp);
	http_fatal("�ż�������, �޷�ɾ��");
}*/
int main()
{
    FILE *fp;
    struct fileheader f;
    char path[80], file[80], *id,dirname[15],title[20];
    int num = 0;

    initwww_all();
    if (loginok == 0)
        http_fatal("����δ��¼");
    id = getCurrentUser()->userid;
    strsncpy(file, getparm("file"), 20);
    strsncpy(dirname, getparm("dir"), 15);
    strsncpy(title,getparm("title"),20);

    if (strncmp(file, "M.", 2) || strstr(file, ".."))
        http_fatal("����Ĳ���");
    sprintf(path, "mail/%c/%s/%s", toupper(id[0]), id,dirname);
    fp = fopen(path, "r");
    if (fp == 0)
        http_fatal("����Ĳ���2");
    while (1) {
        if (fread(&f, sizeof(f), 1, fp) <= 0)
            break;
        num++;
        if (!strcmp(f.filename, file)) {
            fclose(fp);
            sprintf(path, "mail/%c/%s/%s", toupper(id[0]), id, dirname);
            del_mail(num, &f, path);
	    printf("�ż���ɾ��.<br><a href=\"/bbsreadmail.php?path=%s&title=%s\">�����ż��б�</a>\n",dirname,title);
            http_quit();
        }
    }
    fclose(fp);
    http_fatal("�ż�������, �޷�ɾ��");
}
