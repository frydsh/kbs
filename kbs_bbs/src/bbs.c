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
/* 所有 的注释 由 Alex&Sissi 添加 ， alex@mars.net.edu.cn */

#include "bbs.h"
#include <time.h>

/*#include "../SMTH2000/cache/cache.h"*/

extern int numofsig;
int scrint = 0 ;
int local_article;
int readpost;
int digestmode;
int usernum ;
char currboard[STRLEN-BM_LEN] ;
char currBM[BM_LEN-1] ;
int selboard = 0 ;

char    ReadPost[STRLEN]="";
char    ReplyPost[STRLEN]="";
int     FFLL=0;
int     Anony;
char genbuf[ 1024 ];
char quote_title[120],quote_board[120];
char quote_file[120], quote_user[120];
struct friends_info *topfriend;
#ifndef NOREPLY
char replytitle[STRLEN];
#endif

char    *filemargin() ;
void    cancelpost();
/*For read.c*/
int     auth_search_down();
int     auth_search_up();
int     t_search_down();
int     t_search_up();
int     post_search_down();
int     post_search_up();
int     thread_up();
int     thread_down();
int     deny_user();
#ifndef LEEWARD_X_FILTER
int     BoardFilter(); /* Leeward 98.04.02 */
#endif
int     show_author();
/*int     b_jury_edit();  stephen 2001.11.1*/
int     add_author_friend();
int     show_authorinfo();/*Haohmaru.98.12.05*/
int	show_authorBM();/* cityhunter 00.10.18 */
int	m_read();/*Haohmaru.2000.2.25*/
int     SR_first_new();
int     SR_last();
int     SR_first();
int     SR_read();
int     SR_readX(); /* Leeward 98.10.03 */
int     SR_author();
int     SR_authorX(); /* Leeward 98.10.03 */
int     SR_BMfunc();
int     SR_BMfuncX(); /* Leeward 98.04.16 */
int	Goodbye();
int i_read_mail(); /* period 2000.11.12 */

void    RemoveAppendedSpace(); /* Leeward 98.02.13 */
int set_delete_mark(int ent,struct fileheader *fileinfo,char *direct ); /* KCN */

extern time_t   login_start_time;
extern char     BoardName[];
extern int      cmpbnames();
extern int 	B_to_b;

extern struct screenline *big_picture;
extern struct userec *user_data;

int totalusers, usercounter;

#ifndef LEEWARD_X_FILTER
int
check_invalid_post(checked, boardname, title) /* Leeward 98.03.29 - 08.05 */
char *checked;              /* 改动本函数必须同步 bbs.c 和 bbssnd.c (4 WWW) */
char *boardname;
char *title;
{
    FILE *fp, *fpX;
    char buf[8192], bufX[8192];
    int  i;
    char checkedX[1024], titleX[1024];

    strcpy(checkedX, checked);
    strcpy(titleX, title);

    sprintf(buf, "tmp/security.%d", getpid());
    if (!strcmp(buf, checkedX))
        return NA; /* 这是系统安全记录，需要保持原样，不返回 YEA (不可过滤) */
    if (!strcmp(boardname, "notepad"))
        return NA; /* 系统留言版, 每篇文章都要发表, 不返回 YEA (不可过滤) */
    if (!strcmp(boardname, "Filter"))
        return NA; /* 系统过滤记录区，允许改动被过滤内容以便放行, 不返回 YEA */
    if (!strcmp(boardname, "sys_discuss"))
        return NA; /* 系统内部讨论区,不用过滤 */

    for (i = 0; i < 1 + sysconf_eval("RUN_SYSTEM_FILTER"); i ++)
    {

        if (0 == i) sprintf(buf, "boards/%s/.badword", boardname);
        else        sprintf(buf, ".badword"); /* 版面检查优先于系统检查 */

        fpX = fopen(buf, "r");
        if (NULL == fpX)
            continue; /* If can't open, not return YEA (no invalid word) */

        fp = fopen(checkedX, "r");
        if (NULL == fp)
        {
            fclose(fpX);
            continue; /* If can't open, not return YEA (no invalid post) */
        }

        while (!feof(fp))
        {
            char *p, *q;

            fgets(buf, 8192, fp); /* Reading a line from the article checked */
            if (feof(fp))
                break;
            else
            { /* 这段代码用来去除汉字间的空格，同时忽略西文大小写，以提高过滤效果 */
#define XRULER "/\\#$^*'\"-~+<>[] =.|%&_`{}" /* Leeward 98.07.24 这些也去除 */
                int f, g;
                char *x;

                for (f = g = 0, q = bufX, p = buf; *p; p ++)
                {
                    if (strchr(XRULER, *p)) /* 除了空格，还有些字符也... */
                    {
                        g ++; /* 空格字符先不复制，仅记录数目和位置 */
                        if (1 == g) x = p;
                        continue;
                    }
                    else
                    { /* 扫描到非空格字符 */
                        if (g)
                        { /* 前面是空格字符 */
                            if ( 0 == f || ( (*(p + 1) < 128) && !strchr(XRULER, *(p + 1)) ) )
                                while (g -- > 0) *q ++ = *x ++; /* 不夹在汉字间的空格，原数复制 */
                        }
                        g = 0;
                        if (*p < 128) *p = toupper(*p), f = 0; else f = 1;
                        /* 忽略大小写 */   /* 记录空格是否接在汉字后 */
                        *q ++ = *p; /* 复制非空格字符 */
                    }
                }
                *q = 0; /* Do not forget this! */
                strcpy(buf, bufX);
            }

            while (!feof(fpX))
            {
                fgets(bufX, 8192, fpX); /* Reading a forbidden keyword (line) */
                if (feof(fpX))
                {
                    rewind(fpX);
                    break;
                }
                else
                {
                    for (p = bufX; ' ' == *p; p ++)
                        ;
                    if (bufX != p) strcpy(bufX, p); /* 忽略前导空格 */
                    for (p = bufX; *p; p ++)
                    {
                        if (*p < 128) *p = toupper(*p); /* 忽略大小写 */
                        if ('\n' == *p) *p = 0; /* 去除换行字符 */
                        if ('\r' == *p) *p = 0; /* 去除回车字符 */
                    }
                    if (0 == bufX[0]) continue; /* 忽略空行 */
                    else RemoveAppendedSpace(bufX); /* 忽略后带空格 */

                    if (p = strstr(buf, bufX))
                    {
                        if ((bufX[0] > 127) && (bufX[1] > 127) && (0 == bufX[2]))
                        { /* 单个汉字作为过滤词语必须校验其位置的合理性 */
                            int ich;
                            for (ich = 0, -- p; p >= buf; p --)
                            { /* 统计该字前面的汉字字节数目 */
                                if (*p < 128) break; else ich ++;
                            }
                            if (ich % 2) continue;
                            /* 必须是偶数字节才合理，否则会错误过滤 */
                        }

                        fclose(fp);
                        fclose(fpX);
                        sprintf(buf, "%s含“%s”的文章被过滤", boardname, bufX);
                        securityreport(buf,NULL);

                        p = strrchr(checkedX, '/');

                        if (p ++)
                        {
                            struct fileheader FilterFile;

                            FilterFile.accessed[0] = FILE_MARKED;
                            strcpy(FilterFile.owner, currentuser->userid);
                            strcpy(FilterFile.title, titleX);
                            sprintf(FilterFile.filename, "%s.%s", p, boardname);
                            sprintf(buf, "/bin/cp -f %s boards/Filter/%s", checkedX, FilterFile.filename);
                            system(buf);
                            sprintf(buf, "boards/Filter/.DIR");
                            append_record(buf, &FilterFile, sizeof(FilterFile));
                        }

                        return YEA; /* An invalid word was found, post is invalid */
                    } /* End if (strstr(buf, bufX)) */
                } /* End if (feof(fpX)) else ... */
            } /* End while (!feof(fpX)) */
        } /* End while (!feof(fp)) */

        fclose(fp);
        fclose(fpX);
    } /* End for (i = 0; i < 2; i ++) */

    return NA; /* Not return YEA (no invalid word at all) */
}

PassFilter(ent,fileinfo,direct)  /* 放行一篇文章 Leeward 98.04.06 */
int ent;
struct fileheader *fileinfo;
char *direct;
{
    char *p, buf[1024];
    struct fileheader FilterFile;

    p = strrchr(fileinfo->filename, '.');
    if(NULL == p ++) return DONOTHING;
    else if (!strcmp(p, "A")) return DONOTHING;

    if ((fileinfo->accessed[0] & FILE_FORWARDED))
    {
        clear();
        move(2,0);
        prints("本文已放行过了\n");
        pressreturn();

        return del_post(ent,fileinfo,direct);
    }

    FilterFile.accessed[0] = 0;
    strcpy(FilterFile.owner, fileinfo->owner);
    strcpy(FilterFile.title, fileinfo->title);
    strcpy(FilterFile.filename, fileinfo->filename);

    sprintf(buf, "/bin/cp -f boards/%s/%s boards/%s/%s",
            currboard, fileinfo->filename, p, FilterFile.filename);
    system(buf);

    sprintf(buf, "boards/%s/.DIR", p);
    append_record(buf, &FilterFile, sizeof(FilterFile));

    fileinfo->accessed[0] |= FILE_FORWARDED;
    fileinfo->accessed[0] &= ~FILE_MARKED;
    substitute_record(direct, fileinfo, sizeof(*fileinfo),ent) ;
    sprintf(buf,"passed %s's “%s” on %s", FilterFile.owner, FilterFile.title, currboard);
    report(buf);

    clear();
    move(2,0);
    prints("'%s' 已放行到 %s 板 \n", fileinfo->title, p);
    pressreturn();

    return del_post(ent,fileinfo,direct);
}

#endif

int
check_readonly(char *checked) /* Leeward 98.03.28 */
{
    struct stat st;
    char        buf[STRLEN];

    if (checkreadonly(checked)) /* Checking if DIR access mode is "555" */
    {
        if (currboard == checked)
        {
            move(0, 0 );
            clrtobot();
            move(8, 0);
            prints("                                        "); /* 40 spaces */
            move(8, (80 - (24 + strlen(checked))) / 2); /* Set text in center */
            prints("[1m[33m很抱歉：[31m%s 版目前是只读模式[33m\n\n                          您不能在该版发表或者修改文章[0m\n", checked);
            pressreturn();
            clear();
        }
        return YEA;
    }
    else
        return NA;
}

/* undelete 一篇文章 Leeward 98.05.18 */
/* modified by ylsdd */
UndeleteArticle(ent,fileinfo,direct)
int ent;
struct fileheader *fileinfo;
char *direct;
{
    char *p, buf[1024];
    char UTitle[128];
    struct fileheader UFile;
    int i;
    FILE *fp;

    if(digestmode!=4&&digestmode!=5) return DONOTHING;
    if(!chk_currBM(currBM,currentuser)) return DONOTHING;

    sprintf(buf, "boards/%s/%s", currboard, fileinfo->filename);
    if(!dashf(buf)) {
       clear();
       move(2,0);
       prints("该文章不存在，已被恢复, 删除或列表出错");
       pressreturn();
       return FULLUPDATE;
    }
    fp = fopen(buf, "r");
    if (!fp) return DONOTHING;


    strcpy(UTitle, fileinfo->title);
    if (p = strrchr(UTitle, '-'))
    { /* create default article title */
      *p = 0;
      for (i = strlen(UTitle) - 1; i >= 0; i --)
      {
        if (UTitle[i] != ' ')
          break;
        else
          UTitle[i] = 0;
      }
    }

    i = 0;
    while (!feof(fp) && i < 2)
    {
      fgets(buf, 1024, fp);
      if (feof(fp))  break;
      if (strstr(buf, "发信人: ") && strstr(buf, "), 信区: "))
      {
        i ++;
      }
      else if (strstr(buf, "标  题: "))
      {
        i ++;
        strcpy(UTitle, buf + 8);
        if (p = strchr(UTitle, '\n'))
          *p = 0;
      }
    }
    fclose(fp);

    bzero(&UFile, sizeof(UFile));
    strcpy(UFile.owner, fileinfo->owner);
    strcpy(UFile.title, UTitle);
    strcpy(UFile.filename, fileinfo->filename);

    sprintf(buf, "boards/%s/.DIR", currboard);
    append_record(buf, &UFile, sizeof(UFile));
	updatelastpost(currboard);
    fileinfo->filename[0]='\0';
    substitute_record(direct, fileinfo, sizeof(*fileinfo),ent) ;
    sprintf(buf,"undeleted %s's “%s” on %s", UFile.owner, UFile.title, currboard);
    report(buf);

    clear();
    move(2,0);
    prints("'%s' 已恢复到板面 \n", UFile.title);
    pressreturn();

    return FULLUPDATE;
}

XArticle(ent,fileinfo,direct)  /* 关于文章的特殊功能 Leeward 98.05.18 */
int ent;
struct fileheader *fileinfo;
char *direct;
{
#ifndef LEEWARD_X_FILTER
    if (!strcmp(currboard, "Filter"))
        return PassFilter(ent,fileinfo,direct);
    else
#endif
    if (digestmode==4||digestmode==5||!strcmp(currboard, "deleted") || !strcmp(currboard, "xdeleted") || !strcmp(currboard, "junk") )
        return UndeleteArticle(ent,fileinfo,direct);
    else
        return DONOTHING;
}

int
check_stuffmode()
{
    if(uinfo.mode==RMAIL)
        return YEA;
    else
        return NA;
}

/*Add by SmallPig*/
void
setqtitle(stitle)   /* 取 Reply 文章后新的 文章title */
char *stitle;
{
    FFLL=1;
    if(strncmp(stitle,"Re: ",4)!=0&&strncmp(stitle,"RE: ",4)!=0)
    {
        sprintf(ReplyPost,"Re: %s",stitle);
        strcpy(ReadPost,stitle);
    }
    else
    {
        strcpy(ReplyPost,stitle);
        strcpy(ReadPost,ReplyPost+4);
    }
}

void
setquotefile(filepath)
char filepath[];
{
    strcpy(quote_file,filepath);
}

char *
setbdir( buf, boardname )  /* 根据阅读模式 取某版 目录路径 */
char *buf, *boardname;
{
    char dir[STRLEN];

    switch(digestmode)
    {
    case NA:
        strcpy(dir,DOT_DIR);
        break;
    case YEA:
        strcpy(dir,DIGEST_DIR);
        break;
    case 2:
        strcpy(dir,THREAD_DIR);
        break;
    case 4:
	strcpy(dir,".DELETED");
	break;
    case 5:
	strcpy(dir,".JUNK");
	break;
    }
    sprintf( buf, "boards/%s/%s", boardname, dir);
    return buf;
}



/*Add by SmallPig*/
void
shownotepad()   /* 显示 notepad */
{
    modify_user_mode( NOTEPAD );
    ansimore("etc/notepad", YEA);
    clear();
    return;
}

/* 时间转换成 中文 */
/*
char *
chtime(clock) 
time_t *clock;
{
    char chinese[STRLEN],week[10],mont[10],date[4],time[10],year[5];
    char *ptr = ctime(clock),*seg,*returndate;

    seg=strtok(ptr," \n\0");
    if(!strcmp(seg,"Sun"))
        strcpy(week,"星期天");
    if(!strcmp(seg,"Mon"))
        strcpy(week,"星期一");
    if(!strcmp(seg,"Tue"))
        strcpy(week,"星期二");
    if(!strcmp(seg,"Wed"))
        strcpy(week,"星期三");
    if(!strcmp(seg,"Thu"))
        strcpy(week,"星期四");
    if(!strcmp(seg,"Fri"))
        strcpy(week,"星期五");
    if(!strcmp(seg,"Sat"))
        strcpy(week,"星期六");

    seg=strtok(NULL," \n\0");
    if(!strcmp(seg,"Jan"))
        strcpy(mont," 1月");
    if(!strcmp(seg,"Feb"))
        strcpy(mont," 2月");
    if(!strcmp(seg,"Mar"))
        strcpy(mont," 3月");
    if(!strcmp(seg,"Apr"))
        strcpy(mont," 4月");
    if(!strcmp(seg,"May"))
        strcpy(mont," 5月");
    if(!strcmp(seg,"Jun"))
        strcpy(mont," 6月");
    if(!strcmp(seg,"Jul"))
        strcpy(mont," 7月");
    if(!strcmp(seg,"Aug"))
        strcpy(mont," 8月");
    if(!strcmp(seg,"Sep"))
        strcpy(mont," 9月");
    if(!strcmp(seg,"Oct"))
        strcpy(mont,"10月");
    if(!strcmp(seg,"Nov"))
        strcpy(mont,"11月");
    if(!strcmp(seg,"Dec"))
        strcpy(mont,"12月");
    strcpy(date,strtok(NULL," \n\0"));
    strcpy(time,strtok(NULL," \n\0"));
    strcpy(year,strtok(NULL," \n\0"));

    sprintf(chinese,"  %4.4d年%4.4s%2.2s日 %s %s",atoi(year),mont,date,week,time);
    strncpy(ptr,chinese,strlen(chinese));
    return (ptr);

}
*/


