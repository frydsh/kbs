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
/* ���� ��ע�� �� Alex&Sissi ��� �� alex@mars.net.edu.cn */

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
char *checked;              /* �Ķ�����������ͬ�� bbs.c �� bbssnd.c (4 WWW) */
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
        return NA; /* ����ϵͳ��ȫ��¼����Ҫ����ԭ���������� YEA (���ɹ���) */
    if (!strcmp(boardname, "notepad"))
        return NA; /* ϵͳ���԰�, ÿƪ���¶�Ҫ����, ������ YEA (���ɹ���) */
    if (!strcmp(boardname, "Filter"))
        return NA; /* ϵͳ���˼�¼��������Ķ������������Ա����, ������ YEA */
    if (!strcmp(boardname, "sys_discuss"))
        return NA; /* ϵͳ�ڲ�������,���ù��� */

    for (i = 0; i < 1 + sysconf_eval("RUN_SYSTEM_FILTER"); i ++)
    {

        if (0 == i) sprintf(buf, "boards/%s/.badword", boardname);
        else        sprintf(buf, ".badword"); /* ������������ϵͳ��� */

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
            { /* ��δ�������ȥ�����ּ�Ŀո�ͬʱ�������Ĵ�Сд������߹���Ч�� */
#define XRULER "/\\#$^*'\"-~+<>[] =.|%&_`{}" /* Leeward 98.07.24 ��ЩҲȥ�� */
                int f, g;
                char *x;

                for (f = g = 0, q = bufX, p = buf; *p; p ++)
                {
                    if (strchr(XRULER, *p)) /* ���˿ո񣬻���Щ�ַ�Ҳ... */
                    {
                        g ++; /* �ո��ַ��Ȳ����ƣ�����¼��Ŀ��λ�� */
                        if (1 == g) x = p;
                        continue;
                    }
                    else
                    { /* ɨ�赽�ǿո��ַ� */
                        if (g)
                        { /* ǰ���ǿո��ַ� */
                            if ( 0 == f || ( (*(p + 1) < 128) && !strchr(XRULER, *(p + 1)) ) )
                                while (g -- > 0) *q ++ = *x ++; /* �����ں��ּ�Ŀո�ԭ������ */
                        }
                        g = 0;
                        if (*p < 128) *p = toupper(*p), f = 0; else f = 1;
                        /* ���Դ�Сд */   /* ��¼�ո��Ƿ���ں��ֺ� */
                        *q ++ = *p; /* ���Ʒǿո��ַ� */
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
                    if (bufX != p) strcpy(bufX, p); /* ����ǰ���ո� */
                    for (p = bufX; *p; p ++)
                    {
                        if (*p < 128) *p = toupper(*p); /* ���Դ�Сд */
                        if ('\n' == *p) *p = 0; /* ȥ�������ַ� */
                        if ('\r' == *p) *p = 0; /* ȥ���س��ַ� */
                    }
                    if (0 == bufX[0]) continue; /* ���Կ��� */
                    else RemoveAppendedSpace(bufX); /* ���Ժ���ո� */

                    if (p = strstr(buf, bufX))
                    {
                        if ((bufX[0] > 127) && (bufX[1] > 127) && (0 == bufX[2]))
                        { /* ����������Ϊ���˴������У����λ�õĺ����� */
                            int ich;
                            for (ich = 0, -- p; p >= buf; p --)
                            { /* ͳ�Ƹ���ǰ��ĺ����ֽ���Ŀ */
                                if (*p < 128) break; else ich ++;
                            }
                            if (ich % 2) continue;
                            /* ������ż���ֽڲź�������������� */
                        }

                        fclose(fp);
                        fclose(fpX);
                        sprintf(buf, "%s����%s�������±�����", boardname, bufX);
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

PassFilter(ent,fileinfo,direct)  /* ����һƪ���� Leeward 98.04.06 */
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
        prints("�����ѷ��й���\n");
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
    sprintf(buf,"passed %s's ��%s�� on %s", FilterFile.owner, FilterFile.title, currboard);
    report(buf);

    clear();
    move(2,0);
    prints("'%s' �ѷ��е� %s �� \n", fileinfo->title, p);
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
            prints("[1m[33m�ܱ�Ǹ��[31m%s ��Ŀǰ��ֻ��ģʽ[33m\n\n                          �������ڸð淢������޸�����[0m\n", checked);
            pressreturn();
            clear();
        }
        return YEA;
    }
    else
        return NA;
}

/* undelete һƪ���� Leeward 98.05.18 */
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
       prints("�����²����ڣ��ѱ��ָ�, ɾ�����б����");
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
      if (strstr(buf, "������: ") && strstr(buf, "), ����: "))
      {
        i ++;
      }
      else if (strstr(buf, "��  ��: "))
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
    sprintf(buf,"undeleted %s's ��%s�� on %s", UFile.owner, UFile.title, currboard);
    report(buf);

    clear();
    move(2,0);
    prints("'%s' �ѻָ������� \n", UFile.title);
    pressreturn();

    return FULLUPDATE;
}

XArticle(ent,fileinfo,direct)  /* �������µ����⹦�� Leeward 98.05.18 */
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
setqtitle(stitle)   /* ȡ Reply ���º��µ� ����title */
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
setbdir( buf, boardname )  /* �����Ķ�ģʽ ȡĳ�� Ŀ¼·�� */
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
shownotepad()   /* ��ʾ notepad */
{
    modify_user_mode( NOTEPAD );
    ansimore("etc/notepad", YEA);
    clear();
    return;
}

/* ʱ��ת���� ���� */
/*
char *
chtime(clock) 
time_t *clock;
{
    char chinese[STRLEN],week[10],mont[10],date[4],time[10],year[5];
    char *ptr = ctime(clock),*seg,*returndate;

    seg=strtok(ptr," \n\0");
    if(!strcmp(seg,"Sun"))
        strcpy(week,"������");
    if(!strcmp(seg,"Mon"))
        strcpy(week,"����һ");
    if(!strcmp(seg,"Tue"))
        strcpy(week,"���ڶ�");
    if(!strcmp(seg,"Wed"))
        strcpy(week,"������");
    if(!strcmp(seg,"Thu"))
        strcpy(week,"������");
    if(!strcmp(seg,"Fri"))
        strcpy(week,"������");
    if(!strcmp(seg,"Sat"))
        strcpy(week,"������");

    seg=strtok(NULL," \n\0");
    if(!strcmp(seg,"Jan"))
        strcpy(mont," 1��");
    if(!strcmp(seg,"Feb"))
        strcpy(mont," 2��");
    if(!strcmp(seg,"Mar"))
        strcpy(mont," 3��");
    if(!strcmp(seg,"Apr"))
        strcpy(mont," 4��");
    if(!strcmp(seg,"May"))
        strcpy(mont," 5��");
    if(!strcmp(seg,"Jun"))
        strcpy(mont," 6��");
    if(!strcmp(seg,"Jul"))
        strcpy(mont," 7��");
    if(!strcmp(seg,"Aug"))
        strcpy(mont," 8��");
    if(!strcmp(seg,"Sep"))
        strcpy(mont," 9��");
    if(!strcmp(seg,"Oct"))
        strcpy(mont,"10��");
    if(!strcmp(seg,"Nov"))
        strcpy(mont,"11��");
    if(!strcmp(seg,"Dec"))
        strcpy(mont,"12��");
    strcpy(date,strtok(NULL," \n\0"));
    strcpy(time,strtok(NULL," \n\0"));
    strcpy(year,strtok(NULL," \n\0"));

    sprintf(chinese,"  %4.4d��%4.4s%2.2s�� %s %s",atoi(year),mont,date,week,time);
    strncpy(ptr,chinese,strlen(chinese));
    return (ptr);

}
*/


void
printutitle()  /* ��Ļ��ʾ �û��б� title */
{
    /*---	modified by period	2000-11-02	hide posts/logins	---*/
    int isadm;
    const char *fmtadm = "#��վ #����", *fmtcom = "           ";
    isadm = HAS_PERM(currentuser,PERM_ADMINMENU);

    move(2,0) ;
    prints(
#ifdef _DETAIL_UINFO_
        "[44m �� ��  ʹ���ߴ���     %-19s  #��վ #���� %4s    �����������   [m\n",
#else
        "[44m �� ��  ʹ���ߴ���     %-19s  %11s %4s    �����������   [m\n",
#endif
#if defined(ACTS_REALNAMES)
        "��ʵ����",
#else
        "ʹ�����ǳ�",
#endif
#ifndef _DETAIL_UINFO_
        isadm? fmtadm:fmtcom,
#endif
        "���" ) ;
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
make_blist()  /* ���а� ���� �б� */
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
Post()  /* ���˵��ڵ� �ڵ�ǰ�� POST ���� */
{
    if(!selboard) {
        prints("\n\n���� (S)elect ȥѡ��һ����������\n") ;
        pressreturn() ;  /* �ȴ���return�� */
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
postfile(filename,nboard,posttitle,mode)  /* ��ĳ�ļ� POST ��ĳ�� */
char *filename,*nboard,*posttitle;
int mode;
{
    char dbname[STRLEN];
    if(getboardnum(nboard,NULL) <= 0)
    {  /* ����ҪPOST�İ� ,�ж��Ƿ���ڸð� */
        sprintf(dbname,"%s �������Ҳ���",nboard);
        report(dbname);
        return -1;
    }
    in_mail = NA ;   /* ��quote_board,quote_file,quote_title����ҪPOST�����²�����Ȼ��POST */
    strcpy(quote_board,nboard);
    strcpy(dbname,currboard); /* ���浱ǰ�� */
    strcpy(currboard,nboard);
    strcpy(quote_file, filename);
    strcpy(quote_title,posttitle);
    post_cross('l',mode);  /* post �ļ� */
    strcpy(currboard,dbname); /* �ָ�����ĵ�ǰ�� */
    return 0;
}

int
get_a_boardname(bname,prompt)  /* ����һ������ */
char *bname,*prompt;
{
    /*    struct boardheader fh;*/

    make_blist();
    namecomplete(prompt,bname); /* �����Զ����� */
    if (*bname == '\0') {
        return 0;
    }
    /*---	Modified by period	2000-10-29	---*/
    if(getbnum(bname) <= 0)
        /*---	---*/
    {
        move(1,0);
        prints("���������������\n");
        pressreturn();
        move(1,0);
        return 0;
    }
    return 1;
}

/* Add by SmallPig */
int
do_cross(ent,fileinfo,direct)  /* ת�� һƪ ���� */
int ent;
struct fileheader *fileinfo;
char *direct;
{
    char bname[STRLEN];
    char dbname[STRLEN];
    char ispost[10];

    if (!HAS_PERM(currentuser,PERM_POST)) /* �ж��Ƿ���POSTȨ */
    {return DONOTHING;}

    if ((fileinfo->accessed[0] & FILE_FORWARDED) && !HAS_PERM(currentuser,PERM_SYSOP))
    {
        clear();
        move(1,0);
        prints("�������Ѿ�ת����һ�Σ��޷��ٴ�ת��");
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
    prints("[1m[33m��ע�⣺[31m��վվ��涨��ͬ�����ݵ������Ͻ��� 5 (��) ���������������ظ�������\n\nΥ����[33m���������»ᱻɾ��֮�⣬������[31m��������������µ�Ȩ����[33m��ϸ�涨����գ�\n\n    Announce ���վ�棺������ת�����������µĹ涨����\n\n���ҹ�ͬά�� BBS �Ļ�������ʡϵͳ��Դ��лл������\n\n[0m");
    if(!get_a_boardname(bname,"������Ҫת��������������: "))
    {
        return FULLUPDATE;
    }
    /* if (!strcmp(bname,currboard))Haohmaru,98.10.03*/
    if( !strcmp(bname,currboard) && (uinfo.mode !=RMAIL) )
    {
        move( 3, 0 );
        clrtobot();
        prints("\n\n                          ��������²���Ҫת��������!");
        pressreturn();
        clear();
        return FULLUPDATE;
    }
    { /* Leeward 98.01.13 ���ת����������ת���İ����Ƿ񱻽�ֹ�� POST Ȩ */
        char szTemp[STRLEN];

        strcpy(szTemp, currboard); /* ���浱ǰ���� */
        strcpy(currboard, bname);  /* ���õ�ǰ����ΪҪת�����İ��� */
        if(deny_me(currentuser->userid,currboard)&&!HAS_PERM(currentuser,PERM_SYSOP))     /* ������ֹPOST ��� */
        {
            move( 3, 0 );
            clrtobot();
            prints("\n\n                �ܱ�Ǹ�����ڸð汻�����ֹͣ�� POST ��Ȩ��...\n");
            pressreturn();
            clear();
            strcpy(currboard, szTemp); /* �ָ���ǰ���� */
            return FULLUPDATE;
        }
        else if (YEA == check_readonly(currboard)) /* Leeward 98.03.28 */
        {
            strcpy(currboard, szTemp); /* �ָ���ǰ���� */
            return FULLUPDATE;
        }
        else
            strcpy(currboard, szTemp); /* �ָ���ǰ���� */
    }

    move(0,0);
    clrtoeol();
    prints("ת�� ' %s ' �� %s �� ",quote_title,bname);
    move(1,0);
    getdata(1,0,"(S)ת�� (L)��վ (A)ȡ��? [A]: ",ispost, 9, DOECHO, NULL,YEA);
    if(ispost[0]=='s'||ispost[0]=='S'||ispost[0]=='L'||ispost[0]=='l')
    {
        strcpy(quote_board,currboard);
        strcpy(dbname,currboard);
        strcpy(currboard,bname);
        if(post_cross(ispost[0],0)==-1) /* ת�� */
        {
            pressreturn();
            move(2,0);
            strcpy(currboard,dbname);
            return FULLUPDATE;
        }
        strcpy(currboard,dbname);
        move(2,0);
        prints("' %s ' ��ת���� %s �� \n",quote_title,bname);
        fileinfo->accessed[0] |= FILE_FORWARDED;  /*added by alex, 96.10.3 */
        substitute_record(direct, fileinfo, sizeof(*fileinfo),ent) ;
    }
    else
    {
        prints("ȡ��");
    }
    move(2,0);
    pressreturn();
    return FULLUPDATE;
}


void
readtitle()  /* ���� ��ʾ�����б� �� title */
{
    struct boardheader    *bp;
    char        header[ STRLEN ], title[ STRLEN ];
    char        readmode[10];
    int chkmailflag=0;

    bp = getbcache( currboard );
    memcpy( currBM, bp->BM, BM_LEN -1);
    if( currBM[0] == '\0' || currBM[0] == ' ' ) {
        strcpy( header, "����������" );
    } else {
        sprintf( header, "����: %s", currBM );
    }
    chkmailflag=chkmail();
    if(chkmailflag==2)/*Haohmaru.99.4.4.������Ҳ������*/
        strcpy( title,"[�������䳬������,����������!]");
    else if ( chkmailflag ) /* �ż���� */
        strcpy( title, "[�����ż�]" );
    else if ( (bp->flag&VOTE_FLAG))  /* ͶƱ��� */
        sprintf( title, "ͶƱ�У��� V ����ͶƱ");
    else
        strcpy( title, bp->title+13 );

    showtitle( header, title );  /* ��ʾ ��һ�� */
    prints("�뿪[��,e] ѡ��[��,��] �Ķ�[��,r] ��������[Ctrl-P] ����[d] ����¼[TAB] ����[h][m\n" );
    if(digestmode==0)  /* �Ķ�ģʽ */
        strcpy(readmode,"һ��");
    else if(digestmode==1)
        strcpy(readmode,"��ժ");
    else if(digestmode==2)
        strcpy(readmode,"����");
    else if(digestmode==4)
	strcpy(readmode,"����");
    else if(digestmode==5)
	strcpy(readmode,"ֽ¦");

    prints("[37m[44m ���   %-12s %6s %-40s[%4sģʽ] [m\n", "�� �� ��", "��  ��", " ���±���",readmode) ;
    clrtobot();
}

char *
readdoent(char* buf,int num,struct fileheader* ent)  /* �������б��� ��ʾ һƪ���±��� */
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

    type = brc_unread(FILENAME2POSTTIME( ent->filename )) ? cUnreadMark : ' ';
    if ((ent->accessed[0] & FILE_DIGEST) /*&& HAS_PERM(currentuser,PERM_MARKPOST)*/)
    {  /* ��ժģʽ �ж� */
        if (type == ' ')
            type = 'g';
        else
            type = 'G';
    }
    if(ent->accessed[0] & FILE_MARKED) /* ����ļ���markס�ˣ��ı��ʶ */
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
    /*    if(HAS_PERM(currentuser,PERM_OBOARDS) && ent->accessed[1] & FILE_READ) *//*�����ܹ����ϵ��ܿ�����re��־,Haohmaru.99.6.7*/
    if (manager & ent->accessed[1] & FILE_READ) /* ���������ܿ�����re��־, Bigman.2001.2.27 */
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
        /*�����ܹ����ϵ��ܿ�Sign��־, Bigman: 2000.8.12*/
    {
        type='#';
    }

    if(manager && ent->accessed[1] & FILE_DEL) /* ����ļ���mark deleteס�ˣ���ʾX*/
    {
        type = 'X';
    }

    if (manager&&(ent->accessed[0]&FILE_IMPORTED)) /* �ļ��Ѿ������뾫���� */
   	{
           if (type==' ') {
   		typeprefix="\x1b[42m";
   		typesufix="\x1b[m";
           } else {
   		typeprefix="\x1b[32m";
   		typesufix="\x1b[m";
           }
   	}
    filetime = atoi( ent->filename + 2 ); /* ���ļ���ȡ��ʱ�� */
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
    /*        date = ctime( &filetime ) + 4;   ʱ�� -> Ӣ�� */
    else
        /* date = ""; char *���ͱ���, ���ܴ���, modified by dong, 1998.9.19 */
        /*        { date = ctime( &filetime ) + 4; date = ""; } */
        date[0]=0;

    /*  Re-Write By Excellent */

    TITLE=ent->title;  /*���±���TITLE*/

    if(FFLL==0)
    {
        if (!strncmp("Re:",ent->title,3) || !strncmp("RE:",ent->title,3) || !strncmp("�� ",ent->title,3) || !strncmp("�� ",ent->title,3)) /*Re������*/
            sprintf(buf," %4d %s%c%s %-12.12s %6.6s  %-47.47s ", num, typeprefix, type, typesufix, ent->owner, date, TITLE);
        else   /* ��Re������ */
            sprintf(buf," %4d %s%c%s %-12.12s %6.6s  �� %-44.44s ",num, typeprefix, type, typesufix, ent->owner,date,TITLE);
    }
    else /* ���� ��ͬ�����ʶ */
    {
        if (!strncmp("Re:",ent->title,3) || !strncmp("RE:",ent->title,3))       /*Re������*/
        {
            if(!strcmp( ReplyPost+3, ent->title+3)) /* ��ǰ�Ķ����� ��ʶ */
                sprintf(buf," [36m%4d[m %s%c%s %-12.12s %6.6s[36m��%-47.47s[m ", num, typeprefix, type, typesufix, ent->owner, date, TITLE);
            else
                sprintf(buf," %4d %s%c%s %-12.12s %6.6s  %-47.47s", num, typeprefix, type, typesufix, ent->owner, date, TITLE) ;
        }
        else if (!strncmp("�� ",ent->title,3) || !strncmp("�� ",ent->title,3)) /* �������е�����*/
        {
            if(strcmp( ReplyPost+4, ent->title+3)==0) /* ��ǰ�Ķ����� ��ʶ */
                sprintf(buf," [36m%4d[m %s%c%s %-12.12s %6.6s[36m��%-47.47s[m", num, typeprefix, type, typesufix,
                        ent->owner, date, TITLE) ;
            else
                sprintf(buf," %4d %s%c%s %-12.12s %6.6s  %-47.47s", num, typeprefix, type, typesufix,
                        ent->owner, date, TITLE) ;
        }
        else
        {
            if(strcmp(ReadPost,ent->title)==0)  /* ��ǰ�Ķ����� ��ʶ */
                sprintf(buf," [33m%4d[m %s%c%s %-12.12s %6.6s[33m���� %-44.44s[m ",num,typeprefix, type, typesufix,
                        ent->owner,date,TITLE);
            else
                sprintf(buf," %4d %s%c%s %-12.12s %6.6s  �� %-44.44s ",num,typeprefix, type, typesufix,
                        ent->owner,date,TITLE);
        }
    }
    return buf ;
}

char currfile[STRLEN] ;

int
cmpfilename(fhdr)  /* �Ƚ� ĳ�ļ����Ƿ�� ��ǰ�ļ� ��ͬ */
struct fileheader *fhdr ;
{
    if(!strncmp(fhdr->filename,currfile,STRLEN))
        return 1 ;
    return 0 ;
}

int
cmpname(fhdr,name)  /* Haohmaru.99.3.30.�Ƚ� ĳ�ļ����Ƿ�� ��ǰ�ļ� ��ͬ */
struct fileheader *fhdr ;
char  name[STRLEN];
{
    if(!strncmp(fhdr->filename,name,STRLEN))
        return 1 ;
    return 0 ;
}

int
cpyfilename(fhdr)  /* �ı�ɾ������ļ��� */
struct fileheader *fhdr ;
{
    char        buf[ STRLEN ];

    sprintf( buf, "-%s", fhdr->owner );
    strncpy( fhdr->owner, buf, IDLEN );
    sprintf( buf, "<< �����ѱ� %s ��ɾ�� >>", currentuser->userid );
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
    /****** ���δ�����������1����д.DIR�ļ� ******/
    /*if (HAS_PERM(currentuser,PERM_POST) && brc_unread(fileinfo->filename))
      {
        fileinfo->ldReadCount++;
        substitute_record(direct, fileinfo, sizeof(*fileinfo), ent); 
       }*/
    /****** Luzi add in 99/01/13 ******************/

    /*Haohmaru.99.11.27.���´�������ͳ���������Ķ�����
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
    ch = ansimore(genbuf,NA) ;  /* ��ʾ�������� */
#else
    ch = ansimore(genbuf,YEA) ; /* ��ʾ�������� */
#endif
    brc_add_read( fileinfo->filename ) ;
#ifndef NOREPLY
    move(t_lines-1, 0);
    clrtoeol();  /* ��������β */
    if (haspostperm(currentuser,currboard)) {  /* �����Ƿ���POSTȨ ��ʾ����һ�� */
        prints("[44m[31m[�Ķ�����] [33m ���� R �� ���� Q,�� ����һ�� ������һ�� <Space>,���������Ķ� ^X��p [m");
    } else {
        prints("[44m[31m[�Ķ�����]  [33m���� Q,�� ����һ�� ������һ�� <Space>,<Enter>,���������Ķ� ^X �� p [m");
    }

    FFLL=1; /* ReplyPost��ΪReply�����������ReadPostΪȥ��Re:�������� */
    if(!strncmp(fileinfo->title,"Re:",3))
    {
        strcpy(ReplyPost,fileinfo->title);
        for(cou=0;cou<STRLEN;cou++)
            ReadPost[cou]=ReplyPost[cou+4];
    }
    else if (!strncmp(fileinfo->title,"�� ",3) || !strncmp(fileinfo->title,"�� ",3))
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
            prints("\n\n                    �ܱ�Ǹ���ð���ܷ�������,���ܻ�����...\n");
            pressreturn();
            break;/*Haohmaru.98.12.19,���ܻ����µİ�*/
        }
        if(fileinfo->accessed[1] & FILE_READ)/*Haohmaru.99.01.01.���²���re*/
        {
            clear();
            move( 3, 0 );
            prints("\n\n            �ܱ�Ǹ�������Ѿ�����Ϊ����reģʽ,�벻Ҫ��ͼ���۱���...\n");
            pressreturn();
            break;
        }
        do_reply(fileinfo->title);
        break;
    case Ctrl('R'):
                    post_reply( ent, fileinfo, direct ); /* ������ */
        break;
    case 'g':
        digest_post( ent, fileinfo, direct ); /* ��ժģʽ */
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
    case Ctrl('Q'):/*Haohmaru.98.12.05,ϵͳ����Աֱ�Ӳ���������*/
                    clear();
        show_authorinfo(0,fileinfo,'\0');
        return READ_NEXT;
        break;
    case Ctrl('W'):/*cityhunter 00.10.18�쿴������Ϣ */
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
do_select( ent, fileinfo, direct )  /* ������������ ѡ�������� */
int ent ;
struct fileheader *fileinfo ;
char *direct ;
{
    char bname[STRLEN], bpath[ STRLEN ];
    struct stat st ;

    move(0,0) ;
    clrtoeol();
    prints("ѡ��һ�������� (Ӣ����ĸ��Сд�Կ�)\n") ;
    prints("������������ (���հ׼��Զ���Ѱ): ") ;
    clrtoeol() ;

    make_blist() ;  /* ��������Board�� �б� */
    namecomplete((char *)NULL,bname) ; /* ��ʾ���� board �� */
    setbpath( bpath, bname );
    if((*bname == '\0') || (stat(bpath,&st) == -1)) { /* �ж�board�Ƿ���� */
        move(2,0);
        prints("����ȷ��������.\n");
        pressreturn();
        return FULLUPDATE ;
    }
    if(!(st.st_mode & S_IFDIR)) {
        move(2,0) ;
        prints("����ȷ��������.\n") ;
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
    setbdir( direct, currboard ); /* direct �趨 Ϊ ��ǰboardĿ¼ */
    return NEWDIRECT ;
}

int
digest_mode()  /* ��ժģʽ �л� */
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
    prints("[5m���Ժ�ϵͳ���������...[m\n");
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
        prints("ϵͳ�������أ���ʱ������Ӧ�����Ķ�������...");
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
        /* �����ж� Bigman 2000.12.26 */
        saveline(t_lines-2, 0, NULL);
        move(t_lines-2, 0);
        clrtoeol();
        strcpy(buf,"ȷ��ͬ�����Ķ�? (Y/N) [N]:");
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
dele_digest(dname,direc)         /* ɾ����ժ��һƪPOST, dname=post�ļ���,direc=��ժĿ¼�� */
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
    pos=search_record(new_dir, &fh, sizeof(fh), cmpname, digest_name); /* ��ժĿ¼�� .DIR�� ���� ��POST */
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

    if(!chk_currBM(currBM,currentuser))       /* Ȩ����� */
    {
        return DONOTHING ;
    }
    if (digestmode==YEA||digestmode==4||digestmode==5)      /* ��ժģʽ�� ���� �����ժ, ���պ�ֽ¨ģʽҲ���� */
        return DONOTHING;

    if (fhdr->accessed[0] & FILE_DIGEST)  /* ����Ѿ�����ժ�Ļ��������ժ��ɾ����post */
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
            prints("��Ǹ�������ժ�����Ѿ����� %d ƪ���޷��ټ���...\n",MAX_DIGEST);
            pressanykey();
            return PARTUPDATE;
        }
        append_record(buf, &digest, sizeof(digest));  /* ��ժĿ¼����� .DIR */
        fhdr->accessed[0] = fhdr->accessed[0] | FILE_DIGEST;
    }
    substitute_record(direct, fhdr, sizeof(*fhdr), ent);  /* ��Ŀ¼�� .DIR �ı� (�����g��־) */
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
garbage_line( str )                  /* �жϱ����Ƿ��� ���õ� */
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
        if( strstr( str, "�ᵽ:\n" )||strstr( str, ": ��\n" ) || strncmp( str, "==>", 3 ) == 0 ||strstr( str, "������ ˵" ))
            return 1;
    return( *str == '\n' );
}

/* When there is an old article that can be included -jjyang */
void
do_quote( filepath ,quote_mode)   /* �������£� ȫ�ֱ���quote_file,quote_user, */
char    *filepath;
char quote_mode;
{
    FILE        *inf, *outf;
    char        *qfile, *quser;
    char        buf[256], *ptr;
    char        ans[4], op;
    int         bflag;
    int		line_count=0;	/* ��Ӽ���ģʽ���� Bigman: 2000.7.2 */

    qfile = quote_file;
    quser = quote_user;
    bflag = strncmp( qfile, "mail", 4 );  /* �ж����õ������»����� */
    outf = fopen( filepath, "w" );
    if( *qfile != '\0' && (inf = fopen( qfile, "r" )) != NULL ) {  /* �򿪱������ļ�*/
        op = quote_mode;
        if( op != 'N' ) {                 /* ����ģʽΪ N ��ʾ ������ */
            fgets( buf, 256, inf );                      /* ȡ����һ���� ���������µ� ������Ϣ */
            if( (ptr = strrchr( buf, ')' )) != NULL ) {  /* ��һ��':'�����һ�� ')' �е��ַ��� */
                ptr[1] = '\0';
                if( (ptr = strchr( buf, ':' )) != NULL ) {
                    quser = ptr + 1;
                    while( *quser == ' ' )  quser++;
                }
            }
            /*---	period	2000-10-21	add '\n' at beginning of Re-article	---*/
            if( bflag ) fprintf( outf, "\n�� �� %s �Ĵ������ᵽ: ��\n", quser );
            else fprintf( outf, "\n�� �� %s ���������ᵽ: ��\n", quser );

            if( op == 'A' ) {      /* ����һ���⣬ȫ������ */
                while( fgets( buf, 256, inf ) != NULL )
                {
                    fprintf( outf, ": %s", buf );
                }
            } else if( op == 'R' ) {
                while (fgets( buf, 256, inf ) != NULL)
                    if( buf[0] == '\n' )  break;
                while( fgets( buf, 256, inf ) != NULL )
                {
                    if(Origin2(buf))   /* �ж��Ƿ� ������� */
                        continue;
                    fprintf( outf, "%s", buf );

                }
            } else {
                while (fgets( buf, 256, inf ) != NULL)
                    if( buf[0] == '\n' )  break;
                while (fgets( buf, 256, inf ) != NULL) {
                    if( strcmp( buf, "--\n" ) == 0 )   /* ���� ��ǩ����Ϊֹ */
                        break;
                    if( buf[ 250 ] != '\0' )
                        strcpy( buf+250, "\n" );
                    if( !garbage_line( buf ) )   /* �ж��Ƿ��������� */
                    {    fprintf( outf, ": %s", buf );
                        if (op == 'S') {  /* ����ģʽ,ֻ����ǰ���� Bigman:2000.7.2 */
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

    if(!(currentuser->signature==0||Anony==1))  /* ǩ����Ϊ0����� */
    {
        addsignature(outf,1);
    }
    fclose(outf);
}

/* Add by SmallPig */
void
getcross(filepath,mode)      /* ��quote_file���Ƶ�filepath (ת�����Զ�����)*/
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
    if(mode==0/*ת��*/)
    {
        int normal_file;         
        int header_count;        
        normal_file=1;           

        write_header(of,1/*��д�� .posts*/);
        if(fgets( buf, 256, inf ) != NULL)
            { for(count=8;buf[count]!=' ';count++)
                owner[count-8]=buf[count];}
        owner[count-8]='\0';
        if(in_mail==YEA)
            fprintf( of, "[1;37m�� ��������ת���� [32m%s [37m������ ��\n",currentuser->userid);
        else
            fprintf( of, "�� ��������ת���� %s ������ ��\n",quote_board);
        if (id_invalid(owner)) normal_file=0;
       if (normal_file) {                                                   
         for (header_count=0;header_count<3;header_count++) {               
           if ( fgets( buf, 256, inf ) == NULL) break;/*Clear Post header*/ 
          }                                                        
          if ((header_count!=2)||(buf[0]!='\n')) normal_file=0;    
        }                                                          
        if (normal_file)                                           
            fprintf( of, "�� ԭ���� %s ������ ��\n",owner);        
        else                                                       
            fseek(inf,0,SEEK_SET);                                 

    }else if(mode==1/*�Զ�����*/)
    {
        fprintf( of,"������: deliver (�Զ�����ϵͳ), ����: %s\n",quote_board);
        fprintf( of,"��  ��: %s\n",quote_title);
        fprintf( of,"����վ: %s�Զ�����ϵͳ (%24.24s)\n\n",BoardName,ctime(&now));
        fprintf( of,"����ƪ���������Զ�����ϵͳ��������\n\n");
    }else if(mode==2)
    {
        write_header(of,0/*д�� .posts*/);
    }
    while( fgets( buf, 256, inf ) != NULL)
    {
        if((strstr(buf,"�� ��������ת���� ")&&strstr(buf,"������ ��"))||(strstr(buf,"�� ԭ����")&&strstr(buf,"������ ��")))
            continue; /* ���������ظ� */
        else
            fprintf( of, "%s", buf );
    }
    fclose( inf );
    fclose( of);
    *quote_file = '\0';
}


int
do_post()                 /* �û�post */
{
    *quote_file = '\0';
    *quote_user = '\0';
    return post_article();
}

/*ARGSUSED*/
int
post_reply(ent,fileinfo,direct)    /* ���Ÿ�POST���� */
int ent ;
struct fileheader *fileinfo ;
char *direct ;
{
    char        uid[STRLEN] ;
    char        title[STRLEN] ;
    char        *t ;
    FILE        *fp;


    if(!HAS_PERM(currentuser,PERM_LOGINOK) || !strcmp(currentuser->userid,"guest")) /* guest ��Ȩ */
        return 0;
    /*̫���˰�,����post�Ͳ��û�����
        if (!HAS_PERM(currentuser,PERM_POST)) return; Haohmaru.99.1.18*/

    /* ���Mail Bigman:2000.8.22 */
    if (HAS_PERM(currentuser,PERM_DENYMAIL))
    {
        clear();
        move(3,10);
        prints("�ܱ�Ǹ,��Ŀǰû��MailȨ��!");
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
        if ( strncmp( t, "������", 6 ) == 0 ||
                strncmp( t, "Posted By", 9) == 0 ||
                strncmp( t, "��  ��", 6) == 0 ) {
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
    case -1: prints("ϵͳ�޷�����\n"); break;
    case -2: prints("���Ŷ����Ѿ���ֹ\n"); break;
    case -3: prints("ʹ���� '%s' �޷�����\n", uid); break;
    case -4: prints("�Է������������޷�����\n"); break;
    default: prints("�ż��ѳɹ��ؼĸ�ԭ���� %s\n", uid);
    }
    pressreturn() ;
    return FULLUPDATE ;
}

/* Add by SmallPig */
int
post_cross(islocal,mode)    /* (�Զ������ļ���) ת�����Զ����� */
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
        prints("������Ȩ���� %s ��������.\n",currboard);
        prints("�������δע�ᣬ���ڸ��˹���������ϸע�����\n");
        prints("δͨ�����ע����֤���û���û�з������µ�Ȩ�ޡ�\n");
        prints("лл������ :-) \n");
        return -1;
    }

    memset(&postfile,0,sizeof(postfile)) ;
    strncpy(save_filename,fname,4096) ;

    if(!mode){
        if(!strstr(quote_title,"(ת��)"))
            sprintf(buf4,"%s (ת��)",quote_title);
        else
            strcpy(buf4,quote_title);
    }else
        strcpy(buf4,quote_title);
    strncpy(save_title,buf4,STRLEN) ;

#ifndef LEEWARD_X_FILTER
    if(mode != 1)
    { /* Leeward 98.05.15: mode Ϊ 1 ��ϵͳ�Զ�����(deliver)�����ɹ��� */
        if (YEA == check_invalid_post(quote_file, currboard, save_title))
            return -1; /* Leeward 98.03.29, 04.05 */
    }
#endif

    setbfile( filepath, currboard, "");

    if ((aborted=get_postfilename(postfile.filename,filepath))!=0) {
        move( 3, 0 );
        clrtobot();
        prints("\n\n�޷������ļ�:%d...\n",aborted);
        pressreturn();
        clear();
        return FULLUPDATE;
    }

    if(mode==1)
        strcpy(whopost,"deliver"); /* mode==1Ϊ�Զ����� */
    else
        strcpy(whopost,currentuser->userid);

    strncpy(postfile.owner,whopost,STRLEN) ;
    setbfile( filepath, currboard, postfile.filename );

    local_article = 0;
    if ( !strcmp( postfile.title, buf ) && quote_file[0] != '\0' )
        if(islocal=='l'||islocal=='L')
            local_article=YEA;

    modify_user_mode( POSTING );
    getcross( filepath ,mode); /*����fname��� �ļ����� */

    /* Changed by KCN,disable color title
    if(mode != 1)
{
        int i;
        for (i=0;(i<strlen(save_title))&&(i<STRLEN-1);i++) 
          if (save_title[i]==0x1b) postfile.title[i]=' ';
          else 
    	postfile.title[i]=save_title[i];
        postfile.title[i]=0;
}changed by Haohmaru,ת�����������
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
    /*   setbdir( buf, currboard );Haohmaru.99.11.26.�ĳ�����һ�У���Ϊ������ת�������Զ����Ķ������õ���ժģʽ*/
    sprintf( buf, "boards/%s/%s", currboard, DOT_DIR);
    if (!strcmp(currboard, "syssecurity")
            && strstr(quote_title, "�޸� ")
            && strstr(quote_title, " ��Ȩ��"))
        postfile.accessed[0] |= FILE_MARKED; /* Leeward 98.03.29 */
    if (strstr(quote_title, "����Ȩ��") && mode == 2)
    {
        postfile.accessed[0] |= FILE_MARKED;/* Haohmaru 99.11.10*/
        postfile.accessed[1] |= FILE_READ;
        postfile.accessed[0] |= FILE_FORWARDED;
    }
    if (append_record( buf, &postfile, sizeof(postfile)) == -1) { /* ���POST��Ϣ����ǰ��.DIR */
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
    if(!mode)       /* �û�post�����Զ�����*/
        sprintf(buf,"cross_posted '%s' on '%s'", postfile.title, currboard) ;
    else
        sprintf(buf,"�Զ�����ϵͳ POST '%s' on '%s'", postfile.title, currboard) ;
    bbslog("1user",buf) ;
    return 1;
}


void add_loginfo(filepath)    /* POST ���һ�� ��� */
char *filepath;
{       FILE *fp;
    FILE *fp2;
    int color,noidboard;
    char fname[STRLEN];

    noidboard=(seek_in_file("etc/anonymous",currboard)&&Anony); /* etc/anonymous�ļ��� ����������� */
    color=(currentuser->numlogins%7)+31; /* ��ɫ����仯 */
    sethomefile( fname, currentuser->userid,"signatures" );
    fp=fopen(filepath,"a");
    if ((fp2=fopen(fname, "r"))== NULL||          /* �ж��Ƿ��Ѿ� ���� ǩ���� */
            currentuser->signature==0||noidboard)
    {       fputs("\n--\n", fp);
    }else{ /*Bigman 2000.8.10�޸�,���ٴ��� */
        fprintf(fp,"\n");}
    /* ��Bigman����:2000.8.10 Announce�������������� */
    if (!strcmp(currboard,"Announce"))
        fprintf(fp, "[m[%2dm�� ��Դ:��%s %s��[FROM: %s][m\n"
                ,color,BoardName,email_domain(),
                NAME_BBS_CHINESE" BBSվ");
    else
        fprintf(fp, "\n[m[%2dm�� ��Դ:��%s %s��[FROM: %s][m\n"
                ,color,BoardName,email_domain(),(noidboard)?NAME_ANONYMOUS_FROM:currentuser->lasthost);

    if (fp2) fclose(fp2);
    fclose(fp);
    return;
}

int
show_board_notes(bname)     /* ��ʾ�����Ļ� */
char bname[30];
{
    char buf[256];

    sprintf( buf, "vote/%s/notes", bname );   /* ��ʾ����İ����Ļ� vote/����/notes */
    if( dashf( buf ) ) {
        ansimore2( buf,NA,0,23/*19*/);
        return 1;
    } else if( dashf( "vote/notes" ) ) {      /* ��ʾϵͳ�Ļ� vote/notes */
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
post_article()                         /*�û� POST ���� */
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
    if (!haspostperm(currentuser,currboard))     /* POSTȨ�޼��*/
    {
        move( 3, 0 );
        clrtobot();
        if(digestmode==NA)
        {
            prints("\n\n        ����������Ψ����, ����������Ȩ���ڴ˷�������.\n");
            prints("        �������δע�ᣬ���ڸ��˹���������ϸע�����\n");
            prints("        δͨ�����ע����֤���û���û�з������µ�Ȩ�ޡ�\n");
            prints("        лл������ :-) \n");
        }
        else
        {
            prints("\n\n     Ŀǰ����ժ������ģʽ, ���Բ��ܷ�������.(������뿪��ժģʽ)\n");
        }
        pressreturn();
        clear();
        return FULLUPDATE;
    } else if(deny_me(currentuser->userid,currboard)&&!HAS_PERM(currentuser,PERM_SYSOP))     /* ������ֹPOST ��� */
    {
        move( 3, 0 );
        clrtobot();
        prints("\n\n                     �ܱ�Ǹ���㱻����ֹͣ�� POST ��Ȩ��...\n");
        pressreturn();
        clear();
        return FULLUPDATE;
    }

    memset(&post_file,0,sizeof(post_file)) ;
    clear() ;
    show_board_notes(currboard);    /* �����Ļ� */
#ifndef NOREPLY /* title�Ƿ���Re: */
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
    if(currentuser->signature>numofsig||currentuser->signature<0) /*ǩ����No.���*/
        currentuser->signature=1;
    anonyboard=seek_in_file("etc/anonymous",currboard); /* �Ƿ�Ϊ������ */
    /* by zixia: ������ȱʡ��ʹ������ */
    if (!strcmp(currboard,"Announce")) 
	Anony=1;
    else
        Anony = 0;

    while(1) /* ����ǰ�޸Ĳ����� ���Կ������'��ʾǩ����' */
    {
        sprintf(buf3,"����ģʽ [%c]",include_mode);
        move( t_lines-4, 0 );
        clrtoeol();
        prints("[m��������� %s ������     %s\n",currboard,
               (anonyboard)?(Anony==1?"[1mҪ[mʹ������":"[1m��[mʹ������"):"");
        clrtoeol();
        prints("ʹ�ñ���: %-50s\n", (buf[0]=='\0') ? "[�����趨����]":buf);
        clrtoeol();
        prints("ʹ�õ� %d ��ǩ����     %s",currentuser->signature
               ,(replymode)? buf3: " ");

        if(buf4[0]=='\0'||buf4[0]=='\n'){
            move(t_lines-1,0);
            clrtoeol();
            strcpy(buf4,buf);
            getdata(t_lines-1,0,"����: ",buf4,50,DOECHO,NULL,NA);
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
        sprintf(buf2,"�밴 [1;32m0[m~[1;32m%d V[m ѡ/��ǩ����%s��[1;32mT[m �ı��⣬%s[1;32mEnter[m ���������趨: ",numofsig,(replymode) ? "��[1;32mS/Y[m/[1;32mN[m/[1;32mR[m/[1;32mA[m ������ģʽ" : "",(anonyboard)?"[1;32mM[m ������":"");
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
            if (askyn("Ԥ����ʾǰ����ǩ����, Ҫ��ʾȫ����",NA,YEA)==YEA)
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
    }/* ������� */

    setbfile( filepath, currboard, "");
    if ((aborted=get_postfilename(post_file.filename,filepath))!=0) {
        move( 3, 0 );
        clrtobot();
        prints("\n\n�޷������ļ�:%d...\n",aborted);
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

    do_quote( filepath ,include_mode);  /*����ԭ���� */

    aborted = vedit(filepath,YEA) ;  /* ����༭״̬ */

    add_loginfo(filepath); /*������һ��*/

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

    if (aborted  == -1) { /* ȡ��POST */
        unlink( filepath );
        clear() ;
        return FULLUPDATE ;
    }
    setbdir( buf, currboard );

    /* ��boards����������Զ�������±�� Bigman:2000.8.12*/
    if (!strcmp(currboard, "Board")  && !HAS_PERM(currentuser,PERM_OBOARDS)   && HAS_PERM(currentuser,PERM_BOARDS) )
    {
        post_file.accessed[0] |= FILE_SIGN;
    }

    if (append_record( buf, &post_file, sizeof(post_file)) == -1) { /* ���POST��Ϣ �� ��ǰ��.DIR */
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
                                    if(!step && !strncmp(buf,"����վ: ",8))
                                    {
                                            step=1;
                                    }*/
            if(!strncmp(buf,"[36m�� �޸�:��",17))
                continue;
            /*if(step!=3&&(!strncmp(buf,"��  ��: ",8)||!strncmp(buf,"ת��վ: ",8)))
                    step=1;*/
            if(Origin2(buf))
            {
                now=time(0);
                fprintf(out,"[36m�� �޸�:��%s � %15.15s �޸ı��ġ�[FROM: %15.15s][m\n",currentuser->userid,ctime(&now)+4,currentuser->lasthost);
                step=3;
            }
            fputs(buf,out);
        }else
        {
            if(step!=3&&!strncmp(buf,"��  ��: ",8))
            {
                step=3;
                fprintf(out,"��  ��: %s\n",title);
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
edit_post(ent,fileinfo,direct)  /* POST �༭ */
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

    if (!HAS_PERM(currentuser,PERM_SYSOP))      /* SYSOP����ǰ������ԭ������ ���Ա༭ */
        if ( !chk_currBM( currBM,currentuser) )
            /* change by KCN 1999.10.26
                    if(strcmp( fileinfo->owner, currentuser->userid))
            */
            if (!isowner(currentuser,fileinfo))
                return DONOTHING ;

    if(deny_me(currentuser->userid,currboard)&&!HAS_PERM(currentuser,PERM_SYSOP))     /* ������ֹPOST ��� */
    {
        move( 3, 0 );
        clrtobot();
        prints("\n\n                     �ܱ�Ǹ���㱻����ֹͣ�� POST ��Ȩ��...\n");
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
            { /* ������������ add_edit_mark �� check_invalid_post Ϊ YEA */
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
edit_title(ent,fileinfo,direct)  /* �༭���±��� */
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

    if(!HAS_PERM(currentuser,PERM_SYSOP)) /* Ȩ�޼�� */
        if( !chk_currBM(currBM,currentuser))
            /* change by KCN 1999.10.26
              if(strcmp( fileinfo->owner, currentuser->userid))
            */
            if(!isowner(currentuser,fileinfo))
            {
                return DONOTHING ;
            }
    getdata(t_lines-1,0,"�����±���: ",buf,50,DOECHO,NULL,YEA) ; /*�������*/
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
        else /* ������������ add_edit_mark �� check_invalid_post Ϊ YEA */
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
    /*Haohmaru.98.10.12.����ģʽ�²�����mark����*/
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
    if(!dashf( &genbuf[512]) ) newent = 0; /* ����һ��newent :PP   */
    if(!newent || get_record(direct, &mkpost, sizeof(mkpost), ent) < 0
            || strcmp(mkpost.filename, fileinfo->filename)) {
        if(newent) /* newent = 0 ˵���ļ��ѱ�ɾ��,������search��   */
            newent = search_record_back(direct, sizeof(struct fileheader),
                                        ent, strcmp, fileinfo, &mkpost, 1);
        if(newent <= 0) {
            move(2,0) ;
            prints(" �����б����䶯������[%s]�����ѱ�ɾ����\n", fileinfo->title) ;
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
noreply_post(ent,fileinfo,direct)  /*Haohmaru.99.01.01�趨���²���re */
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

    /*Haohmaru.98.10.12.����ģʽ�²������趨����re����*/
    if (strstr(direct, "/.THREAD")) return DONOTHING;
    if (fileinfo->accessed[1] & FILE_READ)
    {
        fileinfo->accessed[1] &= ~FILE_READ;
        a_prompt( -1, " ��������ȡ������reģʽ, �밴 Enter ���� << ",ans );
    }
    else
    {
        fileinfo->accessed[1] |= FILE_READ;
        a_prompt( -1, " ����������Ϊ����reģʽ, �밴 Enter ���� << ",ans );

        /* Bigman:2000.8.29 sysmail�洦����Ӱ������� */
        if (!strcmp(currboard,"sysmail"))
        {
            sprintf(ans,"��%s�� ����: %s",currentuser->userid,fileinfo->title);
            strncpy(fileinfo->title, ans, STRLEN);
            fileinfo->title[STRLEN-1] = 0;
        }
    }

    substitute_record(direct, fileinfo, sizeof(*fileinfo), ent);
    return PARTUPDATE;
}

int
noreply_post_noprompt(ent,fileinfo,direct)  /*Haohmaru.99.01.01�趨���²���re */
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

    /*Haohmaru.98.10.12.����ģʽ�²������趨����re����*/
    if (strstr(direct, "/.THREAD")) return DONOTHING;
    if (fileinfo->accessed[1] & FILE_READ)
    {
        fileinfo->accessed[1] &= ~FILE_READ;
    }
    else
    {
        fileinfo->accessed[1] |= FILE_READ;
        /* Bigman:2000.8.29 sysmail�洦����Ӱ������� */
        if (!strcmp(currboard,"sysmail"))
        {
            sprintf(ans,"��%s�� ����: %s",currentuser->userid,fileinfo->title);
            strncpy(fileinfo->title, ans, STRLEN);
            fileinfo->title[STRLEN-1] = 0;
        }
    }

    substitute_record(direct, fileinfo, sizeof(*fileinfo), ent);
    return PARTUPDATE;
}

int
sign_post(ent,fileinfo,direct)  /*Bigman:2000.8.12 �趨���±�־ */
int ent;
struct fileheader *fileinfo;
char *direct;
{
    char ans[STRLEN];
    if( !HAS_PERM(currentuser,PERM_OBOARDS) )
    {
        return (int)DONOTHING;
    }

    /*Bigman:2000.8.12 ��ժ��ʽ�²����趨�������ѱ�־ */
    if (strstr(direct, "/.THREAD")) return DONOTHING;
    if (fileinfo->accessed[0] & FILE_SIGN)
    {
        fileinfo->accessed[0] &= ~FILE_SIGN;
        a_prompt( -1, " �������ѳ������ģʽ, �밴 Enter ���� << ",ans );
    }
    else
    {
        fileinfo->accessed[0] |= FILE_SIGN;
        a_prompt( -1, " ����������Ϊ���ģʽ, �밴 Enter ���� << ",ans );
    }
    substitute_record(direct, fileinfo, sizeof(*fileinfo), ent);
    return PARTUPDATE;
}
int
del_range(int ent,struct fileheader *fileinfo ,char *direct ,int mailmode)
  /* ����ɾ�� */
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
    prints("����ɾ��\n") ;
    /*Haohmaru.99.4.20.���ӿ���ǿ��ɾ����mark���µĹ���*/
    getdata(1,0,"ɾ��ģʽ [0]���ɾ�� [1]��ͨɾ�� [2]ǿ��ɾ��(��mark������һ��ɾ) (0): ",del_mode,10,DOECHO,NULL,YEA) ;
    idel_mode=atoi(del_mode);
    /*   if (idel_mode!=0 || idel_mode!=1)
       {
    return FULLUPDATE ;
}*/
    getdata(2,0,"��ƪ���±��(����0���������Ϊɾ��������): ",num1,10,DOECHO,NULL,YEA) ;
    inum1 = atoi(num1) ;
    if(inum1==0) {
       inum2=-1;
       goto THERE;
    }
    if(inum1 <= 0) {
        prints("������\n") ;
        pressreturn() ;
        return FULLUPDATE ;
    }
    getdata(3,0,"ĩƪ���±��: ",num2,10,DOECHO,NULL,YEA) ;
    inum2 = atoi(num2) ;
    if(inum2 <= inum1) {
        prints("������\n") ;
        pressreturn() ;
        return FULLUPDATE ;
    }
THERE:
    getdata(4,0,"ȷ��ɾ�� (Y/N)? [N]: ",num1,10,DOECHO,NULL,YEA) ;
    if(*num1 == 'Y' || *num1 == 'y') {
        result = delete_range(direct,inum1,inum2,idel_mode) ;
        if(inum1!=0) fixkeep(direct, inum1, inum2);
        else fixkeep(direct, 1, 1);
        if(uinfo.mode!=RMAIL) {
			updatelastpost(currboard);
            sprintf(genbuf, "del %d-%d on %s", inum1, inum2, currboard);
            report(genbuf); /*bbslog*/
        }
        prints("ɾ��%s\n", result ? "ʧ�ܣ�" : "���") ; /* Leeward: 97.12.15 */
DO_REPAIR:
        if (result)/* prints("�������: %d;%s �뱨��վ����лл��", result,direct);
            added by Haohmaru,�޸�����ɾ������,98.9.12 */	{
            prints("�������: %d;%s",result,direct);
            getdata(8,0,"����ɾ������,������޸�,��ȷ��[35m�����ڱ���ִ������ɾ����������'Y'[0m (Y/N)? [N]: ",num1,10,DOECHO,NULL,YEA) ;
            if (*num1 == 'Y' ||*num1 == 'y')
            {
                if(!mailmode) {
                    sprintf(fullpath,"mail/%c/%s/.tmpfile",toupper(currentuser->userid[0]),currentuser->userid);
                    unlink(fullpath);
                    sprintf(fullpath,"mail/%c/%s/.deleted",toupper(currentuser->userid[0]),currentuser->userid);
                    unlink(fullpath); }
                else
                {
                    if (YEA == checkreadonly(currboard))/*Haohmaru,ֻ������´�����~bbsroot/boards/.����tmpfile �ļ�����*/
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
                prints("\n�����Ѿ�����,������ִ������ɾ��!");
            }
            else
            {
                prints("����ȷ��");
            }
        }
        /*Haohamru.99.5.14.ɾ��.deleted�ļ�*/
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
    keep = sysconf_eval( "KEEP_DELETED_HEADER" ); /*�Ƿ񱣳ֱ�ɾ����POST�� title */
    if( fileinfo->owner[0] == '-' && keep > 0 &&!SR_BMDELFLAG) {
        clear();
        prints( "��������ɾ��.\n" );
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
        prints("ɾ������ '%s'.",fileinfo->title) ;
        getdata(1,0,"(Y/N) [N]: ",genbuf,3,DOECHO,NULL,YEA) ;
        if(genbuf[0] != 'Y' && genbuf[0] != 'y') { /* if not yes quit */
            move(2,0) ;
            prints("ȡ��\n") ;
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
        if(keep >0)  if/*����title*/( (fn = fopen( genbuf, "w" )) != NULL ) {
            fprintf( fn, "\n\n\t\t�������ѱ� %s ɾ��.\n",
                     currentuser->userid );
            fclose( fn );
        }

        if ((YEA != digestmode) /* �������� ��NA ==�� �жϣ�digestmode ��ֵ */
                &&!((fileinfo->accessed[0]&FILE_MARKED)
                    &&(fileinfo->accessed[1]& FILE_READ)
                    &&(fileinfo->accessed[0]& FILE_FORWARDED)))
        { /* Leeward 98.06.17 ����ժ��ɾ�Ĳ���������Ŀ */
            if (owned)
            {
                if ((int)currentuser->numposts > 0 && !junkboard(currboard))
                {
                    currentuser->numposts--;/*�Լ�ɾ�������£�����post��*/
                }
            } else if ( !strstr(usrid,".")&&BMDEL_DECREASE&&!B_to_b/*����ɾ��,����POST��*/){
                struct userec* lookupuser;
                int id = getuser(usrid,&lookupuser);
                if(id && (int)lookupuser->numposts > 0 && !junkboard(currboard) && strcmp(currboard, "sysmail") ) /* SYSOP MAIL��ɾ�Ĳ������� Bigman: 2000.8.12*/
                { /* Leeward 98.06.21 adds above later 2 conditions */
                    lookupuser->numposts--;
                }
            }
        }

        return DIRCHANGED;
    }
    move(2,0) ;
    prints("ɾ��ʧ��\n") ;
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

/* Semi_save ���������´浽�ݴ浵��ͬʱɾ�����µ�ͷβ Life 1997.4.6 */
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
        a_prompt(-1, "������������¼����������. �����ٴ���¼��? (Y/N) [N]: ",
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
        prints( "�����������ޡ�����¼����" );
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
forward_post(ent,fileinfo,direct) /*ת��*/
int ent;
struct fileheader *fileinfo;
char *direct;
{
    if( strcmp( "guest", currentuser->userid) == 0 )
    {
        clear();
        move(3,10);
        prints("�ܱ�Ǹ,��ת��������������ʽID!");
        pressreturn();
        return FULLUPDATE;
    }

    /* ���Mail Bigman:2000.8.22 */
    if (HAS_PERM(currentuser,PERM_DENYMAIL))
    {
        clear();
        move(3,10);
        prints("�ܱ�Ǹ,��Ŀǰû��MailȨ��!");
        pressreturn();
        return FULLUPDATE;
    }

    return(mail_forward(ent, fileinfo, direct));
}

int
forward_u_post(ent,fileinfo,direct) /*ת��*/
int ent;
struct fileheader *fileinfo;
char *direct;
{
    if( strcmp( "guest", currentuser->userid) == 0 )
        return DONOTHING;

    /* ���Mail Bigman:2000.8.22 */
    if (HAS_PERM(currentuser,PERM_DENYMAIL))
    {
        clear();
        move(3,10);
        prints("�ܱ�Ǹ,��Ŀǰû��MailȨ��!");
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
        if( !brc_unread(FILENAME2POSTTIME(  fptr->filename )) )  return 0; /*�Ѷ� �� ����*/
        if (*continue_flag != 0) {
            genbuf[ 0 ] = 'y';
        } else {
            prints("������: '%s' ����:\n\"%s\" posted by %s.\n",
                   currboard,fptr->title,fptr->owner) ;
            getdata(3,0,"��ȡ (Y/N/Quit) [Y]: ",genbuf,5,DOECHO,NULL,YEA) ;
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
        prints("\033[1;44;31m[��������]  \033[33m���� R �� ���� Q,�� ����һ�� ' ',�� ��^R ���Ÿ�����                \033[m");
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
            do_reply(fptr->title); /*����*/
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

struct one_key  read_comms[] = { /*�Ķ�״̬�������� */
                                   'r',        read_post,
                                   'K',        skip_post,
                                   /*   'u',        skip_post,    rem by Haohmaru.99.11.29*/
                                   'd',        del_post,
                                   'D',        del_range,
                                   'm',        mark_post,
                                   ';',	noreply_post, /*Haohmaru.99.01.01,�趨����reģʽ*/
                                   '#',	sign_post,	/* Bigman: 2000.8.12  �趨���±��ģʽ */
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
/*�༭������ٲ�ίԱ����,stephen on 2001.11.1 */
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
        prints("����ѡ��������\n") ;
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
    if(vote_flag(currboard,'\0',1/*�������µı���¼û*/)==0)
    {
        if(dashf( notename ))
        {
            /*  period  2000-09-15  disable ActiveBoard while reading notes */
            modify_user_mode(READING);
            /*-	-*/
            ansimore(notename,YEA);
            vote_flag(currboard,'R',1/*д������µı���¼*/);
        }
    }
    usetime=time(0);
    i_read( READING, buf,readtitle,readdoent,&read_comms[0],sizeof(struct fileheader)) ;/*���뱾��*/
    board_usage(currboard,time(0)-usetime);/*boardʹ��ʱ���¼*/

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
    prints("��ʼ������԰ɣ��������Ŀ�Դ�....\n");
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
            getdata(5,0,"�Ƿ����Ĵ����������԰� (Y)�ǵ� (N)��Ҫ (E)�ٱ༭ [Y]: ",note1, 3, DOECHO, NULL,YEA);
            if(note1[0]=='e' || note1[0]=='E')
                continue;
            else
                break;
        }
        if(note1[0]!='N' && note1[0]!='n')
        {
            sprintf(tmp,"[32m%s[37m��%.24s��",currentuser->userid,currentuser->username);
            fprintf(in,"[31m[40m�ѩС�����������������������������[37m���������[31m�������������������������������С�[m\n");
            fprintf(in,"[31m����%-43s[33m�� [36m%.19s[33m �뿪ʱ���µĻ�[31m����\n",tmp,Ctime(thetime));
            if (i>2) i=2;
            for(n=0;n<=i;n++)
            {
                if (note[n][0] == '\0')
                    break;
                fprintf(in,"[31m��[m%-74.74s[31m��[m\n",note[n]);
            }
            fprintf(in,"[31m���С����������������������������������������������������������������������С�[m\n");
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

time_t get_exit_time(id,exittime) /* ��ȡ����ʱ�䣬id:�û�ID,
                                   exittime:���淵�ص�ʱ�䣬������Ϊ\n
                                            ���鶨��Ϊ char exittime[40]
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

void record_exit_time()   /* ��¼����ʱ��  Luzi 1998/10/23 */
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
Goodbye()    /*��վ ѡ��*/
{
    extern int  started;
    time_t      stay;
    char        fname[STRLEN],notename[STRLEN];
    char        sysoplist[20][STRLEN],syswork[20][STRLEN],spbuf[STRLEN],buf[STRLEN];
    int         i,num_sysop,choose,logouts,mylogout=NA;
    FILE        *sysops;
    long	Time=10;/*Haohmaru*/

    /* Add by SmallPig */
    brc_update(currentuser->userid);
    strcpy(quote_file,"");

/*---	��ʾ����¼�Ĺص������Ļ����	2001-07-01	---*/
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
    prints("���Ҫ�뿪 %s ���� %s һЩ������\n",BoardName, BoardName);
    if(strcmp(currentuser->userid,"guest")!=0)
        prints("[[33m1[m] ���Ÿ�"NAME_BBS_CHINESE"\n");
    prints("[[33m2[m] ����[32m*"NAME_BBS_CHINESE" BBS*[m\n");
    if(strcmp(currentuser->userid,"guest")!=0){
        if( USE_NOTEPAD == 1)
            prints("[[33m3[m] дд*���԰�*[m\n");
    }
    prints("[[33m4[m] �뿪��BBSվ\n");
    sprintf(spbuf,"���ѡ���� [4]��");
    getdata(7,0, spbuf,genbuf, 4, DOECHO, NULL,YEA );
    clear();
    choose=genbuf[0]-'0';
    if(strcmp(currentuser->userid,"guest")&&choose==1){ /* д�Ÿ�վ�� */
        if ( PERM_LOGINOK & currentuser->userlevel )/*Haohmaru.98.10.05.ûͨ��ע���ֻ�ܸ�ע��վ������*/
        {
            prints("        ID        �����ְ��\n");
            prints("   ============ =============\n");
            for(i=1;i<=num_sysop;i++){
                prints("[[33m%1d[m] [1m%-12s %s[m\n",
                       i,sysoplist[i-1],syswork[i-1]);}

            prints("[[33m%1d[m] ���������ޣ�\n",num_sysop+1); /*���һ��ѡ��*/

            sprintf(spbuf,"���ѡ���� [[32m%1d[m]��",num_sysop+1);
            getdata(num_sysop+5,0, spbuf,genbuf, 4, DOECHO, NULL ,YEA);
            choose=genbuf[0]-'0';
            if (0 != genbuf[1])
                choose = genbuf[1] - '0' + 10;
            if(choose>=1&&choose<=num_sysop)
                /*        do_send(sysoplist[choose-1], "ʹ���߼����ĵĽ�����");*/
                if(choose==1) /*modified by Bigman : 2000.8.8 */
                    do_send(sysoplist[0], "��վ���ܹܡ�ʹ���߼����Ľ�����");
                else if(choose==2)
                    do_send(sysoplist[1], "��ϵͳά����ʹ���߼����Ľ�����");
                else if(choose==3)
                    do_send(sysoplist[2], "���������ʹ���߼����Ľ�����");
                else if(choose==4)
                    do_send(sysoplist[3], "�����ȷ�ϡ�ʹ���߼����Ľ�����");
    		else if(choose==5)
		    do_send(sysoplist[4], "���ٲ����ˡ�ʹ���߼����Ľ�����");
/* added by stephen 11/13/01 */
	        choose=-1;
        }
        else
        {
            /* ����ע�����ʾ��Ϣ Bigman:2000.10.31*/
            prints("\n    �����һֱδ�õ������֤,��ȷ�����Ƿ񵽸��˹�������д��ע�ᵥ,\n");
            prints("    ������յ����ȷ����,��û�з��������Ȩ��,����������дһ��ע�ᵥ\n\n");
            prints("     վ���� ID   �����ְ��\n");
            prints("   ============ =============\n");

            /* added by Bigman: 2000.8.8  �޸���վ */
            prints("[[33m%1d[m] [1m%-12s %s[m\n",
                   1,sysoplist[3],syswork[3]);
            prints("[[33m%1d[m] ���������ޣ�\n",2); /*���һ��ѡ��*/

            sprintf(spbuf,"���ѡ���� %1d��",2);
            getdata(num_sysop+6,0, spbuf,genbuf, 4, DOECHO, NULL ,YEA);
            choose=genbuf[0]-'0';
            if(choose==1) /*modified by Bigman : 2000.8.8 */
                do_send(sysoplist[3], "�����ȷ�ϡ�ʹ���߼����Ľ�����");
            choose=-1;

            /*	for(i=0;i<=3;i++)
                    prints("[[33m%1d[m] [1m%-12s %s[m\n",
                                   i,sysoplist[i+4],syswork[i+4]);
                    prints("[[33m%1d[m] ���������ޣ�\n",4);*/ /*���һ��ѡ��*/
            /*        sprintf(spbuf,"���ѡ���� [[32m%1d[m]��",4);
                    getdata(num_sysop+6,0, spbuf,genbuf, 4, DOECHO, NULL ,YEA);
                    choose=genbuf[0]-'0';
                    if(choose==1)
                            do_send(sysoplist[5], "ʹ���߼����ĵĽ�����");
            	else if(choose==2)
                            do_send(sysoplist[6], "ʹ���߼����ĵĽ�����");
                    else if(choose==3)
                            do_send(sysoplist[7], "ʹ���߼����ĵĽ�����");
            	else if(choose==0)
                            do_send(sysoplist[4], "ʹ���߼����ĵĽ�����");
                            choose=-1;*/
        }
    }
    if(choose==2)  /*����BBS*/
        return 0;
    if(strcmp(currentuser->userid,"guest")!=0){
        if(choose==3) /*���Բ�*/
            if( USE_NOTEPAD ==1 &&HAS_PERM(currentuser,PERM_POST))
                notepad();
    }

    clear();
    prints("\n\n\n\n");
    stay = time(NULL) - login_start_time;    /*��������ʱ��*/
    
    currentuser->stay+=stay;

    if(DEFINE(currentuser,DEF_OUTNOTE/*�˳�ʱ��ʾ�û�����¼*/))
    {
        sethomefile(notename,currentuser->userid,"notes");
        if(dashf(notename))
            ansimore(notename,YEA);
    }

    /* Leeward 98.09.24 Use SHARE MEM and disable the old code */
    if(DEFINE(currentuser,DEF_LOGOUT)) /* ʹ���Լ�����վ���� */
    {
        sethomefile( fname, currentuser->userid,"logout" );
        if(dashf(fname))
            mylogout=YEA;
    }
    if(mylogout)
    {
        logouts=countlogouts(fname); /* logouts Ϊ ��վ���� ���� */
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

    /*if(DEFINE(currentuser,DEF_LOGOUT\*ʹ���Լ�����վ����*\)) Leeward: disable the old code
{
        sethomefile( fname,currentuser->userid, "logout" );
        if(!dashf(fname))
                strcpy(fname,"etc/logout");
}else
        strcpy(fname,"etc/logout");
    if(dashf(fname))
{
        logouts=countlogouts(fname);      \* logouts Ϊ ��վ���� ���� *\
        if(logouts>=1)
        {
                user_display(fname,(logouts==1)?1:
                                   (currentuser->numlogins%(logouts))+1,YEA);
        }
}*/
    report("exit") ;

    /* Leeward 98.04.24 */
    if(strcmp(currentuser->userid,"guest")) /* guest ���� */
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

    /*   stay = time(NULL) - login_start_time;    ��������ʱ��*/
    /*Haohmaru.98.11.10.���ж��Ƿ�����վ��*/
    if(/*strcmp(currentuser->username,"guest")&&*/stay<=Time) {
        char lbuf[256];
        char tmpfile[256];
        FILE* fp;

        strcpy(lbuf,"����-");
        strftime(lbuf+5, 30, "%Y-%m-%d%y:%H:%M", localtime(&login_start_time));
        sprintf(tmpfile,"tmp/.tmp%d",getpid());
        fp = fopen(tmpfile,"w");
        if (fp) {
            fputs(lbuf,fp);
            fclose(fp);
            mail_file(tmpfile,"surr","����");
        }
        unlink(tmpfile);
    }
    /* stephen on 2001.11.1: ��վ����5���Ӳ�������վ���� */
    if (stay<=300 && currentuser->numlogins > 5){
	currentuser->numlogins --;
        if (currentuser->stay>stay) 
            currentuser->stay-=stay;
    }
    if(started) {
        record_exit_time(); /* ��¼�û����˳�ʱ�� Luzi 1998.10.23*/
        /*---	period	2000-10-19	4 debug	---*/
        /*        sprintf( genbuf, "Stay:%3ld (%s)", stay / 60, currentuser->username );*/
        bbslog( "1system", "EXIT: Stay:%3ld (%s)[%d %d]", stay / 60, currentuser->username, utmpent, usernum );
        u_exit() ;
        started = 0;
    }

    if(num_user_logins(currentuser->userid)==0||!strcmp(currentuser->userid,"guest"))/*��黹��û����������*/
    {
        FILE *fp;
        char buf[STRLEN],*ptr;
        sethomefile(fname,currentuser->userid,"msgindex");
        unlink(fname);
        sethomefile(fname,currentuser->userid,"msgfile");
        if(DEFINE(currentuser,DEF_MAILMSG/*��վʱ�Ļ�������Ϣ*/)&&dashf(fname))
        {
            char title[STRLEN];
            time_t now;

            now=time(0);
            sprintf(title,"[%12.12s] ����ѶϢ����",ctime(&now)+4);
            mail_file(fname,currentuser->userid,title);
        }
        unlink(fname);
        if (strcmp(currentuser->userid,"Luzi")) {
            fp=fopen("friendbook","r");  /*����ϵͳ Ѱ������ */
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
                if(!strcmp(uid,currentuser->userid)) /*ɾ�����û��� Ѱ������ */
                    del_from_file("friendbook",buf);/*Ѱ������ֻ�ڱ���������Ч*/
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
Info()                   /* ��ʾ�汾��ϢVersion.Info */
{
    modify_user_mode( XMENU );
    ansimore("Version.Info",YEA) ;
    clear() ;
    return 0 ;
}

int
Conditions()             /* ��ʾ��Ȩ��ϢCOPYING */
{
    modify_user_mode( XMENU );
    ansimore("COPYING",YEA) ;
    clear() ;
    return 0 ;
}

int
ShowWeather()                   /* ��ʾ�汾��ϢVersion.Info */
{
    modify_user_mode( XMENU );
    ansimore("WEATHER",YEA) ;
    clear() ;
    return 0 ;
}
int
Welcome()               /* ��ʾ��ӭ���� Welcome */
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
    getdata(3,0,"(E)�༭ or (D)ɾ�� Welcome? [E]: ",ans,7,DOECHO,NULL,YEA);
    if (ans[0] == 'D' || ans[0] == 'd') {
        unlink("Welcome");
        move(5,0);
        prints("��ɾ��!\n");
        pressreturn();
        clear();
        report( "del welcome" ) ;
        return 0;
    }
    aborted = vedit("Welcome", NA);             
    clear() ;
    if (aborted)
        prints("ȡ���༭.\n");
    else {
        report("edit Welcome") ;
        prints("�޸����.\n") ;
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
cancelpost_old( board, userid, fh, owned ) /* ɾ������ ת�Ƶ�junk��deleted�� */
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
        brd = owned ? "junk" : "deleted"; /*�ж�ɾ���� ��junk��deleted�� */
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
        postfile.filename[ STRLEN - 1 ] = 'D'; /*��ʶ��ɾ��*/
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
            } else if( strncmp( genbuf, "������: ", 8 ) == 0 ) {
                if( (ptr = strrchr( genbuf, ',' )) != NULL )
                    *ptr = '\0';
                strcpy( from, genbuf + 8 );
            } else if( strncmp( genbuf, "ת��վ: ", 8 ) == 0 ) {
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
    /*Haohmaru.98.10.12.����ģʽ�²�����mark delete����*/
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
            prints(" �����б����䶯������[%s]�ѱ�ɾ����\n", fileinfo->title) ;
            clrtobot();
            pressreturn() ;
            return DIRCHANGED;
    }
    newent=substitute_record_comp(direct, fileinfo, sizeof(*fileinfo), ent,fileinfo,strcmp,&mkpost);
    if (newent)
    {
            move(2,0) ;
            prints(" �����б����䶯������[%s]�ѱ�ɾ����\n", fileinfo->title) ;
            clrtobot();
            pressreturn() ;
            return DIRCHANGED;
    }

    return DIRCHANGED;
}
