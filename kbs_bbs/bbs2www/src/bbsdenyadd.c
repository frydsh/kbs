/*
 * $Id$
 */
#include "bbslib.h"

struct deny {
    char id[80];
    char exp[80];
    char comment[80];
    time_t free_time;
} denyuser[1024];

int denynum = 0;

int loaddenyuser(char *board)
{
    FILE *fp;
    char path[80], buf[256];
    char *id, *nick;

    sprintf(path, "boards/%s/deny_users", board);
    fp = fopen(path, "r");
    if (fp == 0)
        return;
    while (denynum < (sizeof(denyuser) / sizeof(denyuser[0]))) {
        if (fgets(buf, sizeof(buf), fp) == 0)
            break;
        id = strchr(buf, ' ');
        if (id != NULL)
            *id = '\0';
        strcpy(denyuser[denynum].id, buf);
        strncpy(denyuser[denynum].exp, buf + 13, 30);
        nick = strrchr(buf + 13, '[');
        if (nick != NULL) {
            denyuser[denynum].free_time = atol(nick + 1);
            nick--;
            if (nick - buf > 43) {
                *nick = '\0';
                strcpy(denyuser[denynum].comment, buf + 43);
            }
        }
        denynum++;
    }
    fclose(fp);
}

int savedenyuser(char *board)
{
    FILE *fp;
    int i;
    char path[80], buf[256], *exp;

    sprintf(path, "boards/%s/deny_users", board);
    fp = fopen(path, "w");
    if (fp == 0)
        return;
    for (i = 0; i < denynum; i++) {
        int m;

        exp = denyuser[i].exp;
        if (denyuser[i].id[0] == 0)
            continue;
        for (m = 0; exp[m]; m++) {
            if (exp[m] < 32 && exp[m] > 0)
                exp[m] = '.';
        }
        fprintf(fp, "%-12.12s %-30.30s%s\x1b[%um\n", denyuser[i].id, denyuser[i].exp, denyuser[i].comment, denyuser[i].free_time);
    }
    fclose(fp);
}

int show_form(char *board)
{
    printf("<center>%s -- ������� [������: %s]<hr color=\"green\">\n", BBSNAME, board);
    printf("<form action=\"bbsdenyadd\"><input type=\"hidden\" name=\"board\" value=\"%s\">", board);
    printf("���ʹ����<input name=\"userid\" size=\"12\"> ����POSTȨ <input name=\"dt\" size=\"2\"> ��, ԭ��<input name=\"exp\" size=\"20\">\n");
    printf("<input type=\"submit\" value=\"ȷ��\"></form>");
}

