/*
 * $Id$
 */
#include "bbslib.h"

int get_edit_post(char *userid, char *board, char *file, struct fileheader *x)
{
    struct userec *user;
    boardheader_t *brd;
    char fname[STRLEN];

    if (getuser(userid, &user) == 0)
        return -1;
    brd = getbcache(board);
    if (brd == NULL) {
        /*http_fatal("�����������"); */
        return -1;
    }
    if (!strcmp(board, "syssecurity")
        || !strcmp(board, "junk")
        || !strcmp(board, "deleted"))   /* Leeward : 98.01.22 */
        return -1;
    if (checkreadonly(board) == true) {
        /*http_fatal("������ֻ��"); */
        return -1;
    }
    if (get_file_ent(board, file, x) == 0) {
        /*http_fatal("�޷�ȡ���ļ���¼"); */
        return -1;
    }
    if (!HAS_PERM(user, PERM_SYSOP)     /* SYSOP����ǰ������ԭ������ ���Ա༭ */
        &&!chk_currBM(brd->BM, user)) {
        setbfile(fname, board, file);
        if (!isowner(user, x)) {
            /*http_fatal("��������"); */
            return -1;
        }
        /* ͬ��ID�����޸���ID������ */
        else if (file_time(fname) < user->firstlogin) {
            /*time_t t;

               t = file_time(fname);
               printf("file_time = %s<br>", ctime(&t));
               t = user->firstlogin;
               printf("firstlogin = %s<br>", ctime(&t));
               http_fatal("����ʱ��"); */
            return -1;
        }
    }
    /* ������ֹPOST ��� */
    if (deny_me(user->userid, board) && !HAS_PERM(user, PERM_SYSOP)) {
        /*http_fatal("�����POST"); */
        return -1;
    }
    return 0;
}

int get_file_attach( char *infile, long *attach_pt, long * attach_length){
    int ch;
    char attachpad[10];
    int matched;
    char* ptr;
    off_t size;
	long ret=0;
	int fd;

	if((fd = open(infile, O_RDONLY)) < 0)
		return 0;

	*attach_length=0;
    matched=0;

        if (safe_mmapfile_handle(fd,  PROT_READ, MAP_SHARED, (void **) &ptr, &size) == 1) {
            char* data;
            long not;
            data=ptr;
            for (not=0;not<size;not++,data++) {
                if (*data==0) {
                    matched++;
                    if (matched==ATTACHMENT_SIZE) {
                        int d, size2;
						char *sstart = data;
                        data++; not++;
						if(ret == 0)
							ret = data - ptr - ATTACHMENT_SIZE + 1;
                        while(*data){
							data++;
							not++;
						}
                        data++;
                        not++;
                        memcpy(&d, data, 4);
                        size2 = htonl(d);
                        data+=4+size2-1;
                        not+=4+size2-1;
                        matched = 0;
						*attach_length += data - sstart + ATTACHMENT_SIZE;
                    }
                    continue;
                }
            }
        }else{
			close(fd);
			return 0;
		}

		end_mmapfile( (void *)ptr, size ,-1);

		*attach_pt = ret;
		close(fd);
		if( ret <= 0) return 0;
		return 1;
}

