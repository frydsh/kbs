#define MY_CSS "/bbsold.css"
#include "bbslib.h"

int main() {
	char buf[1024], *ptr;
	int i;

  	init_all();
	printf("<style type=\"text/css\">A {color: #000080} BODY {BACKGROUND-COLOR: #b0e0b0; FONT-SIZE: 14px;}</style><br>\n"
	"<script>"
"	function closebut(x, y) {"
"		if(document.img0) document.img0.src='/folder.gif';"
"		if(document.img1) document.img1.src='/folder.gif';"
"		if(document.img2) document.img2.src='/folder.gif';"
"		if(document.img3) document.img3.src='/folder.gif';"
"		if(document.img4) document.img4.src='/folder.gif';"
"		if(document.img5) document.img5.src='/folder.gif';"
"		if(document.all.div0) document.all.div0.style.display='none';"
"		if(document.all.div1) document.all.div1.style.display='none';"
"		if(document.all.div2) document.all.div2.style.display='none';"
"		if(document.all.div3) document.all.div3.style.display='none';"
"		if(document.all.div4) document.all.div4.style.display='none';"
"		if(document.all.div5) document.all.div5.style.display='none';"
"		x.style.display='block';"
"		y.src='/folder2.gif';"
"	}"
"	function t(x, y) {"
"		if(x.style.display!='none') {"
"			x.style.display='none';"
"			y.src='/folder.gif';"
"		}"
"		else"
"			closebut(x, y);"
"	}"
"	function openchat() {"
"		url='bbschat';"
"		chatWidth=screen.availWidth;"
"		chatHeight=screen.availHeight;"
"		winPara='toolbar=no,location=no,status=no,menubar=no,scrollbars=auto,resizable=yes,left=0,top=0,'+'width='+(chatWidth-10)+',height='+(chatHeight-27);"
"		window.open(url,'_blank',winPara);"
"	}"
"	</script>");
	printf("<nobr>\n");
	if(!loginok) {
                printf("<center>"
"               	<form action=\"bbslogin\" method=\"post\" target=\"_top\"><br>"
"		bbs�û���¼<br>"
"               	�ʺ� <input style=\"height:20px;BACKGROUND-COLOR:e0f0e0\" type=\"text\" name=\"id\" maxlength=\"12\" size=\"8\"><br>"
"               	���� <input style=\"height:20px;BACKGROUND-COLOR:e0f0e0\" type=\"password\" name=\"pw\" maxlength=\"39\" size=\"8\"><br>"
"               	<input style=\"width:72px; height:22px; BACKGROUND-COLOR:b0e0b0\" type=\"submit\" value=\"��¼��վ\">"
"		</center>");
	}
	else
	{
        char buf[256]="δע���û�";
		printf("�û�: <a href=\"bbsqry?userid=%s\" target=\"f3\">%s</a><br>", 
				getcurruserid(), getcurruserid());
		uleveltochar(buf, getcurrusr());
        printf("���: %s<hr style=\"color:2020f0; height:1px\" width=\"84px\" align=\"left\">", buf);
        printf("<a href=\"bbslogout\" target=\"_top\">ע�����ε�¼</a><br>\n");
	}
  	printf("<hr style=\"color:2020f0; height=1px\" width=\"84px\" align=\"left\"><br><img src=\"/link0.gif\"><a href=\"bbsall\" target=\"f3\">��������ҳ</a><br>\n");
        printf("<img src=\"/link0.gif\"><a target=\"f3\" href=\"bbs0an\">����������</a><br>\n");
   	printf("<img src=\"/link0.gif\"><a target=\"f3\" href=\"bbstop10\">����ʮ��</a><br>\n");
	printf("<img src=\"/link0.gif\"><a target=\"f3\" href=\"bbstopb10\">����������</a><br>\n");
	if(loginok)
	{
		int i, mybrdnum=0;
		const struct boardheader  *bptr;
		printf("<img src=\"/folder.gif\" name=\"img0\"><a href=\"javascript: t(document.all.div0, document.img0)\">���˶�����</a><br>\n");
		printf("<div id=\"div0\" style=\"display:none\">\n");
		load_favboard(0);
		mybrdnum = get_favboard_count();
  		for(i=0; i<mybrdnum; i++)
		{
			bptr = getboard(get_favboard(i+1)+1);
			if (bptr == NULL)
				continue;
     		printf("<img src=\"/link.gif\">"
					"<a target=\"f3\" href=\"bbsdoc?board=%s\"> %s</a><br>\n",
					bptr->filename, bptr->filename);
		}
		printf("<img src=\"/link.gif\"><a target=\"f3\" href=\"bbsmybrd\">"
				" Ԥ������</a><br>\n");
		printf("</div>\n");
	}
  	printf(" <img src=\"/folder.gif\" name=\"img1\"><a href=\"javascript: t(document.all.div1, document.img1)\">����������</a><br>"
"		<div id=\"div1\" style=\"display:none\">\n");
	for (i = 0; i < SECNUM; i++)
		printf("<img src=\"/link.gif\"><a target=\"f3\" href=\"bbsboa?%d\"> %s</a><br>",
				i, secname[i][0]);
	printf("</div><img src=\"/folder.gif\" name=\"img2\"><a href=\"javascript: t(document.all.div2, document.img2)\"≯��˵����</a><br>\n");
	printf("<div id=\"div2\" style=\"display:none\">\n");
  	if(loginok) {
		printf("<img src=\"/link.gif\"><a href=\"bbsfriend\" target=\"f3\"> ���ߺ���</a><br>\n");
	}
  	printf("<img src=\"/link.gif\"><a href=\"bbsusr\" target=\"f3\"> �����ķ�</a><br>\n");
  	printf("<img src=\"/link.gif\"><a href=\"bbsqry\" target=\"f3\"> ��ѯ����</a><br>\n");
        if(loginok&&currentuser->userlevel & PERM_PAGE) {
                printf("<img src=\"/link.gif\"><a href=\"bbssendmsg\" target=\"f3\"> ����ѶϢ</a><br>\n");
                printf("<img src=\"/link.gif\"><a href=\"bbsmsg\" target=\"f3\"> �鿴����ѶϢ</a><br>\n");
	}
	printf("</div>\n");
	ptr="";
	if(loginok&&currentuser->userlevel & PERM_CLOAK)
		ptr="<img src=\"/link.gif\"><a target=\"f3\" onclick=\"return confirm('ȷʵ�л�����״̬��?')\" href=\"bbscloak\"> �л�����</a><br>\n";
	if(loginok)
                printf("<img src=\"/folder.gif\" name=\"img3\"><a href=\"javascript: t(document.all.div3, document.img3)\">���˹�����</a><br>"
"			<div id=\"div3\" style=\"display:none\">"
"			<img src=\"/link.gif\"><a target=\"f3\" href=\"bbsinfo\"> ��������</a><br>"
"			<img src=\"/link.gif\"><a target=\"f3\" href=\"bbsplan\"> ��˵����<a><br>"
"			<img src=\"/link.gif\"><a target=\"f3\" href=\"bbssig\"> ��ǩ����<a><br>"
"			<img src=\"/link.gif\"><a target=\"f3\" href=\"bbspwd\"> �޸�����</a><br>"
"			<img src=\"/link.gif\"><a target=\"f3\" href=\"bbsparm\"> �޸ĸ��˲���</a><br>"
"			<img src=\"/link.gif\"><a target=\"f3\" href=\"bbsmywww\"> WWW���˶���</a><br>"
"			<img src=\"/link.gif\"><a target=\"f3\" href=\"bbsnick\"> ��ʱ���ǳ�</a><br>"
"			<img src=\"/link.gif\"><a target=\"f3\" href=\"bbsstat\"> ����ͳ��</a><br>"
"			<img src=\"/link.gif\"><a target=\"f3\" href=\"bbsfall\"> �趨����</a><br>"
"			%s</div>"
"			<img src=\"/folder.gif\" name=\"img5\"><a href=\"javascript: t(document.all.div5, document.img5)\">�����ż���</a><br>"
"			<div id=\"div5\" style=\"display:none\">"
"			<img src=\"/link.gif\"><a target=\"f3\" href=\"bbsnewmail\"> �������ʼ�</a><br>"
"			<img src=\"/link.gif\"><a target=\"f3\" href=\"bbsmail\"> �����ʼ�</a><br>"
"			<img src=\"/link.gif\"><a target=\"f3\" href=\"bbspstmail\"> �����ʼ�</a><br>"
"			</div>", ptr);
	printf("<img src=\"/folder.gif\" name=\"img4\">");
	printf("<a href=\"javascript: t(document.all.div4, document.img4)\">�ر������</a><br>\n");
	printf("<div id=\"div4\" style=\"display:none\">\n");
	printf("<img src=\"/link.gif\"><a target=\"f3\" href=\"bbsalluser\">����ʹ����</a><br>\n");
	printf("<img src=\"/link.gif\"><a target=\"f3\" href=\"bbsadl\">���ؾ�����</a><br>\n");
	printf("</div>\n");
  	printf("<img src=\"/link0.gif\"><a href=\"bbsfind\" target=\"f3\">���²�ѯ</a><br>\n");
	printf("<img src=\"/link0.gif\"><a href=\"bbssel\" target=\"f3\">����������</a><br>\n");
	printf("<img src=\"/telnet.gif\"><a href=\"telnet:%s\">Telnet��¼</a><br>\n", NAME_BBS_ENGLISH);
	if(!loginok) 
		printf("<img src=\"/link0.gif\"><a href=\"javascript:void open('bbsreg', '', 'width=620,height=550')\">���û�ע��</a><br>\n");
   	if(loginok) {
		if(HAS_PERM(currentuser,PERM_LOGINOK) && !HAS_PERM(currentuser,PERM_POST))
			printf("<script>alert('���������ȫվ�������µ�Ȩ��, ��ο�Announce�湫��, ��������sysop��������. ��������, ����appeal���������.')</script>\n");
		if(count_new_mails()>0) 
			printf("<script>alert('�������ż�!')</script>\n");
	}
	if(loginok && !(currentuser->userlevel & PERM_LOGINOK) && !has_fill_form()) 
		printf("<a target=\"f3\" href=\"bbsform\">��дע�ᵥ</a><br>\n");
	if(loginok) printf("<br><a href=\"javascript:openchat()\">[����㳡<font color=\"red\">������</font>]</a>");
  	printf("</body>");
}

int count_new_mails() {
        struct fileheader x1;
        int n, unread=0;
	char buf[1024];
        FILE *fp;
        if(!loginok&&currentuser->userid[0]==0) return 0;
        sprintf(buf, "%s/mail/%c/%s/.DIR", BBSHOME, toupper(currentuser->userid[0]), currentuser->userid);
        fp=fopen(buf, "r");
        if(fp==0)
			return unread;
        while(fread(&x1, sizeof(x1), 1, fp)>0)
                if(!(x1.accessed[0] & FILE_READ)) unread++;
        fclose(fp);
	return unread;
}
