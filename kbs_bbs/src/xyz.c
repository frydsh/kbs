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

#define EXTERN

#include "bbs.h"
int use_define=0;
extern int iscolor;
extern int switch_code(); /* KCN,99.09.05 */
extern int convcode; /* KCN,99.09.05 */

int
modify_user_mode( mode )
int     mode;
{
    if(uinfo.mode==mode) return 0; /* �������update_ulist�Ĵ���. ylsdd 2001.4.27 */
    uinfo.mode = mode;
    UPDATE_UTMP(mode,uinfo);
    return 0;
}
/*
int
x_csh()
{
    int save_pager;
    clear() ;
    refresh() ;
    reset_tty() ;
    save_pager = uinfo.pager;
    uinfo.pager = 0 ;
    UPDATE_UTMP(pager,uinfo);
    report("shell out");
#ifdef SYSV
    do_exec("sh", NULL) ;
#else
    do_exec("csh", NULL);
#endif
    restore_tty() ;
    uinfo.pager = save_pager;
    UPDATE_UTMP(pager,uinfo);
    clear() ;
    return 0 ;
}
*/

int
showperminfoX( pbits, i ) /* Leeward 98.06.05 */
int     pbits, i;
{
    char        buf[ STRLEN ];

    if ( 16 == i || 11 == i || 20==i || 10 == i || 14 == i || 17 == i || 21 == i || 29==i || 28==i || 26==i)
    {
        sprintf( buf, "%c. %-30s %3s", 'A' + i, (use_define)?user_definestr[i]:permstrings[i], ((pbits >> i) & 1 ? "ON" : "OFF"));
        move(6 + (i >= 16) + (i >= 10) + (i >=11) + (i >= 14) + (i >= 17) + (i >=20) + (i >= 21) + (i>=29) + (i>=28) + (i>=26), 0);
        prints( buf );
        refresh();
        return YEA;
    }
    else
    {
        if (pbits) bell();
        return NA;
    }
}

int
showperminfo( pbits, i )
int     pbits, i;
{
    char        buf[ STRLEN ];

    sprintf( buf, "%c. %-30s %3s", 'A' + i, (use_define)?user_definestr[i]:permstrings[i],
             ((pbits >> i) & 1 ? "ON" : "OFF"));
    move( i+6-(( i>15)? 16:0) , 0+(( i>15)? 40:0) );
    prints( buf );
    refresh();
    return YEA;
}

