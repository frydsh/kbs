/*
 * $Id$
 */
#include "bbslib.h"

int main()
{
    FILE *fp;
    char buf[256], tmp[256], name[256], cname[256], cc[256];
    int i, r;

    initwww_all();
    fp = fopen("0Announce/bbslists/board2", "r");
    if (fp == 0)
        http_fatal("error 1");
    printf("<center>%s -- ����������<hr color=\"green\">", BBSNAME);
    printf("<table border=\"1\">\n");
    printf("<tr><td>����</td><td>����</td><td>���İ���</td><td>����</td></tr>\n");
    for (i = 0; i <= 15; i++) {
        if (fgets(buf, 150, fp) == 0)
            break;
        if (i == 0)
            continue;
        r = sscanf(buf, "%s %s %s %s %s %s", tmp, tmp, name, tmp, cname, cc);
        if (r == 6) {
            printf("<tr><td>%d</td><td><a href=\"/bbsdoc.php?board=%s\">%s</a></td><td width=\"200\"><a href=\"/bbsdoc.php?board=%s\">%s</a></td><td>%s</td></tr>\n", i, name, name, name, cname, cc);
        }
    }
    printf("</table>\n</center>\n");
    fclose(fp);
}
