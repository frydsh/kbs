/*
 * $Id$
 */
#include "bbslib.h"

uinfo_t user[USHM_SIZE];

int cmpuser(a, b)
    uinfo_t *a, *b;
{
    char id1[80], id2[80];

    sprintf(id1, "%d%s", !isfriend(a->userid), a->userid);
    sprintf(id2, "%d%s", !isfriend(b->userid), b->userid);
    return strcasecmp(id1, id2);
}

int main()
{
    int i, start, total = 0, total2 = 0, fh, shmkey, shmid;
    uinfo_t *x;
    char search;
    uinfo_t **usr;

    initwww_all();
    fill_userlist();
    usr = get_ulist_addr();
    printf("<center>\n");
    printf("%s -- �����û���ѯ [����������: %d��]<hr>\n", BBSNAME, count_online());
    for (i = 0; i < USHM_SIZE; i++) {
        x = usr[i];
        if (x == NULL || x->active == 0)
            continue;
        if (x->invisible && !HAS_PERM(getCurrentUser(), PERM_SEECLOAK))
            continue;
        memcpy(&user[total], x, sizeof(uinfo_t));
        total++;
    }
    search = toupper(getparm("search")[0]);
    if ( /*search!='*' && */ (search < 'A' || search > 'Z'))
        http_fatal("����Ĳ���");
    if (search == '*') {
        printf("��������ʹ����<br>\n");
    } else {
        printf("��ĸ'%c'��ͷ������ʹ����.<br>\n", search);
    }

    printf("<table border=\"1\" width=\"610\">\n");
    printf("<tr><td>���<td>��<td>ʹ���ߴ���<td>ʹ�����ǳ�<td>����<td>��̬<td>����\n");
    qsort(user, total, sizeof(uinfo_t), cmpuser);
    for (i = 0; i < total; i++) {
        int dt = (time(0) - get_idle_time(&user[i])) / 60;

        if (toupper(user[i].userid[0]) != search && search != '*')
            continue;
        printf("<tr><td>%d</td>", i + 1);
        printf("<td>%s", isfriend(user[i].userid) ? "��" : "  ");
        printf("%s</td>", user[i].invisible ? "<font color=\"green\">C</font>" : " ");
        printf("<td><a href=\"/bbsqry.php?userid=%s\">%s</a></td>", user[i].userid, user[i].userid);
        printf("<td><a href=\"/bbsqry.php?userid=%s\">%24.24s </a></td>", user[i].userid, nohtml(user[i].username));
        printf("<td>%20.20s </td>", user[i].from);
        printf("<td>%s</td>", user[i].invisible ? "������..." : ModeType(user[i].mode));
        if (dt == 0)
            printf("<td> </td></tr>\n");
        else
            printf("<td>%d</td></tr>\n", dt);
        total2++;
    }
    printf("</table>\n");
    printf("��������: %d��", total2);
    printf("<hr>");
    /*if(search!='*')
       printf("[<a href=\"bbsufind?search=*\">ȫ��</a>] "); */
    for (i = 'A'; i <= 'Z'; i++) {
        if (i == search)
            printf("[%c]", i);
        else
            printf("[<a href=\"bbsufind?search=%c\">%c</a>]", i, i);
    }
    printf("<br>\n");
    printf("[<a href=\"javascript:history.go(-1)\">����</a>] [<a href=\"/bbsuser.php\">һ��ģʽ</a>] ");
    printf("</center>\n");
    http_quit();
}
