#include "bbslib.h"

struct deny
{
	char id[80];
	char exp[80];
	char comment[80];
	time_t free_time;
} denyuser[256];

int denynum=0;

int loaddenyuser(char *board)
{
	FILE *fp;
	char path[80], buf[256];
	char *id, *nick;

	sprintf(path, "boards/%s/deny_users", board);
	fp=fopen(path, "r");
	if(fp==0)
		return;
	while (denynum < (sizeof(denyuser)/sizeof(denyuser[0])))
	{
		if(fgets(buf, sizeof(buf), fp)==0)
			break;
		id = strchr(buf, ' ');
		if (id != NULL)
			*id = '\0';
        strcpy( denyuser[denynum].id, buf );
		strncpy(denyuser[denynum].exp, buf+13, 30);
		nick = strrchr(buf+13, '[');
		if (nick != NULL)
		{
			denyuser[denynum].free_time = atol(nick+1);
			nick--;
			if (nick - buf > 43)
			{
				*nick = '\0';
				strcpy(denyuser[denynum].comment, buf+43);
			}
		}
		denynum++;
	}
	fclose(fp);
}

int main()
{
	int i; 
	char board[80];

	init_all();
   	if(!loginok)
		http_fatal("����δ��¼, ���ȵ�¼");
	strsncpy(board, getparm("board"), 30);
	if(!has_read_perm(currentuser, board))
		http_fatal("�����������");
	if(!has_BM_perm(currentuser, board))
		http_fatal("����Ȩ���б�����");
	loaddenyuser(board);
   	printf("<center>\n");
   	printf("%s -- �����û����� [������: %s]<hr color=\"green\"><br>\n", BBSNAME, board);
   	printf("���湲�� %d �˱���<br>", denynum);
   	printf("<table border=\"1\"><tr><td>���</td><td>�û��ʺ�</td><td>����</td><td>˵��</td><td>����</td></tr>\n");
   	for(i=0; i<denynum; i++)
	{
		printf("<tr><td>%d</td>", i+1);
		printf("<td><a href=\"bbsqry?userid=%s\">%s</a></td>", denyuser[i].id, denyuser[i].id);
		printf("<td>%s</td>\n", nohtml(denyuser[i].exp));
		printf("<td>%s</td>\n", denyuser[i].comment);
		printf("<td>[<a onclick=\"return confirm('ȷʵ�����?')\" href=\"bbsdenydel?board=%s&userid=%s\">���</a>]</td></tr>\n", 
			board, denyuser[i].id);
	}
   	printf("</table><hr color=\"green\">\n");
	printf("[<a href=\"bbsdenyadd?board=%s\">�趨�µĲ���POST�û�</a>]</center>\n", board);
	http_quit();
}
