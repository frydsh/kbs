/*
 * $Id$
 */
#include "bbslib.h"
char *stat1();

int main()
{
    FILE *fp;
    char board[80], dir[80], *ptr;
    char brdencode[256], buf[256];
    bcache_t *x1;
    struct fileheader *data;
    int i, start, total2 = 0, total, sum = 0;

    init_all();
    strsncpy(board, getparm("board"), 32);
    x1 = getbcache(board);
    if (x1 == 0)
        http_fatal("�����������");
    strcpy(board, x1->filename);
    if (!check_read_perm(currentuser, x1))
        http_fatal("�����������");
    sprintf(dir, "boards/%s/.DIR", board);
    fp = fopen(dir, "r");
    if (fp == 0)
        http_fatal("�����������Ŀ¼");
    total = file_size(dir) / sizeof(struct fileheader);
    data = calloc(sizeof(struct fileheader), total);
    if (data == 0)
        http_fatal("�ڴ����");
    total = fread(data, sizeof(struct fileheader), total, fp);
    fclose(fp);
    for (i = 0; i < total; i++)
        if (strncmp(data[i].title, "Re:", 3))
            total2++;
    start = atoi(getparm("start"));
    if (strlen(getparm("start")) == 0 || start > total2 - 19)
        start = total2 - 19;
    if (start < 0)
        start = 0;
    encode_url(brdencode, board, sizeof(brdencode));
    printf("<nobr><center>\n");
    printf("%s -- �����Ķ�: [������: %s] ����[%s] ����%d, ����%d��<hr color=\"green\">\n", BBSNAME, board, userid_str(x1->BM), total, total2);
    if (total <= 0)
        http_fatal("��������Ŀǰû������");
    printf("<table width=\"610\">\n");
    printf("<tr><td>���</td><td>״̬</td><td>����</td><td>����</td><td>����</td><td>����</td></tr>\n");
    for (i = 0; i < total; i++) {
        if (!strncmp(data[i].title, "Re:", 3))
            continue;
        sum++;
        if (sum - 1 < start)
            continue;
        if (sum - 1 > start + 19)
            break;
        printf("<tr><td>%d</td><td>%s</td><td>%s</td>", sum + 1, flag_str(data[i].accessed[0]), userid_str(data[i].owner));
        printf("<td>%6.6s</td>", wwwCTime(atoi(data[i].filename + 2)) + 4);
        printf("<td><a href=\"bbstcon?board=%s&gid=%d\">�� %38.38s </a></td><td>%s</td></tr>", brdencode, data[i].groupid, nohtml(data[i].title), stat1(data, i, total));
    }
    printf("</table><hr>\n");
    if (start > 0)
        printf("<a href=\"bbstdoc?board=%s&start=%d\">��һҳ</a> ", brdencode, start - 19);
    if (start < total2 - 19)
        printf("<a href=\"bbstdoc?board=%s&start=%d\">��һҳ</a> ", brdencode, start + 19);
    printf("<a href=\"/bbsnot.php?board=%s\">���滭��</a> ", brdencode);
    printf("<a href=\"/bbsdoc.php?board=%s\">һ��ģʽ</a> ", brdencode);
    printf("<a href=\"/bbsgdoc.php?board=%s\">��ժ��</a> ", brdencode);
    ann_get_path(board, buf, sizeof(buf));
    printf("<a href=\"bbs0an?path=%s\">������</a> ", http_encode_string(buf, sizeof(buf)));
    /*printf("<a href=/an/%s.tgz>���ؾ�����</a> ", board); */
    printf("<a href=\"bbspst.php?board=%s\">��������</a> <br>\n", brdencode);
    free(data);
    printf("<form action=\"bbstdoc?board=%s\" method=\"post\">\n", brdencode);
    printf("<input type=\"submit\" value=\"��ת��\"> �� <input type=\"text\" name=\"start\" size=\"4\"> ƪ");
    printf("</form>\n");
    http_quit();
}

char *stat1(struct fileheader *data, int from, int total)
{
    static char buf[256];
    char *ptr = data[from].title;
    int i, re = 0 /*, click=*(int*)(data[from].title+73) */ ;

    for (i = from; i < total; i++) {
        if (!strncmp(ptr, data[i].title + 4, 40)) {
            re++;
            /*click+=*(int*)(data[i].title+73); */
        }
    }
    sprintf(buf, "<font color=\"%s\">%d</font>", re > 9 ? "red" : "black", re);
    return buf;
}
