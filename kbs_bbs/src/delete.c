/*
    Pirate Bulletin Board System
    Copyright (C) 1990, Edward Luke, lush@Athena.EE.MsState.EDU
    Eagles Bulletin Board System
    Copyright (C) 1992, Raymond Rocker, rocker@rock.b11.ingr.com
                        Guy Vega, gtvega@seabass.st.usm.edu
                        Dominic Tynes, dbtynes@seabass.st.usm.edu

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 1, or (at your option)
    any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

    checked global variable
*/

#include "bbs.h"

static void mail_info()
{
    FILE *fn;
    time_t now;
    char filename[STRLEN];

    now = time(0);
    sprintf(filename, "etc/%s.tmp", getCurrentUser()->userid);
    fn = fopen(filename, "w");
    fprintf(fn, "\033[1m%s\033[m �Ѿ��� \033[1m%24.24s\033[m ��ɱ�ˣ���������(��)�����ϣ��뱣��...", getCurrentUser()->userid, ctime(&now));
    getuinfo(fn, getCurrentUser());
    fprintf(fn, "\n                      \033[1m ϵͳ�Զ�����ϵͳ��\033[m\n");
    fclose(fn);
    mail_file(getCurrentUser()->userid, filename, "acmanager", "��ɱ֪ͨ....", BBSPOST_MOVE, NULL);
}


int d_board()
{
    char bname[STRLEN];
    char title[STRLEN];

    modify_user_mode(ADMIN);
    if (!check_systempasswd()) {
        return -1;
    }
    clear();
    stand_title("ɾ��������");
    make_blist(0);
    move(1, 0);
    namecomplete("������������: ", genbuf);
    if (genbuf[0] == '\0')
        return 0;
    strcpy(bname, genbuf);
    if (delete_board(bname, title,getSession()) != 0)
        return 0;
    if (seek_in_file("0Announce/.Search", bname)) {
#ifdef BBSMAIN
        getdata(3, 0, "�Ƴ������� (Yes, or No) [Y]: ", genbuf, 4, DOECHO, NULL, true);
        if (genbuf[0] != 'N' && genbuf[0] != 'n') {
#endif

            del_grp(bname, title + 13);
        }
    }
    if (!bname[0]) {
        if (anonymousboard(bname))
            del_from_file("etc/anonymous", bname);
        sprintf(genbuf, "deleted board %s", bname);
        bbslog("user","%s",genbuf);
        /*
           sprintf(genbuf,"/bin/rm -fr boards/%s",bname) ;
           sprintf(genbuf,"/bin/rm -fr vote/%s",bname) ;
         */
        sprintf(genbuf, "boards/%s", bname);
        f_rm(bname);
        sprintf(genbuf, "vote/%s", bname);
        f_rm(bname);
    }

    move(4, 0);
    prints("���������Ѿ�ɾ��...\n");
    pressreturn();
    clear();
    return 0;
}

