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
#include "read.h"
#include <time.h>
#include <dlfcn.h>

/*#include "../SMTH2000/cache/cache.h"*/

int scrint = 0;
int local_article = 0;  //0:ת�Ű���Ĭ��ת�ţ�1:��ת�Ű��棬2:ת�Ű���Ĭ�ϲ�ת�� - atppp
int readpost;
int helpmode = 0;
int usernum;
//char currboard[STRLEN - BM_LEN];
struct boardheader* currboard=NULL;
int currboardent;
char currBM[BM_LEN - 1];
int selboard = 0;

int Anony;
char genbuf[1024];
unsigned int tmpuser = 0;
char quote_title[120], quote_board[120];
char quote_user[120];

#ifndef NOREPLY
char replytitle[STRLEN];
#endif

char *filemargin();
#define ARG_NOPROMPT_FLAG   1 /*��������ʾ*/
#define ARG_DELDECPOST_FLAG 2 /*ɾ������Ҫ��������*/
#define ARG_BMFUNC_FLAG 4 /*����������־*/

/* bad 2002.8.1 */

int deny_user();

int m_template();

/*int     b_jury_edit();  stephen 2001.11.1*/
int add_author_friend();
int m_read();                   /*Haohmaru.2000.2.25 */
int Goodbye();

void RemoveAppendedSpace();     /* Leeward 98.02.13 */
extern time_t login_start_time;
extern int cmpbnames();

extern struct screenline *big_picture;
extern struct userec *user_data;
int post_reply(struct _select_def* conf,struct fileheader *fileinfo,void* extraarg);
extern int b_vote(struct _select_def* conf,struct fileheader *fileinfo,void* extraarg);
extern int b_vote_maintain(struct _select_def* conf,struct fileheader *fileinfo,void* extraarg);
extern int b_jury_edit(struct _select_def* conf,struct fileheader *fileinfo,void* extraarg);
extern int mainreadhelp(struct _select_def* conf,struct fileheader *fileinfo,void* extraarg);
static int choose_tmpl_refresh(struct _select_def *conf);
static int choose_tmpl(char *title, char *fname);

int check_readonly(char *checked)
{                               /* Leeward 98.03.28 */
    if (checkreadonly(checked)) {       /* Checking if DIR access mode is "555" */
        if (!strcmp(currboard->filename,checked)) {
            move(0, 0);
            clrtobot();
            move(8, 0);
            prints("                                        "); /* 40 spaces */
            move(8, (80 - (24 + strlen(checked))) / 2); /* Set text in center */
            prints("\033[1m\033[33m�ܱ�Ǹ��\033[31m%s ��Ŀǰ��ֻ��ģʽ\033[33m\n\n                          �������ڸð淢������޸�����\033[m\n", checked);
            pressreturn();
            clear();
        }
        return true;
    } else
        return false;
}

int insert_func(int fd, struct fileheader *start, int ent, int total, struct fileheader *data, bool match)
{
    int i;
    struct fileheader UFile;

    if (match)
        return 0;
    UFile = start[total - 1];
    for (i = total - 1; i >= ent; i--)
        start[i] = start[i - 1];
    lseek(fd, 0, SEEK_END);
    if (safewrite(fd, &UFile, sizeof(UFile)) == -1)
        bbslog("user", "%s", "apprec write err!");
    start[ent - 1] = *data;
    return ent;
}

/* undelete һƪ���� Leeward 98.05.18 */
/* modified by ylsdd */
int UndeleteArticle(struct _select_def* conf,struct fileheader *fileinfo,void* extraarg)
{
    char *p, buf[1024];
    char UTitle[128];
    struct fileheader UFile;
    int i;
    FILE *fp;
    int fd;
    struct read_arg* arg=(struct read_arg*)conf->arg;

    if (fileinfo==NULL)
        return DONOTHING;
    if ((arg->mode!= DIR_MODE_JUNK)&& (arg->mode != DIR_MODE_DELETED))
        return DONOTHING;
    if (!chk_currBM(currBM, getCurrentUser()))
        return DONOTHING;

    sprintf(buf, "boards/%s/%s", currboard->filename, fileinfo->filename);
    if (!dashf(buf)) {
        clear();
        move(2, 0);
        prints("�����²����ڣ��ѱ��ָ�, ɾ�����б����");
        pressreturn();
        return FULLUPDATE;
    }
    fp = fopen(buf, "r");
    if (!fp)
        return DONOTHING;


    strcpy(UTitle, fileinfo->title);
    if ((p = strrchr(UTitle, '-')) != NULL) {   /* create default article title */
        *p = 0;
        for (i = strlen(UTitle) - 1; i >= 0; i--) {
            if (UTitle[i] != ' ')
                break;
            else
                UTitle[i] = 0;
        }
    }

    i = 0;
    while (!feof(fp) && i < 2) {
        skip_attach_fgets(buf, 1024, fp);
        if (feof(fp))
            break;
        if (strstr(buf, "������: ") && strstr(buf, "), ����: ")) {
            i++;
        } else if (strstr(buf, "��  ��: ")) {
            i++;
            strcpy(UTitle, buf + 8);
            if ((p = strchr(UTitle, '\n')) != NULL)
                *p = 0;
        }
    }
    fclose(fp);

    bzero(&UFile, sizeof(UFile));
    strcpy(UFile.owner, fileinfo->owner);
    strncpy(UFile.title, UTitle, ARTICLE_TITLE_LEN - 1);
	UFile.title[ARTICLE_TITLE_LEN - 1] = '\0';
    strcpy(UFile.filename, fileinfo->filename);
    UFile.attachment=fileinfo->attachment;
    UFile.accessed[0]=fileinfo->accessed[0];
    UFile.accessed[1]=fileinfo->accessed[1]&(~FILE_DEL);

    if (UFile.filename[1] == '/')
        UFile.filename[2] = 'M';
    else
        UFile.filename[0] = 'M';
    UFile.id = fileinfo->id;
    UFile.groupid = fileinfo->groupid;
    UFile.reid = fileinfo->reid;
    set_posttime2(&UFile, fileinfo);

    setbfile(genbuf, currboard->filename, fileinfo->filename);
    setbfile(buf, currboard->filename, UFile.filename);
    f_mv(genbuf, buf);

    sprintf(buf, "boards/%s/.DIR", currboard->filename);
    if ((fd = open(buf, O_RDWR | O_CREAT, 0644)) != -1) {
        if ((UFile.id == 0) || mmap_search_apply(fd, &UFile, insert_func) == 0) {
            flock(fd, LOCK_EX);
            if (UFile.id == 0) {
                UFile.id = get_nextid(currboard->filename);
                UFile.groupid = UFile.id;
                UFile.groupid = UFile.id;
            }
            lseek(fd, 0, SEEK_END);
            if (safewrite(fd, &UFile, sizeof(UFile)) == -1)
                bbslog("user", "%s", "apprec write err!");
            flock(fd, LOCK_UN);
        }
        close(fd);
    }

    updatelastpost(currboard->filename);
    fileinfo->filename[0] = '\0';
    substitute_record(arg->direct, fileinfo, sizeof(*fileinfo), conf->pos);
    sprintf(buf, "undeleted %s's ��%s�� on %s", UFile.owner, UFile.title, currboard->filename);
    bbslog("user", "%s", buf);

    clear();
    move(2, 0);
    prints("'%s' �ѻָ������� \n", UFile.title);
    pressreturn();
    bmlog(getCurrentUser()->userid, currboard->filename, 9, 1);

    return FULLUPDATE;
}

int check_stuffmode()
{
    if (uinfo.mode == RMAIL)
        return true;
    else
        return false;
}

/*Add by SmallPig*/
int shownotepad()
{                               /* ��ʾ notepad */
    modify_user_mode(NOTEPAD);
    ansimore("etc/notepad", true);
    clear();
    return 1;
}

void printutitle()
{                               /* ��Ļ��ʾ �û��б� title */
    /*---	modified by period	2000-11-02	hide posts/logins	---*/
#ifndef _DETAIL_UINFO_
    int isadm;
    const char *fmtadm = "#��վ #����", *fmtcom = "           ";

    isadm = HAS_PERM(getCurrentUser(), PERM_ADMINMENU);
#endif

    move(2, 0);
    clrtoeol();
    prints(
#ifdef _DETAIL_UINFO_
              "\033[44m �� ��  ʹ���ߴ���     %-19s  #��վ #���� %4s    �����������   \033[m\n",
#else
              "\033[44m �� ��  ʹ���ߴ���     %-19s  %11s %4s    �����������   \033[m\n",
#endif
#if defined(ACTS_REALNAMES)
              "��ʵ����",
#else
              "ʹ�����ǳ�",
#endif
#ifndef _DETAIL_UINFO_
              isadm ? fmtadm : fmtcom,
#endif
              "���");
}


int g_board_names(struct boardheader *fhdrp,void* arg)
{
    if (check_read_perm(getCurrentUser(), fhdrp)) {
        AddNameList(fhdrp->filename);
    }
    return 0;
}

void make_blist(int addfav)
{                               /* ���а� ���� �б� */
    CreateNameList();
    apply_boards((int (*)()) g_board_names,NULL);
	if(addfav){
		int i;
		for( i=0; i<bdirshm->allbrd_list_t; i++){
			if( bdirshm->allbrd_list[i].ename[0] && HAS_PERM(getCurrentUser(), bdirshm->allbrd_list[i].level) )
				AddNameList( bdirshm->allbrd_list[i].ename );
		}
	}
}

int Select()
{
    do_select(0, NULL, NULL);
    return 0;
}

int Post()
{                               /* ���˵��ڵ� �ڵ�ǰ�� POST ���� */
    if (!selboard) {
        prints("\n\n���� (S)elect ȥѡ��һ����������\n");
        pressreturn();          /* �ȴ���return�� */
        clear();
        return 0;
    }
#ifndef NOREPLY
    *replytitle = '\0';
#endif
    do_post(NULL,NULL,NULL);
    return 0;
}

int get_a_boardname(char *bname, char *prompt)
{                               /* ����һ������ */
    /*
     * struct boardheader fh; 
     */

    make_blist(0);
    namecomplete(prompt, bname);        /* �����Զ����� */
    if (*bname == '\0') {
        return 0;
    }
    /*---	Modified by period	2000-10-29	---*/
    if (getbnum(bname) <= 0)
        /*---	---*/
    {
        move(1, 0);
        prints("���������������\n");
        pressreturn();
        move(1, 0);
        return 0;
    }
    return 1;
}

int set_article_flag(struct _select_def* conf,struct fileheader *fileinfo,long flag)
{
    struct read_arg* arg=(struct read_arg*)conf->arg;
    bool    isbm=chk_currBM(currboard->BM, getCurrentUser());
    struct write_dir_arg dirarg;
    struct fileheader data;
    int ret;
    struct actionlist{
        int action;
        int pos;
        long flag;
        char* name;
    };
    const struct actionlist *ptr;
    static const struct actionlist flaglist[]= {
            {FILE_MARK_FLAG,0,FILE_MARKED,"���m"},
            {FILE_NOREPLY_FLAG,1,FILE_READ,"����Re"},
            {FILE_SIGN_FLAG,0,FILE_SIGN,"���#"},
            {FILE_DELETE_FLAG,1,FILE_DEL,"���#"},
            {FILE_DIGEST_FLAG,0,FILE_DIGEST,"��ժ"},
            {FILE_TITLE_FLAG,0,0,"ԭ��"},
            {FILE_IMPORT_FLAG,0,FILE_IMPORTED,"���뾫����"},
#ifdef FILTER
            {FILE_CENSOR_FLAG,0,0,"���ͨ��"},
#endif
#ifdef COMMEND_ARTICLE
            {FILE_COMMEND_FLAG,1,FILE_COMMEND,"���ͨ��"},
#endif
            {0,0,0,NULL}
    };
    if (fileinfo==NULL)
        return DONOTHING;
    
#ifdef FILTER
#ifdef SMTH
    if (!strcmp(currboard->filename,"NewsClub")&&haspostperm(getCurrentUser(), currboard->filename)) 
            isbm=true;
#endif
#endif
    
    if (!isbm)
        return DONOTHING;
    data=*fileinfo;
    init_write_dir_arg(&dirarg);
    dirarg.fd=arg->fd;
    dirarg.ent = conf->pos;
    ptr=flaglist;
    while (ptr->action!=0) {
        if (ptr->action==flag) {
            data.accessed[ptr->pos] = (fileinfo->accessed[ptr->pos] & ptr->flag)?0 : ptr->flag;
            break;
        }
        ptr++;
    }
    if (ptr->action!=0) {
        ret=change_post_flag(&dirarg, arg->mode, currboard,  
            fileinfo,flag, &data,isbm,getSession());
        if (ret==0) {
//prompt...
            ret=DIRCHANGED;
        } else {
            char buf[STRLEN];
            a_prompt(-1, "����ʧ��, �밴 Enter ���� << ", buf);
            ret=FULLUPDATE;
        }
    } else
        ret=DONOTHING;
    free_write_dir_arg(&dirarg);
    return ret;
}

#ifdef COMMEND_ARTICLE
/* stiger, �Ƽ����� */
int do_commend(struct _select_def* conf,struct fileheader *fileinfo,void* extraarg)
{

    char ispost[3];
    struct read_arg* arg=conf->arg;

    if (fileinfo==NULL)
        return DONOTHING;
    if (arg->mode!=DIR_MODE_NORMAL) {
        return DONOTHING;
    }
    if ((fileinfo->accessed[1] & FILE_COMMEND) && !HAS_PERM(getCurrentUser(), PERM_SYSOP)) {
        clear();
        move(1, 0);
        prints("�������Ѿ��Ƽ�������л���������Ƽ�");
        move(2, 0);
        pressreturn();
        return FULLUPDATE;
    }
    if( ! normal_board(currboard->filename) ){
        clear();
        move(1, 0);
        prints("�Բ��������Ƽ��ڲ���������");
        move(2, 0);
        pressreturn();
        return FULLUPDATE;
    }
    if ( deny_me(getCurrentUser()->userid, COMMEND_ARTICLE) ) {
        clear();
        move(1, 0);
        prints("�Բ�������ֹͣ���Ƽ���Ȩ��");
        move(2, 0);
        pressreturn();
        return FULLUPDATE;
    }

    clear();
    move(0, 0);
    prints("�Ƽ� ' %s '", fileinfo->title);
    clrtoeol();
    move(1, 0);
    getdata(1, 0, "ȷ��? [y/N]: ", ispost, 2, DOECHO, NULL, true);
    if (ispost[0] == 'y' || ispost[0] == 'Y') {
        if (post_commend(getCurrentUser(), currboard->filename, fileinfo ) == -1) {
        	prints("�Ƽ�ʧ�ܣ�ϵͳ���� \n");
            pressreturn();
            move(2, 0);
            return FULLUPDATE;
        }
        move(2, 0);
        prints("�Ƽ��ɹ� \n");
	set_article_flag(conf,fileinfo,FILE_COMMEND_FLAG);
	/*
        fileinfo->accessed[1] |= FILE_COMMEND;
        substitute_record(direct, fileinfo, sizeof(*fileinfo), ent);
	*/
    } else {
        prints("ȡ��");
    }
    move(2, 0);
    pressreturn();
    return FULLUPDATE;
}
#endif	/* COMMEND_ARTICLE */

/* Add by SmallPig */
int do_cross(struct _select_def* conf,struct fileheader *fileinfo,void* extraarg)
{                               /* ת�� һƪ ���� */
    char bname[STRLEN];
    char ispost[10];
    char q_file[STRLEN];
    struct read_arg* arg=(struct read_arg*)conf->arg;
    int outgo_board = true, proceed = false;

    if (fileinfo==NULL)
        return DONOTHING;
    if (!HAS_PERM(getCurrentUser(), PERM_POST)) {    /* �ж��Ƿ���POSTȨ */
        return DONOTHING;
    }
#ifndef NINE_BUILD
    if ((fileinfo->accessed[0] & FILE_FORWARDED) && !HAS_PERM(getCurrentUser(), PERM_SYSOP)) {
        clear();
        move(1, 0);
        prints("�������Ѿ�ת����һ�Σ��޷��ٴ�ת��");
        move(2, 0);
        pressreturn();
        return FULLUPDATE;
    }
#endif
    if (uinfo.mode != RMAIL)
        sprintf(q_file, "boards/%s/%s", currboard->filename, fileinfo->filename);
    else
        setmailfile(q_file, getCurrentUser()->userid, fileinfo->filename);
    strcpy(quote_title, fileinfo->title);

    clear();
#ifndef NINE_BUILD
    move(4, 0);                 /* Leeward 98.02.25 */
    prints
        ("\033[1m\033[33m��ע�⣺\033[31m��վվ��涨��ͬ�����ݵ������Ͻ��� 5 (��) ���������������ظ�������\n\nΥ����\033[33m���������»ᱻɾ��֮�⣬������\033[31m��������������µ�Ȩ����\033[33m��ϸ�涨����գ�\n\n    Announce ���վ�棺������ת�����������µĹ涨����\n\n���ҹ�ͬά�� BBS �Ļ�������ʡϵͳ��Դ��лл������\n\n\033[m");
#endif
    move(1, 0);
    if (!get_a_boardname(bname, "������Ҫת��������������: ")) {
        return FULLUPDATE;
    }
#ifndef NINE_BUILD
    if (!strcmp(bname, currboard->filename) && (uinfo.mode != RMAIL)) {
        move(3, 0);
        clrtobot();
        prints("\n\n                          ��������²���Ҫת��������!");
        pressreturn();
        clear();
        return FULLUPDATE;
    }
#endif
    {                           /* Leeward 98.01.13 ���ת����������ת���İ����Ƿ񱻽�ֹ�� POST Ȩ */
        struct boardheader* bh;

        bh=getbcache(bname);
        if ((fileinfo->attachment!=0)&&!(bh->flag&BOARD_ATTACH)) {
            move(3, 0);
            clrtobot();
            prints("\n\n                �ܱ�Ǹ���ð��治�ܷ��������������...\n");
            pressreturn();
            clear();
            return FULLUPDATE;
        }
        if (deny_me(getCurrentUser()->userid, bname) ) {    /* ������ֹPOST ��� */
			if( HAS_PERM(getCurrentUser(), PERM_SYSOP) ){
				char buf[3];

				clear();
            	getdata(3, 0, "��������ֹͣ��POSTȨ��,ǿ��ת����[y/N]: ", buf, 2, DOECHO, NULL, true);
				if( buf[0]!='y' && buf[0]!='Y' ){
					clear();
					return FULLUPDATE;
				}
			}else{
            	move(3, 0);
            	clrtobot();
            	prints("\n\n                �ܱ�Ǹ�����ڸð汻�����ֹͣ�� POST ��Ȩ��...\n");
            	pressreturn();
            	clear();
            	return FULLUPDATE;
			}
        }
		if (true == check_readonly(bname)) { /* Leeward 98.03.28 */
            return FULLUPDATE;
        }
        outgo_board = (bh->flag & BOARD_OUTFLAG);
    }

    move(0, 0);
    prints("ת�� ' %s ' �� %s �� ", quote_title, bname);
    clrtoeol();
    move(1, 0);
    if (outgo_board) {
        getdata(1, 0, "(S)ת�� (L)��վ (A)ȡ��? [A]: ", ispost, 9, DOECHO, NULL, true);
        proceed = (ispost[0] == 's' || ispost[0] == 'S' || ispost[0] == 'L' || ispost[0] == 'l');
    } else {
        getdata(1, 0, "(L)��վ (A)ȡ��? [A]: ", ispost, 9, DOECHO, NULL, true);
        proceed = (ispost[0] == 'L' || ispost[0] == 'l');
    }
    if (proceed) {
        /*add by stiger
        if(conf->pos<=arg->filecount) {
            int i=0;
            for (i = conf->pos-arg->filecount; i > 0; i--) {
                if (0 == strcmp(arg->boardstatus->topfh[i].filename, fileinfo->filename)) {
                    conf->new_pos=i;
                    break;
                }
           }
	    if (i==0){
                move(2, 0);
	        prints("�����б����仯��ȡ��");
		move(2,0);
		pressreturn();
		return FULLUPDATE;
	    }
        } else conf->new_pos=conf->pos; */
        /*add old*/
        if (post_cross(getCurrentUser(), bname, currboard->filename, 
            quote_title, q_file, Anony, 
            arg->mode==DIR_MODE_MAIL?1:0, 
            ispost[0], 0, getSession()) == -1) { /* ת�� */
            pressreturn();
            move(2, 0);
            return FULLUPDATE;
        }
        move(2, 0);
        prints("' %s ' ��ת���� %s �� \n", quote_title, bname);
        fileinfo->accessed[0] |= FILE_FORWARDED;        /*added by alex, 96.10.3 */
	if(conf->pos<=arg->filecount)
            substitute_record(arg->direct, fileinfo, sizeof(*fileinfo), conf->pos);
    /* �ö������Զ��ת��Ӧ��û�������...
        else
            substitute_record(arg->dingdirect, fileinfo, sizeof(*fileinfo), conf->new_pos);
    */
        conf->new_pos=0;
    } else {
        prints("ȡ��");
    }
    move(2, 0);
    pressreturn();
    return FULLUPDATE;
}


void readtitle(struct _select_def* conf)
{                               /* ���� ��ʾ�����б� �� title */
    struct boardheader bp;
    struct BoardStatus * bs;
    char header[STRLEN*2], title[STRLEN];
    char readmode[10];
    int chkmailflag = 0;
    int bnum;
    struct read_arg* arg=(struct read_arg*)conf->arg;

    bnum = getboardnum(currboard->filename,&bp);
    bs = getbstatus(bnum);
    memcpy(currBM, bp.BM, BM_LEN - 1);
    if (currBM[0] == '\0' || currBM[0] == ' ') {
        strcpy(header, "����������");
    } else {
        //if (HAS_PERM(getCurrentUser(), PERM_OBOARDS)) {
		if ( chk_currBM(currBM, getCurrentUser()) ) {
            char *p1, *p2;

            strcpy(header, "����: ");
            p1 = currBM;
            p2 = p1;
            while (1) {
                if ((*p2 == ' ') || (*p2 == 0)) {
                    int end;

                    end = 0;
                    if (p1 == p2) {
                        if (*p2 == 0)
                            break;
                        p1++;
                        p2++;
                        continue;
                    }
                    if (*p2 == 0)
                        end = 1;
                    *p2 = 0;
                    if (apply_utmp(NULL, 1, p1, NULL)) {
                        sprintf(genbuf, "\x1b[32m%s\x1b[33m ", p1);
                        strcat(header, genbuf);
                    } else {
                        strcat(header, p1);
                        strcat(header, " ");
                    }
                    if (end)
                        break;
                    p1 = p2 + 1;
                    *p2 = ' ';
                }
                p2++;
            }
        } else {
            sprintf(header, "����: %s", currBM);
        }
    }
    chkmailflag = chkmail();
    if (chkmailflag == 2)       /*Haohmaru.99.4.4.������Ҳ������ */
        strcpy(title, "[�������䳬������,����������!]");
    else if (chkmailflag)       /* �ż���� */
        strcpy(title, "[�����ż�]");
    else if ((bp.flag & BOARD_VOTEFLAG))       /* ͶƱ��� */
        sprintf(title, "ͶƱ�У��� V ����ͶƱ");
    else
        strcpy(title, bp.title + 13);

    showtitle(header, title);   /* ��ʾ ��һ�� */
    update_endline();
    move(1, 0);
    clrtoeol();
    if (DEFINE(getCurrentUser(), DEF_HIGHCOLOR))
        prints
            ("�뿪[\x1b[1;32m��\x1b[m,\x1b[1;32me\x1b[m] ѡ��[\x1b[1;32m��\x1b[m,\x1b[1;32m��\x1b[m] �Ķ�[\x1b[1;32m��\x1b[m,\x1b[1;32mr\x1b[m] ��������[\x1b[1;32mCtrl-P\x1b[m] ����[\x1b[1;32md\x1b[m] ����¼[\x1b[1;32mTAB\x1b[m] ����[\x1b[1;32mh\x1b[m]\033[m");
    else
        prints("�뿪[��,e] ѡ��[��,��] �Ķ�[��,r] ��������[Ctrl-P] ����[d] ����¼[TAB] ����[h]\x1b[m");
    if (arg->mode== DIR_MODE_NORMAL)        /* �Ķ�ģʽ */
        strcpy(readmode, "һ��");
    else if (arg->mode== DIR_MODE_DIGEST)
        strcpy(readmode, "��ժ");
    else if (arg->mode== DIR_MODE_THREAD)
        strcpy(readmode, "����");
    else if (arg->mode== DIR_MODE_MARK)
        strcpy(readmode, "����");
    else if (arg->mode== DIR_MODE_DELETED)
        strcpy(readmode, "����");
    else if (arg->mode== DIR_MODE_JUNK)
        strcpy(readmode, "ֽ¦");
    else if (arg->mode== DIR_MODE_ORIGIN)
        strcpy(readmode, "ԭ��");
    else if (arg->mode== DIR_MODE_AUTHOR)
        strcpy(readmode, "����");
    else if (arg->mode== DIR_MODE_TITLE)
        strcpy(readmode, "����");
    else if (arg->mode== DIR_MODE_SUPERFITER)
        strcpy(readmode, "����");

    move(2, 0);
    setfcolor(WHITE, DEFINE(getCurrentUser(), DEF_HIGHCOLOR));
    setbcolor(BLUE);
    clrtoeol();
    prints(" ���   %-12s %6s %s", 
        "�� �� ��", "��  ��", " ���±���", bs->currentusers, readmode);
    sprintf(title, "����:%4d [%4sģʽ]", bs->currentusers, readmode);
    move(2, -strlen(title)-1);
    prints("%s", title);
    resetcolor();
}

#ifdef COLOR_ONLINE

int count_login(struct user_info *uentp, int *arg, int pos)
{
     if (uentp->invisible == 1) {
         return COUNT;
     }
     (*arg)++;
     UNUSED_ARG(pos);
     return COUNT;
}

