#include "bbslib.h"

int cmpfnames2(char *userid, struct friends *uv)
{
	return !strcasecmp(userid, uv->id);
}

int deleteoverride2(char *uident)
{
    int deleted;
    struct friends fh;
	char buf[STRLEN];

    sethomefile( buf, currentuser->userid,"friends" );
    deleted = search_record( buf, &fh, sizeof(fh), cmpfnames2, uident );
    if(deleted > 0)
    {
        if(delete_record(buf, sizeof(fh), deleted) != -1)
            getfriendstr();
        else
        {
            deleted=-1;
            report("delete friend error");
        }
    }
    return (deleted>0)?1:-1;
}

int main()
{
   	FILE *fp;
   	int i, total=0;
	char userid[80];

	init_all();
   	if(!loginok)
	   	http_fatal("����δ��¼�����ȵ�¼");
	getfriendstr();
   	printf("<center>%s -- �������� [ʹ����: %s]<hr color=\"green\">\n",
		   	BBSNAME, currentuser->userid);
	strsncpy(userid, getparm("userid"), 13);
	if(userid[0] == 0)
   	{
		printf("<form action=\"bbsfdel\">\n");
		printf("��������ɾ���ĺ����ʺ�: <input type=\"text\"><br>\n");
		printf("<input type=\"submit\">\n");
		printf("</form>");
		http_quit();
	}
	if(get_friends_num() <= 0)
	   	http_fatal("��û���趨�κκ���");
   	if(!isfriend(userid))
	   	http_fatal("���˱����Ͳ�����ĺ���������");
	if (deleteoverride2(userid) == -1)
	   	http_fatal("�Ӻ���������ɾ�����û�ʧ��");
   	printf("[%s]�Ѵ����ĺ���������ɾ��.<br>\n <a href=\"bbsfall\">���غ�������</a>", userid);
	http_quit();
}