void
printutitle()  /* 屏幕显示 用户列表 title */
{
    /*---	modified by period	2000-11-02	hide posts/logins	---*/
    int isadm;
    const char *fmtadm = "#上站 #文章", *fmtcom = "           ";
    isadm = HAS_PERM(currentuser,PERM_ADMINMENU);

    move(2,0) ;
    prints(
#ifdef _DETAIL_UINFO_
        "[44m 编 号  使用者代号     %-19s  #上站 #文章 %4s    最近光临日期   [m\n",
#else
        "[44m 编 号  使用者代号     %-19s  %11s %4s    最近光临日期   [m\n",
#endif
#if defined(ACTS_REALNAMES)
        "真实姓名",
#else
        "使用者昵称",
#endif
#ifndef _DETAIL_UINFO_
        isadm? fmtadm:fmtcom,
#endif
        "身份" ) ;
}


int
g_board_names(fhdrp)
struct boardheader *fhdrp ;
{
    if ((fhdrp->level & PERM_POSTMASK) || HAS_PERM(currentuser,fhdrp->level)
            ||(fhdrp->level & PERM_NOZAP))
    {
        AddNameList(fhdrp->filename) ;
    }
    return 0 ;
}

void
make_blist()  /* 所有版 版名 列表 */
{
    CreateNameList() ;
    apply_boards(g_board_names) ;
}

int
Select()
{
    modify_user_mode( SELECT );
    do_select( 0, NULL, genbuf );
    return 0 ;
}

int
Post()  /* 主菜单内的 在当前版 POST 文章 */
{
    if(!selboard) {
        prints("\n\n先用 (S)elect 去选择一个讨论区。\n") ;
        pressreturn() ;  /* 等待按return键 */
        clear() ;
        return 0 ;
    }
#ifndef NOREPLY
    *replytitle = '\0';
#endif
    do_post();
    return 0 ;
}

int
postfile(filename,nboard,posttitle,mode)  /* 将某文件 POST 在某版 */
char *filename,*nboard,*posttitle;
int mode;
{
    char dbname[STRLEN];
    if(getboardnum(nboard,NULL) <= 0)
    {  /* 搜索要POST的版 ,判断是否存在该版 */
        sprintf(dbname,"%s 讨论区找不到",nboard);
        report(dbname);
        return -1;
    }
    in_mail = NA ;   /* 在quote_board,quote_file,quote_title填入要POST的文章参数，然后POST */
    strcpy(quote_board,nboard);
    strcpy(dbname,currboard); /* 保存当前版 */
    strcpy(currboard,nboard);
    strcpy(quote_file, filename);
    strcpy(quote_title,posttitle);
    post_cross('l',mode);  /* post 文件 */
    strcpy(currboard,dbname); /* 恢复保存的当前版 */
    return 0;
}

int
get_a_boardname(bname,prompt)  /* 输入一个版名 */
char *bname,*prompt;
{
    /*    struct boardheader fh;*/

    make_blist();
    namecomplete(prompt,bname); /* 可以自动搜索 */
    if (*bname == '\0') {
        return 0;
    }
    /*---	Modified by period	2000-10-29	---*/
    if(getbnum(bname) <= 0)
        /*---	---*/
    {
        move(1,0);
        prints("错误的讨论区名称\n");
        pressreturn();
        move(1,0);
        return 0;
    }
    return 1;
}

/* Add by SmallPig */
int
do_cross(ent,fileinfo,direct)  /* 转贴 一篇 文章 */
int ent;
struct fileheader *fileinfo;
char *direct;
{
    char bname[STRLEN];
    char dbname[STRLEN];
    char ispost[10];

    if (!HAS_PERM(currentuser,PERM_POST)) /* 判断是否有POST权 */
    {return DONOTHING;}

    if ((fileinfo->accessed[0] & FILE_FORWARDED) && !HAS_PERM(currentuser,PERM_SYSOP))
    {
        clear();
        move(1,0);
        prints("本文章已经转贴过一次，无法再次转贴");
        move(2,0);
        pressreturn();
        return FULLUPDATE;
    }

    if(uinfo.mode!=RMAIL)
        sprintf(genbuf,"boards/%s/%s",currboard,fileinfo->filename) ;
    else
        setmailfile(genbuf,currentuser->userid,fileinfo->filename) ;
    strcpy( quote_file, genbuf );
    strcpy(quote_title,fileinfo->title);

    clear();
    move(4, 0); /* Leeward 98.02.25 */
    prints("[1m[33m请注意：[31m本站站规规定：同样内容的文章严禁在 5 (含) 个以上讨论区内重复张贴。\n\n违反者[33m除所贴文章会被删除之外，还将被[31m剥夺继续发表文章的权力。[33m详细规定请参照：\n\n    Announce 版的站规：“关于转贴和张贴文章的规定”。\n\n请大家共同维护 BBS 的环境，节省系统资源。谢谢合作。\n\n[0m");
    if(!get_a_boardname(bname,"请输入要转贴的讨论区名称: "))
    {
        return FULLUPDATE;
    }
    /* if (!strcmp(bname,currboard))Haohmaru,98.10.03*/
    if( !strcmp(bname,currboard) && (uinfo.mode !=RMAIL) )
    {
        move( 3, 0 );
        clrtobot();
        prints("\n\n                          本板的文章不需要转贴到本板!");
        pressreturn();
        clear();
        return FULLUPDATE;
    }
    { /* Leeward 98.01.13 检查转贴者在其欲转到的版面是否被禁止了 POST 权 */
        char szTemp[STRLEN];

        strcpy(szTemp, currboard); /* 保存当前版面 */
        strcpy(currboard, bname);  /* 设置当前版面为要转贴到的版面 */
        if(deny_me(currentuser->userid,currboard)&&!HAS_PERM(currentuser,PERM_SYSOP))     /* 版主禁止POST 检查 */
        {
            move( 3, 0 );
            clrtobot();
            prints("\n\n                很抱歉，你在该版被其版主停止了 POST 的权力...\n");
            pressreturn();
            clear();
            strcpy(currboard, szTemp); /* 恢复当前版面 */
            return FULLUPDATE;
        }
        else if (YEA == check_readonly(currboard)) /* Leeward 98.03.28 */
        {
            strcpy(currboard, szTemp); /* 恢复当前版面 */
            return FULLUPDATE;
        }
        else
            strcpy(currboard, szTemp); /* 恢复当前版面 */
    }

    move(0,0);
    clrtoeol();
    prints("转贴 ' %s ' 到 %s 板 ",quote_title,bname);
    move(1,0);
    getdata(1,0,"(S)转信 (L)本站 (A)取消? [A]: ",ispost, 9, DOECHO, NULL,YEA);
    if(ispost[0]=='s'||ispost[0]=='S'||ispost[0]=='L'||ispost[0]=='l')
    {
        strcpy(quote_board,currboard);
        strcpy(dbname,currboard);
        strcpy(currboard,bname);
        if(post_cross(ispost[0],0)==-1) /* 转贴 */
        {
            pressreturn();
            move(2,0);
            strcpy(currboard,dbname);
            return FULLUPDATE;
        }
        strcpy(currboard,dbname);
        move(2,0);
        prints("' %s ' 已转贴到 %s 板 \n",quote_title,bname);
        fileinfo->accessed[0] |= FILE_FORWARDED;  /*added by alex, 96.10.3 */
        substitute_record(direct, fileinfo, sizeof(*fileinfo),ent) ;
    }
    else
    {
        prints("取消");
    }
    move(2,0);
    pressreturn();
    return FULLUPDATE;
}


void
readtitle()  /* 版内 显示文章列表 的 title */
{
    struct boardheader    *bp;
    char        header[ STRLEN ], title[ STRLEN ];
    char        readmode[10];
    int chkmailflag=0;

    bp = getbcache( currboard );
    memcpy( currBM, bp->BM, BM_LEN -1);
    if( currBM[0] == '\0' || currBM[0] == ' ' ) {
        strcpy( header, "诚征板主中" );
    } else {
        sprintf( header, "板主: %s", currBM );
    }
    chkmailflag=chkmail();
    if(chkmailflag==2)/*Haohmaru.99.4.4.对收信也加限制*/
        strcpy( title,"[您的信箱超过容量,不能再收信!]");
    else if ( chkmailflag ) /* 信件检查 */
        strcpy( title, "[您有信件]" );
    else if ( (bp->flag&VOTE_FLAG))  /* 投票检查 */
        sprintf( title, "投票中，按 V 进入投票");
    else
        strcpy( title, bp->title+13 );

    showtitle( header, title );  /* 显示 第一行 */
    prints("离开[←,e] 选择[↑,↓] 阅读[→,r] 发表文章[Ctrl-P] 砍信[d] 备忘录[TAB] 求助[h][m\n" );
    if(digestmode==0)  /* 阅读模式 */
        strcpy(readmode,"一般");
    else if(digestmode==1)
        strcpy(readmode,"文摘");
    else if(digestmode==2)
        strcpy(readmode,"主题");
    else if(digestmode==4)
	strcpy(readmode,"回收");
    else if(digestmode==5)
	strcpy(readmode,"纸娄");

    prints("[37m[44m 编号   %-12s %6s %-40s[%4s模式] [m\n", "刊 登 者", "日  期", " 文章标题",readmode) ;
    clrtobot();
}

char *
readdoent(char* buf,int num,struct fileheader* ent)  /* 在文章列表中 显示 一篇文章标题 */
{
    time_t      filetime;
    char        date[20];
    char        *TITLE;
    int         type;
    int         coun;
    int manager;
    char cUnreadMark=(DEFINE(currentuser,DEF_UNREADMARK)?'*':'N');
    char* typeprefix;
    char* typesufix;
    typesufix = typeprefix = "";

	manager = (HAS_PERM(currentuser,PERM_OBOARDS)||(chk_currBM(currBM,currentuser))) ;

    type = brc_unread_t(FILENAME2POSTTIME( ent->filename )) ? cUnreadMark : ' ';
    if ((ent->accessed[0] & FILE_DIGEST) /*&& HAS_PERM(currentuser,PERM_MARKPOST)*/)
    {  /* 文摘模式 判断 */
        if (type == ' ')
            type = 'g';
        else
            type = 'G';
    }
    if(ent->accessed[0] & FILE_MARKED) /* 如果文件被mark住了，改变标识 */
    {
        switch(type)
        {
        case ' ':
            type='m';
            break;
        case '*':
        case 'N':
            type='M';
            break;
        case 'g':
            type='b';
            break;
        case 'G':
            type='B';
            break;
        }
    }
    /*    if(HAS_PERM(currentuser,PERM_OBOARDS) && ent->accessed[1] & FILE_READ) *//*板务总管以上的能看不可re标志,Haohmaru.99.6.7*/
    if (manager & ent->accessed[1] & FILE_READ) /* 版主以上能看不可re标志, Bigman.2001.2.27 */
    {
        switch(type)
        {
        case 'g':
#ifdef _DEBUG_
            type='o'; break;
#endif /* _DEBUG_ */
        case 'G':
            type='O';
            break;
        case 'm':
#ifdef _DEBUG_
            type='u'; break;
#endif /* _DEBUG_ */
        case 'M':
            type='U';
            break;
        case 'b':
#ifdef _DEBUG_
            type = 'd'; break;
#endif /* _DEBUG_ */
        case 'B':
            type='8';
            break;
        case ' ':
#ifdef _DEBUG_
            type=','; break;
#endif /* _DEBUG_ */
        case '*':
        case 'N':
        default:
            type=';';
            break;
        }
    }
    else  if(HAS_PERM(currentuser,PERM_OBOARDS) && ent->accessed[0] & FILE_SIGN)
        /*板务总管以上的能看Sign标志, Bigman: 2000.8.12*/
    {
        type='#';
    }

    if(manager && ent->accessed[1] & FILE_DEL) /* 如果文件被mark delete住了，显示X*/
    {
        type = 'X';
    }

    if (manager&&(ent->accessed[0]&FILE_IMPORTED)) /* 文件已经被收入精华区 */
   	{
           if (type==' ') {
   		typeprefix="\x1b[42m";
   		typesufix="\x1b[m";
           } else {
   		typeprefix="\x1b[32m";
   		typesufix="\x1b[m";
           }
   	}
    filetime = atoi( ent->filename + 2 ); /* 由文件名取得时间 */
    if( filetime > 740000000 ) {
        /* add by KCN
                char* datestr = ctime( &filetime ) + 4;
        */
        strncpy(date,ctime(&filetime)+4,6);
        /*
                strcpy(date,"[0m[m      [0m");
                strncpy(date+7,datestr,6);
        */
        /*
                date[5]='1'+(atoi(datestr+4)%7);
                if (date[5]=='2') date[5]='7';
        */
    }
    /*        date = ctime( &filetime ) + 4;   时间 -> 英文 */
    else
        /* date = ""; char *类型变量, 可能错误, modified by dong, 1998.9.19 */
        /*        { date = ctime( &filetime ) + 4; date = ""; } */
        date[0]=0;

    /*  Re-Write By Excellent */

    TITLE=ent->title;  /*文章标题TITLE*/

    if(FFLL==0)
    {
        if (!strncmp("Re:",ent->title,3) || !strncmp("RE:",ent->title,3) || !strncmp("├ ",ent->title,3) || !strncmp("└ ",ent->title,3)) /*Re的文章*/
            sprintf(buf," %4d %s%c%s %-12.12s %6.6s  %-47.47s ", num, typeprefix, type, typesufix, ent->owner, date, TITLE);
        else   /* 非Re的文章 */
            sprintf(buf," %4d %s%c%s %-12.12s %6.6s  ● %-44.44s ",num, typeprefix, type, typesufix, ent->owner,date,TITLE);
    }
    else /* 允许 相同主题标识 */
    {
        if (!strncmp("Re:",ent->title,3) || !strncmp("RE:",ent->title,3))       /*Re的文章*/
        {
            if(!strcmp( ReplyPost+3, ent->title+3)) /* 当前阅读主题 标识 */
                sprintf(buf," [36m%4d[m %s%c%s %-12.12s %6.6s[36m．%-47.47s[m ", num, typeprefix, type, typesufix, ent->owner, date, TITLE);
            else
                sprintf(buf," %4d %s%c%s %-12.12s %6.6s  %-47.47s", num, typeprefix, type, typesufix, ent->owner, date, TITLE) ;
        }
        else if (!strncmp("├ ",ent->title,3) || !strncmp("└ ",ent->title,3)) /* 主题排列的文章*/
        {
            if(strcmp( ReplyPost+4, ent->title+3)==0) /* 当前阅读主题 标识 */
                sprintf(buf," [36m%4d[m %s%c%s %-12.12s %6.6s[36m．%-47.47s[m", num, typeprefix, type, typesufix,
                        ent->owner, date, TITLE) ;
            else
                sprintf(buf," %4d %s%c%s %-12.12s %6.6s  %-47.47s", num, typeprefix, type, typesufix,
                        ent->owner, date, TITLE) ;
        }
        else
        {
            if(strcmp(ReadPost,ent->title)==0)  /* 当前阅读主题 标识 */
                sprintf(buf," [33m%4d[m %s%c%s %-12.12s %6.6s[33m．● %-44.44s[m ",num,typeprefix, type, typesufix,
                        ent->owner,date,TITLE);
            else
                sprintf(buf," %4d %s%c%s %-12.12s %6.6s  ● %-44.44s ",num,typeprefix, type, typesufix,
                        ent->owner,date,TITLE);
        }
    }
    return buf ;
}

char currfile[STRLEN] ;

int
cmpfilename(fhdr)  /* 比较 某文件名是否和 当前文件 相同 */
struct fileheader *fhdr ;
{
    if(!strncmp(fhdr->filename,currfile,STRLEN))
        return 1 ;
    return 0 ;
}

int
cmpname(fhdr,name)  /* Haohmaru.99.3.30.比较 某文件名是否和 当前文件 相同 */
struct fileheader *fhdr ;
char  name[STRLEN];
{
    if(!strncmp(fhdr->filename,name,STRLEN))
        return 1 ;
    return 0 ;
}

int
cpyfilename(fhdr)  /* 改变删除后的文件名 */
struct fileheader *fhdr ;
{
    char        buf[ STRLEN ];

    sprintf( buf, "-%s", fhdr->owner );
    strncpy( fhdr->owner, buf, IDLEN );
    sprintf( buf, "<< 文章已被 %s 所删除 >>", currentuser->userid );
    strncpy( fhdr->title, buf, STRLEN );
    fhdr->filename[ STRLEN - 1 ] = 'L';
    fhdr->filename[ STRLEN - 2 ] = 'L';
    return 0;
}
int
add_author_friend(ent,fileinfo,direct)
int ent ;
struct fileheader *fileinfo ;
char *direct ;
{
    if(!strcmp("guest",currentuser->userid))
        return DONOTHING;;

    if(!strcmp(fileinfo->owner,"Anonymous")||!strcmp(fileinfo->owner,"deliver"))
        return DONOTHING;
    else
    {
        clear();
        addtooverride(fileinfo->owner);
    }
    return FULLUPDATE;
}

