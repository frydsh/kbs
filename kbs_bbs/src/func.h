/* define all function that need to by used in whole project */
#ifndef __FUNC_H__
#define __FUNC_H__
#if HAVE_MYSQL_SMTH == 1
#include <mysql.h>
#endif

#ifdef __cplusplus
extern "C" {
#endif

	char *showuserip(struct userec *user, char *ip);
	int def_list(long long XX);

/* defined in pass.c */
    int checkpasswd(const char *passwd, const char *test);
    int checkpasswd2(const char *passwd, const struct userec *user);
    int setpasswd(const char *passwd, struct userec *user);


    char *Cdate(time_t clock);  /* ʱ�� --> Ӣ�� */
    char *Ctime(time_t clock);  /* ʱ�� ת�� �� Ӣ�� */
    char *nextword(const char **str, char *buf, int sz);


/* defined in ucache.c */
    void load_user_title(ARG_VOID);
    struct userec *getuserbynum(int num);
    int getnewuserid2(char *userid);
    unsigned int ucache_hash(const char *userid);       /* hash function export for utmp usage */

    int resolve_ucache(ARG_VOID);
    void detach_ucache(ARG_VOID);

    int getuserid(char *userid, int uid);
    void setuserid(int num, const char *userid);        /* ����user num��idΪuser id */
    int setuserid2(int num, const char *userid);        /* ����user num��idΪuser id,userdʹ�� */
    int searchuser(const char *userid);
    int getuser(const char *userid, struct userec **lookupuser);        /* ȡ�û���Ϣ ������ֵ��lookupuser���� */
    char *u_namearray(char buf[][IDLEN + 1], int *pnum, char *tag);
    char *getuserid2(int uid);
    int update_user(struct userec *user, int num, int all);
    int apply_users(int (*fptr) (struct userec *, char *), char *arg);
    int getnewuserid(char *userid);

    int flush_ucache(ARG_VOID);
    int load_ucache(ARG_VOID);
    int get_giveupinfo(char *userid, int *basicperm, int s[10][2]);
    void save_giveupinfo(struct userec *lookupuser, int lcount, int s[10][2]);
    int do_after_login(struct userec* user,int unum,int mode);
    int do_after_logout(struct userec* user,struct user_info* uinfo,int unum, int mode);

#if USE_TMPFS==1
void setcachehomefile(char* path,const char* user,int unum, char* file);
void init_cachedata(const char* userid,int unum);
void flush_cachedata(const char* userid);
int clean_cachedata(const char* userid,int unum);
#endif

#ifdef HAVE_CUSTOM_USER_TITLE
char* get_user_title(unsigned char titleidx);
void set_user_title(unsigned char titleidx,char* newtitle);
#endif

/* ����tag ,���� ƥ���user id �б� (�������ע���û�)*/


/* 
   in bbslog.c 
   
   ���from[0]��һ�����֣���ô�������log�����ȼ���ȱʡ���ȼ��� 0

*/
    int bbslog(const char *from, const char *fmt, ...);
    int bmlog(char *id, char *boardname, int type, int value);
    int init_bbslog(ARG_VOID);
    void newbbslog(int type, const char *fmt, ...);

/* defined in utmp.c */
    int getnewutmpent2(struct user_info *up);
    typedef int (*APPLY_UTMP_FUNC) (struct user_info *, void *, int pos);
    void resolve_utmp(ARG_VOID);
    void detach_utmp(ARG_VOID);
    int getnewutmpent(struct user_info *up);
    int real_getnewutmpent(struct user_info *up);
#define CHECK_UENT(uident) ((utmpent > 0 && utmpent <= USHM_SIZE ) && \
			(utmpshm->uinfo[ utmpent - 1 ].uid==(uident)))
#define UPDATE_UTMP(field,entp) { if (CHECK_UENT((entp).uid)) \
			utmpshm->uinfo[ utmpent- 1 ].field=(entp).field; }
#define UPDATE_UTMP_STR(field,entp) { if (CHECK_UENT((entp).uid)) \
			strcpy(utmpshm->uinfo[ utmpent- 1 ].field,(entp).field); }
    int search_ulist(struct user_info *uentp, int (*fptr) (int, struct user_info *), int farg); /* ulist �� search ����fptr������ user */