int isonline(char *s){

	int tuid;
	int logincount=0,seecount=0;
    struct userec *lookupuser;

	if( ! (tuid=getuser(s,&lookupuser)) ){
		return 7;
	}
	logincount = apply_utmp((APPLY_UTMP_FUNC) count_login, 0, lookupuser->userid, &seecount);
	if( logincount == 0 )
		return 7;
	if( seecount != 0 )
		return 2;
    if (HAS_PERM(getCurrentUser(), PERM_SEECLOAK))
		return 6;
	return 7;
}
#endif

char *readdoent(char *buf, int num, struct fileheader *ent,struct fileheader* readfh,struct _select_def* conf)
{                               /* �������б��� ��ʾ һƪ���±��� */
    time_t filetime;
    char date[20];
    char *TITLE;
//	char TITLE[256];
    int type;
    int manager;
    char *typeprefix;
    char *typesufix;
    char attachch;
    struct read_arg * arg=(struct read_arg*)conf->arg;

   /* typesufix = typeprefix = "";*/
   typesufix = typeprefix = ""; 

    manager = chk_currBM(currBM, getCurrentUser());

    type = get_article_flag(ent, getCurrentUser(), currboard->filename, manager,getSession());
    if (manager && (ent->accessed[0] & FILE_IMPORTED)) {        /* �ļ��Ѿ������뾫���� */
        if (type == ' ') {
            typeprefix = "\x1b[42m";
            typesufix = "\x1b[m";
        } else {
            typeprefix = "\x1b[32m";
            typesufix = "\x1b[m";
        }
    }
    filetime = get_posttime(ent);
    if (filetime > 740000000) {
#ifdef HAVE_COLOR_DATE
        struct tm *mytm;
        char *datestr = ctime(&filetime) + 4;

        mytm = localtime(&filetime);
        sprintf(date, "\033[1;%dm%6.6s\033[m", mytm->tm_wday + 31, datestr);
#else
        strncpy(date, ctime(&filetime) + 4, 6);
        date[6] = 0;
#endif
    }
    /*
     * date = ctime( &filetime ) + 4;   ʱ�� -> Ӣ�� 
     */
    else
        /*
         * date = ""; char *���ͱ���, ���ܴ���, modified by dong, 1998.9.19 
         */
        /*
         * { date = ctime( &filetime ) + 4; date = ""; } 
         */
        date[0] = 0;

    /*
     * Re-Write By Excellent 
     */
    if (ent->attachment!=0)
        attachch='@';
    else
        attachch=' ';
    TITLE = ent->title;         /*���±���TITLE */
//	sprintf(TITLE,"%s(%d)",ent->title,ent->eff_size);
    if ((type=='d')||(type=='D')) { //�ö�����
        sprintf(buf, " \x1b[1;33m[��ʾ]\x1b[m %-12.12s %s %c�� %-44.44s ", ent->owner, date, attachch, TITLE);
        return buf;
    }

    if (uinfo.mode != RMAIL && arg->mode != DIR_MODE_DIGEST && arg->mode != DIR_MODE_DELETED && arg->mode != DIR_MODE_JUNK
        && strcmp(currboard->filename, "sysmail")) { /* �·����Ƚ�*/
            if ((ent->groupid != ent->id)&&(arg->mode==DIR_MODE_THREAD||!strncasecmp(TITLE,"Re:",3)||!strncmp(TITLE,"�ظ�:",5))) {      /*Re������ */
                if ((readfh&&readfh->groupid == ent->groupid))     /* ��ǰ�Ķ����� ��ʶ */
                    if (DEFINE(getCurrentUser(), DEF_HIGHCOLOR))
#ifdef COLOR_ONLINE
                        sprintf(buf, " \033[1;36m%4d\033[m %s%c%s \033[1;3%dm%-12.12s\033[m %s\033[1;36m.%c%-44.44s\033[m ", num, typeprefix, type, typesufix, isonline(ent->owner), ent->owner, date, attachch, TITLE);
#else
                        sprintf(buf, " \033[1;36m%4d\033[m %s%c%s %-12.12s %s\033[1;36m.%c%-44.44s\033[m ", num, typeprefix, type, typesufix, ent->owner, date, attachch, TITLE);
#endif
                    else
#ifdef COLOR_ONLINE
                        sprintf(buf, " \033[36m%4d\033[m %s%c%s \033[1;3%dm%-12.12s\033[m %s\033[36m.%c%-44.44s\033[m ", num, typeprefix, type, typesufix, isonline(ent->owner), ent->owner, date, attachch, TITLE);
#else
                        sprintf(buf, " \033[36m%4d\033[m %s%c%s %-12.12s %s\033[36m.%c%-44.44s\033[m ", num, typeprefix, type, typesufix, ent->owner, date, attachch, TITLE);
#endif
                else
#ifdef COLOR_ONLINE
                    sprintf(buf, " %4d %s%c%s \033[1;3%dm%-12.12s\033[m %s %c%-44.44s", num, typeprefix, type, typesufix, isonline(ent->owner), ent->owner, date, attachch, TITLE);
#else
                    sprintf(buf, " %4d %s%c%s %-12.12s %s %c%-44.44s", num, typeprefix, type, typesufix, ent->owner, date, attachch, TITLE);
#endif
            } else {
                if (readfh&&(readfh->groupid == ent->groupid))     /* ��ǰ�Ķ����� ��ʶ */
                    if (DEFINE(getCurrentUser(), DEF_HIGHCOLOR))
#ifdef COLOR_ONLINE
                        sprintf(buf, " \033[1;33m%4d\033[m %s%c%s \033[1;3%dm%-12.12s\033[m %s\033[1;33m.%c�� %-44.44s\033[m ", num, typeprefix, type, typesufix, isonline(ent->owner), ent->owner, date, attachch, TITLE);
#else
                        sprintf(buf, " \033[1;33m%4d\033[m %s%c%s %-12.12s %s\033[1;33m.%c�� %-44.44s\033[m ", num, typeprefix, type, typesufix, ent->owner, date, attachch, TITLE);
#endif
                    else
#ifdef COLOR_ONLINE
                        sprintf(buf, " \033[33m%4d\033[m %s%c%s \033[1;3%dm%-12.12s\033[m %s\033[33m.%c�� %-44.44s\033[m ", num, typeprefix, type, typesufix, isonline(ent->owner), ent->owner, date, attachch, TITLE);
#else
                        sprintf(buf, " \033[33m%4d\033[m %s%c%s %-12.12s %s\033[33m.%c�� %-44.44s\033[m ", num, typeprefix, type, typesufix, ent->owner, date, attachch, TITLE);
#endif
                else
#ifdef COLOR_ONLINE
                    sprintf(buf, " %4d %s%c%s \033[1;3%dm%-12.12s\033[m %s %c�� %-44.44s ", num, typeprefix, type, typesufix, isonline(ent->owner), ent->owner, date, attachch, TITLE);
#else
                    sprintf(buf, " %4d %s%c%s %-12.12s %s %c�� %-44.44s ", num, typeprefix, type, typesufix, ent->owner, date, attachch, TITLE);
#endif
            }

    } else                     /* ���� ��ͬ�����ʶ */
        if (!strncmp("Re:", ent->title, 3)) {   /*Re������ */
            if (readfh&&isThreadTitle(readfh->title, ent->title)) /* ��ǰ�Ķ����� ��ʶ */
                if (DEFINE(getCurrentUser(), DEF_HIGHCOLOR))
#ifdef COLOR_ONLINE
                    sprintf(buf, " \033[1;36m%4d\033[m %s%c%s \033[1;3%dm%-12.12s\033[m %s\033[1;36m.%c%-44.44s\033[m ", num, typeprefix, type, typesufix, isonline(ent->owner), ent->owner, date, attachch, TITLE);
#else
                    sprintf(buf, " \033[1;36m%4d\033[m %s%c%s %-12.12s %s\033[1;36m.%c%-44.44s\033[m ", num, typeprefix, type, typesufix, ent->owner, date, attachch, TITLE);
#endif
                else
#ifdef COLOR_ONLINE
                    sprintf(buf, " \033[36m%4d\033[m %s%c%s \033[1;3%dm%-12.12s\033[m %s\033[36m.%c%-44.44s\033[m ", num, typeprefix, type, typesufix, isonline(ent->owner), ent->owner, date, attachch, TITLE);
#else
                    sprintf(buf, " \033[36m%4d\033[m %s%c%s %-12.12s %s\033[36m.%c%-44.44s\033[m ", num, typeprefix, type, typesufix, ent->owner, date, attachch, TITLE);
#endif
            else
#ifdef COLOR_ONLINE
                sprintf(buf, " %4d %s%c%s \033[1;3%dm%-12.12s\033[m %s %c%-44.44s", num, typeprefix, type, typesufix, isonline(ent->owner), ent->owner, date, attachch, TITLE);
#else
                sprintf(buf, " %4d %s%c%s %-12.12s %s %c%-44.44s", num, typeprefix, type, typesufix, ent->owner, date, attachch, TITLE);
#endif
        } else {
            if ((readfh!=NULL)&&!strcmp(readfh->title, ent->title))      /* ��ǰ�Ķ����� ��ʶ */
                if (DEFINE(getCurrentUser(), DEF_HIGHCOLOR))
#ifdef COLOR_ONLINE
                    sprintf(buf, " \033[1;33m%4d\033[m %s%c%s \033[1;3%dm%-12.12s\033[m %s\033[1;33m.%c�� %-44.44s\033[m ", num, typeprefix, type, typesufix, isonline(ent->owner), ent->owner, date, attachch, TITLE);
#else
                    sprintf(buf, " \033[1;33m%4d\033[m %s%c%s %-12.12s %s\033[1;33m.%c�� %-44.44s\033[m ", num, typeprefix, type, typesufix, ent->owner, date, attachch, TITLE);
#endif
                else
#ifdef COLOR_ONLINE
                    sprintf(buf, " \033[33m%4d\033[m %s%c%s \033[1;3%dm%-12.12s\033[m %s\033[33m.%c�� %-44.44s\033[m ", num, typeprefix, type, typesufix, isonline(ent->owner), ent->owner, date, attachch, TITLE);
#else
                    sprintf(buf, " \033[33m%4d\033[m %s%c%s %-12.12s %s\033[33m.%c�� %-44.44s\033[m ", num, typeprefix, type, typesufix, ent->owner, date, attachch, TITLE);
#endif
            else
#ifdef COLOR_ONLINE
                sprintf(buf, " %4d %s%c%s \033[1;3%dm%-12.12s\033[m %s %c�� %-44.44s ", num, typeprefix, type, typesufix, isonline(ent->owner), ent->owner, date, attachch, TITLE);
#else
                sprintf(buf, " %4d %s%c%s %-12.12s %s %c�� %-44.44s ", num, typeprefix, type, typesufix, ent->owner, date, attachch, TITLE);
#endif
        }
    return buf;
}

#define SESSIONLEN 9
void get_telnet_sessionid(char* buf,int unum)
{
    static const char encode[] = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ";
    struct user_info* pui=get_utmpent(unum);
    int utmpkey=pui->utmpkey;
    buf[0]=encode[unum%36];
    unum/=36;
    buf[1]=encode[unum%36];
    unum/=36;
    buf[2]=encode[unum%36];

    buf[3]=encode[utmpkey%36];
    utmpkey/=36;
    buf[4]=encode[utmpkey%36];
    utmpkey/=36;
    buf[5]=encode[utmpkey%36];
    utmpkey/=36;
    buf[6]=encode[utmpkey%36];
    utmpkey/=36;
    buf[7]=encode[utmpkey%36];
    utmpkey/=36;
    buf[8]=encode[utmpkey%36];
    utmpkey/=36;

    buf[9]=0;
}

void  board_attach_link(char* buf,int buf_len,long attachpos,void* arg)
{
    struct fileheader* fh=(struct fileheader*)arg;
    char* server=sysconf_str("BBS_WEBDOMAIN");
    if (server==NULL)
        server=sysconf_str("BBSDOMAIN");
    if (attachpos!=-1)
        snprintf(buf,buf_len,"http://%s/bbscon.php?bid=%d&id=%d&ap=%ld",
            server,getbnum(currboard->filename),fh->id,attachpos);
    else
        snprintf(buf,buf_len,"http://%s/bbscon.php?bid=%d&id=%d",
            server,getbnum(currboard->filename),fh->id );
}

int zsend_attach(int ent, struct fileheader *fileinfo, char *direct)
{
    char *t;
    char buf1[512];
    char *ptr, *p;
    off_t size;
    long left;

    if(fileinfo->attachment==0) return -1;
    strcpy(buf1, direct);
    if ((t = strrchr(buf1, '/')) != NULL)
        *t = '\0';
    snprintf(genbuf, 512, "%s/%s", buf1, fileinfo->filename);
    BBS_TRY {
        if (safe_mmapfile(genbuf, O_RDONLY, PROT_READ, MAP_SHARED, (void **) &ptr, &size, NULL) == 0) {
            BBS_RETURN(-1);
        }
        for (p=ptr,left=size;left>0;p++,left--) {
            long attach_len;
            char* file,*attach;
            FILE* fp;
            char name[100];
            if (NULL !=(file = checkattach(p, left, &attach_len, &attach))) {
                left-=(attach-p)+attach_len-1;
                p=attach+attach_len-1;
#if USE_TMPFS==1
                setcachehomefile(name, getCurrentUser()->userid,utmpent, "attach.tmp");
#else
                gettmpfilename(name, "attach%06d", rand()%100000);
#endif
                fp=fopen(name, "wb");
                fwrite(attach, 1, attach_len, fp);
                fclose(fp);
                if (bbs_zsendfile(name, file)==-1) {
                    unlink(name);
                    break;
                }
                unlink(name);
                continue;
            }
        }
    }
    BBS_CATCH {
    }
    BBS_END end_mmapfile((void *) ptr, size, -1);
    return 0;
}

int read_post(struct _select_def* conf,struct fileheader *fileinfo,void* extraarg)
{
    char *t;
    char buf[512];
    int ch;
    int ent=conf->pos;
    struct read_arg* arg=conf->arg;
    int ret=FULLUPDATE;

    if (fileinfo==NULL)
        return DONOTHING;
    /* czz 2003.3.4 forbid reading cancelled post in board */
    if ((fileinfo->owner[0] == '-') 
        && (arg->readmode != DIR_MODE_DELETED) 
        && (arg->readmode != DIR_MODE_JUNK) )
	    return DONOTHING;

    clear();
    strcpy(buf, read_getcurrdirect(conf));
    if ((t = strrchr(buf, '/')) != NULL)
        *t = '\0';
    sprintf(genbuf, "%s/%s", buf, fileinfo->filename);
/*
    strcpy(quote_file, genbuf);
*/
    strcpy(quote_board, currboard->filename);
    strncpy(quote_title, fileinfo->title, ARTICLE_TITLE_LEN);
/*    quote_file[119] = fileinfo->filename[STRLEN - 2];
*/
    strncpy(quote_user, fileinfo->owner, OWNER_LEN);
    quote_user[OWNER_LEN - 1] = 0;

    register_attach_link(board_attach_link, fileinfo);
#ifndef NOREPLY
    ch = ansimore_withzmodem(genbuf, false, fileinfo->title);   /* ��ʾ�������� */
#else
    ch = ansimore_withzmodem(genbuf, true, fileinfo->title);    /* ��ʾ�������� */
#endif
    register_attach_link(NULL,NULL);
#ifdef HAVE_BRC_CONTROL
    brc_add_read(fileinfo->id,getSession());
#endif
#ifndef NOREPLY
    move(t_lines - 1, 0);
    switch (arg->readmode) {
    case READ_THREAD:
        if (DEFINE(getCurrentUser(), DEF_HIGHCOLOR))
	        prints("\x1b[44m\x1b[1;31m[�����Ķ�] \x1b[33m ���� R �� ���� Q,�� ����һ�� ������һ�� <Space>,��");
        else
	        prints("\x1b[44m\x1b[31m[�����Ķ�] \x1b[33m ���� R �� ���� Q,�� ����һ�� ������һ�� <Space>,��");
        break;
    case READ_AUTHOR:
        if (DEFINE(getCurrentUser(), DEF_HIGHCOLOR))
	        prints("\x1b[44m\x1b[1;31m[ͬ�����Ķ�] \x1b[33m ���� R �� ���� Q,�� ����һ�� ������һ�� <Space>,��");
        else
	        prints("\x1b[44m\x1b[31m[ͬ�����Ķ�] \x1b[33m ���� R �� ���� Q,�� ����һ�� ������һ�� <Space>,��");
        break;
    default:
        if (DEFINE(getCurrentUser(), DEF_HIGHCOLOR))
            prints("\033[44m\033[1;31m[�Ķ�����] \033[33m ���� R �� ���� Q,�� ����һ�� ������һ�� <Space>,���������Ķ� ^X��p ");
        else
            prints("\033[44m\033[31m[�Ķ�����] \033[33m ���� R �� ���� Q,�� ����һ�� ������һ�� <Space>,���������Ķ� ^X��p ");
    }

    clrtoeol();                 /* ��������β */
    resetcolor();

    setreadpost(conf, fileinfo);

    if (!(ch == KEY_RIGHT || ch == KEY_UP || ch == KEY_PGUP
        || ch == KEY_DOWN) && (ch <= 0 || strchr("RrEexp", ch) == NULL))
reget:
        ch = igetkey();

    switch (ch) {
    case Ctrl('Z'):
        r_lastmsg();            /* Leeward 98.07.30 support msgX */
        break;
    case Ctrl('Y'):
        read_zsend(conf, fileinfo, NULL);
        break;
    case Ctrl('D'):
        zsend_attach(ent, fileinfo, read_getcurrdirect(conf));
        break;
    case 'Q':
    case 'q':
    case KEY_LEFT:
        break;
    case KEY_REFRESH:
        break;
    case ' ':
    case 'j':
    case 'n':
    case KEY_DOWN:
    case KEY_PGDN:
        return READ_NEXT;
    case KEY_UP:
    case KEY_PGUP:
    case 'k':
    case 'l':
        return READ_PREV;
    case 'Y':
    case 'R':
    case 'y':
    case 'r':
        if ( currboard->flag & BOARD_NOREPLY ) 
        {
            clear();
            move(3, 0);
            clrtobot();
            prints("\n\n                    �ܱ�Ǹ���ð���ܷ�������,���ܻ�����...\n");
            pressreturn();
            break;              /*Haohmaru.98.12.19,���ܻ����µİ� */
        }
        if (fileinfo->accessed[1] & FILE_READ) {        /*Haohmaru.99.01.01.���²���re */
            clear();
            move(3, 0);
            prints("\n\n            �ܱ�Ǹ�������Ѿ�����Ϊ����reģʽ,�벻Ҫ��ͼ���۱���...\n");
            pressreturn();
            break;
        }
        do_reply(conf,fileinfo);
        ret=DIRCHANGED;
        break;
    case Ctrl('R'):
        post_reply(conf, fileinfo, extraarg);      /* ������ */
        break;
    case 'g':
        ret=set_article_flag(conf , fileinfo, FILE_DIGEST_FLAG);       /* Leeward 99.03.02 */
        break;
    case 'M':
        ret=set_article_flag(conf , fileinfo, FILE_MARK_FLAG);       /* Leeward 99.03.02 */
        break;
    case Ctrl('U'):
        if (arg->readmode==READ_NORMAL) {
            move(t_lines - 1, 0);
            if (DEFINE(getCurrentUser(), DEF_HIGHCOLOR))
                prints("\x1b[44m\x1b[1;31m[ͬ�����Ķ�] \x1b[33m ���� R �� ���� Q,�� ����һ�� ������һ�� <Space>,��");
            else
	        prints("\x1b[44m\x1b[31m[ͬ�����Ķ�] \x1b[33m ���� R �� ���� Q,�� ����һ�� ������һ�� <Space>,��");
            clrtoeol();
            arg->readmode=READ_AUTHOR;
            arg->oldpos=0;
            goto reget;
        } else return READ_NEXT;
    case Ctrl('H'):
        if (arg->readmode==READ_NORMAL) {
            move(t_lines - 1, 0);
            if (DEFINE(getCurrentUser(), DEF_HIGHCOLOR))
                prints("\x1b[44m\x1b[1;31m[ͬ�����Ķ�] \x1b[33m ���� R �� ���� Q,�� ����һ�� ������һ�� <Space>,��");
            else
	        prints("\x1b[44m\x1b[31m[ͬ�����Ķ�] \x1b[33m ���� R �� ���� Q,�� ����һ�� ������һ�� <Space>,��");
            clrtoeol();
            arg->readmode=READ_AUTHOR;
            arg->oldpos=conf->pos;
            goto reget;
        } else return READ_NEXT;
    case Ctrl('N'):
        list_select_add_key(conf,Ctrl('N'));
        break;
    case Ctrl('S'):
    case 'p':                  /*Add by SmallPig */
        if (arg->readmode==READ_NORMAL) {
           move(t_lines - 1, 0);
           if (DEFINE(getCurrentUser(), DEF_HIGHCOLOR))
	        prints("\x1b[44m\x1b[1;31m[�����Ķ�] \x1b[33m ���� R �� ���� Q,�� ����һ�� ������һ�� <Space>,��");
           else
	        prints("\x1b[44m\x1b[31m[�����Ķ�] \x1b[33m ���� R �� ���� Q,�� ����һ�� ������һ�� <Space>,��");
	   clrtoeol();
            arg->readmode=READ_THREAD;
            arg->oldpos=0;
            goto reget;
        } else return READ_NEXT;
    case Ctrl('X'):            /* Leeward 98.10.03 */
    case KEY_RIGHT:
        if (arg->readmode==READ_NORMAL) {
           move(t_lines - 1, 0);
           if (DEFINE(getCurrentUser(), DEF_HIGHCOLOR))
	        prints("\x1b[44m\x1b[1;31m[�����Ķ�] \x1b[33m ���� R �� ���� Q,�� ����һ�� ������һ�� <Space>,��");
           else
	        prints("\x1b[44m\x1b[31m[�����Ķ�] \x1b[33m ���� R �� ���� Q,�� ����һ�� ������һ�� <Space>,��");
	   clrtoeol();
            arg->readmode=READ_THREAD;
            arg->oldpos=conf->pos;
            goto reget;
        } else return READ_NEXT;
    case Ctrl('Q'):            /*Haohmaru.98.12.05,ϵͳ����Աֱ�Ӳ��������� */
        clear();
        read_showauthorinfo(conf, fileinfo, NULL);
        return READ_NEXT;
        break;
    case Ctrl('W'):            /*cityhunter 00.10.18�쿴������Ϣ */
        clear();
        read_showauthorBM(conf, fileinfo, NULL);
        return READ_NEXT;
        break;
    case Ctrl('O'):
        clear();
        read_addauthorfriend(conf, fileinfo, NULL);
        return READ_NEXT;
    case 'Z':
    case 'z':
        if (!HAS_PERM(getCurrentUser(), PERM_PAGE))
            break;
        read_sendmsgtoauthor(conf, fileinfo, NULL);
        return READ_NEXT;
        break;
    case Ctrl('A'):            /*Add by SmallPig */
        clear();
        read_showauthor(conf, fileinfo, NULL);
        return READ_NEXT;
        break;
    case 'L':
        if (uinfo.mode != LOOKMSGS) {
            show_allmsgs();
            break;
        } else
            return DONOTHING;
    case '!':                  /*Haohmaru 98.09.24 */
        Goodbye();
        break;
    case 'H':                  /* Luzi 1997.11.1 */
        r_lastmsg();
        break;
    case 'w':                  /* Luzi 1997.11.1 */
        if (!HAS_PERM(getCurrentUser(), PERM_PAGE))
            break;
        s_msg();
        break;
    case 'O':
    case 'o':                  /* Luzi 1997.11.1 */
#ifdef NINE_BUILD
    case 'C':
    case 'c':
#endif
        if (!HAS_PERM(getCurrentUser(), PERM_BASIC))
            break;
        t_friends();
        break;
    case 'u':                  /* Haohmaru 1999.11.28 */
        clear();
        modify_user_mode(QUERY);
        t_query(NULL);
        break;
    }
#endif
    if ((ret==FULLUPDATE)&&(arg->oldpos!=0)) {
        conf->new_pos=arg->oldpos;
	arg->oldpos=0;
	ret=SELCHANGE;
	list_select_add_key(conf,KEY_REFRESH);
        arg->readmode=READ_NORMAL;
    }
    return ret;
}

int skip_post(struct _select_def* conf,struct fileheader *fileinfo,void* extraarg)
{
#ifdef HAVE_BRC_CONTROL
    brc_add_read(fileinfo->id,getSession());
#endif
    return GOTO_NEXT;
}

/************
  ��������ѡ��add by stiger
  *****/
#define MAXBOARDSEARCH 100
int super_board_count;
int super_board_now=0;

static int sb_show(struct _select_def *conf, int i)
{
	const struct boardheader *bp;
	int *result = (int *) (conf->arg);
	struct BoardStatus *bptr;

	bp=getboard(result[i-1]);
	if(bp==NULL){
		prints(" ERROR ");
		return SHOW_CONTINUE;
	}
    bptr = getbstatus(result[i-1]);

//	prints(" %2d %-20s %-40s", i, bp->filename, bp->title);
	prints(" %4d%s ", bptr->total, bptr->total > 9999 ? " " : "  " ); 
    prints("%-16s %s%-36s", bp->filename, (bp->flag & BOARD_VOTEFLAG) ? "\033[31;1mV\033[m" : " ",  bp->title+1); 

	return SHOW_CONTINUE;
}

static int sb_prekey(struct _select_def *conf, int *key)
{
    switch (*key) {
    case 'q':
	case 'e':
        *key = KEY_LEFT;
        break;
    case 'p':
    case 'k':
        *key = KEY_UP;
        break;
    case 'N':
        *key = KEY_PGDN;
        break;
    case 'n':
    case 'j':
        *key = KEY_DOWN;
        break;
    }
    return SHOW_CONTINUE;
}

static int sb_key(struct _select_def *conf, int key)
{
	switch(key){
	default:
		break;
	}

	return SHOW_CONTINUE;
}

static int sb_refresh(struct _select_def *conf)
{
    clear();
    docmdtitle("[��������ѡ��]",
               "�˳�[\x1b[1;32m��\x1b[0;37m] ѡ��[\x1b[1;32m��\x1b[0;37m,\x1b[1;32m��\x1b[0;37m] �������[\x1b[1;32mENTER\x1b[0;37m]");
    move(2, 0);
    setfcolor(WHITE, DEFINE(getCurrentUser(), DEF_HIGHCOLOR));
    setbcolor(BLUE);
    clrtoeol();
    prints("  %s ����������        V ��� ת��  %-24s            ", "ȫ�� " , "��  ��  ��  ��");
    resetcolor();
    update_endline();
    return SHOW_CONTINUE;
}