int
read_post(ent,fileinfo,direct)
int ent ;
struct fileheader *fileinfo ;
char *direct ;
{
    char *t ;
    char buf[512];
    int  ch;
    int cou;

    int fd;/*Haohmaru*/
    struct one_key  read_comms[] = {
                                       'r',        read_post,
                                       '\0',	    NULL
                                   };
    char counterfile[STRLEN],chen;

    clear() ;
    strcpy(buf,direct) ;
    if( (t = strrchr(buf,'/')) != NULL )
        *t = '\0' ;
    sprintf(genbuf,"%s/%s",buf,fileinfo->filename) ;
    strcpy( quote_file, genbuf );
    strcpy( quote_board, currboard );
    strcpy(quote_title,fileinfo->title);
    quote_file[119] = fileinfo->filename[STRLEN-2];
    strcpy( quote_user, fileinfo->owner );
    /****** 如果未读，则计数加1，回写.DIR文件 ******/
    /*if (HAS_PERM(currentuser,PERM_POST) && brc_unread(fileinfo->filename))
      {
        fileinfo->ldReadCount++;
        substitute_record(direct, fileinfo, sizeof(*fileinfo), ent); 
       }*/
    /****** Luzi add in 99/01/13 ******************/

    /*Haohmaru.99.11.27.以下代码用于统计文章总阅读次数
    sprintf(counterfile,"/home0/bbs/Haohmaru/counter/%s.counter",currentuser->userid);
    if( (fd = open(counterfile,O_WRONLY|O_CREAT,0664)) == -1 )
    	return(-1);
    if( lseek(fd,sizeof(char),SEEK_END) == -1 )
{
    	close(fd);
    	return(-1);
}
    ch = 'K';
    write(fd,ch,sizeof(char));
    close(fd);*/

#ifndef NOREPLY
    ch = ansimore(genbuf,NA) ;  /* 显示文章内容 */
#else
    ch = ansimore(genbuf,YEA) ; /* 显示文章内容 */
#endif
    brc_add_read( fileinfo->filename ) ;
#ifndef NOREPLY
    move(t_lines-1, 0);
    clrtoeol();  /* 清屏到行尾 */
    if (haspostperm(currentuser,currboard)) {  /* 根据是否有POST权 显示最下一行 */
        prints("[44m[31m[阅读文章] [33m 回信 R │ 结束 Q,← │上一封 ↑│下一封 <Space>,↓│主题阅读 ^X或p [m");
    } else {
        prints("[44m[31m[阅读文章]  [33m结束 Q,← │上一封 ↑│下一封 <Space>,<Enter>,↓│主题阅读 ^X 或 p [m");
    }

    FFLL=1; /* ReplyPost中为Reply后的文章名，ReadPost为去掉Re:的文章名 */
    if(!strncmp(fileinfo->title,"Re:",3))
    {
        strcpy(ReplyPost,fileinfo->title);
        for(cou=0;cou<STRLEN;cou++)
            ReadPost[cou]=ReplyPost[cou+4];
    }
    else if (!strncmp(fileinfo->title,"├ ",3) || !strncmp(fileinfo->title,"└ ",3))
    {
        strcpy(ReplyPost,"Re: ");
        strncat(ReplyPost,fileinfo->title + 3,STRLEN-4);
        for(cou=0;cou<STRLEN;cou++)
            ReadPost[cou]=ReplyPost[cou+4];
    }
    else
    {
        strcpy(ReplyPost,"Re: ");
        strncat(ReplyPost,fileinfo->title,STRLEN-4);
        strcpy(ReadPost,fileinfo->title);
    }

    refresh();
    /* sleep(1);*/  /* ????? */
    if (!( ch == KEY_RIGHT || ch == KEY_UP || ch == KEY_PGUP ))
        ch = egetch();

    switch( ch ) {
    case Ctrl('Z'): r_lastmsg(); /* Leeward 98.07.30 support msgX */
        break;
case 'N': case 'Q':
case 'n': case 'q': case KEY_LEFT:
        break;
    case ' ':
case 'j': case KEY_RIGHT: case KEY_DOWN: case KEY_PGDN:
        return READ_NEXT;
case KEY_UP: case KEY_PGUP:
        return READ_PREV;
case 'Y' : case 'R':
case 'y' : case 'r':
        if((!strcmp(currboard,"News"))||(!strcmp(currboard,"Original")))
        {
            clear();
            move( 3, 0 );
            clrtobot();
            prints("\n\n                    很抱歉，该版仅能发表文章,不能回文章...\n");
            pressreturn();
            break;/*Haohmaru.98.12.19,不能回文章的板*/
        }
        if(fileinfo->accessed[1] & FILE_READ)/*Haohmaru.99.01.01.文章不可re*/
        {
            clear();
            move( 3, 0 );
            prints("\n\n            很抱歉，本文已经设置为不可re模式,请不要试图讨论本文...\n");
            pressreturn();
            break;
        }
        do_reply(fileinfo->title);
        break;
    case Ctrl('R'):
                    post_reply( ent, fileinfo, direct ); /* 回文章 */
        break;
    case 'g':
        digest_post( ent, fileinfo, direct ); /* 文摘模式 */
        break;
    case 'M':
        mark_post( ent, fileinfo, direct ); /* Leeward 99.03.02 */
        break;
    case Ctrl('U'):
                    sread(0,1,NULL/*ent*/,1,fileinfo); /* Leeward 98.10.03 */
        break;
    case Ctrl('H'):
                    sread(-1003,1,NULL/*ent*/,1,fileinfo);
        break;
    case Ctrl('N'):
                    sread(2,0,ent,0,fileinfo);
        sread(3,0,ent,0,fileinfo);
        sread(0,1,ent,0,fileinfo);
        break;
    case Ctrl('S'):case 'p':/*Add by SmallPig*/
        sread(0,0,ent,0,fileinfo);
        break;
    case Ctrl('X'): /* Leeward 98.10.03 */
                    sread(-1003,0,ent,0,fileinfo);
        break;
    case Ctrl('Q'):/*Haohmaru.98.12.05,系统管理员直接查作者资料*/
                    clear();
        show_authorinfo(0,fileinfo,'\0');
        return READ_NEXT;
        break;
    case Ctrl('W'):/*cityhunter 00.10.18察看版主信息 */
                    clear();
        show_authorBM(0,fileinfo,'\0');
        return READ_NEXT;
        break;
    case Ctrl('O'):
                    clear();
        add_author_friend(0,fileinfo,'\0');
        return READ_NEXT;
case 'Z':case 'z':
        if (!HAS_PERM(currentuser,PERM_PAGE)) break;
        sendmsgtoauthor(0,fileinfo,'\0');
        return READ_NEXT;
        break;
    case Ctrl('A'):/*Add by SmallPig*/
                    clear();
        show_author(0,fileinfo,'\0');
        return READ_NEXT;
        break;
case 'L': case 'l':         /* Luzi 1997.11.1 */
        if(uinfo.mode!=LOOKMSGS)
{
            show_allmsgs();
            break;
        }
        else
            return DONOTHING;
    case '!':/*Haohmaru 98.09.24*/
        Goodbye();
        break;
    case 'H':                   /* Luzi 1997.11.1 */
        r_lastmsg();
        break;
    case 'w':                   /* Luzi 1997.11.1 */
        if (!HAS_PERM(currentuser,PERM_PAGE)) break;
        s_msg();
        break;
case 'O': case 'o':         /* Luzi 1997.11.1 */
        if (!HAS_PERM(currentuser,PERM_BASIC)) break;
        t_friends();
        break;
    case 'u':		    /* Haohmaru 1999.11.28*/
        clear();
        modify_user_mode(QUERY);
        t_query();
        break;
    }
#endif
    return FULLUPDATE ;
}

int
skip_post( ent, fileinfo, direct )
int ent ;
struct fileheader *fileinfo ;
char *direct ;
{
    brc_add_read( fileinfo->filename ) ;
    return GOTO_NEXT;
}

int
do_select( ent, fileinfo, direct )  /* 输入讨论区名 选择讨论区 */
int ent ;
struct fileheader *fileinfo ;
char *direct ;
{
    char bname[STRLEN], bpath[ STRLEN ];
    struct stat st ;

    move(0,0) ;
    clrtoeol();
    prints("选择一个讨论区 (英文字母大小写皆可)\n") ;
    prints("输入讨论区名 (按空白键自动搜寻): ") ;
    clrtoeol() ;

    make_blist() ;  /* 生成所有Board名 列表 */
    namecomplete((char *)NULL,bname) ; /* 提示输入 board 名 */
    setbpath( bpath, bname );
    if((*bname == '\0') || (stat(bpath,&st) == -1)) { /* 判断board是否存在 */
        move(2,0);
        prints("不正确的讨论区.\n");
        pressreturn();
        return FULLUPDATE ;
    }
    if(!(st.st_mode & S_IFDIR)) {
        move(2,0) ;
        prints("不正确的讨论区.\n") ;
        pressreturn() ;
        return FULLUPDATE ;
    }

    selboard = 1;
    brc_initial(currentuser->userid, bname );

    move(0,0);
    clrtoeol();
    move(1,0);
    clrtoeol();
    if(digestmode!=NA&&digestmode!=YEA)
	digestmode=NA;
    setbdir( direct, currboard ); /* direct 设定 为 当前board目录 */
    return NEWDIRECT ;
}

int
digest_mode()  /* 文摘模式 切换 */
{
    extern  char  currdirect[ STRLEN ];

    if(digestmode==YEA)
    {
        digestmode=NA;
        setbdir(currdirect,currboard);
    }
    else
    {
        digestmode=YEA;
        setbdir(currdirect,currboard);
        if(!dashf(currdirect))
        {
            digestmode=NA;
            setbdir(currdirect,currboard);
            return DONOTHING;
        }
    }
    return NEWDIRECT ;
}

/*stephen : check whether current useris in the list of "jury" 2001.11.1*/
int
isJury()
{
    char buf[STRLEN];
    if(!HAS_PERM(currentuser,PERM_JURY)) return 0;
    makevdir(currboard);
    setvfile(buf,currboard,"jury");
    return seek_in_file(buf,currentuser->userid);
}

int
deleted_mode()
{
    extern  char  currdirect[ STRLEN ];
/* Allow user in file "jury" to see deleted area. stephen 2001.11.1 */
  if (!chk_currBM(currBM,currentuser) && !isJury()) {
    return DONOTHING;
  }
  if(digestmode==4)
  {
    digestmode=NA;
    setbdir(currdirect,currboard);
  }   
  else
  {
    digestmode=4;
    setbdir(currdirect,currboard);
    if(!dashf(currdirect))
    {
            digestmode=NA;
            setbdir(currdirect,currboard);
            return DONOTHING;
    }
  }
    return NEWDIRECT ;
}

int
junk_mode()
{
   extern  char  currdirect[ STRLEN ];

  if (!HAS_PERM(currentuser,PERM_SYSOP)) {
      return DONOTHING;
  }

  if(digestmode==5)
  {
    digestmode=NA;
    setbdir(currdirect,currboard);
  }   
  else
  {
    digestmode=5;
    setbdir(currdirect,currboard);
    if(!dashf(currdirect))
    {
            digestmode=NA;
            setbdir(currdirect,currboard);
            return DONOTHING;
    }
  }
    return NEWDIRECT ;
}  

int
do_thread()
{
    char buf[STRLEN];

    sprintf(buf,"Threading %s",currboard);
    report(buf);
    move(t_lines-1,0);
    clrtoeol();
    prints("[5m请稍候，系统处理标题中...[m\n");
    refresh();
    /*sprintf(buf,"bin/thread %s&",currboard);*/
    sprintf(buf,"bin/thread %s",currboard);
    system(buf);
}

int
thread_mode()
{
    extern  char  currdirect[ STRLEN ];
    struct stat st;

    /* add by Bigman to verrify the thread */
    char buf[STRLEN],ch[4];
    /* add by KCN if  heavyload can't thread */
    if (!stat("heavyload",&st)) {
        move(t_lines-1,0);
        clrtoeol();
        prints("系统负担过重，暂时不能响应主题阅读的请求...");
        refresh();
        pressanykey();
        return FULLUPDATE;
    }

    if(digestmode==2)
    {
        digestmode=NA;
        setbdir(currdirect,currboard);
    }
    else
    {
        /* 增加判断 Bigman 2000.12.26 */
        saveline(t_lines-2, 0, NULL);
        move(t_lines-2, 0);
        clrtoeol();
        strcpy(buf,"确定同主题阅读? (Y/N) [N]:");
        getdata(t_lines-2, 0,buf,ch,3,DOECHO,NULL,YEA);

        if(ch[0]=='y' || ch[0]=='Y')
        {
            digestmode=2;
            setbdir(currdirect,currboard);
            do_thread();
            /*  while(!dashf(currdirect))
                {
                sleep(1);
                if(++i==5)
                break;
                } */
            if(!dashf(currdirect))
            {
                digestmode=NA;
                setbdir(currdirect,currboard);
                return PARTUPDATE;
            }
        }
        else
        {
            saveline(t_lines-2, 1, NULL);
            return DONOTHING;
        }
    }
    return NEWDIRECT ;
}

int
dele_digest(dname,direc)         /* 删除文摘内一篇POST, dname=post文件名,direc=文摘目录名 */
char *dname;
char *direc;
{
    char digest_name[STRLEN];
    char new_dir[STRLEN];
    char buf[STRLEN];
    char *ptr;
    struct fileheader fh;
    int  pos;

    strcpy(digest_name,dname);
    strcpy(new_dir,direc);

    digest_name[0]='G';
    ptr = strrchr(new_dir, '/') + 1;
    strcpy(ptr, DIGEST_DIR);
    strcpy(buf,currfile);
    strcpy(currfile,digest_name);
    pos=search_record(new_dir, &fh, sizeof(fh), cmpname, digest_name); /* 文摘目录下 .DIR中 搜索 该POST */
    if(pos<=0)
    {
        return;
    }
    delete_file(new_dir,sizeof(struct fileheader),pos,cmpfilename);
    strcpy(currfile,buf);
    *ptr='\0';
    sprintf(buf,"%s%s",new_dir,digest_name);
    unlink(buf);
}

int
digest_post(ent, fhdr, direct)
int ent;
struct fileheader *fhdr;
char *direct;
{

    if(!chk_currBM(currBM,currentuser))       /* 权力检查 */
    {
        return DONOTHING ;
    }
    if (digestmode==YEA||digestmode==4||digestmode==5)      /* 文摘模式内 不能 添加文摘, 回收和纸篓模式也不能 */
        return DONOTHING;

    if (fhdr->accessed[0] & FILE_DIGEST)  /* 如果已经是文摘的话，则从文摘中删除该post */
    {
        fhdr->accessed[0]  = (fhdr->accessed[0] & ~FILE_DIGEST);
        dele_digest(fhdr->filename,direct);
    }
    else
    {
        struct fileheader digest;
        char *ptr, buf[64];

        memcpy(&digest, fhdr, sizeof(digest));
        digest.filename[0]='G';
        strcpy(buf, direct);
        ptr = strrchr(buf, '/') + 1;
        ptr[0] = '\0';
        sprintf(genbuf, "%s%s", buf, digest.filename);
        if (dashf(genbuf))
        {
            fhdr->accessed[0] = fhdr->accessed[0] | FILE_DIGEST;
            substitute_record(direct, fhdr, sizeof(*fhdr), ent);
            return PARTUPDATE;
        }
        digest.accessed[0] = 0;
        sprintf( &genbuf[512], "%s%s", buf, fhdr->filename);
        link(&genbuf[512], genbuf);
        strcpy(ptr, DIGEST_DIR);
        if( get_num_records(buf,sizeof(digest) )>MAX_DIGEST)
        {
            move(3,0);
            clrtobot();
            move(4,10);
            prints("抱歉，你的文摘文章已经超过 %d 篇，无法再加入...\n",MAX_DIGEST);
            pressanykey();
            return PARTUPDATE;
        }
        append_record(buf, &digest, sizeof(digest));  /* 文摘目录下添加 .DIR */
        fhdr->accessed[0] = fhdr->accessed[0] | FILE_DIGEST;
    }
    substitute_record(direct, fhdr, sizeof(*fhdr), ent);  /* 版目录下 .DIR 改变 (添加了g标志) */
    return PARTUPDATE;
}

#ifndef NOREPLY
int
do_reply(title)         /* reply POST */
char *title;
{
    strcpy(replytitle, title);
    post_article();
    replytitle[0] = '\0';
    return FULLUPDATE;
}
#endif

int
garbage_line( str )                  /* 判断本行是否是 无用的 */
char *str;
{
    int qlevel = 0;

    while( *str == ':' || *str == '>' ) {
        str++;
        if( *str == ' ' )  str++;
        if( qlevel++ >= 0 )  return 1;
    }
    while( *str == ' ' || *str == '\t' )  str++;
    if( qlevel >= 0 )
        if( strstr( str, "提到:\n" )||strstr( str, ": 】\n" ) || strncmp( str, "==>", 3 ) == 0 ||strstr( str, "的文章 说" ))
            return 1;
    return( *str == '\n' );
}

