
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
#include <time.h>
#include <ctype.h>

extern cmpbnames();

/*Add by SmallPig*/
char filebuf[STRLEN];
char deadline[STRLEN];/*补充说明*/
char denymsg[STRLEN];
int  denyday;

int
listdeny(int page)  /* Haohmaru.12.18.98.为那些变态得封人超过一屏的板主而写 */
{
    FILE *fp;
    int x = 0, y = 3, cnt = 0, max = 0, len;
    int i;
    char u_buf[STRLEN*2], line[STRLEN*2], *nick;

    clear();
    prints("设定无法 Post 的名单\n");
    move(y,x);
    CreateNameList();
    setbfile( genbuf, currboard,"deny_users" );
    if ((fp = fopen(genbuf, "r")) == NULL) {
        prints("(none)\n");
        return 0;
    }
    for(i=1;i<=page*20;i++)
    {
        if(fgets(genbuf, 2*STRLEN, fp)==NULL)
            break;
    }
    while(fgets(genbuf, 2*STRLEN, fp) != NULL)
    {
        strtok( genbuf, " \n\r\t" );
        strcpy( u_buf, genbuf );
        AddNameList( u_buf );
        nick = (char *) strtok( NULL, "\n\r\t" );
        if( nick != NULL )
        {
            while( *nick == ' ' )  nick++;
            if( *nick == '\0' )  nick = NULL;
        }
        if( nick == NULL )
        {
            strcpy( line, u_buf );
        } else
        {
            if (cnt<20)
                sprintf( line, "%-12s%s", u_buf, nick );
        }
        if( (len = strlen( line )) > max )
            max = len;
/*        if( x + len > 90 )
            line[ 90 - x ] = '\0';*-P-*/
        if( x + len > 79 ) line[ 79 ] = '\0';
        if (cnt<20)/*haohmaru.12.19.98*/
            prints( "%s", line );
        cnt++;
        if ((++y) >= t_lines-1)
        {
            y = 3;
            x += max + 2;
            max = 0;
            /*            if( x > 90 )  break;*/
        }
        move(y,x);
    }
    fclose(fp);
    if (cnt == 0) prints("(none)\n");
    return cnt;
}

