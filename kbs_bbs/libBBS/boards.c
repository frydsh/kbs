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

#define BRC_MAXNUM      50
#define BRC_STRLEN      8
#define BRC_ITEMSIZE    (BRC_STRLEN  + BRC_MAXNUM * sizeof( int ))
#define BRC_MAXBOARD	200  /*��󱣴���ٸ����δ�����*/
#define BRC_FILESIZE BRC_ITEMSIZE*BRC_MAXBOARD
/* added period 2000-09-11	4 FavBoard */
#define FAVBOARDNUM     20

#define BRC_CACHE_NUM 20 /* δ�����cache 20����*/

static struct brc_cache_entry {
	int id;
	int list[ BRC_MAXNUM ];
	int changed;
} brc_cache_entry[BRC_CACHE_NUM];

struct brc_struct {
	int bid;
	char boardname[BRC_STRLEN];
	int list[BRC_MAXNUM];
};

/* added period 2000-09-11	4 FavBoard */
int     favbrd_list[FAVBOARDNUM+1];

struct newpostdata *nbrd; /*ÿ�������Ϣ*/
int     *zapbuf;
int	zapbuf_changed=0;
int     brdnum, yank_flag = 0;
char    *boardprefix;

void load_favboard(int dohelp)
{
    char fname[STRLEN];
    int  fd, size, idx;
    sethomefile(fname,currentuser->userid, "favboard");
    if( (fd = open( fname, O_RDONLY, 0600 )) != -1 ) {
        size = (FAVBOARDNUM+1) * sizeof( int );
        read( fd, favbrd_list, size );
        close( fd );
    }
#ifdef BBSMAIN
    else if(dohelp) {
        int savmode;
        savmode = uinfo.mode;
        modify_user_mode(CSIE_ANNOUNCE);	/* û���ʵ�mode.������"��ȡ����"��. */
        show_help("help/favboardhelp");
        modify_user_mode(savmode);
    }
#endif
    if(*favbrd_list<= 0) {
        *favbrd_list = 1;       /*  favorate board count    */
        *(favbrd_list+1) = 0;   /*  default sysop board     */
    }
    else {
        int num = *favbrd_list;
        if(*favbrd_list > FAVBOARDNUM)	/*	maybe file corrupted	*/
            *favbrd_list = FAVBOARDNUM;
        idx = 0;
        while(++idx <= *favbrd_list) {
        	struct boardheader* bh;
            fd = favbrd_list[idx];
            bh = (struct boardheader*) getboard(fd+1);
            if(fd >= 0 && fd <= get_boardcount() && (
            			bh &&
                        bh->filename[0]
                        && ( (bh->level & PERM_POSTMASK)
                             || HAS_PERM(currentuser,bh->level)
                             || (bh->level&PERM_NOZAP) )
                    )
              )
                continue;
            DelFavBoard(idx);   /*  error correction    */
        }
        if(num != *favbrd_list) save_favboard();
    }
}

void save_favboard()
{
    save_userfile("favboard", (FAVBOARDNUM+1), (char *)favbrd_list);
}

int IsFavBoard(int idx)
{
    int i;
    for(i=1;i<=*favbrd_list;i++) if(idx == favbrd_list[i]) return i;
    return 0;
}

int DelFavBoard(int i)
{
    int lnum;
    if(i > *favbrd_list) return *favbrd_list;
    lnum = --(*favbrd_list);
    for(;i<=lnum;i++) favbrd_list[i] = favbrd_list[i+1];
    if(!lnum) {
        *favbrd_list = 1;       /*  favorate board count    */
        *(favbrd_list+1) = 0;   /*  default sysop board     */
    }
    return 0;
}
/*---   ---*/
void
load_zapbuf()  /* װ��zap��Ϣ*/
{
    char        fname[ STRLEN ];
    int         fd, size, n;

    size = MAXBOARD * sizeof( int );
    zapbuf = (int *) malloc( size );
    for( n = 0; n < MAXBOARD; n++ )
        zapbuf[n] = 1;
    sethomefile( fname,currentuser->userid, ".lastread" ); /*user��.lastread�� zap��Ϣ*/
    if( (fd = open( fname, O_RDONLY, 0600 )) != -1 ) {
        size = get_boardcount() * sizeof( int );
        read( fd, zapbuf, size );
        close( fd );
    }
    zapbuf_changed=0;
}
/*---	Modified for FavBoard functions, by period	2000-09-11 */
void save_userfile(char * fname, int numblk, char * buf)
{
    char        fbuf[ 256 ];
    int         fd, size;

    sethomefile( fbuf, currentuser->userid,fname );
    if( (fd = open( fbuf, O_WRONLY | O_CREAT, 0600 )) != -1 ) {
        size = numblk * sizeof( int );
        write( fd, buf, size );
        close( fd );
    }
}

