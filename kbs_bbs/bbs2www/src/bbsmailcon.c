/*
 * $Id$
 */
#include "bbslib.h"

int main()
{
    FILE *fp;
    char buf[512], dirname[15], dir[80], file[80], path[80], *ptr, *id,title[20];
    struct fileheader x;
    int num, tmp, total;

    initwww_all();
    strsncpy(file, getparm("file"), 32);
    strsncpy(dirname, getparm("dir"), 15);
    strsncpy(title,getparm("title"),20);
    num = atoi(getparm("num"));
    printf("<center>\n");
    id = getCurrentUser()->userid;
    printf("%s -- �Ķ��ż� [ʹ����: %s]<hr>\n", BBSNAME, id);
    if (strncmp(file, "M.", 2))
        http_fatal("����Ĳ���1");
    if (strstr(file, "..") || strstr(file, "/"))
        http_fatal("����Ĳ���2");
    if (strstr(dir, "..") || strstr(dir, "/"))
        http_fatal("����Ĳ���3");
    if (dirname[0]==0)
        strcpy(dirname,".DIR");
    sprintf(dir, "mail/%c/%s/%s", toupper(id[0]), id, dirname);
    total = file_size(dir) / sizeof(x);
    if (total < 0)
        http_fatal("����Ĳ���3");
    if (total==0)
	http_fatal("�ż��ѱ�ɾ��");
    printf("<table width=610 border=1>\n");
    printf("<tr><td>\n<pre>");
    sprintf(path, "mail/%c/%s/%s", toupper(id[0]), id, file);
    fp = fopen(path, "r");
    if (fp == 0)
        printf("���Ĳ����ڻ����ѱ�ɾ��</pre>\n</table><hr>\n");
    else
    {
    while (1) {
        if (fgets(buf, 512, fp) == 0)
            break;
        hhprintf("%s", void1(buf));
    }
    fclose(fp);
    printf("</pre>\n</table><hr>\n");
    printf("[<a onclick='return confirm(\"�����Ҫɾ���������?\")' href=/bbsdelmail.php?file=%s&dir=%s&title=%s> ɾ��</a>]", file,dirname,title);
    }
    fp = fopen(dir, "r+");
    if (fp == 0)
        http_fatal("dir error2");
    if (num > 0) {
        fseek(fp, sizeof(x) * (num - 1), SEEK_SET);
        fread(&x, sizeof(x), 1, fp);
        printf("[<a href=bbsmailcon?file=%s&dir=%s&num=%d>��һƪ</a>]", x.filename, dirname, num - 1);
    }
    printf("[<a href=\"/bbsreadmail.php\">�����ż��б�</a>]");
    if (num < total - 1) {
        fseek(fp, sizeof(x) * (num + 1), SEEK_SET);
        fread(&x, sizeof(x), 1, fp);
        printf("[<a href=bbsmailcon?file=%s&dir=%s&num=%d>��һƪ</a>]", x.filename, dirname, num + 1);
    }
    if (num >= 0 && num < total) {
        char title2[80];

        fseek(fp, sizeof(x) * num, SEEK_SET);
        if (fread(&x, sizeof(x), 1, fp) > 0) {
            x.accessed[0] |= FILE_READ;
            fseek(fp, sizeof(x) * num, SEEK_SET);
            fwrite(&x, sizeof(x), 1, fp);
            ptr = strtok(x.owner, " (");
            if (ptr == 0)
                ptr = " ";
            strcpy(title2, x.title);
            if (strncmp(x.title, "Re:", 3))
                sprintf(title2, "Re: %s", x.title);
            title2[60] = 0;
            printf("[<a href='/bbspstmail.php?userid=%s&file=%s&title=%s'>����</a>]", ptr, x.filename, title2);
        }
    }
    fclose(fp);
    printf("</center>\n");
    http_quit();
}
