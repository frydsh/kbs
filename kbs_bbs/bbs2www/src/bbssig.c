/*
 * $Id$
 */
#include "bbslib.h"
FILE *fp;

int main()
{
    FILE *fp;
    char *ptr, path[256], buf[10000];

    initwww_all();
    if (!loginok)
        http_fatal("�Ҵҹ��Ͳ�������ǩ���������ȵ�¼");
    printf("<center>%s -- ����ǩ���� [ʹ����: %s]<hr>\n", BBSNAME, getCurrentUser()->userid);
	sethomefile(path, getCurrentUser()->userid, "signatures");
    if (!strcasecmp(getparm("type"), "1"))
        save_sig(path);
    printf("<form method=\"post\" action=\"bbssig?type=1\">\n");
    fp = fopen(path, "r");
    bzero(buf, 10000);
    if (fp) {
        fread(buf, 9999, 1, fp);
        ptr = strcasestr(buf, "<textarea>");
        if (ptr)
            ptr[0] = 0;
        fclose(fp);
    }
    printf("ǩ����ÿ6��Ϊһ����λ, �����ö��ǩ����.<table width=\"610\" border=\"1\"><tr><td>");
    printf("<textarea name=\"text\" rows=\"20\" cols=\"80\" wrap=\"physical\">\n");
    printf("%s", void1(buf));
    printf("</textarea></table>\n");
    printf("<input type=\"submit\" value=\"����\"> ");
    printf("<input type=\"reset\" value=\"��ԭ\">\n");
    printf("</form><hr>\n");
    http_quit();
}

int save_sig(char *path)
{
    char *buf;

    fp = fopen(path, "w");
    buf = getparm("text");
    fprintf(fp, "%s", unix_string(buf));
    fclose(fp);
    printf("ǩ�����޸ĳɹ���");
    http_quit();
}