unsigned int
setperms(pbits,prompt,numbers,showfunc)
unsigned int pbits;
char *prompt;
int numbers;
int (*showfunc)();
{
    int lastperm = numbers - 1;
    int i, done = NA;
    char choice[3];

    move(4,0);
    prints("�밴����Ҫ�Ĵ������趨%s���� Enter ����.\n",prompt);
    move(6,0);
    clrtobot();
    /*    pbits &= (1 << numbers) - 1;*/
    for (i=0; i<=lastperm; i++) {
        (*showfunc)( pbits, i,NA);
    }
    while (!done) {
        getdata(t_lines-1, 0, "ѡ��(ENTER ����): ",choice,2,DOECHO,NULL,YEA);
        *choice = toupper(*choice);
        if (*choice == '\n' || *choice == '\0') done = YEA;
        else if (*choice < 'A' || *choice > 'A' + lastperm) bell();
        else {
            i = *choice - 'A';
            pbits ^= (1 << i);
            if((*showfunc)( pbits, i ,YEA)==NA)
            {
                pbits ^= (1 << i);
            }
        }
    }
    return( pbits );
}
/* inserted by cityhunter to let OBOARDS to change a users basic level */
int
p_level()
{
    int id ;
    unsigned int newlevel;
    char ulbuf[40];
    char secu[STRLEN];
    int num;
    struct userec* lookupuser;

    if (!HAS_PERM(PERM_OBOARDS) )
    {
        move( 3, 0 );
        clrtobot();
        prints( "��Ǹ, ֻ�а������Ȩ�޵Ĺ���Ա�����޸�");
        pressreturn();
        return;
    }

    modify_user_mode( ADMIN );
    clear();
    move(8,0) ;
    prints("��������û�����Ȩ��\n") ;
    clrtoeol() ;
    move(9,0) ;
    usercomplete("������Ҫ����Ȩ�޵��û�ID: ",genbuf) ;
    if(genbuf[0] == '\0') {
        clear() ;
        return 0 ;
    }
    if(!(id = getuser(genbuf,&lookupuser))) {
        move(11,24) ;
        prints("��Ч���û�ID!!!") ;
        clrtoeol() ;
        pressreturn() ;
        clear() ;
        return 0 ;
    }
    if(strcmp(genbuf,"SYSOP")==0 && strcmp(currentuser->userid, "SYSOP")){
        move(11,0) ;
        prints("�������޸�SYSOP��Ȩ��!!!") ;
        clrtoeol() ;
        pressreturn() ;
        clear() ;
        return 0 ;
    }

    move(11,0);
    if(lookupuser->userlevel & PERM_POST)
    {
        prints("�û� '%s' ���ھ��з���Ȩ��\n",lookupuser->userid) ;
    }
    else
    {
        prints("�û� '%s' ����û�з���Ȩ��\n",lookupuser->userid) ;
    }
    getdata(12,0,"ȷ��Ҫ�޸ĸ��û��ķ���Ȩ�� (Y/N)? [N]: ",genbuf,4,DOECHO,NULL,YEA);

    if(*genbuf=='y'||*genbuf=='Y'){
        lookupuser->userlevel ^= PERM_POST;/* �ı���û�Ȩ�� */
        sprintf(secu,"�޸� %s �ķ���Ȩ��",lookupuser->userid);
        securityreport(secu,lookupuser);
        move(13,0);
        if(lookupuser->userlevel & PERM_POST)
        {
            prints("�û� '%s' �ѱ��ָ�����Ȩ��\n",lookupuser->userid) ;
        }
        else
        {
            prints("��ȡ���û� '%s' �ķ���Ȩ��\n",lookupuser->userid) ;
        }
    }
    clrtoeol() ;
    pressreturn() ;
    clear() ;
    return 0 ;
}
/* end of this insertion */
int
x_level()
{
    int id ;
    int newlevel;
    int flag=0;/*Haohmaru,98.10.05*/
    int flag1=0,flag2=0; /* bigman 2000.1.5 */
    struct userec* lookupuser;

    /* add by alex, 97.7 , strict the power of sysop */
    if (!HAS_PERM(PERM_ADMIN) || !HAS_PERM(PERM_SYSOP))
    {
        move( 3, 0 );
        clrtobot();
        prints( "��Ǹ, ֻ��ADMINȨ�޵Ĺ���Ա�����޸������û�Ȩ��");
        pressreturn();
        return;
    }

    modify_user_mode( ADMIN );
    if(!check_systempasswd())
    {
        return;
    }
    clear();
    move(0,0) ;
    prints("���İ��ڵ�Ȩ��\n") ;
    clrtoeol() ;
    move(1,0) ;
    usercomplete("�������û� ID: ",genbuf) ;
    if(genbuf[0] == '\0') {
        clear() ;
        return 0 ;
    }
    if(!(id = getuser(genbuf,&lookupuser))) {
        move(3,0) ;
        prints("�Ƿ� ID") ;
        clrtoeol() ;
        pressreturn() ;
        clear() ;
        return 0 ;
    }
    if ((lookupuser->userlevel & PERM_BOARDS ))/*Haohmaru.98.10.05*/
        flag=1;
    if ((lookupuser->userlevel & PERM_CLOAK ))/* Bigman 2000.1.5 */
        flag1=1;
    if ((lookupuser->userlevel & PERM_XEMPT ))
        flag2=1;

    move(1,0);
    clrtobot();
    move(2,0);
    prints("���趨���� '%s' ��Ȩ��\n", genbuf);
    newlevel = setperms(lookupuser->userlevel,"Ȩ��",NUMPERMS,showperminfo);
    move(2,0);
    if (newlevel == lookupuser->userlevel)
        prints("���� '%s' ��Ȩ��û�и���\n", lookupuser->userid);
    else { /* Leeward: 1997.12.02 : Modification starts */
        char        secu[STRLEN];

        sprintf(secu,"�޸� %s ��Ȩ��XPERM%d %d",
                lookupuser->userid, lookupuser->userlevel, newlevel);
        securityreport(secu,lookupuser);
        lookupuser->userlevel = newlevel;
        /* Leeward: 1997.12.02 : Modification stops */

        prints("���� '%s' ��Ȩ���Ѹ���\n",lookupuser->userid) ;
        sprintf(genbuf, "changed permissions for %s", lookupuser->userid);
        report(genbuf);
        /*Haohmaru.98.10.03.�����ΰ����Զ�����*/
        if ((lookupuser->userlevel & PERM_BOARDS ) && flag==0 )
            mail_file("etc/forbm",lookupuser->userid,"���ζ����ҵıض�");
        /* Bigman 2000.1.5 �޸�Ȩ���Զ����� */
        if ((lookupuser->userlevel & PERM_CLOAK ) && flag1==0 )
            mail_file("etc/forcloak",lookupuser->userid,"��������������Ȩ��");
        if ((lookupuser->userlevel & PERM_XEMPT ) && flag2==0 )
            mail_file("etc/forlongid",lookupuser->userid,"���������������ʺ�Ȩ��");
    }
    pressreturn() ;
    clear() ;
    return 0 ;
}

