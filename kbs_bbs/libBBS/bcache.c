/* ��.BOARDS��cache ����
                    KCN 2001.05.16 */
#include "bbs.h"
#include <sys/ipc.h>
#include <sys/shm.h>
static void bcache_setreadonly(int readonly);
static int bcache_lock()
{
    int lockfd;

    lockfd = creat("bcache.lock", 0600);
    if (lockfd < 0) {
        bbslog("3system", "CACHE:lock bcache:%s", strerror(errno));
        return -1;
    }
    bcache_setreadonly(0);
    flock(lockfd, LOCK_EX);
    return lockfd;
}
static void bcache_unlock(int fd)
{
    flock(fd, LOCK_UN);
    bcache_setreadonly(1);
    close(fd);
}
static void bcache_setreadonly(int readonly)
{
    int boardfd;
	void *oldptr = bcache;
    munmap(bcache, MAXBOARD * sizeof(struct boardheader));
    if ((boardfd = open(BOARDS, O_RDWR | O_CREAT, 0644)) == -1) {
        bbslog("3system", "Can't open " BOARDS "file %s", strerror(errno));
        exit(-1);
    }
    if (readonly)
        bcache = (struct boardheader *) mmap(oldptr, MAXBOARD * sizeof(struct boardheader), PROT_READ, MAP_SHARED, boardfd, 0);
    else
        bcache = (struct boardheader *) mmap(oldptr, MAXBOARD * sizeof(struct boardheader), PROT_READ | PROT_WRITE, MAP_SHARED, boardfd, 0);
    close(boardfd);
}
int getlastpost(char *board, int *lastpost, int *total)
{
    struct fileheader fh;
    struct stat st;
    char filename[STRLEN * 2];
    int fd, atotal;

    sprintf(filename, "boards/%s/" DOT_DIR, board);
    if ((fd = open(filename, O_RDWR)) < 0)
        return 0;
    fstat(fd, &st);
    atotal = st.st_size / sizeof(fh);
    if (atotal <= 0) {
        *lastpost = 0;
        *total = 0;
        close(fd);
        return 0;
    }
    *total = atotal;
    lseek(fd, (atotal - 1) * sizeof(fh), SEEK_SET);
    if (read(fd, &fh, sizeof(fh)) > 0) {
        *lastpost = fh.id;
    }
    close(fd);
    return 0;
}
int updatelastpost(char *board)
{
    int pos;

    pos = getboardnum(board,NULL);       /* board name --> board No. */
    if (pos > 0) {
        getlastpost(board, &brdshm->bstatus[pos - 1].lastpost, &brdshm->bstatus[pos - 1].total);
        return 0;
    } else
        return -1;
}

int setboardmark(char *board, int i)
{
    int pos;

    pos = getboardnum(board,NULL);       /* board name --> board No. */
    if (pos > 0) {
    	 int j;
    	 j=brdshm->bstatus[pos - 1].updatemark;
    	 if(i>=0&&i<=1)
        brdshm->bstatus[pos - 1].updatemark = i;
        return j;
    } else
        return -1;
}

int setboardorigin(char *board, int i)
{
    int pos;

    pos = getboardnum(board,NULL);       /* board name --> board No. */
    if (pos > 0) {
    	 int j;
    	 j=brdshm->bstatus[pos - 1].updateorigin;
    	 if(i>=0&&i<=1)
        brdshm->bstatus[pos - 1].updateorigin = i;
        return j;
    } else
        return -1;
}

int setboardtitle(const char *board, int i)
{
    int pos;

    pos = getboardnum(board,NULL);       /* board name --> board No. */
    if (pos > 0) {
    	 int j;
    	 j=brdshm->bstatus[pos - 1].updatetitle;
    	 if(i>=0&&i<=1)
        brdshm->bstatus[pos - 1].updatetitle = i;
        return j;
    } else
        return -1;
}

int get_nextid(char* boardname)
{
    int fd,ret;

    fd = bcache_lock();
    ret=getboardnum(boardname,NULL);
    if (ret!=0) {
        bcache[ret-1].nowid++;
        ret=bcache[ret-1].nowid;
    } else
       bbslog("3system", "wrong get_nextid %s", boardname);
    bcache_unlock(fd);
    return ret;
}

int get_nextid_bid(int bid)
{
    int fd,ret;

	ret = bid;
	if (ret > 0)
	{
		fd = bcache_lock();
		bcache[ret-1].nowid++;
		ret=bcache[ret-1].nowid;
		bcache_unlock(fd);
	}
    return ret;
}

