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

int savedenyuser(char *board)
{
	FILE *fp;
	int i;
	char path[80], buf[256], *exp;

	sprintf(path, "boards/%s/deny_users", board);
	fp=fopen(path, "w");
	if(fp==0)
		return;
	for(i=0; i<denynum; i++)
	{
		int m;
		exp=denyuser[i].exp;
		if(denyuser[i].id[0]==0)
			continue;
		for(m=0; exp[m]; m++)
		{
			if(exp[m]<32 && exp[m]>0)
				exp[m]='.';
		}
		fprintf(fp, "%-12.12s %-30.30s%s\x1b[%um\n", denyuser[i].id,
			   	denyuser[i].exp, denyuser[i].comment, denyuser[i].free_time);
	}
	fclose(fp);
}

int main()
{
	int i; 
	char board[80], *userid;

	init_all();
   	if(!loginok)
	   	http_fatal("����δ��¼, ���ȵ�¼");
	strsncpy(board, getparm("board"), 30);
	if(!has_read_perm(currentuser, board))
	   	http_fatal("�����������");
	if(!has_BM_perm(currentuser, board))
	   	http_fatal("����Ȩ���б�����");
	loaddenyuser(board);
	userid = getparm("userid");
   	for(i=0; i<denynum; i++)
   	{
		if(!strcasecmp(denyuser[i].id, userid))
	   	{
			denyuser[i].id[0]=0;
			savedenyuser(board);
			printf("�Ѿ��� %s ���. <br>\n", userid);
			inform(board, userid);
			printf("[<a href=\"bbsdenyall?board=%s\">���ر�������</a>]", board);
			http_quit();
		}
	}
	http_fatal("����û����ڱ���������");
	http_quit();
	return 0;
}

int inform(char *board, char *user)
{
    FILE* fn1;
    char filename[STRLEN];
    char buffer[STRLEN];
    time_t now;
    struct userec* lookupuser;
	struct userec* usr;
	postinfo_t pi;

    now = time(0);
	usr = getcurrusr();
	bzero(&pi, sizeof(pi));
    /*Haohmaru.4.1.�Զ�����֪ͨ*/
    sprintf(filename,"etc/%s.dny", usr->userid);
    fn1 = fopen(filename,"w");
	sprintf(buffer,"[֪ͨ]");
	fprintf(fn1,"������: %s \n",usr->userid) ;
	fprintf(fn1,"��  ��: %s\n",buffer) ;
	fprintf(fn1,"����վ: %s (%24.24s)\n",
			"BBS "NAME_BBS_CHINESE"վ",ctime(&now)) ;
	fprintf(fn1,"��  Դ: %s \n",usr->lasthost) ;
	fprintf(fn1,"\n");
    if (HAS_PERM(currentuser,PERM_SYSOP) || HAS_PERM(currentuser,PERM_OBOARDS))
        fprintf(fn1,"����վ����Ա %s ����� %s ��ķ��\n",usr->userid,board);
    else
        fprintf(fn1,"���� %s ����� %s ������\n",board,usr->userid);
    fclose(fn1);
    mail_file(getcurruserid(),filename, user, buffer,0);

    /*���ͬ�����ĵ�undenypost��  Bigman:2000.6.30*/ 
    getuser(user, &lookupuser); 
    if (PERM_BOARDS & lookupuser->userlevel)
    	sprintf(buffer,"%s ���ĳ����� %s �� %s ", usr->userid, user, board); 
    else
        sprintf(buffer,"%s ��� %s �� %s", usr->userid, user, board);
	pi.userid = usr->userid;
	pi.username = usr->username;
	pi.title = buffer;
	pi.board = "undenypost";
	pi.local = 1;
	pi.anony = 0;
	pi.access = 0;
	post_file(filename, &pi);
    unlink(filename);

	printf("ϵͳ�Ѿ�����֪ͨ��%s.<br>\n", user);
}