int
XCheckLevel() /* Leeward 98.06.05 */
{
    int newlevel;
    struct userec scanuser;

    if (!HAS_PERM(PERM_ADMIN) || !HAS_PERM(PERM_SYSOP))
    {
        move( 3, 0 );
        clrtobot();
        prints( "��Ǹ, ��û�д�Ȩ��");
        pressreturn();
        return;
    }

    modify_user_mode( ADMIN );
    if(!check_systempasswd())
    {
        return;
    }
    clear();
    move(0,0) ;
    prints("��ʾ�����ض�Ȩ�޵��û�������\n") ;
    clrtoeol() ;
    move(2,0);
    prints("���趨��Ҫ����Ȩ��\n");
    scanuser.userlevel = 0;
    newlevel = setperms(scanuser.userlevel,"Ȩ��",NUMPERMS,showperminfoX);
    move(2,0);
    if (newlevel == scanuser.userlevel)
        prints("��û���趨�κ�Ȩ��\n");
    else
    {
        char        secu[STRLEN];
        char        buffer[256];
        int         fhp;
        FILE        *fpx;
        long        count = 0L;

        sprintf(buffer, "tmp/XCL.%s%d", currentuser->userid, getpid());
        if (- 1 == (fhp = open(".PASSWDS", O_RDONLY)))
        {
            prints("ϵͳ����: �޷��򿪿����ļ�\n");
        }
        else if (NULL == (fpx = fopen(buffer, "w")))
        {
            close(fhp);
            prints("ϵͳ����: �޷�����ʱ�ļ�\n");
        }
        else
        {
            prints("��ʾ����������Ҫ�ϳ�ʱ��������, �����ĵȴ�. ");
            clrtoeol();
            if (askyn("��ȷ��Ҫ������ʾ��", 0))
            {
                while (read(fhp, &scanuser, sizeof(struct userec)) > 0)
                {
                    if ((scanuser.userlevel & newlevel) == newlevel
                            &&  strcmp("SYSOP", scanuser.userid))
                    {
                        count ++;
                        fprintf(fpx,"[1m[33m�뱣����һ��λ����Ļ��һ�У���ʱ�� X ���ɸ������û�����Ҫ���䲹�����ע������[m\n\n");
                        fprintf(fpx,"�û�����(�ǳ�) : %s(%s)\n\n", scanuser.userid, scanuser.username);
                        fprintf(fpx,"��  ʵ  ��  �� : %s\n\n", scanuser.realname);
                        fprintf(fpx,"��  ס  ס  ַ : %s\n\n", scanuser.address);
                        fprintf(fpx,"��  ��  ��  �� : %s\n\n", scanuser.email);
                        fprintf(fpx,"��λ$�绰@��֤ : %s\n\n", scanuser.realemail);
                        fprintf(fpx,"ע  ��  ��  �� : %s\n", ctime(&scanuser.firstlogin));
                        fprintf(fpx,"���ĵ�¼���� : %s\n", ctime(&scanuser.lastlogin));
                        fprintf(fpx,"���ĵ�¼���� : %s\n\n", scanuser.lasthost );
                        fprintf(fpx,"��  վ  ��  �� : %d ��\n\n", scanuser.numlogins);
                        fprintf(fpx,"��  ��  ��  Ŀ : %d ƪ\n\n", scanuser.numposts);
                    }
                }
                fprintf(fpx, "[1m[33mһ���г��� %ld ����д�Ȩ�޵��û�����[m\n\n*** ������ʾ��������һ�У���������ϣ��밴 q ������ *** (���¾�Ϊ����)", count);
                { int dummy; /* process the situation of a too high screen :PP */
                    for (dummy = 0; dummy < t_lines * 4; dummy ++) fputs("\n", fpx);
                }
                close(fhp);
                fclose(fpx);

                sprintf(secu, "[1m[33mһ���г��� %ld ����д�Ȩ�޵��û�����[m", count);
                move(2, 0);
                prints(secu);
                clrtoeol();
                sprintf(genbuf, "listed %ld userlevel of %d", count, newlevel);
                report(genbuf);
                pressanykey();

                /*sprintf(secu, "��ʾ�����ض�Ȩ�޵� %ld ���û�������", count);*/
                clear();
                ansimore(buffer, NA);
                clear();
                move(2, 0);
                prints("��ʾ�������");
                clrtoeol();

                unlink(buffer);
            }
            else
            {
                move(2, 0);
                prints("ȡ����ʾ����");
                clrtoeol();
            }
        }
    }
    pressreturn() ;
    clear() ;
    return 0 ;
}

