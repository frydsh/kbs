#include "bbslib.h"

int addtooverride2(char *uident, char *exp)
{
    friends_t tmp;
    int  n;
    char buf[STRLEN];

    memset(&tmp, 0, sizeof(tmp));
    sethomefile(buf,currentuser->userid, "friends" );
    if((!HAS_PERM(currentuser,PERM_ACCOUNTS) && !HAS_PERM(currentuser,PERM_SYSOP))
		   	&& (get_num_records(buf, sizeof(struct friends)) >= MAXFRIENDS) )
    {
        hprintf("��Ǹ����վĿǰ�������趨 %d ������.", MAXFRIENDS);
        return -1;
    }
    if( myfriend( searchuser(uident) , NULL) )
        return -1;
	strsncpy(tmp.id, uident, sizeof(tmp.id));
	strsncpy(tmp.exp, exp, sizeof(tmp.exp));
    n=append_record(buf, &tmp, sizeof(friends_t));
    if(n != -1)
        getfriendstr();
    else
        report("append friendfile error");
    return n;
}

int main()
{
   	FILE *fp;
	char path[80], userid[80], exp[80];
	struct userec *x = NULL;
	int rv;

	init_all();
   	if(!loginok)
	   	http_fatal("����δ��¼�����ȵ�¼");
	sethomefile(path, currentuser->userid,"friends");
   	printf("<center>%s -- �������� [ʹ����: %s]<hr color=\"green\">\n", BBSNAME, currentuser->userid);
	strsncpy(userid, getparm("userid"), 13);
	strsncpy(exp, getparm("exp"), 32);
	if(userid[0]==0 || exp[0]==0)
   	{
		if(userid[0])
		   	printf("<font color=\"red\">���������˵��</font>");
		printf("<form action=\"bbsfadd\">\n");
		printf("������������ĺ����ʺ�: <input type=\"text\" name=\"userid\" value=\"%s\"><br>\n",
			userid);
		printf("�������������ѵ�˵��: <input type=\"text\" name=\"exp\">\n", 
			exp);
		printf("<br><input type=\"submit\" value=\"ȷ��\"></form>\n");
		http_quit();
	}
	getuser(userid, &x);
	if(x == NULL)
	   	http_fatal("�����ʹ�����ʺ�");
	rv = addtooverride2(x->userid, exp);
	if (rv == -1)
		http_fatal("���ĺ��������Ѵﵽ����, ��������µĺ���");
	else if (rv == -2)
		http_fatal("�����Ѿ�����ĺ�����������");
	else if (rv == -3)
		http_fatal("�������������ʧ��");

   	printf("[%s]�Ѽ������ĺ�������.<br>\n <a href=bbsfall>���غ�������</a>",
			userid);
	http_quit();
}