static int sb_getdata(struct _select_def *conf, int pos, int len)
{
    conf->item_count = super_board_count;
    return SHOW_CONTINUE;
}

static int sb_select(struct _select_def *conf)
{
	super_board_now = conf->pos;
	return SHOW_QUIT;
}

static int do_super_board_select(int result[])
{
	POINT *pts;
    struct _select_def grouplist_conf;
    int i;

    pts = (POINT *) malloc(sizeof(POINT) * BBS_PAGESIZE);
    for (i = 0; i < BBS_PAGESIZE; i++) {
        pts[i].x = 2;
        pts[i].y = i + 3;
    }
    bzero(&grouplist_conf, sizeof(struct _select_def));

	grouplist_conf.arg = result;
    grouplist_conf.item_count = super_board_count;
    grouplist_conf.item_per_page = BBS_PAGESIZE;
    grouplist_conf.flag = LF_VSCROLL | LF_BELL | LF_LOOP | LF_MULTIPAGE;
    grouplist_conf.prompt = "��";
    grouplist_conf.item_pos = pts;
    grouplist_conf.title_pos.x = 0;
    grouplist_conf.title_pos.y = 0;
    grouplist_conf.pos = 1;
    grouplist_conf.page_pos = 1;

    grouplist_conf.show_data = sb_show;
    grouplist_conf.pre_key_command = sb_prekey;
    grouplist_conf.key_command = sb_key;
    grouplist_conf.show_title = sb_refresh;
    grouplist_conf.get_data = sb_getdata;
	grouplist_conf.on_select = sb_select;

    list_select_loop(&grouplist_conf);

	free(pts);

	return 0;
}

int super_select_board(char *bname)
{
	int result[MAXBOARDSEARCH];
	char searchname[STRLEN];

	clear();

	bname[0]='\0';
	searchname[0]='\0';

//	while(bname[0]=='\0'){
    	getdata(1, 0, "��������ؼ���: ", searchname, STRLEN-1, DOECHO, NULL, false);
		if (searchname[0] == '\0' || searchname[0]=='\n')
			return 0;
		if( ( super_board_count = fill_super_board (getCurrentUser(),searchname, result, MAXBOARDSEARCH) ) <= 0 ){
			move(5,0);
			prints("û���ҵ��κ���ذ���\n");
			pressanykey();
			return 0;
		}
		super_board_now=0;
		do_super_board_select(result);
		if (super_board_now != 0){
			const struct boardheader *bp;
			bp=getboard(result[super_board_now-1]);
			if (bp==NULL){
				bname[0]='\0';
				return 0;
			}else
				strcpy(bname, bp->filename);
		}
//	}

	return 0;
}

extern int in_do_sendmsg;

int do_select(struct _select_def* conf,struct fileheader *fileinfo,void* extraarg)
        /*
         * ������������ ѡ�������� 
         */
{
    char bname[STRLEN], bpath[STRLEN];
    struct stat st;
    int bid;
    struct read_arg* arg=NULL;
	int addfav=0;
    
    if (conf!=NULL) arg=conf->arg;
	if( extraarg ) addfav = * ( (int *) extraarg ) ;

    move(0, 0);
    prints("ѡ��һ�������� (Ӣ����ĸ��Сд�Կ�,��#�����������)");
    clrtoeol();
    prints("\n������������ (���հ׼��Զ�����): ");
    clrtoeol();

    make_blist(addfav);               /* ��������Board�� �б� */
	in_do_sendmsg=true;
    if ( namecomplete((char *) NULL, bname) == '#' ) { /* ��ʾ���� board �� */
		super_select_board(bname);
	}
	in_do_sendmsg=0;

    if (*bname == '\0')
    	return FULLUPDATE;

	if(addfav){
		bid = EnameInFav(bname,getSession());

		if(bid){
			*((int *)extraarg) = bid;
    		board_setcurrentuser(uinfo.currentboard, -1);
    		uinfo.currentboard = 0;
    		UPDATE_UTMP(currentboard,uinfo);
    		return CHANGEMODE;
		}else
			*((int *)extraarg) = 0;
	}

    setbpath(bpath, bname);
    if (stat(bpath, &st) == -1) { /* �ж�board�Ƿ���� */
        move(2, 0);
        prints("����ȷ��������.");
        clrtoeol();
        pressreturn();
        return FULLUPDATE;
    }
    if (!(st.st_mode & S_IFDIR)) {
        move(2, 0);
        prints("����ȷ��������.");
        clrtoeol();
        pressreturn();
        return FULLUPDATE;
    }

    board_setcurrentuser(uinfo.currentboard, -1);
    uinfo.currentboard = getbnum(bname);
    UPDATE_UTMP(currentboard,uinfo);
    board_setcurrentuser(uinfo.currentboard, 1);
    
    selboard = 1;

    bid = getbnum(bname);

    currboardent=bid;
    currboard=(struct boardheader*)getboard(bid);

#ifdef HAVE_BRC_CONTROL
    brc_initial(getCurrentUser()->userid, bname,getSession());
#endif

    move(0, 0);
    clrtoeol();
    move(1, 0);
    clrtoeol();
    if (arg!=NULL) {
    arg->mode=DIR_MODE_NORMAL;
    setbdir(arg->mode, arg->direct, currboard->filename);     /* direct �趨 Ϊ ��ǰboardĿ¼ */
    }
    return CHANGEMODE;
}

int digest_mode(struct _select_def* conf,struct fileheader *fileinfo,void* extraarg)
{                               /* ��ժģʽ �л� */
    struct read_arg* arg=(struct read_arg*)conf->arg;
    if (arg->mode == DIR_MODE_DIGEST) {
        arg->newmode = DIR_MODE_NORMAL;
        setbdir(arg->newmode, arg->direct, currboard->filename);
    } else {
        arg->newmode = DIR_MODE_DIGEST;
        setbdir(arg->newmode, arg->direct, currboard->filename);
        if (!dashf(arg->direct)) {
            setbdir(arg->mode, arg->direct, currboard->filename);
            return FULLUPDATE;
        }
    }
    return NEWDIRECT;
}

/*stephen : check whether current useris in the list of "jury" 2001.11.1*/
int isJury()
{
    char buf[STRLEN];

    if (!HAS_PERM(getCurrentUser(), PERM_JURY))
        return 0;
    makevdir(currboard->filename);
    setvfile(buf, currboard->filename, "jury");
    return seek_in_file(buf, getCurrentUser()->userid);
}

int deleted_mode(struct _select_def* conf,struct fileheader *fileinfo,void* extraarg)
{
    struct read_arg* arg=(struct read_arg*)conf->arg;
/* Allow user in file "jury" to see deleted area. stephen 2001.11.1 */
    if (!chk_currBM(currBM, getCurrentUser()) && !isJury()) {
        return DONOTHING;
    }

    if (arg->mode == DIR_MODE_DELETED) {
        arg->newmode = DIR_MODE_NORMAL;
        setbdir(arg->newmode, arg->direct, currboard->filename);
    } else {
        arg->newmode = DIR_MODE_DELETED;
        setbdir(arg->newmode, arg->direct, currboard->filename);
        if (!dashf(arg->direct)) {
            arg->mode = DIR_MODE_NORMAL;
            setbdir(arg->mode, arg->direct, currboard->filename);
            return DONOTHING;
        }
    }
    return NEWDIRECT;
}

int generate_mark(struct read_arg* arg)
{
    struct fileheader mkpost;
    struct flock ldata, ldata2;
    int fd, fd2, size = sizeof(fileheader), total, i, count = 0;
    char direct[PATHLEN],normaldirect[PATHLEN];
    char *ptr, *ptr1;
    struct stat buf;

    setbdir(DIR_MODE_MARK, direct, currboard->filename);
    setbdir(DIR_MODE_NORMAL, normaldirect, currboard->filename);
    if ((fd = open(direct, O_WRONLY | O_CREAT, 0664)) == -1) {
        bbslog("3user", "%s", "recopen err");
        return -1;              /* �����ļ���������*/
    }
    ldata.l_type = F_WRLCK;
    ldata.l_whence = 0;
    ldata.l_len = 0;
    ldata.l_start = 0;
    if (fcntl(fd, F_SETLKW, &ldata) == -1) {
        bbslog("3user", "%s", "reclock err");
        close(fd);
        return -1;              /* lock error*/
    }
    /* ��ʼ�������*/
    if (!setboardmark(currboard->filename, -1)) {
        ldata.l_type = F_UNLCK;
        fcntl(fd, F_SETLKW, &ldata);
        close(fd);
        return -1;
    }

    if ((fd2 = open(normaldirect, O_RDONLY, 0664)) == -1) {
        bbslog("3user", "%s", "recopen err");
        ldata.l_type = F_UNLCK;
        fcntl(fd, F_SETLKW, &ldata);
        close(fd);
        return -1;
    }
    fstat(fd2, &buf);
    ldata2.l_type = F_RDLCK;
    ldata2.l_whence = 0;
    ldata2.l_len = 0;
    ldata2.l_start = 0;
    fcntl(fd2, F_SETLKW, &ldata2);
    total = buf.st_size / size;

    BBS_TRY {
        if (safe_mmapfile_handle(fd2,  PROT_READ, MAP_SHARED, (void **) &ptr, & buf.st_size) == 0) {
            ldata2.l_type = F_UNLCK;
            fcntl(fd2, F_SETLKW, &ldata2);
            close(fd2);
            ldata.l_type = F_UNLCK;
            fcntl(fd, F_SETLKW, &ldata);
            close(fd);
            BBS_RETURN(-1);
        }
        ptr1 = ptr;
        for (i = 0; i < total; i++) {
            memcpy(&mkpost, ptr1, size);
            if (mkpost.accessed[0] & FILE_MARKED) {
                write(fd, &mkpost, size);
                count++;
            }
            ptr1 += size;
        }
    }
    BBS_CATCH {
        ldata2.l_type = F_UNLCK;
        fcntl(fd2, F_SETLKW, &ldata2);
        close(fd2);
        ldata.l_type = F_UNLCK;
        fcntl(fd, F_SETLKW, &ldata);
        close(fd);
        end_mmapfile((void *) ptr, buf.st_size, -1);
        BBS_RETURN(-1);
    }
    BBS_END end_mmapfile((void *) ptr, buf.st_size, -1);

    ldata2.l_type = F_UNLCK;
    fcntl(fd2, F_SETLKW, &ldata2);
    close(fd2);
    ftruncate(fd, count * size);

    setboardmark(currboard->filename, 0); /* ���flag*/

    ldata.l_type = F_UNLCK;
    fcntl(fd, F_SETLKW, &ldata);        /* �˳���������*/
    close(fd);
    return 0;
}

int marked_mode(struct _select_def* conf,struct fileheader *fileinfo,void* extraarg)
{
    struct read_arg* arg=NULL;
    if (conf!=NULL)
        arg=(struct read_arg*)conf->arg;
    if (arg==NULL||arg->mode!= DIR_MODE_MARK) {
        if (setboardmark(currboard->filename, -1)) {
            if (generate_mark(arg) == -1) {
                return FULLUPDATE;
            }
        }
        arg->newmode= DIR_MODE_MARK;
        setbdir(arg->newmode, arg->direct, currboard->filename);
        if (!dashf(arg->direct)) {
            arg->mode=DIR_MODE_NORMAL;
            setbdir(DIR_MODE_NORMAL, arg->direct, currboard->filename);
            return FULLUPDATE;
        }
        return NEWDIRECT;
    }
    return DONOTHING;
}

int title_mode(struct _select_def* conf,struct fileheader *fileinfo,void* extraarg)
{
    struct stat st;

    struct read_arg* arg=NULL;
    if (conf!=NULL)
        arg=(struct read_arg*)conf->arg;
    if (!stat("heavyload", &st)) {
        move(t_lines - 1, 0);
        clrtoeol();
        prints("ϵͳ�������أ���ʱ������Ӧ�����Ķ�������...");
        pressanykey();
        return FULLUPDATE;
    }

    arg->newmode = DIR_MODE_THREAD;
    if (setboardtitle(currboard->filename, -1)) {
    	setbdir(arg->newmode, arg->direct, currboard->filename);
        if (gen_title(currboard->filename) == -1) {
            arg->mode=DIR_MODE_NORMAL;
            setbdir(DIR_MODE_NORMAL, arg->direct, currboard->filename);
            return FULLUPDATE;
        }
    }
    setbdir(arg->newmode, arg->direct, currboard->filename);
    if (!dashf(arg->direct)) {
        arg->mode=DIR_MODE_NORMAL;
        setbdir(DIR_MODE_NORMAL, arg->direct, currboard->filename);
        return FULLUPDATE;
    }
    return NEWDIRECT;
}

int junk_mode(struct _select_def* conf,struct fileheader *fileinfo,void* extraarg)
{
    struct read_arg* arg=(struct read_arg*)conf->arg;
    if (!HAS_PERM(getCurrentUser(), PERM_SYSOP)) {
        return DONOTHING;
    }

    if (arg->mode == DIR_MODE_JUNK) {
        arg->newmode = DIR_MODE_NORMAL;
        setbdir(arg->newmode, arg->direct, currboard->filename);
    } else {
        arg->newmode = DIR_MODE_JUNK;
        setbdir(DIR_MODE_JUNK, arg->direct, currboard->filename);
        if (!dashf(arg->direct)) {
            arg->newmode = DIR_MODE_NORMAL;
            setbdir(arg->mode, arg->direct, currboard->filename);
            return DONOTHING;
        }
    }
    return NEWDIRECT;
}

static char search_data[STRLEN];

int search_mode(struct _select_def* conf,struct fileheader *fileinfo,int mode, char *index)
/* added by bad 2002.8.8 search mode*/
{
    int count = 0;
    struct read_arg* arg=(struct read_arg*)conf->arg;

    strncpy(search_data, index, STRLEN);
    arg->newmode=mode;
    setbdir(mode,arg->direct, currboard->filename);
    if (mode == DIR_MODE_ORIGIN && !setboardorigin(currboard->filename, -1)) {
        return NEWDIRECT;
    }

    count = board_regenspecial(currboard->filename, mode, index);

    if (count<0)
	return FULLUPDATE;

    if (count==0) {
        clear();
        move(t_lines-2,0);
        prints("û����������������");
	pressanykey();
	unlink(arg->direct);
	arg->newmode=DIR_MODE_NORMAL;
        setbdir(DIR_MODE_NORMAL, arg->direct, currboard->filename);
    }
    return NEWDIRECT;
}

int search_x(char * b, char * s)
{
    void *hdll;
	typedef int (*iquery_board_func)(char *b, char *s);
	iquery_board_func iquery_board;
    int oldmode;

    oldmode = uinfo.mode;
    modify_user_mode(SERVICES);
    hdll=dlopen("service/libiquery.so",RTLD_LAZY);
    if(hdll)
    {
        __const__ char* error;
        if((iquery_board = (iquery_board_func)dlsym(hdll,"iquery_board"))!=NULL)
            iquery_board(b, s);
        else
        if ((error = dlerror()) != NULL)  {
            clear();
            prints ("%s\n", error);
            pressanykey();
        }
        dlclose(hdll);
    }
    modify_user_mode(oldmode);
    return 0;
}

int change_mode(struct _select_def* conf,struct fileheader *fileinfo,int newmode)
{
    char ans[4];
    char buf[STRLEN], buf2[STRLEN];
    static char title[31] = "";
    struct read_arg* arg=(struct read_arg*)conf->arg;

    if (newmode==0) {
        move(t_lines - 2, 0);
        clrtoeol();
        prints("�л�ģʽ��: 1)��ժ 2)ͬ���� 3)��m���� 4)ԭ�� 5)ͬ���� 6)����ؼ��� ");
        move(t_lines - 1, 0);
        clrtoeol();
        getdata(t_lines - 1, 12, "7)��������ѡ�� 8)���澫�������� [1]: ", ans, 3, DOECHO, NULL, true);
        if (ans[0] == ' ') {
            ans[0] = ans[1];
            ans[1] = 0;
        }
        switch (ans[0]) {
        case '1':
            newmode=DIR_MODE_DIGEST;
            break;
        case '2':
            newmode=DIR_MODE_THREAD;
            break;
        case '3':
            newmode=DIR_MODE_MARK;
            break;
        case '4':
            newmode=DIR_MODE_ORIGIN;
            break;
        case '5':
            newmode=DIR_MODE_AUTHOR;
            move(t_lines - 1, 0);
            clrtoeol();
            move(t_lines - 2, 0);
            clrtoeol();
            sprintf(buf, "���������λ���ѵ�����[%s]: ", fileinfo->owner);
            getdata(t_lines - 1, 0, buf, buf2, 13, DOECHO, NULL, true);
            if (buf2[0])
                strcpy(buf, buf2);
            else
                strcpy(buf, fileinfo->owner);
            if (buf[0] == 0)
                return FULLUPDATE;
            break;
        case '6':
            newmode=DIR_MODE_TITLE;
            move(t_lines - 1, 0);
            clrtoeol();
            move(t_lines - 2, 0);
            clrtoeol();
            sprintf(buf, "������ҵ����±���ؼ���[%s]: ", title);
            getdata(t_lines - 1, 0, buf, buf2, 30, DOECHO, NULL, true);
            if (buf2[0])
                strcpy(title, buf2);
            strcpy(buf, title);
            if (buf[0] == 0)
                return FULLUPDATE;
            break;
        case '7':
            newmode=DIR_MODE_SUPERFITER;
            break;
        case '8':
            move(t_lines - 1, 0);
            clrtoeol();
            move(t_lines - 2, 0);
            clrtoeol();
            sprintf(buf, "������ҵ��������ݹؼ���[%s]: ", title);
            getdata(t_lines - 1, 0, buf, buf2, 70, DOECHO, NULL, true);
            if (buf2[0])
                strcpy(title, buf2);
            strcpy(buf, title);
            if(buf[0]) search_x(currboard->filename, buf);
            return FULLUPDATE;
        default:
            newmode=0;
        }
        if (arg->mode > DIR_MODE_NORMAL&&ans[0]!='7') {
            if (arg->mode==DIR_MODE_AUTHOR|| arg->mode==DIR_MODE_TITLE)
                unlink(arg->direct);
        }
    }
    switch (newmode) {
    case DIR_MODE_NORMAL:
    case DIR_MODE_DIGEST:
        return digest_mode(conf,fileinfo,0);
    case DIR_MODE_THREAD:
        return title_mode(conf,fileinfo,0);
    case DIR_MODE_MARK:
        return marked_mode(conf,fileinfo,0);
    case DIR_MODE_ORIGIN:
        return search_mode(conf,fileinfo,6, buf);
    case DIR_MODE_AUTHOR:
        return search_mode(conf,fileinfo,7, buf);
    case DIR_MODE_TITLE:
        return search_mode(conf,fileinfo, 8, buf);
    case DIR_MODE_SUPERFITER:
        return super_filter(conf,fileinfo,0);
    case DIR_MODE_DELETED:
        return deleted_mode(conf, fileinfo, 0);
    case DIR_MODE_JUNK:
        return junk_mode(conf, fileinfo, 0);
    }
    return DIRCHANGED;
}

int read_hot_info()
{
    char ans[4];
    move(t_lines - 1, 0);
    clrtoeol();
    getdata(t_lines - 1, 0, "ѡ���Ķ�: 1)����ʮ������ 2)ף����У�� 3)�����ȵ� 4)ϵͳ�ȵ� 5)�����ռ�[1]: ", ans, 3, DOECHO, NULL, true);
    switch (ans[0])
	{
    case '2':
        show_help("etc/posts/bless");
        break;
    case '3':
		show_help("0Announce/hotinfo");
        break;
    case '4':
		showsysinfo("0Announce/systeminfo");
        pressanykey();
        break;
    case '5':
            if (getCurrentUser()&&!HAS_PERM(getCurrentUser(),PERM_DENYRELAX))
            exec_mbem("@mod:service/libcalendar.so#calendar_main");
        break;
    case '1':
	default:
		show_help("etc/posts/day");
    }
    return FULLUPDATE;
}

#ifndef NOREPLY
int do_reply(struct _select_def* conf,struct fileheader *fileinfo)
/* reply POST */
{
    char buf[255];

    if (fileinfo==NULL)
        return DONOTHING;
    if (fileinfo->accessed[1] & FILE_READ) {    /*Haohmaru.99.01.01.���²���re */
        clear();
        move(3, 0);
        prints("\n\n            �ܱ�Ǹ�������Ѿ�����Ϊ����reģʽ,�벻Ҫ��ͼ���۱���...\n");
        pressreturn();
        return FULLUPDATE;
    }
    setbfile(buf, currboard->filename, fileinfo->filename);
    strcpy(replytitle, fileinfo->title);
    post_article(conf,buf, fileinfo);
    replytitle[0] = '\0';
    return FULLUPDATE;
}
#endif

int garbage_line(const char *str)
{                               /* �жϱ����Ƿ��� ���õ� */
    int qlevel = 0;

    while (*str == ':' || *str == '>') {
        str++;
        if (*str == ' ')
            str++;
        if (qlevel++ >= QUOTELEV)
            return 1;
    }
    while (*str == ' ' || *str == '\t')
        str++;
    if (qlevel >= QUOTELEV)
        if (strstr(str, "�ᵽ:\n") || strstr(str, ": ��\n") || strncmp(str, "==>", 3) == 0 || strstr(str, "������ ˵"))
            return 1;
    return (*str == '\n');
}

/* When there is an old article that can be included -jjyang */
void do_quote(char *filepath, char quote_mode, char *q_file, char *q_user)
{                               /* �������£� ȫ�ֱ���quote_file,quote_user, */
    FILE *inf, *outf;
    char *qfile, *quser;
    char buf[256], *ptr;
    char op;
    int bflag;
    int line_count = 0;         /* ��Ӽ���ģʽ���� Bigman: 2000.7.2 */

    qfile = q_file;
    quser = q_user;
    bflag = strncmp(qfile, "mail", 4);  /* �ж����õ������»����� */
    outf = fopen(filepath, "a");
    if (outf==NULL) {
    	bbslog("3user","do_quote() fopen(%s):%s",filepath,strerror(errno));
    	return;
    }
    if (*qfile != '\0' && (inf = fopen(qfile, "rb")) != NULL) {  /* �򿪱������ļ� */
        op = quote_mode;
		if (op != 'N') {        /* ����ģʽΪ N ��ʾ ������ */
            skip_attach_fgets(buf, 256, inf);
            /* ȡ����һ���� ���������µ� ������Ϣ */
            if ((ptr = strrchr(buf, ')')) != NULL) {    /* ��һ��':'�����һ�� ')' �е��ַ��� */
                ptr[1] = '\0';
                if ((ptr = strchr(buf, ':')) != NULL) {
                    quser = ptr + 1;
                    while (*quser == ' ')
                        quser++;
                }
            }
            /*---	period	2000-10-21	add '\n' at beginning of Re-article	---*/
            if (bflag)
                fprintf(outf, "\n�� �� %s �Ĵ������ᵽ: ��\n", quser);
            else
                fprintf(outf, "\n�� �� %s ���������ᵽ: ��\n", quser);

            if (op == 'A') {    /* ����һ���⣬ȫ������ */
                while (skip_attach_fgets(buf, 256, inf) != 0) {
                    fprintf(outf, ": %s", buf);
                    if(buf[strlen(buf)-1]!='\n') {
                        char ch;
                        while((ch=fgetc(inf))!=EOF)
                            if(ch=='\n') break;
                    }
                }
            } else if (op == 'R') {
                while (skip_attach_fgets(buf, 256, inf) != 0)
                    if (buf[0] == '\n')
                        break;
                while (skip_attach_fgets(buf, 256, inf) != 0) {
                    if (Origin2(buf))   /* �ж��Ƿ� ������� */
                        continue;
                    fprintf(outf, "%s", buf);

                }
            } else {
                while (skip_attach_fgets(buf, 256, inf) != 0)
                    if (buf[0] == '\n')
                        break;
                while (skip_attach_fgets(buf, 256, inf) != 0) {
                    if (strcmp(buf, "--\n") == 0)       /* ���� ��ǩ����Ϊֹ */
                        break;
                    if(buf[strlen(buf)-1]!='\n') {
                        char ch;
                        while((ch=fgetc(inf))!=EOF)
                            if(ch=='\n') break;
                    }
                    if (buf[250] != '\0')
                        strcpy(buf + 250, "\n");
                    if (!garbage_line(buf)) {   /* �ж��Ƿ��������� */
                        fprintf(outf, ": %s", buf);
#if defined(QUOTED_LINES) && QUOTED_LINES >= 3
                        if (op == 'S') {        /* ����ģʽ,ֻ����ǰ���� Bigman:2000.7.2 */
                            line_count++;
                            if (line_count >= QUOTED_LINES) {
                                fprintf(outf, ": ...................");
                                break;
                            }
                        }
#endif
                    }
                }
            }
        }

        fprintf(outf, "\n");
        fclose(inf);
    }
    /*
     * *q_file = '\0';
     * *q_user = '\0';
     */

    if ((currentmemo->ud.signum > 0) && !(getCurrentUser()->signature == 0 || Anony == 1)) {       /* ǩ����Ϊ0����� */
        if (getCurrentUser()->signature < 0)
            addsignature(outf, getCurrentUser(), (rand() % currentmemo->ud.signum) + 1);
        else
            addsignature(outf, getCurrentUser(), getCurrentUser()->signature);
    }
    fclose(outf);
}

int do_post(struct _select_def* conf,struct fileheader *fileinfo,void* extraarg)
{                               /* �û�post */
#ifndef NOREPLY
    *replytitle = '\0';
#endif
    *quote_user = '\0';
    return post_article(conf,"", NULL);
}

