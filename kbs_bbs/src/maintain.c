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

char            cexplain[STRLEN];
char           *Ctime();
char            lookgrp[30];
char        fdata[ 7 ][ STRLEN ];

int showperminfo(int, int);

/* modified by wwj, 2001/5/7, for new md5 passwd */
void igenpass(const char *passwd,const char *userid,unsigned char md5passwd[]);

int check_systempasswd()
{
    FILE           *pass;
    char            passbuf[40], prepass[STRLEN];

    clear();
    if ((pass = fopen("etc/systempassword", "rb")) != NULL)
    {
        fgets(prepass, STRLEN, pass);
        prepass[strlen(prepass) - 1] = '\0';
        if(!strcmp(prepass,"md5")){
            fread(&prepass[16],1,16,pass);
        }
        fclose(pass);
        
        getdata(1, 0, "������ϵͳ����: ", passbuf, 39, NOECHO, NULL, YEA);
        if (passbuf[0] == '\0' || passbuf[0] == '\n')
            return NA;
            
        
        if(!strcmp(prepass,"md5")){
            igenpass(passbuf,"[system]",(unsigned char *)prepass);
            passbuf[0]=(char)!memcmp(prepass,&prepass[16],16);
        } else {
            passbuf[0]=(char)checkpasswd(prepass, passbuf);
        }
        if (!passbuf[0]) {
            move(2, 0);
            prints( MSG_ERR_USERID );
            securityreport("ϵͳ�����������...",NULL);
            pressanykey();
            return NA;
        }
    }
    return YEA;
}

int setsystempasswd()
{
    FILE           *pass;
    char            passbuf[40], prepass[40];

    modify_user_mode(ADMIN);
    if (strcmp(currentuser->userid, "SYSOP"))
        return;
    if (!check_systempasswd())
        return;
    getdata(2, 0, "�������µ�ϵͳ����: ", passbuf, 39, NOECHO, NULL, YEA);
    getdata(3, 0, "ȷ���µ�ϵͳ����: ", prepass, 39, NOECHO, NULL, YEA);
    if (strcmp(passbuf, prepass))
        return;
    if ((pass = fopen("etc/systempassword", "w")) == NULL)
    {
        move(4, 0);
        prints("ϵͳ�����޷��趨....");
        pressanykey();
        return;
    }
    fwrite("md5\n",4,1,pass);
    
    igenpass(passbuf,"[system]",(unsigned char *)prepass);
    fwrite(prepass,16,1,pass);
    
    fclose(pass);
    move(4, 0);
    prints("ϵͳ�����趨���....");
    pressanykey();
    return;
}



