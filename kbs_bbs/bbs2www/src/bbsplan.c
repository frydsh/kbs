/*
 * $Id$
 */
#include "bbslib.h"
FILE *fp;

int main()
{
    FILE *fp;
    char *ptr, plan[256], buf[10000];

    initwww_all();
    printf("<center>\n");
    if (!loginok)
        http_fatal("�Ҵҹ��Ͳ�������˵���������ȵ�¼");
	sethomefile(plan, getCurrentUser()->userid, "plans");
    if (!strcasecmp(getparm("type"), "update"))
        save_plan(plan);
    printf("%s -- ���ø���˵���� [%s]<hr>\n", BBSNAME, getCurrentUser()->userid);
    printf("<form method=\"post\" action=\"bbsplan?type=update\">\n");
    fp = fopen(plan, "r");
    if (fp) {
        fread(buf, 9999, 1, fp);
        ptr = strcasestr(buf, "<textarea>");
        if (ptr)
            ptr[0] = 0;
        fclose(fp);
    }
    printf("<table width=\"610\" border=\"1\"><tr><td>");
    printf("<textarea name=\"text\" rows=\"20\" cols=\"80\" wrap=\"physical\">\n");
    printf("%s", void1(buf));
    printf("</textarea></table>\n");
    printf("<input type=\"submit\" value=\"����\"> ");
    printf("<input type=\"reset\" value=\"��ԭ\">\n");
    printf("<hr>\n");
    http_quit();
}

int save_plan(char *plan)
{
    char buf[10000];

    fp = fopen(plan, "w");
    strsncpy(buf, getparm("text"), 9999);
    fprintf(fp, "%s", unix_string(buf));
    fclose(fp);
    printf("����˵�����޸ĳɹ���");
    http_quit();
}
