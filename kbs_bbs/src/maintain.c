/*
 * Pirate Bulletin Board System Copyright (C) 1990, Edward Luke,
 * lush@Athena.EE.MsState.EDU Eagles Bulletin Board System Copyright (C)
 * 1992, Raymond Rocker, rocker@rock.b11.ingr.com Guy Vega,
 * gtvega@seabass.st.usm.edu Dominic Tynes, dbtynes@seabass.st.usm.edu
 *
 * This program is free software; you can redistribute it and/or modify it under
 * the terms of the GNU General Public License as published by the Free
 * Software Foundation; either version 1, or (at your option) any later
 * version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along with
 * this program; if not, write to the Free Software Foundation, Inc., 675
 * Mass Ave, Cambridge, MA 02139, USA.
 */

#include "bbs.h"

char cexplain[STRLEN];
char *Ctime();
static int sysoppassed = 0;

/* modified by wwj, 2001/5/7, for new md5 passwd */
void igenpass(const char *passwd, const char *userid, unsigned char md5passwd[]);

int check_systempasswd()
{
    FILE *pass;
    char passbuf[40], prepass[STRLEN];

#ifdef NINE_BUILD
    return true;
#endif

    if ((sysoppassed) && (time(NULL) - sysoppassed < 60 * 60))
        return true;
    clear();
    if ((pass = fopen("etc/systempassword", "rb")) != NULL) {
        fgets(prepass, STRLEN, pass);
        prepass[strlen(prepass) - 1] = '\0';
        if (!strcmp(prepass, "md5")) {
            fread(&prepass[16], 1, 16, pass);
        }
        fclose(pass);

        getdata(1, 0, "������ϵͳ����: ", passbuf, 39, NOECHO, NULL, true);
        if (passbuf[0] == '\0' || passbuf[0] == '\n')
            return false;


        if (!strcmp(prepass, "md5")) {
            igenpass(passbuf, "[system]", (unsigned char *) prepass);
            passbuf[0] = (char) !memcmp(prepass, &prepass[16], 16);
        } else {
            passbuf[0] = (char) checkpasswd(prepass, passbuf);
        }
        if (!passbuf[0]) {
            move(2, 0);
            prints("ϵͳ�����������...");
            securityreport("ϵͳ�����������...", NULL, NULL);
            pressanykey();
            return false;
        }
    }
    sysoppassed = time(NULL);
    return true;
}

int setsystempasswd()
{
    FILE *pass;
    char passbuf[40], prepass[40];

    modify_user_mode(ADMIN);
    if (strcmp(currentuser->userid, "SYSOP"))
        return -1;
    if (!check_systempasswd())
        return -1;
    getdata(2, 0, "�������µ�ϵͳ����: ", passbuf, 39, NOECHO, NULL, true);
    getdata(3, 0, "ȷ���µ�ϵͳ����: ", prepass, 39, NOECHO, NULL, true);
    if (strcmp(passbuf, prepass))
        return -1;
    if ((pass = fopen("etc/systempassword", "w")) == NULL) {
        move(4, 0);
        prints("ϵͳ�����޷��趨....");
        pressanykey();
        return -1;
    }
    fwrite("md5\n", 4, 1, pass);

    igenpass(passbuf, "[system]", (unsigned char *) prepass);
    fwrite(prepass, 16, 1, pass);

    fclose(pass);
    move(4, 0);
    prints("ϵͳ�����趨���....");
    pressanykey();
    return 0;
}


void deliverreport(title, str)
char *title;
char *str;
{
    FILE *se;
    char fname[STRLEN];
    int savemode;

    savemode = uinfo.mode;
	gettmpfilename( fname, "deliver" );
    //sprintf(fname, "tmp/deliver.%s.%05d", currentuser->userid, uinfo.pid);
    if ((se = fopen(fname, "w")) != NULL) {
        fprintf(se, "%s\n", str);
        fclose(se);
        post_file(currentuser, "", fname, currboard->filename, title, 0, 2);
        unlink(fname);
        modify_user_mode(savemode);
    }
}


void securityreport(char *str, struct userec *lookupuser, char fdata[7][STRLEN])
{                               /* Leeward: 1997.12.02 */
    FILE *se;
    char fname[STRLEN];
    int savemode;
    char *ptr;

    savemode = uinfo.mode;
	gettmpfilename( fname, "security" );
    //sprintf(fname, "tmp/security.%d", getpid());
    if ((se = fopen(fname, "w")) != NULL) {
        if (lookupuser) {
            if (strstr(str, "��") && strstr(str, "ͨ�����ȷ��")) {
                struct userdata ud;

                read_userdata(lookupuser->userid, &ud);
                fprintf(se, "ϵͳ��ȫ��¼ϵͳ\n[32mԭ��%s[m\n", str);
                fprintf(se, "������ͨ���߸�������");
                /*
                 * getuinfo(se, lookupuser); 
                 */
                /*
                 * Haohmaru.99.4.15.�ѱ�ע��������еø���ϸ,ͬʱȥ��ע���ߵ����� 
                 */
                fprintf(se, "\n\n���Ĵ���     : %s\n", fdata[1]);
                fprintf(se, "�����ǳ�     : %s\n", lookupuser->username);
                fprintf(se, "��ʵ����     : %s\n", fdata[2]);
                fprintf(se, "�����ʼ����� : %s\n", ud.email);
                fprintf(se, "��ʵ E-mail  : %s$%s@%s\n", fdata[3], fdata[5], currentuser->userid);
                fprintf(se, "����λ     : %s\n", fdata[3]);
                fprintf(se, "Ŀǰסַ     : %s\n", fdata[4]);
                fprintf(se, "����绰     : %s\n", fdata[5]);
                fprintf(se, "ע������     : %s", ctime(&lookupuser->firstlogin));
                fprintf(se, "����������� : %s", ctime(&lookupuser->lastlogin));
                fprintf(se, "������ٻ��� : %s\n", lookupuser->lasthost);
                fprintf(se, "��վ����     : %d ��\n", lookupuser->numlogins);
                fprintf(se, "������Ŀ     : %d(Board)\n", lookupuser->numposts);
                fprintf(se, "��    ��     : %s\n", fdata[6]);
                /*
                 * fprintf(se, "\n[33m��������֤�߸�������[35m");
                 * getuinfo(se, currentuser);rem by Haohmaru.99.4.16 
                 */
                fclose(se);
                post_file(currentuser, "", fname, "Registry", str, 0, 2);
            } else if (strstr(str, "ɾ��ʹ���ߣ�")) {
                fprintf(se, "ϵͳ��ȫ��¼ϵͳ\n[32mԭ��%s[m\n", str);
                fprintf(se, "�����Ǳ�ɾ�߸�������");
                getuinfo(se, lookupuser);
                fprintf(se, "\n������ɾ���߸�������");
                getuinfo(se, currentuser);
                fclose(se);
                post_file(currentuser, "", fname, "syssecurity", str, 0, 2);
            } else if ((ptr = strstr(str, "��Ȩ��XPERM")) != NULL) {
                int oldXPERM, newXPERM;
                int num;
                char XPERM[48];

                sscanf(ptr + strlen("��Ȩ��XPERM"), "%d %d", &oldXPERM, &newXPERM);
                *(ptr + strlen("��Ȩ��")) = 0;

                fprintf(se, "ϵͳ��ȫ��¼ϵͳ\n[32mԭ��%s[m\n", str);

                strcpy(XPERM, XPERMSTR);
                for (num = 0; num < (int) strlen(XPERM); num++)
                    if (!(oldXPERM & (1 << num)))
                        XPERM[num] = ' ';
                XPERM[num] = '\0';
                fprintf(se, "�����Ǳ�����ԭ����Ȩ��\n\033[1m\033[33m%s", XPERM);

                strcpy(XPERM, XPERMSTR);
                for (num = 0; num < (int) strlen(XPERM); num++)
                    if (!(newXPERM & (1 << num)))
                        XPERM[num] = ' ';
                XPERM[num] = '\0';
                fprintf(se, "\n%s\033[m\n�����Ǳ��������ڵ�Ȩ��\n", XPERM);

                fprintf(se, "\n"
                        "\033[1m\033[33mb\033[m����Ȩ�� \033[1m\033[33mT\033[m�������� \033[1m\033[33mC\033[m�������� \033[1m\033[33mP\033[m������ \033[1m\033[33mR\033[m������ȷ \033[1m\033[33mp\033[mʵϰվ�� \033[1m\033[33m#\033[m������ \033[1m\033[33m@\033[m�ɼ�����\n"
                        "\033[1m\033[33mX\033[m�����ʺ� \033[1m\033[33mW\033[m�༭ϵͳ���� \033[1m\033[33mB\033[m���� \033[1m\033[33mA\033[m�ʺŹ��� \033[1m\033[33m$\033[m������ \033[1m\033[33mV\033[m������� \033[1m\033[33mS\033[mϵͳά��\n"
                        "\033[1m\033[33m!\033[mRead/Post���� \033[1m\033[33mD\033[m�������ܹ� \033[1m\033[33mE\033[m�������ܹ� \033[1m\033[33mM\033[m������ܹ� \033[1m\033[33m1\033[m����ZAP \033[1m\033[33m2\033[m������OP\n"
                        "\033[1m\033[33m3\033[mϵͳ�ܹ���Ա \033[1m\033[33m4\033[m�����ʺ� \033[1m\033[33m5 7\033[m ����Ȩ�� \033[1m\033[33m6\033[m�ٲ� \033[1m\033[33m8\033[m��ɱ \033[1m\033[33m9\033[m�����ʺ� \033[1m\033[33m0\033[m��ϵͳ���۰�\n"
			"\033[1m\033[33m%%\033[m���Mail"
                        "\n");

                fprintf(se, "\n�����Ǳ����߸�������");
                getuinfo(se, lookupuser);
                fprintf(se, "\n�������޸��߸�������");
                getuinfo(se, currentuser);
                fclose(se);
                post_file(currentuser, "", fname, "syssecurity", str, 0, 2);
            } else {            /* Modified for change id by Bigman 2001.5.25 */

                fprintf(se, "ϵͳ��ȫ��¼ϵͳ\0x1b[32mԭ��%s\x1b[m\n", str);
                fprintf(se, "�����Ǹ�������");
                getuinfo(se, lookupuser);
                fclose(se);
                post_file(currentuser, "", fname, "syssecurity", str, 0, 2);
            }
        } else {
            fprintf(se, "ϵͳ��ȫ��¼ϵͳ\n[32mԭ��%s[m\n", str);
            fprintf(se, "�����Ǹ�������");
            getuinfo(se, currentuser);
            fclose(se);
            if (strstr(str, "�趨ʹ����ע������"))      /* Leeward 98.03.29 */
                post_file(currentuser, "", fname, "Registry", str, 0, 2);
            else
                post_file(currentuser, "", fname, "syssecurity", str, 0, 2);
        }
        unlink(fname);
        modify_user_mode(savemode);
    }
}

