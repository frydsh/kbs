/*
 * $Id$
 */
#include "bbslib.h"

int main()
{
    FILE *fp;
    int i;
    char userid[80], buf[512], buf2[512], path[512], file[512], board[512], title[80] = "",
    		oldfilename[80];
    struct fileheader DirInfo;
    bool attach=false;

    initwww_all();
    if (!loginok)
        http_fatal("�Ҵҹ��Ͳ��ܷ������£����ȵ�¼");
    strsncpy(board, getparm("board"), 20);
    strsncpy(file, getparm("file"), 20);
    strsncpy(title, getparm("title"), 50);
    strsncpy(oldfilename, getparm("refilename"), 80);
    strsncpy(buf, getparm("attach"), 3);
	strsncpy(userid, getparm("userid"), 40);
    if (atoi(buf)!=0)
        attach=true;
    if (title[0] && strncmp(title, "Re: ", 4))
        sprintf(title, "Re: %s", getparm("title"));
    if (file[0]&&(VALID_FILENAME(file) < 0))
        http_fatal("������ļ���");
    if (!haspostperm(getCurrentUser(), board))
        http_fatal("�������������������Ȩ�ڴ���������������");
    if (file[0] != '\0' && !can_reply_post(board, file))
        http_fatal("�����ܻظ�����");
    printf("<center>\n");
    printf("%s -- �������� [ʹ����: %s]<hr color=\"green\">\n", BBSNAME, getCurrentUser()->userid);
    printf("<form name=\"postform\" method=\"post\" action=\"bbssnd?board=%s&refilename=%s\">\n<table border=\"1\">\n", encode_url(buf, board, sizeof(buf)), oldfilename);
    printf("<tr><td>");
    printf("<font color=\"green\">����ע������: <br>\n");
    printf("����ʱӦ���ؿ������������Ƿ��ʺϹ������Ϸ������������ˮ��лл���ĺ�����<br></font></td></tr>\n");
    printf("<tr><td>\n");
    printf("����: %s<br>\n", getCurrentUser()->userid);
    printf("ʹ�ñ���: <input type=\"text\" name=\"title\" size=\"40\" maxlength=\"100\" value=\"%s\">\n", encode_html(buf, void1(title), sizeof(buf)));
    printf("������: [%s]<br>\n", board);
    if (attach)
        printf("<br />\n������<input type=\"text\" name=\"attachname\" size=\"50\" value=\"\" disabled > <br />");
    printf("ʹ��ǩ���� <select name=\"signature\">\n");
    if (getCurrentUser()->signature == 0)
        printf("<option value=\"0\" selected>��ʹ��ǩ����</option>\n");
    else
        printf("<option value=\"0\">��ʹ��ǩ����</option>\n");
    for (i = 1; i < 6; i++) {
        if (getCurrentUser()->signature == i)
            printf("<option value=\"%d\" selected>�� %d ��</option>\n", i, i);
        else
            printf("<option value=\"%d\">�� %d ��</option>\n", i, i);
    }
    printf("</select>\n");
    printf(" [<a target=\"_balnk\" href=\"/bbssig.php\">�鿴ǩ����</a>] \n");
    /* ����ת��֧�� */
    printf("<input type=\"checkbox\" name=\"outgo\" value=\"1\" CHECKED>ת��\n");
    /* ��������������֧�� */
    if (seek_in_file("etc/anonymous", board))
        printf("<input type=\"checkbox\" name=\"anony\" value=\"1\">ʹ������\n");
    printf("<br>\n<textarea name=\"text\" rows=\"20\" cols=\"80\" wrap=\"physical\">\n\n");
    if (file[0]) {
        int lines = 0;

		setbfile(path, board, file);
        fp = fopen(path, "r");
        if (fp) {
			char *ptr;
			char *quser;

			quser = userid;
            fgets(buf, 256, fp);       /* ȡ����һ���� ���������µ� ������Ϣ */
            if ((ptr = strrchr(buf, ')')) != NULL) {    /* ��һ��':'�����һ�� ')' �е��ַ��� */
                ptr[1] = '\0';
                if ((ptr = strchr(buf, ':')) != NULL) {
                    quser = ptr + 1;
                    while (*quser == ' ')
                        quser++;
                }
            }
			printf("\n�� �� %s �Ĵ������ᵽ: ��\n", quser);
            for (i = 0; i < 3; i++) {
                if (skip_attach_fgets(buf, 500, fp) == 0)
                    break;
            }
            while (1) {
                if (skip_attach_fgets(buf, 500, fp) == 0)
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
                    printf(": ...................\n");
                    break;
                }
                /* faint, why check </textarea> */
                if (!strcasestr(buf, "</textarea>"))
                    printf(": %s", buf);
            }
            fclose(fp);
        }
    }
    printf("</textarea></td></tr>\n");
    printf("<tr><td class=\"post\" align=\"center\">\n");
    printf("<input type=\"submit\" value=\"����\"> \n");
    printf("<input type=\"reset\" value=\"���\">\n");
    if (attach) {
        printf("<script language=\"JavaScript\">\n");
        printf("<!--\n");
        printf("   function GoAttachWindow(){     \n");
        printf("    var hWnd = window.open(\"/bbsupload.php\",\"_blank\",\"width=600,height=300,scrollbars=yes\");  \n");
        printf("    if ((document.window != null) && (!hWnd.opener))  \n");
        printf("    hWnd.opener = document.window;  \n");
        printf("    hWnd.focus();  \n");
        printf("    return false;  \n");
        printf("  }  \n");
        printf("-->\n");
        printf("</script>\n");
        printf("<input type=\"button\" value=\"����\" name=\"attach22\" onClick=\"GoAttachWindow()\">");
    }
    printf("</td></tr></table></form>\n");
    http_quit();
}
