#include "bbs.h"

char seccode[SECNUM][5]={
	"0", "1", "2", "3", "4", "5", "6", "7", "8", "9"
};

char secname[SECNUM][2][20]={
	"�� ͷ ��", "[�ڰ�/ϵͳ]",
	"������", "[��/����]",
	"��С�ֶ�", "[����/У��]",
	"�Ժ�����", "[����/����]",
	"��������", "[����/����]",
	"����Ū��", "[����/�Ļ�]",
	"����֮·", "[����/�ۻ�]",
	"������ʩ", "[����/����]",
	"�¹ⱦ��", "[ת��/�б�]",
	"��ʥȡ��", "[רҵ/����]",
};

struct _shmkey
{
	char key[20];
	int value;
};

static const struct _shmkey shmkeys[]= {
{ "BCACHE_SHMKEY",  3693 },
{ "UCACHE_SHMKEY",  3696 },
{ "UTMP_SHMKEY",    3699 },
{ "ACBOARD_SHMKEY", 9013 },
{ "ISSUE_SHMKEY",   5010 },
{ "GOODBYE_SHMKEY", 5020 },
{ "PASSWDCACHE_SHMKEY", 3697 },
{ "STAT_SHMKEY",    5100 },
{ "CONVTABLE_SHMKEY",    5101 },
{ "MSG_SHMKEY",    5200 },
{    "",   0 }
};

int get_shmkey(char *s)
{
	int n=0;
	while(shmkeys[n].key!=0)
	{
		if(!strcasecmp(shmkeys[n].key, s))
			return shmkeys[n].value;
		n++;
	}
	return 0;
}

int
uleveltochar( char* buf, struct userec *lookupuser )
{
	unsigned lvl;
	char userid[IDLEN+2];
	
	lvl = lookupuser->userlevel;
	strncpy( userid, lookupuser->userid, IDLEN+2 );

	// buf[10], buf ��� 4 ������ + 1 byte ��\0��β��
	//���� level
	//
	if( ( lvl & PERM_DENYPOST ) || !( lvl | PERM_POST ) ) strcpy( buf, "�ư�" ); 
      	else if ( ! (lvl & PERM_BASIC) ) strcpy( buf, "Ϲ��" ); 
	else if( lvl < PERM_DEFAULT ) strcpy( buf, "����" );
    	else if( lvl & PERM_SYSOP ) strcpy(buf,"����");
      	else if( lvl & PERM_MM && lvl & PERM_CHATCLOAK ) strcpy(buf,"��������");
      	else if( lvl & PERM_MM ) strcpy(buf,"�������");
      	else  if( lvl & PERM_CHATCLOAK ) strcpy(buf,"�޵�ţʭ");
      	else if  ( lvl & PERM_BOARDS ) strcpy(buf,"�����ҵ�");
      	else strcpy(buf, NAME_USER_SHORT);

	//����˵�������� level
    	if( !strcmp(lookupuser->userid,"SYSOP")
		    || !strcmp(lookupuser->userid,"Rama") )
	    strcpy( buf, "ǿ��ͷ" );
    	else if( !strcmp(lookupuser->userid,"zixia") )
	    strcpy( buf, "����" );
    	else if( !strcmp(lookupuser->userid,"halen") )
	    strcpy( buf, "СƤ����" );
    	else if( !strcmp(lookupuser->userid,"cclu") )
	    strcpy( buf, "������è" );
    	else if( !strcmp(lookupuser->userid,"Bison") )
	    strcpy( buf, "����" );
    	else if( !strcmp(lookupuser->userid,"KCN") )
	    strcpy( buf, "�ϵ�" );
    	else if( !strcmp(lookupuser->userid,"cityhunter") 
		    || !strcmp(lookupuser->userid,"soso")
		    || !strcmp(lookupuser->userid,"Czz")
		    || !strcmp(lookupuser->userid,"flyriver") )
	    strcpy( buf, "ţħ��" );
    	else if( !strcmp(lookupuser->userid,"guest") )
	    strcpy( buf, "����" );

    	return 1;
}

#include "modes.h"

char *
ModeType(mode)
int     mode;
{
    switch(mode) {
    case IDLE:      return "^@^zz..ZZ" ;
    case NEW:       return "��" NAME_USER_SHORT "ע��" ;
    case LOGIN:     return "����" NAME_BBS_NICK;
    case CSIE_ANNOUNCE:     return "��ȡ����";
    case CSIE_TIN:          return "ʹ��TIN";
    case CSIE_GOPHER:       return "ʹ��Gopher";
    case MMENU:     return "���˵�";
    case ADMIN:     return NAME_SYS_MANAGE;
    case SELECT:    return "ѡ��������";
    case READBRD:   return "���������";
    case READNEW:   return "�쿴������";
    case  READING:  return "�������";
    case  POSTING:  return "��������" ;
    case MAIL:      return "�ż�ѡ��" ;
    case  SMAIL:    return "������";
    case  RMAIL:    return "������";
    case TMENU:     return "̸��˵����";
    case  LUSERS:   return "��˭������";
    case  FRIEND:   return "�����Ϻ���";
    case  MONITOR:  return "�࿴��";
    case  QUERY:    return "��ѯ" NAME_USER_SHORT;
    case  TALK:     return "����" ;
    case  PAGE:     return "����" NAME_USER_SHORT;
    case  CHAT2:    return "�λù���";
    case  CHAT1:    return CHAT_SERVER "��";
    case  CHAT3:    return "����ͤ";
    case  CHAT4:    return "�ϴ�������";
    case  IRCCHAT:  return "��̸IRC";
    case LAUSERS:   return "̽��" NAME_USER_SHORT;
    case XMENU:     return "ϵͳ��Ѷ";
    case  VOTING:   return "ͶƱ";
    case  BBSNET:   return "����������";
    case  EDITWELC: return "�༭ Welc";
    case EDITUFILE: return "�༭����";
    case EDITSFILE: return NAME_SYS_MANAGE;
    case  EDITSIG:  return "��ӡ";
    case  EDITPLAN: return "��ƻ�";
    case ZAP:       return "����������";
    case EXCE_MJ:   return "Χ������";
    case EXCE_BIG2: return "�ȴ�Ӫ";
    case EXCE_CHESS:return "���Ӻ���";
    case NOTEPAD:   return "���԰�";
    case GMENU:     return "������";
    case FOURM:     return "4m Chat";
    case ULDL:      return "UL/DL" ;
    case MSG:       return NAME_SEND_MSG;
    case USERDEF:   return "�Զ�����";
    case EDIT:      return "�޸�����";
    case OFFLINE:   return "��ɱ��..";
    case EDITANN:   return "���޾���";
    case CCUGOPHER: return "��վ����";
    case LOOKMSGS:  return NAME_VIEW_MSG;
    case WFRIEND:   return "Ѱ������";
    case LOCKSCREEN:return "��Ļ����";
    case WEBEXPLORE:return "Web���";
    default: return "ȥ������!?" ;
    }
}