int inform(boardheader_t * bp, char *user, char *exp, int dt)
{
    FILE *fn;
    char buf[80], title[80];
    struct userec *usr;
    struct userec saveusr;
    int my_flag;
    char *board;

    /*postinfo_t pi; */
    time_t now;
    struct userec *lookupuser;

    /*bzero(&pi, sizeof(pi)); */
    board = bp->filename;
    usr = getcurrusr();
    now = time(0);
    memcpy(&saveusr, usr, sizeof(saveusr));
    sprintf(buf, "%s ȡ�� %s �� %s �� POST Ȩ��", usr->userid, user, board);
    report(buf);

    /*Haohmaru.4.1.�Զ�����֪ͨ���������ڰ��� */
    sprintf(buf, "etc/%s.deny", usr->userid);
    fn = fopen(buf, "w+");
    snprintf(title, ARTICLE_TITLE_LEN ,"%s��ȡ����%s��ķ���Ȩ��", user, board);

    if ((HAS_PERM(currentuser, PERM_SYSOP) || HAS_PERM(currentuser, PERM_OBOARDS)) && !chk_BM_instr(bp->BM, currentuser->userid)) {
        my_flag = 0;
        fprintf(fn, "������: SYSOP (System Operator) \n");
        fprintf(fn, "��  ��: %s\n", title);
        fprintf(fn, "����վ: %s (%24.24s)\n", "BBS " NAME_BBS_CHINESE "վ", ctime(&now));
        fprintf(fn, "��  Դ: " NAME_BBS_ENGLISH "\n");
        fprintf(fn, "\n");
        fprintf(fn, "�������� \x1b[4m%s\x1b[m �� \x1b[4m%s\x1b[m���Һ��ź���֪ͨ���� \n", board, exp);
        if (dt)
            fprintf(fn, "������ʱȡ���ڸð�ķ���Ȩ�� \x1b[4m%d\x1b[m �죬���ں���ظ�\n", dt);
        else
            fprintf(fn, "������ʱȡ���ڸð�ķ���Ȩ�������ں���ظ�\n");
        fprintf(fn, "��������ָ�Ȩ�ޡ�\n");
        fprintf(fn, "\n");
        fprintf(fn, "                            " NAME_BBS_CHINESE NAME_SYSOP_GROUP "ֵ��վ��\x1b[4m%s\x1b[m\n", usr->userid);
        fprintf(fn, "                              %s\n", ctime(&now));
        strcpy(usr->userid, "SYSOP");
        strcpy(usr->username, NAME_SYSOP);
        /*strcpy(usr->realname, NAME_SYSOP);*/
    } else {
        my_flag = 1;
        fprintf(fn, "������: %s \n", usr->userid);
        fprintf(fn, "��  ��: %s\n", title);
        fprintf(fn, "����վ: %s (%24.24s)\n", "BBS " NAME_BBS_CHINESE "վ", ctime(&now));
        fprintf(fn, "��  Դ: %s \n", usr->lasthost);
        fprintf(fn, "\n");
        fprintf(fn, "�������� \x1b[4m%s\x1b[m �� \x1b[4m%s\x1b[m���Һ��ź���֪ͨ���� \n", board, exp);
        if (dt)
            fprintf(fn, "������ʱȡ���ڸð�ķ���Ȩ�� \x1b[4m%d\x1b[m �죬���ں���ظ�\n", dt);
        else
            fprintf(fn, "������ʱȡ���ڸð�ķ���Ȩ�������ں���ظ�\n");
        fprintf(fn, "��������ָ�Ȩ�ޡ�\n");
        fprintf(fn, "\n");
        fprintf(fn, "                              " NAME_BM ":\x1b[4m%s\x1b[m\n", usr->userid);
        fprintf(fn, "                              %s\n", ctime(&now));
    }
    fclose(fn);

    mail_file(getcurruserid(), buf, user, title, BBSPOST_MOVE, NULL);

    fn = fopen(buf, "w+");
    fprintf(fn, "���� \x1b[4m%s\x1b[m �� \x1b[4m%s\x1b[m ��� \x1b[4m%s\x1b[m ��Ϊ��\n", user, board, exp);
    if (dt)
        fprintf(fn, "����ʱȡ���ڱ���ķ���Ȩ�� \x1b[4m%d\x1b[m �졣\n", dt);
    else
        fprintf(fn, "������ʱȡ���ڸð�ķ���Ȩ�������ں���ظ�\n");

    if (my_flag == 0)
        fprintf(fn, "                            " NAME_BBS_CHINESE NAME_SYSOP_GROUP "ֵ��վ��\x1b[4m%s\x1b[m\n", saveusr.userid);
    else
        fprintf(fn, "                              " NAME_BM ":\x1b[4m%s\x1b[m\n", usr->userid);
    fprintf(fn, "                              %s\n", ctime(&now));
    fclose(fn);
    /*
       pi.userid = usr->userid;
       pi.username = usr->username;
       pi.title = title;
       pi.board = board;
       pi.local = 1;
       pi.anony = 0;
       pi.access = (FILE_READ << 8) | (FILE_MARKED | FILE_FORWARDED);
     */
    post_file(usr, "", buf, board, title, 0, 2);
    memcpy(usr, &saveusr, sizeof(saveusr));
    snprintf(title, ARTICLE_TITLE_LEN ,"%s �� %s �������POSTȨ", user, usr->userid);
    getuser(user, &lookupuser);

    if (HAS_PERM(lookupuser, PERM_BOARDS))
        snprintf(title, ARTICLE_TITLE_LEN , "%s ��ĳ��" NAME_BM " %s �� %s", usr->userid, user, board);
    else
        snprintf(title, ARTICLE_TITLE_LEN ,"%s �� %s �� %s", usr->userid, user, board);
    post_file(usr, "", buf, "denypost", title, 0, 8);

    unlink(buf);
    printf("ϵͳ�Ѿ�����֪ͨ��%s.<br>\n", user);
}

int main()
{
    int i;
    char exp[80], board[80], *userid;
    int dt;
    struct userec *u = NULL;
    boardheader_t *bp;
    struct tm *tmtime;
    time_t undenytime;
    char buf[STRLEN];
    struct boardheader bh;

    init_all();
    if (!loginok)
        http_fatal("����δ��¼, ���ȵ�¼");
    strsncpy(board, getparm("board"), 30);
    strsncpy(exp, getparm("exp"), 30);
    dt = atoi(getparm("dt"));
    if (getboardnum(board,&bh)==0||!check_read_perm(currentuser, &bh))
        http_fatal("�����������");
    if (!has_BM_perm(currentuser, board))
        http_fatal("����Ȩ���б�����");
    loaddenyuser(board);
    userid = getparm("userid");
    if (userid[0] == 0)
        return show_form(board);
    if (getuser(userid, &u) == 0)
        http_fatal("�����ʹ�����ʺ�");
    strcpy(userid, u->userid);
    if (dt < 1 || dt > 70)
        http_fatal("�����뱻������(1-70)");
    if (exp[0] == 0)
        http_fatal("������˵��");
    for (i = 0; i < denynum; i++) {
        if (!strcasecmp(denyuser[i].id, userid))
            http_fatal("���û��Ѿ�����");
    }
    if (denynum > 512)
        http_fatal("̫���˱�����");
    strsncpy(denyuser[denynum].id, userid, 13);
    strsncpy(denyuser[denynum].exp, exp, 30);
    undenytime = time(0) + dt * 86400;
    tmtime = gmtime(&undenytime);
    sprintf(buf, "%-12.12s %2d��%2d�ս�", getcurruserid(), tmtime->tm_mon + 1, tmtime->tm_mday);
    strsncpy(denyuser[denynum].comment, buf, STRLEN);
    denyuser[denynum].free_time = undenytime;
    denynum++;
    savedenyuser(board);
    printf("��� %s �ɹ�<br>\n", userid);
    bp = getbcache(board);
    inform(bp, userid, exp, dt);
    printf("[<a href=\"bbsdenyall?board=%s\">���ر����ʺ�����</a>]", http_encode_string(board, sizeof(board)));
    http_quit();
}
