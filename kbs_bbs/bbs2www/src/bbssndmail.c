/*
 * $Id$
 */
#include "bbslib.h"

int main()
{
    FILE *fp;
    char userid[80], filename[80], dir[80], title[ARTICLE_TITLE_LEN], buf[80], *content;
    int t, i, sig, backup;
    struct fileheader x;
    struct userec *u = NULL;

    initwww_all();
    if (!loginok)
        http_fatal("�Ҵҹ��Ͳ���д�ţ����ȵ�¼");
    if (!can_send_mail())
        http_fatal("�����ܷ����ż�");
    strsncpy(userid, getparm("userid"), 40);
    strncpy(title, getparm("title"), ARTICLE_TITLE_LEN - 1);
	title[ARTICLE_TITLE_LEN - 1] = '\0';
    backup = strlen(getparm("backup"));
    if (strchr(userid, '@') || strchr(userid, '|')
        || strchr(userid, '&') || strchr(userid, ';')) {
        http_fatal("������������ʺ�");
    }
    getuser(userid, &u);
    if (u == 0)
        http_fatal("������������ʺ�");
    strcpy(userid, u->userid);
    for (i = 0; i < strlen(title); i++)
        if (title[i] < 27 && title[i] >= -1)
            title[i] = ' ';
    sig = atoi(getparm("signature"));
    content = getparm("text");
    if (title[0] == 0)
        strcpy(title, "û����");
    sprintf(filename, "tmp/%s.%d.tmp", userid, getpid());
    if (f_append(filename, unix_string(content)) < 0)
        http_fatal("����ʧ��");
    
    if ((i=post_mail(userid, title, filename, getCurrentUser()->userid, getCurrentUser()->username, fromhost, sig, backup))!=0)
    {
        switch (i) {
        case -1:
        	http_fatal("����ʧ��:�޷������ļ�");
        case -2:
        	http_fatal("����ʧ��:�Է���������ʼ�");
        case -3:
        	http_fatal("����ʧ��:�Է�������");
        case -5:
        	http_fatal("����ʧ��:������ӵ�.DIR");
        case -6:
            printf("���ž���:���ܱ��浽������<br>\n");
            break;
        default:
        	http_fatal("����ʧ��");
        }
    }
    printf("�ż��Ѽĸ�%s.<br>\n", userid);
    unlink(filename);
    printf("<a href=\"javascript:history.go(-2)\">����</a>");
    http_quit();
}
