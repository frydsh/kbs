#include "bbslib.h"

#if 0
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
        "��Ļ����ɫ�ʣ�һ��/�任",/* DEF_TITLECOLOR */
        "���������˵�ѶϢ",     /* DEF_ALLMSG */
        "���ܺ��ѵ�ѶϢ",       /* DEF_FRIENDMSG */
        "�յ�ѶϢ��������",     /* DEF_SOUNDMSG */
        "��վ��Ļ�����ѶϢ",   /* DEF_MAILMSG */
        "������ʱʵʱ��ʾѶϢ",/*"���к�����վ��֪ͨ",    DEF_LOGININFORM */
        "�˵�����ʾ������Ϣ",   /* DEF_SHOWSCREEN */
        "��վʱ��ʾʮ������",   /* DEF_SHOWHOT */
        "��վʱ�ۿ����԰�",     /* DEF_NOTEPAD*/
        "����ѶϢ���ܼ�: Enter/Esc", /* DEF_IGNOREMSG */
        "δʹ��", /* DEF_IGNOREANSIX */ /* Leeward 98.01.12 */
/*        "��ֹ����ANSI��չָ��",  DEF_IGNOREANSIX */ /* Leeward 98.01.12 */
        "��վʱ�ۿ���վ����ͳ��ͼ",  /* DEF_SHOWSTATISTIC Haohmaru 98.09.24*/
        "δ�����ʹ�� *", /* DEF_UNREADMARK Luzi 99.01.12 */
        "ʹ��GB���Ķ�", /* DEF_USEGB KCN 99.09.03 */
	"�����Լ���IP", /* DEF_HIDEIP Haohmaru 99.12.18 */
};
#endif

int main()
{
	int i, type;
	unsigned int perm = 1;

	init_all();
	type=atoi(getparm("type"));
	printf("<center>%s -- �޸ĸ��˲��� [ʹ����: %s]<hr color=\"green\">\n",
			BBSNAME, currentuser->userid);
	if(!loginok)
		http_fatal("�Ҵҹ��Ͳ����趨����");
	if(type)
		return read_form();
	printf("<form action=\"bbsparm?type=1\" method=\"post\">\n");
	printf("<table width=\"610\">\n");
	for(i=0; i < 16; i++)
	{
		char *ptr="";

		printf("<tr>\n");
		if(currentuser->userdefine & perm)
			ptr=" checked";
		printf("<td><input type=\"checkbox\" name=\"perm%d\"%s></td><td>%s</td>\n", i, ptr, user_definestr[i]);
		ptr = "";
		if(currentuser->userdefine & (perm << 16))
			ptr=" checked";
		if (i + 16 < NUMDEFINES)
			printf("<td><input type=\"checkbox\" name=\"perm%d\"%s></td><td>%s</td>\n", i+16, ptr, user_definestr[i+16]);
		else
			printf("<td>&nbsp;</td><td>&nbsp;</td>\n");
		printf("</tr>");
		perm = perm << 1;
	}
	printf("</table>");
	printf("<input type=\"submit\" value=\"ȷ���޸�\"></form><br>���ϲ���������telnet��ʽ�²�������\n");
	http_quit();
	return 0;
}

int read_form()
{
	int i, perm=1, def=0;
	char var[100];

	for(i=0; i<NUMDEFINES; i++)
	{
		sprintf(var, "perm%d", i);
		if(strlen(getparm(var))==2)
			def+=perm;
		perm=perm*2;
	}
	currentuser->userdefine=def;
	printf("���˲������óɹ�.<br><a href=\"bbsparm\">���ظ��˲�������ѡ��</a>");
}