int update_form(char *board, char *file)
{
    FILE *fin;
    FILE *fout;
    char *buf = getparm("text");
    char infile[80], outfile[80];
    char buf2[256];
    int i;
	long attach_pt,attach_size;

    setbfile(infile, board, file);
    sprintf(outfile, "tmp/%s.%d.editpost", getcurruserid(), getpid());
    if ((fin = fopen(infile, "r")) == NULL)
        return -1;
    if ((fout = fopen(outfile, "w")) == NULL) {
        fclose(fin);
        return -1;
    }
    for (i = 0; i < 4; i++) {
        fgets(buf2, sizeof(buf2), fin);
        fprintf(fout, "%s", buf2);
    }
    fprintf(fout, "%s", unix_string(buf));
    fprintf(fout, "\033[36m�� �޸�:��%s � %s �޸ı��ġ�[FROM: %s]\033[m\n", currentuser->userid, wwwCTime(time(0)) + 4, SHOW_USERIP(currentuser, fromhost));
    while (fgets(buf2, sizeof(buf2), fin) != NULL) {
        if (Origin2(buf2)) {
            fprintf(fout, "%s", buf2);
            break;
        }
    }
    fclose(fin);
    fclose(fout);
#ifdef FILTER
    if (check_badword(outfile) !=0) {
		unlink(outfile);
        printf("�޸�����ʧ�ܣ����¿��ܺ��в�ǡ������.<br><a href=\"/bbsdoc.php?board=%s\">���ر�������</a>", board);
    }
    else {
#endif
		if( get_file_attach( infile, &attach_pt, &attach_size) ){
			int fsrc,fdst;
			if( (fsrc = open(infile, O_RDONLY) ) >= 0){
				if((fdst = open( outfile, O_WRONLY| O_CREAT | O_APPEND ,0644) ) >=0){
					char *src = (char *)malloc(10240);
					long ret;
					long lsize = attach_size;
					long ndread;
					lseek(fsrc, attach_pt-1, SEEK_SET);
					do{
						if( lsize > 10240 )
							ndread = 10240;
						else
							ndread = lsize;
						ret = read(fsrc, src,ndread);
						if(ret <= 0)
							break;
						lsize -= ret;
					}while( write(fdst, src, ret) > 0 && lsize > 0);
					close(fdst);
					free(src);
				}close(fsrc);
			}
		}
        f_mv(outfile, infile);
        printf("�޸����³ɹ�.<br><a href=\"/bbsdoc.php?board=%s\">���ر�������</a>", board);
#ifdef FILTER
    }
#endif
    return 0;
}

int main()
{
    FILE *fp;
    int type = 0, i, num;
    struct fileheader x;
    char userid[80], buf[512], path[512], file[512], board[512], title[80] = "";

    init_all();
    if (!loginok)
        http_fatal("�Ҵҹ��Ͳ����޸����£����ȵ�¼");
    strsncpy(board, getparm("board"), 20);
    type = atoi(getparm("type"));
    strsncpy(file, getparm("file"), 20);
    if (VALID_FILENAME(file) < 0)
        http_fatal("����Ĳ���");
    if (get_edit_post(getcurruserid(), board, file, &x) < 0)
        http_fatal("�����ܱ༭��ƪ����");
    printf("<center>%s -- �޸����� [ʹ����: %s]<hr color=\"green\">\n", BBSNAME, currentuser->userid);
    if (type != 0)
        return update_form(board, file);
    printf("<table border=\"1\">\n");
    printf("<tr><td>");
    printf("<tr><td><form method=\"post\" action=\"bbsedit\">\n");
    printf("ʹ�ñ���: %s ������: %s<br>\n", nohtml(x.title), board);
    printf("�������ߣ�%s<br>\n", currentuser->userid);
    printf("<textarea name=\"text\" rows=\"20\" cols=\"80\" wrap=\"physical\">");
    setbfile(path, board, file);
    fp = fopen(path, "r");
    if (fp == 0)
        http_fatal("�ļ���ʧ");
    /* ���ﻹ��Ҫ��Ӵ��룬�Է�ֹ�û��޸��ļ�ͷ����Դ */
    for (i = 0; i < 4; i++)
        if (skip_attach_fgets(buf, sizeof(buf), fp)==0) break;
    
    while (skip_attach_fgets(buf, sizeof(buf), fp) != 0) {
        char tmp[256];
        if (Origin2(buf))
            break;
        sprintf(tmp, "�� ��Դ:��%s ", BBS_FULL_NAME);
        if (strstr(buf, tmp) && *buf != ':')
            break;
        if (!strcasestr(buf, "</textarea>"))
            printf("%s", buf);
    }
    fclose(fp);
    printf("</textarea>\n");
    printf("<tr><td class=\"post\" align=\"center\">\n");
    printf("<input type=\"hidden\" name=\"type\" value=\"1\">\n");
    printf("<input type=\"hidden\" name=\"board\" value=\"%s\">\n", board);
    printf("<input type=\"hidden\" name=\"file\" value=\"%s\">\n", file);
    printf("<input type=\"submit\" value=\"����\"> \n");
    printf("<input type=\"reset\" value=\"����\"></form>\n");
    printf("</table>");
    http_quit();
    return 0;
}