int
x_userdefine()
{
    int id ;
    int newlevel;
    extern int nettyNN;
    struct userec* lookupuser;

    modify_user_mode( USERDEF );
    if(!(id = getuser(currentuser->userid,&lookupuser))) {
        move(3,0) ;
        prints("����� ���� ID...") ;
        clrtoeol() ;
        pressreturn() ;
        clear() ;
        return 0 ;
    }
    if( !strcmp(currentuser->userid,"guest") )
	return 0;

    move(1,0);
    clrtobot();
    move(2,0);
    use_define=1;
    newlevel = setperms(lookupuser->userdefine,"����",NUMDEFINES,showperminfo);
    move(2,0);
    if (newlevel == lookupuser->userdefine)
        prints("����û���޸�...\n");
    else {
        lookupuser->userdefine = newlevel;
        currentuser->userdefine=newlevel;
        if (((convcode)&&(newlevel&DEF_USEGB))  /* KCN,99.09.05 */
                ||((!convcode)&&!(newlevel&DEF_USEGB)))
            switch_code();
        uinfo.pager|=FRIEND_PAGER;
        if(!(uinfo.pager&ALL_PAGER))
        {
            if(!DEFINE(DEF_FRIENDCALL))
                uinfo.pager&=~FRIEND_PAGER;
        }
        uinfo.pager&=~ALLMSG_PAGER;
        uinfo.pager&=~FRIENDMSG_PAGER;
        if(DEFINE(DEF_FRIENDMSG))
        {
            uinfo.pager|=FRIENDMSG_PAGER;
        }
        if(DEFINE(DEF_ALLMSG))
        {
            uinfo.pager|=ALLMSG_PAGER;
            uinfo.pager|=FRIENDMSG_PAGER;
        }
		UPDATE_UTMP(pager,uinfo);
        if(DEFINE(DEF_ACBOARD))
            nettyNN=NNread_init();
        prints("�µĲ����趨���...\n\n") ;
    }
    iscolor=(DEFINE(DEF_COLOR))?1:0;
    pressreturn() ;
    clear() ;
    use_define=0;
    return 0 ;
}




int
x_cloak()
{
    modify_user_mode( GMENU );
    report("toggle cloak");
    uinfo.invisible = (uinfo.invisible)?NA:YEA ;
	UPDATE_UTMP(invisible,uinfo);
    if (!uinfo.in_chat) {
        move(1,0) ;
        clrtoeol();
        prints( "������ (cloak) �Ѿ�%s��!",
                (uinfo.invisible) ? "����" : "ֹͣ" ) ;
        pressreturn();
    }
    return 0 ;
}

int
x_date()
{
    time_t t;
    char        ans[3];

    modify_user_mode( XMENU );
    clear();
    move(8,0);
    time(&t);
    prints("Ŀǰϵͳ������ʱ��: %s", ctime(&t));
    clrtoeol();
    pressreturn();
    return 0;
}