void suicide()
{
    char buf[STRLEN];
    FILE *fn;
    time_t now;
    char filename[STRLEN];

    char XPERM[48];
    int oldXPERM;
    int num;

    modify_user_mode(OFFLINE);
#ifndef NINE_BUILD
    if (HAS_PERM(getCurrentUser(), PERM_SYSOP) || HAS_PERM(getCurrentUser(), PERM_BOARDS) || HAS_PERM(getCurrentUser(), PERM_OBOARDS) || HAS_PERM(getCurrentUser(), PERM_ACCOUNTS)
        || HAS_PERM(getCurrentUser(), PERM_ANNOUNCE)
        || HAS_PERM(getCurrentUser(), PERM_JURY) || HAS_PERM(getCurrentUser(), PERM_SUICIDE) || HAS_PERM(getCurrentUser(), PERM_CHATOP) || (!HAS_PERM(getCurrentUser(), PERM_POST))
        || HAS_PERM(getCurrentUser(), PERM_DENYMAIL)
        || HAS_PERM(getCurrentUser(), PERM_DENYRELAX)) {
        clear();
        move(11, 28);
        prints("\033[1m\033[33m������������������ɱ��\033[m");
        pressanykey();
        return;
    }

    clear();
    move(1, 0);
    prints("ѡ����ɱ��ʹ�������������ٵ�14�죬14��������ʺ��Զ���ʧ��");
    move(3, 0);
    prints("����14�������ı�����Ļ��������ͨ����¼��վһ�λָ�ԭ������");
    move(5, 0);
    prints("��ɱ�û�����ʧ����\033[33m����Ȩ��\033[m������");
    move(7, 0);
    /*
       clear();
       move(1,0);
       prints("ѡ����ɱ��ʹ�����ʺ��Զ�������ʧ!");
       move(3,0);
       prints("�����ʺŽ����ϴ�ϵͳ��ɾ��");
     */

    if (askyn("��ȷ��Ҫ��ɱ��", 0) == 1) {
        clear();
        getdata(0, 0, "������ԭ����(������ȷ�Ļ������̶���): ", buf, 39, NOECHO, NULL, true);   /*Haohmaru,98.10.12,check the passwds */
        if (*buf == '\0' || !checkpasswd2(buf, getCurrentUser())) {
            prints("\n\n�ܱ�Ǹ, ����������벻��ȷ��\n");
            pressanykey();
            return;
        }

        oldXPERM = getCurrentUser()->userlevel;
        strcpy(XPERM, XPERMSTR);
        for (num = 0; num < (int) strlen(XPERM); num++)
            if (!(oldXPERM & (1 << num)))
                XPERM[num] = ' ';
        XPERM[num] = '\0';
        getCurrentUser()->userlevel &= 0x3F; /*Haohmaru,99.3.20.��ɱ��ֻ��������Ȩ�� */
        getCurrentUser()->userlevel ^= PERM_SUICIDE;

        /*Haohmaru.99.3.20.��ɱ֪ͨ */
        now = time(0);
        sprintf(filename, "etc/%s.tmp", getCurrentUser()->userid);
        fn = fopen(filename, "w");
        fprintf(fn, "\033[1m%s\033[m �Ѿ��� \033[1m%24.24s\033[m ��ɱ�ˣ��������������ϣ��뱣��...", getCurrentUser()->userid, ctime(&now));
        fprintf(fn, "\n\n��������ɱ��ԭ����Ȩ��\n\033[1m\033[33m%s\n\033[m", XPERM);
        getuinfo(fn, getCurrentUser());
        fprintf(fn, "\n                      \033[1m ϵͳ�Զ�����ϵͳ��\033[m\n");
        fclose(fn);
        sprintf(buf, "%s ����ɱ֪ͨ", getCurrentUser()->userid);
        post_file(getCurrentUser(), "", filename, "Goodbye", buf, 0, 1,getSession());
        unlink(filename);

        /*kick_user(&uinfo);
           exit(0); */
        abort_bbs(0);
    }
#else
    if (HAS_PERM(getCurrentUser(), PERM_SYSOP) || HAS_PERM(getCurrentUser(), PERM_BOARDS)) {
        clear();
        move(11, 28);
        prints("\033[1m\033[33m������������������ɱ��\033[m");
        pressanykey();
        return;
    }

    clear();
    move(1, 0);
    prints("һ����ɱ�����޷����");
    move(3, 0);
    prints("�����Ҫһ����֮��? ");
    move(5, 0);

    if (askyn("��ȷ��Ҫ��ɱ��", 0) == 1) {
        char buf2[STRLEN], tmpbuf[PATHLEN], genbuf[PATHLEN];
	 int id;
        clear();
	 getdata(0, 0, "������һ���̵���ɱ����: ", buf2, 75, DOECHO, NULL, true);
        getdata(0, 0, "������ԭ����(������ȷ�Ļ������̶��߲����޷����): ", buf, 39, NOECHO, NULL, true);   /*Haohmaru,98.10.12,check the passwds */
        if (*buf == '\0' || !checkpasswd2(buf, getCurrentUser())) {
            prints("\n\n�ܱ�Ǹ, ����������벻��ȷ��\n");
            pressanykey();
            return;
        }

        now = time(0);
        sprintf(filename, "etc/%s.tmp", getCurrentUser()->userid);
        fn = fopen(filename, "w");
        fprintf(fn, "��Һ�,\n\n���� %s (%s)�� ���Ѿ��뿪�����ˡ�", getCurrentUser()->userid, getCurrentUser()->username);
        fprintf(fn, "\n\n�Ҳ���������������� %s", ctime(&(getCurrentUser()->firstlogin)));
        fprintf(fn, "�������ڱ�վ %d �� login ���ܹ� %d ���Ӷ����ڼ�ĵ��εΡ�", getCurrentUser()->numlogins, getCurrentUser()->stay/60);
        fprintf(fn, "\n���ҵĺ��Ѱ� %s �����ǵĺ����������õ��ɡ�", getCurrentUser()->userid);
        fprintf(fn, "\n\n�����г�һ���һ�����ġ� ����!! �ټ�!!");
        fprintf(fn, "\n\n��ɱ�ߵļ������: %s", buf2);
        fclose(fn);
        sprintf(buf, "%s ����ɱ֪ͨ", getCurrentUser()->userid);
        post_file(getCurrentUser(), "", filename, "Goodbye", buf, 0, 2);
        unlink(filename);

    setmailpath(tmpbuf, getCurrentUser()->userid);
    f_rm(tmpbuf);
    sethomepath(tmpbuf, getCurrentUser()->userid);
    f_rm(tmpbuf);
    sprintf(genbuf, "tmp/email/%s", getCurrentUser()->userid);
    f_rm(genbuf);
    getCurrentUser()->userlevel = 0;
    id = searchuser(getCurrentUser()->userid);
    setuserid(id, "");
    /*strcpy(lookupuser->address, "");*/
    strcpy(getCurrentUser()->username, "");
    /*strcpy(lookupuser->realname, "");*/
	/*read_userdata(lookupuser->userid, &ud);
	strcpy(ud.address, "");
	strcpy(ud.realname, "");
	write_userdata(lookupuser->userid, &ud);*/
/*    lookupuser->userid[0] = '\0' ; */
        abort_bbs(0);
    }

#endif
}

