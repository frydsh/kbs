/*
 * $Id$
 */
#include "bbslib.h"

int main()
{
    FILE *fp;
    char *ptr, path[256], buf[256], buf1[256], buf2[256];
    int t_lines = 20, link_mode = 0, def_mode = 0, type;

    initwww_all();
    if (!loginok)
        http_fatal("�Ҵҹ��Ͳ��ܶ��ƽ���");
    sprintf(path, "home/%c/%s/.mywww", toupper(getCurrentUser()->userid[0]), getCurrentUser()->userid);
    fp = fopen(path, "r");
    if (fp) {
        while (1) {
            if (fgets(buf, 80, fp) == 0)
                break;
            if (sscanf(buf, "%80s %80s", buf1, buf2) != 2)
                continue;
            if (!strcmp(buf1, "t_lines"))
                t_lines = atoi(buf2);
            if (!strcmp(buf1, "link_mode"))
                link_mode = atoi(buf2);
            if (!strcmp(buf1, "def_mode"))
                def_mode = atoi(buf2);
        }
        fclose(fp);
    }
    type = atoi(getparm("type"));
    ptr = getparm("t_lines");
    if (ptr[0])
        t_lines = atoi(ptr);
    ptr = getparm("link_mode");
    if (ptr[0])
        link_mode = atoi(ptr);
    ptr = getparm("def_mode");
    if (ptr[0])
        def_mode = atoi(ptr);
    printf("<center>%s -- WWW���˶��� [ʹ����: %s]<hr color=green>", BBSNAME, getCurrentUser()->userid);
    if (type > 0) {
        save_set(path, t_lines, link_mode, def_mode);
        return;
    }
    printf("<table>\n");
    if (t_lines < 10 || t_lines > 40)
        t_lines = 20;
    if (link_mode < 0 || link_mode > 1)
        link_mode = 0;
    printf("<tr><td><form action=bbsmywww>\n");
    printf("<input type=hidden name=type value=1>");
    printf("һ����ʾ����������(10-40): <input name=t_lines size=8 value=%d><br>\n", t_lines);
    printf("����ʶ�� (0ʶ��, 1��ʶ��): <input name=link_mode size=8 value=%d><br>\n", link_mode);
    printf("ȱʡģʽ (0����, 1һ��): &nbsp;&nbsp;<input name=def_mode size=8 value=%d><br><br>\n", def_mode);
    printf("<tr><td align=center><input type=submit value=ȷ��> <input type=reset value=��ԭ>\n");
    printf("</form>\n");
}

int save_set(char *path, int t_lines, int link_mode, int def_mode)
{
    FILE *fp;
    char buf[80];

    if (t_lines < 10 || t_lines > 40)
        http_fatal("���������");
    if (link_mode < 0 || link_mode > 1)
        http_fatal("���������ʶ�����");
    if (def_mode < 0 || def_mode > 1)
        http_fatal("�����ȱʡģʽ");
    fp = fopen(path, "w");
    fprintf(fp, "t_lines %d\n", t_lines);
    fprintf(fp, "link_mode %d\n", link_mode);
    fprintf(fp, "def_mode %d\n", def_mode);
    fclose(fp);
    sprintf(buf, "%d", t_lines);
    setcookie("my_t_lines", buf);
    sprintf(buf, "%d", link_mode);
    setcookie("my_link_mode", buf);
    sprintf(buf, "%d", def_mode);
    setcookie("my_def_mode", buf);
    printf("WWW���Ʋ����趨�ɹ�.<br>\n");
    printf("[<a href='javascript:history.go(-2)'>����</a>]");
}