int post_reply(struct _select_def* conf,struct fileheader *fileinfo,void* extraarg)
        /*
         * ���Ÿ�POST���� 
         */
{
    char uid[STRLEN];
    char title[STRLEN];
    char *t;
    FILE *fp;
    char q_file[STRLEN];


    if (fileinfo==NULL)
        return DONOTHING;
    if (!HAS_PERM(getCurrentUser(), PERM_LOGINOK) || !strcmp(getCurrentUser()->userid, "guest"))  /* guest ��Ȩ */
        return 0;
    /*
     * ̫���˰�,����post�Ͳ��û�����
     * if (!HAS_PERM(getCurrentUser(),PERM_POST)) return; Haohmaru.99.1.18 
     */

    /*
     * ���Mail Bigman:2000.8.22 
     */
    if (HAS_PERM(getCurrentUser(), PERM_DENYMAIL)) {
        clear();
        move(3, 10);
        prints("�ܱ�Ǹ,��Ŀǰû��MailȨ��!");
        pressreturn();
        return FULLUPDATE;
    }

    modify_user_mode(SMAIL);

    /*
     * indicate the quote file/user 
     */
    setbfile(q_file, currboard->filename, fileinfo->filename);
    strncpy(quote_user, fileinfo->owner, OWNER_LEN);
    quote_user[OWNER_LEN - 1] = 0;

    /*
     * find the author 
     */
    if (strchr(quote_user, '.')) {
        genbuf[0] = '\0';
        fp = fopen(q_file, "rb");
        if (fp != NULL) {
            skip_attach_fgets(genbuf, 255, fp);
            fclose(fp);
        }

        t = strtok(genbuf, ":");
        if (strncmp(t, "������", 6) == 0 || strncmp(t, "Posted By", 9) == 0 || strncmp(t, "��  ��", 6) == 0) {
            t = (char *) strtok(NULL, " \r\t\n");
            strcpy(uid, t);
        } else {
            prints("Error: Cannot find Author ... \n");
            pressreturn();
        }
    } else
        strcpy(uid, quote_user);

    /*
     * make the title 
     */
    if (toupper(fileinfo->title[0]) != 'R' || fileinfo->title[1] != 'e' || fileinfo->title[2] != ':')
        strcpy(title, "Re: ");
    else
        title[0] = '\0';
    strncat(title, fileinfo->title, ARTICLE_TITLE_LEN - 5);

    clear();

    switch (do_send(uid, title, q_file)) {
    case -1:
        prints("ϵͳ�޷�����\n");
        break;
    case -2:
        prints("���Ŷ����Ѿ���ֹ\n");
        break;
    case -3:
        prints("ʹ���� '%s' �޷�����\n", uid);
        break;
    case -4:
        prints("�Է������������޷�����\n");
        break;
    default:
        prints("�ż��ѳɹ��ؼĸ�ԭ���� %s\n", uid);
    }
	/* �ָ� in_mail ����ԭ����ֵ.
	 * do_send() �������, �ͼ��������, by flyriver, 2003.5.9 */
    pressreturn();
    return FULLUPDATE;
}

int show_board_notes(char bname[30])
{                               /* ��ʾ�����Ļ� */
    char buf[256];

    sprintf(buf, "vote/%s/notes", bname);       /* ��ʾ����İ����Ļ� vote/����/notes */
    if (dashf(buf)) {
        ansimore2(buf, false, 0, 23 /*19 */ );
        return 1;
    } else if (dashf("vote/notes")) {   /* ��ʾϵͳ�Ļ� vote/notes */
        ansimore2("vote/notes", false, 0, 23 /*19 */ );
        return 1;
    }
    return -1;
}

int add_attach(char* file1, char* file2, char* filename)
{
    FILE* fp,*fp2;
    struct stat st;
    uint32_t size;
    char o[8]={0,0,0,0,0,0,0,0};
    char buf[1024*16];
    int i;
    if(stat(file2, &st)==-1)
        return 0;
    if(st.st_size>=2*1024*1024&&!HAS_PERM(getCurrentUser(), PERM_SYSOP)) {
        unlink(file2);
        return 0;
    }
    size=htonl(st.st_size);
    fp2=fopen(file2, "rb");
    if(!fp2) return 0;
    fp=fopen(file1, "ab");
    fwrite(o,1,8,fp);
    for(i=0;i<strlen(filename);i++)
        if(!isalnum(filename[i])&&filename[i]!='.'&&filename[i]>0) filename[i]='A';
    fwrite(filename, 1, strlen(filename)+1, fp);
    fwrite(&size,1,4,fp);
    while((i=fread(buf,1,1024*16,fp2))) {
        fwrite(buf,1,i,fp);
    }
    
    fclose(fp2);
    fclose(fp);
    unlink(file2);
    return st.st_size;
}

int post_article(struct _select_def* conf,char *q_file, struct fileheader *re_file)
{                               /*�û� POST ���� */
    struct fileheader post_file;
    char filepath[STRLEN];
    char buf[256], buf2[256], buf3[STRLEN], buf4[STRLEN];
//	char tmplate[STRLEN];
	int use_tmpl=0;
    int aborted, anonyboard;
    int replymode = 1;          /* Post New UI */
    char ans[4], include_mode = 'S';
    struct boardheader *bp;
    long eff_size;/*����ͳ�����µ���Ч����*/
    char* upload = NULL;
    int mailback = 0;		/* stiger,�ظ������� */

    char direct[PATHLEN];
    int cmdmode;
#ifdef FILTER
    int returnvalue;
#endif

    if (conf!=NULL)  {
        struct read_arg* arg;
        arg=(struct read_arg*)conf->arg;
        cmdmode=arg->mode;
    }
    else {
	cmdmode=DIR_MODE_NORMAL;
    }

    if (true == check_readonly(currboard->filename))      /* Leeward 98.03.28 */
        return FULLUPDATE;

#ifdef AIX_CANCELLED_BY_LEEWARD
    if (true == check_RAM_lack())       /* Leeward 98.06.16 */
        return FULLUPDATE;
#endif

    modify_user_mode(POSTING);
    setbdir(DIR_MODE_NORMAL, direct, currboard->filename);
    if (!((cmdmode == DIR_MODE_MARK)||( cmdmode == DIR_MODE_THREAD)||( cmdmode == DIR_MODE_NORMAL)))
    {
        move(3, 0);
        clrtobot();
        prints("\n\n     Ŀǰ����ժ������ģʽ, ���Բ��ܷ�������.(������뿪��ժģʽ)\n");
        pressreturn();
        clear();
        return FULLUPDATE;
    }
    if (!haspostperm(getCurrentUser(), currboard->filename)) { /* POSTȨ�޼�� */
        move(3, 0);
        clrtobot();
            prints("\n\n        ����������Ψ����, ����������Ȩ���ڴ˷�������.\n");
            prints("        �������δע�ᣬ���ڸ��˹���������ϸע�����\n");
            prints("        δͨ�����ע����֤���û���û�з������µ�Ȩ�ޡ�\n");
            prints("        лл������ :-) \n");
        clear();
        return FULLUPDATE;
    } else if (deny_me(getCurrentUser()->userid, currboard->filename)) { /* ������ֹPOST ��� */
		if( !HAS_PERM(getCurrentUser(), PERM_SYSOP) ){
        	move(3, 0);
        	clrtobot();
        	prints("\n\n                     �ܱ�Ǹ���㱻����ֹͣ�� POST ��Ȩ��...\n");
        	pressreturn();
        	clear();
        	return FULLUPDATE;
		}else{
			clear();
            getdata(3, 0, "��������ֹͣ��POSTȨ��,ǿ�Ʒ�����[y/N]: ", buf, 2, DOECHO, NULL, true);
			if( buf[0]!='y' && buf[0]!='Y' ){
				clear();
				return FULLUPDATE;
			}
		}
    }

    memset(&post_file, 0, sizeof(post_file));
//	tmplate[0]='\0';
    clear();
    show_board_notes(currboard->filename);        /* �����Ļ� */
#ifndef NOREPLY                 /* title�Ƿ���Re: */
    if (replytitle[0] != '\0') {
        buf4[0] = ' ';
        /*
         * if( strncasecmp( replytitle, "Re:", 3 ) == 0 ) Change By KCN:
         * why use strncasecmp? 
         */
        if (strncmp(replytitle, "Re:", 3) == 0)
            strcpy(buf, replytitle);
        else if (strncmp(replytitle, "�� ", 3) == 0)
            sprintf(buf, "Re: %s", replytitle + 3);
        else if (strncmp(replytitle, "�� ", 3) == 0)
            sprintf(buf, "Re: %s", replytitle + 3);
        else
            sprintf(buf, "Re: %s", replytitle);
        buf[78] = '\0';
    } else
#endif
    {
        buf[0] = '\0';
        buf4[0] = '\0';
        replymode = 0;
    }
    if (currentmemo->ud.signum == 0)
        getCurrentUser()->signature = 0;
    else if (getCurrentUser()->signature > currentmemo->ud.signum)      /*ǩ����No.��� */
        getCurrentUser()->signature = 1;
    anonyboard = anonymousboard(currboard->filename);     /* �Ƿ�Ϊ������ */
    if (!strcmp(currboard->filename, "Announce"))
        Anony = 1;
    else if (anonyboard)
        Anony = ANONYMOUS_DEFAULT;
    else
        Anony = 0;

    while (1) {                 /* ����ǰ�޸Ĳ����� ���Կ������'��ʾǩ����' */
        sprintf(buf3, "����ģʽ [%c]", include_mode);
        move(t_lines - 4, 0);
        clrtoeol();
        prints("\033[m���������� %s ������  %s %s %s\n", currboard->filename, (anonyboard) ? (Anony == 1 ? "\033[1mҪ\033[mʹ������" : "\033[1m��\033[mʹ������") : "", mailback? "�ظ�������":"",use_tmpl?"ʹ��ģ��":"");
        clrtoeol();
        prints("ʹ�ñ���: %s\n", (buf[0] == '\0') ? "[�����趨����]" : buf);
        clrtoeol();
        if (getCurrentUser()->signature < 0)
            prints("ʹ�����ǩ����     %s", (replymode) ? buf3 : " ");
        else
            prints("ʹ�õ� %d ��ǩ����     %s", getCurrentUser()->signature, (replymode) ? buf3 : " ");

        if (buf4[0] == '\0' || buf4[0] == '\n') {
            move(t_lines - 1, 0);
            clrtoeol();
            strcpy(buf4, buf);
            getdata(t_lines - 1, 0, "����: ", buf4, 79, DOECHO, NULL, false);
            if ((buf4[0] == '\0' || buf4[0] == '\n')) {
                if (buf[0] != '\0') {
                    buf4[0] = ' ';
                    continue;
                } else
                    return FULLUPDATE;
            }
            strcpy(buf, buf4);
            continue;
        }
        move(t_lines - 1, 0);
        clrtoeol();
        /*
         * Leeward 98.09.24 add: viewing signature(s) while setting post head 
         */
        sprintf(buf2, "��\033[1;32m0\033[m~\033[1;32m%d/V/L\033[mѡ/��/���ǩ����%s��\033[1;32mT\033[m�ı��⣬%s\033[1;32mEnter\033[m���������趨: ", currentmemo->ud.signum,
                (replymode) ? "��\033[1;32mS/Y\033[m/\033[1;32mN\033[m/\033[1;32mR\033[m/\033[1;32mA\033[m ������ģʽ" : "��\033[1;32mP\033[mʹ��ģ��", (anonyboard) ? "\033[1;32mM\033[m������" : "");
        if(replymode&&anonyboard) buf2[strlen(buf2)-10]=0;
        getdata(t_lines - 1, 0, buf2, ans, 3, DOECHO, NULL, true);
        ans[0] = toupper(ans[0]);       /* Leeward 98.09.24 add; delete below toupper */
        if ((ans[0] - '0') >= 0 && ans[0] - '0' <= 9) {
            if (atoi(ans) <= currentmemo->ud.signum)
                getCurrentUser()->signature = atoi(ans);
        } else if ((ans[0] == 'S' || ans[0] == 'Y' || ans[0] == 'N' || ans[0] == 'A' || ans[0] == 'R') && replymode) {
            include_mode = ans[0];
        } else if (ans[0] == 'T') {
            buf4[0] = '\0';
		} else if (ans[0] == 'P') {
			if( use_tmpl >= 0)
				use_tmpl = use_tmpl ? 0 : 1;
				/*
			if( replymode == 0 ){
				choose_tmpl( tmplate );
				clear();
				if( tmplate[0] ){
					setbfile( buf2, currboard->filename, tmplate );
					ansimore2(buf2, false, 0, 18);
				}
			}*/
		} else if (ans[0] == 'B') {
			if( replymode == 0 )
				mailback = mailback ? 0 : 1;
        } else if (ans[0] == 'M') {
            Anony = (Anony == 1) ? 0 : 1;
        } else if (ans[0] == 'L') {
            getCurrentUser()->signature = -1;
        } else if (ans[0] == 'V') {     /* Leeward 98.09.24 add: viewing signature(s) while setting post head */
            sethomefile(buf2, getCurrentUser()->userid, "signatures");
            move(t_lines - 1, 0);
            if (askyn("Ԥ����ʾǰ����ǩ����, Ҫ��ʾȫ����", false) == true)
                ansimore(buf2, 0);
            else {
                clear();
                ansimore2(buf2, false, 0, 18);
            }
        } else if (ans[0] == 'U') {
            struct boardheader* b=currboard;
            if(b->flag&BOARD_ATTACH && use_tmpl<=0) {
                chdir("tmp");
                upload = bbs_zrecvfile();
                chdir("..");
//				use_tmpl = -1;
            }
        } else {
            /*
             * Changed by KCN,disable color title 
             */
            {
                unsigned int i;

                for (i = 0; (i < strlen(buf)) && (i < ARTICLE_TITLE_LEN - 1); i++)
                    if (buf[i] == 0x1b)
                        post_file.title[i] = ' ';
                    else
                        post_file.title[i] = buf[i];
                post_file.title[i] = 0;
            }
            /*
             * strcpy(post_file.title, buf); 
             */
            strncpy(save_title, post_file.title, ARTICLE_TITLE_LEN );
            if (save_title[0] == '\0') {
                return FULLUPDATE;
            }
            break;
        }
    }                           /* ������� */

    setbfile(filepath, currboard->filename, "");
    if ((aborted = GET_POSTFILENAME(post_file.filename, filepath)) != 0) {
        move(3, 0);
        clrtobot();
        prints("\n\n�޷������ļ�:%d...\n", aborted);
        pressreturn();
        clear();
        return FULLUPDATE;
    }

    in_mail = false;

    /*
     * strncpy(post_file.owner,(anonyboard&&Anony)?
     * "Anonymous":getCurrentUser()->userid,STRLEN) ;
     */
    strncpy(post_file.owner, (anonyboard && Anony) ? currboard->filename : getCurrentUser()->userid, OWNER_LEN);
    post_file.owner[OWNER_LEN - 1] = 0;

	/* �ظ������䣬stiger */
	if(mailback)
		post_file.accessed[1] |= FILE_MAILBACK;

    /*
     * if ((!strcmp(currboard,"Announce"))&&(!strcmp(post_file.owner,"Anonymous")))
     * strcpy(post_file.owner,"SYSOP");
     */

    if ((!strcmp(currboard->filename, "Announce")) && (!strcmp(post_file.owner, currboard->filename)))
        strcpy(post_file.owner, "SYSOP");

    setbfile(filepath, currboard->filename, post_file.filename);

    bp=currboard;
    if (bp->flag & BOARD_OUTFLAG) {
        local_article = 0;
        /* �ظ� local_save ������Ĭ�� local_save - atppp */
        if (re_file) local_article = (re_file->innflag[0]=='L' && re_file->innflag[1]=='L')?2:0;
    } else
        local_article = 1;
    if (!strcmp(post_file.title, buf) && q_file[0] != '\0')
        if (q_file[119] == 'L') /* FIXME */
            local_article = 1; //����ط�̫�����ˣ���ȫ����������֪���� local_save ��ʲôӰ�졣

    modify_user_mode(POSTING);

	if( use_tmpl > 0 ){
		FILE *fp,*fp1;
		char filepath1[STRLEN];
		char buff[256];
		char title_prefix[STRLEN];

		if( ! strncmp(post_file.title, "Re: ",4) )
			strncpy(title_prefix, post_file.title+4, ARTICLE_TITLE_LEN);
		else
			strncpy(title_prefix, post_file.title, ARTICLE_TITLE_LEN);

		title_prefix[STRLEN-1]='\0';

		sprintf(filepath1,"%s.mbak",filepath);
		aborted =  choose_tmpl( title_prefix , filepath ) ;
		if(aborted != -1){
			if( (fp=fopen( filepath1,"w"))!=NULL){
				if((fp1=fopen(filepath,"r"))==NULL){
					aborted = -1;
				}else{
					if( title_prefix[0] ){
							/*
						i = strlen( title_prefix ) + strlen(post_file.title) ;
						if( i >= STRLEN )
							i = STRLEN - 1 ;
						post_file.title[i]='\0';

						for (i-- ; i >= strlen(title_prefix); i--)
							post_file.title[i] = post_file.title[i-strlen(title_prefix)];
						for (; i>=0 ; i--)
                 		    if (title_prefix[i] == 0x1b || title_prefix[i] == '\n')
                       			post_file.title[i] = ' ';
                    		else
                        		post_file.title[i] = title_prefix[i];
								*/
						if( ! strncmp(post_file.title, "Re: ",4) )
							snprintf(save_title, ARTICLE_TITLE_LEN, "Re: %s", title_prefix );
						else
							snprintf(save_title, ARTICLE_TITLE_LEN , "%s", title_prefix );

						save_title[ARTICLE_TITLE_LEN-1]='\0';
            			strncpy(post_file.title, save_title, ARTICLE_TITLE_LEN - 1);
						post_file.title[ARTICLE_TITLE_LEN - 1] = '\0';
					}

					write_header(fp, getCurrentUser(), 0, currboard->filename, post_file.title, Anony, 0,getSession());
					while(fgets(buff,255,fp1))
						fprintf(fp,"%s",buff);
					fclose(fp);
					fclose(fp1);

					unlink(filepath);
					rename(filepath1,filepath);

				}
			}else{
				aborted = -1;
			}
		}
	}
				

	do_quote(filepath, include_mode, q_file, quote_user);       /*����ԭ���� */

    strcpy(quote_title, save_title);
    strcpy(quote_board, currboard->filename);

	if( use_tmpl <= 0 )
	    aborted = vedit(filepath, true, &eff_size, NULL);    /* ����༭״̬ */
	else{
    	struct stat st;
		if(stat(filepath, &st)!=-1)
			eff_size = st.st_size;
		else
			eff_size = 0;
	}


    post_file.eff_size = eff_size;

    add_loginfo(filepath, getCurrentUser(), currboard->filename, Anony,getSession());       /*������һ�� */

    strncpy(post_file.title, save_title, ARTICLE_TITLE_LEN - 1);
	post_file.title[ARTICLE_TITLE_LEN - 1] = '\0';
    if (aborted == 1 || !(bp->flag & BOARD_OUTFLAG)) {  /* local save */
        post_file.innflag[1] = 'L';
        post_file.innflag[0] = 'L';
    } else if (aborted == 0) {
        post_file.innflag[1] = 'S';
        post_file.innflag[0] = 'S';
        outgo_post(&post_file, currboard->filename, save_title,getSession());
    }
    Anony = 0;                  /*Inital For ShowOut Signature */

    if (aborted == -1) {        /* ȡ��POST */
        my_unlink(filepath);
        clear();
        return FULLUPDATE;
    }
    /*
     * ��boards����������Զ�������±�� Bigman:2000.8.12 
     */
    if (!strcmp(currboard->filename, "BM_Apply") && !HAS_PERM(getCurrentUser(), PERM_OBOARDS) && HAS_PERM(getCurrentUser(), PERM_BOARDS)) {
        post_file.accessed[0] |= FILE_SIGN;
    }
    if(upload) {
        post_file.attachment = 1;
    }
#ifdef FILTER
    returnvalue =
#endif
        after_post(getCurrentUser(), &post_file, currboard->filename, re_file, !(Anony && anonyboard),getSession());

    if(upload) {
        char sbuf[PATHLEN];
        strcpy(sbuf,"tmp/");
        strcpy(sbuf+strlen(sbuf), upload);
#ifdef FILTER
        if(returnvalue==2)
            setbfile(filepath, FILTER_BOARD, post_file.filename);
#endif
        add_attach(filepath, sbuf, upload);
    }
    
    if (!junkboard(currboard->filename)) {
        getCurrentUser()->numposts++;
    }
#ifdef FILTER
    if (returnvalue == 2) {
        clear();
        move(3, 0);
        prints("\n\n            �ܱ�Ǹ�����Ŀ��ܺ��в��ʵ������ݣ��辭��˷��ɷ�\n�������ĵȴ�...\n");
        pressreturn();
        return FULLUPDATE;
    }
#endif
    switch (cmdmode) {
    case 2:
        title_mode(conf,NULL,NULL);
        break;
    case 3:
        marked_mode(conf,NULL,NULL);
        break;
    }
    return DIRCHANGED;
}

int edit_post(struct _select_def* conf,struct fileheader *fileinfo,void* extraarg)
        /*
         * POST �༭
         */
{
    char buf[512];
    char *t;
    long eff_size;
    long attachpos;
    bool dobmlog=false;
    struct read_arg* arg=(struct read_arg*) conf->arg;

    if (fileinfo==NULL)
        return DONOTHING;
    if (!strcmp(currboard->filename, "syssecurity")
        || !strcmp(currboard->filename, "junk")
        || !strcmp(currboard->filename, "deleted"))       /* Leeward : 98.01.22 */
        return DONOTHING;
    if ((arg->mode== DIR_MODE_DELETED) || (arg->mode== DIR_MODE_JUNK))
        return DONOTHING;       /* no edit in dustbin as requested by terre */
    if (true == check_readonly(currboard->filename))      /* Leeward 98.03.28 */
        return FULLUPDATE;

#ifdef AIX_CANCELLED_BY_LEEWARD
    if (true == check_RAM_lack())       /* Leeward 98.06.16 */
        return FULLUPDATE;
#endif

    modify_user_mode(EDIT);

    if (!HAS_PERM(getCurrentUser(), PERM_SYSOP))     /* SYSOP����ǰ������ԭ������ ���Ա༭ */
        if (!chk_currBM(currBM, getCurrentUser())) {
            /*
             * change by KCN 1999.10.26
             * if(strcmp( fileinfo->owner, getCurrentUser()->userid))
             */
            if (!isowner(getCurrentUser(), fileinfo))
                return DONOTHING;
            else dobmlog=true;
        }

    if (deny_me(getCurrentUser()->userid, currboard->filename) && !HAS_PERM(getCurrentUser(), PERM_SYSOP)) {        /* ������ֹPOST ��� */
        move(3, 0);
        clrtobot();
        prints("\n\n                     �ܱ�Ǹ���㱻����ֹͣ�� POST ��Ȩ��...\n");
        pressreturn();
        clear();
        return FULLUPDATE;
    }

    clear();
    strcpy(buf, arg->direct);
    if ((t = strrchr(buf, '/')) != NULL)
        *t = '\0';
#ifndef LEEWARD_X_FILTER
    sprintf(genbuf, "/bin/cp -f %s/%s tmp/%d.editpost.bak", buf, fileinfo->filename, getpid()); /* Leeward 98.03.29 */
    system(genbuf);
#endif

    sprintf(genbuf, "%s/%s", buf, fileinfo->filename);
	strncpy(buf, genbuf, 512);
	buf[511]=0;
	attachpos = fileinfo->attachment;
    if (vedit_post(buf, false, &eff_size,&attachpos) != -1) {
		if( fileinfo->eff_size != eff_size ){
        	fileinfo->eff_size = eff_size;
			//fileinfo->eff_size = calc_effsize(genbuf);
            //change_post_flag(currBM, getCurrentUser(), arg->mode, currboard->filename, ent, 
                //fileinfo, direct, FILE_EFFSIZE_FLAG, 0);
		}
        if (ADD_EDITMARK)
            add_edit_mark(buf, 0, /*NULL*/ fileinfo->title,getSession());
        if (attachpos!=fileinfo->attachment) {
            struct write_dir_arg dirarg;
            fileinfo->attachment=attachpos;
            init_write_dir_arg(&dirarg);
            dirarg.fd=arg->fd;
            dirarg.ent = conf->pos;
            change_post_flag(&dirarg, arg->mode, currboard,  
                fileinfo,FILE_ATTACHPOS_FLAG|FILE_EFFSIZE_FLAG, fileinfo,dobmlog,getSession());
        }
    }
    newbbslog(BBSLOG_USER, "edited post '%s' on %s", fileinfo->title, currboard->filename);
    return FULLUPDATE;
}

