/*
 * $Id$
 */
#include "bbslib.h"
char day[20], user[20], title[80];

int main()
{
    char user[32], title3[80], title[80], title2[80];
    char board[BOARDNAMELEN+1];
    int day;

    init_all();
    strsncpy(user, getparm("user"), 13);
    strsncpy(title, getparm("title"), 50);
    strsncpy(title2, getparm("title2"), 50);
    strsncpy(title3, getparm("title3"), 50);
    strsncpy(board, getparm("board"), BOARDNAMELEN+1);
    day = atoi(getparm("day"));
    if (day == 0) {
        printf("%s -- վ�����²�ѯ<hr color=green>\n", BBSNAME);
        printf("<form action=bbsfind>\n");
        printf("��������: <input maxlength=12 size=12 type=text name=user> (���������������)<br>\n");
        printf("���⺬��: <input maxlength=60 size=20 type=text name=title>");
        printf(" AND <input maxlength=60 size=20 type=text name=title2><br>\n");
        printf("���ⲻ��: <input maxlength=60 size=20 type=text name=title3><br>\n");
        printf("�������: <input maxlength=5 size=5 type=text name=day value=7> �����ڵ�����<br><br>\n");
        printf("<input type=submit value=�ύ��ѯ>\n");
    } else {
        search(board,user, title, title2, title3, day * 86400);
    }
    http_quit();
}

int search(char* boardname,char *id, char *pat, char *pat2, char *pat3, int dt)
{
    FILE *fp;
    char board[256], dir[256], buf2[150];
    int total, now = time(0), i, sum = 0, n, t;
    struct fileheader x;
    boardheader_t *bc;

    bc = getbcacheaddr();       /* Assume shm_init() had been called. */
    printf("%s -- վ�����²�ѯ��� <br>\n", BBSNAME);
    printf("����: %s ", id);
    printf("���⺬��: '%s' ", nohtml(pat));
    if (pat2[0])
        printf("�� '%s' ", nohtml(pat2));
    if (pat3[0])
        printf("���� '%s'", nohtml(pat3));
    if (dt>0)
        printf("ʱ��: %d ��<br><hr color=green>\n", dt / 86400);
    for (i = 0; i < MAXBOARD; i++) {
        total = 0;
        if (boardname[0]&&strcasemp(bc[i].filename,boardname))
            continue;
        strcpy(board, bc[i].filename);
        if (!has_read_perm(currentuser, board))
            continue;
        sprintf(dir, "boards/%s/.DIR", board);
        fp = fopen(dir, "r");
        if (fp == 0)
            continue;
        n = 0;
        printf("<table width=610 border=1>\n");
        while (1) {
            n++;
            if (fread(&x, sizeof(x), 1, fp) <= 0)
                break;
            t = get_posttime(&x);
            if (id[0] != 0 && strcasecmp(x.owner, id))
                continue;
            if (pat[0] && !strcasestr(x.title, pat))
                continue;
            if ((dt>0) &&(abs(now - t) > dt))
                continue;
            if (pat2[0] && !strcasestr(x.title, pat2))
                continue;
            if (pat3[0] && strcasestr(x.title, pat3))
                continue;
            printf("<tr><td>%d<td><a href=/bbsqry.php?userid=%s>%s</a>", n, x.owner, x.owner);
            printf("<td>%6.6s", wwwCTime(get_posttime(&x)) + 4);
            printf("<td><a href=/bbscon.php?board=%s&id=%d>%s</a>\n", board, x.id, nohtml(x.title));
            total++;
            sum++;
            if (sum > 1999) {
                printf("</table> ....");
                http_quit();
            }
        }
        printf("</table>\n");
        if (total == 0)
            continue;
        printf("<br>����%dƪ���� <a href=/bbsdoc.php?board=%s>%s</a><br><br>\n", total, board, board);
    }
    printf("һ���ҵ�%dƪ���·��ϲ�������<br>\n", sum);
}
