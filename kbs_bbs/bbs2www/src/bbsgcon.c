/*
 * $Id$
 */
#include "bbslib.h"

int main()
{
    FILE *fp;
    char buf[512], board[80], dir[80], file[80], filename[80], *ptr;
    struct fileheader x;
    int num, tmp, total;
    struct boardheader bh;
    char board_url[80];

    initwww_all();
    strsncpy(board, getparm("board"), 32);
    strsncpy(file, getparm("file"), 32);
    num = atoi(getparm("num"));
    printf("<center>\n");
    if (getboardnum(board,&bh) == 0 || !check_read_perm(getCurrentUser(), &bh))
        http_fatal("�����������");
    strcpy(board, getbcache(board)->filename);
    printf("%s -- �����Ķ� [������: %s]<hr class=\"default\">", BBSNAME, board);
    if (VALID_FILENAME(file) < 0)
        http_fatal("����Ĳ���");
    sprintf(dir, "boards/%s/.DIGEST", board);
    total = file_size(dir) / sizeof(x);
    if (total <= 0)
        http_fatal("�������������ڻ���Ϊ��");
    printf("<table width=\"610\" border=\"1\">\n");
    printf("<tr><td>\n<pre>");
    sprintf(filename, "boards/%s/%s", board, file);
    fp = fopen(filename, "r");
    if (fp == 0)
        http_fatal("���Ĳ����ڻ����ѱ�ɾ��");
    while (fgets(buf, 512, fp) != NULL) {
        hhprintf("%s", void1(buf));
    }
    fclose(fp);
    printf("</pre></td></tr>\n</table><hr class=\"default\">\n");
    printf("[<a href=\"/bbssec.php\">����������</a>]");
    printf("[<a href=\"bbsall\">ȫ��������</a>]");
    fp = fopen(dir, "r+");
    if (fp == 0)
        http_fatal("dir error2");
    encode_url(board_url, board, sizeof(board_url));
    if (num > 1) {
        fseek(fp, sizeof(x) * ((num-1) - 1), SEEK_SET);
        fread(&x, sizeof(x), 1, fp);
        printf("[<a href=\"bbsgcon?board=%s&file=%s&num=%d\">��һƪ</a>]", board_url, x.filename, num - 1);
    }
    printf("[<a href=\"/bbsdoc.php?board=%s\">��������</a>]", board_url);
    if (num < total) {
        fseek(fp, sizeof(x) * ((num-1) + 1), SEEK_SET);
        fread(&x, sizeof(x), 1, fp);
        printf("[<a href=\"bbsgcon?board=%s&file=%s&num=%d\">��һƪ</a>]", board_url, x.filename, num + 1);
    }
	/* FIXME: quick & dirty patch */
	fseek(fp, sizeof(x) * (num-1), SEEK_SET);
	fread(&x, sizeof(x), 1, fp);
    fclose(fp);
    ptr = x.title;
    if (!strncmp(ptr, "Re: ", 4))
        ptr += 4;
    printf("[<a href=\"bbstfind?board=%s&title=%s\">ͬ�����Ķ�</a>]\n", board_url, encode_url(buf, void1(ptr), sizeof(buf)));
    printf("</center>\n");
    http_quit();
}
