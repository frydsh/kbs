#ifndef __SYSNAME_H_
#define __SYSNAME_H_

#undef NINE_BUILD
#undef HAVE_BIRTHDAY
#undef HAPPY_BBS
#undef HAVE_COLOR_DATE
#undef HAVE_TEMPORARY_NICK
#undef HAVE_FRIENDS_NUM
#undef HAVE_REVERSE_DNS
#define CHINESE_CHARACTER


#define SMTH
#define FILTER
/* 
 *    Define DOTIMEOUT to set a timer to bbslog out users who sit idle on the system.
 *       Then decide how long to let them stay: MONITOR_TIMEOUT is the time in
 *          seconds a user can sit idle in Monitor mode; IDLE_TIMEOUT applies to all
 *             other modes. 
 *             */
#define DOTIMEOUT 1

/* 
 *    These are moot if DOTIMEOUT is commented; leave them defined anyway. 
 *    */
#define IDLE_TIMEOUT    (60*20) 
#define MONITOR_TIMEOUT (60*20) 

#define BBSUID 			9999
#define BBSGID 			99

/* for bbs2www, by flyriver, 2001.3.9 */
#define SECNUM 13
#define BBS_PAGE_SIZE 20

#define SQUID_ACCL

#define DEFAULTBOARD    	"test"
#define FILTER_BOARD        "Filter"
#define SYSMAIL_BOARD       "sysmail"
#define BLESS_BOARD "Blessing"

#define MAXUSERS  		200000
#define MAXCLUB			128
#define MAXBOARD  		600
#define MAXACTIVE 		8000
#define MAX_GUEST_NUM		800

#define POP3PORT		110
#define POP3SPORT		995
/* ASCIIArt, by czz, 2002.7.5 */
#define       LENGTH_SCREEN_LINE      255
#define       LENGTH_FILE_BUFFER      255
#define       LENGTH_ACBOARD_BUFFER   200
#define       LENGTH_ACBOARD_LINE     300

#define LIFE_DAY_USER		120
#define LIFE_DAY_YEAR          365
#define LIFE_DAY_LONG		666
#define LIFE_DAY_SYSOP		120
#define LIFE_DAY_NODIE		999
#define LIFE_DAY_NEW		15
#define LIFE_DAY_SUICIDE	15

#define DAY_DELETED_CLEAN	97
#define SEC_DELETED_OLDHOME	2592000 /* 3600*24*30��ע�����û������������û���Ŀ¼������ʱ�� */

#define	REGISTER_WAIT_TIME	(72*60*60)
#define	REGISTER_WAIT_TIME_NAME	"72 Сʱ"

#define MAIL_BBSDOMAIN      "smth.org"
#define MAIL_MAILSERVER     "166.111.8.236:25"

#define NAME_BBS_ENGLISH	"smth.org"
#define	NAME_BBS_CHINESE	"ˮľ�廪"
#define NAME_BBS_NICK		"BBS վ"

#define BBS_FULL_NAME "BBS ˮľ�廪վ"

#define FOOTER_MOVIE		"��  ӭ  Ͷ  ��"
/*#define ISSUE_LOGIN		"��վʹ����⹫˾������ݷ�����"*/
#define ISSUE_LOGIN		"���PC  ��21����˻�ָ��������"
#define ISSUE_LOGOUT		"����������"

#define NAME_USER_SHORT		"�û�"
#define NAME_USER_LONG		"ˮľ�û�"
#define NAME_SYSOP		"System Operator"
#define NAME_BM			"����"
#define NAME_POLICE		"����"
#define	NAME_SYSOP_GROUP	"վ����"
#define NAME_ANONYMOUS		"ˮĸ���� Today!"
#define NAME_ANONYMOUS_FROM	"������ʹ�ļ�"
#define ANONYMOUS_DEFAULT 0

#define NAME_MATTER		"վ��"
#define NAME_SYS_MANAGE		"ϵͳά��"
#define NAME_SEND_MSG		"��ѶϢ"
#define NAME_VIEW_MSG		"��ѶϢ"

