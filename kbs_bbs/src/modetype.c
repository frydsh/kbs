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
    case  BBSNET:   return "��������";
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
    case WWW:       return "���� WWW";
    case CCUGOPHER: return "��վ����";
    case LOOKMSGS:  return NAME_VIEW_MSG;
    case WFRIEND:   return "Ѱ������";
    case LOCKSCREEN:return "��Ļ����";
    default: return "ȥ������!?" ;
    }
}