int securityreport(char *str,struct userec* lookupuser)		/* Leeward: 1997.12.02 */
{
    FILE           *se;
    char            fname[STRLEN];
    int             savemode;
    char           *ptr;

    savemode = uinfo.mode;
    sprintf(fname, "tmp/security.%d", getpid());
    if ((se = fopen(fname, "w")) != NULL)
    {
    	if (lookupuser) {
	        if (strstr(str, "��") && strstr(str, "ͨ�����ȷ��"))
	        {
	            fprintf(se, "ϵͳ��ȫ��¼ϵͳ\n[32mԭ��%s[m\n", str);
	            fprintf(se, "������ͨ���߸�������");
	            /*    getuinfo(se, lookupuser);*/
	            /*Haohmaru.99.4.15.�ѱ�ע��������еø���ϸ,ͬʱȥ��ע���ߵ�����*/
	            fprintf(se,"\n\n���Ĵ���     : %s\n",fdata[1]);
	            fprintf(se,"�����ǳ�     : %s\n",lookupuser->username);
	            fprintf(se,"��ʵ����     : %s\n",fdata[2]);
	            fprintf(se,"�����ʼ����� : %s\n",lookupuser->email);
	            fprintf(se,"��ʵ E-mail  : %s$%s@%s\n",fdata[3],fdata[5],currentuser->userid);
	            fprintf(se,"����λ     : %s\n",fdata[3]);
	            fprintf(se,"Ŀǰסַ     : %s\n",fdata[4]);
	            fprintf(se,"����绰     : %s\n",fdata[5]);
	            fprintf(se,"ע������     : %s", ctime( &lookupuser->firstlogin));
	            fprintf(se,"����������� : %s", ctime( &lookupuser->lastlogin));
	            fprintf(se,"������ٻ��� : %s\n", lookupuser->lasthost );
	            fprintf(se,"��վ����     : %d ��\n", lookupuser->numlogins);
	            fprintf(se,"������Ŀ     : %d / %d (Board/1Discuss)\n",lookupuser->numposts, post_in_tin( lookupuser->userid ));
	            fprintf(se,"��    ��     : %s\n",fdata[6]);
	            /*    fprintf(se, "\n[33m��������֤�߸�������[35m");
	                getuinfo(se, currentuser);rem by Haohmaru.99.4.16*/
	            fclose(se);
	            postfile(fname, "Registry", str, 2);
	        }
	        else if (strstr(str, "ɾ��ʹ���ߣ�"))
	        {
	            fprintf(se, "ϵͳ��ȫ��¼ϵͳ\n[32mԭ��%s[m\n", str);
	            fprintf(se, "�����Ǳ�ɾ�߸�������");
	            getuinfo(se, lookupuser);
	            fprintf(se, "\n������ɾ���߸�������");
	            getuinfo(se, currentuser);
	            fclose(se);
	            postfile(fname, "syssecurity", str, 2);
	        }
	        else if (ptr = strstr(str, "��Ȩ��XPERM"))
	        {
	            int             oldXPERM, newXPERM;
	            int             num;
#define XPERMSTR "bTCPRp#@XWBA$VS!DEM1234567890%"
	            char            XPERM[48];

	            sscanf(ptr + strlen("��Ȩ��XPERM"), "%d %d",
	                   &oldXPERM, &newXPERM);
	            *(ptr + strlen("��Ȩ��")) = 0;

	            fprintf(se, "ϵͳ��ȫ��¼ϵͳ\n[32mԭ��%s[m\n", str);

	            strcpy(XPERM, XPERMSTR);
	            for (num = 0; num < (int)strlen(XPERM); num++)
	                if (!(oldXPERM & (1 << num)))
	                    XPERM[num] = ' ';
	            XPERM[num] = '\0';
	            fprintf(se, "�����Ǳ�����ԭ����Ȩ��\n\033[1m\033[33m%s", XPERM);

	            strcpy(XPERM, XPERMSTR);
	            for (num = 0; num < (int)strlen(XPERM); num++)
	                if (!(newXPERM & (1 << num)))
	                    XPERM[num] = ' ';
	            XPERM[num] = '\0';
	            fprintf(se, "\n%s\033[0m\n�����Ǳ��������ڵ�Ȩ��\n", XPERM);

	            fprintf(se, "\n"
	                    "\033[1m\033[33mb\033[0m����Ȩ�� \033[1m\033[33mT\033[0m�������� \033[1m\033[33mC\033[0m�������� \033[1m\033[33mP\033[0m������ \033[1m\033[33mR\033[0m������ȷ \033[1m\033[33mp\033[0m�������� \033[1m\033[33m#\033[0m������ \033[1m\033[33m@\033[0m�ɼ�����\n"
	                    "\033[1m\033[33mX\033[0m�����ʺ� \033[1m\033[33mW\033[0m�༭ϵͳ���� \033[1m\033[33mB\033[0m���� \033[1m\033[33mA\033[0m�ʺŹ��� \033[1m\033[33m$\033[0m������ \033[1m\033[33mV\033[0mͶƱ���� \033[1m\033[33mS\033[0mϵͳά��\n"
	                    "\033[1m\033[33m!\033[0mRead/Post���� \033[1m\033[33mD\033[0m�������ܹ� \033[1m\033[33mE\033[0m�������ܹ� \033[1m\033[33mM\033[0m������ܹ� \033[1m\033[33m1\033[0m����ZAP \033[1m\033[33m2\033[0m������OP\n"
	                    "\033[1m\033[33m3\033[0mϵͳ�ܹ���Ա \033[1m\033[33m4\033[0m�����ʺ� \033[1m\033[33m5->9\033[0m ����Ȩ��5->9 \033[1m\033[33m0\033[0m��ϵͳ���۰� \033[1m\033[33m%%\033[0m���Mail"
	                    "\n");

	            fprintf(se, "\n�����Ǳ����߸�������");
	            getuinfo(se, lookupuser);
	            fprintf(se, "\n�������޸��߸�������");
	            getuinfo(se, currentuser);
	            fclose(se);
	            postfile(fname, "syssecurity", str, 2);
	        }
                else    /* Modified for change id by Bigman 2001.5.25 */                
                {                                                                       
                      fprintf(se, "ϵͳ��ȫ��¼ϵͳ\n^[[32mԭ��%s^[[m\n", str);     
                      fprintf(se, "�����Ǹ�������");                                  
                      getuinfo(se, currentuser);                                      
                      fclose(se);                                                     
                      postfile(fname, "syssecurity", str, 2);                         
                }                                                                       
    	}
        else
        {
            fprintf(se, "ϵͳ��ȫ��¼ϵͳ\n[32mԭ��%s[m\n", str);
            fprintf(se, "�����Ǹ�������");
            getuinfo(se, currentuser);
            fclose(se);
            if (strstr(str, "�趨ʹ����ע������"))	/* Leeward 98.03.29 */
                postfile(fname, "Registry", str, 2);
#ifndef LEEWARD_X_FILTER
            else if (strstr(str, "��") && strstr(str, "�����±�����"))
                postfile(fname, "Filter", str, 2);	/* Leeward 98.04.01 */
#endif
            else
                postfile(fname, "syssecurity", str, 2);
        }
        unlink(fname);
        modify_user_mode(savemode);
    }
}

int get_grp(seekstr)
char            seekstr[STRLEN];
{
    FILE           *fp;
    char            buf[STRLEN];
    char           *namep;

    if ((fp = fopen("0Announce/.Search", "r")) == NULL)
        return 0;
    while (fgets(buf, STRLEN, fp) != NULL)
    {
        namep = strtok(buf, ": \n\r\t");
        if (namep != NULL && strcasecmp(namep, seekstr) == 0)
        {
            fclose(fp);
            strtok(NULL, "/");
            namep = strtok(NULL, "/");
            strcpy(lookgrp, namep);
            return 1;
        }
    }
    fclose(fp);
    return 0;
}

void stand_title(title)
char           *title;
{
    clear();
    standout();
    prints(title);
    standend();
}

int m_info()
{
    struct userec   uinfo;
    int             id;
    struct userec* lookupuser;


    modify_user_mode(ADMIN);
    if (!check_systempasswd())	/* Haohmaru.98.12.19 */
    {
        return -1;
    }
    clear();
    stand_title("�޸�ʹ���ߴ���");
    move(1, 0);
    usercomplete("������ʹ���ߴ���: ", genbuf);
    if (*genbuf == '\0')
    {
        clear();
        return -1;
    }
    if (!(id = getuser(genbuf,&lookupuser)))
    {
        move(3, 0);
        prints( MSG_ERR_USERID );
        clrtoeol();
        pressreturn();
        clear();
        return -1;
    }
    uinfo=*lookupuser;

    move(1, 0);
    clrtobot();
    disply_userinfo(&uinfo, 1);
    uinfo_query(&uinfo, 1, id);
    return 0;
}

extern int      cmpbnames();