void
x_edits()
{
    int aborted;
    char ans[7],buf[STRLEN];
    int ch,num;
    char *e_file[]={"plans","signatures","notes","logout",NULL};
    char *explain_file[]={"����˵����","ǩ����","�Լ��ı���¼","��վ�Ļ���",NULL};

    modify_user_mode( GMENU );
    clear();
    move(1,0);
    prints("���޸��˵���\n\n");
    for(num=0;e_file[num]!=NULL&&explain_file[num]!=NULL;num++)
    {
        prints("[[32m%d[m] %s\n",num+1,explain_file[num]);
    }
    prints("[[32m%d[m] �������\n",num+1);

    getdata(num+5,0,"��Ҫ������һ����˵���: ",ans,2,DOECHO,NULL,YEA);
    if(ans[0]-'0'<=0 || ans[0]-'0'>num|| ans[0]=='\n'|| ans[0]=='\0')
        return;

    ch=ans[0]-'0'-1;
    switch( ch ){
    	case 0:
        	modify_user_mode( EDITPLAN );
	case 1:
	    	modify_user_mode( EDITSIG );
    }
    setuserfile(genbuf,e_file[ch]);
    move(3,0);
    clrtobot();
    sprintf(buf,"(E)�༭ (D)ɾ�� %s? [E]: ",explain_file[ch]);
    getdata(3,0,buf,ans,2,DOECHO,NULL,YEA);
    if (ans[0] == 'D' || ans[0] == 'd') {
        unlink(genbuf);
        move(5,0);
        prints("%s ��ɾ��\n",explain_file[ch]);
        sprintf(buf,"delete %s",explain_file[ch]);
        report(buf);
        pressreturn();
        clear();
        return;
    }
    modify_user_mode( EDITUFILE);
    aborted = vedit(genbuf, NA);
    clear();
    if (!aborted) {
        prints("%s ���¹�\n",explain_file[ch]);
        sprintf(buf,"edit %s",explain_file[ch]);
        if(!strcmp(e_file[ch],"signatures"))
        {
            set_numofsig();
            prints("ϵͳ�����趨�Լ��������ǩ����...");
        }
        report(buf);
    }else
        prints("%s ȡ���޸�\n",explain_file[ch]);
    pressreturn();
}

