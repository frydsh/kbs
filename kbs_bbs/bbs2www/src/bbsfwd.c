#include "bbslib.h"

/*void debug_abort(int signo)
{
	abort();
}*/

int main()
{
	struct fileheader *x;
	char board[80], file[80], target[80];
	struct userec *u = NULL;
	int big5;
	int noansi;

	init_all();
	strsncpy(board, getparm("board"), 30);
	strsncpy(file, getparm("file"), 30);
	strsncpy(target, getparm("target"), 30);
	big5 = atoi(getparm("big5"));
	noansi = atoi(getparm("noansi"));
	if(!loginok)
		http_fatal("�Ҵҹ��Ͳ��ܽ��б������");
	if(!has_read_perm(currentuser, board))
		http_fatal("�����������");
	x = get_file_ent(board, file);
	if(x==0)
		http_fatal("������ļ���");
	printf("<center>%s -- ת��/�Ƽ������� [ʹ����: %s]<hr color=\"green\">\n", BBSNAME, currentuser->userid);
	if(target[0])
	{
		if(!strchr(target, '@'))
		{
			if(getuser(target, &u) == 0)
				http_fatal("�����ʹ�����ʺ�");
			strcpy(target, u->userid);
			big5 = 0;
			noansi = 0;
		}
		return do_fwd(x, board, target, big5, noansi);
	}
	printf("<table><tr><td>\n");
	printf("���±���: %s<br>\n", nohtml(x->title));
	printf("��������: %s<br>\n", x->owner);
	printf("ԭ������: %s<br>\n", board);
	printf("<form action=\"bbsfwd\" method=\"post\">\n");
	printf("<input type=\"hidden\" name=\"board\" value=\"%s\">", board);
	printf("<input type=\"hidden\" name=\"file\" value=\"%s\">", file);
	printf("������ת�ĸ� <input type=\"text\" name=\"target\" size=\"30\" maxlength=\"30\" value=\"%s\"> (������Է���id��email��ַ). <br>\n",
		currentuser->email);
	printf("<input type=\"checkbox\" name=\"big5\" value=\"1\"> ʹ��BIG5��<br>\n");
	printf("<input type=\"checkbox\" name=\"noansi\" value=\"1\" checked> ����ANSI���Ʒ�<br>\n");
	printf("<input type=\"submit\" value=\"ȷ��ת��\"></form>");
	return 0;
}

int do_fwd(struct fileheader *x, char *board, char *target, int big5, int noansi)
{
	char title[512], path[200];
	int rv;

	sprintf(path, "boards/%s/%s", board, x->filename);
	if(!file_exist(path))
		http_fatal("�ļ������Ѷ�ʧ, �޷�ת��");
	sprintf(title, "%.50s(ת��)", x->title);
	if (!strchr(target, '@'))
	{
		/*post_mail(target, title, path, currentuser->userid,
				currentuser->username, fromhost, -1);*/
		mail_file(getcurruserid(), path, target, title, 0);
		printf("������ת�ĸ�'%s'<br>\n", nohtml(target));
		rv = 0;
	}
	else
	{
		if (big5 == 1)
			conv_init();
		if (bbs_sendmail(path, title, target, 0, big5, noansi) == 0)
		{
			printf("������ת�ĸ�'%s'<br>\n", nohtml(target));
			rv = 0;
		}
		else
		{
			printf("ת��ʧ��\n", nohtml(target));
			rv = -1;
		}
	}
	printf("[<a href=\"javascript:history.go(-2)\">����</a>]");

	return rv;
}
