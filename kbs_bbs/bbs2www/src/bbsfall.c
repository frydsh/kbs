#include "bbslib.h"

int main()
{
	int i; 
	int fnum;
	char filename[STRLEN];
	friends_t *frnds;

	init_all();
   	if(!loginok)
	   	http_fatal("����δ��¼, ���ȵ�¼");
	sethomefile(filename,currentuser->userid, "friends");
	fnum = get_num_records(filename, sizeof(friends_t));
	if (fnum <= 0)
		http_fatal("����δ�趨��������");
	if ((!HAS_PERM(currentuser,PERM_ACCOUNTS) && !HAS_PERM(currentuser,PERM_SYSOP)))
		fnum = (fnum >= MAXFRIENDS) ? MAXFRIENDS : fnum;
	frnds = (friends_t *)calloc(sizeof(friends_t), fnum);
	get_records(filename, frnds, sizeof(friends_t), 1, fnum);
   	printf("<center>\n");
   	printf("%s -- �������� [ʹ����: %s]<hr color=\"green\"><br>\n",
		   	BBSNAME, currentuser->userid);
   	printf("�����趨�� %d λ����<br>", fnum);
   	printf("<table border=\"1\"><tr><td>���</td><td>���Ѵ���</td><td>����˵��</td><td>ɾ������</td></tr>");
   	for(i = 0; i < fnum; i++)
   	{
		printf("<tr><td>%d</td>", i+1);
		printf("<td><a href=\"bbsqry?userid=%s\">%s</a></td>",
			   	frnds[i].id, frnds[i].id);
		printf("<td>%s</td>\n", nohtml(frnds[i].exp));
		printf("<td>[<a onclick=\"return confirm('ȷʵɾ����?')\" href=\"bbsfdel?userid=%s\">ɾ��</a>]</td></tr>", frnds[i].id);
	}
   	printf("</table><hr color=\"green\">\n");
	printf("[<a href=\"bbsfadd\">����µĺ���</a>]</center>\n");
	http_quit();
}