int edit_title(struct _select_def* conf,struct fileheader *fileinfo,void* extraarg)
        /*
         * �༭���±��� 
         */
{
    char buf[STRLEN];
    struct read_arg* arg=(struct read_arg*) conf->arg;
    int ent=conf->pos;

    /*
     * Leeward 99.07.12 added below 2 variables 
     */
    long i=0;
    struct fileheader xfh;
    int fd;
    if (fileinfo==NULL)
        return DONOTHING;


    if (!strcmp(currboard->filename, "syssecurity")
        || !strcmp(currboard->filename, "junk")
        || !strcmp(currboard->filename, "deleted"))       /* Leeward : 98.01.22 */
        return DONOTHING;

    if ((arg->mode>= DIR_MODE_THREAD) && (arg->mode<= DIR_MODE_WEB_THREAD)) /*��Դdirect�����޸�*/
        return DONOTHING;
    if (true == check_readonly(currboard->filename))      /* Leeward 98.03.28 */
        return FULLUPDATE;

    if (!HAS_PERM(getCurrentUser(), PERM_SYSOP))     /* Ȩ�޼�� */
        if (!chk_currBM(currBM, getCurrentUser()))
            /*
             * change by KCN 1999.10.26
             * if(strcmp( fileinfo->owner, getCurrentUser()->userid))
             */
            if (!isowner(getCurrentUser(), fileinfo)) {
                return DONOTHING;
            }
    strcpy(buf, fileinfo->title);
    getdata(t_lines - 1, 0, "�����±���: ", buf, 78, DOECHO, NULL, false);      /*������� */
    if (buf[0] != '\0'&&strcmp(buf,fileinfo->title)) {
        char tmp[STRLEN * 2], *t;
        char tmp2[STRLEN];      /* Leeward 98.03.29 */

#ifdef FILTER
        if (check_badword_str(buf, strlen(buf))) {
            clear();
            move(3, 0);
            outs("     �ܱ�Ǹ���ñ�����ܺ��в�ǡ�������ݣ�����ϸ��黻�����⡣");
            pressreturn();
            return PARTUPDATE;
        }
#endif
        strcpy(tmp2, fileinfo->title);  /* Do a backup */
        /*
         * Changed by KCN,disable color title
         */
        {
            unsigned int i;

            for (i = 0; (i < strlen(buf)) && (i < ARTICLE_TITLE_LEN - 1); i++)
                if (buf[i] == 0x1b)
                    fileinfo->title[i] = ' ';
                else
                    fileinfo->title[i] = buf[i];
            fileinfo->title[i] = 0;
        }
        strcpy(tmp, arg->direct);
        if ((t = strrchr(tmp, '/')) != NULL)
            *t = '\0';
        sprintf(genbuf, "%s/%s", tmp, fileinfo->filename);

        if(strcmp(tmp2,buf)) add_edit_mark(genbuf, 2, buf,getSession());
        /*
         * Leeward 99.07.12 added below to fix a big bug
         */
		/* add by stiger */
	if(conf->pos>arg->filecount) {
            ent = get_num_records(arg->dingdirect,sizeof(struct fileheader));
            fd = open(arg->dingdirect, O_RDONLY, 0);
	} else fd=arg->fd;
	/* add end */
	if (fd!=-1) {
            for (i = ent; i > 0; i--) {//todo: dingdirect needn't read,should read from boardstatus
                if (0 == get_record_handle(fd, &xfh, sizeof(xfh), i)) {
                    if (0 == strcmp(xfh.filename, fileinfo->filename)) {
                        ent = i;
                        break;
                    }
                }
            }
	    if(conf->pos>arg->filecount) {
                close(fd);
                if (i!=0) 
                    substitute_record(arg->dingdirect, fileinfo, sizeof(*fileinfo), ent);
                board_update_toptitle(arg->bid, true);
            } else
                if (i!=0) 
                    substitute_record(arg->direct, fileinfo, sizeof(*fileinfo), ent);
        }
        if (0 == i)
            return PARTUPDATE;
        /*
         * Leeward 99.07.12 added above to fix a big bug
         */

		if(arg->mode != DIR_MODE_ORIGIN && fileinfo->id == fileinfo->groupid){
			if( setboardorigin(currboard->filename, -1) ){
				board_regenspecial(currboard->filename,DIR_MODE_ORIGIN,NULL);
			}else{
				char olddirect[PATHLEN];
	    		setbdir(DIR_MODE_ORIGIN, olddirect, currboard->filename);
				if ((fd = open(olddirect, O_RDWR, 0644)) >= 0){
					struct fileheader tmpfh;
					if (get_records_from_id(fd, fileinfo->id, &tmpfh, 1, &ent) == 0){
						close(fd);
					}else{
						close(fd);
   	                	substitute_record(olddirect, fileinfo, sizeof(*fileinfo), ent);
					}
				}
   	     //setboardorigin(currboard->filename, 1);
			}
		}
        setboardtitle(currboard->filename, 1);
    }
    return PARTUPDATE;
}

/* add by stiger,delete �ö����� */
int del_ding(struct _select_def* conf,struct fileheader *fileinfo,void* extraarg)
{
    int failed;
    char tmpname[100];
    struct read_arg* arg=(struct read_arg*)conf->arg;

    if (fileinfo==NULL)
        return DONOTHING;
    if ( arg->mode != DIR_MODE_NORMAL) return DONOTHING;

    if (!HAS_PERM(getCurrentUser(), PERM_SYSOP) && !chk_currBM(currBM, getCurrentUser()))
            return DONOTHING;
    clear();
    prints("ɾ������ '%s'.", fileinfo->title);
    getdata(1, 0, "(Y/N) [N]: ", genbuf, 3, DOECHO, NULL, true);
    if (genbuf[0] != 'Y' && genbuf[0] != 'y') {     /* if not yes quit */
        move(2, 0);
        prints("ȡ��\n");
        pressreturn();
        clear();
        return FULLUPDATE;
    }

    failed=delete_record(arg->dingdirect, sizeof(struct fileheader), conf->pos-arg->filecount, 
        (RECORD_FUNC_ARG) cmpname, fileinfo->filename);

    if(failed){
        move(2, 0);
        prints("ɾ��ʧ��\n");
        pressreturn();
        clear();
        board_update_toptitle(arg->bid, true);
        return FULLUPDATE;
    }else{
        snprintf(tmpname,100,"boards/%s/%s",currboard->filename,fileinfo->filename);
        my_unlink(tmpname);
        board_update_toptitle(arg->bid, true);
    }
    return DIRCHANGED;
}
/* add end */

/* stiger, �ö� */
int zhiding_post(struct _select_def* conf,struct fileheader *fileinfo,void* extraarg)
{
    struct read_arg* arg=(struct read_arg*)conf->arg;
    if (fileinfo==NULL)
        return DONOTHING;
    if(conf->pos>arg->filecount)
        return del_ding(conf,fileinfo,extraarg);
    if (add_top(fileinfo, currboard->filename, 0)!=0)
        return DONOTHING;
    return DIRCHANGED;
}

int noreply_post(struct _select_def* conf,struct fileheader *fileinfo,void* extraarg)
{
	/* add by stiger ,20030414, �ö�ѡ��*/
    char ans[4];
    int mode=0; /* 0x1����, 0x2:�Ƽ������ */
    int ret=FULLUPDATE;

#ifdef COMMEND_ARTICLE
    int bnum;
    struct boardheader bp;

    bnum = getboardnum(COMMEND_ARTICLE,&bp);
    if( bnum && chk_currBM(bp.BM, getCurrentUser()) )
		mode |= 0x2 ;
#endif
    if (fileinfo==NULL)
        return DONOTHING;
	if(chk_currBM(currBM, getCurrentUser())) mode |= 0x1;

#ifdef COMMEND_ARTICLE
	if ( ! (mode & 0x1) && !(mode & 0x2) ) return DONOTHING;
#else
	if ( ! (mode & 0x1) ) return DONOTHING;
#endif

    move(t_lines - 1, 0);
    clrtoeol();
#ifdef COMMEND_ARTICLE
	if( mode & 0x1 )
    	getdata(t_lines - 1, 0, "�л�: 0)ȡ�� 1)����re��� 2)�ö���� 3)�Ƽ� [1]: ", ans, 3, DOECHO, NULL, true);
	else
    	getdata(t_lines - 1, 0, "3) �Ƽ� [0]: ", ans, 3, DOECHO, NULL, true);
#else
    getdata(t_lines - 1, 0, "�л�: 0)ȡ�� 1)����re��� 2)�ö���� [1]: ", ans, 3, DOECHO, NULL, true);
#endif
    if (ans[0]=='0') return FULLUPDATE;
    if (ans[0] == ' ') {
        ans[0] = ans[1];
        ans[1] = 0;
    }
	if(ans[0]=='2'){
#ifdef COMMEND_ARTICLE
		if( !(mode & 0x1) )
		return FULLUPDATE;
#endif
		return zhiding_post(conf,fileinfo,extraarg);
	}
#ifdef COMMEND_ARTICLE
	else if(ans[0]=='3') return do_commend(conf,fileinfo,extraarg);
#endif
	else{
#ifdef COMMEND_ARTICLE
		if( !(mode & 0x1) )
		return FULLUPDATE;
#endif
		ret=set_article_flag(conf,fileinfo, FILE_NOREPLY_FLAG);
	}
	return ret;
}

int del_range(struct _select_def* conf,struct fileheader *fileinfo,void* extraarg)
  /*
   * ����ɾ�� 
   */
{
    char del_mode[11], num1[11], num2[11];
    int inum1, inum2;
    int result;                 /* Leeward: 97.12.15 */
    int idel_mode;              /*haohmaru.99.4.20 */
    struct read_arg* arg=(struct read_arg*)conf->arg;
    bool mailmode=(arg->mode==DIR_MODE_MAIL);

    if (!strcmp(currboard->filename, "syssecurity")
        || !strcmp(currboard->filename, "junk")
        || !strcmp(currboard->filename, "deleted")
        || strstr(arg->direct, ".THREAD") /*Haohmaru.98.10.16 */ )   /* Leeward : 98.01.22 */
        return DONOTHING;

    if (arg->mode!=DIR_MODE_MAIL&&!HAS_PERM(getCurrentUser(), PERM_SYSOP))
        if (!chk_currBM(currBM, getCurrentUser())) {
            return DONOTHING;
        }

/*��������ģʽ,����ģʽ,�ʼ�ģʽ,����Ӧ��������*/
    if ((arg->mode >= DIR_MODE_THREAD)&&(arg->mode <= DIR_MODE_WEB_THREAD))
        return DONOTHING;
    clear();
    prints("����ɾ��\n");
    /*
     * Haohmaru.99.4.20.���ӿ���ǿ��ɾ����mark���µĹ��� 
     */
    getdata(1, 0, "ɾ��ģʽ [0]���ɾ�� [1]��ͨɾ�� [2]ǿ��ɾ�� [3]ȡ��ɾ����� (0): ", del_mode, 10, DOECHO, NULL, true);
    idel_mode = atoi(del_mode);
    /*
     * if (idel_mode!=0 || idel_mode!=1)
     * {
     * return FULLUPDATE ;
     * } 
     */
    getdata(2, 0, "��ƪ���±��(����0���������Ϊɾ��������): ", num1, 10, DOECHO, NULL, true);
    inum1 = atoi(num1);
    if (inum1 == 0) {
        inum2 = -1;
        goto THERE;
    }
    if (inum1 <= 0) {
        prints("������\n");
        pressreturn();
        return FULLUPDATE;
    }
    getdata(3, 0, "ĩƪ���±��: ", num2, 10, DOECHO, NULL, true);
    inum2 = atoi(num2);
    if (inum2 <= inum1) {
        prints("������\n");
        pressreturn();
        return FULLUPDATE;
    }
  THERE:
    getdata(4, 0, "ȷ��ɾ�� (Y/N)? [N]: ", num1, 10, DOECHO, NULL, true);
    if (*num1 == 'Y' || *num1 == 'y') {
        struct write_dir_arg dirarg;
	if (!mailmode)
            bmlog(getCurrentUser()->userid, currboard->filename, 5, 1);
        init_write_dir_arg(&dirarg);
        dirarg.fd=arg->fd;
        dirarg.filename=arg->direct;
        result = delete_range(&dirarg, inum1, inum2, idel_mode,arg->mode,currboard,getSession());
        /* todo ����conf��pos
        if (inum1 != 0)
            fixkeep(arg->direct, inum1, inum2);
        else
            fixkeep(arg->direct, 1, 1);*/
        if (!mailmode) {
            updatelastpost(currboard->filename);
            bmlog(getCurrentUser()->userid, currboard->filename, 8, inum2-inum1);
            newbbslog(BBSLOG_USER, "del %d-%d on %s", inum1, inum2, currboard->filename);
        }
        prints("ɾ��%s\n", result ? "ʧ�ܣ�" : "���"); /* Leeward: 97.12.15 */
        pressreturn();
        return DIRCHANGED;
    }
    prints("Delete Aborted\n");
    pressreturn();
    return FULLUPDATE;
}

int del_post(struct _select_def* conf,struct fileheader *fileinfo,void* extraarg)
{
    char usrid[STRLEN],direct[MAXPATH];
    int owned;
    struct read_arg* arg=conf->arg;
    int ent,ret;
    int flag=(POINTDIFF)extraarg;
    struct write_dir_arg delarg;

    if (fileinfo==NULL)
        return DONOTHING;
    ent=conf->pos;
    /* add by stiger */
    if (ent>arg->filecount)
        return del_ding(conf,fileinfo,extraarg);

    if (!strcmp(currboard->filename, "syssecurity")
        || !strcmp(currboard->filename, "junk")
        || !strcmp(currboard->filename, "deleted"))       /* Leeward : 98.01.22 */
        return DONOTHING;

    if (arg->mode== DIR_MODE_DELETED|| arg->mode== DIR_MODE_JUNK)
        return DONOTHING;
    owned = isowner(getCurrentUser(), fileinfo);
    /*
     * change by KCN  ! strcmp( fileinfo->owner, getCurrentUser()->userid ); 
     */
    strcpy(usrid, fileinfo->owner);
    if (!(owned) && !HAS_PERM(getCurrentUser(), PERM_SYSOP))
        if (!chk_currBM(currboard->BM, getCurrentUser())) {
            return DONOTHING;
        }
    if (!(flag&&ARG_NOPROMPT_FLAG)) {
        clear();
        prints("ɾ������ '%s'.", fileinfo->title);
        getdata(1, 0, "(Y/N) [N]: ", genbuf, 3, DOECHO, NULL, true);
        if (genbuf[0] != 'Y' && genbuf[0] != 'y') {     /* if not yes quit */
            move(2, 0);
            prints("ȡ��\n");
            pressreturn();
            clear();
            return FULLUPDATE;
        }
    }

    if (arg->writearg==NULL) {
        init_write_dir_arg(&delarg);
        if ((arg->mode!=DIR_MODE_NORMAL)&& arg->mode != DIR_MODE_DIGEST) {
            setbdir(DIR_MODE_NORMAL, direct, currboard->filename);
            delarg.filename=direct;
        } else {
            delarg.fd=arg->fd;
            delarg.ent=conf->pos;
        }
        ret=do_del_post(getCurrentUser(), &delarg, fileinfo, currboard->filename, DIR_MODE_NORMAL, flag&ARG_DELDECPOST_FLAG,getSession());
        free_write_dir_arg(&delarg);
    } else
        ret=do_del_post(getCurrentUser(), arg->writearg, fileinfo, currboard->filename, DIR_MODE_NORMAL, flag&ARG_DELDECPOST_FLAG,getSession());
    if (ret != 0) {
        if (!(flag&ARG_NOPROMPT_FLAG)) {
            move(2, 0);
            prints("ɾ��ʧ��\n");
            pressreturn();
            clear();
            return FULLUPDATE;
        }
    }
    if (!(flag&ARG_BMFUNC_FLAG)&&arg->mode) {
        switch (arg->mode) {
        case DIR_MODE_THREAD:
            title_mode(conf,fileinfo,extraarg);
            break;
        case DIR_MODE_MARK:
            marked_mode(conf,fileinfo,extraarg);
            break;
        case DIR_MODE_ORIGIN:
        case DIR_MODE_AUTHOR:
        case DIR_MODE_TITLE:
            search_mode(conf,fileinfo,arg->mode, search_data);
            break;
        default:
            break;
        }
    }
    return DIRCHANGED;
}

/* Added by netty to handle post saving into (0)Announce */
int Save_post(struct _select_def* conf,struct fileheader *fileinfo,void* extraarg)
{
    int ret;
    struct read_arg* arg=(struct read_arg*)conf->arg;
    if (!HAS_PERM(getCurrentUser(), PERM_SYSOP))
        if (!chk_currBM(currBM, getCurrentUser()))
            return DONOTHING;
    ret=a_Save(NULL, currboard->filename, fileinfo, false, arg->direct, conf->pos);
    if (ret) {
        struct write_dir_arg dirarg;
        struct fileheader data;
        init_write_dir_arg(&dirarg);
        dirarg.fd=arg->fd;
        dirarg.ent = conf->pos;
        data.accessed[0]=FILE_IMPORTED;
        change_post_flag(&dirarg, 
            arg->mode,
            currboard, 
            fileinfo, 
            FILE_IMPORT_FLAG, &data, true,getSession());
        free_write_dir_arg(&dirarg);
	return DIRCHANGED;
    }
    return DONOTHING;
}

/* Semi_save ���������´浽�ݴ浵��ͬʱɾ�����µ�ͷβ Life 1997.4.6 */
int Semi_save(struct _select_def* conf,struct fileheader *fileinfo,void* extraarg)
{
    int ret;
    struct read_arg* arg=(struct read_arg*)conf->arg;
    if (!HAS_PERM(getCurrentUser(), PERM_SYSOP))
        if (!chk_currBM(currBM, getCurrentUser()))
            return DONOTHING;
    ret=a_SeSave("0Announce", currboard->filename, fileinfo, false,arg->direct,conf->pos,1);
    if (ret) {
        struct write_dir_arg dirarg;
        struct fileheader data;
        init_write_dir_arg(&dirarg);
        dirarg.fd=arg->fd;
        dirarg.ent = conf->pos;
        data.accessed[0]=FILE_IMPORTED;
        change_post_flag(&dirarg, 
            arg->mode,
            currboard, 
            fileinfo, 
            FILE_IMPORT_FLAG, &data, true,getSession());
        free_write_dir_arg(&dirarg);
    }
    return ret;
}

/* Added by netty to handle post saving into (0)Announce */
int Import_post(struct _select_def* conf,struct fileheader *fileinfo,void* extraarg)
{
    char szBuf[STRLEN];
    struct read_arg* arg=(struct read_arg*)conf->arg;
    int ret=FULLUPDATE;

    if (!HAS_PERM(getCurrentUser(), PERM_SYSOP))
        if (!chk_currBM(currBM, getCurrentUser()))
            return DONOTHING;

    if (fileinfo->accessed[0] & FILE_IMPORTED) {        /* Leeward 98.04.15 */
        a_prompt(-1, "������������¼����������. �����ٴ���¼��? (Y/N) [N]: ", szBuf);
        if (szBuf[0] != 'y' && szBuf[0] != 'Y')
            return FULLUPDATE;
    }

    /*
     * oldbug 2003.08.01
     */
    if (arg->mode == DIR_MODE_DELETED|| arg->mode == DIR_MODE_JUNK){
            char *p;
            int i;
            if ((p = strrchr(fileinfo->title, '-')) != NULL) {
                *p = 0;
                for (i = strlen(fileinfo->title) - 1; i >= 0; i--) {
                    if (fileinfo->title[i] != ' ')
                        break;
                    else
                        fileinfo->title[i] = 0;
                }
            }
    }

    /*
     * Leeward 98.04.15 
     */
    if (a_Import(NULL, currboard->filename, fileinfo, false, arg->direct, conf->pos)==0) {
        if (!(fileinfo->accessed[0] & FILE_IMPORTED)) {
            ret=set_article_flag(conf, fileinfo, FILE_IMPORT_FLAG);
        }
    }
    return ret;
}

int show_b_note(struct _select_def* conf,struct fileheader *fileinfo,void* extraarg)
{
    clear();
    if (show_board_notes(currboard->filename) == -1) {
        move(3, 30);
        prints("�����������ޡ�����¼����");
    }
    pressanykey();
    return FULLUPDATE;
}

#ifdef NINE_BUILD
int show_sec_board_notes(char bname[30])
{                               /* ��ʾ�����Ļ� */
    char buf[256];

    sprintf(buf, "vote/%s/secnotes", bname);    /* ��ʾ����İ����Ļ� vote/����/notes */
    if (dashf(buf)) {
        ansimore2(buf, false, 0, 23 /*19 */ );
        return 1;
    } else if (dashf("vote/secnotes")) {        /* ��ʾϵͳ�Ļ� vote/notes */
        ansimore2("vote/secnotes", false, 0, 23 /*19 */ );
        return 1;
    }
    return -1;
}

int show_sec_b_note(struct _select_def* conf,struct fileheader *fileinfo,void* extraarg)
{
    clear();
    if (show_sec_board_notes(currboard->filename) == -1) {
        move(3, 30);
        prints("�����������ޡ����ܱ���¼����");
    }
    pressanykey();
    return FULLUPDATE;
}
#endif

int into_announce(struct _select_def* conf,struct fileheader *fileinfo,void* extraarg)
{
    if (a_menusearch("0Announce", currboard->filename, (HAS_PERM(getCurrentUser(), PERM_ANNOUNCE) || HAS_PERM(getCurrentUser(), PERM_SYSOP) || HAS_PERM(getCurrentUser(), PERM_OBOARDS)) ? PERM_BOARDS : 0))
        return FULLUPDATE;
    return DONOTHING;
}

int sequential_read(struct _select_def* conf,struct fileheader *fileinfo,void* extraarg)
{
    struct read_arg* arg=conf->arg;
    int findpos;
    
    findpos=find_nextnew(conf,0);
    if (findpos) {
        conf->new_pos=findpos;
        arg->readmode=READ_NEW;
        list_select_add_key(conf, 'r');
    }
    return SHOW_SELCHANGE;
}

int clear_new_flag(struct _select_def* conf,struct fileheader *fileinfo,void* extraarg)
{
#ifdef HAVE_BRC_CONTROL
    struct read_arg* arg=conf->arg;
    /* add by stiger */
    if (conf->pos>arg->filecount)
        brc_clear(getSession());
    else brc_clear_new_flag(fileinfo->id,getSession());
#endif
    return PARTUPDATE;
}

int clear_all_new_flag(struct _select_def* conf,struct fileheader *fileinfo,void* extraarg)
{
#ifdef HAVE_BRC_CONTROL
    brc_clear(getSession());
#endif
    return FULLUPDATE;
}

int range_flag(struct _select_def* conf,struct fileheader *fileinfo,void* extraarg)
{
    char ans[4], buf[80];
    char num1[10], num2[10];
    int inum1, inum2, total=0;
    struct stat st;
    int i,k;
    int fflag;
    struct read_arg* arg=conf->arg;
    
    if (!chk_currBM(currBM, getCurrentUser())) return DONOTHING;
    if (arg->mode!=DIR_MODE_SUPERFITER) return DONOTHING;
    if(stat(arg->direct, &st)==-1) return DONOTHING;
    total = st.st_size/sizeof(struct fileheader);
    
    clear();
    prints("���α��, �����ʹ��");
    getdata(2, 0, "��ƪ���±��: ", num1, 10, DOECHO, NULL, true);
    inum1 = atoi(num1);
    if (inum1 <= 0) return FULLUPDATE;
    getdata(3, 0, "ĩƪ���±��: ", num2, 10, DOECHO, NULL, true);
    inum2 = atoi(num2);
    if (inum2 <= inum1) {
        prints("������\n");
        pressreturn();
        return FULLUPDATE;
    }
    sprintf(buf, "1-�������m  2-ɾ�����t  3-��ժ���g  4-����Re���  5-���#%s:[0]",
#ifdef FILTER
        HAS_PERM(getCurrentUser(), PERM_SYSOP)?"  6-�����@":"");
#else
        "");
#endif
    getdata(4, 0, buf, ans, 4, DOECHO, NULL, true);
#ifdef FILTER
    if(ans[0]=='6'&&!HAS_PERM(getCurrentUser(), PERM_SYSOP)) return FULLUPDATE;
#else
    if(ans[0]=='6') return FULLUPDATE;
#endif
    if(ans[0]<'1'||ans[0]>'6') return FULLUPDATE;
    if(askyn("�����ؿ���, ȷ�ϲ�����?", 0)==0) return FULLUPDATE;
    k=ans[0]-'0';
    if(k==1) fflag=FILE_MARK_FLAG;
    else if(k==2) fflag=FILE_DELETE_FLAG;
    else if(k==3) fflag=FILE_DIGEST_FLAG;
    else if(k==4) fflag=FILE_NOREPLY_FLAG;
    else if(k==5) fflag=FILE_SIGN_FLAG;
#ifdef FILTER
    else if(k==6) fflag=FILE_CENSOR_FLAG;
#endif
    else return FULLUPDATE;
    for(i=inum1;i<=inum2;i++) 
    if(i>=1&&i<=total) {
        struct write_dir_arg dirarg;
        struct fileheader data;
        data.accessed[0]=0xff;
        data.accessed[1]=0xff;

        init_write_dir_arg(&dirarg);
        dirarg.fd=arg->fd;
        dirarg.filename=arg->direct;
        dirarg.ent = i;
        dirarg.needlock=false;
        data.accessed[0]=FILE_IMPORTED;
        flock(arg->fd,LOCK_EX);
        malloc_write_dir_arg(&dirarg);
        change_post_flag(&dirarg, 
            arg->mode,
            currboard, 
            dirarg.fileptr+(i-1), 
            fflag, &data, true,getSession());
        flock(arg->fd,LOCK_UN);
        free_write_dir_arg(&dirarg);
    }
    prints("\n��ɱ��\n");
    pressreturn();
    return DIRCHANGED;
}

int show_t_friends()
{
    if (!HAS_PERM(getCurrentUser(), PERM_BASIC))
        return PARTUPDATE;
    t_friends();
    return FULLUPDATE;
}

/* add by stiger, add template */
int b_note_edit_new(struct _select_def* conf,struct fileheader *fileinfo,void* extraarg)
{
	char ans[4];

	if(!chk_currBM(currBM, getCurrentUser())) return DONOTHING;

    move(t_lines - 1, 0);
    clrtoeol();
    getdata(t_lines - 1, 0, "�༭: 0)ȡ�� 1)����¼ 2)����ģ��"
#ifdef FLOWBANNER
		" 3)�ײ�������Ϣ"
#endif
		" [0]: ", ans, 3, DOECHO, NULL, true);
    if (ans[0]=='1') return b_notes_edit();
	else if(ans[0]=='2'){
		int ret;	
#ifdef NEW_HELP
		int oldhelpmode=helpmode;
		helpmode = HELP_TMPL;
#endif
		ret =  m_template();
#ifdef NEW_HELP
		helpmode = oldhelpmode;
#endif
		return ret;
	}
#ifdef FLOWBANNER
	else if(ans[0]=='3'){
		return b_banner_edit();		
	}
#endif

	return FULLUPDATE;
}

int ReadBoard()
{
    int returnmode;
    while (1) {
        returnmode=Read();
        
        if ((returnmode==-2)||(returnmode==CHANGEMODE)) { //is directory or select another board
            if (currboard->flag&BOARD_GROUP) {
                choose_board(0,NULL,currboardent,0);
                break;
            }
        } else break;
    }
    return 0;
}

/*Add by SmallPig*/
static int catnotepad(FILE * fp, char *fname)
{
    char inbuf[256];
    FILE *sfp;
    int count;

    count = 0;
    if ((sfp = fopen(fname, "r")) == NULL) {
        fprintf(fp, "\033[31m\033[41m�ѩء����������������������������������������������������������������������ء�\033[m\n\n");
        return -1;
    }
    while (fgets(inbuf, sizeof(inbuf), sfp) != NULL) {
        if (count != 0)
            fputs(inbuf, fp);
        else
            count++;
    }
    fclose(sfp);
    return 0;
}

