
/*
   These are the 16 basic permission bits. 
   All but the last one are used in comm_lists.c and help.c to control user 
   access to priviliged functions. The symbolic names are given to roughly
   correspond to their actual usage; feel free to use them for different
   purposes though. The PERM_SPECIAL1 and PERM_SPECIAL2 are not used by 
   default and you can use them to set up restricted boards or chat rooms.
*/

#define PERM_BASIC      000001
#define PERM_CHAT       000002
#define PERM_PAGE       000004
#define PERM_POST       000010
#define PERM_LOGINOK    000020
#define PERM_DENYPOST   000040
#define PERM_CLOAK      000100
#define PERM_SEECLOAK   000200
#define PERM_XEMPT      000400
#define PERM_WELCOME    001000
#define PERM_BOARDS     002000
#define PERM_ACCOUNTS   004000
#define PERM_CHATCLOAK  010000
#define PERM_OVOTE      020000
#define PERM_SYSOP      040000
#define PERM_POSTMASK  0100000     
#define PERM_ANNOUNCE  0200000
#define PERM_OBOARDS   0400000
#define PERM_ACBOARD   01000000
#define PERM_NOZAP     02000000
#define UNUSE2         04000000
#define UNUSE3         010000000
#define UNUSE4         020000000
#define UNUSE5         040000000
#define UNUSE6         0100000000
#define UNUSE7         0200000000
#define UNUSE8         0400000000
#define UNUSE9         01000000000
#define UNUSE10        02000000000
#define UNUSE11        04000000000


/* means the rest is a post mask */

/* This is the default permission granted to all new accounts. */
#define PERM_DEFAULT    (PERM_BASIC | PERM_CHAT | PERM_PAGE | PERM_POST | PERM_LOGINOK)

/* These permissions are bitwise ORs of the basic bits. They work that way
   too. For example, anyone with PERM_SYSOP or PERM_BOARDS or both has
   PERM_SEEBLEVELS. */

#define PERM_ADMINMENU  (PERM_ACCOUNTS | PERM_OVOTE | PERM_SYSOP|PERM_OBOARDS|PERM_WELCOME)
#define PERM_MULTILOG   PERM_SYSOP
#define PERM_LOGINCLOAK (PERM_SYSOP | PERM_ACCOUNTS | PERM_BOARDS | PERM_WELCOME)
#define PERM_SEEULEVELS (PERM_SYSOP | PERM_BOARDS)
#define PERM_SEEBLEVELS (PERM_SYSOP | PERM_BOARDS)
#define PERM_MARKPOST   (PERM_SYSOP | PERM_BOARDS)
#define PERM_UCLEAN     (PERM_SYSOP | PERM_ACCOUNTS)
#define PERM_NOTIMEOUT  PERM_SYSOP

#define PERM_SENDMAIL   0
#define PERM_READMAIL   PERM_BASIC
#define PERM_VOTE       PERM_BASIC

/* These are used only in Internet Mail Forwarding */
/* You may want to be more restrictive than the default, especially for an
   open access BBS. */

#define PERM_SETADDR    PERM_BASIC     /* to set address for forwarding */
#define PERM_FORWARD    PERM_BASIC     /* to do the forwarding */

/* Don't mess with this. */
#define HAS_PERM(x)     ((x)?currentuser.userlevel&(x):1)
#define DEFINE(x)     ((x)?currentuser.userdefine&(x):1)


#ifndef EXTERN
extern char *permstrings[];
#else

/*#define NUMPERMS (30)*//*Define in bbs.h*/

/* You might want to put more descriptive strings for SPECIAL1 and SPECIAL2
   depending on how/if you use them. */