#if 0
    void clear_utmp2(struct user_info *uentp);
#endif
    void clear_utmp2(int uentp);
    void clear_utmp(int uentp, int useridx, int pid);   /*����סutmp����clear_utmp2,��У��useridx */
    int apply_ulist(APPLY_UTMP_FUNC fptr, void *arg);
    int apply_utmpuid(APPLY_UTMP_FUNC fptr, int uid, void *arg);
    int apply_ulist_addr(APPLY_UTMP_FUNC fptr, void *arg);      /* apply func on user list */
    int get_utmp_number(ARG_VOID);      /* return active user */
    struct user_info *get_utmpent(int utmpnum);
    int get_utmpent_num(struct user_info *uent);        /* return utmp number */

    int t_cmpuids(int uid, struct user_info *up);
    int apply_utmp(APPLY_UTMP_FUNC fptr, int maxcount,const char *userid, void *arg);
    int getfriendstr(struct userec *user, struct user_info *puinfo);
    int myfriend(int uid, char *fexp);
    bool hisfriend(int uid, struct user_info *him);

/* defined in newio.c */
    void output(const char *s, int len);

/* defined in stuff.c */
	int calc_numofsig();
    int getuinfopid(ARG_VOID);
    int add_mailgroup_user(mailgroup_list_t * mgl, int entry, mailgroup_t * users, mailgroup_t * user);
    int delete_mailgroup_user(mailgroup_list_t * mgl, int entry, mailgroup_t * users, int pos);
    int modify_mailgroup_user(mailgroup_t * users, int pos, mailgroup_t * user);
    unsigned int load_mailgroup_list(const char *userid, mailgroup_list_t * mgl);
    int load_mailgroup(const char *userid, const char *group, mailgroup_t * mg, int num);
    int store_mailgroup_list(const char *userid, const mailgroup_list_t * mgl);
    int store_mailgroup(const char *userid, const char *group, const mailgroup_t * mg, int num);
    int import_friends_mailgroup(const char *userid, mailgroup_list_t * mgl);
    int add_mailgroup_item(const char *userid, mailgroup_list_t * mgl, mailgroup_list_item * item);
    int delete_mailgroup_item(const char *userid, mailgroup_list_t * mgl, int entry);
    int add_default_mailgroup_item(const char *userid, mailgroup_list_t * mgl);
    int modify_mailgroup_item(const char *userid, mailgroup_list_t * mgl, int entry, mailgroup_list_item * item);
    int import_old_mailgroup(const char *userid, mailgroup_list_t * mgl);

    int my_unlink(char *fname);/*��װunlink,���ڶ��屸�ݵ��ļ�*/
    int gettmpfilename(char *retchar, char *fmt, ...); /*�����ʱ�ļ���*/
    int setmailcheck(char *userid); /*����mail����־*/
    void save_maxuser(ARG_VOID);
    void detach_publicshm(ARG_VOID);
    int multilogin_user(struct userec *user, int usernum, int mode);
    time_t get_exit_time(char *id, char *exittime);
    int dashf(char *fname);
    int dashd(char *fname);
    int seek_in_file(const char* filename, const char* seekstr);
    char *setbdir(int digestmode, char *buf, const char *boardname);
    int my_system(const char *cmdstring);
    char *modestring(int mode, int towho, int complete, char *chatid);
    int countexp(struct userec *udata);
    int countperf(struct userec *udata);
    int compute_user_value(struct userec *urec);
    void *attach_shm(char *shmstr, int defaultkey, int shmsize, int *iscreate);
    void *attach_shm1(char *shmstr, int defaultkey, int shmsize, int *iscreate, int readonly, void *shmaddr);
    void    remove_shm(char *shmstr, int defaultkey, int shmsize);
    void attach_err(int shmkey, char *name);
    int id_invalid(char *userid);
    int addtofile(char filename[STRLEN], char str[STRLEN]);
    void bbssettime(time_t t);
    time_t bbstime(time_t * t);
    int getwwwguestcount(ARG_VOID);
    int del_from_file(char filename[STRLEN], char str[STRLEN]);
    int setpublicshmreadonly(int readonly);
    struct public_data *get_publicshm(ARG_VOID);        /*���public shmָ�� */
    char *sethomefile(char *buf, const char *userid, const char *filename);     /*ȡĳ�û��ļ� ·�� */
    char *sethomepath(char *buf, char *userid); /* ȡ ĳ�û� ��home */

    char *setmailfile(char *buf, const char *userid, const char *filename);     /* ȡĳ�û�mail�ļ� ·�� */
    char *setmailpath(char *buf, char *userid); /* ȡ ĳ�û� ��mail */
    char *setbpath(char *buf, char *boardname); /* ȡĳ�� ·�� */
    char *setbfile(char *buf,const char *boardname,const char *filename); /* ȡĳ�����ļ� */
    void RemoveMsgCountFile(char *userID);
    int bad_user_id(char *userid);      /* ���.badname�Ƿ�����ע��� */
    int valid_ident(char *ident);       /* ���Ϸ���ident */
    int getunifopid(ARG_VOID);
    struct user_info *t_search(const char *sid, int pid);
    int cmpinames(const char *userid, const char *uv);
    int cmpfnames(char *userid, struct friends *uv);
    int cmpfileinfoname(char *filename, struct fileheader *fi);
	int cmpfileid(int *id, struct fileheader *fi);
    int dodaemon(char *daemonname, bool single, bool closefd);

    int canIsend2(struct userec *user, char *userid);
    void sigbus(int signo);
    void encodestr(register char *str);
    int Isspace(char ch);
    char *idle_str(struct user_info *uent);
    int read_userdata(const char *userid, struct userdata *ud);
    int write_userdata(const char *userid, struct userdata *ud);
    void getuinfo(FILE * fn, struct userec *ptr_urec);
    int simplepasswd(char *str);
    void logattempt(char *uid, char *frm);
    int check_ban_IP(char *IP, char *buf);
    int is_valid_date(int year, int month, int day);
    int valid_filename(char *file, int use_subdir);
    int cmpuids2(int unum, struct user_info *urec);
    unsigned int load_mailbox_prop(char *userid);
    unsigned int store_mailbox_prop(char *userid);
    unsigned int get_mailbox_prop(char *userid);
    unsigned int update_mailbox_prop(char *userid, unsigned int prop);
    int gen_title(const char *boardname );
    off_t read_user_memo( char *userid, struct usermemo ** ppum );