void resolve_boards()
{
    int boardfd=-1;
    int iscreate = 0;

    if (bcache == NULL) {
        if ((boardfd = open(BOARDS, O_RDWR | O_CREAT, 0644)) == -1) {
            bbslog("3system", "Can't open " BOARDS "file %s", strerror(errno));
            exit(-1);
        }
		ftruncate(boardfd, MAXBOARD * sizeof(struct boardheader));
        bcache = (struct boardheader *) mmap(NULL, MAXBOARD * sizeof(struct boardheader), PROT_READ , MAP_SHARED, boardfd, 0);
        if (bcache == (struct boardheader *) -1) {
            bbslog("4system", "Can't map " BOARDS "file %s", strerror(errno));
            close(boardfd);
            exit(-1);
        }
    }
    if (brdshm == NULL) {
        brdshm = attach_shm("BCACHE_SHMKEY", 3693, sizeof(*brdshm), &iscreate); /* attach board share memory */
        if (iscreate) {
            int i, maxi = -1;
            int fd;

            bbslog("3system", "reload bcache!");
            fd = bcache_lock();
            for (i = 0; i < MAXBOARD; i++)
                if (bcache[i].filename[0]) {
                    getlastpost(bcache[i].filename, &brdshm->bstatus[i].lastpost, &brdshm->bstatus[i].total);
                    maxi = i;
                }
            if (maxi != -1)
                brdshm->numboards = maxi + 1;
            bcache_unlock(fd);
        }
    }
    if (boardfd!=-1)
        close(boardfd);
}

void detach_boards()
{
    munmap(bcache, MAXBOARD * sizeof(struct boardheader));
    bcache=NULL;
    shmdt(brdshm);
    brdshm=NULL;
}

struct BoardStatus *getbstatus(int index)
{
    return &brdshm->bstatus[index-1];
}
int apply_boards(int (*func) (struct boardheader *, void* ),void* arg)
{                               /* �����а� Ӧ�� func���� */
    register int i;

    for (i = 0; i < brdshm->numboards; i++)
            if (bcache[i].filename[0])
                if ((*func) (&bcache[i],arg) == QUIT)
                    return QUIT;
    return 0;
}

int fill_super_board(char *searchname, int result[], int max)
{
	register int i;
	int total=0;

    for (i = 0; i < brdshm->numboards && total < max ; i++){
        if (bcache[i].filename[0] == '\0')
			continue;
    	if (check_read_perm(currentuser, &bcache[i])) {
			if (strcasestr(bcache[i].filename, searchname) || strcasestr(bcache[i].des, searchname) || strcasestr(bcache[i].title, searchname) ){
				result[total] = i + 1;
				total ++;
			}
		}
	}
	return total;
}

int getbnum(const char *bname)
{
    register int i;

    for (i = 0; i < brdshm->numboards; i++)
#ifdef BBSMAIN
        if (check_read_perm(currentuser,&bcache[i]))
#endif
            if (!strncasecmp(bname, bcache[i].filename, STRLEN))
                return i + 1;
    return 0;
}

/*---	added by period		2000-11-07	to be used in postfile	---*/
int getboardnum(const char *bname, struct boardheader *bh)
{                               /* board name --> board No. & not check level */
    register int i;

    for (i = 0; i < brdshm->numboards; i++)
        if (!strncasecmp(bname, bcache[i].filename, STRLEN)) {
            if (bh)
                *bh = bcache[i];
            return i + 1;
        }
    return 0;
}/*---	---*/
struct boardheader *getbcache(const char *bname)
{
    int i;

    i = getbnum(bname);
    if (i == 0)
        return NULL;
    return &bcache[i - 1];
}