void save_zapbuf()
{
    if (zapbuf_changed!=0)
    	save_userfile(".lastread", get_boardcount(), (char *)zapbuf);
}

#if 0
void
save_zapbuf() /*����Zap��Ϣ*/
{
    char        fname[ STRLEN ];
    int         fd, size;

    sethomefile( fname, currentuser->userid,".lastread" );
    if( (fd = open( fname, O_WRONLY | O_CREAT, 0600 )) != -1 ) {
        size = numboards * sizeof( int );
        write( fd, zapbuf, size );
        close( fd );
    }
}
#endif

int
load_boards()
{
    struct boardheader  *bptr;
    struct newpostdata  *ptr;
    int         n;

    if( zapbuf == NULL ) {
        load_zapbuf();
    }
    brdnum = 0;
    for( n = 0; n < get_boardcount(); n++ ) {
    	bptr = (struct boardheader*)getboard(n+1);
    	if (!bptr) continue;
#ifndef _DEBUG_
        if(!*bptr->filename) continue;
#endif /*_DEBUG_*/
        if( !(bptr->level & PERM_POSTMASK) && !HAS_PERM(currentuser,bptr->level) && !(bptr->level&PERM_NOZAP))
        {
            continue;
        }
        if( boardprefix != NULL &&
                strchr( boardprefix, bptr->title[0]) == NULL&&boardprefix[0]!='*')
            continue;
        if(boardprefix != NULL&&boardprefix[0]=='*')
        {
            if(!strstr(bptr->title,"��")&&!strstr(bptr->title,"��")
                    && bptr->title[0]!='*')
                continue;
        }
        if(boardprefix == NULL && bptr->title[0]=='*')
            continue;
        /*---	period	2000-09-11	4 FavBoard	---*/
        if( ( 1 == yank_flag || (!yank_flag && (zapbuf[ n ] != 0||(bptr->level&PERM_NOZAP) )) )
                || (2 == yank_flag && IsFavBoard(n)) ) {
            ptr = &nbrd[ brdnum++ ];
            ptr->name  = bptr->filename;
            ptr->title = bptr->title;
            ptr->BM    = bptr->BM;
            ptr->flag  = bptr->flag|((bptr->level&PERM_NOZAP)?NOZAP_FLAG:0);
            ptr->pos = n;
            ptr->total = -1;
            ptr->zap = (zapbuf[ n ] == 0);
        }
    }
    if(brdnum==0&&!yank_flag&&boardprefix == NULL)
    {
        brdnum=-1;
        yank_flag=1;
        return -1;
    }
    return 0;
}

void brc_update(char *userid, char *board) {
        char    dirfile[STRLEN], *ptr;
        char    tmp_buf[BRC_MAXSIZE - BRC_ITEMSIZE], *tmp;
        char    tmp_name[BRC_STRLEN];
        int     tmp_list[BRC_MAXNUM], tmp_num;
        int     fd, tmp_size;
	if (brc_changed == 0) {
		return;
	}

        ptr = brc_buf;
        if (brc_num > 0) {
                ptr = brc_putrecord(ptr, brc_name, brc_num, brc_list);
        }
        if (1) {
                sethomefile(dirfile, userid, ".boardrc");
                if ((fd = open(dirfile, O_RDONLY)) != -1) {
                        tmp_size = read(fd, tmp_buf, sizeof(tmp_buf));
                        close(fd);
                } else {
                        tmp_size = 0;
                }
        }
        tmp = tmp_buf;
        while (tmp < &tmp_buf[tmp_size] && (*tmp >= ' ' && *tmp <= 'z')) {
                tmp = brc_getrecord(tmp, tmp_name, &tmp_num, tmp_list);
                if (strncmp(tmp_name, board, BRC_STRLEN) != 0) {
                        ptr = brc_putrecord(ptr, tmp_name, tmp_num, tmp_list);
                }
        }
        brc_size = (int) (ptr - brc_buf);
        if ((fd = open(dirfile, O_WRONLY | O_CREAT, 0644)) != -1) {
                ftruncate(fd, 0);
                write(fd, brc_buf, brc_size);
                close(fd);
        }
	brc_changed = 0;
}