void notepad()
{
    char tmpname[STRLEN], note1[4];
    char note[3][STRLEN - 4];
    char tmp[STRLEN];
    FILE *in;
    int i, n;
    time_t thetime = time(0);

    clear();
    move(0, 0);
    prints("��ʼ������԰ɣ��������Ŀ�Դ�....\n");
    sprintf(tmpname, "etc/notepad_tmp/%s.notepad", getCurrentUser()->userid);
    if ((in = fopen(tmpname, "w")) != NULL) {
        for (i = 0; i < 3; i++)
            memset(note[i], 0, STRLEN - 4);
        while (1) {
            for (i = 0; i < 3; i++) {
                getdata(1 + i, 0, ": ", note[i], STRLEN - 5, DOECHO, NULL, false);
                if (note[i][0] == '\0')
                    break;
            }
            if (i == 0) {
                fclose(in);
                unlink(tmpname);
                return;
            }
            getdata(5, 0, "�Ƿ����Ĵ����������԰� (Y)�ǵ� (N)��Ҫ (E)�ٱ༭ [Y]: ", note1, 3, DOECHO, NULL, true);
            if (note1[0] == 'e' || note1[0] == 'E')
                continue;
            else
                break;
        }
        if (note1[0] != 'N' && note1[0] != 'n') {
            sprintf(tmp, "\033[32m%s\033[37m��%.24s��", getCurrentUser()->userid, getCurrentUser()->username);
            fprintf(in, "\033[m\033[31m�ѩС�����������������������������\033[37m���������\033[31m�������������������������������С�\033[m\n");
            fprintf(in, "\033[31m����%-43s\033[33m�� \033[36m%.19s\033[33m �뿪ʱ���µĻ�\033[31m����\n", tmp, Ctime(thetime));
            if (i > 2)
                i = 2;
            for (n = 0; n <= i; n++) {
#ifdef FILTER
	        if (check_badword_str(note[n],strlen(note[n]))) {
			int t;
                        for (t = n; t <= i; t++) 
                            fprintf(in, "\033[31m��\033[m%-74.74s\033[31m��\033[m\n", note[t]);
			fclose(in);

                        post_file(getCurrentUser(), "", tmpname, FILTER_BOARD, "---���԰������---", 0, 2,getSession());

			unlink(tmpname);
			return;
		}
#endif
                if (note[n][0] == '\0')
                    break;
                fprintf(in, "\033[31m��\033[m%-74.74s\033[31m��\033[m\n", note[n]);
            }
            fprintf(in, "\033[31m���С����������������������������������������������������������������������С�\033[m\n");
            catnotepad(in, "etc/notepad");
            fclose(in);
            f_mv(tmpname, "etc/notepad");
        } else {
            fclose(in);
            unlink(tmpname);
        }
    }
    if (talkrequest) {
        talkreply();
    }
    clear();
    return;
}

void record_exit_time()
{                               /* ��¼����ʱ��  Luzi 1998/10/23 */
    getCurrentUser()->exittime = time(NULL);
    /*
     * char path[80];
     * FILE *fp;
     * time_t now;
     * sethomefile( path, getCurrentUser()->userid , "exit");
     * fp=fopen(path, "wb");
     * if (fp!=NULL)
     * {
     * now=time(NULL);
     * fwrite(&now,sizeof(time_t),1,fp);
     * fclose(fp);
     * }
     */
}

extern int icurrchar, ibufsize;

int Goodbye()
{                               /*��վ ѡ�� */
    extern int started;
    time_t stay;
    char fname[STRLEN], notename[STRLEN];
    char sysoplist[20][STRLEN], syswork[20][STRLEN], spbuf[STRLEN], buf[STRLEN];
    int i, num_sysop, choose, logouts, mylogout = false;
    FILE *sysops;
    long Time = 10;             /*Haohmaru */
    int left = (80 - 36) / 2;
    int top = (scr_lns - 11) / 2;
    struct _select_item level_conf[] = {
        {-1, -1, -1, SIT_SELECT, (void *) ""},
        {-1, -1, -1, SIT_SELECT, (void *) ""},
        {-1, -1, -1, SIT_SELECT, (void *) ""},
        {-1, -1, -1, SIT_SELECT, (void *) ""},
        {-1, -1, -1, 0, NULL}
    };
    level_conf[0].x = left + 7;
    level_conf[1].x = left + 7;
    level_conf[2].x = left + 7;
    level_conf[3].x = left + 7;
    level_conf[0].y = top + 2;
    level_conf[1].y = top + 3;
    level_conf[2].y = top + 4;
    level_conf[3].y = top + 5;

/*---	��ʾ����¼�Ĺص������Ļ����	2001-07-01	---*/
    modify_user_mode(READING);

    i = 0;
    if ((sysops = fopen("etc/sysops", "r")) != NULL) {
        while (fgets(buf, STRLEN, sysops) != NULL && i < 20) {
            strcpy(sysoplist[i], (char *) strtok(buf, " \n\r\t"));
            strcpy(syswork[i], (char *) strtok(NULL, " \n\r\t"));
            i++;
        }
        fclose(sysops);
    }
    num_sysop = i;
    move(1, 0);
    clear();
    move(top, left);
    outs("\x1b[1;47;37m�X�T[*]�T�T�T Message �T�T�T�T�T�T�[\x1b[m");
    move(top + 1, left);
    outs("\x1b[1;47;37m�U\x1b[44;37m                                \x1b[47;37m�U\x1b[m");
    move(top + 2, left);
    prints("\x1b[1;47;37m�U\x1b[44;37m     [\x1b[33m1\x1b[37m] ���Ÿ�%-10s       \x1b[47;37m�U\x1b[m", NAME_BBS_CHINESE);
    move(top + 3, left);
    prints("\x1b[1;47;37m�U\x1b[44;37m     [\x1b[33m2\x1b[37m] \x1b[32m����%-15s\x1b[37m    \x1b[47;37m�U\x1b[m", NAME_BBS_CHINESE "BBSվ");
    move(top + 4, left);
    outs("\x1b[1;47;37m�U\x1b[44;37m     [\x1b[33m3\x1b[37m] дд*���԰�*           \x1b[47;37m�U\x1b[m");
    move(top + 5, left);
    outs("\x1b[1;47;37m�U\x1b[44;37m     [\x1b[33m4\x1b[37m] �뿪��BBSվ            \x1b[47;37m�U\x1b[m");
    move(top + 6, left);
    outs("\x1b[1;47;37m�U\x1b[0;44;34m________________________________\x1b[1;47;37m�U\x1b[m");
    move(top + 7, left);
    outs("\x1b[1;47;37m�U                                �U\x1b[m");
    move(top + 8, left);
    outs("\x1b[1;47;37m�U          \x1b[42;33m  ȡ��(ESC) \x1b[0;47;30m�z  \x1b[1;37m      �U\x1b[m");
    move(top + 9, left);
    outs("\x1b[1;47;37m�U            \x1b[0;40;37m�z�z�z�z�z�z\x1b[1;47;37m        �U\x1b[m");
    move(top + 10, left);
    outs("\x1b[1;47;37m�^�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�a\x1b[m");
    outs("\x1b[1;44;37m");

    choose = simple_select_loop(level_conf, SIF_SINGLE | SIF_ESCQUIT | SIF_NUMBERKEY, t_columns, t_lines, NULL);
    if (choose == 0)
        choose = 2;
    clear();
    if (strcmp(getCurrentUser()->userid, "guest") && choose == 1) {  /* д�Ÿ�վ�� */
        if (PERM_LOGINOK & getCurrentUser()->userlevel) {    /*Haohmaru.98.10.05.ûͨ��ע���ֻ�ܸ�ע��վ������ */
            prints("        ID        �����ְ��\n");
            prints("   ============ =============\n");
            for (i = 1; i <= num_sysop; i++) {
                prints("[\033[33m%1d\033[m] \033[1m%-12s %s\033[m\n", i, sysoplist[i - 1], syswork[i - 1]);
            }

            prints("[\033[33m%1d\033[m] ���������ޣ�\n", num_sysop + 1);      /*���һ��ѡ�� */

            sprintf(spbuf, "���ѡ���� [\033[32m%1d\033[m]��", num_sysop + 1);
            getdata(num_sysop + 5, 0, spbuf, genbuf, 4, DOECHO, NULL, true);
            choose = genbuf[0] - '0';
            if (0 != genbuf[1])
                choose = genbuf[1] - '0' + 10;
            if (choose >= 1 && choose <= num_sysop) {
                /*
                 * do_send(sysoplist[choose-1], "ʹ���߼����ĵĽ�����"); 
                 */
                if (choose == 1)        /*modified by Bigman : 2000.8.8 */
                    do_send(sysoplist[0], "��վ���ܹܡ�ʹ���߼����Ľ�����", "");
                else if (choose == 2)
                    do_send(sysoplist[1], "��ϵͳά����ʹ���߼����Ľ�����", "");
                else if (choose == 3)
                    do_send(sysoplist[2], "���������ʹ���߼����Ľ�����", "");
                else if (choose == 4)
                    do_send(sysoplist[3], "�����ȷ�ϡ�ʹ���߼����Ľ�����", "");
                else if (choose == 5)
                    do_send(sysoplist[4], "���ٲ����ˡ�ʹ���߼����Ľ�����", "");
            }
/* added by stephen 11/13/01 */
            choose = -1;
        } else {
            /*
             * ����ע�����ʾ��Ϣ Bigman:2000.10.31 
             */
            prints("\n    �����һֱδ�õ������֤,��ȷ�����Ƿ񵽸��˹�������д��ע�ᵥ,\n");
            prints("    ������յ����ȷ����,��û�з��������Ȩ��,����������дһ��ע�ᵥ\n\n");
            prints("     վ���� ID   �����ְ��\n");
            prints("   ============ =============\n");

            /*
             * added by Bigman: 2000.8.8  �޸���վ 
             */
            prints("[\033[33m%1d\033[m] \033[1m%-12s %s\033[m\n", 1, sysoplist[3], syswork[3]);
            prints("[\033[33m%1d\033[m] ���������ޣ�\n", 2);  /*���һ��ѡ�� */

            sprintf(spbuf, "���ѡ���� %1d��", 2);
            getdata(num_sysop + 6, 0, spbuf, genbuf, 4, DOECHO, NULL, true);
            choose = genbuf[0] - '0';
            if (choose == 1)    /*modified by Bigman : 2000.8.8 */
                do_send(sysoplist[3], "�����ȷ�ϡ�ʹ���߼����Ľ�����", "");
            choose = -1;

            /*
             * for(i=0;i<=3;i++)
             * prints("[\033[33m%1d\033[m] \033[1m%-12s %s\033[m\n",
             * i,sysoplist[i+4],syswork[i+4]);
             * prints("[\033[33m%1d\033[m] ���������ޣ�\n",4); 
 *//*
 * * * * * * * * * * * ���һ��ѡ�� 
 */
            /*
             * sprintf(spbuf,"���ѡ���� [\033[32m%1d\033[m]��",4);
             * getdata(num_sysop+6,0, spbuf,genbuf, 4, DOECHO, NULL ,true);
             * choose=genbuf[0]-'0';
             * if(choose==1)
             * do_send(sysoplist[5], "ʹ���߼����ĵĽ�����");
             * else if(choose==2)
             * do_send(sysoplist[6], "ʹ���߼����ĵĽ�����");
             * else if(choose==3)
             * do_send(sysoplist[7], "ʹ���߼����ĵĽ�����");
             * else if(choose==0)
             * do_send(sysoplist[4], "ʹ���߼����ĵĽ�����");
             * choose=-1; 
             */
        }
    }
    if (choose == 2)            /*����BBS */
        return 0;
    if (strcmp(getCurrentUser()->userid, "guest") != 0) {
        if (choose == 3)        /*���Բ� */
            if (USE_NOTEPAD == 1 && HAS_PERM(getCurrentUser(), PERM_POST))
                notepad();
    }

    clear();
    prints("\n\n\n\n");
    stay = time(NULL) - login_start_time;       /*��������ʱ�� */

    getCurrentUser()->stay += stay;

    if (DEFINE(getCurrentUser(), DEF_OUTNOTE /*�˳�ʱ��ʾ�û�����¼ */ )) {
        sethomefile(notename, getCurrentUser()->userid, "notes");
        if (dashf(notename))
            ansimore(notename, true);
    }

    /*
     * Leeward 98.09.24 Use SHARE MEM and disable the old code 
     */
    if (DEFINE(getCurrentUser(), DEF_LOGOUT)) {      /* ʹ���Լ�����վ���� */
        sethomefile(fname, getCurrentUser()->userid, "logout");
        if (dashf(fname))
            mylogout = true;
    }
    if (mylogout) {
        logouts = countlogouts(fname);  /* logouts Ϊ ��վ���� ���� */
        if (logouts >= 1) {
            user_display(fname, (logouts == 1) ? 1 : (getCurrentUser()->numlogins % (logouts)) + 1, true);
        }
    } else {
        logouts = countlogouts("etc/logout");   /* logouts Ϊ ��վ���� ���� */
        user_display("etc/logout", rand() % logouts + 1, true);
    }

    /*
     * if(DEFINE(getCurrentUser(),DEF_LOGOUT\*ʹ���Լ�����վ����*\)) Leeward: disable the old code
     * {
     * sethomefile( fname,getCurrentUser()->userid, "logout" );
     * if(!dashf(fname))
     * strcpy(fname,"etc/logout");
     * }else
     * strcpy(fname,"etc/logout");
     * if(dashf(fname))
     * {
     * logouts=countlogouts(fname);      \* logouts Ϊ ��վ���� ���� *\
     * if(logouts>=1)
     * {
     * user_display(fname,(logouts==1)?1:
     * (getCurrentUser()->numlogins%(logouts))+1,true);
     * }
     * } 
     */
    bbslog("user", "%s", "exit");

    /*
     * stay = time(NULL) - login_start_time;    ��������ʱ�� 
     */
    /*
     * Haohmaru.98.11.10.���ж��Ƿ�����վ�� 
     */
    if ( /*strcmp(getCurrentUser()->username,"guest")&& */ stay <= Time) {
/*        strcpy(lbuf, "����-");
        strftime(lbuf + 5, 30, "%Y-%m-%d%Y:%H:%M", localtime(&login_start_time));
        sprintf(tmpfile, "tmp/.tmp%d", getpid());
        fp = fopen(tmpfile, "w");
        if (fp) {
            fputs(lbuf, fp);
            fclose(fp);
            mail_file(getCurrentUser()->userid, tmpfile, "surr", "����", BBSPOST_MOVE, NULL);
        }*/
    }
    /*
     * stephen on 2001.11.1: ��վ����5���Ӳ�������վ���� 
     */
    if (stay <= 300 && getCurrentUser()->numlogins > 5) {
        getCurrentUser()->numlogins--;
        if (getCurrentUser()->stay > stay)
            getCurrentUser()->stay -= stay;
    }
    if (started) {
        record_exit_time();     /* ��¼�û����˳�ʱ�� Luzi 1998.10.23 */
        /*---	period	2000-10-19	4 debug	---*/
        /*
         * sprintf( genbuf, "Stay:%3ld (%s)", stay / 60, getCurrentUser()->username ); 
         */
        newbbslog(BBSLOG_USIES, "EXIT: Stay:%3ld (%s)[%d %d]", stay / 60, getCurrentUser()->username, utmpent, usernum);
        u_exit();
        started = 0;
    }

    if (num_user_logins(getCurrentUser()->userid) == 0 || !strcmp(getCurrentUser()->userid, "guest")) {   /*��黹��û���������� */
        FILE *fp;
        char buf[STRLEN], *ptr;

//        sethomefile(fname, getCurrentUser()->userid, "msgfile");
        if (DEFINE(getCurrentUser(), DEF_MAILMSG /*��վʱ�Ļ�������Ϣ */ ) && (get_msgcount(0, getCurrentUser()->userid))) {
                mail_msg(getCurrentUser(),getSession());
/*    #ifdef NINE_BUILD
            time_t now, timeout;
            char ans[3];

            timeout = time(0) + 60;
            do {
                move(t_lines - 1, 0);
                clrtoeol();
                getdata(t_lines - 1, 0, "�Ƿ񽫴˴����յ�������ѶϢ�浵 (Y/N)? ", ans, 2, DOECHO, NULL, true);
                if ((toupper(ans[0]) == 'Y') || (toupper(ans[0]) == 'N'))
                    break;
            } while (time(0) < timeout);
            if (toupper(ans[0]) == 'Y') {
#endif
                char title[STRLEN];
                time_t now;

                now = time(0);
                sprintf(title, "[%12.12s] ����ѶϢ����", ctime(&now) + 4);
                mail_file(getCurrentUser()->userid, fname, getCurrentUser()->userid, title, BBSPOST_MOVE);
#ifdef NINE_BUILD
            }
#endif*/
        } else
            clear_msg(getCurrentUser()->userid);
        fp = fopen("friendbook", "r");  /*����ϵͳ Ѱ������ */
        while (fp != NULL && fgets(buf, sizeof(buf), fp) != NULL) {
            char uid[14];

            ptr = strstr(buf, "@");
            if (ptr == NULL) {
                del_from_file("friendbook", buf);
                continue;
            }
            ptr++;
            strcpy(uid, ptr);
            ptr = strstr(uid, "\n");
            *ptr = '\0';
            if (!strcmp(uid, getCurrentUser()->userid))      /*ɾ�����û��� Ѱ������ */
                del_from_file("friendbook", buf);       /*Ѱ������ֻ�ڱ���������Ч */
        }
        if (fp)                                                                                        /*---	add by period 2000-11-11 fix null hd bug	---*/
            fclose(fp);
    }
    sleep(1);
    pressreturn();              /*Haohmaru.98.10.18 */
    output("\x1b[m",3);
    output("\x1b[H\x1b[J",6);
    oflush();

    end_mmapfile(currentmemo, sizeof(struct usermemo), -1);

    shutdown(0, 2);
    close(0);
    exit(0);
    return -1;
}



int Info()
{                               /* ��ʾ�汾��ϢVersion.Info */
    modify_user_mode(XMENU);
    ansimore("Version.Info", true);
    clear();
    return 0;
}

int Conditions()
{                               /* ��ʾ��Ȩ��ϢCOPYING */
    modify_user_mode(XMENU);
    ansimore("COPYING", true);
    clear();
    return 0;
}

int ShowWeather()
{                               /* ��ʾ�汾��ϢVersion.Info */
    modify_user_mode(XMENU);
    ansimore("WEATHER", true);
    clear();
    return 0;
}

int Welcome()
{                               /* ��ʾ��ӭ���� Welcome */
    modify_user_mode(XMENU);
    ansimore("Welcome", true);
    clear();
    return 0;
}

int cmpbnames(char *bname, struct fileheader *brec)
{
    if (!strncasecmp(bname, brec->filename, sizeof(brec->filename)))
        return 1;
    else
        return 0;
}

void RemoveAppendedSpace(char *ptr)
{                               /* Leeward 98.02.13 */
    int Offset;

    /*
     * Below block removing extra appended ' ' in article titles 
     */
    Offset = strlen(ptr);
    for (--Offset; Offset > 0; Offset--) {
        if (' ' != ptr[Offset])
            break;
        else
            ptr[Offset] = 0;
    }
}

int i_read_mail()
{
    if(!HAS_PERM(getCurrentUser(), PERM_BASIC)||!strcmp(getCurrentUser()->userid, "guest")) return DONOTHING;
    if (HAS_MAILBOX_PROP(&uinfo, MBP_MAILBOXSHORTCUT))
    	MailProc();
    else
    	m_read();
    setmailcheck(getCurrentUser()->userid);
    return FULLUPDATE;
}

#define ACL_MAX 10

struct acl_struct {
    unsigned int ip;
    char len;
    char deny;
} * acl;
int aclt=0;

static int set_acl_list_show(struct _select_def *conf, int i)
{
    char buf[80];
    unsigned int ip,ip2;
    ip = acl[i-1].ip;
    if(i-1<aclt) {
        if(acl[i-1].len==0) ip2=ip+0xffffffff;
        else ip2=ip+((1<<(32-acl[i-1].len))-1);
        sprintf(buf, "%d.%d.%d.%d--%d.%d.%d.%d", ip>>24, (ip>>16)%0x100, (ip>>8)%0x100, ip%0x100, ip2>>24, (ip2>>16)%0x100, (ip2>>8)%0x100, ip2%0x100);
        prints("  %2d  %-40s  %4s", i, buf, acl[i-1].deny?"�ܾ�":"����");
    }
    return SHOW_CONTINUE;
}

static int set_acl_list_prekey(struct _select_def *conf, int *key)
{
    switch (*key) {
    case 'e':
    case 'q':
        *key = KEY_LEFT;
        break;
    case 'p':
    case 'k':
        *key = KEY_UP;
        break;
    case ' ':
    case 'N':
        *key = KEY_PGDN;
        break;
    case 'n':
    case 'j':
        *key = KEY_DOWN;
        break;
    }
    return SHOW_CONTINUE;
}

static int set_acl_list_key(struct _select_def *conf, int key)
{
    int oldmode;

    switch (key) {
    case 'a':
        if (aclt<ACL_MAX) {
            char buf[20];
            int ip[4], i, k=0, err=0;
            getdata(0, 0, "������IP��ַ: ", buf, 18, 1, 0, 1);
            for(i=0;i<strlen(buf);i++) if(buf[i]=='.') k++;
            if(k!=3) err=1;
            else {
                if(sscanf(buf, "%d.%d.%d.%d", &ip[0], &ip[1], &ip[2], &ip[3])!=4) err=1;
                else {
                    if(ip[0]==0) err=1;
                    for(i=0;i<4;i++) if(ip[i]<0||ip[i]>=256) err=1;
                }
            }
            if(err) {
                move(0, 0);
                prints("IP�������!");
                clrtoeol();
                refresh(); sleep(1);
            }
            else {
                getdata(0, 0, "�����볤��(��λ:bit): ", buf, 4, 1, 0, 1);
                acl[aclt].len = atoi(buf);
                if(acl[aclt].len<0 || acl[aclt].len>32) err=1;
                if(err) {
                    move(0, 0);
                    prints("�����������!");
                    clrtoeol();
                    refresh(); sleep(1);
                }
                else {
                    getdata(0, 0, "����/�ܾ�(0-����,1-�ܾ�): ", buf, 4, 1, 0, 1);
                    if(buf[0]=='0') acl[aclt].deny=0;
                    else acl[aclt].deny=1;
                    acl[aclt].ip = (ip[0]<<24)+(ip[1]<<16)+(ip[2]<<8)+ip[3];
                    if(acl[aclt].len<32)
                        acl[aclt].ip = acl[aclt].ip&(((1<<acl[aclt].len)-1)<<(32-acl[aclt].len));
                    aclt++;
                    return SHOW_DIRCHANGE;
                }
            }
            return SHOW_REFRESH;
        }
        break;
    case 'd':
        if (aclt > 0) {
            char ans[3];

            getdata(0, 0, "ȷʵҪɾ����(Y/N)? [N]: ", ans, sizeof(ans), DOECHO, NULL, true);
            if (ans[0] == 'Y' || ans[0] == 'y') {
                int i;
                aclt--;
                for(i=conf->pos-1;i<aclt;i++)
                    memcpy(acl+i, acl+i+1, sizeof(struct acl_struct));
                bzero(acl+aclt, sizeof(struct acl_struct));
            }
            return SHOW_DIRCHANGE;
        }
        break;
    case 'm':
        if (aclt > 0) {
            char ans[3];
            int d;

            getdata(0, 0, "������Ҫ�ƶ�����λ��: ", ans, 3, DOECHO, NULL, true);
            d=atoi(ans)-1;
            if (d>=0&&d<=aclt-1&&d!=conf->pos-1) {
                struct acl_struct temp;
                int i, p;
                p = conf->pos-1;
                memcpy(&temp, acl+p, sizeof(struct acl_struct));
                if(p>d) {
                    for(i=p;i>d;i--)
                        memcpy(acl+i, acl+i-1, sizeof(struct acl_struct));
                } else {
                    for(i=p;i<d;i++)
                        memcpy(acl+i, acl+i+1, sizeof(struct acl_struct));
                }
                memcpy(acl+d, &temp, sizeof(struct acl_struct));
                return SHOW_DIRCHANGE;
            }
        }
        break;
    case 'L':
    case 'l':
        oldmode = uinfo.mode;
        show_allmsgs();
        modify_user_mode(oldmode);
        return SHOW_REFRESH;
    case 'W':
    case 'w':
        oldmode = uinfo.mode;
        if (!HAS_PERM(getCurrentUser(), PERM_PAGE))
            break;
        s_msg();
        modify_user_mode(oldmode);
        return SHOW_REFRESH;
    case 'u':
        oldmode = uinfo.mode;
        clear();
        modify_user_mode(QUERY);
        t_query(NULL);
        modify_user_mode(oldmode);
        clear();
        return SHOW_REFRESH;
    }

    return SHOW_CONTINUE;
}

static int set_acl_list_refresh(struct _select_def *conf)
{
    clear();
    docmdtitle("[��½IP�����б�]",
               "�˳�[\x1b[1;32m��\x1b[0;37m,\x1b[1;32me\x1b[0;37m] ѡ��[\x1b[1;32m��\x1b[0;37m,\x1b[1;32m��\x1b[0;37m] ���[\x1b[1;32ma\x1b[0;37m] ɾ��[\x1b[1;32md\x1b[0;37m]\x1b[m");
    move(2, 0);
    prints("\033[0;1;37;44m  %4s  %-40s %-31s", "����", "IP��ַ��Χ", "����/�ܾ�");
    clrtoeol();
    update_endline();
    return SHOW_CONTINUE;
}

static int set_acl_list_getdata(struct _select_def *conf, int pos, int len)
{
    conf->item_count = aclt;
    if(conf->item_count==0)
        conf->item_count=1;

    return SHOW_CONTINUE;
}