/* When there is an old article that can be included -jjyang */
void
do_quote( filepath ,quote_mode)   /* 引用文章， 全局变量quote_file,quote_user, */
char    *filepath;
char quote_mode;
{
    FILE        *inf, *outf;
    char        *qfile, *quser;
    char        buf[256], *ptr;
    char        ans[4], op;
    int         bflag;
    int		line_count=0;	/* 添加简略模式计数 Bigman: 2000.7.2 */

    qfile = quote_file;
    quser = quote_user;
    bflag = strncmp( qfile, "mail", 4 );  /* 判断引用的是文章还是信 */
    outf = fopen( filepath, "w" );
    if( *qfile != '\0' && (inf = fopen( qfile, "r" )) != NULL ) {  /* 打开被引用文件*/
        op = quote_mode;
        if( op != 'N' ) {                 /* 引用模式为 N 表示 不引用 */
            fgets( buf, 256, inf );                      /* 取出第一行中 被引用文章的 作者信息 */
            if( (ptr = strrchr( buf, ')' )) != NULL ) {  /* 第一个':'到最后一个 ')' 中的字符串 */
                ptr[1] = '\0';
                if( (ptr = strchr( buf, ':' )) != NULL ) {
                    quser = ptr + 1;
                    while( *quser == ' ' )  quser++;
                }
            }
            /*---	period	2000-10-21	add '\n' at beginning of Re-article	---*/
            if( bflag ) fprintf( outf, "\n【 在 %s 的大作中提到: 】\n", quser );
            else fprintf( outf, "\n【 在 %s 的来信中提到: 】\n", quser );

            if( op == 'A' ) {      /* 除第一行外，全部引用 */
                while( fgets( buf, 256, inf ) != NULL )
                {
                    fprintf( outf, ": %s", buf );
                }
            } else if( op == 'R' ) {
                while (fgets( buf, 256, inf ) != NULL)
                    if( buf[0] == '\n' )  break;
                while( fgets( buf, 256, inf ) != NULL )
                {
                    if(Origin2(buf))   /* 判断是否 多次引用 */
                        continue;
                    fprintf( outf, "%s", buf );

                }
            } else {
                while (fgets( buf, 256, inf ) != NULL)
                    if( buf[0] == '\n' )  break;
                while (fgets( buf, 256, inf ) != NULL) {
                    if( strcmp( buf, "--\n" ) == 0 )   /* 引用 到签名档为止 */
                        break;
                    if( buf[ 250 ] != '\0' )
                        strcpy( buf+250, "\n" );
                    if( !garbage_line( buf ) )   /* 判断是否是无用行 */
                    {    fprintf( outf, ": %s", buf );
                        if (op == 'S') {  /* 简略模式,只引用前几行 Bigman:2000.7.2 */
                            line_count++;
                            if (line_count>10)
                            {
                                fprintf( outf, ": ..................." ) ;
                                break;
                            }
                        }
                    }
                }
            }
        }

        fprintf(outf,"\n");
        fclose( inf );
    }
    *quote_file = '\0';
    *quote_user = '\0';

    if(!(currentuser->signature==0||Anony==1))  /* 签名档为0则不添加 */
    {
        addsignature(outf,1);
    }
    fclose(outf);
}

/* Add by SmallPig */
void
getcross(filepath,mode)      /* 把quote_file复制到filepath (转贴或自动发信)*/
char *filepath;
int mode;
{
    FILE        *inf, *of;
    char        buf[256];
    char        owner[256];
    int                     count;
    time_t      now;

    now=time(0);
    inf=fopen(quote_file,"r");
    of = fopen( filepath, "w" );
    if(inf==NULL || of ==NULL)
    {
        /*---	---*/
        if(NULL != inf) fclose(inf);
        if(NULL != of ) fclose(of) ;
        /*---	---*/

        report("Cross Post error");
        return ;
    }
    if(mode==0/*转贴*/)
    {
        int normal_file;         
        int header_count;        
        normal_file=1;           

        write_header(of,1/*不写入 .posts*/);
        if(fgets( buf, 256, inf ) != NULL)
            { for(count=8;buf[count]!=' ';count++)
                owner[count-8]=buf[count];}
        owner[count-8]='\0';
        if(in_mail==YEA)
            fprintf( of, "[1;37m【 以下文字转载自 [32m%s [37m的信箱 】\n",currentuser->userid);
        else
            fprintf( of, "【 以下文字转载自 %s 讨论区 】\n",quote_board);
        if (id_invalid(owner)) normal_file=0;
       if (normal_file) {                                                   
         for (header_count=0;header_count<3;header_count++) {               
           if ( fgets( buf, 256, inf ) == NULL) break;/*Clear Post header*/ 
          }                                                        
          if ((header_count!=2)||(buf[0]!='\n')) normal_file=0;    
        }                                                          
        if (normal_file)                                           
            fprintf( of, "【 原文由 %s 所发表 】\n",owner);        
        else                                                       
            fseek(inf,0,SEEK_SET);                                 

    }else if(mode==1/*自动发信*/)
    {
        fprintf( of,"发信人: deliver (自动发信系统), 信区: %s\n",quote_board);
        fprintf( of,"标  题: %s\n",quote_title);
        fprintf( of,"发信站: %s自动发信系统 (%24.24s)\n\n",BoardName,ctime(&now));
        fprintf( of,"【此篇文章是由自动发信系统所张贴】\n\n");
    }else if(mode==2)
    {
        write_header(of,0/*写入 .posts*/);
    }
    while( fgets( buf, 256, inf ) != NULL)
    {
        if((strstr(buf,"【 以下文字转载自 ")&&strstr(buf,"讨论区 】"))||(strstr(buf,"【 原文由")&&strstr(buf,"所发表 】")))
            continue; /* 避免引用重复 */
        else
            fprintf( of, "%s", buf );
    }
    fclose( inf );
    fclose( of);
    *quote_file = '\0';
}


int
do_post()                 /* 用户post */
{
    *quote_file = '\0';
    *quote_user = '\0';
    return post_article();
}

/*ARGSUSED*/
int
post_reply(ent,fileinfo,direct)    /* 回信给POST作者 */
int ent ;
struct fileheader *fileinfo ;
char *direct ;
{
    char        uid[STRLEN] ;
    char        title[STRLEN] ;
    char        *t ;
    FILE        *fp;


    if(!HAS_PERM(currentuser,PERM_LOGINOK) || !strcmp(currentuser->userid,"guest")) /* guest 无权 */
        return 0;
    /*太狠了吧,被封post就不让回信了
        if (!HAS_PERM(currentuser,PERM_POST)) return; Haohmaru.99.1.18*/

    /* 封禁Mail Bigman:2000.8.22 */
    if (HAS_PERM(currentuser,PERM_DENYMAIL))
    {
        clear();
        move(3,10);
        prints("很抱歉,您目前没有Mail权限!");
        pressreturn();
        return FULLUPDATE;
    }

    modify_user_mode( SMAIL );

    /* indicate the quote file/user */
    setbfile( quote_file, currboard, fileinfo->filename );
    strcpy( quote_user, fileinfo->owner );

    /* find the author */
    if (strchr(quote_user, '.')) {
        genbuf[ 0 ] = '\0';
        fp = fopen( quote_file, "r" );
        if (fp != NULL) {
            fgets( genbuf, 255, fp );
            fclose( fp );
        }

        t = strtok( genbuf, ":" );
        if ( strncmp( t, "发信人", 6 ) == 0 ||
                strncmp( t, "Posted By", 9) == 0 ||
                strncmp( t, "作  家", 6) == 0 ) {
            t = (char *)strtok( NULL, " \r\t\n" );
            strcpy( uid, t );
        } else {
            prints("Error: Cannot find Author ... \n");
            pressreturn();
        }
    } else strcpy( uid, quote_user );

    /* make the title */
    if (toupper(fileinfo->title[0]) != 'R' || fileinfo->title[1] != 'e' ||
            fileinfo->title[2] != ':') strcpy(title,"Re: ") ;
    else title[0] = '\0';
    strncat(title,fileinfo->title,STRLEN-5) ;

    clear();

    /* edit, then send the mail */
    switch (do_send(uid,title)) {
    case -1: prints("系统无法送信\n"); break;
    case -2: prints("送信动作已经中止\n"); break;
    case -3: prints("使用者 '%s' 无法收信\n", uid); break;
    case -4: prints("对方信箱已满，无法收信\n"); break;
    default: prints("信件已成功地寄给原作者 %s\n", uid);
    }
    pressreturn() ;
    return FULLUPDATE ;
}

/* Add by SmallPig */
int
post_cross(islocal,mode)    /* (自动生成文件名) 转贴或自动发信 */
char islocal;
int mode;
{
    struct fileheader postfile ;
    char        filepath[STRLEN], fname[STRLEN];
    char        buf[256],buf4[STRLEN],whopost[IDLEN];
    int         fp,i;
    int aborted;

    if (!haspostperm(currentuser,currboard)&&!mode)
    {
        move( 1, 0 );
        prints("您尚无权限在 %s 发表文章.\n",currboard);
        prints("如果您尚未注册，请在个人工具箱内详细注册身份\n");
        prints("未通过身份注册认证的用户，没有发表文章的权限。\n");
        prints("谢谢合作！ :-) \n");
        return -1;
    }

    memset(&postfile,0,sizeof(postfile)) ;
    strncpy(save_filename,fname,4096) ;

    if(!mode){
        if(!strstr(quote_title,"(转载)"))
            sprintf(buf4,"%s (转载)",quote_title);
        else
            strcpy(buf4,quote_title);
    }else
        strcpy(buf4,quote_title);
    strncpy(save_title,buf4,STRLEN) ;

#ifndef LEEWARD_X_FILTER
    if(mode != 1)
    { /* Leeward 98.05.15: mode 为 1 是系统自动发信(deliver)，不可过滤 */
        if (YEA == check_invalid_post(quote_file, currboard, save_title))
            return -1; /* Leeward 98.03.29, 04.05 */
    }
#endif

    setbfile( filepath, currboard, "");

    if ((aborted=get_postfilename(postfile.filename,filepath))!=0) {
        move( 3, 0 );
        clrtobot();
        prints("\n\n无法创建文件:%d...\n",aborted);
        pressreturn();
        clear();
        return FULLUPDATE;
    }

    if(mode==1)
        strcpy(whopost,"deliver"); /* mode==1为自动发信 */
    else
        strcpy(whopost,currentuser->userid);

    strncpy(postfile.owner,whopost,STRLEN) ;
    setbfile( filepath, currboard, postfile.filename );

    local_article = 0;
    if ( !strcmp( postfile.title, buf ) && quote_file[0] != '\0' )
        if(islocal=='l'||islocal=='L')
            local_article=YEA;

    modify_user_mode( POSTING );
    getcross( filepath ,mode); /*根据fname完成 文件复制 */

    /* Changed by KCN,disable color title
    if(mode != 1)
{
        int i;
        for (i=0;(i<strlen(save_title))&&(i<STRLEN-1);i++) 
          if (save_title[i]==0x1b) postfile.title[i]=' ';
          else 
    	postfile.title[i]=save_title[i];
        postfile.title[i]=0;
}changed by Haohmaru,转贴文章则禁掉
    else*/
    strncpy( postfile.title, save_title, STRLEN );
    if ( local_article == 1 ) /* local save */
    {
        postfile.filename[ STRLEN - 1 ] = 'L';
        postfile.filename[ STRLEN - 2 ] = 'L';
    }else
    {
        postfile.filename[ STRLEN - 1 ] = 'S';
        postfile.filename[ STRLEN - 2 ] = 'S';
        outgo_post(&postfile,currboard);
    }
    /*   setbdir( buf, currboard );Haohmaru.99.11.26.改成下面一行，因为不管是转贴还是自动发文都不会用到文摘模式*/
    sprintf( buf, "boards/%s/%s", currboard, DOT_DIR);
    if (!strcmp(currboard, "syssecurity")
            && strstr(quote_title, "修改 ")
            && strstr(quote_title, " 的权限"))
        postfile.accessed[0] |= FILE_MARKED; /* Leeward 98.03.29 */
    if (strstr(quote_title, "发文权限") && mode == 2)
    {
        postfile.accessed[0] |= FILE_MARKED;/* Haohmaru 99.11.10*/
        postfile.accessed[1] |= FILE_READ;
        postfile.accessed[0] |= FILE_FORWARDED;
    }
    if (append_record( buf, &postfile, sizeof(postfile)) == -1) { /* 添加POST信息到当前版.DIR */
        if(!mode)
        {
            bbslog("1user", "cross_posting '%s' on '%s': append_record failed!",
                    postfile.title, quote_board);
        }else{
            bbslog("1user", "Posting '%s' on '%s': append_record failed!",
                    postfile.title, quote_board);
        }
        pressreturn() ;
        clear() ;
        return 1 ;
    }
    /* brc_add_read( postfile.filename ) ;*/
	updatelastpost(currboard);
    if(!mode)       /* 用户post还是自动发信*/
        sprintf(buf,"cross_posted '%s' on '%s'", postfile.title, currboard) ;
    else
        sprintf(buf,"自动发表系统 POST '%s' on '%s'", postfile.title, currboard) ;
    bbslog("1user",buf) ;
    return 1;
}


void add_loginfo(filepath)    /* POST 最后一行 添加 */
char *filepath;
{       FILE *fp;
    FILE *fp2;
    int color,noidboard;
    char fname[STRLEN];

    noidboard=(seek_in_file("etc/anonymous",currboard)&&Anony); /* etc/anonymous文件中 是匿名版版名 */
    color=(currentuser->numlogins%7)+31; /* 颜色随机变化 */
    sethomefile( fname, currentuser->userid,"signatures" );
    fp=fopen(filepath,"a");
    if ((fp2=fopen(fname, "r"))== NULL||          /* 判断是否已经 存在 签名档 */
            currentuser->signature==0||noidboard)
    {       fputs("\n--\n", fp);
    }else{ /*Bigman 2000.8.10修改,减少代码 */
        fprintf(fp,"\n");}
    /* 由Bigman增加:2000.8.10 Announce版匿名发文问题 */
    if (!strcmp(currboard,"Announce"))
        fprintf(fp, "[m[%2dm※ 来源:·%s %s·[FROM: %s][m\n"
                ,color,BoardName,email_domain(),
                NAME_BBS_CHINESE" BBS站");
    else
        fprintf(fp, "\n[m[%2dm※ 来源:·%s %s·[FROM: %s][m\n"
                ,color,BoardName,email_domain(),(noidboard)?NAME_ANONYMOUS_FROM:currentuser->lasthost);

    if (fp2) fclose(fp2);
    fclose(fp);
    return;
}

int
show_board_notes(bname)     /* 显示版主的话 */
char bname[30];
{
    char buf[256];

    sprintf( buf, "vote/%s/notes", bname );   /* 显示本版的版主的话 vote/版名/notes */
    if( dashf( buf ) ) {
        ansimore2( buf,NA,0,23/*19*/);
        return 1;
    } else if( dashf( "vote/notes" ) ) {      /* 显示系统的话 vote/notes */
        ansimore2( "vote/notes",NA,0,23/*19*/);
        return 1;
    }
    return -1;
}

int
outgo_post(fh, board)
struct fileheader *fh;
char *board;
{
    FILE *foo;

    if (foo = fopen("innd/out.bntp", "a"))
    {
        fprintf(foo, "%s\t%s\t%s\t%s\t%s\n", board,
                fh->filename, currentuser->userid, currentuser->username, save_title);
        fclose(foo);
    }
}

