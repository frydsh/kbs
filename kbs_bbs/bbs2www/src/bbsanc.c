/*
 * $Id$
 */
#include "bbslib.h"

void ann_show_file(char *path)
{
    FILE *fp;
    char board[STRLEN];
    char pathbuf[256];
    char buf[STRLEN];
    int len;

    if (strstr(path, ".Names")
        || strstr(path, "..") || strstr(path, "SYSHome"))
        http_fatal("������ļ���");
    if (path[0] != '\0') {
        len = strlen(path);
        if (path[len - 1] == '/')
            path[len - 1] = '\0';
        if (path[0] == '/')
            snprintf(pathbuf, sizeof(pathbuf), "0Announce%s", path);
        else
            snprintf(pathbuf, sizeof(pathbuf), "0Announce/%s", path);
        if (ann_traverse_check(pathbuf, getCurrentUser()) < 0)
            http_fatal("��Ŀ¼������");
    } else
        http_fatal("������ļ���");
    board[0] = '\0';
    ann_get_board(path, board, sizeof(board));
    if (board[0])
        snprintf(buf, sizeof(buf), "%s��", board);
    printf("<center>%s -- %s�����������Ķ�<hr class=\"default\">\n", BBSNAME, board);
    printf("<table class=\"default\" border=\"1\" width=\"610\">");
    printf("<tr><td><pre>");
    fp = fopen(pathbuf, "r");
    if (fp == NULL)
        http_fatal("������ļ���");
    while (fgets(buf, sizeof(buf), fp) != NULL)
        hhprintf("%s", void1(buf));
    fclose(fp);
    printf("</pre></table><hr>\n");
    printf("[<a href=\"javascript:history.go(-1)\">������һҳ</a>] ");
    if (board[0])
        printf("[<a href=\"/bbsdoc.php?board=%s\">��������</a>]", encode_url(buf, board, sizeof(buf)));
    printf("</center>\n");
}

int main()
{
    char path[512];

    initwww_all();
    strsncpy(path, getparm("path"), 511);
    ann_show_file(path);
    http_quit();
}