int get_boardcount()
{
    return brdshm->numboards;
}
const struct boardheader *getboard(int num)
{
    if (num > 0 && num <= MAXBOARD) {
        return &bcache[num - 1];
    }
    return NULL;
}
int delete_board(char *boardname, char *title)
{
    int bid, i;
    char buf[1024];
    int fd;

    bid = getbnum(boardname);
    if (bid == 0) {
#ifdef BBSMAIN
        move(2, 0);
        prints("����ȷ��������");
        clrtoeol();
        pressreturn();
        clear();
#endif                          /* 
                                 */
        return -1;
    }
    bid--;
    strcpy(boardname, bcache[bid].filename);
    strcpy(title, bcache[bid].title);
#ifdef BBSMAIN
    move(1, 0);
    prints("ɾ�������� '%s'.", bcache[bid].filename);
    clrtoeol();
    getdata(2, 0, "(Yes, or No) [N]: ", genbuf, 4, DOECHO, NULL, true);
    if (genbuf[0] != 'Y' && genbuf[0] != 'y') { /* if not yes quit */
        move(2, 0);
        prints("ȡ��ɾ��....\n");
        pressreturn();
        clear();
        return -1;
    }
    sprintf(buf, "ɾ����������%s", bcache[bid].filename);
    securityreport(buf, NULL, NULL);
#endif                          /* 
                                 */
    sprintf(buf, " << '%s'�� %s ɾ�� >>", bcache[bid].filename, currentuser->userid);
#ifdef BBSMAIN
    getdata(3, 0, "�Ƴ������� (Yes, or No) [Y]: ", genbuf, 4, DOECHO, NULL, true);
    if (genbuf[0] != 'N' && genbuf[0] != 'n')
    {
            del_grp(boardname, title + 13);
    }
#endif
    fd = bcache_lock();
    bid = getbnum(boardname);
    if (bid == 0)
        return -1;              /* maybe delete by other people */
    bid--;
    if (brdshm->numboards == bid + 1) {
        if (bid == 0)
            brdshm->numboards = 0;
        else
            for (i = bid - 1; i >= 0; i--)
                if (bcache[i].filename[0]) {
                    brdshm->numboards = i + 1;
                    break;
                }
    }
    memset(&bcache[bid], 0, sizeof(struct boardheader));
    strcpy(bcache[bid].title, buf);
    bcache[bid].level = PERM_SYSOP;
    bcache_unlock(fd);
    return 0;
}
int add_board(struct boardheader *newboard)
{
    int bid = 0;
    int fd;

    fd = bcache_lock();
    if ((bid = getbnum("")) <= 0)
        if (brdshm->numboards < MAXBOARD)
            bid = brdshm->numboards + 1;
    if (bid > 0) {
        newboard->createtime=time(0);
        memcpy(&bcache[bid - 1], newboard, sizeof(struct boardheader));
        if (bid > brdshm->numboards)
            brdshm->numboards = bid;
        bcache_unlock(fd);
        return 0;
    }
    bcache_unlock(fd);
    return -1;
}

static int clearclubreadright(struct userec* user,struct boardheader* bh)
{
    user->club_read_rights[(bh->clubnum-1)>>5]&=~(1<<(bh->clubnum-1));
    return 0;
}

static int clearclubwriteright(struct userec* user, struct boardheader* bh)
{
    user->club_write_rights[(bh->clubnum-1)>>5]&=~(1<<(bh->clubnum-1));
    return 0;
}

int set_board(int bid, struct boardheader *board,struct boardheader *oldbh)
{
    bcache_setreadonly(0);
    if (oldbh) {
    	char buf[100];
    	if ((board->flag&BOARD_CLUB_READ)^(oldbh->flag&BOARD_CLUB_READ)) {
    	    if (oldbh->clubnum&&oldbh->clubnum<=MAXCLUB) /*������ϵľ��ֲ�*/
    	        apply_users((int (*)(struct userec*,char*))clearclubreadright,(void*)oldbh);
	    setbfile(buf, board->filename, "read_club_users");
	    unlink(buf);
    	}
    	if ((board->flag&BOARD_CLUB_WRITE)^(oldbh->flag&BOARD_CLUB_WRITE)) {
    	    if (oldbh->clubnum&&oldbh->clubnum<=MAXCLUB) /*������ϵľ��ֲ�*/
    	         apply_users((int (*)(struct userec*,char*))clearclubwriteright,(void*)oldbh);
	    setbfile(buf, board->filename, "write_club_users");
	    unlink(buf);
    	}
       if (!(board->flag&BOARD_CLUB_READ)&&!(board->flag&BOARD_CLUB_WRITE))
    	   board->clubnum=0;
       else if ((board->clubnum<=0)||(board->clubnum>=MAXCLUB)) {
    	/* ��Ҫ����clubnum*/
    	    int used[MAXCLUB];
    	    int i;
    	    bzero(used,sizeof(int)*MAXCLUB);
    	    for (i=0;i<MAXBOARD;i++)
    	    	if ((bcache[i].flag&BOARD_CLUB_READ||bcache[i].flag&BOARD_CLUB_WRITE)
    	    	    &&bcache[i].clubnum>0&&bcache[i].clubnum<=MAXCLUB)
    	    	    used[bcache[i].clubnum-1]=1;
    	    for (i=0;i<MAXCLUB;i++)
    	    	if (used[i]==0)
    	    		break;
    	    if (i==MAXCLUB) {
    	    	/* ���ֲ�������!*/
    	    	bbslog("3error","new club error:the number raise to max...");
    	    	board->clubnum=0;
    	    	board->flag&=(~BOARD_CLUB_READ) & (~BOARD_CLUB_WRITE);
    	    } else board->clubnum=i+1;
       }
       board->nowid=bcache[bid-1].nowid;
       /*���¼���Ŀ¼�İ�����*/
       if (board->flag&BOARD_GROUP) {
           int i;
           board->board_data.group_total=0;
    	    for (i=0;i<MAXBOARD;i++)
    	        if (bcache[i].group==bid) 
                    board->board_data.group_total++;
       } else if ((board->group)&&(bcache[bid-1].group!=board->group)) {
       //��������Ŀ¼�İ�����
            if (bcache[bid-1].group)
                bcache[bcache[bid-1].group-1].board_data.group_total--;
            if (board->group)
                bcache[board->group-1].board_data.group_total++;
       }
    }
    if (board->nowid==-1) //����nowid
	    board->nowid=bcache[bid-1].nowid;
    memcpy(&bcache[bid - 1], board, sizeof(struct boardheader));
    bcache_setreadonly(1);
    return 0;
}
int board_setreadonly(const char *board, int readonly)
{
    int fd;
    struct boardheader *bh;

    bh = getbcache(board);
    if (!bh)
        return 0;
    fd = bcache_lock();
    if (readonly)
        bh->flag |= BOARD_READONLY;
    else
        bh->flag &= ~BOARD_READONLY;
    bcache_unlock(fd);
    return 0;
}