int set_ip_acl()
{
    struct _select_def grouplist_conf;
    POINT *pts;
    int i,rip[4];
    int oldmode;
    FILE* fp;
    char fn[80],buf[80];

    clear();
    getdata(3, 0, "�������������: ", buf, 39, NOECHO, NULL, true);
    if (*buf == '\0' || !checkpasswd2(buf, getCurrentUser())) {
        prints("\n\n�ܱ�Ǹ, ����������벻��ȷ��\n");
        pressanykey();
        return 0;
    }

    acl = (struct acl_struct *) malloc(sizeof(struct acl_struct)*ACL_MAX);
    aclt=0;
    bzero(acl, sizeof(struct acl_struct)*ACL_MAX);
    sethomefile(fn, getCurrentUser()->userid, "ipacl");
    fp=fopen(fn, "r");
    if(fp){
        i=0;
        while(!feof(fp)) {
            int len,deny;
            if(fscanf(fp, "%d.%d.%d.%d %d %d", &rip[0], &rip[1], &rip[2], &rip[3], &len, &deny)<=0) break;
            acl[i].len=(char)len;
            acl[i].deny=(char)deny;
            acl[i].ip = (rip[0]<<24)+(rip[1]<<16)+(rip[2]<<8)+rip[3];
            i++;
            if(i>=ACL_MAX) break;
        }
        aclt = i;
        fclose(fp);
    }
    clear();
    oldmode = uinfo.mode;
    modify_user_mode(SETACL);
    //TODO: ���ڴ�С��̬�ı�������������bug
    pts = (POINT *) malloc(sizeof(POINT) * BBS_PAGESIZE);
    for (i = 0; i < BBS_PAGESIZE; i++) {
        pts[i].x = 2;
        pts[i].y = i + 3;
    }
    bzero(&grouplist_conf, sizeof(struct _select_def));
    grouplist_conf.item_count = aclt;
    if(grouplist_conf.item_count==0)
        grouplist_conf.item_count=1;
    grouplist_conf.item_per_page = BBS_PAGESIZE;
    /*
     * ���� LF_VSCROLL ������ LEFT ���˳� 
     */
    grouplist_conf.flag = LF_VSCROLL | LF_BELL | LF_LOOP | LF_MULTIPAGE;
    grouplist_conf.prompt = "��";
    grouplist_conf.item_pos = pts;
    grouplist_conf.title_pos.x = 0;
    grouplist_conf.title_pos.y = 0;
    grouplist_conf.pos = 1;     /* initialize cursor on the first mailgroup */
    grouplist_conf.page_pos = 1;        /* initialize page to the first one */

    grouplist_conf.show_data = set_acl_list_show;
    grouplist_conf.pre_key_command = set_acl_list_prekey;
    grouplist_conf.key_command = set_acl_list_key;
    grouplist_conf.show_title = set_acl_list_refresh;
    grouplist_conf.get_data = set_acl_list_getdata;

    list_select_loop(&grouplist_conf);
    free(pts);
    modify_user_mode(oldmode);
    fp=fopen(fn, "w");
    if(fp){
        for(i=0;i<aclt;i++)
            fprintf(fp, "%d.%d.%d.%d %d %d\n", acl[i].ip>>24, (acl[i].ip>>16)%0x100, (acl[i].ip>>8)%0x100, acl[i].ip%0x100, acl[i].len, acl[i].deny);
        fclose(fp);
    }

    return 0;

}


/***************************************************************************
 * add by stiger, ������ģ��
 ***************************************************************************/

struct a_template * ptemplate = NULL ;
int template_num = 0;
int t_now = 0;

int tmpl_init(int mode){

	int newmode=0;
	int ret;

	if(mode==1 || chk_currBM(currBM, getCurrentUser())) newmode = 1;

	ret = orig_tmpl_init(currboard->filename, newmode, & ptemplate);

	if(ret >= 0) template_num = ret;

	return ret;

}

void tmpl_free(){
	orig_tmpl_free( & ptemplate, template_num );
	template_num = 0;
}

int tmpl_save(){

	return orig_tmpl_save(ptemplate, template_num, currboard->filename);

}

int tmpl_add(){

	char buf[60];
	struct s_template tmpl;

	if( template_num >= MAX_TEMPLATE )
		return -1;

	bzero(&tmpl, sizeof(struct s_template));

	clear();
	buf[0]='\0';
	getdata(t_lines - 1, 0, "ģ�����: ", buf, 50, DOECHO, NULL, false);
	if( buf[0]=='\0' || buf[0]=='\n' ){
		return -1;
	}
	strncpy(tmpl.title, buf, 50);
	tmpl.title[49] = '\0';

	ptemplate[template_num].tmpl = (struct s_template *) malloc( sizeof(struct s_template) );
	memcpy( ptemplate[template_num].tmpl, &tmpl, sizeof(struct s_template) );
	template_num ++;

	tmpl_save();

	return 0;
}

int content_add(){

	struct s_content ct;
	char buf[60];

	if( ptemplate[t_now].tmpl->content_num >= MAX_CONTENT )
		return -1;

	bzero(&ct, sizeof(struct s_content));
	clear();
	buf[0]='\0';
	getdata(t_lines - 1, 0, "ѡ�����: ", buf, 50, DOECHO, NULL, false);
	if( buf[0]=='\0' || buf[0]=='\n' ){
		return -1;
	}
	strncpy(ct.text, buf, 50);
	ct.text[49]='\0';

	buf[0]='\0';
	getdata(t_lines - 1, 0, "ѡ���: ", buf, 5, DOECHO, NULL, false);
	ct.length = atoi(buf);
	if(ct.length <= 0 || ct.length > MAX_CONTENT_LENGTH )
		return -1;

	ptemplate[t_now].cont = (struct s_content *) realloc( ptemplate[t_now].cont, sizeof(struct s_content) * (ptemplate[t_now].tmpl->content_num+1));
	memcpy( &( ptemplate[t_now].cont[ptemplate[t_now].tmpl->content_num] ), &ct, sizeof(struct s_content) );
	ptemplate[t_now].tmpl->content_num ++;

	tmpl_save();

	return 0;

}


static int tmpl_show(struct _select_def *conf, int i)
{
	prints(" %2d %s%-60s %3d", i, ptemplate[i-1].tmpl->flag & TMPL_BM_FLAG ? "\033[1;31mB\033[m":" ", ptemplate[i-1].tmpl->title, ptemplate[i-1].tmpl->content_num);
	return SHOW_CONTINUE;
}

static int content_show(struct _select_def *conf, int i)
{
	prints(" %2d     %-50s  %3d", i,ptemplate[t_now].cont[i-1].text, ptemplate[t_now].cont[i-1].length);
	return SHOW_CONTINUE;
}

static int tmpl_prekey(struct _select_def *conf, int *key)
{
    switch (*key) {
    case 'q':
	case 'e':
        *key = KEY_LEFT;
        break;
    case 'p':
    case 'k':
        *key = KEY_UP;
        break;
    case 'N':
        *key = KEY_PGDN;
        break;
    case 'n':
    case 'j':
        *key = KEY_DOWN;
        break;
    case ' ':
		*key = '\n';
		break;
    }
    return SHOW_CONTINUE;
}

static int tmpl_refresh(struct _select_def *conf)
{
    clear();
    docmdtitle("[����ģ������]",
               "���[\x1b[1;32ma\x1b[0;37m] ɾ��[\x1b[1;32md\x1b[0;37m]\x1b[m ����[\033[1;32mt\033[0;37m] \033[1;33m�鿴\033[m ����[\033[1;32mx\033[m] ����[\033[1;32ms\033[m] \033[1;33m�޸�\033[m ����[\033[1;32mi\033[0;37m] ����[\033[1;32mf\033[0;37m]");
    move(2, 0);
    prints("\033[0;1;37;44m %4s %-60s %8s", "���", "ģ������","�������");
    clrtoeol();
    update_endline();
    return SHOW_CONTINUE;
}

static int content_refresh(struct _select_def *conf)
{
    clear();
    docmdtitle("[����ģ����������]",
               "���[\x1b[1;32ma\x1b[0;37m] ɾ��[\x1b[1;32md\x1b[0;37m]\x1b[m �޸���������[\033[1;32mt\033[0;37m] �޸Ļش𳤶�[\033[1;32ml\033[0;37m]");
    move(2, 0);
    prints("\033[0;1;37;44m %4s     %-50s  %8s", "���", "��������","�ش𳤶�");
    clrtoeol();
    update_endline();
    return SHOW_CONTINUE;
}

static int tmpl_getdata(struct _select_def *conf, int pos, int len)
{
    conf->item_count = template_num;
    return SHOW_CONTINUE;
}

static int content_getdata(struct _select_def *conf, int pos, int len)
{
    conf->item_count = ptemplate[t_now].tmpl->content_num;
    return SHOW_CONTINUE;
}

static int content_key(struct _select_def *conf, int key)
{
	switch (key) {
	case 'm':
		{
			char ans[3];
			int newm;
			getdata(t_lines-1, 0, "�ƶ����´���:", ans, 2, DOECHO, NULL, true);
			if( ans[0]=='\0' || ans[0]=='\n' || ans[0]=='\r' )
				return SHOW_REFRESH;
			newm=atoi(ans);

			if(newm <= 0 || newm > ptemplate[t_now].tmpl->content_num)
				return SHOW_REFRESH;

			if( newm > conf->pos ){
				int i;
				struct s_content sc;
				memcpy(&sc, &ptemplate[t_now].cont[conf->pos-1], sizeof(struct s_content));
				for(i=conf->pos;i<newm;i++)
					memcpy(& ptemplate[t_now].cont[i-1], & ptemplate[t_now].cont[i], sizeof(struct s_content));
				memcpy(&ptemplate[t_now].cont[newm-1], &sc, sizeof(struct s_content));

				tmpl_save();

				return SHOW_REFRESH;
			}else if(newm < conf->pos){
				int i;
				struct s_content sc;
				memcpy(&sc, &ptemplate[t_now].cont[conf->pos-1], sizeof(struct s_content));
				for(i=conf->pos;i>newm;i--)
					memcpy(& ptemplate[t_now].cont[i-1], & ptemplate[t_now].cont[i-2], sizeof(struct s_content));
				memcpy(&ptemplate[t_now].cont[newm-1], &sc, sizeof(struct s_content));

				tmpl_save();

				return SHOW_REFRESH;
			}else
				return SHOW_REFRESH;

		}
	case 'a':
		if( ptemplate[t_now].tmpl->content_num >= MAX_CONTENT ){
			char ans[STRLEN];
			move(t_lines - 1, 0);
			clrtoeol();
			a_prompt(-1, "ѡ�����������س����� << ", ans);
			move(t_lines - 1, 0);
			clrtoeol();
			return SHOW_CONTINUE;
		}
		content_add();
		return SHOW_DIRCHANGE;
		break;
	case 'd':
		{
            char ans[3];

            getdata(t_lines - 1, 0, "ȷʵҪɾ����(Y/N)? [N]: ", ans, sizeof(ans), DOECHO, NULL, true);
            if (ans[0] == 'Y' || ans[0] == 'y') {
                int i=0;
				struct s_content *ct;

				if( ptemplate[t_now].tmpl->content_num == 1){
					ptemplate[t_now].tmpl->content_num = 0;
					free(ptemplate[t_now].cont);

					tmpl_save();
					return SHOW_QUIT;
				}

				ct = (struct s_content *) malloc( sizeof(struct s_content) * (ptemplate[t_now].tmpl->content_num-1));

				memcpy(ct+i,&(ptemplate[t_now].cont[i]),sizeof(struct s_content) * (conf->pos-1));
                for(i=conf->pos-1;i<ptemplate[t_now].tmpl->content_num-1;i++)
                    memcpy(ct+i, &(ptemplate[t_now].cont[i+1]), sizeof(struct s_content));

				free(ptemplate[t_now].cont);
				ptemplate[t_now].cont = ct;

				ptemplate[t_now].tmpl->content_num --;

				tmpl_save();
				return SHOW_DIRCHANGE;
			}
			return SHOW_REFRESH;
		}
	case 't' :
		{
			char newtitle[60];

			strcpy(newtitle, ptemplate[t_now].cont[conf->pos-1].text);
            getdata(t_lines - 1, 0, "�±���: ", newtitle, 50, DOECHO, NULL, false);

			if( newtitle[0] == '\0' || newtitle[0]=='\n' || ! strcmp(newtitle,ptemplate[t_now].cont[conf->pos-1].text) )
				return SHOW_REFRESH;

			strncpy(ptemplate[t_now].cont[conf->pos-1].text, newtitle, 50);
			ptemplate[t_now].cont[conf->pos-1].text[49]='\0';

			tmpl_save();

			return SHOW_REFRESH;
		}
		break;
	case 'l' :
		{
			char newlen[10];
			int nl;

			sprintf(newlen,"%-3d",ptemplate[t_now].cont[conf->pos-1].length);
			getdata(t_lines-1, 0, "�³���: ", newlen, 5, DOECHO, NULL, false);

			nl = atoi(newlen);

			if( nl <= 0 || nl > MAX_CONTENT_LENGTH || nl == ptemplate[t_now].cont[conf->pos-1].length )
				return SHOW_REFRESH;

			ptemplate[t_now].cont[conf->pos-1].length = nl;

			tmpl_save();

			return SHOW_REFRESH;
		}
		break;
	default:
		break;
	}

	return SHOW_CONTINUE;
}

static int tmpl_key(struct _select_def *conf, int key)
{
	switch (key) {
	case 'a' :
		if( template_num >= MAX_TEMPLATE ){
			char ans[STRLEN];
			move(t_lines - 1, 0);
			clrtoeol();
			a_prompt(-1, "ģ�����������س����� << ", ans);
			move(t_lines - 1, 0);
			clrtoeol();
			return SHOW_CONTINUE;
		}
		tmpl_add();
		return SHOW_DIRCHANGE;
		break;
	case 'd' :
		{
            char ans[3];

            getdata(t_lines - 1, 0, "ȷʵҪɾ����(Y/N)? [N]: ", ans, sizeof(ans), DOECHO, NULL, true);
            if (ans[0] == 'Y' || ans[0] == 'y') {
                int i;
				char filepath[STRLEN];

				if( ptemplate[conf->pos-1].tmpl->filename[0] ){
					setbfile(filepath,currboard->filename, ptemplate[conf->pos-1].tmpl->filename);
					if(dashf(filepath))
						my_unlink(filepath);
				}

				if( ptemplate[conf->pos-1].tmpl != NULL)
					free(ptemplate[conf->pos-1].tmpl);
				if( ptemplate[conf->pos-1].cont != NULL)
					free(ptemplate[conf->pos-1].cont);

                template_num--;
                for(i=conf->pos-1;i<template_num;i++)
                    memcpy(ptemplate+i, ptemplate+i+1, sizeof(struct a_template));
				ptemplate[template_num].tmpl = NULL;
				ptemplate[template_num].cont = NULL;

				tmpl_save();
            }
			if(template_num > 0)
            	return SHOW_DIRCHANGE;
			else
				return SHOW_QUIT;
        }
        break;
	case 't' :
		{
			char newtitle[60];

			strcpy(newtitle, ptemplate[conf->pos-1].tmpl->title);
            getdata(t_lines - 1, 0, "������: ", newtitle, 50, DOECHO, NULL, false);

			if( newtitle[0] == '\0' || newtitle[0]=='\n' || ! strcmp(newtitle,ptemplate[conf->pos-1].tmpl->title) )
				return SHOW_REFRESH;

			strncpy(ptemplate[conf->pos-1].tmpl->title, newtitle, 50);
			ptemplate[conf->pos-1].tmpl->title[49]='\0';

			tmpl_save();

			return SHOW_REFRESH;
		}
		break;/*
	case 'z' :
		{
			char newtitle[30];

			strcpy(newtitle, ptemplate[conf->pos-1].tmpl->title_prefix);
            getdata(t_lines - 1, 0, "�������ģ������±���ǰ׺: ", newtitle, 20, DOECHO, NULL, false);

			if( newtitle[0] == '\0' || newtitle[0]=='\n' || ! strcmp(newtitle,ptemplate[conf->pos-1].tmpl->title_prefix) )
				return SHOW_REFRESH;

			strncpy(ptemplate[conf->pos-1].tmpl->title_prefix, newtitle, 20);
			ptemplate[conf->pos-1].tmpl->title_prefix[19]='\0';

			tmpl_save();

			return SHOW_REFRESH;
		}*/
	case 'f' :
		{
			char filepath[STRLEN];
			int oldmode;

        oldmode = uinfo.mode;
        modify_user_mode(EDITUFILE);

			if( ptemplate[conf->pos-1].tmpl->filename[0] == '\0' ){

	    		setbfile(filepath, currboard->filename, "");
   	 			if ( GET_POSTFILENAME(ptemplate[conf->pos-1].tmpl->filename, filepath) != 0) {
					clear();
					move(3,0);
					prints("����ģ���ļ�ʧ��!");
					pressanykey();
					return SHOW_REFRESH;
				}
				tmpl_save();
			}

			setbfile(filepath, currboard->filename, ptemplate[conf->pos-1].tmpl->filename);

			vedit(filepath,0,NULL,NULL);
        modify_user_mode(oldmode);

			return SHOW_REFRESH;
		}
	case 's' :
		{
			char filepath[STRLEN];
			setbfile(filepath, currboard->filename, ptemplate[conf->pos-1].tmpl->filename);
			clear();
			ansimore(filepath,1);
			return SHOW_REFRESH;
		}
	case 'b' :
		{
			if( ptemplate[conf->pos-1].tmpl->flag & TMPL_BM_FLAG )
				ptemplate[conf->pos-1].tmpl->flag &= ~TMPL_BM_FLAG ;
			else
				ptemplate[conf->pos-1].tmpl->flag |= TMPL_BM_FLAG;

			tmpl_save();

			return SHOW_REFRESH;
		}
	case 'i' :
		{
			char newtitle[STRLEN];

			strcpy(newtitle, ptemplate[conf->pos-1].tmpl->title_tmpl);
            getdata(t_lines - 1, 0, "�����±���: ", newtitle, STRLEN, DOECHO, NULL, false);

			if( newtitle[0] == '\0' || newtitle[0]=='\n' || ! strcmp(newtitle,ptemplate[conf->pos-1].tmpl->title_tmpl) )
				return SHOW_REFRESH;

			strncpy(ptemplate[conf->pos-1].tmpl->title_tmpl, newtitle, STRLEN);
			ptemplate[conf->pos-1].tmpl->title_tmpl[STRLEN-1]='\0';

			tmpl_save();

			return SHOW_REFRESH;
		}
	case 'x' :
		{
			clear();
			move(2,0);
			prints("��ģ��ı�������Ϊ");
			move(4,0);
			prints("%s",ptemplate[conf->pos-1].tmpl->title_tmpl);
			pressanykey();
			return SHOW_REFRESH;
		}
		/*
	case 'h':
		{
			clear();
			move(1,0);
			prints("  x  :  �鿴�����ʽ\n");
			prints("  i  :  �޸ı����ʽ");
			pressanykey();
			return SHOW_REFRESH;
		}
		*/
	default :
		break;
	}

	return SHOW_CONTINUE;
}

static int tmpl_select(struct _select_def *conf)
{

	int i;
	POINT *pts;
    struct _select_def grouplist_conf;

	clear();
	t_now = conf->pos - 1;
	
	if( ptemplate[t_now].tmpl->content_num == 0 ){
		char ans[3];
		clear();
        getdata(t_lines - 1, 0, "��ģ������û�����ݣ���Ҫ����������(Y/N)? [N]: ", ans, sizeof(ans), DOECHO, NULL, true);
        if (ans[0] != 'Y' && ans[0] != 'y'){
			return SHOW_REFRESH;
		}
		if( content_add() < 0 ){
			return SHOW_REFRESH;
		}
	}

    pts = (POINT *) malloc(sizeof(POINT) * BBS_PAGESIZE);
    for (i = 0; i < BBS_PAGESIZE; i++) {
        pts[i].x = 2;
        pts[i].y = i + 3;
    }
    bzero(&grouplist_conf, sizeof(struct _select_def));

    grouplist_conf.item_count = ptemplate[t_now].tmpl->content_num;
    grouplist_conf.item_per_page = BBS_PAGESIZE;
    /*
     * ���� LF_VSCROLL ������ LEFT ���˳� 
     */
    grouplist_conf.flag = LF_VSCROLL | LF_BELL | LF_LOOP | LF_MULTIPAGE;
    grouplist_conf.prompt = "��";
    grouplist_conf.item_pos = pts;
    grouplist_conf.title_pos.x = 0;
    grouplist_conf.title_pos.y = 0;
    grouplist_conf.pos = 1;     /* initialize cursor on the first mailgroup */
    grouplist_conf.page_pos = 1;        /* initialize page to the first one */

    grouplist_conf.show_data = content_show;
    grouplist_conf.pre_key_command = tmpl_prekey;
    grouplist_conf.key_command = content_key;
    grouplist_conf.show_title = content_refresh;
    grouplist_conf.get_data = content_getdata;

    list_select_loop(&grouplist_conf);

    free(pts);

	t_now = 0;

	return SHOW_REFRESH;
}

int m_template()
{
	int i;
	POINT *pts;
    struct _select_def grouplist_conf;

	if (!chk_currBM(currBM, getCurrentUser())) {
		return DONOTHING;
	}

	if( tmpl_init(1) < 0 )
		return FULLUPDATE;

	if( template_num == 0 ){
		char ans[3];
		clear();
        getdata(t_lines - 1, 0, "��������û��ģ�壬��Ҫ����������(Y/N)? [N]: ", ans, sizeof(ans), DOECHO, NULL, true);
        if (ans[0] != 'Y' && ans[0] != 'y'){
			tmpl_free();
			return FULLUPDATE;
		}
		if( tmpl_add() < 0 ){
			tmpl_free();
			return FULLUPDATE;
		}
	}

    pts = (POINT *) malloc(sizeof(POINT) * BBS_PAGESIZE);
    for (i = 0; i < BBS_PAGESIZE; i++) {
        pts[i].x = 2;
        pts[i].y = i + 3;
    }
    bzero(&grouplist_conf, sizeof(struct _select_def));

    grouplist_conf.item_count = template_num;
    grouplist_conf.item_per_page = BBS_PAGESIZE;
    /*
     * ���� LF_VSCROLL ������ LEFT ���˳� 
     */
    grouplist_conf.flag = LF_VSCROLL | LF_BELL | LF_LOOP | LF_MULTIPAGE;
    grouplist_conf.prompt = "��";
    grouplist_conf.item_pos = pts;
    grouplist_conf.title_pos.x = 0;
    grouplist_conf.title_pos.y = 0;
    grouplist_conf.pos = 1;     /* initialize cursor on the first mailgroup */
    grouplist_conf.page_pos = 1;        /* initialize page to the first one */

    grouplist_conf.show_data = tmpl_show;
    grouplist_conf.pre_key_command = tmpl_prekey;
    grouplist_conf.key_command = tmpl_key;
    grouplist_conf.show_title = tmpl_refresh;
    grouplist_conf.get_data = tmpl_getdata;
	grouplist_conf.on_select = tmpl_select;

    list_select_loop(&grouplist_conf);

    free(pts);
	tmpl_free();

    return FULLUPDATE;

}

/*********************************
 * ģ��ѡ����
 ***************************/
static int choose_tmpl_refresh(struct _select_def *conf)
{
    clear();
    docmdtitle("[����ģ��ѡ��]",
               "�˳�[\x1b[1;32m��\x1b[0;37m] ѡ��[\x1b[1;32m��\x1b[0;37m,\x1b[1;32m��\x1b[0;37m] ʹ��[\x1b[1;32mSPACE\x1b[0;37m] �鿴����[\033[1;32ms\033[0;37m] �鿴����[\033[1;32mw\033[0;37m] �鿴����[\033[1;32mx\033[m]");
    move(2, 0);
    prints("\033[0;1;37;44m %4s %-60s %8s", "���", "����","�������");
    clrtoeol();
    update_endline();
    return SHOW_CONTINUE;
}

static int choose_tmpl_getdata(struct _select_def *conf, int pos, int len)
{
    conf->item_count = template_num;
    return SHOW_CONTINUE;
}

static int choose_tmpl_select(struct _select_def *conf)
{
	t_now = conf->pos;
	return SHOW_QUIT;
}