#define BRC_OLD_MAXSIZE     32768
#define BRC_OLD_MAXNUM      60
#define BRC_OLD_STRLEN      15
#define BRC_OLD_ITEMSIZE    (BRC_OLD_STRLEN + 1 + BRC_OLD_MAXNUM * sizeof( int ))

static int brc_convert_struct(char* data,int size) /* �Ѿɵ�broardrc�ļ���ʽת�����µ�*/
{
	struct brc_struct brc;
	char* ptr,*newptr;
	newptr=ptr = data;
	while( ptr < &data[ size ] && (*ptr >= ' ' && *ptr <= 'z') ) {
	    int num;
	    char* tmp;
	    strncpy(brc.boardname,*ptr,BRC_STRLEN);
           brc.bid=getbnum(boardname);
           ptr+=BRC_OLD_STRLEN;
           num=(*ptr++) & 0xff;
           tmp=ptr;
    	    ptr+=sizeof(int)*num;
    	    if( num > BRC_MAXNUM ) { 
        		num = BRC_MAXNUM;
    	    }
    	    memcpy( brc.list, tmp, num * sizeof( int ) );
    	    if (newptr==data) {
    	    	*newptr=0; /*�����µĽṹ�汾��:PPP*/
    	    	newptr++;
    	    }
    	    memcpy(newptr,&brc,sizeof(brc));
    	    newptr+=sizeof(brc);/* ��Ϊ�µĽṹС*/
	}
	return newptr-data;
}

int brc_initial(char *userid, char *boardname ) /* ��ȡ�û�.boardrc�ļ���ȡ������ĵ�ǰ���brc_list */
{
    int i;
    char dirfile[MAX_PATH];
    char brc_buffer[BRC_FILESIZE];
    int brc_size;
    int bid=getbnum(boardname);
    int fd;
    for (i=0;i<BRC_CACHE_NUM;i++) 
    	if (brc_cache_entry[i].brd_id==bid)
    		return 1; /* cache ����*/

    sethomefile( dirfile, userid, ".boardrc" );
    if( (fd = open( dirfile, O_RDONLY )) != -1 ) {
            brc_size = read( fd, brc_buffer, sizeof( brc_buffer) );
            close( fd );
        } else {
            brc_size = 0;
        }

    if ((brc_size)&&(brc_buffer[0])) { /* �ϰ��boardrc*/
    	brc_size=brc_convert_struct(brc_buffer, brc_size);
    	if( (fd = open( dirfile, O_WRONLY )) != -1 ) {
            brc_size = write( fd, brc_buffer, brc_size);
            close( fd );
    	}
    }

    for (i=0;i<BRC_MAXBOARD;i++)
    return 0;
}


int brc_unread( int ftime) 
{
    int         n;

    if( brc_num <= 0 )
        return 1;
    for( n = 0; n < brc_num; n++ ) {
        if( ftime > brc_list[n] ) {
            return 1;
        } else if( ftime == brc_list[n] ) {
            return 0;
        }
    }
    return 0;
}

/*
int brc_has_read(char *file) {
	return !brc_unread(FILENAME2POSTTIME( file));
}
*/

