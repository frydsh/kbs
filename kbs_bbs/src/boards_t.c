/*
    Pirate Bulletin Board System
    Copyright (C) 1990, Edward Luke, lush@Athena.EE.MsState.EDU
    Eagles Bulletin Board System
    Copyright (C) 1992, Raymond Rocker, rocker@rock.b11.ingr.com
                        Guy Vega, gtvega@seabass.st.usm.edu
                        Dominic Tynes, dbtynes@seabass.st.usm.edu
    Copyright (C) 2002, Zhou Lin, kcn@cic.tsinghua.edu.cn

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

extern time_t   login_start_time;
extern int     *zapbuf;
extern int	zapbuf_changed;
extern int     brdnum;
extern int yank_flag;
extern char    *boardprefix;
extern int     favbrd_list[FAVBOARDNUM+1];

void
EGroup( cmd )
char *cmd;
{
    char        buf[ STRLEN ];

    sprintf( buf, "EGROUP%c", *cmd );
    boardprefix = sysconf_str( buf );
    choose_board( DEFINE(currentuser,DEF_NEWPOST)?1:0 );
}

void
Boards()
{
    boardprefix = NULL;
    choose_board( 0 );
}

void
New()
{
    boardprefix = NULL;
    choose_board( 1 );
}

int
cmpboard( brd, tmp ) /*������*/
struct newpostdata      *brd, *tmp;
{
    register int        type = 0;

    if( !(currentuser->flags[0] & BRDSORT_FLAG) )
    {
        type = brd->title[0] - tmp->title[0];
        if(type==0)
            type=strncasecmp( brd->title+1, tmp->title+1,6);

    }
    if( type == 0 )
        type = strcasecmp( brd->name, tmp->name );
    return type;
}


int
unread_position( dirfile, ptr )
char    *dirfile;
struct newpostdata *ptr;
{
    struct fileheader fh ;
    char        filename[ STRLEN ];
    int         fd, offset, step, num;

    num = ptr->total + 1;
    if( ptr->unread && (fd = open( dirfile, O_RDWR )) > 0 ) {
        if( !brc_initial(currentuser->userid, ptr->name ) ) {
            num = 1;
        } else {
            offset = (int)((char *)&(fh.filename[0]) - (char *)&(fh));
            num = ptr->total - 1;
            step = 4;
            while( num > 0 ) {
                lseek( fd, offset + num * sizeof(fh), SEEK_SET );
                if( read( fd, filename, STRLEN ) <= 0 ||
                        !brc_unread( filename ) )  break;
                num -= step;
                if( step < 32 )  step += step / 2;
            }
            if( num < 0 )  num = 0;
            while( num < ptr->total ) {
                lseek( fd, offset + num * sizeof(fh), SEEK_SET );
                if( read( fd, filename, STRLEN ) <= 0 ||
                        brc_unread( filename ) )  break;
                num ++;
            }
        }
        close( fd );
    }
    if( num < 0 )  num = 0;
    return num;
}


int
search_board( int *num,int *i,int *find,char* bname)
{
    int         n,ch,tmpn=NA;

    if (*find == YEA)
    {
        bzero(bname, STRLEN);
        *find = NA;
        *i = 0;
    }
    while (1)
    {
        move(t_lines-1, 0);
        clrtoeol();
        prints("������Ҫ��Ѱ�� board ���ƣ�%s",bname);
        ch = egetch();

        if (isprint2(ch))
        {
            bname[(*i)++] = ch;
            for (n = 0; n < brdnum; n++)
            {
                if (!strncasecmp(nbrd[n].name, bname, *i))
                {
                    tmpn=YEA;
                    *num = n;
                    if(!strcmp(nbrd[n].name, bname))
                        return 1/*�ҵ����Ƶİ壬�����ػ�*/;
                }
            }
            if(tmpn)
                return 1;
            if (*find == NA)
            {
                bname[--(*i)] = '\0';
            }
            continue;
        }
        else if (ch == Ctrl('H') || ch == KEY_LEFT || ch == KEY_DEL ||
                 ch == '\177')
        {
            (*i)--;
            if (*i < 0)
            {
                *find = YEA;
                break;
            }
            else
            {
                bname[*i] = '\0';
                continue;
            }
        }
        else if (ch == '\t')
        {
            *find = YEA;
            break;
        }
        else if (Ctrl('Z') == ch)
        {
            r_lastmsg(); /* Leeward 98.07.30 support msgX */
            break;
        }
        else if (ch == '\n' || ch == '\r' || ch == KEY_RIGHT)
        {
            *find = YEA;
            break;
        }
        bell(1);
    }
    if (*find)
    {
        move(t_lines-1, 0);
        clrtoeol();
        return 2/*������*/;
    }
    return 1;
}

