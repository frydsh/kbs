/*
 * $Id$
 */
#include "bbslib.h"

#define RECORDS_NUM 3

int main()
{
    FILE *fp;
    char buf[512], board[80], dir[80], filename[80], *ptr;
    char buf2[512], buf3[512];
    fileheader_t records[RECORDS_NUM];
    int num, tmp, total;
    struct userec *user = NULL;
    struct boardheader *bp;
    int rid;
    int fd;

    init_all();
    strsncpy(board, getparm("board"), 32);
    rid = atoi(getparm("id"));
    num = atoi(getparm("num"));
    printf("<center>\n");
    bp = getbcache(board);
    if (!check_read_perm(currentuser, bp))
        http_fatal("�����������");
    if (rid > bp->nowid)
        http_fatal("���Ĳ�����");
    strcpy(board, bp->filename);
    setbdir(DIR_MODE_NORMAL, dir, board);
    total = file_size(dir) / sizeof(fileheader_t);
    printf("%s -- �����Ķ� [������: %s]<hr color=\"green\">", BBSNAME, board);
    if (total <= 0)
        http_fatal("�������������ڻ���Ϊ��");
    fd = open(dir, O_RDWR, 0644);
    if (get_records_from_id(fd, rid, &records, RECORDS_NUM) == 0)
        http_fatal("���Ĳ�����");
    printf("<table width=\"610\" border=\"1\">\n");
    printf("<tr><td>\n<pre>");
    setbfile(filename, board, records[1].filename);
    fp = fopen(filename, "r");
    if (fp == NULL)
        http_fatal("���Ĳ����ڻ����ѱ�ɾ��");
    while (1) {
        char *id, *s;

        if (fgets(buf, 512, fp) == 0)
            break;
        if (!strncmp(buf, "������: ", 8)) {
            ptr = strdup(buf);
            id = strtok(ptr + 8, " ");
            s = strtok(0, "");
            if (id == 0)
                id = " ";
            if (s == 0)
                s = "\n";
            if (strlen(id) < 13 && getuser(id, &user)) {
                printf("������: %s", userid_str(id));
                hprintf("%s", s);
                free(ptr);
                continue;
            }
            free(ptr);
        }
        if (!strncmp(buf, ": ", 2))
            printf("<font color=\"#008080\"><i>");
        hhprintf("%s", buf);
        if (!strncmp(buf, ": ", 2))
            printf("</i></font>");
    }
    fclose(fp);

    if ((loginok) && strcmp(currentuser->userid, "guest")) {
        brc_initial(currentuser->userid, board);
        brc_add_read(records[1].id);
        brc_update(currentuser->userid);
    }
    encode_url(buf2, board, sizeof(buf2));
    printf("</pre></td></tr>\n</table><hr>\n");
    printf("[<a href=\"bbsfwd?board=%s&file=%s\">ת��/�Ƽ�</a>]", buf2, records[1].filename);
    printf("[<a href=\"bbsccc?board=%s&file=%s\">ת��</a>]", buf2, records[1].filename);
    if (can_delete_post(bp, &records[1], getcurrusr()))
        printf("[<a onclick=\"return confirm('�����Ҫɾ��������?')\" href=\"bbsdel?board=%s&file=%s\">ɾ������</a>]", buf2, records[1].filename);
    if (can_edit_post(bp, &records[1], getcurrusr())) {
        printf("[<a href=\"bbsedit?board=%s&file=%s\">�޸�����</a>]", buf2, records[1].filename);
    }
    if (records[0].id != 0) {
        printf("[<a href=\"bbscon?board=%s&id=%d&num=%d\">��һƪ</a>]", buf2, records[0].id, num - 1);
    }
    printf("[<a href=\"/bbsdoc.php?board=%s&page=%d\">��������</a>]", buf2, (num + BBS_PAGE_SIZE - 1) / BBS_PAGE_SIZE);
    if (records[2].id != 0) {
        printf("[<a href=\"bbscon?board=%s&id=%d&num=%d\">��һƪ</a>]", buf2, records[2].id, num + 1);
    }
    ptr = records[1].title;
    if (!strncmp(ptr, "Re: ", 4))
        ptr += 4;
    ptr[60] = 0;
    if ((records[1].accessed[1] & FILE_READ) == 0) {
        printf("[<a href=\"bbspst?board=%s&file=%s&userid=%s&title=Re: %s&refilename=%s&attach=%d\">������</a>]",
               buf2, records[1].filename, records[1].owner, encode_url(buf, void1(ptr), sizeof(buf)), records[1].filename,bp->flag&BOARD_ATTACH?1:0);
    }
    printf("[<a href=\"bbstfind?board=%s&title=%s\">ͬ�����Ķ�</a>]", buf2, encode_url(buf, void1(ptr), sizeof(buf)));
    printf("[<a href=\"javascript:history.go(-1)\">���ٷ���</a>]\n");
    printf("</center>\n");
    http_quit();
}