int
post_article()                         /*用户 POST 文章 */
{
    struct fileheader post_file ;
    char        filepath[STRLEN], fname[STRLEN];
    char        buf[256],buf2[256],buf3[STRLEN],buf4[STRLEN];
    int         fp, aborted,anonyboard;
    int         replymode=1; /* Post New UI*/
    char        ans[4],include_mode='S';

    if (YEA == check_readonly(currboard)) /* Leeward 98.03.28 */
        return FULLUPDATE;

#ifdef AIX_CANCELLED_BY_LEEWARD
    if (YEA == check_RAM_lack()) /* Leeward 98.06.16 */
        return FULLUPDATE;
#endif

    modify_user_mode(POSTING);
    if (!haspostperm(currentuser,currboard))     /* POST权限检查*/
    {
        move( 3, 0 );
        clrtobot();
        if(digestmode==NA)
        {
            prints("\n\n        此讨论区是唯读的, 或是您尚无权限在此发表文章.\n");
            prints("        如果您尚未注册，请在个人工具箱内详细注册身份\n");
            prints("        未通过身份注册认证的用户，没有发表文章的权限。\n");
            prints("        谢谢合作！ :-) \n");
        }
        else
        {
            prints("\n\n     目前是文摘或主题模式, 所以不能发表文章.(按左键离开文摘模式)\n");
        }
        pressreturn();
        clear();
        return FULLUPDATE;
    } else if(deny_me(currentuser->userid,currboard)&&!HAS_PERM(currentuser,PERM_SYSOP))     /* 版主禁止POST 检查 */
    {
        move( 3, 0 );
        clrtobot();
        prints("\n\n                     很抱歉，你被板主停止了 POST 的权力...\n");
        pressreturn();
        clear();
        return FULLUPDATE;
    }

    memset(&post_file,0,sizeof(post_file)) ;
    clear() ;
    show_board_notes(currboard);    /* 版主的话 */
#ifndef NOREPLY /* title是否不用Re: */
    if( replytitle[0] != '\0' ) {
        buf4[0]=' ';
        /*        if( strncasecmp( replytitle, "Re:", 3 ) == 0 ) Change By KCN:
        	why use strncasecmp?*/
        if( strncmp( replytitle, "Re:", 3 ) == 0 )
            strcpy(buf, replytitle);
        else
            sprintf(buf,"Re: %s", replytitle);
        buf[50] = '\0';
    } else
#endif
    {
        buf[0]='\0';
        buf4[0]='\0';
        replymode=0;
    }
    if(currentuser->signature>numofsig||currentuser->signature<0) /*签名档No.检查*/
        currentuser->signature=1;
    anonyboard=seek_in_file("etc/anonymous",currboard); /* 是否为匿名版 */
    /* by zixia: 匿名版缺省不使用匿名 */
    if (!strcmp(currboard,"Announce")) 
	Anony=1;
    else
        Anony = 0;

    while(1) /* 发表前修改参数， 可以考虑添加'显示签名档' */
    {
        sprintf(buf3,"引言模式 [%c]",include_mode);
        move( t_lines-4, 0 );
        clrtoeol();
        prints("[m发表文章於 %s 讨论区     %s\n",currboard,
               (anonyboard)?(Anony==1?"[1m要[m使用匿名":"[1m不[m使用匿名"):"");
        clrtoeol();
        prints("使用标题: %-50s\n", (buf[0]=='\0') ? "[正在设定主题]":buf);
        clrtoeol();
        prints("使用第 %d 个签名档     %s",currentuser->signature
               ,(replymode)? buf3: " ");

        if(buf4[0]=='\0'||buf4[0]=='\n'){
            move(t_lines-1,0);
            clrtoeol();
            strcpy(buf4,buf);
            getdata(t_lines-1,0,"标题: ",buf4,50,DOECHO,NULL,NA);
            if((buf4[0]=='\0'||buf4[0]=='\n'))
                if(buf[0]!='\0'){
                    buf4[0]=' ';
                    continue;
                }
                else
                    return FULLUPDATE;
            strcpy(buf,buf4);
            continue;
        }
        move(t_lines-1,0);
        clrtoeol();
        /* Leeward 98.09.24 add: viewing signature(s) while setting post head */
        sprintf(buf2,"请按 [1;32m0[m~[1;32m%d V[m 选/看签名档%s，[1;32mT[m 改标题，%s[1;32mEnter[m 接受所有设定: ",numofsig,(replymode) ? "，[1;32mS/Y[m/[1;32mN[m/[1;32mR[m/[1;32mA[m 改引言模式" : "",(anonyboard)?"[1;32mM[m 匿名，":"");
        getdata(t_lines-1,0,buf2,ans,3,DOECHO,NULL,YEA);
        ans[0] = toupper(ans[0]); /* Leeward 98.09.24 add; delete below toupper */
        if((ans[0]-'0')>=0&&ans[0]-'0'<=9)
        {
            if(atoi(ans)<=numofsig)
                currentuser->signature=atoi(ans);
        }else if((ans[0]=='S'||ans[0]=='Y'||ans[0]=='N'||ans[0]=='A'||ans[0]=='R')&&replymode)
        {
            include_mode=ans[0];
        }else if(ans[0]=='T')
        {
            buf4[0]='\0';
        }else if(ans[0]=='M')
        {
            Anony=(Anony==1)?0:1;
        }else if(ans[0]=='V')
        { /* Leeward 98.09.24 add: viewing signature(s) while setting post head */
            sethomefile(buf2,currentuser->userid,"signatures");
            move(t_lines-1,0);
            if (askyn("预设显示前三个签名档, 要显示全部吗",NA,YEA)==YEA)
                ansimore(buf2);
            else
            {
                clear();
                ansimore2(buf2,NA,0,18);
            }
        }else{
            /* Changed by KCN,disable color title */
            {
                unsigned int i;
                for (i=0;(i<strlen(buf))&&(i<STRLEN-1);i++)
                    if (buf[i]==0x1b) post_file.title[i]=' ';
                    else post_file.title[i]=buf[i];
                post_file.title[i]=0;
            }
            /*        strcpy(post_file.title, buf); */
            strncpy(save_title,post_file.title,STRLEN) ;
            strncpy(save_filename,fname,4096) ;
            if( save_title[0] == '\0' )
                return FULLUPDATE;
            break;
        }
    }/* 输入结束 */

    setbfile( filepath, currboard, "");
    if ((aborted=get_postfilename(post_file.filename,filepath))!=0) {
        move( 3, 0 );
        clrtobot();
        prints("\n\n无法创建文件:%d...\n",aborted);
        pressreturn();
        clear();
        return FULLUPDATE;
    }

    in_mail = NA ;

    /*
    strncpy(post_file.owner,(anonyboard&&Anony)?
            "Anonymous":currentuser->userid,STRLEN) ;
	    */
    strncpy(post_file.owner,(anonyboard&&Anony)?
            currboard:currentuser->userid,STRLEN) ;

    /*
    if ((!strcmp(currboard,"Announce"))&&(!strcmp(post_file.owner,"Anonymous")))
        strcpy(post_file.owner,"SYSOP");
	*/

    if ((!strcmp(currboard,"Announce"))&&(!strcmp(post_file.owner,currboard)))
    strcpy(post_file.owner,"SYSOP");

    setbfile( filepath, currboard, post_file.filename );

    local_article = 0;
    if ( !strcmp( post_file.title, buf ) && quote_file[0] != '\0' )
        if ( quote_file[119] == 'L' )
            local_article = 1;

    modify_user_mode( POSTING );

    do_quote( filepath ,include_mode);  /*引用原文章 */

    aborted = vedit(filepath,YEA) ;  /* 进入编辑状态 */

    add_loginfo(filepath); /*添加最后一行*/

    strncpy( post_file.title, save_title, STRLEN );
    if ( aborted == 1 ) /* local save */
    {
        post_file.filename[ STRLEN - 1 ] = 'L';
        post_file.filename[ STRLEN - 2 ] = 'L';
    }else
    {
        post_file.filename[ STRLEN - 1 ] = 'S';
        post_file.filename[ STRLEN - 2 ] = 'S';
        outgo_post(&post_file, currboard);
    }
    Anony=0;/*Inital For ShowOut Signature*/

#ifndef LEEWARD_X_FILTER
    if (YEA == check_invalid_post(filepath, currboard, post_file.title)) /* Leeward 98.03.29, 04.05 */
        aborted = - 1;
#endif

    if (aborted  == -1) { /* 取消POST */
        unlink( filepath );
        clear() ;
        return FULLUPDATE ;
    }
    setbdir( buf, currboard );

    /* 在boards版版主发文自动添加文章标记 Bigman:2000.8.12*/
    if (!strcmp(currboard, "Board")  && !HAS_PERM(currentuser,PERM_OBOARDS)   && HAS_PERM(currentuser,PERM_BOARDS) )
    {
        post_file.accessed[0] |= FILE_SIGN;
    }

    if (append_record( buf, &post_file, sizeof(post_file)) == -1) { /* 添加POST信息 到 当前版.DIR */
        sprintf(buf, "posting '%s' on '%s': append_record failed!",
                post_file.title, currboard);
        report(buf);
        pressreturn() ;
        clear() ;
        return FULLUPDATE ;
    }
	updatelastpost(currboard);
    brc_add_read( post_file.filename ) ;

    bbslog("1user","posted '%s' on '%s'", post_file.title, currboard) ;
    /*      postreport(post_file.title, 1, currboard);*/ /*added by alex, 96.9.12*/
    if ( !junkboard(currboard) )
    {
        currentuser->numposts++;
    }
    return FULLUPDATE ;
}

int
add_edit_mark(fname,mode,title)
char *fname;
int mode;
char *title;
{
    FILE *fp,*out;
    char buf[256];
    time_t now;
    char outname[STRLEN];
    char tmp[STRLEN];/*Haohmaru.99.4.19*/
    int step=0;

    if( ( fp = fopen (fname,"r") ) == NULL )
        return 0;
    sprintf( outname ,"tmp/%d.editpost",getpid());
    if( ( out = fopen ( outname ,"w") ) == NULL )
    {
        fclose(fp);
        return 0;
    }

    while( ( fgets(buf , 256 , fp) ) != NULL)
    {
        if(mode==1)
        {
            /*                        if(step==1)
                                            step=2;
                                    if(!step && !strncmp(buf,"发信站: ",8))
                                    {
                                            step=1;
                                    }*/
            if(!strncmp(buf,"[36m※ 修改:·",17))
                continue;
            /*if(step!=3&&(!strncmp(buf,"出  处: ",8)||!strncmp(buf,"转信站: ",8)))
                    step=1;*/
            if(Origin2(buf))
            {
                now=time(0);
                fprintf(out,"[36m※ 修改:·%s 於 %15.15s 修改本文·[FROM: %15.15s][m\n",currentuser->userid,ctime(&now)+4,currentuser->lasthost);
                step=3;
            }
            fputs(buf,out);
        }else
        {
            if(step!=3&&!strncmp(buf,"标  题: ",8))
            {
                step=3;
                fprintf(out,"标  题: %s\n",title);
                continue;
            }
            fputs(buf,out);
        }
    }
    fclose(fp);
    fclose(out);

#ifndef LEEWARD_X_FILTER
    if (YEA == check_invalid_post(outname, currboard, title)) /* Leeward 98.03.29, 04.05 */
    {
        unlink(outname);
        return 0;
    }
    else
    {
#endif
        Rename(outname,fname);
        pressanykey();
   
        return 1;
#ifndef LEEWARD_X_FILTER
    }
#endif
}

/*ARGSUSED*/
int
edit_post(ent,fileinfo,direct)  /* POST 编辑 */
int ent ;
struct fileheader *fileinfo ;
char *direct ;
{
    char buf[512] ;
    char *t ;

    if (!strcmp(currboard, "syssecurity")
            ||!strcmp(currboard, "junk")
            ||!strcmp(currboard, "deleted"))    /* Leeward : 98.01.22 */
        return DONOTHING ;

    if (YEA == check_readonly(currboard)) /* Leeward 98.03.28 */
        return FULLUPDATE;

#ifdef AIX_CANCELLED_BY_LEEWARD
    if (YEA == check_RAM_lack()) /* Leeward 98.06.16 */
        return FULLUPDATE;
#endif

    modify_user_mode( EDIT );

    if (!HAS_PERM(currentuser,PERM_SYSOP))      /* SYSOP、当前版主、原发信人 可以编辑 */
        if ( !chk_currBM( currBM,currentuser) )
            /* change by KCN 1999.10.26
                    if(strcmp( fileinfo->owner, currentuser->userid))
            */
            if (!isowner(currentuser,fileinfo))
                return DONOTHING ;

    if(deny_me(currentuser->userid,currboard)&&!HAS_PERM(currentuser,PERM_SYSOP))     /* 版主禁止POST 检查 */
    {
        move( 3, 0 );
        clrtobot();
        prints("\n\n                     很抱歉，你被板主停止了 POST 的权力...\n");
        pressreturn();
        clear();
        return FULLUPDATE;
    }

    clear() ;
    strcpy(buf,direct) ;
    if( (t = strrchr(buf,'/')) != NULL )
        *t = '\0' ;
#ifndef LEEWARD_X_FILTER
    sprintf(genbuf, "/bin/cp -f %s/%s tmp/%d.editpost.bak",
            buf, fileinfo->filename, getpid()); /* Leeward 98.03.29 */
    system(genbuf);
#endif

    /* Leeward 2000.01.23: Cache */
    sprintf(genbuf, "/board/%s/%s.html", currboard,fileinfo->filename);
    ca_expire(genbuf);

    sprintf(genbuf,"%s/%s",buf,fileinfo->filename) ;
    if( vedit_post(genbuf,NA)!=-1)
    {
        if (ADD_EDITMARK)
#ifndef LEEWARD_X_FILTER
        { /* Leeward 98.03.29, 04.06 */
            if (!add_edit_mark(genbuf,1,/*NULL*/fileinfo->title))
            { /* 进入这里多半是 add_edit_mark 中 check_invalid_post 为 YEA */
                sprintf(genbuf, "/bin/cp -f tmp/%d.editpost.bak %s/%s",
                        getpid(), buf, fileinfo->filename);
                system(genbuf);
            }
            sprintf(genbuf, "tmp/%d.editpost.bak", getpid());
            unlink(genbuf);
        }
#else
add_edit_mark(genbuf,1,/*NULL*/fileinfo->title);
#endif
    }
    bbslog("1user","edited post '%s' on %s", fileinfo->title, currboard);
    return FULLUPDATE ;
}

int
edit_title(ent,fileinfo,direct)  /* 编辑文章标题 */
int ent;
struct fileheader *fileinfo;
char *direct;
{
    char        buf[ STRLEN ];

    /* Leeward 99.07.12 added below 2 variables */
    long   i;
    struct fileheader xfh;
    int fd;


    if (!strcmp(currboard, "syssecurity")
            ||!strcmp(currboard, "junk")
            ||!strcmp(currboard, "deleted"))    /* Leeward : 98.01.22 */
        return DONOTHING ;

    if ((digestmode==4)||(digestmode==5)) return DONOTHING;
    if (YEA == check_readonly(currboard)) /* Leeward 98.03.28 */
        return FULLUPDATE;

    if(!HAS_PERM(currentuser,PERM_SYSOP)) /* 权限检查 */
        if( !chk_currBM(currBM,currentuser))
            /* change by KCN 1999.10.26
              if(strcmp( fileinfo->owner, currentuser->userid))
            */
            if(!isowner(currentuser,fileinfo))
            {
                return DONOTHING ;
            }
    getdata(t_lines-1,0,"新文章标题: ",buf,50,DOECHO,NULL,YEA) ; /*输入标题*/
    if( buf[0] != '\0' ) {
        char tmp[STRLEN*2],*t;
        char tmp2[STRLEN]; /* Leeward 98.03.29 */

        /* Leeward 2000.01.23: Cache */
        sprintf(genbuf, "/board/%s/%s.html", currboard,fileinfo->filename);
        ca_expire(genbuf);

        strcpy(tmp2, fileinfo->title); /* Do a backup */
        /* Changed by KCN,disable color title */
        {
            unsigned int i;
            for (i=0;(i<strlen(buf))&&(i<STRLEN-1);i++)
                if (buf[i]==0x1b) fileinfo->title[i]=' ';
                else fileinfo->title[i]=buf[i];
            fileinfo->title[i]=0;
        }
        /*        strcpy(fileinfo->title,buf);*/
        strcpy(tmp,direct) ;
        if( (t = strrchr(tmp,'/')) != NULL )
            *t = '\0' ;
        sprintf(genbuf,"%s/%s",tmp,fileinfo->filename) ;

#ifndef LEEWARD_X_FILTER
        if (add_edit_mark(genbuf,2,buf)) /* Leeward 98.03.29 */
        {
#else
        add_edit_mark(genbuf,2,buf);
#endif

            /* Leeward 99.07.12 added below to fix a big bug */
            setbdir(buf, currboard);
            if ((fd = open(buf,O_RDONLY,0)) != -1) {
                for (i = ent; i > 0; i --)
                {
                    if (0 == get_record_handle(fd, &xfh, sizeof(xfh), i))
                    {
                        if (0 == strcmp(xfh.filename, fileinfo->filename))
                        {
                            ent = i;
                            break;
                        }
                    }
                }
                close(fd);
            }
            if (0 == i)
                return PARTUPDATE;
            /* Leeward 99.07.12 added above to fix a big bug */

            substitute_record(direct, fileinfo, sizeof(*fileinfo), ent);

#ifndef LEEWARD_X_FILTER

        }
        else /* 进入这里多半是 add_edit_mark 中 check_invalid_post 为 YEA */
            strcpy(fileinfo->title, tmp2);
#endif
    }
    return PARTUPDATE;
}