int
show_authorBM(ent,fileinfo,direct)
int ent ;
struct fileheader *fileinfo ;
char *direct ;
{
    struct boardheader *bptr;
    int         tuid=0;
    int         n;

    if(!HAS_PERM(currentuser, PERM_ACCOUNTS )||!strcmp(fileinfo->owner,"Anonymous")||!strcmp(fileinfo->owner,"deliver"))
        return DONOTHING;
    else
    {
    	struct userec* lookupuser;
        if( !( tuid=getuser(fileinfo->owner,&lookupuser) ) ) {
            clrtobot();
            prints("����ȷ��ʹ���ߴ���\n") ;
            pressanykey() ;
            move(2,0) ;
            clrtobot() ;
            return FULLUPDATE ;
        }

        move( 3, 0 );
        if( !(lookupuser->userlevel & PERM_BOARDS)){
            clrtobot();
            prints("�û�%s���ǰ���!\n",lookupuser->userid);
            pressanykey() ;
            move(2,0) ;
            clrtobot() ;
            return FULLUPDATE ;
        }
        clrtobot();
        prints("�û�%sΪ���°�İ���\n\n",lookupuser->userid);

        prints("�����������������������������������ש���������������������������������\n");
        prints("��            ��Ӣ����            ��            ��������            ��\n");

        for( n = 0; n < get_boardcount(); n++ ) {
            bptr = (struct boardheader*) getboard(n+1);
            if( chk_BM_instr(bptr->BM,lookupuser->userid) == YEA){
                prints("�ǩ��������������������������������贈��������������������������������\n");
                prints("��%-32s��%-32s��\n",bptr->filename,bptr->title+12);
            }
        }
        prints("�����������������������������������ߩ���������������������������������\n");
        pressanykey() ;
        move(2,0) ;
        clrtobot() ;
        return FULLUPDATE;
    }
}

/* inserted by cityhunter to query BM */
int
query_bm( )
{
    const struct boardheader *bptr;
    int         n;
    char        tmpBM[BM_LEN-1];
    char        uident[STRLEN];
    int         tuid=0;
    struct userec* lookupuser;

    modify_user_mode(QUERY);
    move(2,0);
    clrtobot();
    prints("<����ʹ���ߴ���, ���հ׼����г������ִ�>\n");
    move(1,0);
    clrtoeol();
    prints("��ѯ˭: ");
    usercomplete(NULL,uident);
    if(uident[0] == '\0') {
        clear() ;
        return FULLUPDATE ;
    }
    if(!(tuid = getuser(uident,&lookupuser))) {
        move(2,0) ;
        clrtoeol();
        prints("[1m����ȷ��ʹ���ߴ���[m\n") ;
        pressanykey() ;
        move(2,0) ;
        clrtoeol() ;
        return FULLUPDATE ;
    }

    move( 3, 0 );
    if( !(lookupuser->userlevel & PERM_BOARDS))
    {
        prints("�û�%s���ǰ���!\n",lookupuser->userid);
        pressanykey() ;
        move(2,0) ;
        clrtoeol() ;
        return FULLUPDATE ;
    }
    prints("�û�%sΪ���°�İ���\n\n",lookupuser->userid);

    prints("�����������������������������������ש���������������������������������\n");
    prints("��            ��Ӣ����            ��            ��������            ��\n");

    for( n = 0; n < get_boardcount(); n++ ) {
        bptr = getboard(n+1);
        if( chk_BM_instr(bptr->BM,lookupuser->userid) == YEA)
        {
            prints("�ǩ��������������������������������贈��������������������������������\n");
            prints("��%-32s��%-32s��\n",bptr->filename,bptr->title+12);
        }
    }
    prints("�����������������������������������ߩ���������������������������������\n");
    pressanykey() ;
    move(2,0) ;
    clrtoeol() ;
    return FULLUPDATE;
}
/* end of insertion */

