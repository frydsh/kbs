/*
 * $Id$
 */
#include "bbslib.h"

int main()
{
    char board[80], start[80], buf[256];
    struct boardheader bh;

    initwww_all();
    strsncpy(board, getparm("board"), 32);
    strsncpy(start, getparm("start"), 32);
    if (!loginok)
        http_fatal("�Ҵҹ����޷�ִ�д������, ���ȵ�¼");
    if (getboardnum(board,&bh) == 0 || !check_read_perm(getCurrentUser(), &bh))
        http_fatal("�����������");
    if (strcmp(getCurrentUser()->userid,"guest")) {
#ifdef HAVE_BRC_CONTROL
        brc_initial(getCurrentUser()->userid, board, getSession());
        brc_clear(getSession());
        brc_update(getCurrentUser()->userid, getSession());
#endif
    }
    strcpy(buf, board);
    encode_url(board, buf, sizeof(board));
    sprintf(buf, "/bbsdoc.php?board=%s&start=%s", board, start);
    refreshto(buf, 0);
    http_quit();
}