void stand_title(title)
char *title;
{
    clear();
    prints("\x1b[7m%s\x1b[m", title);
}

int m_info()
{
    struct userec uinfo;
    int id;
    struct userec *lookupuser;


    modify_user_mode(ADMIN);
    if (!check_systempasswd()) {        /* Haohmaru.98.12.19 */
        return -1;
    }
    clear();
    stand_title("�޸�ʹ���ߴ���");
    move(1, 0);
    usercomplete("������ʹ���ߴ���: ", genbuf);
    if (*genbuf == '\0') {
        clear();
        return -1;
    }
    if (!(id = getuser(genbuf, &lookupuser))) {
        move(3, 0);
        prints(MSG_ERR_USERID);
        clrtoeol();
        pressreturn();
        clear();
        return -1;
    }
    uinfo = *lookupuser;

    move(1, 0);
    clrtobot();
    disply_userinfo(&uinfo, 1);
    uinfo_query(&uinfo, 1, id);
    return 0;
}

extern int cmpbnames();

char *chgrp()
{
    int i, ch;
    char buf[STRLEN], ans[6];

    clear();
    move(2, 0);
    prints("ѡ�񾫻�����Ŀ¼\n");
    oflush();

    for (i = 0;; i++) {
        if (secname[i][0] == NULL || groups[i] == NULL)
            break;
        prints("[32m%2d[m. %-20s%-20s\n", i, secname[i][0], groups[i]);
    }
    sprintf(buf, "���������ѡ��(0~%d): ", i - 1);
    while (1) {
        getdata(i + 3, 0, buf, ans, 4, DOECHO, NULL, true);
        if (!isdigit(ans[0]))
            continue;
        ch = atoi(ans);
        if (ch < 0 || ch >= i || ans[0] == '\r' || ans[0] == '\0')
            continue;
        else
            break;
    }
    sprintf(cexplain, "%s", secname[ch][0]);

    return groups[ch];
}


int m_newbrd()
{
    struct boardheader newboard;
    char ans[5];
    char vbuf[100];
    char *group;


    modify_user_mode(ADMIN);
    if (!check_systempasswd()) {
        return -1;
    }
    clear();
    memset(&newboard, 0, sizeof(newboard));
    prints("������������:");
    while (1) {
        getdata(3, 0, "����������:   ", newboard.filename, BOARDNAMELEN, DOECHO, NULL, true);
        if (newboard.filename[0] == '\0')
            return -1;
        if (valid_brdname(newboard.filename))
            break;
        prints("���Ϸ�����...");
    }
    getdata(4, 0, "������˵��:   ", newboard.title, 60, DOECHO, NULL, true);
    strcpy(vbuf, "vote/");
    strcat(vbuf, newboard.filename);
    setbpath(genbuf, newboard.filename);
    if (getbnum(newboard.filename) > 0 || mkdir(genbuf, 0755) == -1 || mkdir(vbuf, 0755) == -1) {
        prints("\n���󣺴��������������\n");
        pressreturn();
        clear();
        return -1;
    }
    newboard.flag = 0;
    getdata(5, 0, "����������Ա: ", newboard.BM, BM_LEN - 1, DOECHO, NULL, true);
    getdata(6, 0, "�Ƿ����ƴ�ȡȨ�� (Y/N)? [N]: ", ans, 4, DOECHO, NULL, true);
    if (*ans == 'y' || *ans == 'Y') {
        getdata(6, 0, "���� Read/Post? [R]: ", ans, 4, DOECHO, NULL, true);
        if (*ans == 'P' || *ans == 'p')
            newboard.level = PERM_POSTMASK;
        else
            newboard.level = 0;
        move(1, 0);
        clrtobot();
        move(2, 0);
        prints("�趨 %s Ȩ��. ������: '%s'\n", (newboard.level & PERM_POSTMASK ? "POST" : "READ"), newboard.filename);
        newboard.level = setperms(newboard.level, 0, "Ȩ��", NUMPERMS, showperminfo, NULL);
        clear();
    } else
        newboard.level = 0;
    getdata(7, 0, "�Ƿ���������� (Y/N)? [N]: ", ans, 4, DOECHO, NULL, true);
    if (ans[0] == 'Y' || ans[0] == 'y') {
        newboard.flag |= BOARD_ANNONY;
        addtofile("etc/anonymous", newboard.filename);
    }
    getdata(8, 0, "�Ƿ񲻼�������(Y/N)? [N]: ", ans, 4, DOECHO, NULL, true);
    if (ans[0] == 'Y' || ans[0] == 'y')
        newboard.flag |= BOARD_JUNK;
    getdata(9, 0, "�Ƿ������ת��(Y/N)? [N]: ", ans, 4, DOECHO, NULL, true);
    if (ans[0] == 'Y' || ans[0] == 'y')
        newboard.flag |= BOARD_OUTFLAG;
    build_board_structure(newboard.filename);
    group = chgrp();
    if (group != NULL) {
        if (newboard.BM[0] != '\0')
            if (strlen(newboard.BM) <= 30)
                sprintf(vbuf, "%-38.38s(BM: %s)", newboard.title + 13, newboard.BM);
            else
                snprintf(vbuf, STRLEN, "%-28.28s(BM: %s)", newboard.title + 13, newboard.BM);
        else
            sprintf(vbuf, "%-38.38s", newboard.title + 13);

        if (add_grp(group, newboard.filename, vbuf, cexplain) == -1)
            prints("\n����������ʧ��....\n");
        else
            prints("�Ѿ����뾫����...\n");
        snprintf(newboard.ann_path,127,"%s/%s",group, newboard.filename);
        newboard.ann_path[127]=0;
    }
    if (add_board(&newboard) == -1) {
        move(t_lines - 1, 0);
        outs("����������ʧ��!\n");
        pressreturn();
        clear();
        return -1;
    }
    prints("\n������������\n");
    sprintf(genbuf, "add brd %s", newboard.filename);
    bbslog("user", "%s", genbuf);
    {
        char secu[STRLEN];

        sprintf(secu, "�����°棺%s", newboard.filename);
        securityreport(secu, NULL, NULL);
    }
    pressreturn();
    clear();
    return 0;
}

