#ifndef __SYSNAME_H_
#define __SYSNAME_H_

#define BBSUID 			9999
#define BBSGID 			99

/* for bbs2www, by flyriver, 2001.3.9 */
#define SECNUM 9

#define DEFAULTBOARD    	"test"
#define MAXUSERS  		160000
#define MAXBOARD  		400
#define MAXACTIVE 		8000
#define MAX_GUEST_NUM		800

#define POP3PORT		110
#define	LENGTH_SCREEN_LINE	220
#define	LENGTH_FILE_BUFFER 	160
#define	LENGTH_ACBOARD_BUFFER	150
#define	LENGTH_ACBOARD_LINE 	80

#define LIFE_DAY_USER		120
#define LIFE_DAY_LONG		666
#define LIFE_DAY_SYSOP		119
#define LIFE_DAY_NODIE		999
#define LIFE_DAY_NEW		15
#define LIFE_DAY_SUICIDE	15

#define DAY_DELETED_CLEAN	20
#define SEC_DELETED_OLDHOME	2592000/* 3600*24*30��ע�����û������������û���Ŀ¼������ʱ��*/

#define	REGISTER_WAIT_TIME	(72*60*60)
#define	REGISTER_WAIT_TIME_NAME	"72 Сʱ"

#define MAIL_BBSDOMAIN      "smth.org"
#define MAIL_MAILSERVER     "166.111.8.236:25"

#define NAME_BBS_ENGLISH	"smth.org"
#define	NAME_BBS_CHINESE	"ˮľ�廪"
#define NAME_BBS_NICK		"BBS վ"

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

#ifndef EXTERN
extern char *permstrings[];
extern char    *groups[];
extern char    *explain[];
#else
char *permstrings[] = {
        "����Ȩ��",             /* PERM_BASIC */
        "����������",           /* PERM_CHAT */
        "������������",         /* PERM_PAGE */
        "��������",             /* PERM_POST */
        "ʹ����������ȷ",       /* PERM_LOGINOK */
        "��ֹ��������",         /* PERM_DENYPOST */
        "������",               /* PERM_CLOAK */
        "�ɼ�����",             /* PERM_SEECLOAK */
        "�����ʺ�",         /* PERM_XEMPT */
        "�༭ϵͳ����",         /* PERM_WELCOME */
        "����",                 /* PERM_BOARDS */
        "�ʺŹ���Ա",           /* PERM_ACCOUNTS */
        "ˮľ�廪������",       /* PERM_CHATCLOAK */
        "ͶƱ����Ա",           /* PERM_OVOTE */
        "ϵͳά������Ա",       /* PERM_SYSOP */
        "Read/Post ����",       /* PERM_POSTMASK */
        "�������ܹ�",           /* PERM_ANNOUNCE*/
        "�������ܹ�",           /* PERM_OBOARDS*/
        "������ܹ�",         /* PERM_ACBOARD*/
        "���� ZAP(������ר��)", /* PERM_NOZAP*/
        "������OP(Ԫ��Ժר��)", /* PERM_CHATOP */
        "ϵͳ�ܹ���Ա",         /* PERM_ADMIN */
        "�����ʺ�",           /* PERM_HONOR*/
        "����Ȩ�� 5",           /* PERM_UNUSE?*/
        "�ٲ�ίԱ",           /* PERM_JURY*/
        "����Ȩ�� 7",           /* PERM_UNUSE?*/
        "��ɱ������",        /*PERM_SUICIDE*/
        "����Ȩ�� 9",           /* PERM_UNUSE?*/
        "��ϵͳ���۰�",           /* PERM_UNUSE?*/
        "���Mail",           /* PERM_DENYMAIL*/

};

char    *explain[] = {
    "��վϵͳ",
    "��������",
    "���Լ���",
    "ѧ����ѧ",
    "��������",
    "̸��˵��",
    "У԰��Ϣ",
    "�����Ļ�",
    "�������",
    "������Ϣ",
    "�廪��ѧ",
    "�ֵ�ԺУ",
    "��  ��",
    NULL
};

char    *groups[] = {
    "system.faq",
    "rec.faq",
    "comp.faq",
    "sci.faq",
    "sport.faq",
    "talk.faq",
    "campus.faq",
    "literal.faq",
    "soc.faq",
    "network.faq",
    "thu.faq",
    "univ.faq",
    "other.faq",
    NULL
};
#endif

#endif