int
mark_post(ent,fileinfo,direct)  /* Mark POST */
int ent;
struct fileheader *fileinfo;
char *direct;
{
    /*---	---*/
    int newent = 1;
    char *ptr, buf[STRLEN];
    struct fileheader mkpost;
    /*---	---*/

    if( !HAS_PERM(currentuser,PERM_SYSOP) )
        if( !chk_currBM(currBM,currentuser) )
        {
            return DONOTHING;
        }

    if (!strcmp(currboard, "syssecurity")
            ||!strcmp(currboard, "Filter")) /* Leeward 98.04.06 */
        return DONOTHING ; /* Leeward 98.03.29 */
    /*Haohmaru.98.10.12.主题模式下不允许mark文章*/
    if (strstr(direct, "/.THREAD")) return DONOTHING;
    if (fileinfo->accessed[0] & FILE_MARKED)
        fileinfo->accessed[0] &= ~FILE_MARKED;
    else fileinfo->accessed[0] |= FILE_MARKED;
    /*    if ( strncmp(fileinfo->title,"Re: ",4)&&strncmp(fileinfo->title,"RE: ",4) )
            sprintf(fileinfo->title,"Re: %s",&(fileinfo->title)+2);
     */
    /*---   Added by period   2000-10-26  add verify when doing idx operation ---*/
    /*#ifdef _DEBUG_*/
    strcpy(buf, direct);
    ptr = strrchr(buf, '/') + 1;
    ptr[0] = '\0';
    sprintf( &genbuf[512], "%s%s", buf, fileinfo->filename);
    if(!dashf( &genbuf[512]) ) newent = 0; /* 借用一下newent :PP   */
    if(!newent || get_record(direct, &mkpost, sizeof(mkpost), ent) < 0
            || strcmp(mkpost.filename, fileinfo->filename)) {
        if(newent) /* newent = 0 说明文件已被删除,不用再search了   */
            newent = search_record_back(direct, sizeof(struct fileheader),
                                        ent, strcmp, fileinfo, &mkpost, 1);
        if(newent <= 0) {
            move(2,0) ;
            prints(" 文章列表发生变动，文章[%s]可能已被删除．\n", fileinfo->title) ;
            clrtobot();
            pressreturn() ;
            return DIRCHANGED;
        }
        ent = newent;
        /* file status may be changed by other BM, so use data *
         * returned from search_record_back()                  */
        if(fileinfo->accessed[0] & FILE_MARKED) mkpost.accessed[0] |= FILE_MARKED;
        else mkpost.accessed[0] &= ~FILE_MARKED;
        memcpy(fileinfo, &mkpost, sizeof(mkpost));
    } else newent = 0;
    /*#endif*/ /* _DEBUG_ */
    /*---	---*/

    substitute_record(direct, fileinfo, sizeof(*fileinfo), ent);
    /*#ifdef _DEBUG_*/
    return (ent == newent) ? DIRCHANGED : PARTUPDATE;
    /*#else
        return PARTUPDATE;
    #endif*/ /* _DEBUG_ */
}

int
noreply_post(ent,fileinfo,direct)  /*Haohmaru.99.01.01设定文章不可re */
int ent;
struct fileheader *fileinfo;
char *direct;
{
    char ans[256];

    if( !HAS_PERM(currentuser,PERM_OBOARDS) )
    {
        if (!chk_currBM(currBM,currentuser))
            return DONOTHING;
    }

    /*Haohmaru.98.10.12.主题模式下不允许设定不可re文章*/
    if (strstr(direct, "/.THREAD")) return DONOTHING;
    if (fileinfo->accessed[1] & FILE_READ)
    {
        fileinfo->accessed[1] &= ~FILE_READ;
        a_prompt( -1, " 该文章已取消不可re模式, 请按 Enter 继续 << ",ans );
    }
    else
    {
        fileinfo->accessed[1] |= FILE_READ;
        a_prompt( -1, " 该文章已设为不可re模式, 请按 Enter 继续 << ",ans );

        /* Bigman:2000.8.29 sysmail版处理添加版务姓名 */
        if (!strcmp(currboard,"sysmail"))
        {
            sprintf(ans,"〖%s〗 处理: %s",currentuser->userid,fileinfo->title);
            strncpy(fileinfo->title, ans, STRLEN);
            fileinfo->title[STRLEN-1] = 0;
        }
    }

    substitute_record(direct, fileinfo, sizeof(*fileinfo), ent);
    return PARTUPDATE;
}

int
noreply_post_noprompt(ent,fileinfo,direct)  /*Haohmaru.99.01.01设定文章不可re */
int ent;
struct fileheader *fileinfo;
char *direct;
{
    char ans[256];

    if( !HAS_PERM(currentuser,PERM_OBOARDS) )
    {
        if (!chk_currBM(currBM,currentuser))
            return DONOTHING;
    }

    /*Haohmaru.98.10.12.主题模式下不允许设定不可re文章*/
    if (strstr(direct, "/.THREAD")) return DONOTHING;
    if (fileinfo->accessed[1] & FILE_READ)
    {
        fileinfo->accessed[1] &= ~FILE_READ;
    }
    else
    {
        fileinfo->accessed[1] |= FILE_READ;
        /* Bigman:2000.8.29 sysmail版处理添加版务姓名 */
        if (!strcmp(currboard,"sysmail"))
        {
            sprintf(ans,"〖%s〗 处理: %s",currentuser->userid,fileinfo->title);
            strncpy(fileinfo->title, ans, STRLEN);
            fileinfo->title[STRLEN-1] = 0;
        }
    }

    substitute_record(direct, fileinfo, sizeof(*fileinfo), ent);
    return PARTUPDATE;
}

int
sign_post(ent,fileinfo,direct)  /*Bigman:2000.8.12 设定文章标志 */
int ent;
struct fileheader *fileinfo;
char *direct;
{
    char ans[STRLEN];
    if( !HAS_PERM(currentuser,PERM_OBOARDS) )
    {
        return (int)DONOTHING;
    }

    /*Bigman:2000.8.12 文摘方式下不能设定文章提醒标志 */
    if (strstr(direct, "/.THREAD")) return DONOTHING;
    if (fileinfo->accessed[0] & FILE_SIGN)
    {
        fileinfo->accessed[0] &= ~FILE_SIGN;
        a_prompt( -1, " 该文章已撤消标记模式, 请按 Enter 继续 << ",ans );
    }
    else
    {
        fileinfo->accessed[0] |= FILE_SIGN;
        a_prompt( -1, " 该文章已设为标记模式, 请按 Enter 继续 << ",ans );
    }
    substitute_record(direct, fileinfo, sizeof(*fileinfo), ent);
    return PARTUPDATE;
}
int
del_range(int ent,struct fileheader *fileinfo ,char *direct ,int mailmode)
  /* 区域删除 */
{
    char del_mode[11],num1[11],num2[11],temp[2] ;
    char fullpath[STRLEN];
    int inum1, inum2 ;
    int result; /* Leeward: 97.12.15 */
    int idel_mode;/*haohmaru.99.4.20*/

    if (!strcmp(currboard, "syssecurity")
            ||!strcmp(currboard, "junk")
            ||!strcmp(currboard, "deleted")
            ||strstr(direct,".THREAD")/*Haohmaru.98.10.16*/)    /* Leeward : 98.01.22 */
        return DONOTHING ;

    if(uinfo.mode == READING && !HAS_PERM(currentuser,PERM_SYSOP ) )
        if(!chk_currBM(currBM,currentuser))
        {
            return DONOTHING ;
        }

    if(digestmode==2)
        return DONOTHING;
    if(digestmode==4||digestmode==5) {
          return DONOTHING;
    }
    clear() ;
    prints("区域删除\n") ;
    /*Haohmaru.99.4.20.增加可以强制删除被mark文章的功能*/
    getdata(1,0,"删除模式 [0]标记删除 [1]普通删除 [2]强制删除(被mark的文章一起删) (0): ",del_mode,10,DOECHO,NULL,YEA) ;
    idel_mode=atoi(del_mode);
    /*   if (idel_mode!=0 || idel_mode!=1)
       {
    return FULLUPDATE ;
}*/
    getdata(2,0,"首篇文章编号(输入0则仅清除标记为删除的文章): ",num1,10,DOECHO,NULL,YEA) ;
    inum1 = atoi(num1) ;
    if(inum1==0) {
       inum2=-1;
       goto THERE;
    }
    if(inum1 <= 0) {
        prints("错误编号\n") ;
        pressreturn() ;
        return FULLUPDATE ;
    }
    getdata(3,0,"末篇文章编号: ",num2,10,DOECHO,NULL,YEA) ;
    inum2 = atoi(num2) ;
    if(inum2 <= inum1) {
        prints("错误编号\n") ;
        pressreturn() ;
        return FULLUPDATE ;
    }
THERE:
    getdata(4,0,"确定删除 (Y/N)? [N]: ",num1,10,DOECHO,NULL,YEA) ;
    if(*num1 == 'Y' || *num1 == 'y') {
        result = delete_range(direct,inum1,inum2,idel_mode) ;
        if(inum1!=0) fixkeep(direct, inum1, inum2);
        else fixkeep(direct, 1, 1);
        if(uinfo.mode!=RMAIL) {
			updatelastpost(currboard);
            sprintf(genbuf, "del %d-%d on %s", inum1, inum2, currboard);
            report(genbuf); /*bbslog*/
        }
        prints("删除%s\n", result ? "失败！" : "完成") ; /* Leeward: 97.12.15 */
DO_REPAIR:
        if (result)/* prints("错误代码: %d;%s 请报告站长，谢谢！", result,direct);
            added by Haohmaru,修复区段删除错误,98.9.12 */	{
            prints("错误代码: %d;%s",result,direct);
            getdata(8,0,"区段删除错误,如果想修复,请确定[35m无人在本板执行区段删除操作并按'Y'[0m (Y/N)? [N]: ",num1,10,DOECHO,NULL,YEA) ;
            if (*num1 == 'Y' ||*num1 == 'y')
            {
                if(!mailmode) {
                    sprintf(fullpath,"mail/%c/%s/.tmpfile",toupper(currentuser->userid[0]),currentuser->userid);
                    unlink(fullpath);
                    sprintf(fullpath,"mail/%c/%s/.deleted",toupper(currentuser->userid[0]),currentuser->userid);
                    unlink(fullpath); }
                else
                {
                    if (YEA == checkreadonly(currboard))/*Haohmaru,只读情况下错误是~bbsroot/boards/.板名tmpfile 文件引起*/
                    {
                        sprintf(fullpath,"boards/.%stmpfile",currboard);
                        unlink(fullpath);
                        sprintf(fullpath,"boards/.%sdeleted",currboard);
                        unlink(fullpath);
                    }
                    else
                    {
                        sprintf(fullpath,"boards/%s/.tmpfile",currboard);
                        unlink(fullpath);
                        sprintf(fullpath,"boards/%s/.deleted",currboard);
                        unlink(fullpath);
                        sprintf(fullpath,"boards/%s/.tmpfilD",currboard);
                        unlink(fullpath);
                        sprintf(fullpath,"boards/%s/.tmpfilJ",currboard);
                        unlink(fullpath);
                    }
                }
                prints("\n错误已经消除,请重新执行区段删除!");
            }
            else
            {
                prints("不能确认");
            }
        }
        /*Haohamru.99.5.14.删除.deleted文件*/
        if(!mailmode) {
            sprintf(fullpath,"mail/%c/%s/.deleted",toupper(currentuser->userid[0]
                                                          ),currentuser->userid);
            unlink(fullpath); }
        else
        {
            sprintf(fullpath,"boards/%s/.deleted",currboard);
            unlink(fullpath);
        }
        pressreturn() ;
        return DIRCHANGED ;
    }
    prints("Delete Aborted\n") ;
    pressreturn() ;
    return FULLUPDATE ;
}

int
del_post(ent,fileinfo,direct)
int ent ;
struct fileheader *fileinfo ;
char *direct ;
{
    FILE        *fn;
    char        buf[512];
    char        usrid[STRLEN];
    char        *t ;
    int         owned, keep, fail;
    extern int SR_BMDELFLAG;

    if (!strcmp(currboard, "syssecurity")
            ||!strcmp(currboard, "junk")
            ||!strcmp(currboard, "deleted"))    /* Leeward : 98.01.22 */
        return DONOTHING ;

    if(digestmode==2||digestmode==4||digestmode==5)
        return DONOTHING;
    keep = sysconf_eval( "KEEP_DELETED_HEADER" ); /*是否保持被删除的POST的 title */
    if( fileinfo->owner[0] == '-' && keep > 0 &&!SR_BMDELFLAG) {
        clear();
        prints( "本文章已删除.\n" );
        pressreturn();
        clear();
        return FULLUPDATE;
    }
    owned = isowner(currentuser,fileinfo);
    /* change by KCN  ! strcmp( fileinfo->owner, currentuser->userid ); */
    strcpy(usrid,fileinfo->owner);
    if( !(owned) && !HAS_PERM(currentuser,PERM_SYSOP) )
        if( !chk_currBM(currBM,currentuser))
        {
            return DONOTHING ;
        }
    if(!SR_BMDELFLAG)
    {
        clear() ;
        prints("删除文章 '%s'.",fileinfo->title) ;
        getdata(1,0,"(Y/N) [N]: ",genbuf,3,DOECHO,NULL,YEA) ;
        if(genbuf[0] != 'Y' && genbuf[0] != 'y') { /* if not yes quit */
            move(2,0) ;
            prints("取消\n") ;
            pressreturn() ;
            clear() ;
            return FULLUPDATE ;
        }
    }
    strcpy(buf,direct) ;
    if( (t = strrchr(buf,'/')) != NULL )
        *t = '\0' ;
    sprintf(genbuf,"Del '%s' on '%s'",fileinfo->title,currboard) ;
    report(genbuf) ; /* bbslog*/
    postreport(fileinfo->title, -1, currboard/* del 1 post*/); /*added by alex, 96.9.12 */
    strncpy(currfile,fileinfo->filename,STRLEN) ;
    if( keep <= 0 ) {
        fail = delete_file(direct,sizeof(struct fileheader),ent,cmpfilename);
    } else {
        fail = update_file(direct,sizeof(struct fileheader),ent,cmpfilename,
                           cpyfilename);
    }
    if( !fail ) {
        cancelpost( currboard, currentuser->userid, fileinfo, owned ,1);
		updatelastpost(currboard);
        sprintf(genbuf,"%s/%s",buf,fileinfo->filename) ;
        if(keep >0)  if/*保留title*/( (fn = fopen( genbuf, "w" )) != NULL ) {
            fprintf( fn, "\n\n\t\t本文章已被 %s 删除.\n",
                     currentuser->userid );
            fclose( fn );
        }

        if ((YEA != digestmode) /* 不可以用 “NA ==” 判断：digestmode 三值 */
                &&!((fileinfo->accessed[0]&FILE_MARKED)
                    &&(fileinfo->accessed[1]& FILE_READ)
                    &&(fileinfo->accessed[0]& FILE_FORWARDED)))
        { /* Leeward 98.06.17 在文摘区删文不减文章数目 */
            if (owned)
            {
                if ((int)currentuser->numposts > 0 && !junkboard(currboard))
                {
                    currentuser->numposts--;/*自己删除的文章，减少post数*/
                }
            } else if ( !strstr(usrid,".")&&BMDEL_DECREASE&&!B_to_b/*版主删除,减少POST数*/){
                struct userec* lookupuser;
                int id = getuser(usrid,&lookupuser);
                if(id && (int)lookupuser->numposts > 0 && !junkboard(currboard) && strcmp(currboard, "sysmail") ) /* SYSOP MAIL版删文不减文章 Bigman: 2000.8.12*/
                { /* Leeward 98.06.21 adds above later 2 conditions */
                    lookupuser->numposts--;
                }
            }
        }

        return DIRCHANGED;
    }
    move(2,0) ;
    prints("删除失败\n") ;
    pressreturn() ;
    clear() ;
    return FULLUPDATE ;
}

/* Added by netty to handle post saving into (0)Announce */
int Save_post(int ent,struct fileheader *fileinfo,char *direct)
{
    if(!HAS_PERM(currentuser,PERM_SYSOP))
        if(!chk_currBM(currBM,currentuser))
            return DONOTHING ;
    return(a_Save( "0Announce", currboard, fileinfo ,NA,direct,ent));
}

/* Semi_save 用来把文章存到暂存档，同时删除文章的头尾 Life 1997.4.6 */
int
Semi_save(ent,fileinfo,direct)
int ent;
struct fileheader *fileinfo;
char *direct;
{
    if(!HAS_PERM(currentuser,PERM_SYSOP))
        if(!chk_currBM(currBM,currentuser))
            return DONOTHING ;
    return(a_SeSave( "0Announce", currboard, fileinfo ,NA));
}

/* Added by netty to handle post saving into (0)Announce */
int
Import_post(ent,fileinfo,direct)
int ent;
struct fileheader *fileinfo;
char *direct;
{
    char szBuf[STRLEN];

    if(!HAS_PERM(currentuser,PERM_SYSOP))
        if(!chk_currBM(currBM,currentuser) )
            return DONOTHING ;

    if (fileinfo->accessed[0] & FILE_IMPORTED) /* Leeward 98.04.15 */
    {
        a_prompt(-1, "本文曾经被收录进精华区过. 现在再次收录吗? (Y/N) [N]: ",
                 szBuf);
        if (szBuf[0] != 'y' && szBuf[0] != 'Y') return FULLUPDATE;
    }
    /* Leeward 98.04.15 */
    return(a_Import( "0Announce", currboard, fileinfo,NA, direct, ent ));
}

int
show_b_note()
{
    clear();
    if(show_board_notes(currboard)==-1)
    {
        move(3,30);
        prints( "此讨论区尚无「备忘录」。" );
    }
    pressanykey();
    return FULLUPDATE;
}

int
into_announce()
{
    if( a_menusearch( "0Announce", currboard, (HAS_PERM(currentuser,PERM_ANNOUNCE)||
                      HAS_PERM(currentuser,PERM_SYSOP)||HAS_PERM(currentuser,PERM_OBOARDS)) ? PERM_BOARDS:0) )
        return FULLUPDATE;
    return DONOTHING;
}