void
a_edits()
{
    int aborted;
    char ans[7],buf[STRLEN];
    int ch,num;

    /* Leeward 98.04.01 added: sysconf.ini */
    /* Leeward 98.07.31 added: .badIP */
    /* stephen 2000.10.17 added: /usr/share/apache/htdocs/script/menucontext.js */
    /* period  2000.10.17 link /backup/www/htdocs/script/menucontext.js --> /home0/bbs/etc/www_menu.js */
    char *e_file[]={"../Welcome","../vote/notes","issue","movie","logout","menu.ini", "mailcheck","s_fill","f_fill.realname","f_fill.unit","f_fill.address","f_fill.telephone","f_fill.real","f_fill.chinese","f_fill.toomany","f_fill.reply","smail","fmail","../.badname", "../.badIP", "../.badword", "sysconf.ini", "www_menu.js", "../0Announce/hotinfo", NULL};
    /* "/usr/share/apache/htdocs/script/menucontext.js", NULL};    */
#ifndef LEEWARD_X_FILTER
    char *explain_file[]={"Welcome","���ñ���¼","��վ��ӭ��","�����","��վ����", "menu.ini","���ȷ�ϵ�","ע�ᵥ��ɵ�","ע�ᵥʧ�ܵ�(��ʵ����)","ע�ᵥʧ�ܵ�(����λ)","ע�ᵥʧ�ܵ�(��ס��ַ)","ע�ᵥʧ�ܵ�(����绰)","ע�ᵥʧ�ܵ�(��ʵ����)","ע�ᵥʧ�ܵ�(������д)","ע�ᵥʧ�ܵ�(�����ID)","ע�ᵥʧ�ܵ�(Mail Reply)" ,"���ȷ����ɵ�        ","���ȷ��ʧ�ܵ�","����ע��� ID         ", "���ɵ�¼�� IP", "ϵͳ�Զ����˵Ĵ���", "sysconf.ini", "WWW���˵�",  "�����ȵ�",NULL};
#else
char *explain_file[]={"Welcome","���ñ���¼","��վ��ӭ��","�����","��վ����", "menu.ini","���ȷ�ϵ�","ע�ᵥ��ɵ�","ע�ᵥʧ�ܵ�(��ʵ����)","ע�ᵥʧ�ܵ�(����λ)","ע�ᵥʧ�ܵ�(��ס��ַ)","ע�ᵥʧ�ܵ�(����绰)","ע�ᵥʧ�ܵ�(��ʵ����)","ע�ᵥʧ�ܵ�(������д)","ע�ᵥʧ�ܵ�(�����ID)","ע�ᵥʧ�ܵ�(Mail Reply)" ,"���ȷ����ɵ�        ","���ȷ��ʧ�ܵ�","����ע��� ID         ", "���ɵ�¼�� IP", "ϵͳ�Զ����˵Ĵ��[1m[31m�˵���Ŀǰ��Ч�������趨���޸�[m��", "sysconf.ini", "WWW���˵�", "�����ȵ�",NULL};
#endif

    modify_user_mode( ADMIN );
    if(!check_systempasswd())
    {
        return;
    }
    clear();
    move(0,0);
    prints("����ϵͳ����\n\n");
    for(num=0;e_file[num]!=NULL&&explain_file[num]!=NULL;num++)
    {
        prints("[[32m%2d[m] %s%s",num+1,explain_file[num],
               (num + 1 >= 9 && num + 1 <= 20 && (num + 1) % 2) ? "      " : "\n");
        /* Leeward 98.03.29 ������ʾ���֣��Ա���롰ϵͳ�Զ����˵Ĵ��һ�� */
        /* Leeward 98.07.31 ������ʾ���֣��Ա���롰���ɵ�¼�� IP��һ�� */
    }
    prints("[[32m%2d[m] �������\n",num+1);

    getdata(num,0,"��Ҫ������һ��ϵͳ����: ",ans,3,DOECHO,NULL,YEA);
    ch=atoi(ans);
    if(!isdigit(ans[0])||ch<=0 || ch>num|| ans[0]=='\n'|| ans[0]=='\0')
        return;
    ch-=1;
    sprintf(genbuf,"etc/%s",e_file[ch]);
    move(2,0);
    clrtobot();
    sprintf(buf,"(E)�༭ (D)ɾ�� %s? [E]: ",explain_file[ch]);
    getdata(3,0,buf,ans,2,DOECHO,NULL,YEA);
    if (ans[0] == 'D' || ans[0] == 'd') {
        {
            char        secu[STRLEN];
            sprintf(secu,"ɾ��ϵͳ������%s",explain_file[ch]);
            securityreport(secu,NULL);
        }
        unlink(genbuf);
        move(5,0);
        prints("%s ��ɾ��\n",explain_file[ch]);
        sprintf(buf,"delete %s",explain_file[ch]);
        report(buf);
        pressreturn();
        clear();
        return;
    }
    modify_user_mode( EDITSFILE);
    aborted = vedit(genbuf, NA);
    clear();
    if (aborted!=-1) {
        prints("%s ���¹�",explain_file[ch]);
        sprintf(buf,"edit %s",explain_file[ch]);
        report(buf);
        {
            char        secu[STRLEN];
            sprintf(secu,"�޸�ϵͳ������%s",explain_file[ch]);
            securityreport(secu,NULL);
        }

        if(!strcmp(e_file[ch],"../Welcome"))
        {
            unlink("Welcome.rec");
            prints("\nWelcome ��¼������");
        }
    }
    pressreturn();
}

#ifdef BBSDOORS

void
ent_bnet()  /* Bill Schwartz */
{
    int save_pager = uinfo.pager;
    uinfo.pager = -1;
    report("BBSNet Enter") ;
    modify_user_mode( BBSNET );
    /* bbsnet.sh is a shell script that can be customized without */
    /* having to recompile anything.  If you edit it while someone */
    /* is in bbsnet they will be sent back to the xyz menu when they */
    /* leave the system they are currently in. */

    reset_tty() ;
    do_exec("bbsnet.sh",NULL) ;
    restore_tty() ;
    uinfo.pager = save_pager;
    report("BBSNet Exit");
    clear() ;
}

#endif