int valid_brdname(brd)
char           *brd;
{
    char            ch;

    ch = *brd++;
    if (!isalnum(ch) && ch != '_')
        return 0;
    while ((ch = *brd++) != '\0')
    {
        if (!isalnum(ch) && ch != '_' && ch != '.')
            return 0;
    }
    return 1;
}

char           *
chgrp()
{
    int             i, ch;
    char            buf[STRLEN], ans[6];

    /*
static char    *explain[] = {
    "��վϵͳ",
    "��������",
    "���Լ���",
    "ѧ����ѧ",
    "��������",
    "̸��˵��",
    "У԰��Ϣ",
    "�����Ļ�",
    "�������",
    "������Ϣ",
    "�廪��ѧ",
    "�ֵ�ԺУ",
    "��  ��",
    NULL
};

static char    *groups[] = {
    "system.faq",
    "rec.faq",
    "comp.faq",
    "sci.faq",
    "sport.faq",
    "talk.faq",
    "campus.faq",
    "literal.faq",
    "soc.faq",
    "network.faq",
    "thu.faq",
    "univ.faq",
    "other.faq",
    NULL
};
*/

    clear();
    move(2, 0);
    prints("ѡ�񾫻�����Ŀ¼\n");
    oflush();

    for (i = 0;; i++)
    {
        if (explain[i] == NULL || groups[i] == NULL)
            break;
        prints("[32m%2d[m. %-20s%-20s\n", i, explain[i], groups[i]);
    }
    sprintf(buf, "���������ѡ��(0~%d): ", i-1);
    while (1)
    {
        getdata(i + 3, 0, buf, ans, 4, DOECHO, NULL, YEA);
        if (!isdigit(ans[0]))
            continue;
        ch = atoi(ans);
        if (ch < 0 || ch >= i || ans[0] == '\r' || ans[0] == '\0')
            continue;
        else
            break;
    }
    sprintf(cexplain, "%s", explain[ch]);

    return groups[ch];
}


int m_newbrd()
{
    struct boardheader newboard;
    char            ans[5];
    char            vbuf[100];
    char           *group;
    int             bid;


    modify_user_mode(ADMIN);
    if (!check_systempasswd())
    {
        return -1;
    }
    clear();
    memset(&newboard, 0, sizeof(newboard));
    prints("������������:");
    while (1)
    {
        getdata(3, 0, "����������:   ", newboard.filename, 18, DOECHO, NULL, YEA);
        if (newboard.filename[0] == '\0')
            return -1;
        if (valid_brdname(newboard.filename))
            break;
        prints("���Ϸ�����...");
    }
    getdata(4, 0, "������˵��:   ", newboard.title, 60, DOECHO, NULL, YEA);
    strcpy(vbuf, "vote/");
    strcat(vbuf, newboard.filename);
    setbpath(genbuf, newboard.filename);
    if (getbnum(newboard.filename) > 0 || mkdir(genbuf, 0755) == -1
            || mkdir(vbuf, 0755) == -1)
    {
        prints("\n���󣺴��������������\n");
        pressreturn();
        clear();
        return -1;
    }
    newboard.flag = 0;
    getdata(5, 0, "����������Ա: ", newboard.BM, BM_LEN - 1, DOECHO, NULL, YEA);
    getdata(6, 0, "�Ƿ����ƴ�ȡȨ�� (Y/N)? [N]: ", ans, 4, DOECHO, NULL, YEA);
    if (*ans == 'y' || *ans == 'Y')
    {
        getdata(6, 0, "���� Read/Post? [R]: ", ans, 4, DOECHO, NULL, YEA);
        if (*ans == 'P' || *ans == 'p')
            newboard.level = PERM_POSTMASK;
        else
            newboard.level = 0;
        move(1, 0);
        clrtobot();
        move(2, 0);
        prints("�趨 %s Ȩ��. ������: '%s'\n",
               (newboard.level & PERM_POSTMASK ? "POST" : "READ"),
               newboard.filename);
        newboard.level = setperms(newboard.level, "Ȩ��", NUMPERMS, showperminfo);
        clear();
    }
    else
        newboard.level = 0;
    if (add_board(&newboard)==-1) {
    	move(t_lines-1,0);
    	outs("����������ʧ��!\n");
        pressreturn();
        clear();
        return -1;
    }
    reload_boards();
    getdata(7, 0, "�Ƿ���������� (Y/N)? [N]: ", ans, 4, DOECHO, NULL, YEA);
    if (ans[0] == 'Y' || ans[0] == 'y')
        addtofile("etc/anonymous", newboard.filename);
    group = chgrp();
    if (group != NULL)
    {
        if (newboard.BM[0] != '\0')
            sprintf(vbuf, "%-38.38s(BM: %s)", newboard.title + 13, newboard.BM);
        else
            sprintf(vbuf, "%-38.38s", newboard.title + 13);

        if (add_grp(group, cexplain, newboard.filename, vbuf) == -1)
            prints("\n����������ʧ��....\n");
        else
            prints("�Ѿ����뾫����...\n");
    }
    prints("\n������������\n");
    sprintf(genbuf, "add brd %s", newboard.filename);
    report(genbuf);
    {
        char            secu[STRLEN];
        sprintf(secu, "�����°壺%s", newboard.filename);
        securityreport(secu,NULL);
    }
    pressreturn();
    clear();
    return 0;
}