int m_editbrd()
{
    char bname[STRLEN], buf[STRLEN], oldtitle[STRLEN], vbuf[256], *group;
    char oldpath[STRLEN], newpath[STRLEN];
    int pos, noidboard, a_mv;
    struct boardheader fh, newfh;
    int line;

    struct boardheader* bh=NULL;
    char* groupname="";

    modify_user_mode(ADMIN);
    if (!check_systempasswd()) {
        return -1;
    }
    clear();
    stand_title("�޸���������Ѷ");
    move(1, 0);
    make_blist();
    namecomplete("��������������: ", bname);
    if (*bname == '\0') {
        move(2, 0);
        prints("���������������");
        pressreturn();
        clear();
        return -1;
    }
    pos = getboardnum(bname, &fh);
    if (!pos) {
        move(2, 0);
        prints("���������������");
        pressreturn();
        clear();
        return -1;
    }
    noidboard = anonymousboard(bname);
    move(2, 0);
    memcpy(&newfh, &fh, sizeof(newfh));
    prints("����������:   %s\n", fh.filename);
    prints("������˵��:   %s\n", fh.title);
    prints("����������Ա: %s\n", fh.BM);
    prints("����������:   %s ����������:   %s �Ƿ���Ŀ¼�� %s\n", 
        (noidboard) ? "Yes" : "No", (fh.flag & BOARD_JUNK) ? "Yes" : "No", (fh.flag & BOARD_GROUP) ? "Yes" : "No");
    if (newfh.group) {
        bh=getboard(newfh.group);
        if (bh) groupname=bh->filename;
    }
    prints("����Ŀ¼��%s\n",bh?groupname:"��");
    prints("������ת��:   %s    ��ճ������: %s    ���� Email ����: %s\n", 
			(fh.flag & BOARD_OUTFLAG) ? "Yes" : "No",
			(fh.flag & BOARD_ATTACH) ? "Yes" : "No",
			(fh.flag & BOARD_EMAILPOST) ? "Yes" : "No");
    if (fh.flag & BOARD_CLUB_READ || fh.flag & BOARD_CLUB_WRITE)
        prints("���ֲ�:   %s %s %s  ���: %d\n", fh.flag & BOARD_CLUB_READ ? "�Ķ�����" : "", fh.flag & BOARD_CLUB_WRITE ? "��������" : "", fh.flag & BOARD_CLUB_HIDE ? "����" : "", fh.clubnum);
    else
        prints("%s", "���ֲ�:   ��\n");
    strcpy(oldtitle, fh.title);
    prints("���� %s Ȩ��: %s      ��Ҫ���û�ְ��: %s(%d)", 
        (fh.level & PERM_POSTMASK) ? "POST" : "READ", 
        (fh.level & ~PERM_POSTMASK) == 0 ? "������" : "������",
        fh.title_level? get_user_title(fh.title_level):"��",fh.title_level);
    getdata(10, 0, "�Ƿ����������Ѷ? (Yes or No) [N]: ", genbuf, 4, DOECHO, NULL, true);
    if (*genbuf == 'y' || *genbuf == 'Y') {
        move(9, 0);
        prints("ֱ�Ӱ� <Return> ���޸Ĵ�����Ѷ\n");
      enterbname:
        getdata(10, 0, "������������: ", genbuf, BOARDNAMELEN, DOECHO, NULL, true);
        if (*genbuf != 0) {
            if (getboardnum(genbuf, NULL) > 0) {
                move(3, 0);
                prints("����! ���������Ѿ�����\n");
                move(11, 0);
                clrtobot();
                goto enterbname;
            }
            strncpy(newfh.filename, genbuf, sizeof(newfh.filename));
            strcpy(bname, genbuf);
        }
        line=11;
        getdata(line++, 0, "��������˵��: ", genbuf, 60, DOECHO, NULL, true);
        if (*genbuf != 0)
            strncpy(newfh.title, genbuf, sizeof(newfh.title));
        getdata(line++, 0, "����������Ա: ", genbuf, 60, DOECHO, NULL, true);
        if (*genbuf != 0)
            strncpy(newfh.BM, genbuf, sizeof(newfh.BM));
        if (*genbuf == ' ')
            strncpy(newfh.BM, "\0", sizeof(newfh.BM));
        /*
         * newfh.BM[ BM_LEN - 1 ]=fh.BM[ BM_LEN - 1 ]; 
         */
        sprintf(buf, "������ (Y/N)? [%c]: ", (noidboard) ? 'Y' : 'N');
        getdata(line++, 0, buf, genbuf, 4, DOECHO, NULL, true);
        if (*genbuf == 'y' || *genbuf == 'Y' || *genbuf == 'N' || *genbuf == 'n') {
            if (*genbuf == 'y' || *genbuf == 'Y')
                noidboard = 1;
            else
                noidboard = 0;
        }
        sprintf(buf, "���������� (Y/N)? [%c]: ", (newfh.flag & BOARD_JUNK) ? 'Y' : 'N');
        getdata(line++, 0, buf, genbuf, 4, DOECHO, NULL, true);
        if (*genbuf == 'y' || *genbuf == 'Y' || *genbuf == 'N' || *genbuf == 'n') {
            if (*genbuf == 'y' || *genbuf == 'Y')
                newfh.flag |= BOARD_JUNK;
            else
                newfh.flag &= ~BOARD_JUNK;
        };
        sprintf(buf, "������ת�� (Y/N)? [%c]: ", (newfh.flag & BOARD_OUTFLAG) ? 'Y' : 'N');
        getdata(line++, 0, buf, genbuf, 4, DOECHO, NULL, true);
        if (*genbuf == 'y' || *genbuf == 'Y' || *genbuf == 'N' || *genbuf == 'n') {
            if (*genbuf == 'y' || *genbuf == 'Y')
                newfh.flag |= BOARD_OUTFLAG;
            else
                newfh.flag &= ~BOARD_OUTFLAG;
        };
        sprintf(buf, "��ճ������ (Y/N)? [%c]: ", (newfh.flag & BOARD_ATTACH) ? 'Y' : 'N');
        getdata(line++, 0, buf, genbuf, 4, DOECHO, NULL, true);
        if (*genbuf == 'y' || *genbuf == 'Y' || *genbuf == 'N' || *genbuf == 'n') {
            if (*genbuf == 'y' || *genbuf == 'Y')
                newfh.flag |= BOARD_ATTACH;
            else
                newfh.flag &= ~BOARD_ATTACH;
        };
        sprintf(buf, "���� Email ���� (Y/N)? [%c]: ", (newfh.flag & BOARD_EMAILPOST) ? 'Y' : 'N');
        getdata(line++, 0, buf, genbuf, 4, DOECHO, NULL, true);
        if (*genbuf == 'y' || *genbuf == 'Y' || *genbuf == 'N' || *genbuf == 'n') {
            if (*genbuf == 'y' || *genbuf == 'Y')
                newfh.flag |= BOARD_EMAILPOST;
            else
                newfh.flag &= ~BOARD_EMAILPOST;
        };
        getdata(line++, 0, "�Ƿ��ƶ���������λ�� (Y/N)? [N]: ", genbuf, 4, DOECHO, NULL, true);
        if (*genbuf == 'Y' || *genbuf == 'y')
            a_mv = 2;           /* ��ʾ�ƶ�������Ŀ¼ */
        else
            a_mv = 0;
        sprintf(buf, "�Ƿ�Ϊ�����ƾ��ֲ�: (Y/N)? [%c]", (newfh.flag & BOARD_CLUB_READ) ? 'Y' : 'N');
        getdata(line++, 0, buf, genbuf, 4, DOECHO, NULL, true);
        if (*genbuf == 'Y' || *genbuf == 'y')
            newfh.flag |= BOARD_CLUB_READ;
        else if (*genbuf == 'N' || *genbuf == 'n')
            newfh.flag &= ~BOARD_CLUB_READ;
        sprintf(buf, "�Ƿ�Ϊ�������ƾ��ֲ�: (Y/N)? [%c]", (newfh.flag & BOARD_CLUB_WRITE) ? 'Y' : 'N');
        getdata(line++, 0, buf, genbuf, 4, DOECHO, NULL, true);
        if (*genbuf == 'Y' || *genbuf == 'y')
            newfh.flag |= BOARD_CLUB_WRITE;
        else if (*genbuf == 'N' || *genbuf == 'n')
            newfh.flag &= ~BOARD_CLUB_WRITE;
        if (newfh.flag & BOARD_CLUB_WRITE || newfh.flag & BOARD_CLUB_READ) {
            sprintf(buf, "�Ƿ�Ϊ�������ƾ��ֲ�: (Y/N)? [%c]", (newfh.flag & BOARD_CLUB_HIDE) ? 'Y' : 'N');
            getdata(line++, 0, buf, genbuf, 4, DOECHO, NULL, true);
            if (*genbuf == 'Y' || *genbuf == 'y')
                newfh.flag |= BOARD_CLUB_HIDE;
            else if (*genbuf == 'N' || *genbuf == 'n')
                newfh.flag &= ~BOARD_CLUB_HIDE;
        } else
            newfh.flag &= ~BOARD_CLUB_HIDE;
        
        sprintf(buf, "�Ƿ�ΪĿ¼ (Y/N)? [%c]", (newfh.flag & BOARD_GROUP) ? 'Y' : 'N');
        getdata(line++, 0, buf, genbuf, 4, DOECHO, NULL, true);
        if (*genbuf == 'Y' || *genbuf == 'y')
            newfh.flag |= BOARD_GROUP;
        else if (*genbuf == 'N' || *genbuf == 'n')
            newfh.flag &= ~BOARD_GROUP;

        while(1) {
            sprintf(buf, "�趨����Ŀ¼[%s]", groupname);
            strcpy(genbuf,groupname);
            getdata(line, 0, buf, genbuf, BOARDNAMELEN, DOECHO, NULL, false);
            if (*genbuf == 0) {
                newfh.group = 0;
                break;
            }
            newfh.group=getbnum(genbuf);
            if (newfh.group) {
		if (!(getboard(newfh.group)->flag&BOARD_GROUP)) {
                    move(line+1,0);
                    prints("����Ŀ¼");
		} else break;
            }
        }
        
        line++;
        
        getdata(line++, 0, "����ְ��: ", genbuf, 60, DOECHO, NULL, true); 
        if (*genbuf != 0)
            newfh.title_level=atoi(genbuf);

        getdata(line++, 0, "�Ƿ���Ĵ�ȡȨ�� (Y/N)? [N]: ", genbuf, 4, DOECHO, NULL, true);
        if (*genbuf == 'Y' || *genbuf == 'y') {
            char ans[5];

            sprintf(genbuf, "���� (R)�Ķ� �� (P)���� ���� [%c]: ", (newfh.level & PERM_POSTMASK ? 'P' : 'R'));
            getdata(line++, 0, genbuf, ans, 4, DOECHO, NULL, true);
            if ((newfh.level & PERM_POSTMASK) && (*ans == 'R' || *ans == 'r'))
                newfh.level &= ~PERM_POSTMASK;
            else if (!(newfh.level & PERM_POSTMASK) && (*ans == 'P' || *ans == 'p'))
                newfh.level |= PERM_POSTMASK;
            move(1, 0);
            clrtobot();
            move(2, 0);
            prints("�趨 %s '%s' ��������Ȩ��\n", newfh.level & PERM_POSTMASK ? "����" : "�Ķ�", newfh.filename);
            newfh.level = setperms(newfh.level, 0, "Ȩ��", NUMPERMS, showperminfo, NULL);
            clear();
            getdata(0, 0, "ȷ��Ҫ������? (Y/N) [N]: ", genbuf, 4, DOECHO, NULL, true);
        } else {
            getdata(line++, 0, "ȷ��Ҫ������? (Y/N) [N]: ", genbuf, 4, DOECHO, NULL, true);
        }
        if (*genbuf == 'Y' || *genbuf == 'y') {
            char lookgrp[30];
            int ret;

            {
                char secu[STRLEN];

                sprintf(secu, "�޸���������%s(%s)", fh.filename, newfh.filename);
                securityreport(secu, NULL, NULL);
            }
            if (strcmp(fh.filename, newfh.filename)) {
                char old[256], tar[256];

                a_mv = 1;       /* ��ʾ�����ı䣬��Ҫ���¾�����·�� */
                setbpath(old, fh.filename);
                setbpath(tar, newfh.filename);
                f_mv(old, tar);
                sprintf(old, "vote/%s", fh.filename);
                sprintf(tar, "vote/%s", newfh.filename);
                f_mv(old, tar);
            }
            if (newfh.BM[0] != '\0')
                if (strlen(newfh.BM) <= 30)
                    sprintf(vbuf, "%-38.38s(BM: %s)", newfh.title + 13, newfh.BM);
                else
                    snprintf(vbuf, STRLEN, "%-28.28s(BM: %s)", newfh.title + 13, newfh.BM);
            else
                sprintf(vbuf, "%-38.38s", newfh.title + 13);
            edit_grp(fh.filename, oldtitle + 13, vbuf);
            if (a_mv >= 1) {
                group = chgrp();
                /*
                 * ��ȡ�ð��Ӧ�� group 
                 */
                ann_get_path(fh.filename, newpath, sizeof(newpath));
                snprintf(oldpath, sizeof(oldpath), "0Announce/%s", newpath);
                sprintf(newpath, "0Announce/groups/%s/%s", group, newfh.filename);
                if (strcmp(oldpath, newpath) || a_mv != 2) {
                    if (group != NULL) {
                        if (newfh.BM[0] != '\0')
                            if (strlen(newfh.BM) <= 30)
                                sprintf(vbuf, "%-38.38s(BM: %s)", newfh.title + 13, newfh.BM);
                            else
                                sprintf(vbuf, "%-28.28s(BM: %s)", newfh.title + 13, newfh.BM);
                        else
                            sprintf(vbuf, "%-38.38s", newfh.title + 13);

                        if (add_grp(group, newfh.filename, vbuf, cexplain) == -1)
                            prints("\n����������ʧ��....\n");
                        else
                            prints("�Ѿ����뾫����...\n");
                        if (dashd(oldpath)) {
                            /*
                             * sprintf(genbuf, "/bin/rm -fr %s", newpath);
                             */
                            f_rm(newpath);
                        }
                        f_mv(oldpath, newpath);
                        del_grp(fh.filename, fh.title + 13);
                        snprintf(newfh.ann_path,127,"%s/%s",group, newfh.filename);
                        newfh.ann_path[127]=0;
                    }
                }
            }
            if (noidboard == 1 && !anonymousboard(newfh.filename)) {
                newfh.flag |= BOARD_ANNONY;
                addtofile("etc/anonymous", newfh.filename);
            } else if (noidboard == 0) {
                newfh.flag &= ~BOARD_ANNONY;
                del_from_file("etc/anonymous", newfh.filename);
            }
            set_board(pos, &newfh, &fh);
            sprintf(genbuf, "���������� %s ������ --> %s", fh.filename, newfh.filename);
            bbslog("user", "%s", genbuf);
        }
    }
    clear();
    return 0;
}

