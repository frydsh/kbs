/*
 * $Id$
 */
#include "bbslib.h"

int main()
{
    FILE *fp;
    int i;
    char userid[80], buf[512], path[512], file[512], board[512], title[80] = "";
	struct boardheader *bp;

    initwww_all();
    if (!loginok)
        http_fatal("�Ҵҹ��Ͳ���д�ţ����ȵ�¼");
    if (!can_send_mail())
        http_fatal("�����ܷ����ż�");
    strsncpy(file, getparm("file"), 20);
    strsncpy(title, nohtml(getparm("title")), 50);
    strsncpy(userid, getparm("userid"), 40);
    strsncpy(board, getparm("board"), 40);
	if (board[0] != '\0')
	{
		if ((bp = getbcache(board)) == NULL)
			http_fatal("���������������");
		strcpy(board, bp->filename);
		if (VALID_FILENAME(file) < 0)
			http_fatal("������ļ���");
	}
	else
	{
		if (file[0] != 'M' && file[0])
			http_fatal("������ļ���");
	}
    printf("<center>\n");
    printf("%s -- �����Ÿ� [ʹ����: %s]<hr color=\"green\">\n", BBSNAME, getCurrentUser()->userid);
    printf("<table border=\"1\"><tr><td>\n");
    printf("<form method=\"post\" action=\"bbssndmail?userid=%s\">\n", userid);
    printf("������: &nbsp;%s<br>\n", getCurrentUser()->userid);
    printf("�ż�����: <input type=\"text\" name=\"title\" size=\"40\" maxlength=\"100\" value=\"%s\"><br> ", title);
    printf("������: &nbsp;&nbsp<input type=\"text\" name=\"userid\" value=\"%s\"><br>\n", nohtml(userid));
    printf("ʹ��ǩ���� <select name=\"signature\">\n");
    if (getCurrentUser()->signature == 0)
        printf("<option value=\"0\" selected=\"selected\">��ʹ��ǩ����</option>\n");
    else
        printf("<option value=\"0\">��ʹ��ǩ����</option>\n");
    for (i = 1; i < 6; i++) {
        if (getCurrentUser()->signature == i)
            printf("<option value=\"%d\" selected=\"selected\">�� %d ��</option>\n", i, i);
        else
            printf("<option value=\"%d\">�� %d ��</option>\n", i, i);
    }
    printf("</select>\n");
    printf(" [<a target=\"_balnk\" href=\"/bbssig.php\">�鿴ǩ����</a>] \n");
    printf(" <input type=\"checkbox\" name=\"backup\">����\n");
    printf("<br>\n");
    printf("<textarea name=\"text\" rows=\"20\" cols=\"80\" wrap=\"physical\">\n\n");
    if (file[0])
	{
        int lines = 0;

		if (board[0] != '\0')
		{
			setbfile(path, board, file);
			printf("�� �� %s �Ĵ������ᵽ: ��\n", userid);
		}
		else
		{
			setmailfile(path, getCurrentUser()->userid, file);
			printf("�� �� %s ���������ᵽ: ��\n", userid);
		}
        fp = fopen(path, "r");
        if (fp) {
            for (i = 0; i < 4; i++)
                if (fgets(buf, 500, fp) == 0)
                    break;
            while (1) {
                if (fgets(buf, 500, fp) == 0)
                    break;
                if (!strncmp(buf, ": ��", 4))
                    continue;
                if (!strncmp(buf, ": : ", 4))
                    continue;
                if (!strncmp(buf, "--\n", 3))
                    break;
                if (buf[0] == '\n')
                    continue;
                if (++lines > 10) {
                    printf(": ...................");
                    break;
                }
                if (!strcasestr(buf, "</textarea>"))
                    printf(": %s", buf);
            }
            fprintf(fp, "\n");
            fclose(fp);
        }
    }
    printf("</textarea><br><div align=\"center\">\n");
    printf("<input type=\"submit\" value=\"����\"> ");
    printf("<input type=\"reset\" value=\"���\"></form>\n");
    printf("</div></table>");
    http_quit();
}
