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

#include "bbs.h"
int use_define=0;
extern int iscolor;
extern int switch_code(); /* KCN,99.09.05 */
extern int convcode; /* KCN,99.09.05 */
int
modify_user_mode( mode )
int     mode;
{
    if(uinfo.mode==mode) return 0; /* 必须减少update_ulist的次数. ylsdd 2001.4.27 */
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

int showperminfoX(unsigned int pbits,int i,int flag ) /* Leeward 98.06.05 */
{
    char        buf[ STRLEN ];

    if ( 16 == i || 11 == i || 20==i || 10 == i || 14 == i || 17 == i || 21 == i || 29==i || 28==i || 26==i || 24==i)
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
showperminfo( unsigned int pbits,int i,int flag)
{
    char        buf[ STRLEN ];

    sprintf( buf, "%c. %-30s %3s", 'A' + i, (use_define)?user_definestr[i]:permstrings[i],
             ((pbits >> i) & 1 ? "ON" : "OFF"));
    move( i+6-(( i>15)? 16:0) , 0+(( i>15)? 40:0) );
    prints( buf );
    refresh();
    return YEA;
}

unsigned int setperms(unsigned int pbits,char *prompt,int numbers,int (*showfunc)(unsigned int ,int ,int))
{
    int lastperm = numbers - 1;
    int i, done = NA;
    char choice[3];

    move(4,0);
    prints("请按下你要的代码来设定%s，按 Enter 结束.\n",prompt);
    move(6,0);
    clrtobot();
    /*    pbits &= (1 << numbers) - 1;*/
    for (i=0; i<=lastperm; i++) {
        (*showfunc)( pbits, i,NA);
    }
    while (!done) {
        getdata(t_lines-1, 0, "选择(ENTER 结束): ",choice,2,DOECHO,NULL,YEA);
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
/* 删除过期的帐号 */
/* 算是给奥运的献礼 */
/* Bigman 2001.7.14 */
int confirm_delete_id()
{	
   char commd[STRLEN],buff[STRLEN];
	int num;

   if (!HAS_PERM(currentuser,PERM_ADMIN) )
    {
        move( 3, 0 );
        clrtobot();
        prints( "抱歉, 只有总管理员才能修改");
        pressreturn();
        return 1;
    }

    modify_user_mode( ADMIN );
    clear();
    move(8,0) ;
    prints("\n") ;
    clrtoeol() ;
    move(9,0) ;
    usercomplete("请输入要确认清除的用户ID: ",genbuf) ;
    if(genbuf[0] == '\0') {
        clear() ;
        return 1 ;}

    if((usernum = searchuser(genbuf )) != 0)
	{
	prints("此帐号有人使用\n") ;
	pressreturn();
        return 1;
	}

    sethomepath( buff,genbuf);
    /*
    sprintf(commd,"rm -rf %s",buff);
    */
    f_rm(buff);
    setmailpath( buff,genbuf);
    f_rm(buff);
    /*
    sprintf(commd,"rm -rf %s",buff);
    */

    report("delete confirmly dead id's directory");

    clrtoeol() ;
    pressreturn() ;
    clear() ;
    return 0 ;
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

    if (!HAS_PERM(currentuser,PERM_OBOARDS) )
    {
        move( 3, 0 );
        clrtobot();
        prints( "抱歉, 只有版务管理权限的管理员才能修改");
        pressreturn();
        return 0;
    }

    modify_user_mode( ADMIN );
    clear();
    move(8,0) ;
    prints("封禁或解除用户发文权限\n") ;
    clrtoeol() ;
    move(9,0) ;
    usercomplete("请输入要更改权限的用户ID: ",genbuf) ;
    if(genbuf[0] == '\0') {
        clear() ;
        return 0 ;
    }
    if(!(id = getuser(genbuf,&lookupuser))) {
        move(11,24) ;
        prints("无效的用户ID!!!") ;
        clrtoeol() ;
        pressreturn() ;
        clear() ;
        return 0 ;
    }
    if(strcmp(genbuf,"SYSOP")==0 && strcmp(currentuser->userid, "SYSOP")){
        move(11,0) ;
        prints("不可以修改SYSOP的权限!!!") ;
        clrtoeol() ;
        pressreturn() ;
        clear() ;
        return 0 ;
    }

    move(11,0);
    if(lookupuser->userlevel & PERM_POST)
    {
        prints("用户 '%s' 现在具有发文权限\n",lookupuser->userid) ;
    }
    else
    {
        prints("用户 '%s' 现在没有发文权限\n",lookupuser->userid) ;
    }
    getdata(12,0,"确定要修改该用户的发文权限 (Y/N)? [N]: ",genbuf,4,DOECHO,NULL,YEA);

    if(*genbuf=='y'||*genbuf=='Y'){
        lookupuser->userlevel ^= PERM_POST;/* 改变该用户权限 */
        sprintf(secu,"修改 %s 的发文权限",lookupuser->userid);
        securityreport(secu,lookupuser,NULL);
        move(13,0);
        if(lookupuser->userlevel & PERM_POST)
        {
            prints("用户 '%s' 已被恢复发文权限\n",lookupuser->userid) ;
        }
        else
        {
            prints("已取消用户 '%s' 的发文权限\n",lookupuser->userid) ;
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
    unsigned int newlevel;
    int flag=0;/*Haohmaru,98.10.05*/
    int flag1=0,flag2=0; /* bigman 2000.1.5 */
    struct userec* lookupuser;

    /* add by alex, 97.7 , strict the power of sysop */
    if (!HAS_PERM(currentuser,PERM_ADMIN) || !HAS_PERM(currentuser,PERM_SYSOP))
    {
        move( 3, 0 );
        clrtobot();
        prints( "抱歉, 只有ADMIN权限的管理员才能修改其他用户权限");
        pressreturn();
        return 0;
    }

    modify_user_mode( ADMIN );
    if(!check_systempasswd())
    {
        return 0;
    }
    clear();
    move(0,0) ;
    prints("更改"NAME_USER_SHORT"的权限\n") ;
    clrtoeol() ;
    move(1,0) ;
    usercomplete("请输入"NAME_USER_SHORT" ID: ",genbuf) ;
    if(genbuf[0] == '\0') {
        clear() ;
        return 0 ;
    }
    if(!(id = getuser(genbuf,&lookupuser))) {
        move(3,0) ;
        prints("非法 ID") ;
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
    prints("请设定"NAME_USER_SHORT" '%s' 的权限\n", genbuf);
    newlevel = setperms(lookupuser->userlevel,"权限",NUMPERMS,showperminfo);
    move(2,0);
    if (newlevel == lookupuser->userlevel)
        prints(NAME_USER_SHORT" '%s' 的权限没有更改\n", lookupuser->userid);
    else { /* Leeward: 1997.12.02 : Modification starts */
        char        secu[STRLEN];

        sprintf(secu,"修改 %s 的权限XPERM%d %d",
                lookupuser->userid, lookupuser->userlevel, newlevel);
        securityreport(secu,lookupuser,NULL);
        lookupuser->userlevel = newlevel;
        /* Leeward: 1997.12.02 : Modification stops */

        prints(NAME_USER_SHORT" '%s' 的权限已更改\n",lookupuser->userid) ;
        sprintf(genbuf, "changed permissions for %s", lookupuser->userid);
        report(genbuf);
        /*Haohmaru.98.10.03.给新任板主自动发信*/
        if ((lookupuser->userlevel & PERM_BOARDS ) && flag==0 )
            mail_file(currentuser->userid,"etc/forbm",lookupuser->userid,"新任"NAME_BM"必读",0);
        /* Bigman 2000.1.5 修改权限自动发信 */
        if ((lookupuser->userlevel & PERM_CLOAK ) && flag1==0 )
            mail_file(currentuser->userid,"etc/forcloak",lookupuser->userid,NAME_SYSOP_GROUP"授予您隐身权限",0);
        if ((lookupuser->userlevel & PERM_XEMPT ) && flag2==0 )
            mail_file(currentuser->userid,"etc/forlongid",lookupuser->userid,NAME_SYSOP_GROUP"授予您长期帐号权限",0);
    }
    pressreturn() ;
    clear() ;
    return 0 ;
}

int
XCheckLevel() /* Leeward 98.06.05 */
{
    unsigned int newlevel;
    struct userec scanuser;

    if (!HAS_PERM(currentuser,PERM_ADMIN) || !HAS_PERM(currentuser,PERM_SYSOP))
    {
        move( 3, 0 );
        clrtobot();
        prints( "抱歉, 您没有此权限");
        pressreturn();
        return -1;
    }

    modify_user_mode( ADMIN );
    if(!check_systempasswd())
    {
        return -1;
    }
    clear();
    move(0,0) ;
    prints("列示具有特定权限的"NAME_USER_SHORT"的资料\n") ;
    clrtoeol() ;
    move(2,0);
    prints("请设定需要检查的权限\n");
    scanuser.userlevel = 0;
    newlevel = setperms(scanuser.userlevel,"权限",NUMPERMS,showperminfoX);
    move(2,0);
    if (newlevel == scanuser.userlevel)
        prints("你没有设定任何权限\n");
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
            prints("系统错误: 无法打开口令文件\n");
        }
        else if (NULL == (fpx = fopen(buffer, "w")))
        {
            close(fhp);
            prints("系统错误: 无法打开临时文件\n");
        }
        else
        {
            prints("列示操作可能需要较长时间才能完成, 请耐心等待. ");
            clrtoeol();
            if (askyn("你确定要进行列示吗", 0))
            {
                while (read(fhp, &scanuser, sizeof(struct userec)) > 0)
                {
                    if ((scanuser.userlevel & newlevel) == newlevel
                            &&  strcmp("SYSOP", scanuser.userid))
                    {
                        count ++;
                        fprintf(fpx,"[1m[33m请保持这一行位于屏幕第一行，此时按 X 键可给下列用户发信要求其补齐个人注册资料[m\n\n");
                        fprintf(fpx,"用户代号(昵称) : %s(%s)\n\n", scanuser.userid, scanuser.username);
                        fprintf(fpx,"真  实  姓  名 : %s\n\n", scanuser.realname);
                        fprintf(fpx,"居  住  住  址 : %s\n\n", scanuser.address);
                        fprintf(fpx,"电  子  邮  件 : %s\n\n", scanuser.email);
                        fprintf(fpx,"单位$电话@认证 : %s\n\n", scanuser.realemail);
                        fprintf(fpx,"注  册  日  期 : %s\n", ctime(&scanuser.firstlogin));
                        fprintf(fpx,"最后的登录日期 : %s\n", ctime(&scanuser.lastlogin));
                        fprintf(fpx,"最后的登录机器 : %s\n\n", scanuser.lasthost );
                        fprintf(fpx,"上  站  次  数 : %d 次\n\n", scanuser.numlogins);
                        fprintf(fpx,"文  章  数  目 : %d 篇\n\n", scanuser.numposts);
                    }
                }
                fprintf(fpx, "[1m[33m一共列出了 %ld 项具有此权限的用户资料[m\n\n*** 这是列示结果的最后一行．如果检查完毕，请按 q 键结束 *** (以下均为空行)", count);
                { int dummy; /* process the situation of a too high screen :PP */
                    for (dummy = 0; dummy < t_lines * 4; dummy ++) fputs("\n", fpx);
                }
                close(fhp);
                fclose(fpx);

                sprintf(secu, "[1m[33m一共列出了 %ld 项具有此权限的用户资料[m", count);
                move(2, 0);
                prints(secu);
                clrtoeol();
                sprintf(genbuf, "listed %ld userlevel of %d", count, newlevel);
                report(genbuf);
                pressanykey();

                /*sprintf(secu, "列示具有特定权限的 %ld 个用户的资料", count);*/
                clear();
                ansimore(buffer, NA);
                clear();
                move(2, 0);
                prints("列示操作完成");
                clrtoeol();

                unlink(buffer);
            }
            else
            {
                move(2, 0);
                prints("取消列示操作");
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
    unsigned int newlevel;
    extern int nettyNN;
    struct userec* lookupuser;

    modify_user_mode( USERDEF );
    if(!(id = getuser(currentuser->userid,&lookupuser))) {
        move(3,0) ;
        prints("错误的 "NAME_USER_SHORT" ID...") ;
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
    newlevel = setperms(lookupuser->userdefine,"参数",NUMDEFINES,showperminfo);
    move(2,0);
    if (newlevel == lookupuser->userdefine)
        prints("参数没有修改...\n");
    else {
        lookupuser->userdefine = newlevel;
        currentuser->userdefine=newlevel;
        if (((convcode)&&(newlevel&DEF_USEGB))  /* KCN,99.09.05 */
                ||((!convcode)&&!(newlevel&DEF_USEGB)))
            switch_code();
        uinfo.pager|=FRIEND_PAGER;
        if(!(uinfo.pager&ALL_PAGER))
        {
            if(!DEFINE(currentuser,DEF_FRIENDCALL))
                uinfo.pager&=~FRIEND_PAGER;
        }
        uinfo.pager&=~ALLMSG_PAGER;
        uinfo.pager&=~FRIENDMSG_PAGER;
        if(DEFINE(currentuser,DEF_FRIENDMSG))
        {
            uinfo.pager|=FRIENDMSG_PAGER;
        }
        if(DEFINE(currentuser,DEF_ALLMSG))
        {
            uinfo.pager|=ALLMSG_PAGER;
            uinfo.pager|=FRIENDMSG_PAGER;
        }
		UPDATE_UTMP(pager,uinfo);
        if(DEFINE(currentuser,DEF_ACBOARD))
            nettyNN=NNread_init();
        prints("新的参数设定完成...\n\n") ;
    }
    iscolor=(DEFINE(currentuser,DEF_COLOR))?1:0;
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
        prints( "隐身术 (cloak) 已经%s了!",
                (uinfo.invisible) ? "启动" : "停止" ) ;
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
    prints("目前系统日期与时间: %s", ctime(&t));
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
    char *explain_file[]={"个人说明档","签名档","自己的备忘录","离站的画面",NULL};

    modify_user_mode( GMENU );
    clear();
    move(1,0);
    prints("编修个人档案\n\n");
    for(num=0;e_file[num]!=NULL&&explain_file[num]!=NULL;num++)
    {
        prints("[[32m%d[m] %s\n",num+1,explain_file[num]);
    }
    prints("[[32m%d[m] 都不想改\n",num+1);

    getdata(num+5,0,"你要编修哪一项个人档案: ",ans,2,DOECHO,NULL,YEA);
    if(ans[0]-'0'<=0 || ans[0]-'0'>num|| ans[0]=='\n'|| ans[0]=='\0')
        return;

    ch=ans[0]-'0'-1;
    switch( ch ){
    	case 0:
        	modify_user_mode( EDITPLAN );
	case 1:
	    	modify_user_mode( EDITSIG );
    }

    sethomefile(genbuf,currentuser->userid,e_file[ch]);
    move(3,0);
    clrtobot();
    sprintf(buf,"(E)编辑 (D)删除 %s? [E]: ",explain_file[ch]);
    getdata(3,0,buf,ans,2,DOECHO,NULL,YEA);
    if (ans[0] == 'D' || ans[0] == 'd') {
        unlink(genbuf);
        move(5,0);
        prints("%s 已删除\n",explain_file[ch]);
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
        prints("%s 更新过\n",explain_file[ch]);
        sprintf(buf,"edit %s",explain_file[ch]);
        if(!strcmp(e_file[ch],"signatures"))
        {
            set_numofsig();
            prints("系统重新设定以及读入你的签名档...");
        }
        report(buf);
    }else
        prints("%s 取消修改\n",explain_file[ch]);
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
    static const char *e_file[]={"../Welcome","../vote/notes","issue","movie","logout","menu.ini", "proxyIP", "mailcheck","s_fill","f_fill.realname","f_fill.unit","f_fill.address","f_fill.telephone","f_fill.real","f_fill.chinese","f_fill.toomany","f_fill.proxy","smail","f_fill","../.badname", "../.badIP", "../.badword", "sysconf.ini", "www_menu.js", "../0Announce/hotinfo", "forbm", "forcloak", "forlongid", "../innd/newsfeeds.bbs", NULL};
    /* "/usr/share/apache/htdocs/script/menucontext.js", NULL};    */
	static const char *explain_file[]={"Welcome","公用备忘录","进站欢迎档","活动看版","离站画面", "menu.ini","穿梭IP","身份确认档","注册单完成档","注册单失败档(真实姓名)","注册单失败档(服务单位)","注册单失败档(居住地址)","注册单失败档(联络电话)","注册单失败档(真实资料)","注册单失败档(中文填写)","注册单失败档(过多的ID)","注册单失败档(不能穿梭注册)" ,"身份确认完成档        ","身份确认失败档","不可注册的 ID         ", "不可登录的 IP", "系统自动过滤的词语    ", "sysconf.ini", "WWW主菜单             ", "近期热点","给新任版主的信        ", "给隐身用户的信","给长期用户的信        ", "转信版和新闻组对应", NULL};

    modify_user_mode( ADMIN );
    if(!check_systempasswd())
    {
        return;
    }
    clear();
    move(0,0);
    prints("编修系统档案\n\n");
    for(num=0;e_file[num]!=NULL&&explain_file[num]!=NULL;num++)
    {
        prints("[[32m%2d[m] %s%s",num+1,explain_file[num],
               (num + 1 >= 1 && num + 1 <= 28 && (num + 1) % 2) ? "      " : "\n");
        /* Leeward 98.03.29 调整显示布局，以便加入“系统自动过滤的词语”一项 */
        /* Leeward 98.07.31 调整显示布局，以便加入“不可登录的 IP”一项 */
	/* Bigman 2001.6.23 调整布局， 增加其他几项内容 */
    }
	
    prints("[[32m%2d[m] 都不想改\n",num+1);

	/* 下面的21是行号，以后添加，可以相应调整 */
    getdata(21,0,"你要编修哪一项系统档案: ",ans,3,DOECHO,NULL,YEA);
    ch=atoi(ans);
    if(!isdigit(ans[0])||ch<=0 || ch>num|| ans[0]=='\n'|| ans[0]=='\0')
        return;
    ch-=1;
    sprintf(genbuf,"etc/%s",e_file[ch]);
    move(2,0);
    clrtobot();
    sprintf(buf,"(E)编辑 (D)删除 %s? [E]: ",explain_file[ch]);
    getdata(3,0,buf,ans,2,DOECHO,NULL,YEA);
    if (ans[0] == 'D' || ans[0] == 'd') {
        {
            char        secu[STRLEN];
            sprintf(secu,"删除系统档案：%s",explain_file[ch]);
            securityreport(secu,NULL,NULL);
        }
        unlink(genbuf);
        move(5,0);
        prints("%s 已删除\n",explain_file[ch]);
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
        prints("%s 更新过",explain_file[ch]);
        sprintf(buf,"edit %s",explain_file[ch]);
        report(buf);
        {
            char        secu[STRLEN];
            sprintf(secu,"修改系统档案：%s",explain_file[ch]);
            securityreport(secu,NULL,NULL);
        }

        if(!strcmp(e_file[ch],"../Welcome"))
        {
            unlink("Welcome.rec");
            prints("\nWelcome 记录档更新");
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

    do_exec("bbsnet.sh",NULL) ;
    uinfo.pager = save_pager;
    report("BBSNet Exit");
    clear() ;
}

#endif