int searchtrace()
{
    int id;
    char tmp_command[80], tmp_id[20];
    char buf[8192];
    struct userec *lookupuser;

    if (check_systempasswd() == false)
        return -1;
    modify_user_mode(ADMIN);
    clear();
    stand_title("��ѯʹ���߷��ļ�¼");
    move(1, 0);
    usercomplete("������ʹ�����ʺ�:", genbuf);
    strcpy(tmp_id, genbuf);
    if (tmp_id[0] == '\0') {
        clear();
        return -1;
    }

    if (!(id = getuser(genbuf, &lookupuser))) {
        move(3, 0);
        prints("����ȷ��ʹ���ߴ���\n");
        clrtoeol();
        pressreturn();
        clear();
        return -1;
    }

    sprintf(tmp_command, "grep -a -w %s user.log | grep posted > tmp/searchresult.%d", tmp_id, getpid());
    system(tmp_command);
    sprintf(tmp_command, "tmp/searchresult.%d", getpid());
    mail_file(currentuser->userid, tmp_command, currentuser->userid, "ϵͳ��ѯ���", BBSPOST_MOVE, NULL);

    sprintf(buf, "��ѯ�û� %s �ķ������", tmp_id);
    securityreport(buf, lookupuser, NULL);      /*д��syssecurity��, stephen 2000.12.21 */
    sprintf(buf, "Search the posts by %s in the trace", tmp_id);
    bbslog("user", "%s", buf);  /*д��trace, stephen 2000.12.21 */

    move(3, 0);
    prints("��ѯ����Ѿ��ĵ��������䣡 \n");
    pressreturn();
    clear();
    return 0;
}                               /* stephen 2000.12.15 let sysop search in trace */


/*
char curruser[IDLEN + 2];
extern int delmsgs[];
extern int delcnt;

void domailclean(struct fileheader *fhdrp, char *arg)
{
    static int newcnt, savecnt, deleted, idc;
    char buf[STRLEN];

    if (fhdrp == NULL) {
        bbslog("clean", "new = %d, saved = %d, deleted = %d", newcnt, savecnt, deleted);
        newcnt = savecnt = deleted = idc = 0;
        if (delcnt) {
            setmailfile(buf, curruser, DOT_DIR);
            while (delcnt--)
                delete_record(buf, sizeof(struct fileheader), delmsgs[delcnt], NULL, NULL);
        }
        delcnt = 0;
        return;
    }
    idc++;
    if (!(fhdrp->accessed[0] & FILE_READ))
        newcnt++;
    else if (fhdrp->accessed[0] & FILE_MARKED)
        savecnt++;
    else {
        deleted++;
        setmailfile(buf, curruser, fhdrp->filename);
        unlink(buf);
        delmsgs[delcnt++] = idc;
    }
}

int cleanmail(struct userec *urec, char *arg)
{
    struct stat statb;

    if (urec->userid[0] == '\0' || !strcmp(urec->userid, "new"))
        return 0;
    setmailfile(genbuf, urec->userid, DOT_DIR);
    if (stat(genbuf, &statb) == -1) {
        bbslog("clean", "%s no mail", urec->userid);
    } else {
        if (statb.st_size == 0) {
            bbslog("clean", "%s no mail", urec->userid);
        } else {
            strcpy(curruser, urec->userid);
            delcnt = 0;
            apply_record(genbuf, (RECORD_FUNC_ARG) domailclean, sizeof(struct fileheader), 0, 1);
            domailclean(NULL, 0);
        }
    }
    return 0;
}

int m_mclean()
{
    char ans[5];

    modify_user_mode(ADMIN);
    if (!check_systempasswd()) {
        return -1;
    }
    clear();
    stand_title("���˽���ż�");
    move(1, 0);
    prints("��������Ѷ���δ mark ���ż�\n");
    getdata(2, 0, "ȷ���� (Y/N)? [N]: ", ans, 3, DOECHO, NULL, true);
    if (ans[0] != 'Y' && ans[0] != 'y') {
        clear();
        return 0;
    }
    {
        char secu[STRLEN];

        sprintf(secu, "�������ʹ�����Ѷ��ż���");
        securityreport(secu, NULL, NULL);
    }

    move(3, 0);
    prints("�����ĵȺ�.\n");
    refresh();
    apply_users(cleanmail, 0);
    move(4, 0);
    prints("������! ��鿴��־�ļ�.\n");
    bbslog("user","%s","Mail Clean");
    pressreturn();
    clear();
    return 0;
}
*/