#ifdef INTERNET_EMAIL
int
forward_post(ent,fileinfo,direct) /*转寄*/
int ent;
struct fileheader *fileinfo;
char *direct;
{
    if( strcmp( "guest", currentuser->userid) == 0 )
    {
        clear();
        move(3,10);
        prints("很抱歉,想转寄文章请申请正式ID!");
        pressreturn();
        return FULLUPDATE;
    }

    /* 封禁Mail Bigman:2000.8.22 */
    if (HAS_PERM(currentuser,PERM_DENYMAIL))
    {
        clear();
        move(3,10);
        prints("很抱歉,您目前没有Mail权限!");
        pressreturn();
        return FULLUPDATE;
    }

    return(mail_forward(ent, fileinfo, direct));
}

int
forward_u_post(ent,fileinfo,direct) /*转寄*/
int ent;
struct fileheader *fileinfo;
char *direct;
{
    if( strcmp( "guest", currentuser->userid) == 0 )
        return DONOTHING;

    /* 封禁Mail Bigman:2000.8.22 */
    if (HAS_PERM(currentuser,PERM_DENYMAIL))
    {
        clear();
        move(3,10);
        prints("很抱歉,您目前没有Mail权限!");
        pressreturn();
        return FULLUPDATE;
    }
    return(mail_uforward(ent, fileinfo, direct));
}

#endif

extern int mainreadhelp() ;
extern int b_results();
extern int b_vote();
extern int b_vote_maintain();
extern int b_notes_edit();
extern int b_jury_edit(); /*stephen 2001.11.1*/

static int sequent_ent ;

int
sequent_messages(struct fileheader *fptr,int* continue_flag)
{
    static int idc;

    if(fptr == NULL) {
        idc = 0 ;
        return 0 ;
    }
    idc++ ;
    if(readpost){
        if(idc < sequent_ent)
            return 0;
        if( !brc_unread_t(FILENAME2POSTTIME(  fptr->filename )) )  return 0; /*已读 则 返回*/
        if (*continue_flag != 0) {
            genbuf[ 0 ] = 'y';
        } else {
            prints("讨论区: '%s' 标题:\n\"%s\" posted by %s.\n",
                   currboard,fptr->title,fptr->owner) ;
            getdata(3,0,"读取 (Y/N/Quit) [Y]: ",genbuf,5,DOECHO,NULL,YEA) ;
        }
        if(genbuf[0] != 'y' && genbuf[0] != 'Y' && genbuf[0] != '\0') {
            if(genbuf[0] == 'q' || genbuf[0] == 'Q') {
                clear() ;
                return QUIT ;
            }
            clear() ;
            return 0;
        }
        setbfile( genbuf, currboard, fptr->filename );
        strcpy( quote_file, genbuf );
        strcpy( quote_user, fptr->owner );
#ifdef NOREPLY
        more(genbuf,YEA);
#else
        ansimore(genbuf,NA) ;
        move(t_lines-1, 0);
        clrtoeol();
        prints("\033[1;44;31m[连续读信]  \033[33m回信 R │ 结束 Q,← │下一封 ' ',↓ │^R 回信给作者                \033[m");
        *continue_flag = 0;
        switch( egetch() ) {
        case Ctrl('Z'): r_lastmsg(); /* Leeward 98.07.30 support msgX */
            break;
    case 'N': case 'Q':
    case 'n': case 'q':
        case KEY_LEFT:
            break;
    case 'Y' : case 'R':
    case 'y' : case 'r':
            do_reply(fptr->title); /*回信*/
    case ' ': case '\n':
        case KEY_DOWN:
            *continue_flag = 1; break;
        case Ctrl('R'):
                        post_reply( 0, fptr, (char *)NULL );
            break;
        default : break;
        }
#endif
        clear() ;}
    setbdir( genbuf, currboard );
    brc_add_read( fptr->filename ) ;
    /* return 0;  modified by dong , for clear_new_flag(), 1999.1.20
    if (strcmp(CurArticleFileName, fptr->filename) == 0)
        return QUIT;
    else*/
        return 0;

}

int
sequential_read(ent,fileinfo,direct)
int ent ;
struct fileheader *fileinfo ;
char *direct ;
{
    readpost=1;
    clear();
    return sequential_read2(ent);
}
/*ARGSUSED*/
int
sequential_read2(ent/*,fileinfo,direct*/)
int ent ;
/*struct fileheader *fileinfo ;
char *direct ;*/
{
    char        buf[ STRLEN ];
    int continue_flag;

    sequent_messages((struct fileheader *)NULL,0) ;
    sequent_ent = ent ;
    continue_flag = 0;
    setbdir( buf, currboard );
    apply_record( buf,sequent_messages,sizeof(struct fileheader),&continue_flag) ;
    return FULLUPDATE ;
}

int
clear_new_flag( int ent , struct fileheader *fileinfo , char *direct )
{
	brc_clear_new_flag(fileinfo->filename);
	return PARTUPDATE;
}

int
clear_all_new_flag( int ent , struct fileheader *fileinfo , char *direct )
{
	brc_clear();
	return PARTUPDATE;
}

struct one_key  read_comms[] = { /*阅读状态，键定义 */
                                   'r',        read_post,
                                   'K',        skip_post,
                                   /*   'u',        skip_post,    rem by Haohmaru.99.11.29*/
                                   'd',        del_post,
                                   'D',        del_range,
                                   'm',        mark_post,
                                   ';',	noreply_post, /*Haohmaru.99.01.01,设定不可re模式*/
                                   '#',	sign_post,	/* Bigman: 2000.8.12  设定文章标记模式 */
                                   'E',        edit_post,
                                   Ctrl('G'),  digest_mode,
                                   '`',        digest_mode,
                                   '.',        deleted_mode,
                                   '>',        junk_mode,
                                   'g',        digest_post,
				   'T',        edit_title,
                                   's',        do_select,
                                   Ctrl('C'),  do_cross,
                                   'Y',        XArticle, /* Leeward 98.05.18 */
                                   Ctrl('P'),  do_post,
                                   'c',        clear_new_flag,
                                   'f', 	clear_all_new_flag, /* added by dong, 1999.1.25 */
                                   'S',        sequential_read,
#ifdef INTERNET_EMAIL
                                   'F',        forward_post,
                                   'U',        forward_u_post,
                                   Ctrl('R'),  post_reply,
#endif
                                   'J',	Semi_save,
                                   'i',        Save_post,
                                   'I',        Import_post,
                                   'R',        b_results,
                                   'V',        b_vote,
                                   'M',        b_vote_maintain,
                                   'W',        b_notes_edit,
                                   'h',        mainreadhelp,
                                   'X',		b_jury_edit,
/*编辑版面的仲裁委员名单,stephen on 2001.11.1 */
                                   KEY_TAB,    show_b_note,
                                   'x',        into_announce,
                                   'a',        auth_search_down,
                                   'A',        auth_search_up,
                                   '/',        t_search_down,
                                   '?',        t_search_up,
                                   '\'',       post_search_down,
                                   '\"',       post_search_up,
                                   ']',        thread_down,
                                   '[',        thread_up,
                                   Ctrl('D'),  deny_user,
#ifndef LEEWARD_X_FILTER
                                   Ctrl('K'),  BoardFilter, /* Leeward 98.04.02 */
#endif
                                   Ctrl('A'),  show_author,
                                   Ctrl('O'),  add_author_friend,
                                   Ctrl('Q'),  show_authorinfo,/*Haohmaru.98.12.05*/
                                   Ctrl('W'),  show_authorBM,  /*cityhunter 00.10.18 */
                                   'z',	sendmsgtoauthor,/*Haohmaru.2000.5.19*/
                                   'Z',	sendmsgtoauthor,/*Haohmaru.2000.5.19*/
                                   Ctrl('N'),  SR_first_new,
                                   'n',        SR_first_new,
                                   '\\',       SR_last,
                                   '=',        SR_first,
                                   Ctrl('S'),  SR_read,
                                   'p',        SR_read,
                                   Ctrl('X'),  SR_readX, /* Leeward 98.10.03 */
                                   Ctrl('U'),  SR_author,
                                   Ctrl('H'),  SR_authorX, /* Leeward 98.10.03 */
                                   'b',       SR_BMfunc,
                                   'B',       SR_BMfuncX, /* Leeward 98.04.16 */
                                   Ctrl('T'),  thread_mode,
                                   't',        set_delete_mark, /*KCN 2001 */
                                   'v',	i_read_mail, /* period 2000-11-12 read mail in article list */
                                   /*'!',	Goodbye,Haohmaru 98.09.21*/
                                   '\0',       NULL
                               } ;

int
Read()
{
    char        buf[ STRLEN ];
    char        notename[STRLEN];
    time_t      usetime;
    struct stat st ;

    if(!selboard) {
        move(2,0) ;
        prints("请先选择讨论区\n") ;
        pressreturn() ;
        move(2,0) ;
        clrtoeol() ;
        return -1 ;
    }
    in_mail = NA;
    brc_initial( currentuser->userid,currboard );
    setbdir( buf, currboard );

    setvfile(notename,currboard,"notes");
    if(stat(notename,&st)!=-1)
    {
        if(st.st_mtime<(time(NULL)-7*86400))
        {
/*            sprintf(genbuf,"touch %s",notename);
	    */
	    f_touch(notename);
            setvfile( genbuf, currboard, "noterec" );
            unlink(genbuf);
        }
    }
    if(vote_flag(currboard,'\0',1/*检查读过新的备忘录没*/)==0)
    {
        if(dashf( notename ))
        {
            /*  period  2000-09-15  disable ActiveBoard while reading notes */
            modify_user_mode(READING);
            /*-	-*/
            ansimore(notename,YEA);
            vote_flag(currboard,'R',1/*写入读过新的备忘录*/);
        }
    }
    usetime=time(0);
    i_read( READING, buf,readtitle,readdoent,&read_comms[0],sizeof(struct fileheader)) ;/*进入本版*/
    board_usage(currboard,time(0)-usetime);/*board使用时间记录*/

    brc_update(currentuser->userid,currboard);
    return 0 ;
}

void
notepad()
{
    char        tmpname[STRLEN],note1[4];
    char        note[3][STRLEN-4];
    char        tmp[STRLEN];
    FILE        *in;
    int         i,n;
    time_t thetime = time(0);
    extern int talkrequest;


    clear();
    move(0,0);
    prints("开始你的留言吧！大家正拭目以待....\n");
    sprintf(tmpname,"etc/notepad_tmp/%s.notepad",currentuser->userid);
    if( (in = fopen( tmpname, "w" )) != NULL ) {
        for(i=0;i<3;i++)
            memset(note[i],0,STRLEN-4);
        while(1)
        {
            for (i = 0; i < 3; i++)
            {
                getdata(1 + i, 0, ": ", note[i], STRLEN-5, DOECHO, NULL,NA);
                if (note[i][0] == '\0')
                    break;
            }
            if(i==0)
            {
                fclose(in);
                unlink(tmpname);
                return;
            }
            getdata(5,0,"是否把你的大作放入留言板 (Y)是的 (N)不要 (E)再编辑 [Y]: ",note1, 3, DOECHO, NULL,YEA);
            if(note1[0]=='e' || note1[0]=='E')
                continue;
            else
                break;
        }
        if(note1[0]!='N' && note1[0]!='n')
        {
            sprintf(tmp,"[32m%s[37m（%.24s）",currentuser->userid,currentuser->username);
            fprintf(in,"[31m[40m⊙┬——————————————┤[37m酸甜苦辣板[31m├——————————————┬⊙[m\n");
            fprintf(in,"[31m□┤%-43s[33m在 [36m%.19s[33m 离开时留下的话[31m├□\n",tmp,Ctime(thetime));
            if (i>2) i=2;
            for(n=0;n<=i;n++)
            {
                if (note[n][0] == '\0')
                    break;
                fprintf(in,"[31m│[m%-74.74s[31m│[m\n",note[n]);
            }
            fprintf(in,"[31m□┬———————————————————————————————————┬□[m\n");
            catnotepad( in, "etc/notepad");
            fclose(in);
            Rename(tmpname,"etc/notepad");
        }else
        {
            fclose(in);
            unlink(tmpname);
        }
    }
    if(talkrequest){
        talkreply();
    }
    clear();
    return;
}

time_t get_exit_time(id,exittime) /* 获取离线时间，id:用户ID,
                                   exittime:保存返回的时间，结束符为\n
                                            建议定义为 char exittime[40]
                                   Luzi 1998/10/23 */
/* Leeward 98.10.26 add return value: time_t */
char *id;
char *exittime;
{
    char path[80];
    FILE *fp;
    time_t now = 1; /* if fopen failed return 1 -- Leeward */
    sethomefile( path, id , "exit");
    fp=fopen(path, "rb");
    if (fp!=NULL)
    {
        fread(&now,sizeof(time_t),1,fp);
        fclose(fp);
        strcpy(exittime, ctime(&now));
    }
    else exittime[0]='\n';

    return now;
}

void record_exit_time()   /* 记录离线时间  Luzi 1998/10/23 */
{
    char path[80];
    FILE *fp;
    time_t now;
    sethomefile( path, currentuser->userid , "exit");
    fp=fopen(path, "wb");
    if (fp!=NULL)
    {
        now=time(NULL);
        fwrite(&now,sizeof(time_t),1,fp);
        fclose(fp);
    }
}

