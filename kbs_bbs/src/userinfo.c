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
*/  
/* KCN modified 2001.5.10 */ 

#include "bbs.h"
extern time_t login_start_time;
char *sysconf_str();

void disply_userinfo(u, real)
	struct userec *u;
    int real;
{
    struct stat st;
    int num, diff;
	struct userdata ud;

	read_userdata(u->userid, &ud);
    move(real >= 1 ? 2 : 3, 0);
    clrtobot();
    prints("���Ĵ���     : %s\n", u->userid);
    prints("�����ǳ�     : %s\n", u->username);
    if (real != 2)  //ע�ᵥ����ʾ��ʵ����
        prints("��ʵ����     : %s\n", ud.realname);
    prints("��ססַ     : %s\n", ud.address);
    if(real)
	prints("ע��E-mail   : %s\n",ud.reg_email);
    else
    	prints("�����ʼ����� : %s\n", ud.email);

	/*�������յ���ʾ added by binxun 2003.5.20*/
#ifdef HAVE_BIRTHDAY
    prints("�����Ա�     : %s\n",(ud.gender=='M')?"��":"Ů");
	prints("��������     : %d-%d-%d\n",ud.birthyear+1900,ud.birthmonth,ud.birthday);
#endif
#ifdef HAVE_CUSTOM_USER_TITLE
    prints("����ְ��: %s\n",get_user_title(u->title));
#endif

    if (real) {
        prints("ԭʼע������ : %s\n", ud.realemail);
    }
    prints("ע������     : %s", ctime(&u->firstlogin));
    prints("����������� : %s", ctime(&u->lastlogin));
    if (real) {
        prints("������ٻ��� : %s\n", u->lasthost);
    }

    /*---	added by period		hide posts/logins	2000-11-02	---*/
/*    if(HAS_PERM(getCurrentUser(),PERM_ADMINMENU)) { *//* removed to let user can see his own data */
        prints("��վ����     : %d ��\n", u->numlogins);
    if (real)
        prints("������Ŀ     : %d ƪ\n", u->numposts);

        /*       if( real ) {
           prints("������Ŀ     : %d / %d (Board/1Discuss)\n",
           u->numposts, post_in_tin( u->userid ));
           }  removed by stephen 2000-11-02 */
        /*    } */
#ifdef NINE_BUILD
        {
           int exp;
           exp=countexp(u);
           prints("����ֵ       : %d(%s)\n",exp,cexp(exp));
           exp=countperf(u);
           prints("����ֵ       : %d(%s)\n",exp,cperf(exp));
        }
#endif
        prints("��վ��ʱ��   : %d Сʱ %d ����\n", u->stay / 3600, (u->stay / 60) % 60);
    setmailfile(genbuf, u->userid, DOT_DIR);
    if (stat(genbuf, &st) >= 0)
        num = st.st_size / (sizeof(struct fileheader));
    
    else
        num = 0;
    prints("˽������     : %d ��\n", num);
    if (real) {
        strcpy(genbuf, "bTCPRp#@XWBA$VS!DEM1234567890%");
        for (num = 0; num < (int) strlen(genbuf); num++)
            if (!(u->userlevel & (1 << num)))
                genbuf[num] = '-';
        genbuf[num] = '\0';
        prints("ʹ����Ȩ��   : %s\n", genbuf);
    } else {
        diff = (time(0) - login_start_time) / 60;
        prints("ͣ���ڼ�     : %d Сʱ %02d ��\n", diff / 60, diff % 60);
        prints("��Ļ��С     : %dx%d\n", t_lines, t_columns);
    }
    prints("\n");
    if (u->userlevel & PERM_LOGINOK) {
        prints("  ����ע������Ѿ����, ��ӭ���뱾վ.\n");
    } else if (u->lastlogin - u->firstlogin < 3 * 86400) {
        prints("  ������·, ���Ķ� Announce ������.\n");
    } else {
        prints("  ע����δ�ɹ�, ��ο���վ��վ����˵��.\n");
    }
}
int uinfo_query(struct userec *u, int real, int unum) 
{
    struct userec newinfo;
    char ans[3], buf[STRLEN], *emailfile, genbuf[STRLEN];
    int i, fail = 0, netty_check = 0, tmpchange = 0;
    FILE * fin, *fout, *dp;
    time_t code;
	struct userdata ud;
	struct usermemo *um;
	
	time_t now;
	struct tm *tmnow;

	now = time(0);
	tmnow = localtime(&now);

    memcpy(&newinfo, u, sizeof(struct userec));
	read_userdata(u->userid, &ud);
	read_user_memo(u->userid, &um);
	//memcpy(&ud, &(getSession()->currentmemo->ud), sizeof(ud));
    getdata(t_lines - 1, 0, real ? "��ѡ�� (0)���� (1)�޸����� (2)�趨���� (3) �� ID ==> [0]" : "��ѡ�� (0)���� (1)�޸����� (2)�趨���� ==> [0]", ans, 2, DOECHO, NULL, true);
    clear();
    i = 3;
    move(i++, 0);
    if (ans[0] != '3' || real)
        prints("ʹ���ߴ���: %s\n", u->userid);
    switch (ans[0]) {
    case '1':
        move(1, 0);
        prints("�������޸�,ֱ�Ӱ� <ENTER> ����ʹ�� [] �ڵ����ϡ�\n");
        sprintf(genbuf, "�ǳ� [%s]: ", u->username);
        getdata(i++, 0, genbuf, buf, NAMELEN, DOECHO, NULL, true);
        if (buf[0])
            strncpy(newinfo.username, buf, NAMELEN);
        sprintf(genbuf, "��ʵ���� [%s]: ", ud.realname);
        getdata(i++, 0, genbuf, buf, NAMELEN, DOECHO, NULL, true);
        if (buf[0])
            strncpy(ud.realname, buf, NAMELEN);
        sprintf(genbuf, "��ס��ַ [%s]: ", ud.address);
        getdata(i++, 0, genbuf, buf, STRLEN, DOECHO, NULL, true);
        if (buf[0])
            strncpy(ud.address, buf, NAMELEN);
        sprintf(genbuf, "�������� [%s]: ", ud.email);
        getdata(i++, 0, genbuf, buf, STRLEN, DOECHO, NULL, true);
        if (buf[0])
             {

                /*netty_check = 1; */
                /* ȡ��email ��֤, alex , 97.7 */
                strncpy(ud.email, buf, STRLEN);
            }
		/* �����޸����յ� added by binxun 2003.5.20 */
#ifdef  HAVE_BIRTHDAY
        sprintf(genbuf, "�����Ա�: [1]�� [2]Ů (1 or 2)[%d]",(ud.gender=='M')?1:2);
	    do{
            getdata(i, 0, genbuf,buf, 2, DOECHO, NULL, true);
			if(buf[0]==0)break;
        }while (buf[0] < '1' || buf[0] > '2');
        i++;
		switch (buf[0])
		{
		case '1':
			ud.gender = 'M';
			break;
		case '2':
			ud.gender = 'F';
			break;
		default:
		    break;
		}

		prints("���������ĳ�������");
		i++;
        do{
			buf[0] = '\0';
			sprintf(genbuf, "��λ����Ԫ��: [%d]: ", ud.birthyear+1900);
			getdata(i, 0, genbuf, buf, 5, DOECHO, NULL, true);
			if(buf[0]=='\0')break;
			if (atoi(buf) < 1900)continue;
			ud.birthyear = atoi(buf) - 1900;
		}while (ud.birthyear < tmnow->tm_year - 98 || ud.birthyear > tmnow->tm_year - 3);
		i++;

        do{
			buf[0] = '\0';
			sprintf(genbuf, "������: (1-12) [%d]: ", ud.birthmonth);
			getdata(i, 0, genbuf, buf, 3, DOECHO, NULL, true);
			if(buf[0]=='\0')break;
			ud.birthmonth = atoi(buf);
		}while (ud.birthmonth < 1 || ud.birthmonth > 12);
		i++;

        do
		{
			buf[0] = '\0';
			sprintf(genbuf, "������: (1-31) [%d]: ", ud.birthday);
			getdata(i, 0, genbuf, buf, 3, DOECHO, NULL, true);
			if(buf[0]=='\0')break;
			ud.birthday = atoi(buf);
		}while (ud.birthday < 1 || ud.birthday > 31);
		i++;
#endif

        if (real) {
#ifdef HAVE_CUSTOM_USER_TITLE
            sprintf(genbuf, "��ǰְ��%s[%d](���������): ", get_user_title(u->title),u->title);
            getdata(i++, 0, genbuf, buf, STRLEN, DOECHO, NULL, true);
            if (buf[0])
                newinfo.title=atoi(buf);
#endif
            sprintf(genbuf, "��ʵEmail[%s]: ", ud.realemail);
            getdata(i++, 0, genbuf, buf, STRLEN, DOECHO, NULL, true);
            if (buf[0])
                strncpy(ud.realemail, buf, STRLEN - 16);
            sprintf(genbuf, "���ߴ��� [%d]: ", u->numlogins);
            getdata(i++, 0, genbuf, buf, 16, DOECHO, NULL, true);
            if (atoi(buf) > 0)
                newinfo.numlogins = atoi(buf);
            sprintf(genbuf, "������Ŀ [%d]: ", u->numposts);
            getdata(i++, 0, genbuf, buf, 16, DOECHO, NULL, true);
             {
                int lres;

                lres = atoi(buf);
                if (lres > 0 || ('\0' == buf[1] && '0' == *buf))
                    newinfo.numposts = lres;
            }
            
                /*            if( atoi( buf ) > 0 ) newinfo.numposts = atoi( buf ); */ 
                sprintf(genbuf, "��ע��������ǰ" REGISTER_WAIT_TIME_NAME " [Y/N]");
            getdata(i++, 0, genbuf, buf, 16, DOECHO, NULL, true);
            if (buf[0] == 'y' || buf[0] == 'Y')
                newinfo.firstlogin -= REGISTER_WAIT_TIME;
            sprintf(genbuf, "�����������������Ϊ������[Y/N]");
            getdata(i++, 0, genbuf, buf, 16, DOECHO, NULL, true);
            if (buf[0] == 'y' || buf[0] == 'Y')
                newinfo.lastlogin = time(0);
        }
        break;
    case '3':
        if (!real) {
			end_mmapfile(um, sizeof(struct usermemo), -1);
            clear();
            return 0;
        }
        
            /* Bigman 2000.10.2 �޸�ʹ����IDλ������ */ 
            getdata(i++, 0, "�µ�ʹ���ߴ���: ", genbuf, IDLEN + 1, DOECHO, NULL, true);
	 if (strncmp(genbuf,u->userid, IDLEN+2)&&!strncasecmp(genbuf,u->userid, IDLEN+2)) tmpchange = 1;
        if (*genbuf != '\0') {
            if (searchuser(genbuf) && !tmpchange) {
                prints("\n����! �Ѿ���ͬ�� ID ��ʹ����\n");
                fail++;
                break;
            } else {
                strncpy(newinfo.userid, genbuf, IDLEN + 2);
				strncpy(ud.userid, genbuf, IDLEN + 2);
            }
        } else {
            break;
        }
        
            /* fall throw, must change passwd for newid, by wwj 2001/5/7 */ 
    case '2':
        if (!real) {
            getdata(i++, 0, "������ԭ����: ", buf, 39, NOECHO, NULL, true);
            if (*buf == '\0' || !checkpasswd2(buf, u)) {
                prints("\n\n�ܱ�Ǹ, ����������벻��ȷ��\n");
                fail++;
                break;
            }
        }
        getdata(i++, 0, "���趨������: ", buf, 39, NOECHO, NULL, true);
        if (buf[0] == '\0') {
            prints("\n\n�����趨ȡ��, ����ʹ�þ�����\n");
            if(ans[0]!='3')
                fail++;
            break;
        }
        getdata(i++, 0, "����������������: ", genbuf, 39, NOECHO, NULL, true);
        if (strcmp(buf, genbuf)) {
            prints("\n\n�������벻һ��, �޷��趨�����롣\n");
            fail++;
            break;
        }
        
            /*      Added by cityhunter to deny others to modify SYSOP's passwd */ 
            if (real && (strcmp(u->userid, "SYSOP") == 0))
             {
            prints("\n\n����!ϵͳ��ֹ�޸�SYSOP������," NAME_POLICE "��������·�� :)");
            pressreturn();
            clear();
			end_mmapfile(um, sizeof(struct usermemo), -1);
            return 0;
            }
        
            /* end of this addin */ 
            setpasswd(buf, &newinfo);
        break;
    default:
        clear();
		end_mmapfile(um, sizeof(struct usermemo), -1);
        return 0;
    }
    if (fail != 0) {
        pressreturn();
        clear();
		end_mmapfile(um, sizeof(struct usermemo), -1);
        return 0;
    }
    for (;;)
	{
        getdata(t_lines - 1, 0, "ȷ��Ҫ�ı���?  (Yes or No): ", ans, 2, DOECHO, NULL, true);
        if (*ans == 'n' || *ans == 'N')
            break;
        if (*ans == 'y' || *ans == 'Y') {
			if (!real) {
                update_user(&newinfo, usernum, 1);
                if (strcmp(newinfo.username, uinfo.username)) {
                    strcpy(uinfo.username, newinfo.username);
                    UPDATE_UTMP_STR(username, uinfo);
                }
            }
	     if (strcmp(u->userid, newinfo.userid)) {
                char src[STRLEN], dst[STRLEN];
                if (id_invalid(newinfo.userid) == 1) {
                    prints("�ʺű�����Ӣ����ĸ��������ɣ����ҵ�һ���ַ�������Ӣ����ĸ!\n");
                    pressanykey();
                } else {
                    setmailpath(src, u->userid);
                    setmailpath(dst, newinfo.userid);
                
                    /*
                       sprintf(genbuf,"mv %s %s",src, dst);
                     */ 
                    f_mv(src, dst);
                    sethomepath(src, u->userid);
                    sethomepath(dst, newinfo.userid);
                
                    /*
                       sprintf(genbuf,"mv %s %s",src ,dst);
                     */ 
                    f_mv(src, dst);
                    sprintf(src, "tmp/email/%s", u->userid);
                    unlink(src);
                    setuserid(unum, newinfo.userid);
                }
            }
            
                /* added by netty to automatically send a mail to new user. */ 
                if ((netty_check == 1))
                 {
                if ((strchr(ud.email, '@') != NULL) && 
                     (!strstr(ud.email, "@firebird.cs")) && (!strstr(ud.email, "@bbs.")) && (!invalidaddr(ud.email)) && (!strstr(ud.email, ".bbs@"))) {
                    if ((emailfile = sysconf_str("EMAILFILE")) != NULL)
                         {
                        code = (time(0) / 2) + (rand() / 10);
                        sethomefile(genbuf, u->userid, "mailcheck");
                        if ((dp = fopen(genbuf, "w")) == NULL)
                             {
                            fclose(dp);
                            return -1;
                            }
                        fprintf(dp, "%9.9lu\n", code);
                        fclose(dp);
                        sprintf(genbuf, "/usr/lib/sendmail -f SYSOP@%s %s ", email_domain(), ud.email);
                        fout = popen(genbuf, "w");
                        fin = fopen(emailfile, "r");
                        if (fin == NULL || fout == NULL)
                            return -1;
                        fprintf(fout, "Reply-To: SYSOP@%s\n", email_domain());
                        fprintf(fout, "From: SYSOP@%s\n", email_domain());
                        fprintf(fout, "To: %s\n", ud.email);
                        fprintf(fout, "Subject: @%s@[-%9.9lu-]firebird mail check.\n", u->userid, code);
                        fprintf(fout, "X-Forwarded-By: SYSOP \n");
                        fprintf(fout, "X-Disclaimer: None\n");
                        fprintf(fout, "\n");
                        fprintf(fout, "���Ļ����������£�\n");
                        fprintf(fout, "ʹ���ߴ��ţ�%s (%s)\n", u->userid, u->username);
                        fprintf(fout, "��      ����%s\n", ud.realname);
                        fprintf(fout, "��վλ��  ��%s\n", u->lasthost);
                        fprintf(fout, "�����ʼ�  ��%s\n\n", ud.email);
                        fprintf(fout, "�װ��� %s(%s):\n", u->userid, u->username);
                        while (fgets(genbuf, 255, fin) != NULL) {
                            if (genbuf[0] == '.' && genbuf[1] == '\n')
                                fputs(". \n", fout);
                            
                            else
                                fputs(genbuf, fout);
                        }
                        fprintf(fout, ".\n");
                        fclose(fin);
                        pclose(fout);
                        }
                } else
                     {
                    if (sysconf_str("EMAILFILE") != NULL)
                         {
                        move(t_lines - 5, 0);
                        prints("\n��ĵ����ʼ���ַ ��\033[33m%s\033[m��\n", ud.email);
                        prints("���� Unix �ʺţ�ϵͳ����Ͷ�����ȷ���ţ��뵽\033[32m������\033[m���޸�..\n");
                        pressanykey();
                        }
                    }
                }
            update_user(&newinfo, unum, 1);
			memcpy(&(um->ud), &ud, sizeof(ud));
			end_mmapfile(um, sizeof(struct usermemo), -1);
			write_userdata(newinfo.userid, &ud);
            if (real)
                 {
                char secu[STRLEN];

                if (strcmp(u->userid, newinfo.userid))
                    sprintf(secu, "%s �� ID �� %s ��Ϊ %s", u->userid, getCurrentUser()->userid, newinfo.userid);   /*Haohmaru.99.5.6 */
                
                else
                    sprintf(secu, "�޸� %s �Ļ������ϻ����롣", u->userid);
                securityreport(secu, &newinfo, NULL);
            }
            break;             /*Haohmaru.98.01.10.faint...Luzi�Ӹ�forѭ��Ҳ��break! */
        }
	} /* for(;;) */
    clear();
    return 0;
}
void x_info() 
{
    modify_user_mode(GMENU);
    disply_userinfo(getCurrentUser(), 1);
    if (!strcmp("guest", getCurrentUser()->userid)) {
        pressreturn();
        return;
    }
    uinfo_query(getCurrentUser(), 0, usernum);
}
    void getfield(line, info, desc, buf, len)  int line, len;
    char *info, *desc, *buf;