int giveupnet()
{                               /* bad 2002.7.5 */
/*
PERM_BASIC   ��վ
PERM_POST    ����
PERM_CHAT    ����
PERM_PAGE    ����
PERM_DENYMAIL����
PERM_DENYRELAX����
*/
    char buf[STRLEN], genbuf[PATHLEN];
    FILE *fn;
    char ans[3], day[10];
    int i, j, k, lcount, tcount;

    modify_user_mode(GIVEUPNET);
    if (!HAS_PERM(getCurrentUser(), PERM_LOGINOK)) {
        clear();
        move(11, 28);
        prints("\033[1m\033[33m���л�û��ע��ͨ�������ܽ�����\033[m");
        pressanykey();
        return -1;
    }

    if (HAS_PERM(getCurrentUser(), PERM_SYSOP) || HAS_PERM(getCurrentUser(), PERM_BOARDS) || HAS_PERM(getCurrentUser(), PERM_OBOARDS) || HAS_PERM(getCurrentUser(), PERM_ACCOUNTS)
        || HAS_PERM(getCurrentUser(), PERM_ANNOUNCE)
        || HAS_PERM(getCurrentUser(), PERM_JURY) || HAS_PERM(getCurrentUser(), PERM_SUICIDE) || HAS_PERM(getCurrentUser(), PERM_CHATOP)) {
        clear();
        move(11, 28);
        prints("\033[1m\033[33m���������������ܽ�����\033[m");
        pressanykey();
        return -1;
    }

    lcount = 0;
    tcount = 0;
    sethomefile(genbuf, getCurrentUser()->userid, "giveup");
    fn = fopen(genbuf, "rt");
    if (fn) {
        clear();
        move(1, 0);
        prints("�����ڵĽ��������\n\n");
        while (!feof(fn)) {
            if (fscanf(fn, "%d %d", &i, &j) <= 0)
                break;
            switch (i) {
            case 1:
                prints("��վȨ��");
                break;
            case 2:
                prints("����Ȩ��");
                break;
            case 3:
                prints("����Ȩ��");
                break;
            case 4:
                prints("����Ȩ��");
                break;
            case 5:
                prints("����Ȩ��");
                break;
            case 6:
                prints("��������Ȩ��");
                break;
            }
            sprintf(buf, "        ����%ld��\n", j - time(0) / 3600 / 24);
            prints(buf);
            lcount++;
        }
        fclose(fn);
        pressanykey();
    }


    clear();
    move(1, 0);
    prints("��ѡ���������:");
    move(3, 0);
    prints("(0) - ����");
    move(4, 0);
    prints("(1) - ��վȨ��");
    move(5, 0);
    prints("(2) - ����Ȩ��");
    move(6, 0);
    prints("(3) - ������Ȩ��");
    move(7, 0);
    prints("(4) - ������Ϣ�ͺ�������Ȩ��");
    move(8, 0);
    prints("(5) - ����Ȩ��");
    move(9, 0);
    prints("(6) - ��������Ȩ��");

    getdata(12, 0, "��ѡ�� [0]", ans, 2, DOECHO, NULL, true);
    if (ans[0] < '1' || ans[0] > '6') {
        return -1;
    }
    k = 1;
    switch (ans[0]) {
    case '1':
        k = k && (getCurrentUser()->userlevel & PERM_BASIC);
        break;
    case '2':
        k = k && (getCurrentUser()->userlevel & PERM_POST);
        break;
    case '3':
        k = k && (getCurrentUser()->userlevel & PERM_CHAT);
        break;
    case '4':
        k = k && (getCurrentUser()->userlevel & PERM_PAGE);
        break;
    case '5':
        k = k && !(getCurrentUser()->userlevel & PERM_DENYMAIL);
        break;
    case '6':
        k = k && !(getCurrentUser()->userlevel & PERM_DENYRELAX);
        break;
    }

    if (!k) {
        prints("\n\n���Ѿ�û���˸�Ȩ��");
        pressanykey();
        return -1;
    }

    getdata(11, 0, "������������� [0]", day, 4, DOECHO, NULL, true);
    i = 0;
    while (day[i]) {
        if (!isdigit(day[i]))
            return -1;
        i++;
    }
    j = atoi(day);
    if (j <= 0)
        return -1;

    if (compute_user_value(getCurrentUser()) <= j) {
        prints("\n\n�Բ������������Դ���������...");
        pressanykey();
        return -1;
    }
    j = time(0) / 3600 / 24 + j;

    move(13, 0);

    if (askyn("��ȷ��Ҫ������\x1b[1;31m(ע��:����֮������ǰ�⿪!\x1b[m)��", 0) == 1) {
        getdata(15, 0, "����������: ", buf, 39, NOECHO, NULL, true);
        if (*buf == '\0' || !checkpasswd2(buf, getCurrentUser())) {
            prints("\n\n�ܱ�Ǹ, ����������벻��ȷ��\n");
            pressanykey();
            return -1;
        }

        sethomefile(genbuf, getCurrentUser()->userid, "giveup");
        fn = fopen(genbuf, "at");
        if (!fn) {
            prints("\n\n����ϵͳ���⣬�����㲻�ܽ���");
            pressanykey();
            return -1;
        }
        fprintf(fn, "%d %d\n", ans[0] - 48, j);
        fclose(fn);

        switch (ans[0]) {
        case '1':
            getCurrentUser()->userlevel &= ~PERM_BASIC;
            break;
        case '2':
            getCurrentUser()->userlevel &= ~PERM_POST;
            break;
        case '3':
            getCurrentUser()->userlevel &= ~PERM_CHAT;
            break;
        case '4':
            getCurrentUser()->userlevel &= ~PERM_PAGE;
            break;
        case '5':
            getCurrentUser()->userlevel |= PERM_DENYMAIL;
            break;
        case '6':
            getCurrentUser()->userlevel |= PERM_DENYRELAX;
            break;
        }
        lcount++;

        if (getCurrentUser()->userlevel & PERM_BASIC)
            tcount++;
        if (getCurrentUser()->userlevel & PERM_POST)
            tcount++;
        if (getCurrentUser()->userlevel & PERM_CHAT)
            tcount++;
        if (getCurrentUser()->userlevel & PERM_PAGE)
            tcount++;
        if (!(getCurrentUser()->userlevel & PERM_DENYMAIL))
            tcount++;
        if (!(getCurrentUser()->userlevel & PERM_DENYRELAX))
            tcount++;

        if (lcount + tcount == 6)
            getCurrentUser()->flags |= GIVEUP_FLAG;
        else
            getCurrentUser()->flags &= ~GIVEUP_FLAG;

        prints("\n\n���Ѿ���ʼ������");
        pressanykey();
        if (ans[0] == '1')
            abort_bbs(0);
    }
    return 0;
}