int
Goodbye()    /*离站 选单*/
{
    extern int  started;
    time_t      stay;
    char        fname[STRLEN],notename[STRLEN];
    char        sysoplist[20][STRLEN],syswork[20][STRLEN],spbuf[STRLEN],buf[STRLEN];
    int         i,num_sysop,choose,logouts,mylogout=NA;
    FILE        *sysops;
    long	Time=10;/*Haohmaru*/

    /* Add by SmallPig */
    brc_update(currentuser->userid,currboard);
    strcpy(quote_file,"");

/*---	显示备忘录的关掉该死的活动看板	2001-07-01	---*/
    modify_user_mode(READING);

    i=0;
    if((sysops=fopen("etc/sysops","r"))!=NULL)
    {
        while(fgets(buf,STRLEN,sysops)!=NULL&&i<20)
        {
            strcpy(sysoplist[i],(char *)strtok( buf, " \n\r\t" ));
            strcpy(syswork[i],(char *)strtok( NULL, " \n\r\t" ));
            i++;
        }
        fclose(sysops);
    }
    num_sysop=i;
    move(1,0);
    clear() ;
    move(0,0);
    prints("你就要离开 %s ，给 %s 一些建议吗？\n",BoardName, BoardName);
    if(strcmp(currentuser->userid,"guest")!=0)
        prints("[[33m1[m] 寄信给"NAME_BBS_CHINESE"\n");
    prints("[[33m2[m] 返回[32m*"NAME_BBS_CHINESE" BBS*[m\n");
    if(strcmp(currentuser->userid,"guest")!=0){
        if( USE_NOTEPAD == 1)
            prints("[[33m3[m] 写写*留言版*[m\n");
    }
    prints("[[33m4[m] 离开本BBS站\n");
    sprintf(spbuf,"你的选择是 [4]：");
    getdata(7,0, spbuf,genbuf, 4, DOECHO, NULL,YEA );
    clear();
    choose=genbuf[0]-'0';
    if(strcmp(currentuser->userid,"guest")&&choose==1){ /* 写信给站长 */
        if ( PERM_LOGINOK & currentuser->userlevel )/*Haohmaru.98.10.05.没通过注册的只能给注册站长发信*/
        {
            prints("        ID        负责的职务\n");
            prints("   ============ =============\n");
            for(i=1;i<=num_sysop;i++){
                prints("[[33m%1d[m] [1m%-12s %s[m\n",
                       i,sysoplist[i-1],syswork[i-1]);}

            prints("[[33m%1d[m] 还是走了罗！\n",num_sysop+1); /*最后一个选项*/

            sprintf(spbuf,"你的选择是 [[32m%1d[m]：",num_sysop+1);
            getdata(num_sysop+5,0, spbuf,genbuf, 4, DOECHO, NULL ,YEA);
            choose=genbuf[0]-'0';
            if (0 != genbuf[1])
                choose = genbuf[1] - '0' + 10;
            if(choose>=1&&choose<=num_sysop)
                /*        do_send(sysoplist[choose-1], "使用者寄来的的建议信");*/
                if(choose==1) /*modified by Bigman : 2000.8.8 */
                    do_send(sysoplist[0], "【站务总管】使用者寄来的建议信");
                else if(choose==2)
                    do_send(sysoplist[1], "【系统维护】使用者寄来的建议信");
                else if(choose==3)
                    do_send(sysoplist[2], "【版面管理】使用者寄来的建议信");
                else if(choose==4)
                    do_send(sysoplist[3], "【身份确认】使用者寄来的建议信");
    		else if(choose==5)
		    do_send(sysoplist[4], "【仲裁事宜】使用者寄来的建议信");
/* added by stephen 11/13/01 */
	        choose=-1;
        }
        else
        {
            /* 增加注册的提示信息 Bigman:2000.10.31*/
            prints("\n    如果您一直未得到身份认证,请确认您是否到个人工具箱填写了注册单,\n");
            prints("    如果您收到身份确认信,还没有发文聊天等权限,请试着再填写一遍注册单\n\n");
            prints("     站长的 ID   负责的职务\n");
            prints("   ============ =============\n");

            /* added by Bigman: 2000.8.8  修改离站 */
            prints("[[33m%1d[m] [1m%-12s %s[m\n",
                   1,sysoplist[3],syswork[3]);
            prints("[[33m%1d[m] 还是走了罗！\n",2); /*最后一个选项*/

            sprintf(spbuf,"你的选择是 %1d：",2);
            getdata(num_sysop+6,0, spbuf,genbuf, 4, DOECHO, NULL ,YEA);
            choose=genbuf[0]-'0';
            if(choose==1) /*modified by Bigman : 2000.8.8 */
                do_send(sysoplist[3], "【身份确认】使用者寄来的建议信");
            choose=-1;

            /*	for(i=0;i<=3;i++)
                    prints("[[33m%1d[m] [1m%-12s %s[m\n",
                                   i,sysoplist[i+4],syswork[i+4]);
                    prints("[[33m%1d[m] 还是走了罗！\n",4);*/ /*最后一个选项*/
            /*        sprintf(spbuf,"你的选择是 [[32m%1d[m]：",4);
                    getdata(num_sysop+6,0, spbuf,genbuf, 4, DOECHO, NULL ,YEA);
                    choose=genbuf[0]-'0';
                    if(choose==1)
                            do_send(sysoplist[5], "使用者寄来的的建议信");
            	else if(choose==2)
                            do_send(sysoplist[6], "使用者寄来的的建议信");
                    else if(choose==3)
                            do_send(sysoplist[7], "使用者寄来的的建议信");
            	else if(choose==0)
                            do_send(sysoplist[4], "使用者寄来的的建议信");
                            choose=-1;*/
        }
    }
    if(choose==2)  /*返回BBS*/
        return 0;
    if(strcmp(currentuser->userid,"guest")!=0){
        if(choose==3) /*留言簿*/
            if( USE_NOTEPAD ==1 &&HAS_PERM(currentuser,PERM_POST))
                notepad();
    }

    clear();
    prints("\n\n\n\n");
    stay = time(NULL) - login_start_time;    /*本次线上时间*/
    
    currentuser->stay+=stay;

    if(DEFINE(currentuser,DEF_OUTNOTE/*退出时显示用户备忘录*/))
    {
        sethomefile(notename,currentuser->userid,"notes");
        if(dashf(notename))
            ansimore(notename,YEA);
    }

    /* Leeward 98.09.24 Use SHARE MEM and disable the old code */
    if(DEFINE(currentuser,DEF_LOGOUT)) /* 使用自己的离站画面 */
    {
        sethomefile( fname, currentuser->userid,"logout" );
        if(dashf(fname))
            mylogout=YEA;
    }
    if(mylogout)
    {
        logouts=countlogouts(fname); /* logouts 为 离站画面 总数 */
        if(logouts>=1)
        {
            user_display(fname,(logouts==1)?1:
                         (currentuser->numlogins%(logouts))+1,YEA);
        }
    }else
    {
        if(fill_shmfile(2,"etc/logout","GOODBYE_SHMKEY"))
        {
            show_goodbyeshm();
        }
    }

    /*if(DEFINE(currentuser,DEF_LOGOUT\*使用自己的离站画面*\)) Leeward: disable the old code
{
        sethomefile( fname,currentuser->userid, "logout" );
        if(!dashf(fname))
                strcpy(fname,"etc/logout");
}else
        strcpy(fname,"etc/logout");
    if(dashf(fname))
{
        logouts=countlogouts(fname);      \* logouts 为 离站画面 总数 *\
        if(logouts>=1)
        {
                user_display(fname,(logouts==1)?1:
                                   (currentuser->numlogins%(logouts))+1,YEA);
        }
}*/
    report("exit") ;

    /* Leeward 98.04.24 */
    if(strcmp(currentuser->userid,"guest")) /* guest 不必 */
    {
        sethomefile(fname, currentuser->userid,".boardrc" );
        if (dashf(fname))
        {
		/*
            sprintf(genbuf, "/bin/cp %s %s.bak", fname, fname);
	    */
            sprintf(genbuf, "%s.bak", fname);
	    f_cp(fname,genbuf,0);
        }
    }

    /*   stay = time(NULL) - login_start_time;    本次线上时间*/
    /*Haohmaru.98.11.10.简单判断是否用上站机*/
    if(/*strcmp(currentuser->username,"guest")&&*/stay<=Time) {
        char lbuf[256];
        char tmpfile[256];
        FILE* fp;

        strcpy(lbuf,"自首-");
        strftime(lbuf+5, 30, "%Y-%m-%d%y:%H:%M", localtime(&login_start_time));
        sprintf(tmpfile,"tmp/.tmp%d",getpid());
        fp = fopen(tmpfile,"w");
        if (fp) {
            fputs(lbuf,fp);
            fclose(fp);
            mail_file(tmpfile,"surr","自首");
        }
        unlink(tmpfile);
    }
    /* stephen on 2001.11.1: 上站不足5分钟不计算上站次数 */
    if (stay<=300 && currentuser->numlogins > 5){
	currentuser->numlogins --;
        if (currentuser->stay>stay) 
            currentuser->stay-=stay;
    }
    if(started) {
        record_exit_time(); /* 记录用户的退出时间 Luzi 1998.10.23*/
        /*---	period	2000-10-19	4 debug	---*/
        /*        sprintf( genbuf, "Stay:%3ld (%s)", stay / 60, currentuser->username );*/
        bbslog( "1system", "EXIT: Stay:%3ld (%s)[%d %d]", stay / 60, currentuser->username, utmpent, usernum );
        u_exit() ;
        started = 0;
    }

    if(num_user_logins(currentuser->userid)==0||!strcmp(currentuser->userid,"guest"))/*检查还有没有人在线上*/
    {
        FILE *fp;
        char buf[STRLEN],*ptr;
        sethomefile(fname,currentuser->userid,"msgindex");
        unlink(fname);
        sethomefile(fname,currentuser->userid,"msgfile");
        if(DEFINE(currentuser,DEF_MAILMSG/*离站时寄回所有信息*/)&&dashf(fname))
        {
            char title[STRLEN];
            time_t now;

            now=time(0);
            sprintf(title,"[%12.12s] 所有讯息备份",ctime(&now)+4);
            mail_file(fname,currentuser->userid,title);
        }
        unlink(fname);
        if (strcmp(currentuser->userid,"Luzi")) {
            fp=fopen("friendbook","r");  /*搜索系统 寻人名单 */
            while(fp!=NULL&&fgets(buf,sizeof(buf),fp)!=NULL)
            {
                char uid[14];

                ptr=strstr(buf,"@");
                if(ptr==NULL)
                {
                    del_from_file("friendbook",buf);
                    continue;
                }
                ptr++;
                strcpy(uid,ptr);
                ptr=strstr(uid,"\n");
                *ptr='\0';
                if(!strcmp(uid,currentuser->userid)) /*删除本用户的 寻人名单 */
                    del_from_file("friendbook",buf);/*寻人名单只在本次上线有效*/
            }
            if(fp) /*---	add by period 2000-11-11 fix null hd bug	---*/
                fclose(fp);
        }
    }
    sleep(1);
    reset_tty() ;
    pressreturn();/*Haohmaru.98.10.18*/
    shutdown(0,2);
    close(0);
    exit(0) ;
    return -1;
}



int
Info()                   /* 显示版本信息Version.Info */
{
    modify_user_mode( XMENU );
    ansimore("Version.Info",YEA) ;
    clear() ;
    return 0 ;
}

int
Conditions()             /* 显示版权信息COPYING */
{
    modify_user_mode( XMENU );
    ansimore("COPYING",YEA) ;
    clear() ;
    return 0 ;
}

int
ShowWeather()                   /* 显示版本信息Version.Info */
{
    modify_user_mode( XMENU );
    ansimore("WEATHER",YEA) ;
    clear() ;
    return 0 ;
}
int
Welcome()               /* 显示欢迎画面 Welcome */
{
    modify_user_mode( XMENU );
    ansimore( "Welcome", YEA );
    clear() ;
    return 0 ;
}

/*int
EditWelcome()
{
    int aborted;
    char ans[8];
    move(3,0);
    
    modify_user_mode( EDITWELC );
    clrtoeol();
    clrtobot();
    getdata(3,0,"(E)编辑 or (D)删除 Welcome? [E]: ",ans,7,DOECHO,NULL,YEA);
    if (ans[0] == 'D' || ans[0] == 'd') {
        unlink("Welcome");
        move(5,0);
        prints("已删除!\n");
        pressreturn();
        clear();
        report( "del welcome" ) ;
        return 0;
    }
    aborted = vedit("Welcome", NA);             
    clear() ;
    if (aborted)
        prints("取消编辑.\n");
    else {
        report("edit Welcome") ;
        prints("修改完成.\n") ;
    }
    pressreturn() ;
    return 0 ;
}
*/

int
cmpbnames( bname, brec)
char *bname;
struct fileheader *brec;
{
    if (!strncasecmp( bname, brec->filename, sizeof(brec->filename)))
        return 1;
    else
        return 0;
}

void
cancelpost_old( board, userid, fh, owned ) /* 删除文章 转移到junk或deleted版 */
char    *board, *userid;
struct fileheader *fh;
int     owned;
{
    struct fileheader   postfile;
    FILE        *fin, *fout;
    int 	fp;
    char        from[ STRLEN ], path[ STRLEN ];
    char        fname[ STRLEN ], *ptr, *brd;
    int         len;
    time_t      now;

    now=time(0); /* Leeward 98.05.18: Fix bug of "no date" article */

    /* Leeward 2000.01.23: Cache */
    sprintf(genbuf, "/board/%s/%s.html", board, fh->filename);
    ca_expire(genbuf);

    setbfile( genbuf, board, fh->filename );
    if( (fin = fopen( genbuf, "r" )) != NULL ) {
        brd = owned ? "junk" : "deleted"; /*判断删除后 到junk或deleted版 */
        sprintf( fname, "M.%d.A", time(NULL) );
        setbfile( genbuf, brd, fname );
        /*        ip = strrchr(fname,'A') ;*/
        /*        while(dashf(genbuf)) {  modified by dong, for create file test */
        while((fp = open(genbuf,O_CREAT|O_EXCL|O_WRONLY,0644)) == -1) {
            /*                if(*ip == 'Z')
                                ip++,*ip = 'A', *(ip + 1) = '\0' ;
                            else
                                (*ip)++ ;*/
            now++;
            sprintf(fname,"M.%d.A",now) ;
            setbfile( genbuf, brd, fname );
        }
        close(fp);
        fout = fopen(genbuf, "w");
        /*        if( (fout = fopen( genbuf, "w" )) != NULL ) { */
        memset(&postfile,0,sizeof(postfile)) ;
        sprintf( genbuf, "%-32.32s - %s", fh->title, userid );
        strcpy( postfile.filename, fname );
        strncpy( postfile.owner, fh->owner, IDLEN );
        strncpy( postfile.title, genbuf, STRLEN );
        postfile.filename[ STRLEN - 1 ] = 'D'; /*标识上删除*/
        postfile.filename[ STRLEN - 2 ] = 'D';
        /*        } */
        while( fgets( genbuf, sizeof( genbuf ), fin ) != NULL ) {
            if( fout != NULL ) {
                fputs( genbuf, fout );
            }
            len = strlen( genbuf ) - 1;
            genbuf[ len ] = '\0';
            if( len <= 8 ) {
                break;
            } else if( strncmp( genbuf, "发信人: ", 8 ) == 0 ) {
                if( (ptr = strrchr( genbuf, ',' )) != NULL )
                    *ptr = '\0';
                strcpy( from, genbuf + 8 );
            } else if( strncmp( genbuf, "转信站: ", 8 ) == 0 ) {
                strcpy( path, genbuf + 8 );
            }
        }
        if( fout != NULL ) {
            while( fgets( genbuf, sizeof( genbuf ), fin ) != NULL )
                fputs( genbuf, fout );
        }
        fclose( fin );
        if( fout != NULL ) {
            fclose( fout );
            setbdir( genbuf, brd );
            append_record( genbuf, &postfile, sizeof(postfile) );
        }
        sprintf( genbuf, "%s\t%s\t%s\t%s\t%s\n",
                 board, fh->filename, fh->owner, from, path );
        if( (fin = fopen( "innd/cancel.bntp", "a" )) != NULL ) {
            fputs( genbuf, fin );
            fclose( fin );
        }
    }
}

/* by ylsdd 
   unlink action is taked within cancelpost if in mail mode,
   otherwise this item is added to the file '.DELETED' under
   the board's directory, the filename is not changed. 
   Unlike the fb code which moves the file to the deleted
   board.
*/
void
cancelpost( board, userid, fh, owned ,autoappend)
char    *board, *userid;
struct fileheader *fh;
int     owned;
int     autoappend;
{
    struct fileheader   postfile;
    char oldpath[sizeof(genbuf)];
    int         tmpdigestmode;
    struct fileheader* ph;
    time_t now;
    if(uinfo.mode==RMAIL)  {
       sprintf(oldpath,"mail/%c/%s/%s",toupper(currentuser->userid[0]),
                currentuser->userid,fh->filename);
       unlink(oldpath);
       return;
    }
    if (autoappend) ph=&postfile;
    else ph=fh;

    sprintf(genbuf, "/board/%s/%s.html", board, fh->filename);
    ca_expire_file(genbuf);

    if (autoappend) {
      bzero(&postfile,sizeof(postfile));
      strcpy( postfile.filename, fh->filename );
      strncpy( postfile.owner, fh->owner, IDLEN+2 );
      postfile.owner[IDLEN+1]=0;
    };
    now=time(NULL);
    sprintf( genbuf, "%-32.32s - %s", fh->title, userid );
    strncpy( ph->title, genbuf, STRLEN );
    ph->title[STRLEN-1]=0;
    ph->accessed[11]=now/(3600*24)%100; /*localtime(&now)->tm_mday;*/
    if (autoappend) {
        tmpdigestmode=digestmode;
        digestmode=(owned)?5:4;
        setbdir( genbuf, board );
        append_record( genbuf, &postfile, sizeof(postfile) );
        digestmode=tmpdigestmode;
    }
}


void
RemoveAppendedSpace(ptr) /* Leeward 98.02.13 */
char *ptr;
{
    int  Offset;

    /* Below block removing extra appended ' ' in article titles */
    Offset = strlen(ptr);
    for (-- Offset; Offset > 0; Offset --)
    {
        if (' ' != ptr[Offset])
            break;
        else
            ptr[Offset] = 0;
    }
}

int i_read_mail()
{
    extern  char  currdirect[ STRLEN ];
    char savedir[STRLEN];
    /* should set digestmode to NA while read mail. or i_read may cause error */
    int  savemode;
    int  mode;
    strcpy(savedir, currdirect);
    savemode = digestmode;
    digestmode = NA;
    mode = m_read();
    digestmode = savemode;
    strcpy(currdirect, savedir);
    return mode;
}

int
set_delete_mark(ent,fileinfo,direct)
int ent ;
struct fileheader *fileinfo ;
char *direct ;
{
    /*---	---*/
    int newent = 1;
    char *ptr, buf[STRLEN];
    struct fileheader mkpost;
    /*---	---*/

    if(digestmode!=NA&&digestmode!=YEA)
        return DONOTHING;
    if( !HAS_PERM(currentuser,PERM_SYSOP) )
        if( !chk_currBM(currBM,currentuser) )
        {
            return DONOTHING;
        }

    if (!strcmp(currboard, "syssecurity")
            ||!strcmp(currboard, "Filter")) /* Leeward 98.04.06 */
        return DONOTHING ; /* Leeward 98.03.29 */
    /*Haohmaru.98.10.12.主题模式下不允许mark delete文章*/
    if (strstr(direct, "/.THREAD")) return DONOTHING;
    if (fileinfo->accessed[1]&FILE_DEL)
        fileinfo->accessed[1]&=!FILE_DEL;
    else
        fileinfo->accessed[1]|=FILE_DEL;

    strcpy(buf, direct);
    ptr = strrchr(buf, '/') + 1;
    ptr[0] = '\0';
    sprintf( &genbuf[512], "%s%s", buf, fileinfo->filename);
    if(!dashf( genbuf) ) 
    {
            move(2,0) ;
            prints(" 文章列表发生变动，文章[%s]垦被删除．\n", fileinfo->title) ;
            clrtobot();
            pressreturn() ;
            return DIRCHANGED;
    }
    newent=substitute_record_comp(direct, fileinfo, sizeof(*fileinfo), ent,fileinfo,strcmp,&mkpost);
    if (newent)
    {
            move(2,0) ;
            prints(" 文章列表发生变动，文章[%s]垦被删除．\n", fileinfo->title) ;
            clrtobot();
            pressreturn() ;
            return DIRCHANGED;
    }

    return DIRCHANGED;
}