char *permstrings[] = {
        "����Ȩ��",             /* PERM_BASIC */
        "����������",           /* PERM_CHAT */
        "������������",         /* PERM_PAGE */
        "��������",             /* PERM_POST */
        "ʹ����������ȷ",       /* PERM_LOGINOK */
        "��ֹ��������",         /* PERM_DENYPOST */
        "��·��Ϸ(QKMJ)",       /* PERM_CLOAK */
        "�˹��ǻ�Ⱥ",           /* PERM_SEECLOAK */
        "�ʺ����ñ���",         /* PERM_XEMPT */
        "�༭ϵͳ����",         /* PERM_WELCOME */
        "����",                 /* PERM_BOARDS */
        "�ʺŹ���Ա",           /* PERM_ACCOUNTS */
        "�İ�������һվ������", /* PERM_CHATCLOAK */
        "ͶƱ����Ա",           /* PERM_OVOTE */
        "ϵͳά������Ա",       /* PERM_SYSOP */
        "Read/Post ����",       /* PERM_POSTMASK */
        "�������ܹ�",           /* PERM_ANNOUNCE*/
        "�������ܹ�",           /* PERM_OBOARDS*/
        "������ܹ�",         /* PERM_ACBOARD*/
        "���� ZAP(������ר��)", /* PERM_NOZAP*/
        "����Ȩ�� 2",           /* PERM_UNUSE?*/
        "����Ȩ�� 3",           /* PERM_UNUSE?*/
        "����Ȩ�� 4",           /* PERM_UNUSE?*/
        "����Ȩ�� 5",           /* PERM_UNUSE?*/
        "����Ȩ�� 6",           /* PERM_UNUSE?*/
        "����Ȩ�� 7",           /* PERM_UNUSE?*/
        "����Ȩ�� 8",           /* PERM_UNUSE?*/
        "����Ȩ�� 9",           /* PERM_UNUSE?*/
        "����Ȩ��10 ",           /* PERM_UNUSE?*/
        "����Ȩ��11",           /* PERM_UNUSE?*/

};
#endif

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
/*#define PERM_POSTMASK  0100000  */   /* means the rest is a post mask */


#define NUMDEFINES 23
#ifndef EXTERN
extern char *user_definestr[];
#else
/* You might want to put more descriptive strings for SPECIAL1 and SPECIAL2
   depending on how/if you use them. */
char *user_definestr[] = {
        "�����",             /* DEF_ACBOARD */
        "ʹ�ò�ɫ",             /* DEF_COLOR */
        "�༭ʱ��ʾ״̬��",     /* DEF_EDITMSG */
        "������������ New ��ʾ",/* DEF_NEWPOST */
        "ѡ����ѶϢ��",         /* DEF_ENDLINE */
        "��վʱ��ʾ��������",   /* DEF_LOGFRIEND */
        "�ú��Ѻ���",           /* DEF_FRIENDCALL */
        "ʹ���Լ�����վ����",   /* DEF_LOGOUT */
        "��վʱ��ʾ����¼",     /* DEF_INNOTE */
        "��վʱ��ʾ����¼",     /* DEF_OUTNOTE */
        "ѶϢ��ģʽ��������/����",/* DEF_NOTMSGFRIEND */
        "�˵�ģʽѡ��һ��/����",/* DEF_NORMALSCR */
        "�Ķ������Ƿ�ʹ���ƾ�ѡ��",/* DEF_CIRCLE */
        "�Ķ������α�ͣ춵�һƪδ��",/* DEF_FIRSTNEW */
        "өĻ����ɫ�ʣ�һ��/�任",/* DEF_TITLECOLOR */
        "���������˵�ѶϢ",     /* DEF_ALLMSG */
        "���ܺ��ѵ�ѶϢ",       /* DEF_FRIENDMSG */
        "�յ�ѶϢ��������",     /* DEF_SOUNDMSG */
        "��վ��Ļ�����ѶϢ",   /* DEF_MAILMSG */
        "���к�����վ��֪ͨ",         /* DEF_LOGININFORM */
        "�˵�����ʾ������Ϣ",         /* DEF_SHOWSCREEN */
        "��վʱ��ʾʮ������",         /* DEF_SHOWHOT */
        "��վʱ�ۿ����԰�",         /* DEF_NOTEPAD*/
};
#endif