void board_setcurrentuser(int idx,int num)
{
    if (idx<=0) return;
    if (num > 0)
        brdshm->bstatus[idx - 1].currentusers++;
    else  if (num < 0) {
      brdshm->bstatus[idx - 1].currentusers--;
      if (brdshm->bstatus[idx - 1].currentusers<0)
          brdshm->bstatus[idx - 1].currentusers=0;
    }
    else
        brdshm->bstatus[idx - 1].currentusers=0;
}

void board_update_toptitle(struct boardheader* bh,int increment)
{
    int fd=bcache_lock();
    if (increment==0)
      bh->toptitle=0;
    else
      bh->toptitle+=increment;
    bcache_unlock(fd);
}

/* add by stiger, 2004,0322 */
int board_regenspecial(char *board, int mode, char *index)
{
    struct fileheader *ptr1;
    struct flock ldata, ldata2;
    int fd, fd2, size = sizeof(fileheader), total, i, count = 0;
    char olddirect[PATHLEN];
    char newdirect[PATHLEN];
    char *ptr;
    struct stat buf;
    bool init;
    size_t bm_search[256];

    setbdir(DIR_MODE_NORMAL, olddirect, board);
    setbdir(mode,newdirect, board);

    if ((fd = open(newdirect, O_WRONLY | O_CREAT, 0664)) == -1) {
        bbslog("3user", "%s", "recopen err");
        return -1;      /* �����ļ���������*/
    }
    ldata.l_type = F_WRLCK;
    ldata.l_whence = 0;
    ldata.l_len = 0;
    ldata.l_start = 0;
    if (fcntl(fd, F_SETLKW, &ldata) == -1) {
        bbslog("3user", "%s", "reclock err");
        close(fd);
        return -1;      /* lock error*/
    }
    /* ��ʼ�������*/
    if (mode == DIR_MODE_ORIGIN && !setboardorigin(board, -1)) {
        ldata.l_type = F_UNLCK;
        fcntl(fd, F_SETLKW, &ldata);
        close(fd);
        return -1;
    }

    if ((fd2 = open(olddirect, O_RDONLY, 0664)) == -1) {
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

    init = false;
    if ((i = safe_mmapfile_handle(fd2, PROT_READ, MAP_SHARED, (void **) &ptr, &buf.st_size)) != 1) {
        if (i == 2)
            end_mmapfile((void *) ptr, buf.st_size, -1);
        ldata2.l_type = F_UNLCK;
        fcntl(fd2, F_SETLKW, &ldata2);
        close(fd2);
        ldata.l_type = F_UNLCK;
        fcntl(fd, F_SETLKW, &ldata);
        close(fd);
        return -1;
    }
    ptr1 = (struct fileheader *) ptr;
    for (i = 0; i < total; i++) {
        if (((mode == DIR_MODE_ORIGIN) && (ptr1->id == ptr1->groupid ))
            || ((mode == DIR_MODE_AUTHOR) && !strcasecmp(ptr1->owner, index) )
            || ((mode == DIR_MODE_TITLE)  && bm_strcasestr_rp(ptr1->title, index, bm_search, &init))) {
            write(fd, ptr1, size);
            count++;
        }
        ptr1++;
    }
    end_mmapfile((void *) ptr, buf.st_size, -1);
    ldata2.l_type = F_UNLCK;
    fcntl(fd2, F_SETLKW, &ldata2);
    close(fd2);
    ftruncate(fd, count * size);

    if (mode == DIR_MODE_ORIGIN)
        setboardorigin(board, 0);   /* ���flag*/

    ldata.l_type = F_UNLCK;
    fcntl(fd, F_SETLKW, &ldata);        /* �˳���������*/
    close(fd);

    return count;
}