#ifndef CYGWIN
#define time(x) bbstime(x)
#endif

    sigjmp_buf* push_sigbus(ARG_VOID);
    void popup_sigbus(ARG_VOID);

#define BBS_TRY \
    if (!sigsetjmp(*push_sigbus(), 1)) { \
        signal(SIGBUS, sigbus);

#define BBS_CATCH \
    } \
    else { \

#define BBS_END } \
    popup_sigbus();

#define BBS_RETURN(x) {popup_sigbus();return (x);}
#define BBS_RETURN_VOID {popup_sigbus();return;}


    int safe_mmapfile(char *filename, int openflag, int prot, int flag, void **ret_ptr, off_t * size, int *ret_fd);
    int safe_mmapfile_handle(int fd, int prot, int flag, void **ret_ptr, off_t * size);
    void end_mmapfile(void *ptr, off_t size, int fd);
    void set_proc_title(char *argv0, char *title);

/* 0-1 locks using semaphore, flock and fcntl lock replacement */
/* @author kxn */

void lock_sem(int lockid);
void unlock_sem(int lockid);
void unlock_sem_check(int lockid);


/* define in bcache.c */
	void flush_bcache(); /* ͬ��bcache*/
    void board_setcurrentuser(int idx,int num); /*���������û�����*/
    int getbnum(const char *bname);
    void resolve_boards(ARG_VOID);
    int get_boardcount(ARG_VOID);
    struct boardheader *getbcache(const char *bname);
    int normal_board(const char *bname);
    int getboardnum(const char *bname, struct boardheader *bh);       /* board name --> board No. & not check level */

	int fill_super_board(char *searchname, int result[], int max);
    int add_board(struct boardheader *newboard);
    void build_board_structure(const char *board);
    int apply_boards(int (*func) (struct boardheader*,void*),void* arg);   /* �����а� Ӧ�� func���� */
    int delete_board(char *boardname, char *title);     /* delete board entry */
    struct boardheader const *getboard(int num);
    int set_board(int bid, struct boardheader *board, struct boardheader *oldbh);
    struct BoardStatus *getbstatus(int index);  /* ��ð���ķ���״̬�ṹ */
    int updatelastpost(char *board);
    int setboardmark(char *board, int i);
    int setboardorigin(char *board, int i);
    int setboardtitle(const char *board, int i);
    int board_setreadonly(const char *board, int readonly);   /* ���ð���ֻ������ */
    int get_nextid(char *boardname);    /*�������������Ų��Զ���һ */
	int get_nextid_bid(int bid);
    void board_update_toptitle(struct boardheader* bh,int increment); /*�ı��ö�����*/
#if HAVE_WWW==1
    int resolve_guest_table(ARG_VOID); /* www guest shm */
    int www_guest_lock(ARG_VOID);
    void www_guest_unlock(int fd);
#endif

/* define in boards.c */
	int valid_brdname(char *brd);
    void detach_boards(ARG_VOID);
    int anonymousboard(const char *board);
    int load_boards(struct newpostdata *nbrd, char *boardprefix, int group, int pos, int len, bool sort, bool yank_flag, const char **input_namelist);
#if USE_TMPFS==1
    void init_brc_cache(const char* userid,bool replace);
    void free_brc_cache(char* userid);
#endif

    void brc_clear_new_flag(unsigned fid);      /* �������ĵ���ƪ����δ����� */

    int getfavnum(ARG_VOID);
    void save_zapbuf(ARG_VOID);
    void addFavBoard(int);
    void addFavBoardDir(int, char *);
    void release_favboard(ARG_VOID);
    int changeFavBoardDir(int i, char *s);
    int ExistFavBoard(int idx);
    void load_favboard(int dohelp);
    void save_favboard(ARG_VOID);
    void save_userfile(char *fname, int blknum, char *buf);
    int IsFavBoard(int idx);
    int MoveFavBoard(int p, int q, int fav_father);
    int DelFavBoard(int i);
    int SetFav(int i);

    int brc_initial(const char *userid, const char *boardname);
    char *brc_putrecord(char *ptr, char *name, int num, int *list);
    int fav_loaddata(struct newpostdata *nbrd, int favnow, int pos, int len, bool sort,const char **input_namelist);
    /*
     * ����һ�����brclist 
     */
    void brc_update(const char *userid);      /* ���浱ǰ��brclist���û���.boardrc */
    void brc_add_read(unsigned int fid);
    void brc_addreaddirectly(char *userid, int bnum, unsigned int fid);
    void brc_clear(ARG_VOID);
    int brc_unread(unsigned int fid);
    int junkboard(const char *currboard);     /* �ж��Ƿ�Ϊ junkboards */
    int checkreadonly(const char *board);     /* �ж��ǲ���ֻ������ */
    int deny_me(const char *user,const char *board);       /* �ж��û� �Ƿ񱻽�ֹ�ڵ�ǰ�淢���� */
    int haspostperm(const struct userec *user,const char *bname);  /* �ж��� bname�� �Ƿ���postȨ */
    int chk_BM_instr(const char BMstr[STRLEN - 1], const char bmname[IDLEN + 2]);       /*��ѯ�ַ������Ƿ���� bmname */
    int chk_currBM(const char BMstr[STRLEN - 1], struct userec *user);  /* ��������İ������� �ж�user�Ƿ��а��� Ȩ�� */
    int deldeny(struct userec *user, char *board, char *uident, int notice_only);       /* ɾ�� ��ֹPOST�û� */
    int check_read_perm(struct userec *user, const struct boardheader *board);
    int check_see_perm(struct userec *user, const struct boardheader *board);
    int is_outgo_board(const char *board);
    int poststatboard(const char *board);
    int is_emailpost_board(const char *board);

/* define in article.c */

    struct write_dir_arg {
      char* filename; /*.dir���ļ���*/
      int fd;                 //�ļ����
      struct fileheader* fileptr;   //�ļ�mmapָ��
      int ent;               //��ǰλ��
      off_t size;           //�ļ���С
      bool needclosefd; //�ͷŽṹ��ʱ���Ƿ���Ҫ�ر��ļ����ڲ�ʹ��
      bool needlock; //�Ƿ���Ҫ�Լ�lock�ļ�
    };
    void malloc_write_dir_arg(struct write_dir_arg*filearg);
    int init_write_dir_arg(struct write_dir_arg*filearg);
    void free_write_dir_arg(struct write_dir_arg*filearg);

    /*�����ö�*/
    int add_top(struct fileheader* fileinfo,char* boardname,int flag);
    /*�Ƽ�����*/
    int post_commend(struct userec *user, char *fromboard, struct fileheader *fileinfo);

	/* Search_Bin 
	 * ���ܣ�����key, ��ptr�����.DIR�������ж��ֲ���
	 */
	int Search_Bin(struct fileheader*ptr, int key, int start, int end);
    int delete_range(struct write_dir_arg* dirarg,int id1,int id2,int del_mode,int curmode,const struct boardheader* board);

    /*
     * mmap_search_dir_apply
     * ����:mmap struct fileheader�ṹ���ļ����ҵ���Ҫ�ļ�¼,
     * �ҵ������func,����start�����ļ�mmap�Ŀ�ʼ��entΪ��¼�ţ��� 1 ������
     * ��������ID�����ֲ���
     * ���õ�ʱ��
     * match=true,��ȫƥ��
     * match=false,�ҵ��ò����ǰһ����¼��
     * ͬʱ����index ���ؼ�¼�ı�š�
     */
    typedef int (*DIR_APPLY_FUNC) (int fd, struct fileheader * start, int ent, int total, struct fileheader * data, bool match);
    typedef int (*search_handler_t) (int fd, fileheader_t * base, int ent, int total, bool match, void *arg);
    int mmap_dir_search(int fd, const fileheader_t * key, search_handler_t func, void *arg);
    int mmap_search_apply(int fd, struct fileheader *buf, DIR_APPLY_FUNC func);
    int get_effsize(char * ffn);
	

/**
 * Get some records from article id. If this function is successfully
 * returned, the article record with the id is put at the center of 
 * buf logically. For example, if the user provided an buffer of three
 * records, then the article record with the id is copied to buf[1],
 * its previous record will be copied to buf[0] (but if the previous
 * record does not exist, buf[0] will be filled with zero), and its
 * next record will be copied to buf[2] (if the next record does not
 * exist, buf[2] will be filled will zero).
 *
 * @param fd The file descriptor of the .DIR file
 * @param id The article id to be searched in the .DIR file
 * @param buf The user provided buffer to hold the matched records
 * @param num The number of records that the buffer can hold
 * @param index The record number of the record corresponding to id
 * @return >0 The number of matched records
 *          0 No record matched or execution failed
 * @author flyriver
 */
    int get_records_from_id(int fd, int id, fileheader_t * buf, int num, int *index);

	int get_threads_from_id(const char *filename, int id, fileheader_t *buf, int num);
	int get_threads_from_gid(const char *filename, int gid, fileheader_t *buf, int num, int start, int * haveprev);

    int Origin2(char text[256]);
/*����edit mark*/
    int add_edit_mark(char *fname, int mode, char *title);
    int get_postfilename(char *filename, char *direct, int use_subdir);
    int mail_file(char *fromid, char *tmpfile, char *userid, char *title, int unlink, struct fileheader *fh);
    int mail_file_sent(char *fromid, char *tmpfile, char *userid, char *title, int unlink);     /*peregrine */
    int update_user_usedspace(int delta, struct userec *user);
    int getmailnum(char *recmaildir);
    int isowner(struct userec *user, struct fileheader *fileinfo);
    int do_del_post(struct userec *user, struct write_dir_arg* delarg,struct fileheader *fileinfo, char *board, int digestmode, int decpost);
    /*
     * ɾ�����£�digestmode�����Ķ�ģʽ��decpost��ʾ����ɾ���Ƿ�������� 
     */

    int cmpname(struct fileheader *fhdr, char name[STRLEN]);
/* Haohmaru.99.3.30.�Ƚ� ĳ�ļ����Ƿ�� ��ǰ�ļ� ��ͬ */

    void addsignature(FILE * fp, struct userec *user, int sig); /*����User��ǩ���� */

/*���  POST ���һ�� ����Դ*/
    void add_loginfo(char *filepath, struct userec *user, char *currboard, int Anony);

/* ��quote_file���Ƶ�filepath (ת�����Զ�����)*/
    void getcross(char *filepath, char *quote_file, struct userec *user, int in_mail, char *board, char *title, int Anony, int mode, char *sourceboard);

    void write_header(FILE * fp, struct userec *user, int in_mail, char *board, char *title, int Anony, int mode);

/*д��.post�ļ�������*/
    int write_posts(char *id, char *board, unsigned int groupid);
    void cancelpost(const char *board, const char *userid, struct fileheader *fh, int owned, int autoappend);
    int outgo_post(struct fileheader *fh, char *board, char *title);
    int after_post(struct userec *user, struct fileheader *fh, char *boardname, struct fileheader *re, int poststat);
    int post_file(struct userec *user, char *fromboard, char *filename, char *nboard, char *posttitle, int Anony, int mode);
    int post_cross(struct userec *user, char *toboard, char *fromboard, char *title, char *filename, int Anony, int in_mail, char islocal, int mode);   /* (�Զ������ļ���) ת�����Զ����� */

    int dele_digest(char *dname, const char *boardname);
    int change_post_flag(struct write_dir_arg* dirarg,int currmode, struct boardheader*board,
        struct fileheader *fileinfo, int flag,struct fileheader * data,bool dobmlog);


/**
 * A function return flag character of an article.
 * 
 * @param ent pointer to fileheader structure of the article
 * @param user pointer to userec structure of the user
 * @param is_bm nonzero for board manager, zero for others
 * @return flag character of the article
 * @author flyriver
 */
    char get_article_flag(struct fileheader *ent, struct userec *user, char *boardname, int is_bm);
    time_t get_posttime(const struct fileheader *fileinfo);
    void set_posttime(struct fileheader *fileinfo);
    char* checkattach(char *buf, long size,long *len,char** attachptr);
/* roy 2003.7.23 */
#ifdef HAVE_USERMONEY
    int get_score(struct userec *user);
    int set_score(struct userec *user, int score);
    int add_score(struct userec *user, int score_addition);
    int get_money(struct userec *user);
    int set_money(struct userec *user, int money);
    int add_money(struct userec *user, int money_addition);
#endif

/**
 * һ���ܼ��attach��fgets
 * ����attach����1
 * �ļ�β����-1
 */
    int attach_fgets(char* s,int size,FILE* stream);
    int skip_attach_fgets(char* s,int size,FILE* stream);
    int put_attach(FILE* in, FILE* out, int size);
/* define in record.c */
    int safewrite(int fd, void *buf, int size);
    typedef int (*RECORD_FUNC_ARG) (void *, void *);
    typedef int (*APPLY_FUNC_ARG) (void *, int, void *);
    int delete_record(char *dirname, int size, int ent, RECORD_FUNC_ARG filecheck, void *arg);
    int apply_record(char *filename, APPLY_FUNC_ARG fptr, int size, void *arg, int applycopy, bool reverse);
    int append_record(char *filename, void *record, int size);
    int substitute_record(char *filename, void *rptr, int size, int id);
    int search_record(char *filename, void *rptr, int size, RECORD_FUNC_ARG fptr, void *farg);
    long get_num_records(char *filename, int size);
    long get_sum_records(char *fpath, int size);        /*���.DIR�����ļ���С���� */
    long get_mailusedspace(struct userec *user, int force);     /*peregrine */
    int get_record_handle(int fd, void *rptr, int size, int id);
    int get_record(char *filename, void *rptr, int size, int id);
    int get_records(char *filename, void *rptr, int size, int id, int number);
    int read_get_records(char *filename, char *filename1, char *rptr, int size, int id, int number);
    int search_record_back(int fd,      /* idx file handle */
                           int size,    /* record size */
                           int start,   /* where to start reverse search */
                           RECORD_FUNC_ARG fptr,        /* compare function */
                           void *farg,  /* additional param to call fptr() / original record */
                           void *rptr,  /* record data buffer to be used for reading idx file */
                           int sorted); /* if records in file are sorted */
    void load_mail_list(struct userec *user, struct _mail_list *mail_list);
    void save_mail_list(struct _mail_list *mail_list);


/* define in sysconf.c */
    char *sysconf_str(char *key);
    int sysconf_eval(char *key, int defaultval);
    struct smenuitem *sysconf_getmenu(char *menu_name);
    void build_sysconf(char *configfile, char *imgfile);
    void load_sysconf(ARG_VOID);
    char *sysconf_relocate(char *data); /*���¶�λmenuitem������ַ��� */
    int check_sysconf(ARG_VOID);

/* libmsg.c */
    int get_unreadcount(char *uident);
    int get_unreadmsg(char *uident);
    int load_msghead(int id, char *uident, int index, struct msghead *head);
    int load_msgtext(char *uident, struct msghead *head, char *msgbuf);
    int translate_msg(char* src, struct msghead *head, char* dest);
    int get_msgcount(int id, char *uident);
    void mail_msg(struct userec* user);
    int clear_msg(char *uident);
    int addto_msglist(int utmpnum, char *userid);
    int sendmsgfunc(struct user_info *uentp, const char *msgstr, int mode);
    int canmsg(struct userec *fromuser, struct user_info *uin);
    int can_override(char *userid, char *whoasks);
    int delfrom_msglist(int utmpnum, char *userid);
    int msg_can_sendmsg(char *userid, int utmpnum);
#ifdef SMS_SUPPORT
#if HAVE_MYSQL_SMTH == 1
int get_sql_smsmsg( struct smsmsg * smdata, char *userid, char *dest, time_t start_time, time_t end_time, int type, 					int level, int start, int num, char *msgtxt, int desc);
int sign_smsmsg_read(int id );
#endif
int DoReplyCheck(char * n, unsigned int sn, char isSucceed);
int sms_init_memory(ARG_VOID);
int DoUnReg(char * n);
int DoSendSMS(char * n, char * d, char * c);
int save_msgtext(char *uident, struct msghead * head,const char *msgbuf);
int save_smsmsg(char *uident, struct msghead *head, char *msgbuf, int readed);
int count_sql_smsmsg( char *userid, char *dest, time_t start_time, time_t end_time, int type, int level, char *msgtxt );
int chk_smsmsg(int force );
#endif

#if HAVE_MYSQL_SMTH == 1
char * get_al_mobile( char *userid, char *mobile);
int get_sql_al( struct addresslist * smdata, char *userid, char *dest, char *group,int start, int num, int order, char *msgtxt);
int add_sql_al(char *userid, struct addresslist *al, char *msgbuf);
int count_sql_al( char *userid, char *dest, char *group, char *msgtxt);
MYSQL * my_connect_mysql(MYSQL *s);
#endif

#ifdef PERSONAL_CORP

char * tt2timestamp( time_t tt, char *c);
time_t timestamp2tt( char *row );
int pc_conv_file_to_body( char **body, char *fname);
int pc_load_usr( struct _pc_selusr **ps, char prefix);
int get_pc_users( struct pc_users * pu, char * userid );
int get_pc_a_node( struct pc_nodes * pn, unsigned long nid);
int count_pc_nodes( int uid, unsigned long pid, int type, int access );
int get_pc_nodes( struct pc_nodes * pn, int uid, unsigned long pid, int type, int access, int start, int num, int withbody);
int get_pc_a_com( struct pc_comments * pn, unsigned long cid );
int count_pc_comments( unsigned long nid);
int get_pc_comments( struct pc_comments * pn, unsigned long nid, int start, int num, int withbody);
int add_pc_users(struct pc_users *pn);
int add_pc_nodes(struct pc_nodes *pn);
int pc_in_blacklist(char * userid , unsigned long pcuid);
int add_pc_comments(struct pc_comments *pn);
int del_pc_users(struct pc_users *pn);
int del_pc_nodes( unsigned long nid , int access , int uid );
int pc_del_junk(int uid);
int del_pc_node_junk(unsigned int nid , int access , int uid );
int del_pc_comments( unsigned long nid, unsigned long cid );
int pc_paste_node(unsigned long nid, int newuid, int newaccess, unsigned long newpid ) ;
int pc_add_visitcount(unsigned long nid);
int pc_add_user(ARG_VOID);
int import_to_pc(int ent, struct fileheader *fileinfo, char *direct);

int pc_read(char *userid);
int pc_selusr(char prefix);
int pc_read_dir(int first);
int pc_sec(ARG_VOID);
int pc_read_comment(ARG_VOID);
int pc_logs(struct pc_logs *pn);

#endif
/* site.c */
    void set_posttime2(struct fileheader *dest, struct fileheader *src);
    char *ModeType(int mode);
    char *email_domain(ARG_VOID);
    int get_shmkey(char *s);
    int uleveltochar(char *buf, struct userec *lookupuser);

    int safe_kill(int x, int y);

    void main_bbs(int convit, char *argv);
    void get_mail_limit(struct userec *user, int *sumlimit, int *numlimit);

/* bbs_sendmail.c */
    int mail_buf(struct userec*fromuser, char *mail_buf, char *userid, char *title);
    int chkusermail(struct userec *user);
    int chkreceiver(struct userec *fromuser, struct userec *touser);
    int bbs_sendmail(char *fname, char *title, char *receiver, int isuu, int isbig5, int noansi);
    int check_query_mail(char qry_mail_dir[STRLEN]);
/* convcode.c */
    void conv_init(ARG_VOID);

/* libann.c */
    void ann_add_item(MENU * pm, ITEM * it);
    int ann_load_directory(MENU * pm);
    ITEM *ann_alloc_items(size_t num);
    void ann_free_items(ITEM * it, size_t num);
    void ann_set_items(MENU * pm, ITEM * it, size_t num);
    int ann_get_board(char *path, char *board, size_t len);
    int ann_get_path(char *board, char *path, size_t len);
	int valid_fname(char *str);
    void a_additem(MENU* pm,const char* title,const char* fname,char* host,int port,long attachpos);    /* ����ITEM object,����ʼ�� */
    int a_loadnames(MENU* pm);             /* װ�� .Names */
    int a_savenames(MENU* pm);             /*���浱ǰMENU�� .Names */
    void a_freenames(MENU * pm);
    int save_import_path(char ** i_path,char ** i_title,time_t* i_path_time );
    void load_import_path(char ** i_path,char ** i_title, time_t* i_path_time,int * i_path_select);
    void free_import_path(char ** i_path,char ** i_title,time_t* i_path_time);
	int linkto(char *path,const char *fname,const char *title);
	int add_grp(const char group[STRLEN],const char bname[STRLEN],const char title[STRLEN],const char gname[STRLEN]);

/* check the user's access for the path
  return < 0 deny access
  return ==0 has access and it can be access by any body
  return >0 need some extra permission to access it
*/
    int ann_traverse_check(char *path, struct userec *user);
    /*
     * in site.c 
     */
    int ann_get_postfilename(char *filename, struct fileheader *fileinfo, MENU * pm);

	/* in libtmpl.c */
int orig_tmpl_init(char * board, int mode, struct a_template ** ptemp);
int orig_tmpl_free(struct a_template ** pptemp, int temp_num);
int orig_tmpl_save(struct a_template * ptemp, int temp_num, char *board);

/* zmodem */
    int zsend_file(char *filename, char *title);
#define FILENAME2POSTTIME(x) (atoi(((char*)x)+2))

/* filter */
    int check_badword(char *checkfile);
    int check_filter(char *patternfile, char *checkfile, int defaultval);
    int check_badword_str(char *string, int str_len);
    
/* register */
#ifdef HAVE_TSINGHUA_INFO_REGISTER
int auto_register(char* userid,char* email,int msize);
#endif

/* libc */
#ifndef HAVE_MEMMEM
void *memmem(register const void *s, size_t slen, register const void *p, 
		size_t plen);
#endif /* ! HAVE_MEMMEM */

/* calltime.c */
time_t calc_calltime(int mode);

/* xml.c */
char *encode_xml(char *buf, const char *str, size_t buflen);
char *string_copy(char *buf, const char *str, size_t * buflen);

/* output.c */
buffered_output_t* alloc_output(size_t buflen);

#ifdef SMS_SUPPORT
int smsid2uid(char* smsid);
void uid2smsid(struct user_info* uin,char* smsid);
int uid2smsnumber(struct user_info* uin);
int smsnumber2uid(byte number[4]);
unsigned int byte2long(byte arg[4]);
void long2byte(unsigned int num, byte* arg);
#endif

#ifdef __cplusplus
}
#endif
#endif