{
    char prompt[STRLEN];

    
        /*    sprintf( genbuf, "  ԭ���趨: %-46.46s (%s)", buf, info ); */ 
        sprintf(genbuf, "  ԭ���趨: %-20.20s (%s)", buf, info);
    move(line, 0);
    prints(genbuf);
    sprintf(prompt, "  %s: ", desc);
    getdata(line + 1, 0, prompt, genbuf, len, DOECHO, NULL, true);
    if (genbuf[0] != '\0') {
        strncpy(buf, genbuf, len);
    }
    move(line, 0);
    clrtoeol();
    prints("  %s: %s\n", desc, buf);
    clrtoeol();
}
void x_fillform() 
{
    char rname[NAMELEN], addr[STRLEN];
    char phone[STRLEN], career[STRLEN], birth[STRLEN];
    char ans[5], *mesg, *ptr;
    FILE * fn;
    time_t now;
    struct userdata ud;

    modify_user_mode(NEW);
    move(3, 0);
    clrtobot();
    if (!strcmp("guest", getCurrentUser()->userid)) {
        prints("��Ǹ, ���� new ����һ�����ʺź����������.");
        pressreturn();
        return;
    }
    if (getCurrentUser()->userlevel & PERM_LOGINOK) {
        prints("�������ȷ���Ѿ��ɹ�, ��ӭ���뱾վ������.");
        pressreturn();
        return;
    }
    if ((time(0) - getCurrentUser()->firstlogin) < REGISTER_WAIT_TIME)
	{
        prints("���״ε��뱾վδ��" REGISTER_WAIT_TIME_NAME "...");
        prints("�����Ĵ���Ϥһ�£�����" REGISTER_WAIT_TIME_NAME "�Ժ�����дע�ᵥ��");
        pressreturn();
        return;
	}
    
	if ((fn = fopen("new_register", "r")) != NULL) {
        while (fgets(genbuf, STRLEN, fn) != NULL) {
            if ((ptr = strchr(genbuf, '\n')) != NULL)
                *ptr = '\0';
            if (strncmp(genbuf, "userid: ", 8) == 0 && strcmp(genbuf + 8, getCurrentUser()->userid) == 0) {
                fclose(fn);
                prints("վ����δ��������ע�����뵥, �����ĵȺ�.");
                pressreturn();
                return;
            }
        }
        fclose(fn);
    }
    
	/* added by KCN 1999.10.25 */ 
	ansimore("etc/register.note", false);
    getdata(t_lines - 1, 0, "��ȷ��Ҫ��дע�ᵥ�� (Y/N)? [N]: ", ans, 3, DOECHO, NULL, true);
    if (ans[0] != 'Y' && ans[0] != 'y')
        return;
//    memcpy(&ud,&curruserdata,sizeof(ud));
    memcpy(&ud,&(getSession()->currentmemo->ud),sizeof(ud));
    strncpy(rname, ud.realname, NAMELEN);
    strncpy(addr, ud.address, STRLEN);
    career[0] = phone[0] = birth[0] = '\0';
    clear();
    while (1) {
        move(3, 0);
        clrtoeol();
        prints("%s ����, ���ʵ��д���µ�����(��ʹ������):\n", getCurrentUser()->userid);
        genbuf[0] = '\0';      /*Haohmaru.99.09.17.�������ݲ��ù��� */
        while (strlen(genbuf) < 3) {
            getfield(6, "��������,��������ĺ�������ƴ��", "��ʵ����", rname, NAMELEN);
        }
        genbuf[0] = '\0';
        while (strlen(genbuf) < 2) {
            getfield(8, "ѧУϵ����λȫ��", "����λ", career, STRLEN);
        }
        genbuf[0] = '\0';
        while (strlen(genbuf) < 6) {
            getfield(10, "����嵽���һ����ƺ���", "Ŀǰסַ", addr, STRLEN);
        }
        genbuf[0] = '\0';
        while (strlen(genbuf) < 2) {
            getfield(12, "����������ʱ��,���޿��ú�����Email��ַ����", "����绰", phone, STRLEN);
        }
#ifndef HAVE_BIRTHDAY
        getfield(14, "��.��.��(��Ԫ)", "��������", birth, STRLEN);
#endif
        mesg = "���������Ƿ���ȷ, �� Q ����ע�� (Y/N/Quit)? [N]: ";
        getdata(t_lines - 1, 0, mesg, ans, 3, DOECHO, NULL, true);
        if (ans[0] == 'Q' || ans[0] == 'q')
            return;
        if (ans[0] == 'Y' || ans[0] == 'y')
            break;
    }
    strncpy(ud.realname, rname, NAMELEN);
    strncpy(ud.address, addr, STRLEN);
	write_userdata(getCurrentUser()->userid, &ud);
//	memcpy(&curruserdata,&ud,sizeof(ud));
	memcpy(&(getSession()->currentmemo->ud),&ud,sizeof(ud));
    if ((fn = fopen("new_register", "a")) != NULL) {
        now = time(NULL);
        fprintf(fn, "usernum: %d, %s", usernum, ctime(&now));
        fprintf(fn, "userid: %s\n", getCurrentUser()->userid);
        fprintf(fn, "realname: %s\n", rname);
        fprintf(fn, "career: %s\n", career);
        fprintf(fn, "addr: %s\n", addr);
        fprintf(fn, "phone: %s\n", phone);
#ifndef HAVE_BIRTHDAY
        fprintf(fn, "birth: %s\n", birth);
#else
        fprintf(fn, "birth: %d-%d-%d\n", ud.birthyear, ud.birthmonth, ud.birthday);
#endif
        fprintf(fn, "----\n");
        fclose(fn);
    }
}


