/*
 * $Id$
 */
#define MY_CSS "/bbsold.css"
#include "bbslib.h"

void printmenubegin()
{
    printf("<tr><td nowrap>\n");
}

void printdiv(int n, char *str, char *imgurl)
{
    printmenubegin();
    printf("<img id=\"img%d\" src=\"%s\">", n, imgurl);
    printf("<A href=\"javascript:changemn('%d');\">%s</A></div>\n", n, str);
    printf("<div id=\"div%d\" style=\"display: none\">\n", n);
}

void printmenuend()
{
    printf("</td><td>&nbsp;</td></tr>\n");
}

void printmenu(char* s)
{
    printmenubegin();
    printf("%s",s);
    printmenuend();
}

int show_fav_all(int f, const char *desc, int d)
{
    char * s;
    int i=0,j,k,l;
    if(d==1) {
		printdiv(d, desc, "/images/folder.gif");
		printf("<img src=\"/images/link.gif\"> <a target=\"f3\" href=\"/bbsfav.php?select=-1\">����</a><br>\n");
    }
        //printf("<div id=div%d style=\"display: none\">\n", f+100);
    else
	{
		printf("<img src=\"/images/folder2.gif\"><a target=\"f3\" href=\"/bbsfav.php?select=%d\">%s</a><br>\n", f, desc);
        printf("<div>");
	}
    do{
        SetFav(f);
        s = get_favboard(i);
        j = get_favboard_type(i);
        k = get_favboard_id(i);
        if(s!=NULL) {
            for(l=0;l<((j!=1)?d:d-1);l++) printf("��");
            if(j==1)
                printf("<img src=\"/images/link.gif\"> <a target=\"f3\" href=\"/bbsdoc.php?board=%s\">%s</a><br>\n", s, s);
            else {
                show_fav_all(k, s, d+1);
            }
        }
        i++;
        
    } while(s!=NULL);
    printf("</div>\n");
}