int
addtodeny(uident) /* 添加 禁止POST用户 */
char *uident;
{
    char buf2[50],strtosave[256],date[STRLEN]="0";
    int maxdeny;
    /*Haohmaru.99.4.1.auto notify*/
    time_t  now;
    int  id;
    char buffer[STRLEN];
    FILE *fn;
    char filename[STRLEN];
    int day, autofree = 0;

    now=time(0);
    strncpy(date,ctime(&now)+4,7);
    setbfile( genbuf, currboard,"deny_users" );
    if( seek_in_file(genbuf, uident) || !strcmp(currboard, "denypost"))
        return -1;
    if (HAS_PERM(currentuser,PERM_SYSOP)||HAS_PERM(currentuser,PERM_OBOARDS))
        maxdeny=70;
    else
        maxdeny=14;

/*MUST:*/
    *denymsg = 0;
    while(0 == strlen(denymsg)) {
	    getdata(2,0,"输入说明(按*取消): ", denymsg,30,DOECHO,NULL,YEA);
    }
/*	  if (0==strlen(denymsg)) goto MUST;*/
    if (denymsg[0]=='*')
        return 0;
#ifdef DEBUG
    autofree = askyn("该封禁是否自动解封？", YEA);
#else
    if (HAS_PERM(currentuser,PERM_SYSOP)||HAS_PERM(currentuser,PERM_OBOARDS))
        sprintf(filebuf,"输入天数(0-手动解封，最长%d天)",maxdeny);
    else
#endif /*DEBUG*/
        sprintf(filebuf,"输入天数(最长%d天)",maxdeny);
/*MUST1:*/
    denyday = 0;
    while(!denyday) {
        getdata(3,0,filebuf, buf2,4,DOECHO,NULL,YEA);
        if ((buf2[0]<'0')||(buf2[0]>'9')) continue; /*goto MUST1;*/
        denyday=atoi(buf2);
        if ((denyday<0)||(denyday>maxdeny)) denyday = 0; /*goto MUST1;*/
        else if (!(HAS_PERM(currentuser,PERM_SYSOP)||HAS_PERM(currentuser,PERM_OBOARDS))&&!denyday)
            denyday = 0; /*goto MUST1;*/
        else if ((HAS_PERM(currentuser,PERM_SYSOP)||HAS_PERM(currentuser,PERM_OBOARDS)) && !denyday && !autofree)
            break;
    }

#ifdef DEBUG
    if(denyday && autofree) {
#else
    if (denyday) {
#endif /*DEBUG*/
        struct tm* tmtime;
        time_t undenytime=now+denyday*24*60*60;
/*        time_t daytime=now+(day+1)*24*60*60;
        now=time(0);*/
        tmtime=gmtime(&undenytime);

        sprintf( strtosave, "%-12.12s %-30.30s%-12.12s %2d月%2d日解\x1b[%um",
                uident, denymsg ,currentuser->userid,
                tmtime->tm_mon+1,tmtime->tm_mday, undenytime);/*Haohmaru 98,09,25,显示是谁什么时候封的 */
    } else {
        struct tm* tmtime;
#ifdef DEBUG
        time_t undenytime = now+denyday*24*60*60;
        tmtime=gmtime(&undenytime);
        sprintf( strtosave, "%-12.12s %-30.30s%-12.12s %2d月%2d日后手动解封\x1b[%um",
                uident, denymsg, currentuser->userid,
                tmtime->tm_mon+1,tmtime->tm_mday, undenytime);
#else
        now=time(0);
        tmtime=gmtime(&now);
        sprintf( strtosave, "%-12.12s %-30.30s%-12.12s at %2d月%2d日 手动解封",
                uident, denymsg, currentuser->userid,
                tmtime->tm_mon+1,tmtime->tm_mday);
#endif
    }

/*
MUST2:
    getdata(4,0,"输入补充说明: ", buf,60,DOECHO,NULL,YEA);
    if (0==strlen(buf))
	goto MUST2;
    sprintf(deadline,"%-60s",buf);
*/
    return addtofile(genbuf,strtosave);
}

int
deldeny(uident)  /* 删除 禁止POST用户 */
char *uident;
{
    char fn[STRLEN];
    FILE* fn1;
    char filename[STRLEN];
    char buffer[STRLEN];
    time_t now;
    struct userec* lookupuser;

    now=time(0);
    setbfile( fn,currboard, "deny_users" );
    /*Haohmaru.4.1.自动发信通知*/
    sprintf(filename,"etc/%s.dny",currentuser->userid);
    fn1=fopen(filename,"w");
    if (HAS_PERM(currentuser,PERM_SYSOP)||HAS_PERM(currentuser,PERM_OBOARDS))
    {sprintf(buffer,"[通知]");
        fprintf(fn1,"寄信人: %s \n",currentuser->userid) ;
        fprintf(fn1,"标  题: %s\n",buffer) ;
        fprintf(fn1,"发信站: %s (%24.24s)\n","BBS "NAME_BBS_CHINESE"站",ctime(&now)) ;
        fprintf(fn1,"来  源: %s \n",currentuser->lasthost) ;
        fprintf(fn1,"\n");
        fprintf(fn1,"您被站务人员 %s 解除在 %s 板的封禁\n",currentuser->userid,currboard);
    }
    else
    {
        sprintf(buffer,"[通知]",currboard,currentuser->userid);
        fprintf(fn1,"寄信人: %s \n",currentuser->userid) ;
        fprintf(fn1,"标  题: %s\n",buffer) ;
        fprintf(fn1,"发信站: %s (%24.24s)\n","BBS "NAME_BBS_CHINESE"站",ctime(&now)) ;
        fprintf(fn1,"来  源: %s \n",currentuser->lasthost) ;
        fprintf(fn1,"\n");
        fprintf(fn1,"您被 %s 板板主 %s 解除封禁\n",currboard,currentuser->userid);
    }
    fclose(fn1);

    /*解封同样发文到undenypost版  Bigman:2000.6.30*/ 
    getuser(uident,&lookupuser); 
    if (lookupuser==NULL) 
    	sprintf(buffer,"%s 解封死掉的帐号 %s 在 %s ",currentuser->userid,uident,currboard); 
    else {
      if (PERM_BOARDS & lookupuser->userlevel)
    	sprintf(buffer,"%s 解封某板板主 %s 在 %s ",currentuser->userid,lookupuser->userid,currboard); 
      else
        sprintf(buffer,"%s 解封 %s 在 %s",currentuser->userid,lookupuser->userid,currboard);
      mail_file(currentuser->userid,filename,uident,buffer,0);
    }
    postfile(filename,"undenypost",buffer,1);
    unlink(filename);
    return del_from_file(fn,lookupuser?lookupuser->userid:uident);
}

int
deny_user(ent, fileinfo, direct)  /* 禁止POST用户名单 维护主函数*/
int ent ;
struct fileheader *fileinfo ;
char *direct ;
{
    char uident[STRLEN];
    int page=0;
    char ans[10],repbuf[STRLEN];
    int count;
    /*Haohmaru.99.4.1.auto notify*/
    time_t  now;
    int  id;
    char buffer[STRLEN];
    FILE *fn,*fp;
    char filename[STRLEN];
    int  find;/*Haohmaru.99.12.09*/
    char *idindex;/*Haohmaru.99.12.09*/
    int my_flag=0;	/* Bigman. 2001.2.19 */

    /*   static page=0;*//*Haohmaru.12.18*/
    now=time(0);
    if(!HAS_PERM(currentuser,PERM_SYSOP))
        if(!chk_currBM(currBM,currentuser))
        {
            return DONOTHING;
        }

    while (1) {
    	char querybuf[0xff];
Here:
	clear();
        count = listdeny(0);
        if (count>0 && count<20)/*Haohmaru.12.18,看下一屏*/
	    snprintf(querybuf,0xff,"(O)增加%s (A)增加 (D)删除 or (E)离开 [E]: ",fileinfo->owner);
	else if (count>20)
	    snprintf(querybuf, 0xff, "(O)增加%s (A)增加 (D)删除 (N)后面第N屏 or (E)离开 [E]: ",fileinfo->owner);
	else 
	    snprintf(querybuf, 0xff, "(O)增加%s (A)增加 or (E)离开 [E]: ",fileinfo->owner);

        getdata(1,0, querybuf, ans,7,DOECHO,NULL,YEA);
    *ans=(char)toupper((int)*ans);
    
	if (*ans == 'A' || *ans == 'O' ) {
            struct userec saveuser;
            move(1,0);
            if (*ans=='A')
		    	usercomplete("增加无法 POST 的使用者: ", uident);
		    else strncpy(uident, fileinfo->owner, STRLEN-4);
            /*Haohmaru.99.4.1,增加被封ID正确性检查*/
            if(!(id = searchuser(uident)))  /* change getuser -> searchuser , by dong, 1999.10.26 */
            {
                move(3,0) ;
                prints("非法 ID") ;
                clrtoeol() ;
                pressreturn() ;
                goto Here;
            }
            if( *uident != '\0' )
            {   sprintf(filebuf,"%-12s","违反某条站规");
                if(addtodeny(uident)==1)
                {
                	struct userec* lookupuser,*saveptr;
                    sprintf(repbuf,"%s 取消 %s 在 %s 的 POST 权力",
                            currentuser->userid,uident,currboard);
                    report(repbuf);

                    /*Haohmaru.4.1.自动发信通知并发文章于板上*/
                    sprintf(filename,"etc/%s.deny",currentuser->userid);
                    fn=fopen(filename,"w+");
                    memcpy(&saveuser,currentuser,sizeof(struct userec));
                    saveptr = currentuser;
                    currentuser = &saveuser;
                    sprintf(buffer,"%s被取消在%s版的发文权限",uident,currboard);

                    if ((HAS_PERM(currentuser,PERM_SYSOP)||HAS_PERM(currentuser,PERM_OBOARDS)) && !chk_BM_instr(currBM,currentuser->userid))
                    {	   my_flag=0;
                        fprintf(fn,"寄信人: SYSOP (System Operator) \n") ;
                        fprintf(fn,"标  题: %s\n",buffer) ;
                        fprintf(fn,"发信站: %s (%24.24s)\n","BBS "NAME_BBS_CHINESE"站",ctime(&now)) ;
                        fprintf(fn,"来  源: smth.org\n") ;
                        fprintf(fn,"\n");
                        fprintf(fn,"由于您在 \x1b[4m%s\x1b[0m 版 \x1b[4m%s\x1b[0m，我很遗憾地通知您， \n",currboard,denymsg);
                        if (denyday)
                            fprintf(fn,"您被暂时取消在该版的发文权力 \x1b[4m%d\x1b[0m 天，到期后请回复\n",denyday);
                        else
                            fprintf(fn,"您被暂时取消在该版的发文权力，到期后请回复\n");
                        fprintf(fn,"此信申请恢复权限。\n");
                        fprintf(fn,"\n");
                        fprintf(fn,"                            "NAME_BBS_CHINESE NAME_SYSOP_GROUP"值班站务：\x1b[4m%s\x1b[0m\n",currentuser->userid);
                        fprintf(fn,"                              %s\n",ctime(&now));
                        strcpy(currentuser->userid,"SYSOP");
                        strcpy(currentuser->username,NAME_SYSOP);
                        strcpy(currentuser->realname,NAME_SYSOP);
                    }
                    else
                    {		my_flag=1;
                        fprintf(fn,"寄信人: %s \n",currentuser->userid) ;
                        fprintf(fn,"标  题: %s\n",buffer) ;
                        fprintf(fn,"发信站: %s (%24.24s)\n","BBS "NAME_BBS_CHINESE"站",ctime(&now)) ;
                        fprintf(fn,"来  源: %s \n",currentuser->lasthost) ;
                        fprintf(fn,"\n");
                        fprintf(fn,"由于您在 \x1b[4m%s\x1b[0m 版 \x1b[4m%s\x1b[0m，我很遗憾地通知您， \n",currboard,denymsg);
                        if (denyday)
                            fprintf(fn,"您被暂时取消在该版的发文权力 \x1b[4m%d\x1b[0m 天，到期后请回复\n",denyday);
                        else
                            fprintf(fn,"您被暂时取消在该版的发文权力，到期后请回复\n");
                        fprintf(fn,"此信申请恢复权限。\n");
                        fprintf(fn,"\n");
                        fprintf(fn,"                              "NAME_BM":\x1b[4m%s\x1b[0m\n",currentuser->userid);
                        fprintf(fn,"                              %s\n",ctime(&now));
                    }
                    fclose(fn);
                    mail_file(currentuser->userid,filename,uident,buffer,0);
                    fn=fopen(filename,"w+");
                    fprintf(fn,"由于 \x1b[4m%s\x1b[0m 在 \x1b[4m%s\x1b[0m 版的 \x1b[4m%s\x1b[0m 行为，\n",uident,currboard,denymsg);
                    if (denyday)
                        fprintf(fn,"被暂时取消在本版的发文权力 \x1b[4m%d\x1b[0m 天。\n",denyday);
                    else
                        fprintf(fn,"您被暂时取消在该版的发文权力，到期后请回复\n");

                    if (my_flag==0)
                    {
                        fprintf(fn,"                            "NAME_BBS_CHINESE NAME_SYSOP_GROUP"值班站务：\x1b[4m%s\x1b[0m\n",saveptr->userid);
                    }
                    else
                    {
                        fprintf(fn,"                              "NAME_BM":\x1b[4m%s\x1b[0m\n",currentuser->userid);
                    }
                    fprintf(fn,"                              %s\n",ctime(&now));
                    fclose(fn);
                    postfile(filename,currboard,buffer,2);
                    /*	unlink(filename); */
                    currentuser = saveptr;

                    sprintf(buffer,"%s 被 %s 封禁本板POST权",uident,currentuser->userid);
                    getuser(uident,&lookupuser);

                    if(PERM_BOARDS & lookupuser->userlevel)
                        sprintf(buffer,"%s 封某板"NAME_BM" %s 在 %s",currentuser->userid,uident,currboard);
                    else
                        sprintf(buffer,"%s 封 %s 在 %s",currentuser->userid,uident,currboard);
                    postfile(filename,"denypost",buffer,8);
                    unlink(filename);
                }
            }
        } else if ((*ans == 'D' ) && count) {
		int len;
            move(1,0);
            /*           namecomplete("删除无法 POST 的使用者: ", uident);by Haohmaru.99.4.1.这种删除法会误删同字母开头的ID
            	     usercomplete("删除无法 POST 的使用者: ", uident);Haohmaru.faint...这个函数也会出错�,比如封的时候ID叫USAleader,后来ID被删，有人又注册了个usaleader,于是就解不了,大小写引起的，Boy板就出现过这种情况，所以改成下面的。
            	     Haohmaru.99.4.1,增加被解ID正确性检查*/
            /*          if(!(id = searchuser(uident)))  change getuser -> searchuser, by dong, 1999.10.26 */
            sprintf(genbuf,"删除无法 POST 的使用者: ");
            getdata(1, 0, genbuf, uident, 13, DOECHO, NULL,YEA);
            find = 0;/*Haohmaru.99.12.09.原来的代码如果被封者已自杀就删不掉了*/
            setbfile( genbuf, currboard,"deny_users" );
            if ((fp = fopen(genbuf, "r")) == NULL)
            {
                prints("(none)\n");
                return 0;
            }
	    len = strlen(uident);
            while(fgets(genbuf, 256/*STRLEN*/, fp) != NULL)
            {
		if (!strncasecmp(genbuf,uident,len)) {
                  if (genbuf[len] == 32) {
	            strncpy(uident,genbuf,len);
		    uident[len]=0;
		    find=1;
		    break;
		  }
		}
            }
            fclose(fp);
            if(!find)
            {
                move(3,0) ;
                prints("该ID不在封禁名单内!");
                clrtoeol() ;
                pressreturn() ;
                goto Here;
            }
            /*---	add to check if undeny time reached.	by period 2000-09-11	---*/
            {
                char lbuf[256], * lptr;
                time_t ldenytime;
                /* now the corresponding line in genbuf */
                if( NULL != (lptr = strrchr(genbuf, '[')) )
                    sscanf(lptr + 1, "%lu", &ldenytime);
                else ldenytime = now + 1;
                if(ldenytime > now) {
                    move(3, 0);
                    prints(genbuf);
                    if(NA == askyn("该用户封禁时限未到，确认要解封？", NA/*, NA*/))
                        goto Here;	/* I hate Goto!!! */
                }
            }
            /*---		---*/
            move(1,0);
            clrtoeol();
            if (uident[0] != '\0')
            {
                if(deldeny(uident))
                {
                    sprintf(repbuf,"%s 恢复 %s 在 %s 的 POST 权力",
                            currentuser->userid,uident,currboard);
                    report(repbuf);
                }
            }
        }
        else if (count>20 )
        {
            page=*ans-'0';
            if(page<0 || page>10) break;/*不会封人超过10屏吧?那可是200人啊!*/
            listdeny(page);
            pressanykey();
        }
        else  break;
    } /*end of while*/
    clear();
    return FULLUPDATE;
}

#ifndef LEEWARD_X_FILTER
/* Leeward added below 98.04.02 */
int
BoardFilter()
{
    int aborted;
    char ans[7],buf[STRLEN],bufX[STRLEN];
    int ch,num;

    char *e_file[]={".badword", NULL};
    char *explain_file[]={bufX, NULL};

    if(!HAS_PERM(currentuser,PERM_SYSOP) && !HAS_PERM(currentuser,PERM_OBOARDS))
        return DONOTHING;
    if(!check_systempasswd())
        return DONOTHING;

    modify_user_mode( ADMIN );
    clear();
    move(0,0);
    sprintf(bufX, "%s 板自动过滤的词语",currboard);
    prints("编修 %s 版的系统档案\n\n",currboard);
    for(num=0;e_file[num]!=NULL&&explain_file[num]!=NULL;num++)
    {
        prints("[[32m%2d[m] %s%s",num+1,explain_file[num],
               (num + 1 >= 9 && num + 1 <= 16 && (num + 1) % 2) ? "      " : "\n");
        /* Leeward 98.03.29 调整显示布局，以便加入“系统自动过滤的词语”一项 */
    }

    /* 耗子：清除无法区段删除的错误的代码不应该放在处理过滤词汇的函数中，
             否则结构就混乱了(Leeward 99.07.25) */
    /*   prints("[[32m%2d[m] 清除无法区段删除的错误(请确认目前没有人在本板执行区段删除)\n",num+1);*/
    prints("[[32m%2d[m] 都不想改\n",num+1);

    getdata(num+4,0,"你要编修哪一项系统档案: ",ans,3,DOECHO,NULL,YEA);
    ch=atoi(ans);
    if(!isdigit(ans[0])||ch<=0 || ch>num|| ans[0]=='\n'|| ans[0]=='\0')
        return FULLUPDATE;
    ch-=1;
    sprintf(genbuf,"boards/%s/%s",currboard,e_file[ch]);
    move(2,0);
    clrtobot();
    sprintf(buf,"(E)编辑 (D)删除 %s? [E]: ",explain_file[ch]);
    getdata(3,0,buf,ans,2,DOECHO,NULL,YEA);
    if (ans[0] == 'D' || ans[0] == 'd') {
        {
            char        secu[STRLEN];
            sprintf(secu,"删除系统档案：%s",explain_file[ch]);
            securityreport(secu,NULL);
        }
        unlink(genbuf);
        move(5,0);
        prints("%s 已删除\n",explain_file[ch]);
        sprintf(buf,"delete %s",explain_file[ch]);
        report(buf);
        pressreturn();
        return FULLUPDATE;
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
            securityreport(secu,NULL);
        }
    }
    pressreturn();
    return FULLUPDATE;
}
#endif /* LEEWARD_X_FILTER */