void offline()
{
    modify_user_mode(OFFLINE);

    if (HAS_PERM(getCurrentUser(), PERM_SYSOP))
        return;
    clear();
    move(1, 0);
    prints("\033[32m���ѹ��.....\033[m");
    move(3, 0);
    if (askyn("��ȷ��Ҫ�뿪������ͥ", 0) == 1) {
        clear();
        if (d_user(getCurrentUser()->userid) == 1) {
            mail_info();
            kick_user(&uinfo);
            exit(0);
        }
    }
}

int kickuser(struct user_info *uentp, char *arg, int count)
{
    kill(uentp->pid, SIGHUP);
    clear_utmp((uentp - utmpshm->uinfo) + 1, uentp->uid, uentp->pid);
    UNUSED_ARG(arg);
    UNUSED_ARG(count);
    return 0;
}

int d_user(cid)
    char cid[IDLEN];
{
    int id, fd;
    char tmpbuf[30];
    char userid[IDLEN + 2];
    struct userec *lookupuser;
       /* ������ʾ�û���Ϣ Bigman:2003.5.11*/
        struct userec uinfo1;
	/*struct userdata ud;*/

    if (uinfo.mode != OFFLINE) {
        modify_user_mode(ADMIN);
        if (!check_systempasswd()) {
            return 0;
        }
        clear();
        stand_title("ɾ��ʹ�����ʺ�");
        move(1, 0);
        usercomplete("��������ɾ����ʹ���ߴ���: ", userid);
        if (userid[0] == '\0') {
            clear();
            return 0;
        }
    } else
        strcpy(userid, cid);
    if (!(id = getuser(userid, &lookupuser))) {
        move(3, 0);
        prints(MSG_ERR_USERID);
        clrtoeol();
        pressreturn();
        clear();
        return 0;
    }
     /* ������ʾ�û���Ϣ Bigman:2003.5.11*/
    uinfo1 = *lookupuser;
    clrtobot();

    disply_userinfo(&uinfo1, 1);

    /*    if (!isalpha(lookupuser->userid[0])) return 0; */
    /* rrr - don't know how... */
    move(22, 0);
    if (uinfo.mode != OFFLINE)
        prints("ɾ��ʹ���� '%s'.", userid);
    else
        prints(" %s ���뿪����", cid);
/*    clrtoeol(); */
    
    getdata(24, 0, "(Yes, or No) [No](ע��Ҫ����ȫ���ʺʹ�Сд): ", genbuf, 5, DOECHO, NULL, true);
    if (strcmp(genbuf,"Yes")) { /* if not yes quit */
        move(24, 0);
        if (uinfo.mode != OFFLINE)
            prints("ȡ��ɾ��ʹ����...\n");
        else
            prints("�����ڻ���ת���ˣ��ø����...");
        pressreturn();
        clear();
        return 0;
    }
    if (uinfo.mode != OFFLINE) {
        char secu[STRLEN];

        sprintf(secu, "ɾ��ʹ���ߣ�%s", lookupuser->userid);
        securityreport(secu, lookupuser, NULL);
    }
    sprintf(genbuf, "%s deleted user %s", getCurrentUser()->userid, lookupuser->userid);
    bbslog("user","%s",genbuf);
    /*Haohmaru.99.12.23.��ɾIDһ�����ڲ���ע�� */
    if ((fd = open(".badname", O_WRONLY | O_CREAT, 0644)) != -1) {
        char buf[STRLEN];
        char thtime[40];
        time_t dtime;

        dtime = time(0);
        sprintf(thtime, "%lu", dtime);
        flock(fd, LOCK_EX);
        lseek(fd, 0, SEEK_END);
        sprintf(buf, "%-12.12s %-66.66s\n", lookupuser->userid, thtime);
        write(fd, buf, strlen(buf));
        flock(fd, LOCK_UN);
        close(fd);
    } else {
        printf("�����뱨��SYSOP");
        pressanykey();
    }
    setmailpath(tmpbuf, lookupuser->userid);
    /*
       sprintf(genbuf,"/bin/rm -fr %s", tmpbuf) ;
     */
    f_rm(tmpbuf);
    sethomepath(tmpbuf, lookupuser->userid);
    /*
       sprintf(genbuf,"/bin/rm -fr %s", tmpbuf) ;
       sprintf(genbuf,"/bin/rm -fr tmp/email/%s", lookupuser->userid) ;
     */
    f_rm(tmpbuf);
    sprintf(genbuf, "tmp/email/%s", lookupuser->userid);
    f_rm(genbuf);
    apply_utmp((APPLY_UTMP_FUNC) kickuser, 0, userid, 0);
    setuserid(id, "");
    lookupuser->userlevel = 0;
    /*strcpy(lookupuser->address, "");*/
    strcpy(lookupuser->username, "");
    /*strcpy(lookupuser->realname, "");*/
	/*read_userdata(lookupuser->userid, &ud);
	strcpy(ud.address, "");
	strcpy(ud.realname, "");
	write_userdata(lookupuser->userid, &ud);*/
/*    lookupuser->userid[0] = '\0' ; */
    move(24, 0);
    prints("%s �Ѿ��Ѿ��ͱ���ͥʧȥ����....\n", userid);
    pressreturn();

    clear();
    return 1;
}

