/*
 * $Id$
 */
#include "bbslib.h"

int main()
{
    FILE *fp;
    char buf[1024], title[80], board[80], dir[80], first_file[80];
    char brdencode[STRLEN];
    bcache_t *x1;
    struct fileheader x, x0;
    int sum = 0, total = 0;
    int title_len;
	int groupid;
	int same;

    init_all();
    strsncpy(board, getparm("board"), 32);
    strsncpy(title, getparm("title"), 42);
	groupid=atoi(getparm("groupid"));
    x1 = getbcache(board);
    if (x1 == 0)
        http_fatal("�����������");
    strcpy(board, x1->filename);
    if (!check_read_perm(currentuser, x1))
        http_fatal("�����������");
    sprintf(dir, "boards/%s/.DIR", board);
    fp = fopen(dir, "r");
    if (fp == 0)
        http_fatal("�����������Ŀ¼");
    encode_url(brdencode, board, sizeof(brdencode));
    sprintf(buf, "bbsman?board=%s&mode=1", brdencode);
    printf("<center>%s -- ͬ������� [������: %s] [���� '%s']<hr class=\"default\">\n", BBSNAME, board, nohtml(title));
    printf("<table border=\"1\"><tr><td>���</td><td>����</td><td>����</td><td>����</td></tr>\n");
    title_len = strlen(title);
    title_len = title_len > 40 ? 40 : title_len;
    while (1) {
        if (fread(&x, sizeof(x), 1, fp) == 0)
            break;
        sum++;
		if( groupid == 0 )
	        same = (!strncmp(title, x.title, title_len) || (!strncmp(title, x.title + 4, title_len) && !strncmp(x.title, "Re: ", 4))) ;
		else same = (x.groupid == groupid);

		if( same ){
            if (total == 0)
			{
                strcpy(first_file, x.filename);
				groupid = x.groupid;
			}
            printf("<tr><td>%d</td>", sum);
            printf("<td>%s</td>", userid_str(x.owner));
            if (!(x.accessed[0] & (FILE_MARKED | FILE_DIGEST))) {
                char buf2[STRLEN];

                snprintf(buf2, sizeof(buf2), "&box%s=on", x.filename);
                if (strlen(buf) < 500)
                    strcat(buf, buf2);
            }
            printf("<td>%6.6s</td>", wwwCTime(get_posttime(&x) + 4));
            printf("<td><a href=\"/bbscon.php?board=%s&id=%d\">%s</a></td></tr>\n", brdencode, x.id, void1(x.title));
            total++;
        }
    }
    fclose(fp);
    printf("</table><hr>\n���ҵ� %d ƪ \n", total);
    printf("<a href=\"/bbsdoc.php?board=%s\">��������</a> ", brdencode);
    if (total > 0) {
        printf("<a href=\"bbstcon?board=%s&gid=%d\">������ȫ��չ��</a> ", brdencode, groupid);
        if (has_BM_perm(currentuser, board))
            printf("<a onclick=\"return confirm('ȷ��ͬ����ȫ��ɾ��?')\" href=\"%s\">ͬ����ɾ��</a>", buf);
    }
    http_quit();
}