static int choose_tmpl_post(char * title, char *fname){

	FILE *fp;
	FILE *fpsrc;
	char filepath[STRLEN];
	int i;
	int write_ok = 0;
	char * tmp[ MAX_CONTENT ];
	char newtitle[STRLEN];
	int oldmode = uinfo.mode;

	if(t_now <= 0 || t_now > MAX_TEMPLATE )
		return -1;

	if( ptemplate[t_now-1].tmpl->content_num <= 0 )
		return -1;

	if((fp = fopen(fname, "w"))==NULL){
		return -1;
	}

    modify_user_mode(POSTTMPL);
	for(i=0; i< ptemplate[t_now-1].tmpl->content_num; i++){
		char *ans;

		ans = (char *)malloc(ptemplate[t_now-1].cont[i].length + 2);
		if( ans == NULL ){
    		modify_user_mode(oldmode);
			fclose(fp);
			return -1;
		}
		clear();
		move(1,0);
		prints("Ctrl+Q ����, ENTER ����");
		move(3,0);
		prints("ģ������:%s",ptemplate[t_now-1].cont[i].text);
		move(4,0);
		prints("ģ��ش�(�%d�ַ�):",ptemplate[t_now-1].cont[i].length);
        multi_getdata(6, 0, 79, NULL, ans, ptemplate[t_now-1].cont[i].length+1, 11, true, 0);
		tmp[i] = ans;
	}
    modify_user_mode(oldmode);

	if( ptemplate[t_now-1].tmpl->filename[0] ){
		setbfile( filepath,currboard->filename, ptemplate[t_now-1].tmpl->filename);
		if( dashf( filepath )){
			if((fpsrc = fopen(filepath,"r"))!=NULL){
				char buf[256];

				while(fgets(buf,255,fpsrc)){
					int l;
					int linex = 0;
					char *pn,*pe;

					for(pn = buf; *pn!='\0'; pn++){
						if( *pn != '[' || *(pn+1)!='$' ){
							fputc(*pn, fp);
							linex++;
						}else{
							pe = strchr(pn,']');
							if(pe == NULL){
								fputc(*pn, fp);
								continue;
							}
							l = atoi(pn+2);
							if( l<=0 || l > ptemplate[t_now-1].tmpl->content_num ){
								fputc('[', fp);
								continue;
							}
							fprintf(fp,"%s",tmp[l-1]);
							pn = pe;
							continue;
						}
					}
				}
				fclose(fpsrc);

				write_ok = 1;
			}
		}
	}
	if(write_ok == 0){
		for(i=0; i< ptemplate[t_now-1].tmpl->content_num; i++)
			fprintf(fp,"\033[1;32m%s:\033[m\n%s\n\n",ptemplate[t_now-1].cont[i].text, tmp[i]);
	}
	fclose(fp);

	if( ptemplate[t_now-1].tmpl->title_tmpl[0] ){
		char *pn,*pe;
		char *buf;
		int l;
		int newl = 0;

		newtitle[0]='\0';
		buf = ptemplate[t_now-1].tmpl->title_tmpl;

		for(pn = buf; *pn!='\0' && newl < STRLEN-1; pn++){
			if( *pn != '[' || *(pn+1)!='$' ){
				if( newl < STRLEN - 1 ){
					newtitle[newl] = *pn ;
					newtitle[newl+1]='\0';
					newl ++;
				}
			}else{
				pe = strchr(pn,']');
				if(pe == NULL){
					if( newl < STRLEN - 1 ){
						newtitle[newl] = *pn ;
						newtitle[newl+1]='\0';
						newl ++;
					}
					continue;
				}
				l = atoi(pn+2);
				if( l<0 || l > ptemplate[t_now-1].tmpl->content_num ){
					if( newl < STRLEN - 1 ){
						newtitle[newl] = *pn ;
						newtitle[newl+1]='\0';
						newl ++;
					}
					continue;
				}
				if( l == 0 ){
					int ti;
					for( ti=0; title[ti]!='\0' && newl < STRLEN - 1; ti++, newl++ ){
						newtitle[newl] = title[ti] ;
						newtitle[newl+1]='\0';
					}
				}else{
					int ti;
					for( ti=0; tmp[l-1][ti]!='\0' && tmp[l-1][ti]!='\n' && tmp[l-1][ti]!='\r' && newl < STRLEN - 1; ti++, newl++ ){
						newtitle[newl] = tmp[l-1][ti] ;
						newtitle[newl+1]='\0';
					}
				}
				pn = pe;
				continue;
			}
		}
		strncpy(title, newtitle, STRLEN);
		title[STRLEN-1]='\0';
	}


	for(i=0; i< ptemplate[t_now-1].tmpl->content_num; i++)
		free( tmp[i] );

	{
		char ans[3];
		clear();
        ansimore2(fname, false, 0, 19 /*19 */ );
		move(21,0);
		prints("����:%s",title);
        getdata(t_lines - 1, 0, "ȷʵҪ������(Y/N)? [Y]: ", ans, sizeof(ans), DOECHO, NULL, true);
        if (ans[0] == 'N' || ans[0] == 'n') {
			return -1;
		}
	}

	return 1;
}

static int choose_tmpl_key(struct _select_def *conf, int key)
{
	switch (key) {
	case 's' :
	{
		char filepath[STRLEN];

		if( conf->pos > template_num )
			return SHOW_CONTINUE;

		if( ptemplate[conf->pos-1].tmpl->filename[0] ){
			clear();
			setbfile(filepath,currboard->filename, ptemplate[conf->pos-1].tmpl->filename);

			ansimore(filepath, 1);
			return SHOW_REFRESH;
		}

		return SHOW_CONTINUE;
	}
		break;
	case 'w':
		{
			clear();
			if( ptemplate[conf->pos-1].tmpl->content_num <= 0 ){
				move(5,0);
				prints("����û���������⣬��ģ���ݲ�����\n");
			}else{
				int i;
				for(i=0;i<ptemplate[conf->pos-1].tmpl->content_num;i++){
					move(i+2,0);
					prints("\033[1;32m���� %d\033[m:%s  \033[1;32m��ش�\033[m%d\033[1;32m�ֽ�\033[m", i+1, ptemplate[conf->pos-1].cont[i].text, ptemplate[conf->pos-1].cont[i].length);
				}
			}
			pressanykey();

			return SHOW_REFRESH;
		}
	case 'x' :
		{
			clear();
			move(2,0);
			prints("��ģ��ı�������Ϊ");
			move(4,0);
			prints("%s",ptemplate[conf->pos-1].tmpl->title_tmpl);
			pressanykey();
			return SHOW_REFRESH;
		}
	default:
		break;
	}

	return SHOW_CONTINUE;
}

static int choose_tmpl(char *title, char *fname)
{
	POINT *pts;
    struct _select_def grouplist_conf;
    int i;

	if( tmpl_init(0) < 0 )
		return  -1;

	if( template_num == 0 ){
		clear();
		move(3,0);
		prints("����û��ģ��ɹ�ʹ��");
		pressanykey();
		tmpl_free();
		return  -1;
	}

	t_now = 0;

    pts = (POINT *) malloc(sizeof(POINT) * BBS_PAGESIZE);
    for (i = 0; i < BBS_PAGESIZE; i++) {
        pts[i].x = 2;
        pts[i].y = i + 3;
    }
    bzero(&grouplist_conf, sizeof(struct _select_def));

	grouplist_conf.arg = fname;
    grouplist_conf.item_count = template_num;
    grouplist_conf.item_per_page = BBS_PAGESIZE;
    grouplist_conf.flag = LF_VSCROLL | LF_BELL | LF_LOOP | LF_MULTIPAGE;
    grouplist_conf.prompt = "��";
    grouplist_conf.item_pos = pts;
    grouplist_conf.title_pos.x = 0;
    grouplist_conf.title_pos.y = 0;
    grouplist_conf.pos = 1;
    grouplist_conf.page_pos = 1;

    grouplist_conf.show_data = tmpl_show;
    grouplist_conf.pre_key_command = tmpl_prekey;
    grouplist_conf.key_command = choose_tmpl_key;
    grouplist_conf.show_title = choose_tmpl_refresh;
    grouplist_conf.get_data = choose_tmpl_getdata;
	grouplist_conf.on_select = choose_tmpl_select;

    list_select_loop(&grouplist_conf);

	if(t_now > 0){
		if( choose_tmpl_post(title, fname) < 0 )
			t_now = 0;
	}

	free(pts);
	tmpl_free();

	if( t_now > 0){
		t_now = 0;
		return 0;
	}
	return -1;
}

int b_results(struct _select_def* conf,struct fileheader *fileinfo,void* extraarg)
{
    return vote_results(currboard->filename);
}

enum {
    BM_DELETE=1,
    BM_MARK,
    BM_DIGEST,
    BM_IMPORT,
    BM_TMP,
    BM_MARKDEL,
    BM_NOREPLY,
    BM_TOTAL
};

const char *SR_BMitems[] = {
    "ɾ��",
    "����",
    "��ժ",
    "���뾫����",
    "�����ݴ浵",
    "���ɾ��",
    "��Ϊ���ɻظ�",
    "���ϼ�"
};
const int item_num = 8;

struct BMFunc_arg {
    bool delpostnum; /*�Ƿ��������*/
    int action;            /*����������ΪBM_DELETE��BM_TOTAL����֮һ*/
    bool saveorigin;    /*�ںϼ�������ʱ������Ƿ񱣴�ԭ��*/
    char* announce_path; /*��¼��������ʱ���λ��*/
    bool setflag; /*���û���ȡ��*/
};

/*����ͬ���⺯��������apply_record�Ļص�����*/
static int BM_thread_func(struct _select_def* conf, struct fileheader* fh,int ent, void* extraarg)
{
    struct read_arg* arg=(struct read_arg*)conf->arg;
    struct BMFunc_arg* func_arg=(struct BMFunc_arg*)extraarg;
    int ret=APPLY_CONTINUE;

    conf->pos=ent;
    if (arg->writearg) {
        arg->writearg->ent=ent;
    }
    switch (func_arg->action) {
        case BM_DELETE:
            if (!(fh->accessed[0] & FILE_MARKED)) {
                if (del_post(conf,fh,(void*)(ARG_BMFUNC_FLAG|ARG_NOPROMPT_FLAG|ARG_BMFUNC_FLAG))==DIRCHANGED)
                    ret=APPLY_REAPPLY;
            }
            break;
        case BM_MARK:
	    if (func_arg->setflag)
            fh->accessed[0] |= FILE_MARKED;
	    else
            fh->accessed[0] &= ~FILE_MARKED;
            break;
        case BM_DIGEST: {
            struct fileheader data;
            data.accessed[0] = FILE_DIGEST;
            change_post_flag(arg->writearg,
                    arg->mode, currboard, 
                    fh, FILE_DIGEST, &data, true,getSession());
            }
            break;
        case BM_MARKDEL:
	    if (func_arg->setflag) {
            if (!(fh->accessed[0] & FILE_MARKED)) {
                fh->accessed[1] |= FILE_DEL;
            }
	    } else
                fh->accessed[1] &= ~FILE_DEL;
            break;
        case BM_NOREPLY:
	    if (func_arg->setflag)
                fh->accessed[1] |= FILE_READ;
	    else
                fh->accessed[1] &= ~FILE_READ;
            break;
        case BM_IMPORT:
            if (a_Import(func_arg->announce_path, 
                currboard->filename, 
                fh, 
                true, 
                arg->direct, 
                ent)==0) {
                fh->accessed[0]=FILE_IMPORTED;
            }            
            break;
        case BM_TOTAL:
        case BM_TMP:
            a_SeSave("0Announce",
                currboard->filename,
                fh,
                true,
                arg->direct,
                ent,
                !func_arg->saveorigin);
            fh->accessed[0]|=FILE_IMPORTED;
            break;
    }
    return ret;
}

static int SR_BMFunc(struct _select_def* conf, struct fileheader* fh, void* extraarg)
{
    int i;
    char buf[256], ch[4], BMch;
    struct BMFunc_arg func_arg;
    bool fromfirst;
    int ent;
    struct read_arg* arg=(struct read_arg*)conf->arg;
    char linebuffer[LINELEN*3];
    char annpath[MAXPATH];
    struct write_dir_arg dirarg;

    if (fh==NULL)
        return DONOTHING;
    func_arg.delpostnum=(bool)extraarg;
    func_arg.setflag=true;
    if (!chk_currBM(currBM, getCurrentUser())) {
        return DONOTHING;
    }
    if (arg->mode != DIR_MODE_NORMAL && arg->mode != DIR_MODE_DIGEST)     /* KCN:�ݲ����� */
        return DONOTHING;
    if (conf->pos>arg->filecount) /*�ö�*/
        return DONOTHING;
    saveline(t_lines - 3, 0, linebuffer);
    saveline(t_lines - 2, 0, NULL);
    move(t_lines - 3, 0);
    clrtoeol();
    strcpy(buf, "��ͬ���� (0)ȡ��  ");
    for (i = 0; i < item_num; i++) {
        char t[40];

        sprintf(t, "(%d)%s", i + 1, SR_BMitems[i]);
        strcat(buf, t);
    };
    strcat(buf, "? [0]: ");
    if (strlen(buf) > 76) {
        char savech = buf[76];

        buf[76] = 0;
        prints("%s", buf);
        buf[76] = savech;

/*        strcpy(buf,buf+76);*/
        getdata(t_lines - 2, 0, buf + 76, ch, 3, DOECHO, NULL, true);
    } else
        getdata(t_lines - 3, 0, buf, ch, 3, DOECHO, NULL, true);
    BMch = atoi(ch);
    if (BMch <= 0 || BMch > item_num) {
        saveline(t_lines - 2, 1, NULL);
        saveline(t_lines - 3, 1, linebuffer);
        return DONOTHING;
    }
    if (arg->mode == DIR_MODE_DIGEST && BMch == 3) {
        saveline(t_lines - 2, 1, NULL);
        saveline(t_lines - 3, 1, linebuffer);
        return DONOTHING;
    }
    move(t_lines - 2, 0);
    clrtoeol();
    move(t_lines - 3, 0);
    clrtoeol();

    /*
     * Leeward 98.04.16
     */
    switch (BMch) {
        case BM_MARK:
            if ((fh->accessed[0] & FILE_MARKED)!=0)
		    func_arg.setflag=false;
            break;
        case BM_MARKDEL:
            if ((fh->accessed[1] & FILE_DEL)!=0)
		    func_arg.setflag=false;
            break;
        case BM_NOREPLY:
            if ((fh->accessed[1] & FILE_DEL)!=0)
		    func_arg.setflag=false;
            break;
        default:
            break;
    }
    snprintf(buf, 256, "�������һƪ��ʼ%s%s (Y)��һƪ (N)Ŀǰ��ƪ (C)ȡ�� (Y/N/C)? [Y]: ",
              func_arg.setflag?"":"ȡ��",SR_BMitems[BMch - 1]);
    getdata(t_lines - 3, 0, buf, ch, 3, DOECHO, NULL, true);
    switch (ch[0]) {
    case 'c':
    case 'C':
        saveline(t_lines - 2, 1, NULL);
        saveline(t_lines - 3, 1, linebuffer);
        return DONOTHING;
    case 'N':
    case 'n':
        fromfirst=false;
        break;
    default:
        fromfirst=true;
        break;
    }
    bmlog(getCurrentUser()->userid, currboard->filename, 14, 1);

    if(BM_TOTAL == BMch ){ //���ϼ�
        sprintf(annpath,"tmp/bm.%s",getCurrentUser()->userid);
        if(dashf(annpath))unlink(annpath);
        snprintf(buf, 256, "�Ƿ�������(Y/N/C)? [Y]: ");
        getdata(t_lines - 2, 0, buf, ch, 3, DOECHO, NULL, true);
        switch (ch[0]){
        case 'n':
        case 'N':
            func_arg.saveorigin=false;
            break;
        case 'c':
        case 'C':
            saveline(t_lines - 2, 1, NULL);
            saveline(t_lines - 3, 1, linebuffer);
            return DONOTHING;
        default:
            func_arg.saveorigin=true;
        }
    } else if (BMch==BM_IMPORT) {
        if (set_import_path(annpath)!=0) {
            saveline(t_lines - 2, 1, NULL);
            saveline(t_lines - 3, 1, linebuffer);
            return DONOTHING;
        }
        func_arg.announce_path=annpath;
    }

    func_arg.action=BMch;
    ent=conf->pos;
    init_write_dir_arg(&dirarg);
    dirarg.fd=arg->fd;
    dirarg.needlock=false;
    arg->writearg=&dirarg;

    flock(arg->fd,LOCK_EX);
    if (fromfirst) {
        /*�ߵ���һƪ*/
        apply_thread(conf,fh,fileheader_thread_read,false,false,(void*)SR_FIRST);
        if (conf->new_pos!=0)
            conf->pos=conf->new_pos;
    }
    apply_thread(conf,fh,BM_thread_func,true,true,(void*)&func_arg);
    flock(arg->fd,LOCK_UN);
    free_write_dir_arg(&dirarg);
    arg->writearg=NULL;
    conf->pos=ent; /*�ָ�ԭ����ent*/
    if(BM_TOTAL == BMch){ //���ϼ�
        char title[STRLEN];
        //create new title
        strcpy(buf,"[�ϼ�] ");
        if(!strncmp(fh->title,"Re: ",4))strcpy(buf+7,fh->title + 4);
        else
            strcpy(buf+7,fh->title);
        if(strlen(buf) >= STRLEN )buf[STRLEN-1] = 0;
        strcpy(title,buf);
        //post file to the board
        if(post_file(getCurrentUser(),"",annpath,currboard->filename,title,0,2,getSession()) < 0) {//fail
            sprintf(buf,"�������µ��������!�밴������˳� << ");
            a_prompt(-1,buf,annpath); //annpath no use
            saveline(t_lines - 2, 1, NULL);
            saveline(t_lines - 3, 1, linebuffer);
        }
        unlink(annpath);
        sprintf(annpath,"tmp/se.%s",getCurrentUser()->userid);
        unlink(annpath);
        return DIRCHANGED;
    }
    return DIRCHANGED;
}

static struct key_command read_comms[] = { /*�Ķ�״̬�������� */
    {'r', (READ_KEY_FUNC)read_post,NULL},
    {'K', (READ_KEY_FUNC)skip_post,NULL},

    {'d', (READ_KEY_FUNC)del_post,(void*)0},
    {'D', (READ_KEY_FUNC)del_range,NULL},
    {Ctrl('C'), (READ_KEY_FUNC)do_cross,NULL},
    {'Y', (READ_KEY_FUNC)UndeleteArticle,NULL},     /* Leeward 98.05.18 */
    {Ctrl('P'), (READ_KEY_FUNC)do_post,NULL},
    {'E', (READ_KEY_FUNC)edit_post,NULL},
    {'T', (READ_KEY_FUNC)edit_title,NULL},
            
    {'m', (READ_KEY_FUNC)set_article_flag,(void*)FILE_MARK_FLAG},
    {';', (READ_KEY_FUNC)noreply_post,(void*)NULL},        /*Haohmaru.99.01.01,�趨����reģʽ */
    {'#', (READ_KEY_FUNC)set_article_flag,(void*)FILE_SIGN_FLAG},           /* Bigman: 2000.8.12  �趨���±��ģʽ */
#ifdef FILTER
    {'@', (READ_KEY_FUNC)set_article_flag,(void*)FILE_CENSOR_FLAG},         /* czz: 2002.9.29 ��˱��������� */
#endif
    {'g', (READ_KEY_FUNC)set_article_flag,(void*)FILE_DIGEST_FLAG},
    {'t', (READ_KEY_FUNC)set_article_flag,(void*)FILE_DELETE_FLAG},     /*KCN 2001 */
    {'|', (READ_KEY_FUNC)set_article_flag,(void*)FILE_TITLE_FLAG},

    {'G', (READ_KEY_FUNC)range_flag,(void*)FILE_TITLE_FLAG},
        
    {'H', read_callfunc0, read_hot_info},   /* flyriver: 2002.12.21 ����������Ϣ��ʾ */
        
    {Ctrl('G'), (READ_KEY_FUNC)change_mode,(void*)0},   /* bad : 2002.8.8 add marked mode */
    {'`', (READ_KEY_FUNC)change_mode,(void*)DIR_MODE_DIGEST},
    {'.', (READ_KEY_FUNC)change_mode,(void*)DIR_MODE_DELETED},
    {'>', (READ_KEY_FUNC)change_mode,(void*)DIR_MODE_JUNK},
    {Ctrl('T'), (READ_KEY_FUNC)change_mode,(void*)DIR_MODE_THREAD},

    {'s', (READ_KEY_FUNC)do_select,NULL},
    {'x', (READ_KEY_FUNC)into_announce,NULL},
    {'v', read_callfunc0,i_read_mail},         /* period 2000-11-12 read mail in article list */

    {'i', (READ_KEY_FUNC)Save_post,NULL},
    {'J', (READ_KEY_FUNC)Semi_save,NULL},
    {'I', (READ_KEY_FUNC)Import_post,NULL},

#ifdef INTERNET_EMAIL
    {'F', (READ_KEY_FUNC)mail_forward,NULL},
    {'U', (READ_KEY_FUNC)mail_uforward,NULL},
    {Ctrl('R'), (READ_KEY_FUNC)post_reply,NULL},
#endif

#ifdef NINE_BUILD
    {'c', read_callfunc0,show_t_friends},
    {'C', (READ_KEY_FUNC)clear_new_flag,NULL},
#else
    {'c', (READ_KEY_FUNC)clear_new_flag,NULL},
#endif
    {'f', (READ_KEY_FUNC)clear_all_new_flag,NULL},

    {'n',  (READ_KEY_FUNC)thread_read,(void*)SR_FIRSTNEW},
    {Ctrl('N'), (READ_KEY_FUNC)thread_read,(void*)SR_FIRSTNEW},
    {'\\', (READ_KEY_FUNC)thread_read,(void*)SR_LAST},
    {'=', (READ_KEY_FUNC)thread_read,(void*)SR_FIRST},

    {'a', (READ_KEY_FUNC)auth_search,(void*)false},
    {'A', (READ_KEY_FUNC)auth_search,(void*)true},
    {'/', (READ_KEY_FUNC)title_search,(void*)false},
    {'?', (READ_KEY_FUNC)title_search,(void*)true},
    {']', (READ_KEY_FUNC)thread_read,(void*)SR_NEXT},
    {'[', (READ_KEY_FUNC)thread_read,(void*)SR_PREV},

    {Ctrl('A'), (READ_KEY_FUNC)read_showauthor,NULL},
    {Ctrl('Q'), (READ_KEY_FUNC)read_showauthorinfo,NULL},     
    {Ctrl('W'), (READ_KEY_FUNC)read_showauthorBM,NULL}, 
    {Ctrl('O'), (READ_KEY_FUNC)read_addauthorfriend,NULL},

    {Ctrl('Y'), (READ_KEY_FUNC)read_zsend,NULL},
#ifdef PERSONAL_CORP
    {'y', (READ_KEY_FUNC)read_importpc,NULL},
#endif

    {'\'',(READ_KEY_FUNC)post_search,(void*)false},
    {'\"', (READ_KEY_FUNC)post_search,(void*)true},

    {'R',  (READ_KEY_FUNC)b_results,NULL},
    {'V',  (READ_KEY_FUNC)b_vote,NULL},
    {'M',  (READ_KEY_FUNC)b_vote_maintain,NULL},
    {'W',  (READ_KEY_FUNC)b_note_edit_new,NULL},
    {'h',  (READ_KEY_FUNC)mainreadhelp,NULL},
    {'X',  (READ_KEY_FUNC)b_jury_edit,NULL},     //�༭������ٲ�ίԱ����,stephen on 2001.11.1 
    {KEY_TAB,  (READ_KEY_FUNC)show_b_note,NULL},
    {Ctrl('D'), (READ_KEY_FUNC)deny_user,NULL},
    {Ctrl('E'), (READ_KEY_FUNC)clubmember,NULL},
#ifdef NINE_BUILD
    {'z',  (READ_KEY_FUNC)show_sec_b_note,NULL},
    {'Z',  (READ_KEY_FUNC)b_sec_notes_edit,NULL},
#else
    {'z', (READ_KEY_FUNC)read_sendmsgtoauthor,NULL},
    {'Z', (READ_KEY_FUNC)read_sendmsgtoauthor,NULL},
#endif

    {'p',  (READ_KEY_FUNC)thread_read,(void*)SR_READ},
    {Ctrl('S'), (READ_KEY_FUNC)thread_read,(void*)SR_READ},
    {Ctrl('X'), (READ_KEY_FUNC)thread_read,(void*)SR_READX},
    
    {Ctrl('U'), (READ_KEY_FUNC)author_read,(void*)SR_READ},
    {Ctrl('H'), (READ_KEY_FUNC)author_read,(void*)SR_READX}, 
    /*----------------------------------------*/
    {'S', (READ_KEY_FUNC)sequential_read,NULL},
    
    {'b', (READ_KEY_FUNC)SR_BMFunc,(void*)true},
    {'B', (READ_KEY_FUNC)SR_BMFunc,(void*)false},
    {',', (READ_KEY_FUNC)read_splitscreen,NULL},
    {'!', (READ_KEY_FUNC)Goodbye,NULL},
    {'\0', NULL},
};

int Read()
{
    char buf[2 * STRLEN];
    char notename[STRLEN];
    time_t usetime;
    struct stat st;
    int bid;
    int returnmode=CHANGEMODE;
#ifdef NEW_HELP
    int oldhelpmode;
#endif

    if (!selboard||!currboard) {
        move(2, 0);
        prints("����ѡ��������\n");
        pressreturn();
        move(2, 0);
        clrtoeol();
        return -1;
    }
    in_mail = false;
    bid = getbnum(currboard->filename);

    currboardent=bid;
    currboard=(struct boardheader*)getboard(bid);

    if (currboard->flag&BOARD_GROUP) return -2;
#ifdef HAVE_BRC_CONTROL
    brc_initial(getCurrentUser()->userid, currboard->filename,getSession());
#endif

    setbdir(DIR_MODE_NORMAL, buf, currboard->filename);

    board_setcurrentuser(uinfo.currentboard, -1);
    uinfo.currentboard = currboardent;;
    UPDATE_UTMP(currentboard,uinfo);
    board_setcurrentuser(uinfo.currentboard, 1);
    
    setvfile(notename, currboard->filename, "notes");
    if (stat(notename, &st) != -1) {
        if (st.st_mtime < (time(NULL) - 7 * 86400)) {
/*            sprintf(genbuf,"touch %s",notename);
	    */
            f_touch(notename);
            setvfile(genbuf, currboard->filename, "noterec");
            my_unlink(genbuf);
        }
    }
    if (vote_flag(currboard->filename, '\0', 1 /*�������µı���¼û */ ) == 0) {
        if (dashf(notename)) {
            /*
             * period  2000-09-15  disable ActiveBoard while reading notes 
             */
            modify_user_mode(READING);
            /*-	-*/
            ansimore(notename, true);
            vote_flag(currboard->filename, 'R', 1 /*д������µı���¼ */ );
        }
    }
    usetime = time(0);
#ifdef NEW_HELP
    oldhelpmode = helpmode;
    helpmode = HELP_ARTICLE;
#endif
    while ((returnmode==CHANGEMODE)&&!(currboard->flag&BOARD_GROUP)) {
    returnmode=new_i_read(DIR_MODE_NORMAL, buf, readtitle, (READ_ENT_FUNC) readdoent, &read_comms[0], sizeof(struct fileheader));  /*���뱾�� */
    setbdir(DIR_MODE_NORMAL, buf, currboard->filename);
    }
#ifdef NEW_HELP
    helpmode = oldhelpmode;
#endif
    newbbslog(BBSLOG_BOARDUSAGE, "%-20s Stay: %5ld", currboard->filename, time(0) - usetime);
    bmlog(getCurrentUser()->userid, currboard->filename, 0, time(0) - usetime);
    bmlog(getCurrentUser()->userid, currboard->filename, 1, 1);

    board_setcurrentuser(uinfo.currentboard, -1);
    uinfo.currentboard = 0;
    UPDATE_UTMP(currentboard,uinfo);
    return returnmode;
}


