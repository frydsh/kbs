/*
 * $Id$
 */
#include "bbslib.h"

int main()
{
    FILE *fp;
    char filename[80], dir[80], board[80], title[80], buf[80], oldfilename[80], *content;
    int r, i, sig;
    struct fileheader x, *oldx;
    bcache_t *brd;
    int local, anony;
    /*int filtered = 0;*/

    init_all();
    if (!loginok)
        http_fatal("�Ҵҹ��Ͳ��ܷ������£����ȵ�¼");
    strsncpy(board, getparm("board"), 18);
    strsncpy(title, getparm("title"), 50);
    strsncpy(oldfilename, getparm("refilename"), 80);
    brd = getbcache(board);
    if (brd == 0)
        http_fatal("���������������");
    strcpy(board, brd->filename);
    for (i = 0; i < strlen(title); i++) {
        if (title[i] <= 27 && title[i] >= -1)
            title[i] = ' ';
    }
    sig = atoi(getparm("signature"));
    local = atoi(getparm("outgo")) ? 0 : 1;
    anony = atoi(getparm("anony")) ? 1 : 0;
    content = getparm("text");
    if (title[0] == 0)
        http_fatal("���±���Ҫ�б���");
    sprintf(dir, "boards/%s/.DIR", board);
    if (true == checkreadonly(board) || !haspostperm(currentuser, board))
        http_fatal("����������Ψ����, ����������Ȩ���ڴ˷�������.");
    if (deny_me(currentuser->userid, board) && !HAS_PERM(currentuser, PERM_SYSOP))
        http_fatal("�ܱ�Ǹ, �㱻������Աֹͣ�˱����postȨ��.");
    if (abs(time(0) - *(int *) (u_info->from + 36)) < 6) {
        *(int *) (u_info->from + 36) = time(0);
        http_fatal("���η��ļ������, ����Ϣ���������");
    }
    *(int *) (u_info->from + 36) = time(0);
    sprintf(filename, "tmp/%s.%d.tmp", getcurruserid(), getpid());
    f_append(filename, unix_string(content));
    if(oldfilename[0]){
        int pos;
        oldx = (struct fileheader*)malloc(sizeof(struct fileheader));
        pos = get_file_ent(board, oldfilename, oldx);
        if (pos <= 0) {
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
    brc_initial(currentuser->userid, board);
    if (is_outgo_board(board) && local == 0)
        local = 0;
    else
        local = 1;
    /*if (filtered == 1)
		r = post_article(FILTER_BOARD, title, filename, currentuser, fromhost, sig, local, anony, oldx);
    else*/
    if (brd->flag&BOARD_ATTACH) {
        snprintf(buf,MAXPATH,"%s/%s_%d",ATTACHTMPPATH,currentuser->userid,utmpent);
        r = post_article(board, title, filename, currentuser, fromhost, sig, local, anony, oldx,buf);
        f_rm(buf);
    }
    else
        r = post_article(board, title, filename, currentuser, fromhost, sig, local, anony, oldx,NULL);
    if (r < 0)
        http_fatal("�ڲ������޷�����");
    brc_update(currentuser->userid);
    if(oldx)
    	free(oldx);
    unlink(filename);
    sprintf(buf, "/bbsdoc.php?board=%s", board);
    if (!junkboard(board)) {
        currentuser->numposts++;
        write_posts(currentuser->userid, board, title);
    }
    redirect(buf);
    return 0;
}
