/* define variable used by whole project */

#ifndef SMTH_API
#define SMTH_API extern
#endif
SMTH_API struct UTMPFILE *utmpshm;
/*extern struct UCACHE *uidshm;*/
SMTH_API struct userec *currentuser;
//extern struct userdata curruserdata;
SMTH_API struct usermemo *currentmemo;

SMTH_API struct friends_info *topfriend;

SMTH_API int scrint;              /* Set when screen has been initialized */

                                  /* Used by namecomplete *only* */

extern struct user_info uinfo;  /* Ditto above...utmp entry is stored here
                                   and written back to the utmp file when
                                   necessary (pretty darn often). */
SMTH_API int usernum;             /* Index into passwds file user record */
SMTH_API int utmpent;             /* Index into this users utmp file entry */
extern int count_friends, count_users;  /*Add by SmallPig for count users and friends */

extern int t_lines, t_columns;  /* Screen size / width */
extern struct userec lookupuser;        /* Used when searching for other user info */


extern int nettyNN;
extern char netty_board[];      /* ���վ��ʼ��֮һ  netty */
extern struct boardheader* currboard;        /* name of currently selected board */
extern int currboardent;
extern char currBM[];           /* BM of currently selected board */

extern int selboard;            /* THis flag is true if above is active */

extern char genbuf[1024];       /* generally used global buffer */

extern struct commands cmdlist[];       /* main menu command list */

extern jmp_buf byebye;          /* Used for exception condition like I/O error */

extern struct commands xyzlist[];       /* These are command lists for all the */
extern struct commands talklist[];      /* sub-menus */
extern struct commands maillist[];
extern struct commands dellist[];
extern struct commands maintlist[];

extern char save_title[];       /* These are used by the editor when inserting */
extern int in_mail;
extern int dumb_term;
extern int showansi;

extern char fromhost[IPLEN + 1];
extern time_t login_start_time;

SMTH_API struct boardheader *bcache;
SMTH_API struct BCACHE *brdshm;
SMTH_API struct BDIRCACHE *bdirshm;

#ifdef BBSMAIN
extern int idle_count;
#endif


extern struct _mail_list user_mail_list;

#if HAVE_WWW==1
extern struct WWW_GUEST_TABLE *wwwguest_shm;
#endif

#ifdef SMS_SUPPORT
extern int smsresult;
extern void* smsbuf;
#endif
extern const char secname[SECNUM][2][20];

SMTH_API int msg_count;

