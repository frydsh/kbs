#ifndef __SYSNAME_H_
#define __SYSNAME_H_

#define ZIXIA

#define BBSUID 			80 //9999
#define BBSGID 			80 //99

/* for bbs2www, by flyriver, 2001.3.9 */
#define SECNUM 10

#define DEFAULTBOARD    	"zixia.net"//test
#define MAXUSERS  		40000 //150,000
#define MAXBOARD  		256 //400
#define MAXACTIVE 		999  //3000
#define MAX_GUEST_NUM		99

#define POP3PORT		3110	//110
#define	LENGTH_SCREEN_LINE	256	//220
#define	LENGTH_FILE_BUFFER 	260	//160
#define	LENGTH_ACBOARD_BUFFER	200	//150
#define	LENGTH_ACBOARD_LINE 	300	//80

#define LIFE_DAY_USER		365	//120
#define LIFE_DAY_LONG		666	//666
#define LIFE_DAY_SYSOP		500	//120
#define LIFE_DAY_NODIE		999	//120
#define LIFE_DAY_NEW		30	//15
#define LIFE_DAY_SUICIDE	15	//15

#define DAY_DELETED_CLEAN	20	//20
#define SEC_DELETED_OLDHOME	0 	/*  3600*24*30��ע�����û������������û���Ŀ¼������ʱ��*/

#define	REGISTER_WAIT_TIME	(30*60) // (72*60*60)
#define	REGISTER_WAIT_TIME_NAME	"30����" //72 Сʱ

#define MAIL_BBSDOMAIN      "bbs.zixia.net"
#define MAIL_MAILSERVER     "127.0.0.1:25"

#define NAME_BBS_ENGLISH	"bbs.zixia.net" //smth.org
#define	NAME_BBS_CHINESE	"������" //ˮľ�廪
#define NAME_BBS_NICK		"��ͷ��" // BBS վ

#define FOOTER_MOVIE		"  ��  ͷ  ��  " // ��  ӭ  Ͷ  ��
#define ISSUE_LOGIN		"��վʹ��һ̨X86������" //��վʹ����⹫˾������ݷ�����
#define ISSUE_LOGOUT		"�������_�ܣ�" //����������

#define NAME_USER_SHORT		"��ͷ����" //�û�
#define NAME_USER_LONG		"��ͷ����" //"ˮľ�û�"
#define NAME_SYSOP		"ǿ��ͷ" //"System Operator"
#define NAME_BM			"�����ҵ�" //����
#define NAME_POLICE		"����" //"����"
#define	NAME_SYSOP_GROUP	"��ͷ��߲�" //"վ����"
#define NAME_ANONYMOUS		"Do you zixia!?" //"ˮĸ���� Today!"
#define NAME_ANONYMOUS_FROM	"ˮ����" //������ʹ�ļ�

#define NAME_MATTER		"����" //վ��
#define NAME_SYS_MANAGE		"���ڹ���" //"ϵͳά��"
#define NAME_SEND_MSG		"����ͷ" // "��ѶϢ"
#define NAME_VIEW_MSG		"�鸫ͷ" // "��ѶϢ"

#define CHAT_MAIN_ROOM		"zixia" //main
#define	CHAT_TOPIC		"��˿����������ˮ�������" //"�����������İ�"
#define CHAT_MSG_NOT_OP		"*** ��˿����Ҫ�Ҵ��� ***" //"*** �����Ǳ������ҵ�op ***"
#define	CHAT_ROOM_NAME		"����"//"������"
#define	CHAT_SERVER		"��˿��" //"����㳡"
#define CHAT_MSG_QUIT		"�ص������ǰ" //"����ϵͳ"
#define CHAT_OP			"����" //"������ op"
#define CHAT_SYSTEM		"����" //"ϵͳ"
#define	CHAT_PARTY		"����" // "���"

#define DEFAULT_NICK		"����"

#define MSG_ERR_USERID		"�ţ������ͷ��˭��..."



#ifndef EXTERN
extern char *permstrings[];
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
        NAME_BBS_CHINESE "������",       /* PERM_CHATCLOAK */
        "ͶƱ����Ա",           /* PERM_OVOTE */
        "ϵͳά������Ա",       /* PERM_SYSOP */
        "Read/Post ����",       /* PERM_POSTMASK */
        "�������ܹ�",           /* PERM_ANNOUNCE*/
        "�������ܹ�",           /* PERM_OBOARDS*/
        "������ܹ�",         /* PERM_ACBOARD*/
        "���� ZAP(������ר��)", /* PERM_NOZAP*/
        "������OP(Ԫ��Ժר��)", /* PERM_CHATOP */
        "ϵͳ�ܹ���Ա",         /* PERM_ADMIN */
        "�����ʺ�",          	/* PERM_HORNOR*/
        "�����ܾ�����",         /* PERM_SECANC*/
        "��AKA��",           /* PERM_GIRL*/
        "��Sexy��",           /* PERM_SEXY*/
        "��ɱ������",           /* PERM_SUICIDE?*/
        "�������",           /* PERM_MM*/
        "��ϵͳ���۰�",           /* PERM_DISS*/
        "���Mail",           /* PERM_DENYMAIL*/

};
#endif

static char    *explain[] = {
	"�� ͷ ��",
	"������",
	"��С�ֶ�",
        "�Ժ�����",
	"��������",
	"����Ū��",
        "����֮·",
        "������ʩ",
	"�¹ⱦ��",
        "��ʥȡ��",
         NULL
};
 
static char    *groups[] = {
	"AxFaction",
	"zixia",
	"Factions",
	"Entertain",
	"Watering",
	"Poem",
	"GoWest",
	"DouFuGirl",
  	"PandoraBox",
  	"Reserve",
         NULL
};
#endif
