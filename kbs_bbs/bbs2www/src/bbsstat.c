/*
 * $Id$
 */
#include "bbslib.h"

int main()
{
    FILE *fp;
    struct userec x;
    int logins = 0, posts = 0, stays = 0, lifes = 0, total = 0;

    initwww_all();
    if (!loginok)
        http_fatal("�Ҵҹ��Ͳ���������");
    fp = fopen(".PASSWDS", "r");
    while (1) {
        if (fread(&x, sizeof(x), 1, fp) <= 0)
            break;
        if (x.userid[0] < 'A')
            continue;
        if (x.userlevel == 0)
            continue;
        if (x.numposts >= getCurrentUser()->numposts)
            posts++;
        if (x.numlogins >= getCurrentUser()->numlogins)
            logins++;
        if (x.stay >= getCurrentUser()->stay)
            stays++;
        if (x.firstlogin <= getCurrentUser()->firstlogin)
            lifes++;
        total++;
    }
    fclose(fp);
    printf("<center>%s -- ��������ͳ�� [ʹ����: %s]<hr color=green>\n", BBSNAME, getCurrentUser()->userid);
    printf("<table width=320><tr><td>��Ŀ<td>��ֵ<td>ȫվ����<td>��Ա���\n");
    printf("<tr><td>��վ����<td>%d��<td>%d<td>TOP %5.2f%%", (time(0) - getCurrentUser()->firstlogin) / 86400, lifes, (lifes * 100.) / total);
    printf("<tr><td>��վ����<td>%d��<td>%d<td>TOP %5.2f%%", getCurrentUser()->numlogins, logins, logins * 100. / total);
    printf("<tr><td>��������<td>%d��<td>%d<td>TOP %5.2f%%", getCurrentUser()->numposts, posts, posts * 100. / total);
    printf("<tr><td>����ʱ��<td>%d��<td>%d<td>TOP %5.2f%%", getCurrentUser()->stay / 60, stays, stays * 100. / total);
    printf("</table><br>���û���: %d", total);
}
