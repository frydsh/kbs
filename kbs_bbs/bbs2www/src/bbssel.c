/*
 * $Id$
 */
#include "bbslib.h"

int main()
{
    char *board, buf[80], *board1, *title;
    int i, total = 0;
    boardheader_t *bc;

    init_all();
    board = nohtml(getparm("board"));
    bc = getbcacheaddr();
    if (board[0] == 0) {
        printf("%s -- ѡ��������<hr color=green>\n", BBSNAME);
        printf("<form action=bbssel>\n");
        printf("����������: <input type=text name=board>");
        printf(" <input type=submit value=ȷ��>");
        printf("</form>\n");
    } else {
        for (i = 0; i < MAXBOARD; i++) {
            board1 = bc[i].filename;
            if (!check_read_perm(currentuser, &bc[i]))
                continue;
            if (!strcasecmp(board, board1)) {
                sprintf(buf, "/bbsdoc.php?board=%s", board1);
                redirect(buf);
                http_quit();
            }
        }
        printf("%s -- ѡ��������<hr color=green>\n", BBSNAME);
        printf("�Ҳ������������, ", board);
        printf("������������к���'%s'����������: <br><br>\n", board);
        printf("<table>");
        for (i = 0; i < MAXBOARD; i++) {
            board1 = bc[i].filename;
            title = bc[i].title + 13;
            if (!check_read_perm(currentuser, &bc[i]))
                continue;
            if (strcasestr(board1, board) || strcasestr(title, board) || strcasestr(bc[i].des, board)) {
                total++;
                printf("<tr><td>%d", total);
                printf("<td><a href=/bbsdoc.php?board=%s>%s</a><td>%s<br>\n", board1, board1, title);
				if (total == 1) {
					sprintf(buf, "/bbsdoc.php?board=%s", board1);
				}
            }
        }
        printf("</table><br>\n");
        printf("���ҵ�%d������������������.\n", total);
		if (total == 1) {
			redirect(buf);
		}
    }
	http_quit();
}
