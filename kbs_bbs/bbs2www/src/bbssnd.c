#include "bbslib.h"
 
int write_posts(char *id, char *board, char *title)
{
    char *ptr;
    time_t now;
    struct
    {
        char author[IDLEN + 1];
        char board[IDLEN + 6];
        char title[66];
        time_t date;
        int number;
    } postlog, pl;

    if(junkboard(board)||normal_board(board)!=1||!strcmp(board,"blame"))
        return ;
    now = time(0) ;
    strcpy(postlog.author, id);
    strcpy(postlog.board, board);
    ptr = title;
    if (!strncmp(ptr, "Re: ", 4))
        ptr += 4;
    strncpy(postlog.title, ptr, 65);
    postlog.date = now;
    postlog.number = 1;

    { /* added by Leeward 98.04.25 */
        char buf[STRLEN];
        int  log = 1;
        FILE *fp = fopen(".Xpost", "r");

        if (fp)
        {
            while (!feof(fp))
            {
                fread(&pl, sizeof(pl), 1, fp);
                if (feof(fp)) break;

                if (!strcmp(pl.title, postlog.title)
                        && !strcmp(pl.author, postlog.author)
                        && !strcmp(pl.board, postlog.board))
                {
                    log = 0;
                    break;
                }
            }
            fclose(fp);
        }

        if (log)
        {
            append_record(".Xpost", &postlog, sizeof(postlog));
            append_record(".post", &postlog, sizeof(postlog));
        }
    }

    append_record(".post.X", &postlog, sizeof(postlog));
}

int main()
{
   	FILE *fp;
	char filename[80], dir[80], board[80], title[80], buf[80], *content;
	int r, i, sig;
	struct fileheader x;
	bcache_t *brd;
	int local, anony;

   	init_all();
	if(!loginok)
		http_fatal("�Ҵҹ��Ͳ��ܷ������£����ȵ�¼");
   	strsncpy(board, getparm("board"), 18);
   	strsncpy(title, getparm("title"), 50);
 	brd=getbcache(board);
	if(brd==0)
		http_fatal("���������������");
	strcpy(board, brd->filename);
  	for(i=0; i<strlen(title); i++)
	{
		if(title[i]<=27 && title[i]>=-1)
			title[i]=' ';
	}
   	sig = atoi(getparm("signature"));
	local = atoi(getparm("outgo")) ? 0 : 1;
	anony = atoi(getparm("anony")) ? 1 : 0;
   	content=getparm("text");
   	if(title[0]==0)
      	http_fatal("���±���Ҫ�б���");
	sprintf(dir, "boards/%s/.DIR", board);
	if(YEA == checkreadonly(board) || !haspostperm(currentuser, board))
		http_fatal("����������Ψ����, ����������Ȩ���ڴ˷�������.");
	if(deny_me(currentuser->userid,board) && !HAS_PERM(currentuser,PERM_SYSOP))
		http_fatal("�ܱ�Ǹ, �㱻������Աֹͣ�˱����postȨ��.");
	if(abs(time(0) - *(int*)(u_info->from+36))<6)
	{
		*(int*)(u_info->from+36)=time(0);
		http_fatal("���η��ļ������, ����Ϣ���������");
	}
	*(int*)(u_info->from+36)=time(0);
	sprintf(filename, "tmp/%d.tmp", getpid());
	f_append(filename, content);
	r = post_article(board, title, filename, currentuser, fromhost, sig, local, anony);
	if(r<=0)
		http_fatal("�ڲ������޷�����");
	sprintf(buf, "M.%d.A", r);
	brc_initial(currentuser->userid, board);
	brc_add_read(buf);
	brc_update(currentuser->userid);
	unlink(filename);
	sprintf(buf, "bbsdoc?board=%s", board);
	if(!junkboard(board))
	{
		currentuser->numposts++;
		write_posts(currentuser->userid, board, title);
	}
	redirect(buf);
}