void trace_state(flag, name, size)
int flag, size;
char *name;
{
    char buf[STRLEN];

    if (flag != -1) {
        sprintf(buf, "ON (size = %d)", size);
    } else {
        strcpy(buf, "OFF");
    }
    prints("%s��¼ %s\n", name, buf);
}

int touchfile(filename)
char *filename;
{
    int fd;

    if ((fd = open(filename, O_RDWR | O_CREAT, 0600)) > 0) {
        close(fd);
    }
    return fd;
}

int m_trace()
{
    struct stat ostatb, cstatb;
    int otflag, ctflag, done = 0;
    char ans[3];
    char *msg;

    modify_user_mode(ADMIN);
    if (!check_systempasswd()) {
        return -1;
    }
    clear();
    stand_title("Set Trace Options");
    while (!done) {
        move(2, 0);
        otflag = stat("trace", &ostatb);
        ctflag = stat("trace.chatd", &cstatb);
        prints("Ŀǰ�趨:\n");
        trace_state(otflag, "һ��", ostatb.st_size);
        trace_state(ctflag, "����", cstatb.st_size);
        move(9, 0);
        prints("<1> �л�һ���¼\n");
        prints("<2> �л������¼\n");
        getdata(12, 0, "��ѡ�� (1/2/Exit) [E]: ", ans, 2, DOECHO, NULL, true);

        switch (ans[0]) {
        case '1':
            if (otflag) {
                touchfile("trace");
                msg = "һ���¼ ON";
            } else {
                f_mv("trace", "trace.old");
                msg = "һ���¼ OFF";
            }
            break;
        case '2':
            if (ctflag) {
                touchfile("trace.chatd");
                msg = "�����¼ ON";
            } else {
                f_mv("trace.chatd", "trace.chatd.old");
                msg = "�����¼ OFF";
            }
            break;
        default:
            msg = NULL;
            done = 1;
        }
        move(t_lines - 2, 0);
        if (msg) {
            prints("%s\n", msg);
            bbslog("user", "%s", msg);
        }
    }
    clear();
    return 0;
}

int valid_userid(ident)         /* check the user has registed, added by dong, 1999.4.18 */
char *ident;
{
    if (strchr(ident, '@') && valid_ident(ident))
        return 1;
    return 0;
}

int check_proxy_IP(ip, buf)
                                /*
                                 * added for rejection of register from proxy,
                                 * Bigman, 2001.11.9 
                                 */
 /*
  * ��bbsd_single�����local_check_ban_IP����һ�������Կ��ǹ��� 
  */
char *ip;
char *buf;
{                               /* Leeward 98.07.31
                                 * RETURN:
                                 * - 1: No any banned IP is defined now
                                 * 0: The checked IP is not banned
                                 * other value over 0: The checked IP is banned, the reason is put in buf
                                 */
    FILE *Ban = fopen("etc/proxyIP", "r");
    char IPBan[64];
    int IPX = -1;
    char *ptr;

    if (!Ban)
        return IPX;
    else
        IPX++;

    while (fgets(IPBan, 64, Ban)) {
        if ((ptr = strchr(IPBan, '\n')) != NULL)
            *ptr = 0;
        if ((ptr = strchr(IPBan, ' ')) != NULL) {
            *ptr++ = 0;
            strcpy(buf, ptr);
        }
        IPX = strlen(ip);
        if (!strncmp(ip, IPBan, IPX))
            break;
        IPX = 0;
    }

    fclose(Ban);
    return IPX;
}

int apply_reg(regfile, fname, pid, num)
/* added by Bigman, 2002.5.31 */
/* ����ָ������ע�ᵥ */
char *regfile, *fname;
long pid;
int num;
{
    FILE *in_fn, *out_fn, *tmp_fn;
    char fname1[STRLEN], fname2[STRLEN];
    int sum, fd;
    char *ptr;

    strcpy(fname1, "reg.ctrl");

    if ((in_fn = fopen(regfile, "r+")) == NULL) {
        move(2, 0);
        prints("ϵͳ����, �޷���ȡע�����ϵ�: %s\n", regfile);
        pressreturn();
        return -1;
    }

    fd = fileno(in_fn);
    flock(fd, LOCK_EX);

    if ((out_fn = fopen(fname, "w")) == NULL) {
        move(2, 0);
        flock(fd, LOCK_UN);
        fclose(in_fn);
        prints("ϵͳ����, �޷�д��ʱע�����ϵ�: %s\n", fname);
        pressreturn();
        return -1;
    }
    sum = 0;

    while (fgets(genbuf, STRLEN, in_fn) != NULL) {
        if ((ptr = (char *) strstr(genbuf, "----")) != NULL)
            sum++;

        fputs(genbuf, out_fn);

        if (sum >= num)
            break;
    }
    fclose(out_fn);

    if (sum >= num) {
        sum = 0;

		gettmpfilename( fname2, "reg" );
        //sprintf(fname2, "tmp/reg.%ld", pid);

        if ((tmp_fn = fopen(fname2, "w")) == NULL) {
            prints("���ܽ�����ʱ�ļ�:%s\n", fname2);
            flock(fd, LOCK_UN);
            fclose(in_fn);
            pressreturn();
            return -1;
        }

        while (fgets(genbuf, STRLEN, in_fn) != NULL) {
            if ((ptr = (char *) strstr(genbuf, "userid")) != NULL)
                sum++;
            fputs(genbuf, tmp_fn);

        }

        flock(fd, LOCK_UN);

        fclose(in_fn);
        fclose(tmp_fn);

        if (sum > 0) {
            f_rm(regfile);
            f_mv(fname2, regfile);
        } else
            f_rm(regfile);

        f_rm(fname2);

    }

    else
        f_rm(regfile);

    if ((out_fn = fopen(fname1, "a")) == NULL) {
        move(2, 0);
        prints("ϵͳ����, �޷�����ע������ļ�: %s\n", fname1);
        pressreturn();
        return -1;
    }

    fd = fileno(out_fn);

    flock(fd, LOCK_UN);
    fprintf(out_fn, "%ld\n", pid);
    flock(fd, LOCK_UN);
    fclose(out_fn);

    return (0);
}

int check_reg(mod)
int mod;

/* added by Bigman, 2002.5.31 */
/* mod=0 ���reg_control�ļ� */
/* mod=1 �����˳�ɾ�����ļ� */
{
    FILE *fn1, *fn2;
    char fname1[STRLEN];
    char fname2[STRLEN];
    long myid;
    int flag = 0, fd;

    strcpy(fname1, "reg.ctrl");

    if ((fn1 = fopen(fname1, "r")) != NULL) {

        fd = fileno(fn1);
        flock(fd, LOCK_EX);

		gettmpfilename( fname2, "reg.c");
        //sprintf(fname2, "tmp/reg.c%ld", getpid());

        if ((fn2 = fopen(fname2, "w")) == NULL) {
            prints("���ܽ�����ʱ�ļ�:%s\n", fname2);
            flock(fd, LOCK_UN);
            fclose(fn1);
            pressreturn();
            return -1;
        } else {
            while (fgets(genbuf, STRLEN, fn1) != NULL) {

                myid = atol(genbuf);

                if (mod == 0) {
/*                    	if (myid==getpid())
                    {
                	prints("��ֻ��һ�����̽��������ʺ�");
                	pressreturn();
                	return -1;
                    }
*/

                    if (kill(myid, 0) == -1) {  /*ע���м�����ˣ��ָ� */
                        flag = 1;
                        restore_reg(myid);
                    } else {
                        fprintf(fn2, "%ld\n", myid);
                    }
                } else {
                    flag = 1;
                    if (myid != getpid())
                        fprintf(fn2, "%ld\n", myid);


                }

            }
            fclose(fn2);
        }
        flock(fd, LOCK_UN);
        fclose(fn1);

        if (flag == 1) {
            f_rm(fname1);
            f_mv(fname2, fname1);
        }
        f_rm(fname2);

    }

    return (0);
}

int restore_reg(pid)
long pid;

/* added by Bigman, 2002.5.31 */
/* �ָ����ߵ�ע���ļ� */
{
    FILE *fn, *freg;
    char *regfile, buf[STRLEN];
    int fd1, fd2;

    regfile = "new_register";

    sprintf(buf, "register.%ld", pid);

    if ((fn = fopen(buf, "r")) != NULL) {
        fd1 = fileno(fn);
        flock(fd1, LOCK_EX);

        if ((freg = fopen(regfile, "a")) != NULL) {
            fd2 = fileno(freg);
            flock(fd2, LOCK_EX);
            while (fgets(genbuf, STRLEN, fn) != NULL)
                fputs(genbuf, freg);
            flock(fd2, LOCK_UN);
            fclose(freg);

        }
        flock(fd1, LOCK_UN);
        fclose(fn);

        f_rm(buf);
    }

    return (0);
}
static const char *field[] = { "usernum", "userid", "realname", "career",
    "addr", "phone", "birth", NULL
};
static const char *reason[] = {
    "��������ʵ����(�������ƴ��).", "������ѧУ��ϵ������λ.",
    "����д������סַ����.", "����������绰(���޿��ú�����Email��ַ����).",
    "��ȷʵ����ϸ����дע�������.", "����������д���뵥.",
    "������Ӵ���ע��", "ͬһ���û�ע���˹���ID",
    NULL
};