#define CHAT_MAIN_ROOM		"main"
#define	CHAT_TOPIC		"�����������İ�"
#define CHAT_MSG_NOT_OP		"*** �����Ǳ������ҵ�op ***"
#define	CHAT_ROOM_NAME		"������"
#define	CHAT_SERVER		"����㳡"
#define CHAT_MSG_QUIT		"����ϵͳ"
#define CHAT_OP			"������ op"
#define CHAT_SYSTEM		"ϵͳ"
#define	CHAT_PARTY		"���"

#define DEFAULT_NICK		"ÿ�찮���һЩ"

#define MSG_ERR_USERID		"�����ʹ���ߴ���..."
#define LOGIN_PROMPT		"���������"
#define PASSWD_PROMPT		"����������"

/* Ȩ��λ���� */
/*
   These are the 16 basic permission bits. 
   All but the last one are used in comm_lists.c and help.c to control user 
   access to priviliged functions. The symbolic names are given to roughly
   correspond to their actual usage; feel free to use them for different
   purposes though. The PERM_SPECIAL1 and PERM_SPECIAL2 are not used by 
   default and you can use them to set up restricted boards or chat rooms.
*/

#define NUMPERMS (30)

#define PERM_BASIC      000001
#define PERM_CHAT       000002
#define PERM_PAGE       000004
#define PERM_POST       000010
#define PERM_LOGINOK    000020
#define PERM_BMAMANGER	000040
#define PERM_CLOAK      000100
#define PERM_SEECLOAK   000200
#define PERM_XEMPT      000400
#define PERM_WELCOME    001000
#define PERM_BOARDS     002000
#define PERM_ACCOUNTS   004000
#define PERM_CHATCLOAK  010000
#define PERM_DENYRELAX      020000
#define PERM_SYSOP      040000
#define PERM_POSTMASK  0100000
#define PERM_ANNOUNCE  0200000
#define PERM_OBOARDS   0400000
#define PERM_ACBOARD   01000000
#define PERM_NOZAP     02000000
#define PERM_CHATOP    04000000
#define PERM_ADMIN     010000000
#define PERM_HORNOR   	020000000
#define PERM_SECANC    040000000
#define PERM_JURY      0100000000
#define PERM_CHECKCD      0200000000
#define PERM_SUICIDE   0400000000
#define PERM_COLLECTIVE 	01000000000
#define PERM_DISS        02000000000
#define PERM_DENYMAIL   	04000000000


#define XPERMSTR "bTCPRp#@XWBA$VS!DEM1234567890%"
/* means the rest is a post mask */

/* This is the default permission granted to all new accounts. */
#define PERM_DEFAULT    (PERM_BASIC | PERM_CHAT | PERM_PAGE | PERM_POST | PERM_LOGINOK)

/* These permissions are bitwise ORs of the basic bits. They work that way
   too. For example, anyone with PERM_SYSOP or PERM_BOARDS or both has
   PERM_SEEBLEVELS. */

#define PERM_ADMINMENU  (PERM_ACCOUNTS | PERM_SYSOP|PERM_OBOARDS|PERM_WELCOME)
#define PERM_MULTILOG   (PERM_SYSOP | PERM_ANNOUNCE | PERM_OBOARDS)
#define PERM_LOGINCLOAK (PERM_SYSOP | PERM_ACCOUNTS | PERM_BOARDS | PERM_OBOARDS | PERM_WELCOME)
#define PERM_SEEULEVELS (PERM_SYSOP | PERM_BOARDS)
#define PERM_SEEBLEVELS (PERM_SYSOP | PERM_BOARDS)
#define PERM_MARKPOST   (PERM_SYSOP | PERM_BOARDS)
#define PERM_UCLEAN     (PERM_SYSOP | PERM_ACCOUNTS)
#define PERM_NOTIMEOUT  PERM_SYSOP
/* PERM_MANAGER will be used to allow 2 windows and 
bigger mailbox. --stephen 2001.10.31*/
#define PERM_MANAGER    (PERM_CHATOP | PERM_JURY | PERM_BOARDS)
#define PERM_SENDMAIL   0
#define PERM_READMAIL   PERM_BASIC
#define PERM_VOTE       PERM_BASIC

/* These are used only in Internet Mail Forwarding */
/* You may want to be more restrictive than the default, especially for an
   open access BBS. */

#define PERM_SETADDR    PERM_BASIC      /* to set address for forwarding */
#define PERM_FORWARD    PERM_BASIC      /* to do the forwarding */

