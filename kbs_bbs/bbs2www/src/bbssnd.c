/*
 * $Id$
 */
#include "bbslib.h"

int main()
{
    FILE *fp;
    char filename[80], dir[80], board[80], title[ARTICLE_TITLE_LEN], buf[80], buf2[80], *content,path[80];
    int r, i, sig;
	int reid;
    struct fileheader x, *oldx;
    bcache_t *brd;
    int local, anony;
    /*int filtered = 0;*/

    initwww_all();
    if (!loginok)
        http_fatal("�Ҵҹ��Ͳ��ܷ������£����ȵ�¼");
    strsncpy(board, getparm("board"), 18);
    strncpy(title, getparm("title"), ARTICLE_TITLE_LEN - 1);
	title[ARTICLE_TITLE_LEN - 1] = '\0';
    /*strsncpy(oldfilename, getparm("refilename"), 80);*/
    brd = getbcache(board);
    if (brd == 0)
        http_fatal("���������������");
    if (brd->flag&BOARD_GROUP)
        http_fatal("���������������");
    strcpy(board, brd->filename);
    for (i = 0; i < strlen(title); i++) {
        if (title[i] <= 27 && title[i] >= -1)
            title[i] = ' ';
    }
    sig = atoi(getparm("signature"));
	reid = atoi(getparm("reid"));
    local = atoi(getparm("outgo")) ? 0 : 1;
    anony = atoi(getparm("anony")) ? 1 : 0;
    content = getparm("text");
    if (title[0] == 0)
        http_fatal("���±���Ҫ�б���");
    sprintf(dir, "boards/%s/.DIR", board);
    if (true == checkreadonly(board) || !haspostperm(getCurrentUser(), board))
        http_fatal("����������Ψ����, ����������Ȩ���ڴ˷�������.");
    if (deny_me(getCurrentUser()->userid, board) && !HAS_PERM(getCurrentUser(), PERM_SYSOP))
        http_fatal("�ܱ�Ǹ, �㱻������Աֹͣ�˱����postȨ��.");
    if (abs(time(0) - *(int *) (u_info->from + 36)) < 6) {
        *(int *) (u_info->from + 36) = time(0);
        http_fatal("���η��ļ������, ����Ϣ���������");
    }
    *(int *) (u_info->from + 36) = time(0);
	if( atoi(getparm("tmpl")) )
   		sprintf(filename, "tmp/%s.tmpl.tmp", getcurruserid());
	else{
   		sprintf(filename, "tmp/%s.%d.tmp", getcurruserid(), getpid());
    	f_append(filename, unix_string(content));
	}

    if(reid > 0){
        int pos;int fd;
        oldx = (struct fileheader*)malloc(sizeof(struct fileheader));

		setbfile(path,board,DOT_DIR);
		fd =open(path,O_RDWR);
		if(fd < 0)http_fatal("�����ļ�������");
		get_records_from_id(fd,reid,oldx,1,&pos);

		close(fd);
        if (pos < 0) {
    		free(oldx);
    		oldx = NULL;
        }
        else
        if (oldx->accessed[1] & FILE_READ)
           http_fatal("���Ĳ��ܻظ�");
    }
    else {
        oldx = NULL;
    }
#ifdef HAVE_BRC_CONTROL
    brc_initial(getCurrentUser()->userid, board, getSession());
#endif
    if (is_outgo_board(board) && local == 0)
        local = 0;
    else
        local = 1;
    /*if (filtered == 1)
		r = post_article(FILTER_BOARD, title, filename, getCurrentUser(), fromhost, sig, local, anony, oldx);
    else*/
    if (brd->flag&BOARD_ATTACH) {
#if USE_TMPFS==1
        snprintf(buf,MAXPATH,"%s/home/%c/%s/%d/upload",TMPFSROOT,toupper(getCurrentUser()->userid[0]),
			getCurrentUser()->userid,utmpent);
#else
        snprintf(buf,MAXPATH,"%s/%s_%d",ATTACHTMPPATH,getCurrentUser()->userid,getSession()->utmpent);
#endif
        r = post_article(board, title, filename, getCurrentUser(), fromhost, sig, local, anony, oldx,buf);
        f_rm(buf);
    }
    else
        r = post_article(board, title, filename, getCurrentUser(), fromhost, sig, local, anony, oldx,NULL);
    if (r < 0)
        http_fatal("�ڲ������޷�����");
#ifdef HAVE_BRC_CONTROL
    brc_update(getCurrentUser()->userid, getSession());
#endif
    if(oldx)
    	free(oldx);
    unlink(filename);
    sprintf(buf, "/bbsdoc.php?board=%s", encode_url(buf2, board, sizeof(buf2)));
    if (!junkboard(board)) {
        getCurrentUser()->numposts++;
		/*  do it in post_article
        write_posts(getCurrentUser()->userid, board, title);
		*/
    }
    redirect(buf);
    return 0;
}