int scan_register_form(logfile, regfile)
char *logfile, *regfile;
{
    static const char *finfo[] = { "�ʺ�λ��", "�������", "��ʵ����", "����λ",
        "Ŀǰסַ", "����绰", "��    ��", NULL
    };
    struct userec uinfo;
    FILE *fn, *fout, *freg;
    char fdata[7][STRLEN];
    char fname[STRLEN], buf[STRLEN], buff;

    /*
     * ^^^^^ Added by Marco 
     */
    char ans[5], *ptr, *uid;
    int n, unum, fd;
    int count, sum, total_num;  /*Haohmaru.2000.3.9.���㻹�ж��ٵ���û���� */
    char result[256], ip[17];   /* Added for IP query by Bigman: 2002.8.20 */
    long pid;                   /* Added by Bigman: 2002.5.31 */

    uid = currentuser->userid;


    stand_title("�����趨������ע������");
/*    sprintf(fname, "%s.tmp", regfile);*/

    pid = getpid();
    sprintf(fname, "register.%ld", pid);

    move(2, 0);
    if (dashf(fname)) {
/*        prints("[1m���� SYSOP ����ʹ�� telnet �� WWW �鿴ע�����뵥������ʹ����״̬��\n\n");
        prints("[33m���û������ SYSOP ���ڲ鿴ע�����뵥���������ڶ�����ɵ��޷�ע�ᡣ\n");
        prints("��� bbsroot �ʻ�����һ���������\n");
        prints("                                   [32mcat new_register.tmp >> new_register[33m\n");
        prints("ȷ�������������гɹ���������һ���������\n");
        prints("                                            [32mrm new_register.tmp\n[m");
        pressreturn();
        return -1;*/

        restore_reg(pid);       /* Bigman,2002.5.31:�ָ����ļ� */
    }
/*    f_mv(regfile, fname);*/
/*����ע�ᵥ added by Bigman, 2002.5.31*/

/*ͳ���ܵ�ע�ᵥ�� Bigman, 2002.6.2 */
    if ((fn = fopen(regfile, "r")) == NULL) {
        move(2, 0);
        prints("ϵͳ����, �޷���ȡע�����ϵ�: %s\n", fname);
        pressreturn();
        return -1;
    }

    fd = fileno(fn);
    flock(fd, LOCK_EX);

    total_num = 0;
    while (fgets(genbuf, STRLEN, fn) != NULL) {
        if ((ptr = (char *) strstr(genbuf, "userid")) != NULL)
            total_num++;
    }
    flock(fd, LOCK_UN);
    fclose(fn);

    apply_reg(regfile, fname, pid, 50);

    if ((fn = fopen(fname, "r")) == NULL) {
        move(2, 0);
        prints("ϵͳ����, �޷���ȡע�����ϵ�: %s\n", fname);
        pressreturn();
        return -1;
    }
    memset(fdata, 0, sizeof(fdata));
    /*
     * Haohmaru.2000.3.9.���㹲�ж��ٵ��� 
     */
    sum = 0;
    while (fgets(genbuf, STRLEN, fn) != NULL) {
        if ((ptr = (char *) strstr(genbuf, "userid")) != NULL)
            sum++;
    }
    fseek(fn, 0, SEEK_SET);
    count = 1;
    while (fgets(genbuf, STRLEN, fn) != NULL) {
        struct userec *lookupuser;

        if ((ptr = (char *) strstr(genbuf, ": ")) != NULL) {
            *ptr = '\0';
            for (n = 0; field[n] != NULL; n++) {
                if (strcmp(genbuf, field[n]) == 0) {
                    strcpy(fdata[n], ptr + 2);
                    if ((ptr = (char *) strchr(fdata[n], '\n')) != NULL)
                        *ptr = '\0';
                }
            }
        } else if ((unum = getuser(fdata[1], &lookupuser)) == 0) {
            move(2, 0);
            clrtobot();
            prints("ϵͳ����, ���޴��ʺ�.\n\n");
            for (n = 0; field[n] != NULL; n++)
                prints("%s     : %s\n", finfo[n], fdata[n]);
            pressreturn();
            memset(fdata, 0, sizeof(fdata));
        } else {
            struct userdata ud;

            uinfo = *lookupuser;
            move(1, 0);
            prints("�ʺ�λ��     : %d   ���� %d ��ע�ᵥ����ǰΪ�� %d �ţ���ʣ %d ��\n", unum, total_num, count++, sum - count + 1);    /*Haohmaru.2000.3.9.���㻹�ж��ٵ���û���� */
            disply_userinfo(&uinfo, 1);
			
			read_userdata(lookupuser->userid, &ud);

/* ��Ӳ�ѯIP, Bigman: 2002.8.20 */
            /*move(8, 20);*/
	     move(8,30); /* ������ŲŲ��  binxun . 2003.5.30 */
            strncpy(ip, uinfo.lasthost, 17);
            find_ip(ip, 2, result);
            prints("\033[33m%s\033[m", result);

            move(15, 0);
            printdash(NULL);
            for (n = 0; field[n] != NULL; n++) {
                /*
                 * added for rejection of register from proxy
                 */
                /*
                 * Bigman, 2001.11.9
                 */
                 clrtoeol();
                if (n == 1) {
                    if (check_proxy_IP(uinfo.lasthost, buf) > 0)
                        prints("%s     : %s \033[33m%s\033[m\n", finfo[n], fdata[n], buf);
                    else
                        prints("%s     : %s\n", finfo[n], fdata[n]);
                } else
                    prints("%s     : %s\n", finfo[n], fdata[n]);
            }
            /*
             * if (uinfo.userlevel & PERM_LOGINOK) modified by dong, 1999.4.18 
             */
            if ((uinfo.userlevel & PERM_LOGINOK) || valid_userid(ud.realemail)) {
                move(t_lines - 1, 0);
                prints("���ʺŲ�������дע�ᵥ.\n");
                pressanykey();
                ans[0] = 'D';
            } else {
                getdata(t_lines - 1, 0, "�Ƿ���ܴ����� (Y/N/Q/Del/Skip)? [S]: ", ans, 3, DOECHO, NULL, true);
            }
            move(2, 0);
            clrtobot();
            switch (ans[0]) {
            case 'D':
            case 'd':
                break;
            case 'Y':
            case 'y':
			{
				struct usermemo *um;

				read_user_memo(uinfo.userid, &um);

                prints("����ʹ���������Ѿ�����:\n");
                n = strlen(fdata[5]);
                if (n + strlen(fdata[3]) > 60) {
                    if (n > 40)
                        fdata[5][n = 40] = '\0';
                    fdata[3][60 - n] = '\0';
                }
                strncpy(ud.realname, fdata[2], NAMELEN);
                strncpy(ud.address, fdata[4], NAMELEN);
                sprintf(genbuf, "%s$%s@%s", fdata[3], fdata[5], uid);
		if(strlen(genbuf) >= STRLEN-16)
			sprintf(genbuf, "%s@%s",fdata[5],uid);
                strncpy(ud.realemail, genbuf, STRLEN - 16);
		ud.realemail[STRLEN - 16 - 1] = '\0';
                sprintf(buf, "tmp/email/%s", uinfo.userid);
                if ((fout = fopen(buf, "w")) != NULL) {
                    fprintf(fout, "%s\n", genbuf);
                    fclose(fout);
                }

                update_user(&uinfo, unum, 0);
                write_userdata(uinfo.userid, &ud);
				memcpy(&(um->ud), &ud, sizeof(ud));
				end_mmapfile(um, sizeof(struct usermemo), -1);

                mail_file(currentuser->userid, "etc/s_fill", uinfo.userid, "�����㣬���Ѿ����ע�ᡣ", 0, NULL);
                sprintf(genbuf, "%s �� %s ͨ�����ȷ��.", uid, uinfo.userid);
                securityreport(genbuf, lookupuser, fdata);
                if ((fout = fopen(logfile, "a")) != NULL) {
                    time_t now;

                    for (n = 0; field[n] != NULL; n++)
                        fprintf(fout, "%s: %s\n", field[n], fdata[n]);
                    now = time(NULL);
                    fprintf(fout, "Date: %s\n", Ctime(now));
                    fprintf(fout, "Approved: %s\n", uid);
                    fprintf(fout, "----\n");
                    fclose(fout);
                }
                /*
                 * user_display( &uinfo, 1 ); 
                 */
                /*
                 * pressreturn(); 
                 */

                /*
                 * ����ע����Ϣ��¼ 2001.11.11 Bigman 
                 */
                sethomefile(buf, uinfo.userid, "/register");
                if ((fout = fopen(buf, "w")) != NULL) {
                    for (n = 0; field[n] != NULL; n++)
                        fprintf(fout, "%s     : %s\n", finfo[n], fdata[n]);
                    fprintf(fout, "�����ǳ�     : %s\n", uinfo.username);
                    fprintf(fout, "�����ʼ����� : %s\n", ud.email);
                    fprintf(fout, "��ʵ E-mail  : %s\n", ud.realemail);
                    fprintf(fout, "ע������     : %s\n", ctime(&uinfo.firstlogin));
                    fprintf(fout, "ע��ʱ�Ļ��� : %s\n", uinfo.lasthost);
                    fprintf(fout, "Approved: %s\n", uid);
                    fclose(fout);
                }

                break;
			}
            case 'Q':
            case 'q':
                if ((freg = fopen(regfile, "a")) != NULL) {
                    fd = fileno(freg);
                    flock(fd, LOCK_EX);

                    for (n = 0; field[n] != NULL; n++)
                        fprintf(freg, "%s: %s\n", field[n], fdata[n]);
                    fprintf(freg, "----\n");
                    while (fgets(genbuf, STRLEN, fn) != NULL)
                        fputs(genbuf, freg);

                    flock(fd, LOCK_UN);
                    fclose(freg);
                }

                break;
            case 'N':
            case 'n':
                for (n = 0; field[n] != NULL; n++)
                    prints("%s: %s\n", finfo[n], fdata[n]);
                move(9, 0);
                prints("��ѡ��/�����˻������ԭ��, �� <enter> ȡ��.\n");
                for (n = 0; reason[n] != NULL; n++)
                    prints("%d) %s\n", n, reason[n]);
                getdata(10 + n, 0, "�˻�ԭ��: ", buf, STRLEN, DOECHO, NULL, true);
                buff = buf[0];  /* Added by Marco */
                if (buf[0] != '\0') {
                    if (buf[0] >= '0' && buf[0] < '0' + n) {
                        strcpy(buf, reason[buf[0] - '0']);
                    }
                    sprintf(genbuf, "<ע��ʧ��> - %s", buf);
                    strncpy(ud.address, genbuf, NAMELEN);
                    write_userdata(uinfo.userid, &ud);
                    update_user(&uinfo, unum, 0);

                    /*
                     * ------------------- Added by Marco 
                     */
                    switch (buff) {
                    case '0':
                        mail_file(currentuser->userid, "etc/f_fill.realname", uinfo.userid, ud.address, BBSPOST_LINK, NULL);
                        break;
                    case '1':
                        mail_file(currentuser->userid, "etc/f_fill.unit", uinfo.userid, ud.address, BBSPOST_LINK, NULL);
                        break;
                    case '2':
                        mail_file(currentuser->userid, "etc/f_fill.address", uinfo.userid, ud.address, BBSPOST_LINK, NULL);
                        break;
                    case '3':
                        mail_file(currentuser->userid, "etc/f_fill.telephone", uinfo.userid, ud.address, BBSPOST_LINK, NULL);
                        break;
                    case '4':
                        mail_file(currentuser->userid, "etc/f_fill.real", uinfo.userid, ud.address, BBSPOST_LINK, NULL);
                        break;
                    case '5':
                        mail_file(currentuser->userid, "etc/f_fill.chinese", uinfo.userid, ud.address, BBSPOST_LINK, NULL);
                        break;
                    case '6':
                        mail_file(currentuser->userid, "etc/f_fill.proxy", uinfo.userid, ud.address, BBSPOST_LINK, NULL);
                        break;
                    case '7':
                        mail_file(currentuser->userid, "etc/f_fill.toomany", uinfo.userid, ud.address, BBSPOST_LINK, NULL);
                        break;
                    default:
                        mail_file(currentuser->userid, "etc/f_fill", uinfo.userid, ud.address, BBSPOST_LINK, NULL);
                        break;
                    }
                    /*
                     * -------------------------------------------------------
                     */
                    /*
                     * user_display( &uinfo, 1 ); 
                     */
                    /*
                     * pressreturn(); 
                     */
                    break;
                }
                move(10, 0);
                clrtobot();
                prints("ȡ���˻ش�ע�������.\n");
                /*
                 * run default -- put back to regfile 
                 */
            default:
                if ((freg = fopen(regfile, "a")) != NULL) {
                    fd = fileno(freg);
                    flock(fd, LOCK_EX);

                    for (n = 0; field[n] != NULL; n++)
                        fprintf(freg, "%s: %s\n", field[n], fdata[n]);
                    fprintf(freg, "----\n");

                    flock(fd, LOCK_UN);
                    fclose(freg);
                }
            }
            memset(fdata, 0, sizeof(fdata));
        }
    }                           /* while */

    check_reg(1);               /* Bigman:2002.5.31 */

    fclose(fn);
    unlink(fname);
    return (0);
}