int m_editbrd()
{
    char   bname[STRLEN], buf[STRLEN], oldtitle[STRLEN], vbuf[256], *group;
    char   oldpath[STRLEN], newpath[STRLEN], tmp_grp[30];
    int    pos, noidboard, a_mv;
    struct boardheader fh, newfh;

    modify_user_mode(ADMIN);
    if (!check_systempasswd())
    {
        return -1;
    }
    clear();
    stand_title("�޸���������Ѷ");
    move(1, 0);
    make_blist();
    namecomplete("��������������: ", bname);
    if (*bname == '\0')
    {
        move(2, 0);
        prints("���������������");
        pressreturn();
        clear();
        return -1;
    }
    pos = getboardnum(bname,&fh);
    if (!pos)
    {
        move(2, 0);
        prints("���������������");
        pressreturn();
        clear();
        return -1;
    }
    noidboard = seek_in_file("etc/anonymous", bname);
    move(3, 0);
    memcpy(&newfh, &fh, sizeof(newfh));
    prints("����������:   %s\n", fh.filename);
    prints("������˵��:   %s\n", fh.title);
    prints("����������Ա: %s\n", fh.BM);
    prints("����������:   %s\n", (noidboard) ? "Yes" : "No");
    strcpy(oldtitle, fh.title);
    prints("���� %s Ȩ��: %s", (fh.level & PERM_POSTMASK) ? "POST" : "READ",
           (fh.level & ~PERM_POSTMASK) == 0 ? "������" : "������");
    getdata(7, 0, "�Ƿ����������Ѷ? (Yes or No) [N]: ", genbuf, 4, DOECHO, NULL, YEA);
    if (*genbuf == 'y' || *genbuf == 'Y')
    {
        move(8, 0);
        prints("ֱ�Ӱ� <Return> ���޸Ĵ�����Ѷ\n");
enterbname:
        getdata(9, 0, "������������: ", genbuf, 18, DOECHO, NULL, YEA);
        if (*genbuf != 0)
        {
            if (getboardnum(genbuf,NULL)>0)
            {
                move(3, 0);
                prints("����! ���������Ѿ�����\n");
                move(10, 0);
                clrtobot();
                goto enterbname;
            }
            strncpy(newfh.filename, genbuf, sizeof(newfh.filename));
            strcpy(bname, genbuf);
        }
        getdata(10, 0, "��������˵��: ", genbuf, 60, DOECHO, NULL, YEA);
        if (*genbuf != 0)
            strncpy(newfh.title, genbuf, sizeof(newfh.title));
        getdata(11, 0, "����������Ա: ", genbuf, 60, DOECHO, NULL, YEA);
        if (*genbuf != 0)
            strncpy(newfh.BM, genbuf, sizeof(newfh.BM));
        if (*genbuf == ' ')
            strncpy(newfh.BM, "\0", sizeof(newfh.BM));
        /* newfh.BM[ BM_LEN - 1 ]=fh.BM[ BM_LEN - 1 ]; */
        sprintf(buf, "������ (Y/N)? [%c]: ", (noidboard) ? 'Y' : 'N');
        getdata(12, 0, buf, genbuf, 4, DOECHO, NULL, YEA);
        if (*genbuf == 'y' || *genbuf == 'Y' || *genbuf == 'N' || *genbuf == 'n')
        {
            if (*genbuf == 'y' || *genbuf == 'Y')
                noidboard = 1;
            else
                noidboard = 0;
        }
        getdata(13, 0, "�Ƿ��ƶ���������λ�� (Y/N)? [N]: ", genbuf, 4, DOECHO, NULL, YEA);
        if (*genbuf == 'Y' || *genbuf == 'y')
            a_mv = 2;
        else
            a_mv = 0;
        getdata(14, 0, "�Ƿ���Ĵ�ȡȨ�� (Y/N)? [N]: ", genbuf, 4, DOECHO, NULL, YEA);
        if (*genbuf == 'Y' || *genbuf == 'y')
        {
            char            ans[5];
            sprintf(genbuf, "���� (R)�Ķ� �� (P)���� ���� [%c]: ",
                    (newfh.level & PERM_POSTMASK ? 'P' : 'R'));
            getdata(14, 0, genbuf, ans, 4, DOECHO, NULL, YEA);
            if ((newfh.level & PERM_POSTMASK) && (*ans == 'R' || *ans == 'r'))
                newfh.level &= ~PERM_POSTMASK;
            else
                if (!(newfh.level & PERM_POSTMASK) && (*ans == 'P' || *ans == 'p'))
                    newfh.level |= PERM_POSTMASK;
            move(1, 0);
            clrtobot();
            move(2, 0);
            prints("�趨 %s '%s' ��������Ȩ��\n",
                   newfh.level & PERM_POSTMASK ? "����" : "�Ķ�", newfh.filename);
            newfh.level = setperms(newfh.level, "Ȩ��", NUMPERMS, showperminfo);
            clear();
            getdata(0, 0, "ȷ��Ҫ������? (Y/N) [N]: ", genbuf, 4, DOECHO, NULL, YEA);
        } else
        {
            getdata(15, 0, "ȷ��Ҫ������? (Y/N) [N]: ", genbuf, 4, DOECHO, NULL, YEA);
        }
        if (*genbuf == 'Y' || *genbuf == 'y')
        {
            {
                char            secu[STRLEN];
                sprintf(secu, "�޸���������%s(%s)", fh.filename, newfh.filename);
                securityreport(secu,NULL);
            }
            if (strcmp(fh.filename, newfh.filename))
            {
                char            old[256], tar[256];
                a_mv = 1;
                setbpath(old, fh.filename);
                setbpath(tar, newfh.filename);
                Rename(old, tar);
                sprintf(old, "vote/%s", fh.filename);
                sprintf(tar, "vote/%s", newfh.filename);
                Rename(old, tar);
            }
            if (newfh.BM[0] != '\0')
                sprintf(vbuf, "%-38.38s(BM: %s)", newfh.title + 13, newfh.BM);
            else
                sprintf(vbuf, "%-38.38s", newfh.title + 13);
            get_grp(fh.filename);
            edit_grp(fh.filename, lookgrp, oldtitle + 13, vbuf);
            if (a_mv >= 1)
            {
                group = chgrp();
                get_grp(fh.filename);
                strcpy(tmp_grp, lookgrp);
                if (strcmp(tmp_grp, group) || a_mv != 2)
                {
                    del_from_file("0Announce/.Search", fh.filename);
                    if (group != NULL)
                    {
                        if (newfh.BM[0] != '\0')
                            sprintf(vbuf, "%-38.38s(BM: %s)", newfh.title + 13, newfh.BM);
                        else
                            sprintf(vbuf, "%-38.38s", newfh.title + 13);

                        if (add_grp(group, cexplain, newfh.filename, vbuf) == -1)
                            prints("\n����������ʧ��....\n");
                        else
                            prints("�Ѿ����뾫����...\n");
                        sprintf(newpath, "0Announce/groups/%s/%s", group, newfh.filename);
                        sprintf(oldpath, "0Announce/groups/%s/%s", tmp_grp, fh.filename);
                        if (dashd(oldpath))
                        {
				/*
                            sprintf(genbuf, "/bin/rm -fr %s", newpath);
			    */
                            f_rm(newpath);
                        }
                        Rename(oldpath, newpath);
                        del_grp(tmp_grp, fh.filename, fh.title + 13);
                    }
                }
            }
            substitute_record(BOARDS, &newfh, sizeof(newfh), pos);
            reload_boards();
            if (noidboard == 1 && !seek_in_file("etc/anonymous", newfh.filename))
                addtofile("etc/anonymous", newfh.filename);
            else
                if (noidboard == 0)
                    del_from_file("etc/anonymous", newfh.filename);
            sprintf(genbuf, "���������� %s ������ --> %s",
                    fh.filename, newfh.filename);
            report(genbuf);
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

    modify_user_mode(ADMIN);
    clear();
    stand_title("��ѯʹ���߷��ļ�¼");
    move(1,0);
    usercomplete("������ʹ�����ʺ�:", genbuf);
    strcpy(tmp_id, genbuf);
    if(  tmp_id[0] == '\0') {
        clear();
        return -1;
    }

    if(!(id = getuser(genbuf,NULL))) {
        move(3,0) ;
        prints("����ȷ��ʹ���ߴ���\n") ;
        clrtoeol();
        pressreturn();
        clear();
        return -1;
    }

    sprintf(tmp_command, "grep -a -w %s user.log | grep posted > tmp/searchresult.%d", tmp_id,getpid());
    system(tmp_command);
    sprintf(tmp_command,"tmp/searchresult.%d",getpid());
    mail_file(currentuser->userid,tmp_command, currentuser->userid, "ϵͳ��ѯ���",1);

    sprintf(buf, "��ѯ�û� %s �ķ������", tmp_id);
    securityreport(buf,NULL);  /*д��syssecurity��, stephen 2000.12.21*/
    sprintf(buf, "Search the posts by %s in the trace", tmp_id);
    report(buf);   /*д��trace, stephen 2000.12.21*/

    move(3,0);
    prints("��ѯ����Ѿ��ĵ��������䣡 \n");
    pressreturn();
    clear();
    return 0;
}           /* stephen 2000.12.15 let sysop search in trace */


int post_in_tin(username)
char           *username;
{
    char            buf[256];
    FILE           *fh;
    int             counter = 0;

    sethomefile(buf, username, ".tin/posted");
    fh = fopen(buf, "r");
    if (fh == NULL)
        return 0;
    else
    {
        while (fgets(buf, 255, fh) != NULL)
        {
            if (buf[9] != 'd' && strncmp(&buf[11], "ccu.cs.test", 11)
                    && strncmp(&buf[11], "tw.bbs.test", 11))
                counter++;
            if (buf[9] == 'd')
                counter--;
        }
        fclose(fh);
        return counter;
    }

}

char            curruser[IDLEN + 2];
extern int      delmsgs[];
extern int      delcnt;

void domailclean(struct fileheader *fhdrp,char* arg)
{
    static int      newcnt, savecnt, deleted, idc;
    char            buf[STRLEN];

    if (fhdrp == NULL)
    {
        bbslog("clean", "new = %d, saved = %d, deleted = %d", newcnt, savecnt, deleted);
        newcnt = savecnt = deleted = idc = 0;
        if (delcnt)
        {
            setmailfile(buf, curruser, DOT_DIR);
            while (delcnt--)
                delete_record(buf, sizeof(struct fileheader), delmsgs[delcnt]);
        }
        delcnt = 0;
        return;
    }
    idc++;
    if (!(fhdrp->accessed[0] & FILE_READ))
        newcnt++;
    else
        if (fhdrp->accessed[0] & FILE_MARKED)
            savecnt++;
        else
        {
            deleted++;
            setmailfile(buf, curruser, fhdrp->filename);
            unlink(buf);
            delmsgs[delcnt++] = idc;
        }
}

int cleanmail(struct userec  *urec,char* arg)
{
    struct stat     statb;
    if (urec->userid[0] == '\0' || !strcmp(urec->userid, "new"))
        return 0;
    setmailfile(genbuf, urec->userid, DOT_DIR);
    if (stat(genbuf, &statb) == -1) {
        bbslog("clean","%s no mail",urec->userid);
    } else {
        if (statb.st_size == 0) {
            bbslog("clean","%s no mail",urec->userid);
        } else {
            strcpy(curruser, urec->userid);
            delcnt = 0;
            apply_record(genbuf, domailclean, sizeof(struct fileheader),0);
            domailclean(NULL,0);
        }
    }
    return 0;
}

int m_mclean()
{
    char            ans[5];

    modify_user_mode(ADMIN);
    if (!check_systempasswd())
    {
        return -1;
    }
    clear();
    stand_title("���˽���ż�");
    move(1, 0);
    prints("��������Ѷ���δ mark ���ż�\n");
    getdata(2, 0, "ȷ���� (Y/N)? [N]: ", ans, 3, DOECHO, NULL, YEA);
    if (ans[0] != 'Y' && ans[0] != 'y')
    {
        clear();
        return 0;
    }
    {
        char            secu[STRLEN];
        sprintf(secu, "�������ʹ�����Ѷ��ż���");
        securityreport(secu,NULL);
    }

    move(3, 0);
    prints("�����ĵȺ�.\n");
    refresh();
    apply_users(cleanmail,0);
    move(4, 0);
    prints("������! ��鿴��־�ļ�.\n");
    report("Mail Clean");
    pressreturn();
    clear();
    return 0;
}

void trace_state(flag, name, size)
int             flag, size;
char           *name;
{
    char            buf[STRLEN];

    if (flag != -1)
    {
        sprintf(buf, "ON (size = %d)", size);
    } else
    {
        strcpy(buf, "OFF");
    }
    prints("%s��¼ %s\n", name, buf);
}

int touchfile(filename)
char           *filename;
{
    int             fd;

    if ((fd = open(filename, O_RDWR | O_CREAT, 0600)) > 0)
    {
        close(fd);
    }
    return fd;
}

int m_trace()
{
    struct stat     ostatb, cstatb;
    int             otflag, ctflag, done = 0;
    char            ans[3];
    char           *msg;

    modify_user_mode(ADMIN);
    if (!check_systempasswd())
    {
        return -1;
    }
    clear();
    stand_title("Set Trace Options");
    while (!done)
    {
        move(2, 0);
        otflag = stat("trace", &ostatb);
        ctflag = stat("trace.chatd", &cstatb);
        prints("Ŀǰ�趨:\n");
        trace_state(otflag, "һ��", ostatb.st_size);
        trace_state(ctflag, "����", cstatb.st_size);
        move(9, 0);
        prints("<1> �л�һ���¼\n");
        prints("<2> �л������¼\n");
        getdata(12, 0, "��ѡ�� (1/2/Exit) [E]: ", ans, 2, DOECHO, NULL, YEA);

        switch (ans[0])
        {
        case '1':
            if (otflag)
            {
                touchfile("trace");
                msg = "һ���¼ ON";
            } else
            {
                Rename("trace", "trace.old");
                msg = "һ���¼ OFF";
            }
            break;
        case '2':
            if (ctflag)
            {
                touchfile("trace.chatd");
                msg = "�����¼ ON";
            } else
            {
                Rename("trace.chatd", "trace.chatd.old");
                msg = "�����¼ OFF";
            }
            break;
        default:
            msg = NULL;
            done = 1;
        }
        move(t_lines - 2, 0);
        if (msg)
        {
            prints("%s\n", msg);
            report(msg);
        }
    }
    clear();
    return 0;
}
int
valid_userid(ident ) /* check the user has registed, added by dong, 1999.4.18 */
char    *ident;
{
    if( strchr( ident, '@' ) && valid_ident( ident ) )
        return 1;
    return 0;
}
int check_proxy_IP(ip, buf) /* added for rejection of register from proxy,
			Bigman, 2001.11.9 */
	/* ��bbsd_single�����local_check_ban_IP����һ�������Կ��ǹ���*/
char *ip;
char *buf;
{ /* Leeward 98.07.31
      RETURN:
                     - 1: No any banned IP is defined now
                       0: The checked IP is not banned
      other value over 0: The checked IP is banned, the reason is put in buf
      */
    FILE *Ban = fopen("etc/proxyIP", "r");
    char IPBan[64];
    int  IPX = - 1;
    char *ptr;

    if (!Ban)
        return IPX;
    else
        IPX ++;

    while (fgets(IPBan, 64, Ban))
       {
        if (ptr = strchr(IPBan, '\n'))
            *ptr = 0;
        if (ptr = strchr(IPBan, ' '))
        {
            *ptr ++ = 0;
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

static const char    *field[] = {"usernum", "userid", "realname", "career",
                           "addr", "phone", "birth", NULL};
static const char    *reason[] = {
    "��������ʵ����(�������ƴ��).", "������ѧУ��ϵ������λ.",
    "����д������סַ����.", "����������绰(���޿��ú�����Email��ַ����).",
    "��ȷʵ����ϸ����дע�������.", "����������д���뵥.",
    "������Ӵ���ע��", "ͬһ���û�ע���˹���ID",
    NULL};

int scan_register_form(logfile, regfile)
char           *logfile, *regfile;
{
    static const char    *finfo[] = {"�ʺ�λ��", "�������", "��ʵ����", "����λ",
                           "Ŀǰסַ", "����绰", "��    ��", NULL};
    struct userec   uinfo;
    FILE           *fn, *fout, *freg;
    /*   char            fdata[7][STRLEN];Haohmaru.99.4.15.��ȫ�ֱ���*/
    char            fname[STRLEN], buf[STRLEN], buff;
    /* ^^^^^ Added by Marco */
    char            ans[5], *ptr, *uid;
    int             n, unum;
    int		    count,sum;/*Haohmaru.2000.3.9.���㻹�ж��ٵ���û����*/

    uid = currentuser->userid;
    stand_title("�����趨������ע������");
    sprintf(fname, "%s.tmp", regfile);
    move(2, 0);
    if (dashf(fname))
    {
        prints("[1m���� SYSOP ����ʹ�� telnet �� WWW �鿴ע�����뵥������ʹ����״̬��\n\n");
        prints("[33m���û������ SYSOP ���ڲ鿴ע�����뵥���������ڶ�����ɵ��޷�ע�ᡣ\n");
        prints("��� bbsroot �ʻ�����һ���������\n");
        prints("                                   [32mcat new_register.tmp >> new_register[33m\n");
        prints("ȷ�������������гɹ���������һ���������\n");
        prints("                                            [32mrm new_register.tmp\n[0m");
        pressreturn();
        return -1;
    }
    Rename(regfile, fname);
    if ((fn = fopen(fname, "r")) == NULL)
    {
        move(2, 0);
        prints("ϵͳ����, �޷���ȡע�����ϵ�: %s\n", fname);
        pressreturn();
        return -1;
    }
    memset(fdata, 0, sizeof(fdata));
    /*Haohmaru.2000.3.9.���㹲�ж��ٵ���*/
    sum=0;
    while (fgets(genbuf, STRLEN, fn) != NULL)
    {
        if ((ptr = (char *) strstr(genbuf, "userid")) != NULL)
            sum++;
    }
    fseek(fn, 0, SEEK_SET);
    count=1;
    while (fgets(genbuf, STRLEN, fn) != NULL)
    {
    	struct userec* lookupuser;
        if ((ptr = (char *) strstr(genbuf, ": ")) != NULL)
        {
            *ptr = '\0';
            for (n = 0; field[n] != NULL; n++)
            {
                if (strcmp(genbuf, field[n]) == 0)
                {
                    strcpy(fdata[n], ptr + 2);
                    if ((ptr = (char *) strchr(fdata[n], '\n')) != NULL)
                        *ptr = '\0';
                }
            }
        }
        else if ((unum = getuser(fdata[1],&lookupuser)) == 0)
        {
            move(2, 0);
            clrtobot();
            prints("ϵͳ����, ���޴��ʺ�.\n\n");
            for (n = 0; field[n] != NULL; n++)
                prints("%s     : %s\n", finfo[n], fdata[n]);
            pressreturn();
            memset(fdata, 0, sizeof(fdata));
        }
        else
        {
            uinfo=*lookupuser;
            move(1, 0);
            prints("�ʺ�λ��     : %d   ���� %d ��ע�ᵥ����ǰΪ�� %d �ţ���ʣ %d ��\n", unum,sum,count++,sum-count+1);/*Haohmaru.2000.3.9.���㻹�ж��ٵ���û����*/
            disply_userinfo(&uinfo, 1);
            move(15, 0);
            printdash(NULL);
            for (n = 0; field[n] != NULL; n++)
		/* added for rejection of register from proxy */
		/* Bigman, 2001.11.9 */
		if (n==1)	{
			if (check_proxy_IP(uinfo.lasthost,buf) > 0)
				prints("%s     : %s \033[33m%s\033[0m\n", finfo[n], fdata[n], buf);
			else prints("%s     : %s\n", finfo[n], fdata[n]);
		} else prints("%s     : %s\n", finfo[n], fdata[n]);
            /* if (uinfo.userlevel & PERM_LOGINOK) modified by dong, 1999.4.18*/
            if ((uinfo.userlevel & PERM_LOGINOK) || valid_userid(uinfo.realemail))
            {
                move(t_lines - 1, 0);
                prints("���ʺŲ�������дע�ᵥ.\n");
                igetkey();
                ans[0] = 'D';
            } else
            {
                getdata(t_lines - 1, 0, "�Ƿ���ܴ����� (Y/N/Q/Del/Skip)? [S]: ",
                        ans, 3, DOECHO, NULL, YEA);
            }
            move(2, 0);
            clrtobot();
            switch (ans[0])
            {
            case 'D':
            case 'd':
                break;
            case 'Y':
            case 'y':
                prints("����ʹ���������Ѿ�����:\n");
                n = strlen(fdata[5]);
                if (n + strlen(fdata[3]) > 60)
                {
                    if (n > 40)
                        fdata[5][n = 40] = '\0';
                    fdata[3][60 - n] = '\0';
                }
                strncpy(uinfo.realname, fdata[2], NAMELEN);
                strncpy(uinfo.address, fdata[4], NAMELEN);
                sprintf(genbuf, "%s$%s@%s", fdata[3], fdata[5], uid);
                strncpy(uinfo.realemail, genbuf, STRLEN - 16);
                sprintf(buf, "tmp/email/%s", uinfo.userid);
                if ((fout = fopen(buf, "w")) != NULL)
                {
                    fprintf(fout, "%s\n", genbuf);
                    fclose(fout);
                }

                update_user(&uinfo,unum,0);
                mail_file(currentuser->userid,"etc/s_fill", uinfo.userid, "�����㣬���Ѿ����ע�ᡣ",0);
                sprintf(genbuf, "%s �� %s ͨ�����ȷ��.", uid, uinfo.userid);
                securityreport(genbuf,lookupuser);
                if ((fout = fopen(logfile, "a")) != NULL)
                {
                	time_t now;
                    for (n = 0; field[n] != NULL; n++)
                        fprintf(fout, "%s: %s\n", field[n], fdata[n]);
                    now = time(NULL);
                    fprintf(fout, "Date: %s\n", Ctime(now));
                    fprintf(fout, "Approved: %s\n", uid);
                    fprintf(fout, "----\n");
                    fclose(fout);
                }
                /* user_display( &uinfo, 1 ); */
                /* pressreturn(); */

               /* ����ע����Ϣ��¼ 2001.11.11 Bigman */
                sethomefile( buf, uinfo.userid, "/register");
               if ((fout = fopen(buf, "w")) != NULL)
                {
			for (n = 0; field[n] != NULL; n++)
				fprintf(fout, "%s     : %s\n", finfo[n], fdata[n]);
			fprintf(fout,"�����ǳ�     : %s\n", uinfo.username);
			fprintf(fout,"�����ʼ����� : %s\n", uinfo.email);
			fprintf(fout,"��ʵ E-mail  : %s\n", uinfo.realemail );
			fprintf(fout,"Ident ����   : %s\n", uinfo.ident );
			fprintf(fout,"ע������     : %s\n", ctime( &uinfo.firstlogin));
			fprintf(fout,"ע��ʱ�Ļ��� : %s\n", uinfo.lasthost );
			fprintf(fout, "Approved: %s\n",uid);
			fclose(fout);
                }

                break;
            case 'Q':
            case 'q':
                if ((freg = fopen(regfile, "a")) != NULL)
                {
                    for (n = 0; field[n] != NULL; n++)
                        fprintf(freg, "%s: %s\n", field[n], fdata[n]);
                    fprintf(freg, "----\n");
                    while (fgets(genbuf, STRLEN, fn) != NULL)
                        fputs(genbuf, freg);
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
                getdata(10 + n, 0, "�˻�ԭ��: ", buf, STRLEN, DOECHO, NULL, YEA);
                buff = buf[0];	/* Added by Marco */
                if (buf[0] != '\0')
                {
                    if (buf[0] >= '0' && buf[0] < '0' + n)
                    {
                        strcpy(buf, reason[buf[0] - '0']);
                    }
                    sprintf(genbuf, "<ע��ʧ��> - %s", buf);
                    strncpy(uinfo.address, genbuf, NAMELEN);
                    update_user(&uinfo,unum,0);

                    /* ------------------- Added by Marco */
                    switch (buff)
                    {
                    case '0':
                        mail_file(currentuser->userid,"etc/f_fill.realname", uinfo.userid, uinfo.address,0);
                        break;
                    case '1':
                        mail_file(currentuser->userid,"etc/f_fill.unit", uinfo.userid, uinfo.address,0);
                        break;
                    case '2':
                        mail_file(currentuser->userid,"etc/f_fill.address", uinfo.userid, uinfo.address,0);
                        break;
                    case '3':
                        mail_file(currentuser->userid,"etc/f_fill.telephone", uinfo.userid, uinfo.address,0);
                        break;
                    case '4':
                        mail_file(currentuser->userid,"etc/f_fill.real", uinfo.userid, uinfo.address,0);
                        break;
                    case '5':
                        mail_file(currentuser->userid,"etc/f_fill.chinese", uinfo.userid, uinfo.address,0);
                        break;
                    case '6':
                        mail_file(currentuser->userid,"etc/f_fill.proxy", uinfo.userid, uinfo.address,0);
                        break;
                    case '7':
                        mail_file(currentuser->userid,"etc/f_fill.toomany", uinfo.userid, uinfo.address,0);
                        break;
                    default:
                        mail_file(currentuser->userid,"etc/f_fill.real", uinfo.userid,uinfo.address,0);
                        break;
                    }
                    /*
                     * -------------------------------------------------------
                     */
                    /* user_display( &uinfo, 1 ); */
                    /* pressreturn(); */
                    break;
                }
                move(10, 0);
                clrtobot();
                prints("ȡ���˻ش�ע�������.\n");
                /* run default -- put back to regfile */
            default:
                if ((freg = fopen(regfile, "a")) != NULL)
                {
                    for (n = 0; field[n] != NULL; n++)
                        fprintf(freg, "%s: %s\n", field[n], fdata[n]);
                    fprintf(freg, "----\n");
                    fclose(freg);
                }
            }
            memset(fdata, 0, sizeof(fdata));
        }
    }
    fclose(fn);
    unlink(fname);
    return (0);
}

int m_register()
{
    FILE           *fn;
    char            ans[3], *fname;
    int             x, y, wid, len;

    modify_user_mode(ADMIN);
    if (!check_systempasswd())
    {
        return -1;
    }
    clear();

    stand_title("�趨ʹ����ע������");
    move(2, 0);
    fname = "new_register";
    if ((fn = fopen(fname, "r")) == NULL)
    {
        prints("Ŀǰ������ע������.");
        pressreturn();
    } else
    {
        y = 2, x = wid = 0;
        while (fgets(genbuf, STRLEN, fn) != NULL && x < 65)
        {
            if (strncmp(genbuf, "userid: ", 8) == 0)
            {
                move(y++, x);
                prints(genbuf + 8);
                len = strlen(genbuf + 8);
                if (len > wid)
                    wid = len;
                if (y >= t_lines - 2)
                {
                    y = 2;
                    x += wid + 2;
                }
            }
        }
        fclose(fn);
        getdata(t_lines - 1, 0, "�趨������ (Y/N)? [N]: ", ans, 2, DOECHO, NULL, YEA);
        if (ans[0] == 'Y' || ans[0] == 'y')
        {
            {
                char            secu[STRLEN];
                sprintf(secu, "�趨ʹ����ע������");
                securityreport(secu,NULL);
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
    if (!HAS_PERM(currentuser,PERM_ADMIN)) return;
    getdata(t_lines - 1, 0, "��ֹ��½�� (Y/N)? [N]: ", ans, 2, DOECHO, NULL, YEA);
    if (ans[0] == 'Y' || ans[0] == 'y')
    {
        if( vedit("NOLOGIN",NA)==-1)
            unlink("NOLOGIN");
    }
}

/* czz added 2002.01.15 */
int inn_start()
{
	char ans[4], tmp_command[80];
	getdata(t_lines - 1, 0, "����ת���� (Y/N)? [N]: ", ans, 2, DOECHO, NULL, YEA);
	if (ans[0] == 'Y' || ans[0] == 'y')
	{
		sprintf(tmp_command, "~bbs/innd/innbbsd");
		system(tmp_command);
	}
}

int inn_reload()
{
	char ans[4], tmp_command[80];
	getdata(t_lines - 1, 0, "�ض������� (Y/N)? [N]: ", ans, 2, DOECHO, NULL, YEA);
	if (ans[0] == 'Y' || ans[0] == 'y')
	{
		sprintf(tmp_command, "~bbs/innd/ctlinnbbsd reload");
		system(tmp_command);
	}
}

int inn_stop()
{
	char ans[4], tmp_command[80];
	getdata(t_lines - 1, 0, "ֹͣת���� (Y/N)? [N]: ", ans, 2, DOECHO, NULL, YEA);
	if (ans[0] == 'Y' || ans[0] == 'y')
	{
		sprintf(tmp_command, "~bbs/innd/ctlinnbbsd shutdown");
		system(tmp_command);
	}
}
/* added end */