void
show_brdlist( page, clsflag, newflag )  /* show board list */
int     page, clsflag, newflag;
{
    struct newpostdata  *ptr;
    int         n;
    char        tmpBM[BM_LEN-1];
    char        buf[STRLEN]; /* Leeward 98.03.28 */

    if( clsflag )
    {
        clear();
        docmdtitle( "[�������б�]", "  [m��ѡ��[��,e] �Ķ�[��,r] ѡ��[��,��] �г�[y] ����[s] ��Ѱ[/] �л�[c] ����[h]\n" );
        prints( "[44m[37m %s ����������       V  ��� ת��  %-24s ��  ��   %s   [m\n",
                newflag ? "ȫ�� δ��" : "���  ", "��  ��  ��  ��" ,newflag ? "":"   ");
    }

    move( 3, 0 );
    for( n = page; n < page + BBS_PAGESIZE; n++ ) {
        if( n >= brdnum ) {
            prints( "\n" );
            continue;
        }
        ptr = &nbrd[n];
        if( !newflag )
            prints( " %4d %c", n+1, ptr->zap&&!(ptr->flag&NOZAP_FLAG) ? '-' : ' ' );/*zap��־*/
        else if( ptr->zap&&!(ptr->flag&NOZAP_FLAG) ) {
            /*ptr->total = ptr->unread = 0;
            prints( "    -    -" );*/
            /* Leeward: 97.12.15: extended display */
            check_newpost( ptr );
            prints( " %4d%s%s ", ptr->total, ptr->total > 9999 ? " " : "  ", ptr->unread ? "��" : "��" );/*�Ƿ�δ��*/
        } else {
            if( ptr->total == -1 ) {
                refresh();
                check_newpost( ptr );
            }
            prints( " %4d%s%s ", ptr->total, ptr->total > 9999 ? " " : "  ", ptr->unread ? "��" : "��" );/*�Ƿ�δ��*/
        }
        strcpy(tmpBM,ptr->BM);

        /* Leeward 98.03.28 Displaying whether a board is READONLY or not */
        if (YEA == checkreadonly(ptr->name))
            sprintf(buf, "��ֻ����%s", ptr->title + 8);
        else
            sprintf(buf, " %s", ptr->title + 1);
        prints("%c%-16s %s%-36s %-12s\n", ((newflag && ptr->zap && !(ptr->flag&NOZAP_FLAG)) ? '*' : ' '), ptr->name,(ptr->flag&VOTE_FLAG) ? "[31mV[m" : " ", buf, ptr->BM[0] <= ' ' ? "����������" : strtok(tmpBM," ")); /*��һ������*/
    }
    refresh();
}