int m_register()
{
    FILE *fn;
    char ans[3], *fname;
    int x, y, wid, len;

    modify_user_mode(ADMIN);
    if (!check_systempasswd()) {
        return -1;
    }
    clear();

    if (check_reg(0) != 0)
        return (-1);            /* added by Bigman, 2002.5.31 */

    stand_title("�趨ʹ����ע������");
    move(2, 0);

    fname = "new_register";

    if ((fn = fopen(fname, "r")) == NULL) {
        prints("Ŀǰ������ע������.");
        pressreturn();
    } else {
        y = 2, x = wid = 0;
        while (fgets(genbuf, STRLEN, fn) != NULL && x < 65) {
            if (strncmp(genbuf, "userid: ", 8) == 0) {
                move(y++, x);
                prints(genbuf + 8);
                len = strlen(genbuf + 8);
                if (len > wid)
                    wid = len;
                if (y >= t_lines - 2) {
                    y = 2;
                    x += wid + 2;
                }
            }
        }
        fclose(fn);
        getdata(t_lines - 1, 0, "�趨������ (Y/N)? [N]: ", ans, 2, DOECHO, NULL, true);
        if (ans[0] == 'Y' || ans[0] == 'y') {
            {
                char secu[STRLEN];

                sprintf(secu, "�趨ʹ����ע������");
                securityreport(secu, NULL, NULL);
            }
            scan_register_form("register.list", fname);
        }
    }
    clear();
    return 0;
}

int m_stoplogin()
{
    char ans[4];

    modify_user_mode(ADMIN);
    if (!check_systempasswd()) {
        return -1;
    }
    if (!HAS_PERM(currentuser, PERM_ADMIN))
        return -1;
    getdata(t_lines - 1, 0, "��ֹ��½�� (Y/N)? [N]: ", ans, 2, DOECHO, NULL, true);
    if (ans[0] == 'Y' || ans[0] == 'y') {
        if (vedit("NOLOGIN", false, NULL, NULL) == -1)
            unlink("NOLOGIN");
    }
    return 0;
}

/* czz added 2002.01.15 */
int inn_start()
{
    char ans[4], tmp_command[80];

    getdata(t_lines - 1, 0, "����ת���� (Y/N)? [N]: ", ans, 2, DOECHO, NULL, true);
    if (ans[0] == 'Y' || ans[0] == 'y') {
        sprintf(tmp_command, "~bbs/innd/innbbsd");
        system(tmp_command);
    }
    return 0;
}

int inn_reload()
{
    char ans[4], tmp_command[80];

    getdata(t_lines - 1, 0, "�ض������� (Y/N)? [N]: ", ans, 2, DOECHO, NULL, true);
    if (ans[0] == 'Y' || ans[0] == 'y') {
        sprintf(tmp_command, "~bbs/innd/ctlinnbbsd reload");
        system(tmp_command);
    }
    return 0;
}

int inn_stop()
{
    char ans[4], tmp_command[80];

    getdata(t_lines - 1, 0, "ֹͣת���� (Y/N)? [N]: ", ans, 2, DOECHO, NULL, true);
    if (ans[0] == 'Y' || ans[0] == 'y') {
        sprintf(tmp_command, "~bbs/innd/ctlinnbbsd shutdown");
        system(tmp_command);
    }
    return 0;
}

