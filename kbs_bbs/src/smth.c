#include "bbs.h"

char seccode[SECNUM][5]={
	"0", "1", "3", "4", "5", "6", "7", "8", "9"
};

char secname[SECNUM][2][20]={
	"BBS ϵͳ", "[վ��]",
	"�廪��ѧ", "[��У]",
	"���Լ���", "[����/ϵͳ]",
	"��������", "[����/����]",
	"�Ļ�����", "[�Ļ�/����]",
	"�����Ϣ", "[���/��Ϣ]",
	"ѧ����ѧ", "[ѧ��/����]",
	"��������", "[�˶�/����]",
	"֪�Ը���", "[̸��/����]",
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
uleveltochar( char *buf, struct userec *lookupuser ) /* ȡ�û�Ȩ������˵�� Bigman 2001.6.24*/
{
	unsigned lvl;
	char userid[IDLEN+2];
	
	lvl = lookupuser->userlevel;
	strncpy( userid, lookupuser->userid, IDLEN+2 );

    if( !(lvl &  PERM_BASIC) ) {
	strcpy( buf, "����");
        return 0;
    }
/*    if( lvl < PERM_DEFAULT )
    {
        strcpy( buf, "- --" );
        return 1;
    }
*/

    /* Bigman: �������Ĳ�ѯ��ʾ 2000.8.10 */
    /*if( lvl & PERM_ZHANWU ) strcpy(buf,"վ��");*/
    if( (lvl & PERM_ANNOUNCE) && (lvl & PERM_OBOARDS) ) strcpy(buf,"վ��");
    else if  (lvl & PERM_JURY) strcpy(buf, "�ٲ�");/* stephen :�������Ĳ�ѯ"�ٲ�" 2001.10.31 */
    else  if( lvl & PERM_CHATCLOAK ) strcpy(buf,"Ԫ��");
    else if (lvl & PERM_CHATOP) strcpy(buf,"ChatOP");
    else if  ( lvl & PERM_BOARDS ) strcpy(buf,"����");
    else  if( lvl & PERM_HORNOR ) strcpy(buf,"����");
	/* Bigman: �޸���ʾ 2001.6.24 */
	else if (lvl & (PERM_LOGINOK))
	{
		if (!(lvl & (PERM_CHAT)) || !(lvl & (PERM_PAGE)) || !(lvl & (PERM_POST)) ||(lvl & (PERM_DENYMAIL)) ||(lvl &(PERM_DENYPOST))) strcpy(buf,"����");	
		else strcpy(buf,"�û�");
	}
	else if (!(lvl & (PERM_CHAT)) && !(lvl & (PERM_PAGE))  && !(lvl & (PERM_POST))) strcpy(buf,"����");
	else strcpy(buf,"����");

/*    else {
        buf[0] = (lvl & (PERM_SYSOP)) ? 'C' : ' ';
        buf[1] = (lvl & (PERM_XEMPT)) ? 'L' : ' ';
        buf[2] = (lvl & (PERM_BOARDS)) ? 'B' : ' ';
        buf[3] = (lvl & (PERM_DENYPOST)) ? 'p' : ' ';
        if( lvl & PERM_ACCOUNTS ) buf[3] = 'A';
        if( lvl & PERM_SYSOP ) buf[3] = 'S'; 
        buf[4] = '\0';
    }
*/

    return 1;
}
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

/* rrr - This is separated so I can suck it into the IRC source for use
   there too */

#include "modes.h"

char *
ModeType(mode)
int     mode;
{
    switch(mode) {
    case IDLE:      return "" ;
    case NEW:       return "��վ��ע��" ;
    case LOGIN:     return "���뱾վ";
    case CSIE_ANNOUNCE:     return "��ȡ����";
    case CSIE_TIN:          return "ʹ��TIN";
    case CSIE_GOPHER:       return "ʹ��Gopher";
    case MMENU:     return "���˵�";
    case ADMIN:     return "ϵͳά��";
    case SELECT:    return "ѡ��������";
    case READBRD:   return "���������";
    case READNEW:   return "�Ķ�������";
    case  READING:  return "�Ķ�����";
    case  POSTING:  return "��������" ;
    case MAIL:      return "�ż�ѡ��" ;
    case  SMAIL:    return "������";
    case  RMAIL:    return "������";
    case TMENU:     return "̸��˵����";
    case  LUSERS:   return "��˭������";
    case  FRIEND:   return "�����Ϻ���";
    case  MONITOR:  return "�࿴��";
    case  QUERY:    return "��ѯ����";
    case  TALK:     return "����" ;
    case  PAGE:     return "��������" ;
    case  CHAT2:    return "�λù���";
    case  CHAT1:    return "��������";
    case  CHAT3:    return "����ͤ";
    case  CHAT4:    return "�ϴ�������";
    case  IRCCHAT:  return "��̸IRC";
    case LAUSERS:   return "̽������";
    case XMENU:     return "ϵͳ��Ѷ";
    case  VOTING:   return "ͶƱ";
    case  BBSNET:   return "��������";
    case  EDITWELC: return "�༭ Welc";
    case EDITUFILE: return "�༭����";
    case EDITSFILE: return "ϵͳ����";
        /*        case  EDITSIG:  return "��ӡ";
                case  EDITPLAN: return "��ƻ�";*/
    case ZAP:       return "����������";
    case EXCE_MJ:   return "Χ������";
    case EXCE_BIG2: return "�ȴ�Ӫ";
    case EXCE_CHESS:return "���Ӻ���";
    case NOTEPAD:   return "���԰�";
    case GMENU:     return "������";
    case FOURM:     return "4m Chat";
    case ULDL:      return "UL/DL" ;
    case MSG:       return "��ѶϢ";
    case USERDEF:   return "�Զ�����";
    case EDIT:      return "�޸�����";
    case OFFLINE:   return "��ɱ��..";
    case EDITANN:   return "���޾���";
    case WEBEXPLORE: return "Web���";
    case CCUGOPHER: return "��վ����";
    case LOOKMSGS:  return "�쿴ѶϢ";
    case WFRIEND:   return "Ѱ������";
    case LOCKSCREEN:return "��Ļ����";
    default: return "ȥ������!?" ;
    }
}