int brc_add_read(char *filename) {
        int     ftime, n, i;
        ftime=atoi(&filename[2]);
        if(filename[0]!='M' && filename[0]!='G')  return;
        if(brc_num<=0) {
                brc_list[brc_num++] = ftime;
		brc_changed = 1;
                return;
        }
        for (n = 0; n < brc_num; n++) {
                if (ftime == brc_list[n]) {
                        return;
                } else if (ftime > brc_list[n]) {
                        if (brc_num < BRC_MAXNUM)
                                brc_num++;
                        for (i = brc_num - 1; i > n; i--) {
                                brc_list[i] = brc_list[i - 1];
                        }
                        brc_list[n] = ftime;
			brc_changed = 1;
                        return;
                }
        }
        if(brc_num<BRC_MAXNUM) brc_list[brc_num++] = ftime;
	brc_changed = 1;
}

int brc_clear() {
	int i;
	char filename[20];
	for(i=0; i<BRC_MAXNUM; i++) {
		sprintf(filename, "M.%d.A", time(0)-i);
		brc_add_read(filename);
	}
}

int brc_clear_new_flag(char* filename)
{
        int     ftime, n, i;
        ftime=atoi(&filename[2]);
        if(filename[0]!='M' && filename[0]!='G')  return;
        for (n = 0; n < brc_num; n++) 
                if (ftime >= brc_list[n]) 
			break;
	if (n<BRC_MAXNUM) {
		brc_list[n] = ftime;
		brc_num=n+1;
        	brc_changed = 1;
		for(i=n+1; i<BRC_MAXNUM; i++) 
			brc_list[i]=ftime-(i-n);
	}
	brc_num=BRC_MAXNUM;
	brc_changed = 1;
	return;
}

int junkboard(char* currboard)  /* �жϵ�ǰ���Ƿ�Ϊ junkboards */
{
    return seek_in_file("etc/junkboards",currboard);
}

int
checkreadonly( char *board) /* ����Ƿ���ֻ������ */
{
    struct boardheader* bh=getbcache(board);
    if (bh&&(bh->flag & BOARD_READONLY)) /* Checking if DIR access mode is "555" */
        return YEA;
    else
        return NA;
}

int
deny_me(char* user,char* board)   /* �ж��û� �Ƿ񱻽�ֹ�ڵ�ǰ�淢���� */
{
    char buf[STRLEN];

    setbfile(buf,board,"deny_users");
    return seek_in_file(buf,user);
}



int haspostperm(struct userec* user,char *bname) /* �ж��� bname�� �Ƿ���postȨ */
{
    register int i;

#ifdef BBSMAIN
    if(digestmode)
        return 0;
#endif
    /*    if( strcmp( bname, DEFAULTBOARD ) == 0 )  return 1; change by KCN 2000.09.01 */
    if ((i = getbnum(bname)) == 0) return 0;
    if (HAS_PERM(user,PERM_DENYPOST))
        /*if(!strcmp(bname, "sysop"))
               return 1;*/ /* Leeward 98.05.21 revised by stephen 2000.10.27*/ 
        /* let user denied post right post at Complain*/
    {if (!strcmp(bname, "Complain")) return 1;/* added by stephen 2000.10.27*/
        else if(!strcmp(bname, "sysop"))
            return 1;} /* stephen 2000.10.27 */
    if (!HAS_PERM(user,PERM_POST)) return 0;
    return (HAS_PERM(user,(bcache[i-1].level&~PERM_NOZAP) & ~PERM_POSTMASK));
}


int chk_BM_instr(char BMstr[STRLEN-1],char    bmname[IDLEN+2])
{
    char *ptr;
    char BMstrbuf[STRLEN-1];

    strcpy(BMstrbuf,BMstr);
    ptr=strtok(BMstrbuf,",: ;|&()\0\n");
    while(1)
    {
        if(ptr==NULL)
            return NA;
        if(!strcmp(ptr,bmname/*,strlen(currentuser->userid)*/))
            return YEA;
        ptr=strtok(NULL,",: ;|&()\0\n");
    }
}


int chk_currBM(char BMstr[STRLEN-1],struct userec* user)   
	/* ��������İ������� �ж�user�Ƿ��а��� Ȩ��*/
{
    char *ptr;
    char BMstrbuf[STRLEN-1];

    if(HAS_PERM(currentuser,PERM_OBOARDS)||HAS_PERM(currentuser,PERM_SYSOP))
        return YEA;

    if(!HAS_PERM(currentuser,PERM_BOARDS))
        return NA;

    return chk_BM_instr(BMstr, currentuser->userid);
}