/* added end */
/* ���Ȩ�޹���*/
int x_deny()
{
    int sel;
    char userid[IDLEN + 2];
    struct userec *lookupuser;
    const int level[] = {
        PERM_BASIC,
        PERM_POST,
        PERM_DENYMAIL,
        PERM_CHAT,
        PERM_PAGE,
        PERM_DENYRELAX,
        -1
    };
    const int normal_level[] = {
        PERM_BASIC,
        PERM_POST,
        0,
        PERM_CHAT,
        PERM_PAGE,
        0,
        -1
    };

    const struct _select_item level_conf[] = {
        {3, 6, -1, SIT_SELECT, (void *) "1)��¼Ȩ��"},
        {3, 7, -1, SIT_SELECT, (void *) "2)��������Ȩ��"},
        {3, 8, -1, SIT_SELECT, (void *) "3)����Ȩ��"},
        {3, 9, -1, SIT_SELECT, (void *) "4)����������Ȩ��"},
        {3, 10, -1, SIT_SELECT, (void *) "5)��������Ȩ��"},
        {3, 11, -1, SIT_SELECT, (void *) "6)��������Ȩ��"},
        {3, 12, -1, SIT_SELECT, (void *) "7)��һ��ID"},
        {3, 13, -1, SIT_SELECT, (void *) "8)�˳�"},
        {-1, -1, -1, 0, NULL}
    };

    modify_user_mode(ADMIN);
    if (!check_systempasswd()) {
        return -1;
    }
    move(0, 0);
    clear();

    while (1) {
        int i;
        int basicperm;
        int s[10][2];
        int lcount;

        move(1, 0);

        usercomplete("������ʹ�����ʺ�:", genbuf);
        strncpy(userid, genbuf, IDLEN + 1);
        if (userid[0] == '\0') {
            clear();
            return 0;
        }

        if (!(getuser(userid, &lookupuser))) {
            move(3, 0);
            prints("����ȷ��ʹ���ߴ���\n");
            clrtoeol();
            pressreturn();
            clear();
            continue;
        }
        lcount = get_giveupinfo(lookupuser->userid, &basicperm, s);
        move(3, 0);
        clrtobot();

        for (i = 0; level[i] != -1; i++)
            if ((lookupuser->userlevel & level[i]) != normal_level[i]) {
                move(6 + i, 40);
                if (level[i] & basicperm)
                    prints("������");
                else
                    prints("�����");
            }
        sel = simple_select_loop(level_conf, SIF_NUMBERKEY | SIF_SINGLE, 0, 6, NULL);
        if (sel == i + 2)
            break;
        if (sel > 0 && sel <= i) {
            /*char buf[40];  commented by binxun*/
            /*---------*/
            char buf[STRLEN]; /*buf is too small...changed by binxun .2003/05/11 */
            /*---------*/
            char reportbuf[120];

            move(40, 0);
            if ((lookupuser->userlevel & level[sel - 1]) == normal_level[sel - 1]) {
                sprintf(buf, "���Ҫ���%s��%s", lookupuser->userid, (char *) level_conf[sel - 1].data + 2);
                if (askyn(buf, 0) != 0) {
                    sprintf(reportbuf, "���%s��%s ", lookupuser->userid, (char *) level_conf[sel - 1].data + 2);
                    lookupuser->userlevel ^= level[sel - 1];
                    securityreport(reportbuf, lookupuser, NULL);
                    break;
                }
            } else {
                if (!(basicperm & level[sel - 1])) {
                    sprintf(buf, "���Ҫ�⿪%s��%s ���", lookupuser->userid, (char *) level_conf[sel - 1].data + 2);
                    sprintf(reportbuf, "�⿪%s��%s ���", lookupuser->userid, (char *) level_conf[sel - 1].data + 2);
                } else {
                    sprintf(buf, "���Ҫ�⿪%s��%s ����", lookupuser->userid, (char *) level_conf[sel - 1].data + 2);
                    sprintf(reportbuf, "�⿪%s��%s ����", lookupuser->userid, (char *) level_conf[sel - 1].data + 2);
                }
                if (askyn(buf, 0) != 0) {
                    lookupuser->userlevel ^= level[sel - 1];
                    securityreport(reportbuf, lookupuser, NULL);
                    save_giveupinfo(lookupuser, lcount, s);
                    break;
                }
            }
        }
    }
    return 0;
}

int set_BM()
/* ֱ��������� Bigman:2002.9.1 */
{

    char bname[STRLEN], oldtitle[STRLEN], vbuf[256], *p;
    int pos, flag = 0, id, m, n, brd_num;
    unsigned int newlevel;
    struct boardheader fh, newfh;
    struct userec *lookupuser, uinfo;
    struct boardheader *bptr;
    char lookgrp[30];

    if (!HAS_PERM(currentuser, PERM_ADMIN) || !HAS_PERM(currentuser, PERM_SYSOP)) {
        move(3, 0);
        clrtobot();
        prints("��Ǹ, ֻ��ADMINȨ�޵Ĺ���Ա�����޸������û�Ȩ��");
        pressreturn();
        return 0;
    }

    modify_user_mode(ADMIN);
    if (!check_systempasswd()) {
        return -1;
    }
    clear();
    stand_title("�������");
    move(1, 0);
    make_blist();
    namecomplete("��������������: ", bname);
    if (*bname == '\0') {
        move(2, 0);
        prints("���������������");
        pressreturn();
        clear();
        return -1;
    }
    pos = getboardnum(bname, &fh);
    memcpy(&newfh, &fh, sizeof(newfh));
    if (!pos) {
        move(2, 0);
        prints("���������������");
        pressreturn();
        clear();
        return -1;
    }
    while (1) {
        clear();
        stand_title("�������");
        move(1, 0);
        prints("����������:   %s\n", fh.filename);
        prints("������˵��:   %s\n", fh.title);
        prints("����������Ա: %s\n", fh.BM);
        strcpy(oldtitle, fh.title);


        getdata(6, 0, "(A)���Ӱ��� (D)ɾ������ (Q)�˳�?: [Q]", genbuf, 2, DOECHO, NULL, true);
        if (*genbuf == 'a' || *genbuf == 'A') {
            flag = 1;
        } else if (*genbuf == 'd' || *genbuf == 'D') {
            flag = 2;
            if (newfh.BM[0] == '\0') {
                flag = 0;
                prints("û�а���������ɾ������!");
                pressreturn();
            }
        } else {
            clear();
            return 0;
        }

        if (flag > 0) {
            usercomplete("������" NAME_USER_SHORT " ID: ", genbuf);
            if (genbuf[0] == '\0') {
                clear();
                flag = 0;
                /*
                 * return 0;
                 */
            } else if (!(id = getuser(genbuf, &lookupuser))) {
                move(3, 0);
                prints("�Ƿ� ID");
                clrtoeol();
                pressreturn();
                clear();
                flag = 0;
                /*
                 * return 0;
                 */
            }

            if (flag > 0) {
                uinfo = *lookupuser;
                disply_userinfo(&uinfo, 1);
                brd_num = 0;

                move(18, 0);

                if (!(lookupuser->userlevel & PERM_BOARDS)) {
                    prints("�û�%s���ǰ���!\n", lookupuser->userid);
                } else {
                    prints("�û� %s Ϊ���°�İ���\n", lookupuser->userid);

                    for (n = 0; n < get_boardcount(); n++) {
                        bptr = (struct boardheader *) getboard(n + 1);
                        if (chk_BM_instr(bptr->BM, lookupuser->userid) == true) {
                            prints("%-32s%-32s\n", bptr->filename, bptr->title + 12);
                            brd_num++;
                        }
                    }

                }

                getdata(23, 0, "ȷ��������û���Y/N)?: [N]", genbuf, 2, DOECHO, NULL, true);

                if (*genbuf == 'Y' || *genbuf == 'y') {
                    newlevel = lookupuser->userlevel;


                    if (flag == 1) {
                        if (newfh.BM[0] != '\0')
                            strcat(newfh.BM, " ");
                        strcat(newfh.BM, lookupuser->userid);
                        newlevel |= PERM_BOARDS;
                        mail_file(currentuser->userid, "etc/forbm", lookupuser->userid, "����" NAME_BM "�ض�", BBSPOST_LINK, NULL);
                    } else if (flag == 2) {
                        m = 0;
                        newfh.BM[0] = '\0';

                        p = strtok(fh.BM, " ");
                        if (p) {
                            if (strcmp(p, lookupuser->userid)) {
                                strncpy(newfh.BM, p, IDLEN + 2);
                                m++;
                            } else if (brd_num == 1) {
                                newlevel &= ~PERM_BOARDS;
                                newlevel &= ~PERM_CLOAK;
                            }
                        }
                        /*
                         * ������Ӱ�����Ŀ���޸����� 
                         */
                        for (n = 1; n < (BM_LEN - 1) / (IDLEN + 2); n++) {
                            p = strtok(NULL, " ");
                            if (p == NULL)
                                break;
                            else if (strcmp(p, lookupuser->userid)) {
                                if (m > 0) {
                                    strcat(newfh.BM, " ");
                                    strcat(newfh.BM, p);
                                } else
                                    strncpy(newfh.BM, p, IDLEN + 2);
                                m++;
                            } else if (brd_num == 1) {
                                newlevel &= ~PERM_BOARDS;
                                newlevel &= ~PERM_CLOAK;
                            }
                        }



                    }

                    if (newfh.BM[0] != '\0') {
                        if (strlen(newfh.BM)<=30)
                            sprintf(vbuf, "%-38.38s(BM: %s)", newfh.title + 13, newfh.BM);
                        else
                            sprintf(vbuf, "%-28.28s(BM: %s)", newfh.title + 13, newfh.BM);
                    }
                    else
                        sprintf(vbuf, "%-38.38s", newfh.title + 13);

                    if (flag == 1)
                        sprintf(genbuf, "���� %s �İ���Ϊ %s", newfh.filename, lookupuser->userid);
                    else if (flag == 2)
                        sprintf(genbuf, "��ȥ %s �İ��� %s ", newfh.filename, lookupuser->userid);
                    securityreport(genbuf, lookupuser, NULL);
                    lookupuser->userlevel = newlevel;

                    edit_grp(fh.filename, oldtitle + 13, vbuf);
                    set_board(pos, &newfh, NULL);

                    sprintf(genbuf, "���������� %s ������ --> %s", fh.filename, newfh.filename);
                    bbslog("user", "%s", genbuf);
                    strncpy(fh.BM, newfh.BM, BM_LEN - 1);
                }

            }
        }
    }
}