int main()
{
    char buf[256], *ptr;
    int i;

    init_all();
    printf("<style type=\"text/css\">A {color: #000080}</style><br>\n");
    printf("<script src=\"/func.js\"></script>" "<body class=\"dark\" leftmargin=\"5\" topmargin=\"1\" MARGINHEIGHT=\"1\" MARGINWIDTH=\"1\">");
    printf("<table  cellSpacing=0 cellPadding=0 width=\"100%\" border=0><tr><td>");
    printf("<table width=\"100%%\" border=\"0\" cellspacing=\"0\" cellpadding=\"0\">\n");
    printf("<tr><td>");
    if (currentuser == NULL)
        http_fatal("hehe:%d", loginok);
    printf("�û�: <a href=\"/bbsqry.php?userid=%s\" target=\"f3\">%s</a><br>", getcurruserid(), getcurruserid());
    uleveltochar(buf, getcurrusr());
    printf("���: %s<br>\n", buf);
#ifdef ZIXIA
	if (!strcmp("guest",getcurruserid())){
		printf("      <form target=\"_top\" action=\"/bbslogin.php\" method=\"post\" name=\"form1\"> \n");
		printf("      <table CELLPADDING=2 CELLSPACING=2><thead> \n");
		printf("      <th colspan=2>���ٵ�½</th> \n");
		printf("      <thead><tbody> \n");
		printf("      <tr><TD ALIGN=\"RIGHT\" nowrap>�˺�:</TD><TD ><INPUT TYPE=text size=\"10\" name=\"id\"></TD></tr> \n");
		printf("      <tr><TD ALIGN=\"RIGHT\" nowrap>����:</TD><TD ><INPUT TYPE=password size=\"10\" name=\"passwd\" maxlength=\"39\"></TD></tr> \n");
		printf("     <tr><TD align=center colspan=2><input type=submit value=\"�� ¼\" name=submit1></TD></tr> \n ");
		printf("      </tbody></table> </form> \n");
	} else {
		printf("<a href=\"/bbslogout.php\" target=\"_top\">ע�����ε�¼</a><br>\n");
	}
#else
    printf("<a href=\"/bbslogout.php\" target=\"_top\">ע�����ε�¼</a><br>\n");
#endif
    printf("</td></tr><tr><td>\n");
    printf("<hr style=\"color:#2020f0; height:1px\" width=\"84px\" align=\"center\">\n");
    printf("</td></tr></table>");
    printf("</td></tr><tr><td>");
    printf("<table border=0 cellspacing=0 cellpadding=0 align=right width=95%>");
    printf("<img src=\"/images/home.gif\"><a href=\"/mainpage.php\" target=\"f3\">��ҳ����</a><br>\n");
    printf("<img src=\"/images/new.gif\"><a target=\"f3\" href=\"/bbsboa.php?group=0&group2=-2\">�¿�������</a><br>\n");
    printf("<img src=\"/images/link0.gif\"><a target=\"f3\" href=\"bbs0an\">����������</a><br>\n");
    printf("<img src=\"/images/link0.gif\"><a target=\"f3\" href=\"/bbstop10.php\">����ʮ��</a><br>\n");
    /*printf("<img src=\"/images/link0.gif\"><a target=\"f3\" href=\"bbstopb10\">����������</a><br>\n"); */
    if (loginok) {
        int i, mybrdnum = 0;
        const struct boardheader *bptr;
        char *name;

        load_favboard(0);
        show_fav_all(-1, "���˶�����", 1);
        release_favboard();
/*		printdiv(1,"���˶�����","/images/folder.gif");
		load_favboard(0);
		mybrdnum = getfavnum();
 		for(i=0; i<mybrdnum; i++)
		{
			name = get_favboard(i);
			if (name == NULL)
				continue;
			if (get_favboard_type(i))
     		printf("<img src=\"/images/link.gif\">"
				" <a target=\"f3\" href=\"/bbsdoc.php?board=%s\">%s</a><br>\n",
				encode_url(buf, name, sizeof(buf)), name);
			else
     		printf("<img src=\"/images/folder.gif\">"
				" <a target=\"f3\" href=\"bbsfav?select=%d\">%s</a><br>\n",
				get_favboard_id(i), name);
		}
		printf("<img src=\"/images/link.gif\"> <a target=\"f3\" href=\"bbsmybrd\">"
				"Ԥ������</a><br>\n");
		printf("</div>\n");
		release_favboard();*/
    }
    printdiv(2, "����������", "/images/folder.gif");
    for (i = 0; i < SECNUM; i++)
        printf("<img src=\"/images/link.gif\"> <a target=\"f3\" href=\"/bbsboa.php?group=%d\">%s</a><br>\n", i, secname[i][0]);
    printf("</div>\n");
    printmenuend();

    printdiv(3, "̸��˵����", "/images/folder.gif");
    if (loginok) {
        printf("<img src=\"/images/link.gif\"> <a href=\"bbsfriend\" target=\"f3\">���ߺ���</a><br>\n");
    }
    printf("<img src=\"/images/link.gif\"> <a href=\"bbsusr\" target=\"f3\">�����ķ�</a><br>\n");
    printf("<img src=\"/images/link.gif\"> <a href=\"/bbsqry.php\" target=\"f3\">��ѯ����</a><br>\n");
    printf("<img src=\"/images/link.gif\"> <a href=\"/bbssendsms.php\" target=\"f3\">���Ͷ���</a><br>\n");
    if (loginok && currentuser->userlevel & PERM_PAGE) {
        printf("<img src=\"/images/link.gif\"> <a href=\"/bbssendmsg.php\" target=\"f3\">����ѶϢ</a><br>\n");
        printf("<img src=\"/images/link.gif\"> <a href=\"/bbsmsg.php\" target=\"f3\">�鿴����ѶϢ</a><br>\n");
    }
    printf("</div>\n");
    printmenuend();
    ptr = "";
    if (loginok && currentuser->userlevel & PERM_CLOAK)
        ptr = "<img src=\"/images/link.gif\"> <a target=\"f3\" onclick=\"return confirm('ȷʵ�л�����״̬��?')\" href=\"/bbscloak.php\">�л�����</a><br>\n";
    if (loginok) {
        printdiv(4, "���˹�����", "/images/folder.gif");
        printf("			<img src=\"/images/link.gif\"> <a target=\"f3\" href=\"/bbsfillform.html\">��дע�ᵥ</a><br>\n"
               "			<img src=\"/images/link.gif\"> <a target=\"f3\" href=\"bbsinfo\">��������</a><br>\n"
               "			<img src=\"/images/link.gif\"> <a target=\"f3\" href=\"/bbsplan.php\">��˵����</a><br>\n"
               "			<img src=\"/images/link.gif\"> <a target=\"f3\" href=\"/bbssig.php\">��ǩ����</a><br>\n"
               "			<img src=\"/images/link.gif\"> <a target=\"f3\" href=\"bbspwd\">�޸�����</a><br>\n"
               "			<img src=\"/images/link.gif\"> <a target=\"f3\" href=\"bbsparm\">�޸ĸ��˲���</a><br>\n"
               "			<img src=\"/images/link.gif\"> <a target=\"f3\" href=\"/bbsal.php\">ͨѶ¼</a><br>\n"
               "			<img src=\"/images/link.gif\"> <a target=\"f3\" href=\"/bbsrsmsmsg.php\">���Ź�����</a><br>\n"
/*
"			<img src=\"/images/link.gif\"> <a target=\"f3\" href=\"bbsmywww\">WWW���˶���</a><br>\n"
*/
               "			<img src=\"/images/link.gif\"> <a target=\"f3\" href=\"bbsnick\">��ʱ���ǳ�</a><br>\n"
/*"		<img src=\"/images/link.gif\"><a target=\"f3\" href=\"bbsstat\"> ����ͳ��</a><br>"*/
               "			<img src=\"/images/link.gif\"> <a target=\"f3\" href=\"/bbsfall.php\">�趨����</a><br>\n" "			%s</div>", ptr);
        printmenuend();
        printdiv(5, "�����ż���", "/images/folder.gif");
        printf("			<img src=\"/images/link.gif\"> <a target=\"f3\" href=\"/bbsnewmail.php\">�������ʼ�</a><br>\n"
               "			<img src=\"/images/link.gif\"> <a target=\"f3\" href=\"/bbsreadmail.php\">�����ʼ�</a><br>\n"
         	   "			<img src=\"/images/link.gif\"> <a target=\"f3\" href=\"/bbsmailbox_system.php\">ϵͳԤ��������</a><br>\n"
		   "			<img src=\"/images/link.gif\"> <a target=\"f3\" href=\"/bbsmailbox_custom.php\">�Զ�������</a><br>\n");
        if (can_send_mail())
            printf("			<img src=\"/images/link.gif\"> <a target=\"f3\" href=\"/bbspstmail.php\">�����ʼ�</a><br>\n");
        printf("			</div>\n");
        printmenuend();
    }

    printdiv(6, "������", "/images/folder.gif");
    printf("<img src=\"/images/link.gif\"> <a href=\"/bbsstyle.php?s=1\">С����</a><br>\n");
    printf("<img src=\"/images/link.gif\"> <a href=\"/bbsstyle.php?s=0\">������</a><br>\n");
    /*printf("<img src=\"/images/link.gif\"><a target=\"f3\" href=\"bbsadl\">���ؾ�����</a><br>\n"); */
    printf("</div>\n");
    printmenuend();
#ifdef SMTH
	printf("<tr><td nowrap>");
	printf("<img id=\"img7\" src=\"/images/folder.gif\"><A href=\"javascript:changemn('7');\">ˮľ�ؿ�web��</A></div>");
	printf("<div id=\"div7\" style=\"display: none\">");
    printf("<img src=\"/images/link.gif\"> <a href=\"/express/0603/smth_express.htm\" target=\"_blank\">2003��6�º�</a><br>\n");
    printf("<img src=\"/images/link.gif\"> <a href=\"/express/0703/smth_express.htm\" target=\"_blank\">2003��7�º�</a><br>\n");
    printf("<img src=\"/images/link.gif\"> <a href=\"/express/0903/smth_express.htm\" target=\"_blank\">2003��9�º�</a><br>\n");
    printf("</div>\n");
    printmenuend();
#endif
    printf("<div class=\"r\">");
    printmenu("<img src=\"/images/find.gif\"><a href=\"/games/index.html\" target=\"f3\">����������</a><br>\n");
    printmenubegin();
	printf("<table><tr><form action=\"bbssel\" target=\"f3\"><td>&nbsp;&nbsp;"
			"<input type=\"text\" name=\"board\" maxlength=\"20\" "
			"size=\"9\" value=\"ѡ��������\" onmouseover=\"this.select()\"></td></form></tr></table>\n");
    printf("<img src=\"/images/telnet.gif\"><a href=\"telnet:%s\">Telnet��¼</a><br>\n", NAME_BBS_ENGLISH);
    printmenuend();
#ifdef SMTH
    printmenu("<img src=\"/images/telnet.gif\"><a href=\"/data/fterm-smth.zip\">fterm����</a><br>\n");
#endif
    /*if(!loginok) 
       printf("<img src=\"/images/link0.gif\"><a href=\"javascript:void open('bbsreg', '', 'width=620,height=550')\">���û�ע��</a><br>\n"); */
    if (loginok) {
        if (HAS_PERM(currentuser, PERM_LOGINOK) && !HAS_PERM(currentuser, PERM_POST))
            printmenu("<script>alert('�����ڽ�����, �������������ȫվ�������µ�Ȩ��, ��ο�Announce�湫��, ��������sysop��������. ��������, ����Complain���������.')</script>\n");
        if (count_new_mails() > 0)
            printf("<script>alert('�������ż�!')</script>\n");
    }
    /*if(loginok && !(currentuser->userlevel & PERM_LOGINOK) && !has_fill_form()) 
       printf("<a target=\"f3\" href=\"bbsform\">��дע�ᵥ</a><br>\n"); */
    if (loginok && can_enter_chatroom())
        printmenu("<br><a href=\"javascript:openchat()\">[" CHAT_SERVER "]</a>");
    printf("<script>if(isNS4) arrange();if(isOP)alarrangeO();</script></p>");
    printf("</table></td></tr>");
#ifdef SMTH
    printf("<tr><td>");
    printf("<TABLE cellSpacing=0 cellPadding=0 width=\"90%\" border=0>");
    printf("<tr><td>\n");
    if (currentuser->userlevel&PERM_SYSOP)
        printf("<a href=\"/bbsmboard.php\" target=\"f3\">�������</a><br></p>");
    printf("</tr></td></table>\n");
    printf("</td></tr>");
    printf("<tr><td>");
    printf("<TABLE cellSpacing=0 cellPadding=0 width=\"90%\" border=0>");
    printf("<tr><td>\n");
        printf("����վ��������<A HREF=\"http://www.dawning.com.cn/\" TARGET=\"_blank\">��⹫˾</A>�ṩ����֧��<A HREF=\"http://www.dawning.com.cn/\" TARGET=\"_blank\">http://www.dawning.com.cn</A></FONT><br /></P>\n");
	printf("<A HREF=\"http://www.dawning.com.cn/\" TARGET=\"_blank\"><IMG SRC=\"/images/dawning.gif\"/></A>\n");
    printf("</tr></td></table>\n");
    printf("</td></tr>");
#endif
    printf("</table>");
    printf("</body>");
}

int count_new_mails()
{
    struct fileheader x1;
    int n, unread = 0;
    char buf[1024];
    FILE *fp;

    if (!loginok && currentuser->userid[0] == 0)
        return 0;
    sprintf(buf, "%s/mail/%c/%s/.DIR", BBSHOME, toupper(currentuser->userid[0]), currentuser->userid);
    fp = fopen(buf, "r");
    if (fp == 0)
        return unread;
    while (fread(&x1, sizeof(x1), 1, fp) > 0)
        if (!(x1.accessed[0] & FILE_READ))
            unread++;
    fclose(fp);
    return unread;
}