/* Don't mess with this. */
#define HAS_PERM(user,x) ((x)?((user)->userlevel)&(x):1)
#define DEFINE(user,x)     ((x)?((user)->userdefine)&(x):1)

#define TDEFINE(x) ((x)?(tmpuser)&(x):1)

/* �û��Զ��������� */
#define DEF_ACBOARD      000001
#define DEF_COLOR        000002
#define DEF_EDITMSG      000004
#define DEF_NEWPOST      000010
#define DEF_ENDLINE      000020
#define DEF_LOGFRIEND    000040
#define DEF_FRIENDCALL   000100
#define DEF_LOGOUT       000200
#define DEF_INNOTE       000400
#define DEF_OUTNOTE      001000
#define DEF_NOTMSGFRIEND 002000
#define DEF_NORMALSCR    004000
#define DEF_CIRCLE       010000
#define DEF_FIRSTNEW     020000
#define DEF_TITLECOLOR   040000
#define DEF_ALLMSG       0100000
#define DEF_FRIENDMSG    0200000
#define DEF_SOUNDMSG     0400000
#define DEF_MAILMSG      01000000
#define DEF_LOGININFORM  02000000
#define DEF_SHOWSCREEN   04000000
#define DEF_SHOWHOT      010000000
#define DEF_NOTEPAD      020000000
#define DEF_IGNOREMSG    040000000      /* Added by Marco */
#define DEF_HIGHCOLOR	0100000000   /*Leeward 98.01.12 */
#define DEF_SHOWSTATISTIC 0200000000    /* Haohmaru */
#define DEF_UNREADMARK 0400000000       /* Luzi 99.01.12 */
#define DEF_USEGB     01000000000       /* KCN,99.09.05 */
#define DEF_CHCHAR    02000000000
/*#define DEF_HIDEIP    02000000000  Haohmaru,99.12.18*/

/*#define PERM_POSTMASK  0100000  *//* means the rest is a post mask */

#define NUMDEFINES 29


#define TDEF_SPLITSCREEN 000001

extern const char *permstrings[];
extern const char    *groups[];
extern const char    *explain[];
extern const char *user_definestr[];
extern const char *mailbox_prop_str[];

/**
 * �������û�ʱ�İ��������ַ���
 */
#define UL_CHANGE_NICK_UPPER   'C'
#define UL_CHANGE_NICK_LOWER   'c'
#define UL_SWITCH_FRIEND_UPPER 'F'
#define UL_SWITCH_FRIEND_LOWER 'f'

/**
 * ������ز��֡�
 */
#define STRLEN          80
#define BM_LEN 60
#define FILENAME_LEN 20
#define OWNER_LEN 30
typedef struct fileheader {     /* This structure is used to hold data in */
    char filename[FILENAME_LEN];      /* the DIR files */
    unsigned int id, groupid, reid;
#ifdef FILTER
    char o_board[STRLEN - BM_LEN];
    unsigned int o_id;
    unsigned int o_groupid;
    unsigned int o_reid;
    char unused1[14];
#else
    char unused1[46];
#endif
    char innflag[2];
    char owner[OWNER_LEN];
    char unused2[50];
    char title[STRLEN];
    unsigned level;
    unsigned char accessed[12]; /* struct size = 256 bytes */
} fileheader;

typedef struct fileheader fileheader_t;

#define GET_POSTFILENAME(x,y) get_postfilename(x,y,0)
#define GET_MAILFILENAME(x,y) get_postfilename(x,y,0)
#define VALID_FILENAME(x) valid_filename(x,0)

// WWW����

//�޸�����֮���ҳ��ʹ��https
#define SECURE_HTTPS

/**
 * Mailbox properties.
 * 
 * @author flyriver
 */

#define MBP_SAVESENTMAIL      0x00000001
#define MBP_FORCEDELETEMAIL   0x00000002
#define MBP_MAILBOXSHORTCUT   0x00000004

#define MBP_NUMS 3

#define HAS_MAILBOX_PROP(u, x) ((u)->mailbox_prop & x)

/**
 * Mailgroup macros.
 *
 * @author flyriver
 */
#define MAX_MAILGROUP_NUM 30
#define MAX_MAILGROUP_USERS 300

/**
attach define
*/
#define ATTACHTMPPATH "boards/_attach"
#endif