/* to be Continue fix kick user problem */
int kick_user(struct user_info *userinfo)
{
    int id, ind;
    struct user_info uin;
    char userid[40];

    if (uinfo.mode != LUSERS && uinfo.mode != OFFLINE && uinfo.mode != FRIEND) {
        modify_user_mode(ADMIN);
        stand_title("Kick User");
        move(1, 0);
        usercomplete("Enter userid to be kicked: ", userid);
        if (*userid == '\0') {
            clear();
            return 0;
        }
        if (!(id = searchuser(userid))) {       /* change getuser -> searchuser, by dong, 1999.10.26 */
            move(3, 0);
            prints("Invalid User Id");
            clrtoeol();
            pressreturn();
            clear();
            return 0;
        }
        move(1, 0);
        prints("Kick User '%s'.", userid);
        clrtoeol();
        getdata(2, 0, "(Yes, or No) [N]: ", genbuf, 4, DOECHO, NULL, true);
        if (genbuf[0] != 'Y' && genbuf[0] != 'y') {     /* if not yes quit */
            move(2, 0);
            prints("Aborting Kick User\n");
            pressreturn();
            clear();
            return 0;
        }
        return apply_utmp((APPLY_UTMP_FUNC) kickuser, 0, userid, 0);
    } else {
        uin = *userinfo;
        strcpy(userid, uin.userid);
        ind = true;
    }
    if (uin.mode == WEBEXPLORE)
        clear_utmp((userinfo - utmpshm->uinfo) + 1, uin.uid, uin.pid);
    if (!ind || !uin.active || (kill(uin.pid, 0) == -1)) {
        if (uinfo.mode != LUSERS && uinfo.mode != OFFLINE && uinfo.mode != FRIEND) {
            move(3, 0);
            prints("User Has Logged Out");
            clrtoeol();
            pressreturn();
            clear();
        }
        return 0;
    }
    if (kill(uin.pid, SIGHUP) == -1) {
        clear_utmp((userinfo - utmpshm->uinfo) + 1, uin.uid, uin.pid);
    }
    newbbslog(BBSLOG_USER, "kicked %s", userid);
    /*sprintf( genbuf, "%s (%s)", kuinfo.userid, kuinfo.username );modified by dong, 1998.11.2 */
    /*bbslog( "1system", "KICK %s (%s)", uin.userid, uin.username ); */
    /*    uin.active = false;
       uin.pid = 0;
       uin.invisible = true;
       uin.sockactive = 0;
       uin.sockaddr = 0;
       uin.destuid = 0;
       update_ulist( &uin, ind ); ��������Ҳ����д�����ע�͵� dong 1998.7.7 */
    move(2, 0);
    if (uinfo.mode != LUSERS && uinfo.mode != OFFLINE && uinfo.mode != FRIEND) {
        prints("User has been Kicked\n");
        pressreturn();
        clear();
    }
    return 1;
}