int
choose_board( newflag ) /* ѡ�� �棬 readnew��readboard */
int     newflag;
{
    static int  num;
    struct newpostdata newpost_buffer[ MAXBOARD ];
    struct newpostdata *ptr;
    int         page, ch, tmp, number,tmpnum;
    int         loop_mode=0;
    int     i = 0,find = YEA;
    char    bname[STRLEN];

    if( !strcmp( currentuser->userid, "guest" ) )
        yank_flag = 1;
    nbrd = newpost_buffer;
    modify_user_mode( newflag ? READNEW : READBRD );
    brdnum = number = 0;
    /* show_brdlist( 0, 1, newflag );*/ /*board list��ʾ �� 2����ʾ������! 96.9.5 alex*/
    while( 1 ) {
        if( brdnum <= 0 ) { /*��ʼ��*/
            if(load_boards()==-1)
                continue;
            qsort( nbrd, brdnum, sizeof( nbrd[0] ), (int (*)(const void *, const void *))cmpboard );
            page = -1;
            if( brdnum <= 0 )  break;
        }
        if( num < 0 )  num = 0;
        if( num >= brdnum )  num = brdnum - 1;
        if( page < 0 ) {
            if( newflag ) {/* �����readnew�Ļ�����������һ��δ����*/
                tmp = num;
                while( num < brdnum ) {
                    ptr = &nbrd[ num ];
                    if( ptr->total == -1 ) check_newpost( ptr );
                    if( ptr->unread )  break;
                    num++;
                }
                if( num >= brdnum )  num = tmp;
            }
            page = (num / BBS_PAGESIZE) * BBS_PAGESIZE; /*page����*/
            show_brdlist( page, 1, newflag );
            update_endline();
        }
        if( num < page || num >= page + BBS_PAGESIZE ) {
            page = (num / BBS_PAGESIZE) * BBS_PAGESIZE;
            show_brdlist( page, 0, newflag );
            update_endline();
        }
        move( 3+num-page,0 ); prints( ">", number ); /*��ʾ��ǰboard��־*/
        if(loop_mode==0)
        {
            ch = egetch();
        }
        move( 3+num-page,0 ); prints( " " );
        if( ch == 'q' || ch == 'e' || ch == KEY_LEFT || ch == EOF )
            break;
        switch( ch ) {
        case Ctrl('Z'): r_lastmsg(); /* Leeward 98.07.30 support msgX */
            break;
        case 'R':  /* Leeward 98.04.24 */
            {
                char fname[STRLEN], restore[256];

                if(!strcmp(currentuser->userid,"guest")) /* guest ���� */
                    break;

                saveline(t_lines-2, 0, NULL);
                move(t_lines-2, 0);
                clrtoeol();
                getdata(t_lines-2, 0,"[1m[5m[31m��������[m��[1m[33m�Ա�ָ��ϴ������뿪��վʱ��δ����� (Y/N)�� [N][m: ", restore,4,DOECHO,NULL,YEA);
                if ('y' == restore[0] || 'Y' == restore[0])
                {
                    sethomefile(fname, currentuser->userid,".boardrc" );
		    /*
                    sprintf(restore, "/bin/cp %s.bak %s", fname, fname);
		    */
                    sprintf(restore, "%s.bak", fname);
                    f_cp(restore,fname,0);

                    move(t_lines-2, 0);
                    clrtoeol();
                    prints("[1m[33m�ѻָ��ϴ������뿪��վʱ��δ�����[m");
                    move(t_lines-1, 0);
                    clrtoeol();
                    getdata(t_lines-1, 0,"[1m[32m�밴 Enter ���ߣ�Ȼ�����µ�¼ 8-) [m", restore,1,DOECHO,NULL,YEA);
                    abort_bbs();
                }
                saveline(t_lines-2, 1, NULL);
                break;
            }
        case 'X': /* Leeward 98.03.28 Set a board READONLY */
            {
                char buf[STRLEN];

                if (!HAS_PERM(currentuser,PERM_SYSOP) && !HAS_PERM(currentuser,PERM_OBOARDS)) break;
                if (!strcmp(nbrd[num].name, "syssecurity")
                        ||!strcmp(nbrd[num].name, "Filter")
                        ||!strcmp(nbrd[num].name, "junk")
                        ||!strcmp(nbrd[num].name, "deleted"))
                    break; /* Leeward 98.04.01 */

	 	if( strlen(nbrd[num].name) ){
	               	sprintf(buf, "chmod 555 boards/%s", nbrd[num].name);
                	system(buf);
			board_setreadonly(nbrd[num].name,1);

                	/* Bigman 2000.12.11:ϵͳ��¼ */
                	sprintf(genbuf,"ֻ�������� %s ",nbrd[num].name);
                	securityreport(genbuf,NULL);
                	sprintf(genbuf, " readonly board %s",nbrd[num].name);
                	report(genbuf);

                	show_brdlist(page, 0, newflag);
		}
                break;
            }
        case 'Y': /* Leeward 98.03.28 Set a board READABLE */
            {
                char buf[STRLEN];

                if (!HAS_PERM(currentuser,PERM_SYSOP) && !HAS_PERM(currentuser,PERM_OBOARDS)) break;

                sprintf(buf, "chmod 755 boards/%s", nbrd[num].name);
                system(buf);
		board_setreadonly(nbrd[num].name,0);

                /* Bigman 2000.12.11:ϵͳ��¼ */
                sprintf(genbuf,"�⿪ֻ�������� %s ",nbrd[num].name);
                securityreport(genbuf,NULL);
                sprintf(genbuf, " readable board %s",nbrd[num].name);
                report(genbuf);

                show_brdlist(page, 0, newflag);
                break;
            }
case 'L':case 'l':  /* Luzi 1997.10.31 */
            if(uinfo.mode!=LOOKMSGS)
            {
                show_allmsgs();
                page = -1;
                break;
            }
            else
                return DONOTHING;
        case 'H':          /* Luzi 1997.10.31 */
            r_lastmsg();
            break;
case 'W':case 'w':       /* Luzi 1997.10.31 */
            if (!HAS_PERM(currentuser,PERM_PAGE)) break;
            s_msg();
            page=-1;
            break;
        case 'u':		/*Haohmaru.99.11.29*/
            {	int oldmode = uinfo.mode;
                clear();
                modify_user_mode(QUERY);
                t_query();
                page=-1;
                modify_user_mode(oldmode);
                break;
            }
        case '!':
            Goodbye();
            page=-1;
            break;
case 'O':case 'o':       /* Luzi 1997.10.31 */
            { /* Leeward 98.10.26 fix a bug by saving old mode */
                int  savemode = uinfo.mode;
                if (!HAS_PERM(currentuser,PERM_BASIC)) break;
                t_friends();
                page=-1;
                modify_user_mode(savemode);
                /* return FULLUPDATE;*/
                break;
            }
case 'P': case 'b': case Ctrl('B'): case KEY_PGUP:
            if( num == 0 )  num = brdnum - 1;
            else  num -= BBS_PAGESIZE;
            break;
case 'C':case 'c': /*�Ķ�ģʽ*/
            if(newflag==1)
                newflag=0;
            else
                newflag=1;
            show_brdlist( page, 1, newflag );
            break;
case 'N': case ' ': case Ctrl('F'): case KEY_PGDN:
            if( num == brdnum - 1 )  num = 0;
            else  num += BBS_PAGESIZE;
            break;
case 'p': case 'k': case KEY_UP:
            if( num-- <= 0 )  num = brdnum - 1;
            break;
case 'n': case 'j': case KEY_DOWN:
            if( ++num >= brdnum )  num = 0;
            break;
        case '$':
            num = brdnum - 1;       break;
        case 'h':
            show_help("help/boardreadhelp");
            page = -1;
            break;
        case '/': /*����board */ 
	            move( 3+num-page,0 ); prints( ">", number );
	            tmpnum=num;
	            tmp = search_board( &num,&i,&find,bname );
	            move( 3+tmpnum-page,0 ); prints( " ", number );
	            if(tmp==1)
	                loop_mode=1;
	            else
	            {
			find=YEA;
			i=0;
	                loop_mode=0;
	                update_endline();
	            }
            break;
        case 's':   /* sort/unsort -mfchen */
            currentuser->flags[0] ^= BRDSORT_FLAG; /*����ʽ*/
            qsort( nbrd, brdnum, sizeof( nbrd[0] ), (int (*)(const void *, const void *))cmpboard );/*����*/
            page = 999;
            break;
            /*---	added period 2000-09-11	4 FavBoard	---*/
        case 'a':
            if(2 == yank_flag) {
                char bname[STRLEN];
                int i = 0;
                if(*favbrd_list >= FAVBOARDNUM) {
                    move(2, 0);
                    clrtoeol();
                    prints("�������Ű����Ѿ�������(%d)��", FAVBOARDNUM);
                    pressreturn();
                    show_brdlist( page, 1, newflag );  /*  refresh screen */
                    break;
                }
                move(0,0) ;
                clrtoeol();
                prints("����������Ӣ���� (��Сд�Կɣ����հ׼��Զ���Ѱ): ") ;
                clrtoeol() ;

                make_blist() ;
                namecomplete((char *)NULL,bname) ;
                CreateNameList() ;             /*  free list memory. */
                if(*bname) i = getbnum(bname);
                if( i > 0 && !IsFavBoard(i-1) ) {
                    int llen;
                    llen = ++(*favbrd_list);
                    favbrd_list[llen] = i-1;
                    save_favboard();
                    brdnum = -1;    /*  force refresh board list */
                } else {
                    move(2,0);
                    prints("����ȷ��������.\n");
                    pressreturn();
                    show_brdlist( page, 1, newflag );  /*  refresh screen */
                }
            }
            break;
        case 'd':
            if(2 == yank_flag) {
                DelFavBoard( IsFavBoard(nbrd[num].pos) );
                save_favboard();
                brdnum = -1;    /*  force refresh board list. */
            }
            break;
            /*---	End of Addition	---*/
        case 'y':
            if(yank_flag < 2) { /*--- Modified 4 FavBoard 2000-09-11	---*/
                yank_flag = !yank_flag;
                brdnum = -1;
            }
            break;
        case 'z': /* Zap*/
            if(yank_flag < 2) { /*--- Modified 4 FavBoard 2000-09-11	---*/
                if( HAS_PERM(currentuser, PERM_BASIC )&&!(nbrd[num].flag&NOZAP_FLAG)) {
                    ptr = &nbrd[num];
                    ptr->zap = !ptr->zap;
                    ptr->total = -1;
                    zapbuf[ ptr->pos ] = (ptr->zap ? 0 : login_start_time);
		    zapbuf_changed=1;
                    page = 999;
                }
            }
            break;
        case KEY_HOME:
            num=0;
            break;
        case KEY_END:
            num = brdnum - 1;
            break;
    case '\n': case '\r': /*ֱ���������� ��ת*/
            if( number > 0 ) {
                num = number - 1;
                break;
            }
            /* fall through */
    case 'r': case KEY_RIGHT: /*���� board*/
            {
                char    buf[ STRLEN ];

                ptr = &nbrd[num];
                brc_initial( currentuser->userid,ptr->name );
                memcpy( currBM, ptr->BM, BM_LEN -1);
                if( DEFINE(currentuser,DEF_FIRSTNEW) ) {
                    setbdir(digestmode, buf, currboard );
                    tmp = unread_position( buf, ptr );
                    page = tmp - t_lines / 2;
                    getkeep( buf, page > 1 ? page : 1, tmp + 1 );
                }
                Read();

                /* Leeward 98.05.17: updating unread flag on exiting Read() */
                /* if (-1 != load_boards())
                   qsort( nbrd, brdnum, sizeof( nbrd[0] ), cmpboard ); */

				/* �����zap�����ʱ�䶨��Ϊ�ϴ��Ķ���ʱ�䣬����û��ʹ��
                if( zapbuf[ ptr->pos ] > 0 ) 
                    zapbuf[ ptr->pos ] = brc_list[0];
		    	*/
                ptr->total = page = -1;
                modify_user_mode( newflag ? READNEW : READBRD );
                break;
            }
        case 'v':		/*Haohmaru.2000.4.26*/
            clear();
            m_read();
            show_brdlist( page, 1, newflag );
            break;
        default:
            ;
        }
        if( ch >= '0' && ch <= '9' ) {
            number = number * 10 + (ch - '0');
            ch = '\0';
        } else {
            number = 0;
        }
    }
    clear();
    save_zapbuf();
    return -1 ;
}

int
check_newpost( ptr )
struct newpostdata *ptr;
{
	struct BoardStatus* bptr;
	ptr->total = ptr->unread = 0;

	bptr = getbstatus(ptr->pos);
	if (bptr == NULL)
		return 0;
	ptr->total = bptr->total;

	if (!brc_initial(currentuser->userid,ptr->name)) {
		ptr->unread = 1;
	} else {
		if (brc_unread_t(bptr->lastpost)) {
			ptr->unread = 1;
		}
	}
	return 1;
}

/*---   Added by period 2000-09-11      Favorate Board List     ---*
 *---   use yank_flag=2 to reflect status                       ---*
 *---   corresponding code added: comm_lists.c                  ---*
 *---           add entry in array sysconf_cmdlist[]            ---*/
void FavBoard()
{
    int ifnew = 1, yanksav;
    /*    if(heavyload()) ifnew = 0; */ /* no heavyload() in FB2.6x */
    yanksav = yank_flag;
    yank_flag = 2;
    boardprefix = NULL;
    if(!*favbrd_list) load_favboard(1);
    choose_board(ifnew);
    yank_flag = yanksav;
}

