#include "bbs.h"
#include <utime.h>

void cancelpost(char *board, char *userid, struct fileheader *fh, int owned, int autoappend);
int outgo_post(struct fileheader *fh, char *board, char *title)
{
    FILE *foo;

    if ((foo = fopen("innd/out.bntp", "a")) != NULL) {
        fprintf(foo, "%s\t%s\t%s\t%s\t%s\n", board, fh->filename, currentuser->userid, currentuser->username, title);
        fclose(foo);
        return 0;
    }
    return -1;
}

extern char alphabet[];

int get_postfilename(char *filename, char *direct, int use_subdir)
{
    static const char post_sufix[] = "0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";
    int fp;
    time_t now;
    int i;
    char fname[255];
    int pid = getpid();
    int rn;
    int len;

    /*
     * �Զ����� POST �ļ��� 
     */
    now = time(NULL);
    len = strlen(alphabet);
    for (i = 0; i < 10; i++) {
        if (use_subdir) {
            rn = 0 + (int) (len * 1.0 * rand() / (RAND_MAX + 1.0));
            sprintf(filename, "%c/M.%lu.%c%c", alphabet[rn], now, post_sufix[(pid + i) % 62], post_sufix[(pid * i) % 62]);
        } else
            sprintf(filename, "M.%lu.%c%c", now, post_sufix[(pid + i) % 62], post_sufix[(pid * i) % 62]);
        sprintf(fname, "%s/%s", direct, filename);
        if ((fp = open(fname, O_CREAT | O_EXCL | O_WRONLY, 0644)) != -1) {
            break;
        };
    }
    if (fp == -1)
        return -1;
    close(fp);
    return 0;
}

int isowner(struct userec *user, struct fileheader *fileinfo)
{
    time_t posttime;

    if (strcmp(fileinfo->owner, user->userid))
        return 0;
    posttime = get_posttime(fileinfo);
    if (posttime < user->firstlogin)
        return 0;
    return 1;
}

int cmpname(fhdr, name)         /* Haohmaru.99.3.30.�Ƚ� ĳ�ļ����Ƿ�� ��ǰ�ļ� ��ͬ */
struct fileheader *fhdr;
char name[STRLEN];
{
    if (!strncmp(fhdr->filename, name, FILENAME_LEN))
        return 1;
    return 0;
}

/*
  * �жϵ�ǰģʽ�Ƿ����ʹ��id����
  */
bool is_sorted_mode(int mode)
{
    switch (mode) {
        case DIR_MODE_NORMAL:
        case DIR_MODE_THREAD:
        case DIR_MODE_MARK:
        case DIR_MODE_ORIGIN:
        case DIR_MODE_AUTHOR:
        case DIR_MODE_TITLE:
        case DIR_MODE_SUPERFITER:
        case DIR_MODE_WEB_THREAD:
            return true;
    }
    return false;
}

/** ��ʼ��filearg�ṹ
  */
void malloc_write_dir_arg(struct write_dir_arg*filearg)
{
    filearg->filename=NULL;
    filearg->fileptr=MAP_FAILED;
    filearg->ent=-1;
    filearg->fd=-1;
    filearg->size=-1;
    filearg->needclosefd=false;
    filearg->needlock=true;
}

/** ��ʼ��filearg�ṹ,�Ѹ�������mmap��
  */
int init_write_dir_arg(struct write_dir_arg*filearg)
{
    if (filearg->fileptr==MAP_FAILED) {
        if (filearg->fd==-1) {
            if (safe_mmapfile(filearg->filename, 
                        O_RDWR, 
                        PROT_READ | PROT_WRITE, 
                        MAP_SHARED,
                        (void **) &filearg->fileptr, &filearg->size, &filearg->fd) == 0)
                return -1;
            filearg->needclosefd=true;
        } else { //��fd����
            if (safe_mmapfile_handle(filearg->fd, 
                        PROT_READ | PROT_WRITE, 
                        MAP_SHARED,
                        (void **) &filearg->fileptr, &filearg->size) == 0)
                return -1;
        }
    }
    return 0;
}

/*
 *  �ͷ�filearg���������Դ��
 */
void free_write_dir_arg(struct write_dir_arg*filearg)
{
    if (filearg->needclosefd&&(filearg->fd!=-1)) {
        close(filearg->fd);
    }
    if (filearg->fileptr!=MAP_FAILED)
        end_mmapfile((void *) filearg->fileptr, filearg->size, -1);
}

/*
 * дdir�ļ�֮ǰ���ļ���λ����ȷ��λ�ã�lockס����������Ӧ������
 * ����߲�������free_write_dir_arg���ͷ���Դ����Ҫ�ϲ��Լ���
 * ���ô˺�������ɹ����ǵü�ʱflock(filearg->fd,LOCK_UN)
 * @param filearg ����Ľṹ�����filearg->fd��Ϊ-1��˵����Ҫ��dirarg->direct�ļ�
 *                        ����ʹ��filearg->fd��Ϊ�ļ������
 *                        filearg->entΪԤ�Ƶ�λ�ã�ent>=1
 *                        filearg->fileptr���������MAP_FAILED,��ô���������mmap�ļ���
 *                        ��дfilearg->fileptr��filearg->size���ڷ�������
 * @param fileinfo ���ڶ�λ�Ķ�������Ϊ�յ�ʱ����Ҫ��λ
 * @param mode ��ǰģʽ��ֻ��sorted�Ŀ���ʹ�ö���
 * @return 0�ɹ�,��ʱmmap��ϡ�
 */
int prepare_write_dir(struct write_dir_arg * filearg,struct fileheader* fileinfo,int mode)
{
    int ret=0;
    bool needrelocation=false;
    BBS_TRY {
        int count;
        struct fileheader* nowFh;
        if (init_write_dir_arg(filearg)!=0)
            BBS_RETURN(-1);
        count=filearg->size/sizeof(struct fileheader);
        if (filearg->needlock)
            flock(filearg->fd,LOCK_EX);
        if (fileinfo) { //��λһ��
            if ((filearg->ent>count)||(filearg->ent<=0))
                needrelocation=true;
            else {
                nowFh=filearg->fileptr+(filearg->ent-1);
                needrelocation=strcmp(fileinfo->filename,nowFh->filename);
			}
            
        }
        if (needrelocation) { //�ض�λ���λ��
            int i;
            if (is_sorted_mode(mode)) {
                filearg->ent=Search_Bin(filearg->fileptr, fileinfo->id, 0, count-1)+1;
            } else {//ƥ���ļ���
                int oldent=filearg->ent;
                nowFh=filearg->fileptr;
                filearg->ent=-1;
                /* �ȴӵ�ǰλ����ǰ�ң���Ϊһ�㶼�Ǳ�ɾ��������ǰ��*/
                nowFh=filearg->fileptr+(oldent-1);
                for (i=oldent;i>=0;i--,nowFh--) {
                    if (!strcmp(fileinfo->filename,nowFh->filename)) {
                        filearg->ent=i+1;
                        break;
                    }
                }
                /* �ٴӵ�ǰλ��������*/
                nowFh=filearg->fileptr+oldent;
                for (i=oldent+1;i<count;i++,nowFh++) {
                    if (!strcmp(fileinfo->filename,nowFh->filename)) {
                        filearg->ent=i+1;
                        break;
                    }
                }
            }
            if (filearg->ent<=0)
                ret=-1;
        }
    }
    BBS_CATCH {
        ret = -1;
    }
    BBS_END;
    if (ret!=0)
        flock(filearg->fd,LOCK_UN);
    return ret;
}

int do_del_post(struct userec *user, struct write_dir_arg*dirarg,struct fileheader *fileinfo, char *board, int currmode, int decpost)
{
    char *t;
    int owned;
    struct fileheader fh;

    if (prepare_write_dir(dirarg,fileinfo,currmode)!=0)
        return-1;
    BBS_TRY {
		fh=*(dirarg->fileptr + (dirarg->ent - 1));
        memcpy(dirarg->fileptr + (dirarg->ent - 1), 
            dirarg->fileptr + dirarg->ent, 
            dirarg->size - sizeof(struct fileheader) * dirarg->ent);
        dirarg->size-=sizeof(struct fileheader);
#ifdef DEBUG
#ifdef BBSMAIN
        newbbslog(BBSLOG_DEBUG,"%s ftruncate %d",
            dirarg->filename?dirarg->filename:currboard->filename,
            dirarg->size);
#endif
#endif      
        ftruncate(dirarg->fd, dirarg->size);
    }
    BBS_CATCH {
    }
    BBS_END;
	if (dirarg->needlock)
        flock(dirarg->fd,LOCK_UN); /*�������Ҫ�Ͻ�����*/
    if (fh.id == fh.groupid)
        setboardorigin(board, 1);
    setboardtitle(board, 1);

    
    owned = isowner(user, &fh);
        cancelpost(board, user->userid, &fh, owned, 1);
        updatelastpost(board);
        if (fh.accessed[0] & FILE_MARKED)
            setboardmark(board, 1);
        if ((DIR_MODE_NORMAL == currmode)        /* �������� ��NA ==�� �жϣ�digestmode ��ֵ */
            &&!((fh.accessed[0] & FILE_MARKED)
                && (fh.accessed[1] & FILE_READ)
                && (fh.accessed[0] & FILE_FORWARDED))) { /* Leeward 98.06.17 ����ժ��ɾ�Ĳ���������Ŀ */
            if (owned) {
                if ((int) user->numposts > 0 && !junkboard(board)) {
                    user->numposts--;   /*�Լ�ɾ�������£�����post�� */
                }
            } else if (!strstr(fh.owner, ".") && BMDEL_DECREASE && decpost /*����ɾ��,����POST�� */ ) {
                struct userec *lookupuser;
                int id = getuser(fh.owner, &lookupuser);

                if (id && (int) lookupuser->numposts > 0 && !junkboard(board) && strcmp(board, SYSMAIL_BOARD)) {        /* SYSOP MAIL��ɾ�Ĳ������� Bigman: 2000.8.12 *//* Leeward 98.06.21 adds above later 2 conditions */
                    lookupuser->numposts--;
                }
            }
        }
        if (user != NULL)
            bmlog(user->userid, board, 8, 1);
        newbbslog(BBSLOG_USER, "Del '%s' on '%s'", fh.title, board);     /* bbslog */
        return 0;
}

/* by ylsdd 
   unlink action is taked within cancelpost if in mail mode,
   otherwise this item is added to the file '.DELETED' under
   the board's directory, the filename is not changed. 
   Unlike the fb code which moves the file to the deleted
   board.
*/
void cancelpost(board, userid, fh, owned, autoappend)
char *board, *userid;
struct fileheader *fh;
int owned;
int autoappend;
{
    struct fileheader postfile;
    char oldpath[50];
    char newpath[50];
    struct fileheader *ph;
    time_t now;

#ifdef BBSMAIN
    if (uinfo.mode == RMAIL) {
        sprintf(oldpath, "mail/%c/%s/%s", toupper(currentuser->userid[0]), currentuser->userid, fh->filename);
        my_unlink(oldpath);
        return;
    }
#endif
    if (autoappend)
        ph = &postfile;
    else
        ph = fh;
/*
    sprintf(oldpath, "/board/%s/%s.html", board, fh->filename);
    ca_expire_file(oldpath);*/

    if ((fh->innflag[1] == 'S')
        && (fh->innflag[0] == 'S')
        && (get_posttime(fh) > now - 14 * 86400)) {
        FILE *fp;
        char buf[256];
        char from[STRLEN];
        int len;
        char *ptr;

        setbfile(buf, board, fh->filename);
        if ((fp = fopen(buf, "rb")) == NULL)
            return;
        while (skip_attach_fgets(buf, sizeof(buf), fp) != NULL) {
            /*
             * �����˵����з� 
             */
            len = strlen(buf) - 1;
            buf[len] = '\0';
            if (len < 8)
                break;
            if (strncmp("������: ", buf, 8) == 0) {
                if ((ptr = strrchr(buf, ')')) == NULL)
                    break;
                *ptr = '\0';
                if ((ptr = strrchr(buf, '(')) == NULL)
                    break;
                strncpy(from, ptr + 1, sizeof(from) - 1);
                from[sizeof(from) - 1] = '\0';
                break;
            }
        }
        fclose(fp);
        sprintf(buf, "%s\t%s\t%s\t%s\t%s\n", board, fh->filename, fh->owner, from, fh->title);
        if ((fp = fopen("innd/cancel.bntp", "a")) != NULL) {
            fputs(buf, fp);
            fclose(fp);
        }
    }

    strcpy(postfile.filename, fh->filename);
    if (fh->filename[1] == '/')
        fh->filename[2] = (owned) ? 'J' : 'D';
    else
        fh->filename[0] = (owned) ? 'J' : 'D';
    setbfile(oldpath, board, postfile.filename);
    setbfile(newpath, board, fh->filename);
    f_mv(oldpath, newpath);
    if (autoappend) {
        bzero(&postfile, sizeof(postfile));
        strcpy(postfile.filename, fh->filename);
        strncpy(postfile.owner, fh->owner, OWNER_LEN - 1);
        postfile.owner[OWNER_LEN - 1] = 0;
        postfile.id = fh->id;
        postfile.groupid = fh->groupid;
        postfile.reid = fh->reid;
		postfile.attachment=fh->attachment;
        set_posttime2(&postfile, fh);
    };
    now = time(NULL);
    sprintf(oldpath, "%-32.32s - %s", fh->title, userid);
    strncpy(ph->title, oldpath, STRLEN);
    ph->title[STRLEN - 1] = 0;
    ph->accessed[11] = now / (3600 * 24) % 100; /*localtime(&now)->tm_mday; */
    if (autoappend) {
        setbdir((owned) ? 5 : 4, oldpath, board);
        append_record(oldpath, &postfile, sizeof(postfile));
    }
}


void add_loginfo(char *filepath, struct userec *user, char *currboard, int Anony)
{                               /* POST ���һ�� ��� */
    FILE *fp;
    int color, noidboard;
    char fname[STRLEN];

    noidboard = (anonymousboard(currboard) && Anony);   /* etc/anonymous�ļ��� ����������� */
    color = (user->numlogins % 7) + 31; /* ��ɫ����仯 */
    sethomefile(fname, user->userid, "signatures");
    fp = fopen(filepath, "ab");
    if (!dashf(fname) ||        /* �ж��Ƿ��Ѿ� ���� ǩ���� */
        user->signature == 0 || noidboard) {
        fputs("\n--\n", fp);
    } else {                    /*Bigman 2000.8.10�޸�,���ٴ��� */
        fprintf(fp, "\n");
    }
    /*
     * ��Bigman����:2000.8.10 Announce�������������� 
     */
    if (!strcmp(currboard, "Announce"))
        fprintf(fp, "[m[1;%2dm�� ��Դ:��%s %s��[FROM: %s][m\n", color, BBS_FULL_NAME, NAME_BBS_ENGLISH, NAME_BBS_CHINESE " BBSվ");
    else
        fprintf(fp, "\n[m[1;%2dm�� ��Դ:��%s %s��[FROM: %s][m\n", color, BBS_FULL_NAME, NAME_BBS_ENGLISH, (noidboard) ? NAME_ANONYMOUS_FROM : fromhost);
    fclose(fp);
    return;
}

void addsignature(FILE * fp, struct userec *user, int sig)
{
    FILE *sigfile;
    int i, valid_ln = 0;
    char tmpsig[MAXSIGLINES][256];
    char inbuf[256];
    char fname[STRLEN];

    if (sig == 0)
        return;
    sethomefile(fname, user->userid, "signatures");
    if ((sigfile = fopen(fname, "r")) == NULL) {
        return;
    }
    fputs("\n--\n", fp);
    for (i = 1; i <= (sig - 1) * MAXSIGLINES && sig != 1; i++) {
        if (!fgets(inbuf, sizeof(inbuf), sigfile)) {
            fclose(sigfile);
            return;
        }
    }
    for (i = 1; i <= MAXSIGLINES; i++) {
        if (fgets(inbuf, sizeof(inbuf), sigfile)) {
            if (inbuf[0] != '\n')
                valid_ln = i;
            strcpy(tmpsig[i - 1], inbuf);
        } else
            break;
    }
    fclose(sigfile);
    for (i = 1; i <= valid_ln; i++)
        fputs(tmpsig[i - 1], fp);
}

int write_posts(char *id, char *board, unsigned int groupid)
{
    char *ptr;
    time_t now;
    struct posttop postlog, pl;
	char xpostfile[PATHLEN];

#ifdef BLESS_BOARD
    if (strcasecmp(board, BLESS_BOARD) && (!poststatboard(board) || normal_board(board) != 1))
#else
    if (!poststatboard(board) || normal_board(board) != 1)
#endif
        return 0;
    now = time(0);
//    strcpy(postlog.author, id);
    strcpy(postlog.board, board);
	postlog.groupid = groupid;
    postlog.date = now;
    postlog.number = 1;

	sprintf(xpostfile, "tmp/Xpost/%s", id);

    {                           /* added by Leeward 98.04.25 
                                 * TODO: ����ط��е㲻��,ÿ�η���Ҫ����һ��,���浽.Xpost��,
                                 * �������ʮ����ͳ�����ID����������.����
                                 * KCN */
        int log = 1;
        FILE *fp = fopen(xpostfile, "r");

        if (fp) {
            while (!feof(fp)) {
                fread(&pl, sizeof(pl), 1, fp);
                if (feof(fp))
                    break;

                if (  pl.groupid == groupid && !strcmp(pl.board,board) ){
                    log = 0;
                    break;
                }
            }
            fclose(fp);
        }

        if (log) {
            append_record(xpostfile, &postlog, sizeof(postlog));
            append_record(".newpost", &postlog, sizeof(postlog));
        }
    }

/*    append_record(".post.X", &postlog, sizeof(postlog));
*/
    return 0;
}

void write_header(FILE * fp, struct userec *user, int in_mail, char *board, char *title, int Anony, int mode)
{
    int noname;
    char uid[20];
    char uname[40];
    time_t now;

    now = time(0);
    strncpy(uid, user->userid, 20);
    uid[19] = '\0';
    if (in_mail)
#if defined(MAIL_REALNAMES)
        strncpy(uname, user->realname, NAMELEN);
#else
        strncpy(uname, user->username, NAMELEN);
#endif
    else
#if defined(POSTS_REALNAMES)
        strncpy(uname, user->realname, NAMELEN);
#else
        strncpy(uname, user->username, NAMELEN);
#endif
    /*
     * uid[39] = '\0' ; SO FUNNY:-) ����� 20 ����ȴ�� 39 !
     * Leeward: 1997.12.11 
     */
    uname[39] = 0;              /* ��ʵ��д���������! �ٺ� */
    if (in_mail)
        fprintf(fp, "������: %s (%s)\n", uid, uname);
    else {
        noname = anonymousboard(board);
		/*
        if (((mode == 0) || (mode == 2)) && !(noname && Anony)) {
            *
             * mode=0�������ķ��Ĳ���local save 
             * * mode=1�ǲ���Ҫ��¼��
             * * mode=2�Ƿ�local save��
             *
            write_posts(user->userid, board, title);
        }
		*/
#ifdef SMTH
        if (!strcmp(board, "Announce") && Anony)
            /*
             * added By Bigman 
             */
            fprintf(fp, "������: %s (%s), ����: %s\n", "SYSOP", NAME_SYSOP, board);
        else
#endif
            fprintf(fp, "������: %s (%s), ����: %s\n", (noname && Anony) ? board : uid, (noname && Anony) ? NAME_ANONYMOUS : uname, board);
    }

    fprintf(fp, "��  ��: %s\n", title);
    /*
     * ����ת�ű�� czz 020819 
     */
    if (in_mail)
        fprintf(fp, "����վ: %s (%24.24s)\n", BBS_FULL_NAME, ctime(&now));
    else if (mode != 2)
        fprintf(fp, "����վ: %s (%24.24s), վ��\n", BBS_FULL_NAME, ctime(&now));
    else
        fprintf(fp, "����վ: %s (%24.24s), ת��\n", BBS_FULL_NAME, ctime(&now));
    if (in_mail)
        fprintf(fp, "��  Դ: %s \n", fromhost);
    fprintf(fp, "\n");

}

void getcross(char *filepath, char *quote_file, struct userec *user, int in_mail, char *board, char *title, int Anony, int mode, char *sourceboard)
{                               /* ��quote_file���Ƶ�filepath (ת�����Զ�����) */
    FILE *inf, *of;
    char buf[256];
    char owner[256];
    int count;
    time_t now;
    int asize;

    now = time(0);
    inf = fopen(quote_file, "rb");
    of = fopen(filepath, "w");
    if (inf == NULL || of == NULL) {
        /*---	---*/
        if (NULL != inf)
            fclose(inf);
        if (NULL != of)
            fclose(of);
        /*---	---*/
#ifdef BBSMAIN
        bbslog("user", "%s", "Cross Post error");
#endif
        return;
    }
    if (mode == 0 /*ת�� */ ) {
        int normal_file;
        int header_count;
        int asize;

        normal_file = 1;

        write_header(of, user, in_mail, sourceboard, title, Anony, 1 /*��д�� .posts */ );
        if (skip_attach_fgets(buf, 256, inf) != NULL) {
            for (count = 8; buf[count] != ' ' && count < 256; count++)
                owner[count - 8] = buf[count];
        }
        owner[count - 8] = '\0';
        if (in_mail == true)
            fprintf(of, "[1;37m�� ��������ת���� [32m%s [37m������ ��[m\n", user->userid);
        else
            fprintf(of, "�� ��������ת���� %s ������ ��\n", board);
        if (id_invalid(owner))
            normal_file = 0;
        if (normal_file) {
            for (header_count = 0; header_count < 3; header_count++) {
                if (skip_attach_fgets(buf, 256, inf) == NULL)
                    break;      /*Clear Post header */
            }
            if ((header_count != 2) || (buf[0] != '\n'))
                normal_file = 0;
        }
        if (normal_file)
            fprintf(of, "�� ԭ���� %s ������ ��\n", owner);
        else
            fseek(inf, 0, SEEK_SET);

    } else if (mode == 1 /*�Զ����� */ ) {
        fprintf(of, "������: deliver (�Զ�����ϵͳ), ����: %s\n", board);
        fprintf(of, "��  ��: %s\n", title);
        fprintf(of, "����վ: %s�Զ�����ϵͳ (%24.24s)\n\n", BBS_FULL_NAME, ctime(&now));
        fprintf(of, "����ƪ���������Զ�����ϵͳ��������\n\n");
    } else if (mode == 2) {
        write_header(of, user, in_mail, sourceboard, title, Anony, 0 /*д�� .posts */ );
    }
    while ((asize=-attach_fgets(buf, 256, inf)) != 0) {
        if ((strstr(buf, "�� ��������ת���� ") && strstr(buf, "������ ��")) || (strstr(buf, "�� ԭ����") && strstr(buf, "������ ��")))
            continue;           /* ���������ظ� */
        else
            fprintf(of, "%s", buf);
        put_attach(inf, of, asize);
    }
    fclose(inf);
    fclose(of);
    /*
     * don't know why 
     * *quote_file = '\0';
     */
}

/* Add by SmallPig */
int post_cross(struct userec *user, char *toboard, char *fromboard, char *title, char *filename, int Anony, int in_mail, char islocal, int mode)
{                               /* (�Զ������ļ���) ת�����Զ����� */
    struct fileheader postfile;
    char filepath[STRLEN];
    char buf4[STRLEN], whopost[IDLEN], save_title[STRLEN];
    int aborted, local_article;
	int oldmode;

    if (!mode && !haspostperm(user, toboard)) {
#ifdef BBSMAIN
        move(1, 0);
        prints("������Ȩ���� %s ��������.\n", toboard);
        prints("�������δע�ᣬ���ڸ��˹���������ϸע�����\n");
        prints("δͨ�����ע����֤���û���û�з������µ�Ȩ�ޡ�\n");
        prints("лл������ :-) \n");
#endif
        return -1;
    }

    memset(&postfile, 0, sizeof(postfile));

    if (!mode) {
        if (!strstr(title, "(ת��)"))
            sprintf(buf4, "%s (ת��)", title);
        else
            strcpy(buf4, title);
    } else
        strcpy(buf4, title);
    strncpy(save_title, buf4, STRLEN);

    setbfile(filepath, toboard, "");

    if ((aborted = GET_POSTFILENAME(postfile.filename, filepath)) != 0) {
#ifdef BBSMAIN
        move(3, 0);
        clrtobot();
        prints("\n\n�޷������ļ�:%d...\n", aborted);
        pressreturn();
        clear();
#endif
        return -2;
    }

    if (mode == 1)
        strcpy(whopost, "deliver");     /* mode==1Ϊ�Զ����� */
    else
        strcpy(whopost, user->userid);

    strncpy(postfile.owner, whopost, OWNER_LEN);
    postfile.owner[OWNER_LEN - 1] = 1;
    setbfile(filepath, toboard, postfile.filename);

    local_article = 1;          /* default is local article */
    if (islocal != 'l' && islocal != 'L') {
        if (is_outgo_board(toboard))
            local_article = 0;
    }
#ifdef BBSMAIN
	oldmode = uinfo.mode;
    modify_user_mode(POSTING);
#endif
    getcross(filepath, filename, user, in_mail, fromboard, title, Anony, mode, toboard);        /*����fname��� �ļ����� */

    postfile.eff_size=get_effsize(filepath);
    /*
     * Changed by KCN,disable color title 
     */
    if (mode != 1) {
        int i;

        for (i = 0; (i < strlen(save_title)) && (i < STRLEN - 1); i++)
            if (save_title[i] == 0x1b)
                postfile.title[i] = ' ';
            else
                postfile.title[i] = save_title[i];
        postfile.title[i] = 0;
    } else
        strncpy(postfile.title, save_title, STRLEN);
    if (local_article == 1) {   /* local save */
        postfile.innflag[1] = 'L';
        postfile.innflag[0] = 'L';
    } else {
        postfile.innflag[1] = 'S';
        postfile.innflag[0] = 'S';
        outgo_post(&postfile, toboard, save_title);
    }
    if (!strcmp(toboard, "syssecurity")
        && strstr(title, "�޸� ")
        && strstr(title, " ��Ȩ��"))
        postfile.accessed[0] |= FILE_MARKED;    /* Leeward 98.03.29 */
    if (strstr(title, "����Ȩ��") && mode == 2) {
#ifndef NINE_BUILD
        /*
         * disable here to avoid Mark deny articles on the board Bigman.2002.11.17
         */
        /*
         * postfile.accessed[0] |= FILE_MARKED;
         *//*
         * Haohmaru 99.11.10 
         */
        postfile.accessed[1] |= FILE_READ;
#endif
        postfile.accessed[0] |= FILE_FORWARDED;
    }
    after_post(user, &postfile, toboard, NULL, !(Anony));
#ifdef BBSMAIN
    modify_user_mode(oldmode);
#endif
    return 1;
}


int post_file(struct userec *user, char *fromboard, char *filename, char *nboard, char *posttitle, int Anony, int mode)
/* ��ĳ�ļ� POST ��ĳ�� */
{
    if (getboardnum(nboard, NULL) <= 0) {       /* ����ҪPOST�İ� ,�ж��Ƿ���ڸð� */
        return -1;
    }
    post_cross(user, nboard, fromboard, posttitle, filename, Anony, false, 'l', mode);  /* post �ļ� */
    return 0;
}

int after_post(struct userec *user, struct fileheader *fh, char *boardname, struct fileheader *re, int poststat)
{
    char buf[256];
    int fd, err = 0, nowid = 0;
    char *p;

#ifdef FILTER
    char oldpath[50], newpath[50];
    int filtered;
    struct boardheader *bh;
#endif

    if ((re == NULL) && (!strncmp(fh->title, "Re:", 3))) {
        strncpy(fh->title, fh->title + 4, STRLEN);
    }
#ifdef FILTER
    setbfile(oldpath, boardname, fh->filename);
    filtered = 0;
    bh = getbcache(boardname);
    if (strcmp(fh->owner, "deliver")) {
        if (((bh && bh->level & PERM_POSTMASK) || normal_board(boardname)) && strcmp(boardname, FILTER_BOARD)
#if 0
            &&strcmp(boardname,"NewsClub") 
#endif
	  )
        {
#ifdef SMTH
#if 0
            int isnews;
            isnews=!strcmp(boardname,"News");
            if (isnews||check_badword_str(fh->title, strlen(fh->title)) || check_badword(oldpath))
#else
            if (check_badword_str(fh->title, strlen(fh->title)) || check_badword(oldpath))
#endif
#else
            if (check_badword_str(fh->title, strlen(fh->title)) || check_badword(oldpath))
#endif
            {
                /*
                 * FIXME: There is a potential bug here. 
                 */
#ifdef SMTH
#if  0
                if (isnews)
                    setbfile(newpath, "NewsClub", fh->filename);
                else
#endif
#endif
                setbfile(newpath, FILTER_BOARD, fh->filename);
                f_mv(oldpath, newpath);
                strncpy(fh->o_board, boardname, STRLEN - BM_LEN);
                nowid = get_nextid(boardname);
                fh->o_id = nowid;
                if (re == NULL) {
                    fh->o_groupid = fh->o_id;
                    fh->o_reid = fh->o_id;
                } else {
                    fh->o_groupid = re->groupid;
                    fh->o_reid = re->id;
                }
#ifdef SMTH
#if  0
                if (isnews)
                    boardname = "NewsClub";
                else
#endif
#endif
                boardname = FILTER_BOARD;
                filtered = 1;
            };
        }
    }
#endif
    setbfile(buf, boardname, DOT_DIR);

    if ((fd = open(buf, O_WRONLY | O_CREAT, 0664)) == -1) {
#ifdef BBSMAIN
        perror(buf);
#endif
        err = 1;
    }
    /*
     * ���˲�ɫ����
     */
    for (p = fh->title; *p; p++)
        if (*p == '\x1b')
            *p = ' ';

    if (!err) {
        flock(fd, LOCK_EX);
        nowid = get_nextid(boardname);
        fh->id = nowid;
        if (re == NULL) {
            fh->groupid = fh->id;
            fh->reid = fh->id;
        } else {
            fh->groupid = re->groupid;
            fh->reid = re->id;
        }
        set_posttime(fh);
        lseek(fd, 0, SEEK_END);
        if (safewrite(fd, fh, sizeof(fileheader)) == -1) {
            bbslog("user", "%s", "apprec write err!");
            err = 1;
        }
        flock(fd, LOCK_UN);
        close(fd);
    }
    if (err) {
        bbslog("3error", "Posting '%s' on '%s': append_record failed!", fh->title, boardname);
        setbfile(buf, boardname, fh->filename);
        my_unlink(buf);
#ifdef BBSMAIN
        pressreturn();
        clear();
#endif
        return 1;
    }
    updatelastpost(boardname);
#ifdef FILTER
    if (filtered)
        sprintf(buf, "posted '%s' on '%s' filtered", fh->title, fh->o_board);
    else {
#endif
#ifdef HAVE_BRC_CONTROL
        brc_add_read(fh->id);
#endif

		/* ���ļĵ�ԭ��������, stiger */
		if( re ){
			if( re->accessed[1] & FILE_MAILBACK ){

				struct userec *lookupuser;
				char newtitle[STRLEN];

				if(getuser(re->owner, &lookupuser) != 0){
					if(( false != canIsend2(currentuser, re->owner)) && !(lookupuser->userlevel & PERM_SUICIDE) && (lookupuser->userlevel & PERM_READMAIL) && !chkusermail(lookupuser)){
						setbfile(buf, boardname, fh->filename);
						snprintf(newtitle, STRLEN, "[����ת��]%s", fh->title);
						mail_file(currentuser->userid, buf, re->owner, newtitle, 0, fh);
					}
				}
			}
		}


        sprintf(buf, "posted '%s' on '%s'", fh->title, boardname);

		if(poststat && user)
			write_posts(user->userid, boardname, fh->groupid);
#ifdef FILTER
    }
#endif
    newbbslog(BBSLOG_USER, "%s", buf);

    if (fh->id == fh->groupid)
        setboardorigin(boardname, 1);
    setboardtitle(boardname, 1);
    if (fh->accessed[0] & FILE_MARKED)
        setboardmark(boardname, 1);
    if (user != NULL)
        bmlog(user->userid, boardname, 2, 1);
#ifdef FILTER
    if (filtered)
        return 2;
    else
#endif
        return 0;
}

int mmap_search_apply(int fd, struct fileheader *buf, DIR_APPLY_FUNC func)
{
    struct fileheader *data;
    size_t filesize;
    int total;
    int low, high;
    int ret;

    if (flock(fd, LOCK_EX) == -1)
        return 0;
    BBS_TRY {
        if (safe_mmapfile_handle(fd, PROT_READ | PROT_WRITE, MAP_SHARED, (void **) &data, &filesize) == 0) {
            flock(fd, LOCK_UN);
            BBS_RETURN(0);
        }
        total = filesize / sizeof(struct fileheader);
        low = 0;
        high = total - 1;
        while (low <= high) {
            int mid, comp;

            mid = (high + low) / 2;
            comp = (buf->id) - ((data + mid)->id);
            if (comp == 0) {
                ret = (*func) (fd, data, mid + 1, total, data, true);
                end_mmapfile((void *) data, filesize, -1);
                flock(fd, LOCK_UN);
                BBS_RETURN(ret);
            } else if (comp < 0)
                high = mid - 1;
            else
                low = mid + 1;
        }
        ret = (*func) (fd, data, low + 1, total, buf, false);
    }
    BBS_CATCH {
    }
    BBS_END end_mmapfile((void *) data, filesize, -1);

    flock(fd, LOCK_UN);
    return ret;
}

int mmap_dir_search(int fd, const fileheader_t * key, search_handler_t func, void *arg)
{
    struct fileheader *data;
    size_t filesize;
    int total;
    int low, high;
    int mid, comp;
    int ret;

    if (flock(fd, LOCK_EX) == -1)
        return 0;
    BBS_TRY {
        if (safe_mmapfile_handle(fd,  PROT_READ | PROT_WRITE, MAP_SHARED, (void **) &data, &filesize) == 0) {
            flock(fd, LOCK_UN);
            BBS_RETURN(0);
        }
        total = filesize / sizeof(fileheader_t);
        low = 0;
        high = total - 1;
        while (low <= high) {
            mid = (high + low) / 2;
            comp = (key->id) - ((data + mid)->id);
            if (comp == 0) {
                ret = (*func) (fd, data, mid + 1, total, true, arg);
                end_mmapfile((void *) data, filesize, -1);
                flock(fd, LOCK_UN);
                BBS_RETURN(ret);
            } else if (comp < 0)
                high = mid - 1;
            else
                low = mid + 1;
        }
        ret = (*func) (fd, data, low + 1, total, false, arg);
    }
    BBS_CATCH {
    }
    BBS_END end_mmapfile((void *) data, filesize, -1);

    flock(fd, LOCK_UN);

    return ret;
}

struct dir_record_set {
    fileheader_t *records;
    int num;
	int rec_no; /* ��¼�����м��¼�������ļ��еļ�¼�ţ��� 1 �ģ�
				   ������¼�ļ�¼�ſ���ͨ�� num �� rec_no ��� */
};

static int get_dir_records(int fd, fileheader_t * base, int ent, int total, bool match, void *arg)
{
    if (match) {
        struct dir_record_set *rs = (struct dir_record_set *) arg;
        int i;
        int off;
        int count = 0;

        off = ent - rs->num / 2;
		rs->rec_no = ent; /* �����ﱣ���¼�� */
        for (i = 0; i < rs->num; i++) {
            if (off < 1 || off > total)
                bzero(rs->records + i, sizeof(fileheader_t));
            else {
                memcpy(rs->records + i, base + off - 1, sizeof(fileheader_t));
                count++;
            }
            off++;
        }
        return count;
    }

    return 0;
}

int get_records_from_id(int fd, int id, fileheader_t *buf, int num, int *index)
{
    struct dir_record_set rs;
    fileheader_t key;
	int ret;

    rs.records = buf;
    rs.num = num;
	rs.rec_no = 0;
    bzero(&key, sizeof(key));
    key.id = id;
	ret = mmap_dir_search(fd, &key, get_dir_records, &rs);
	if (index != NULL)
		*index = rs.rec_no;
	
	return ret;
}

struct dir_thread_set {
    fileheader_t *records;
    int num;
};

static int 
get_dir_threads(int fd, fileheader_t * base, int ent, int total, bool match, void *arg)
{
    if (match) {
        struct dir_thread_set *ts = (struct dir_thread_set *) arg;
        int i;
        int off = 1;
        int count = 0;
		int start = ent + 1;
		int end = total;

		if (ts->num < 0)
		{
			off = -1;
			start = ent - 1;
			end = 1;
			ts->num = -ts->num;
			for (i = start; i >= end; i--)
			{
				if (count == ts->num)
					break;
				if (base[i-1].groupid == base[ent-1].groupid)
				{
					memcpy(ts->records + count, base + i - 1,
							sizeof(fileheader_t));
					count++;
				}
			}
		}
		else
		{
			for (i = start; i <= end; i++)
			{
				if (count == ts->num)
					break;
				if (base[i-1].groupid == base[ent-1].groupid)
				{
					memcpy(ts->records + count, base + i - 1,
							sizeof(fileheader_t));
					count++;
				}
			}
		}
        return count;
    }

    return 0;
}

/* ���� num ��ʾȡ id ��ͬ����� num ƪ���£�
   ���� num ��ʾȡ id ��ͬ����ǰ |num| ƪ���� */
int 
get_threads_from_id(const char *filename, int id, fileheader_t *buf, int num)
{
    struct dir_thread_set ts;
    fileheader_t key;
	int fd;
	int ret;

	if (num == 0)
		return 0;
    ts.records = buf;
    ts.num = num;
    bzero(&key, sizeof(key));
    key.id = id;
	if ((fd = open(filename, O_RDWR, 0644)) < 0)
		return -1;
	ret = mmap_dir_search(fd, &key, get_dir_threads, &ts);
	close(fd);
	
	return ret;
}

struct dir_gthread_set {
    fileheader_t *records;
    int num;
	int groupid;
};

static int 
get_dir_gthreads(int fd, fileheader_t * base, int ent, int total, bool match, 
		void *arg)
{
	struct dir_gthread_set *ts = (struct dir_gthread_set *) arg;
	int i;
	int off = 1;
	int count = 0;
	int start = ent;
	int end = total;

	for (i = start; i <= end; i++)
	{
		if (count == ts->num)
			break;
		if (base[i-1].groupid == ts->groupid)
		{
			memcpy(ts->records + count, base + i - 1,
					sizeof(fileheader_t));
			count++;
		}
	}
	return count;
}

int 
get_threads_from_gid(const char *filename, int gid, fileheader_t *buf, int num)
{
    struct dir_gthread_set ts;
    fileheader_t key;
	int fd;
	int ret;

	if (num == 0)
		return 0;
    ts.records = buf;
    ts.num = num;
	ts.groupid = gid;
    bzero(&key, sizeof(key));
    key.id = gid;
	if ((fd = open(filename, O_RDWR, 0644)) < 0)
		return -1;
	ret = mmap_dir_search(fd, &key, get_dir_gthreads, &ts);
	close(fd);
	
	return ret;
}

//�������ַ���    by yuhuan
//��flyriverͬѧ����������������
int
Search_Bin(struct fileheader*ptr, int key, int start, int end)
{
        // ����������۰������ؼ��ֵ���key������Ԫ�ء�
        // �����ҵ�����������
        // ����Ϊ����key����С����Ԫ������m������(-m-1)
		// -1 ���Ź�������ʹ��
        int low, high, mid;
        struct fileheader *totest;
        low = start;
        high = end;
        while (low <= high) {
                mid = (low + high) / 2;
                totest = (struct fileheader *)(ptr + mid );
                if (key == totest->id)
                        return mid;
                else if (key < totest->id)
                        high = mid - 1;
                else
                        low = mid + 1;
        }
        return -(low+1);
}

char get_article_flag(struct fileheader *ent, struct userec *user, char *boardname, int is_bm)
{
    char unread_mark = (DEFINE(user, DEF_UNREADMARK) ? '*' : 'N');
    char type;

#ifdef HAVE_BRC_CONTROL
    if (strcmp(user->userid, "guest"))
        type = brc_unread(ent->id) ? unread_mark : ' ';
    else
#endif
        type = ' ';
    /* add by stiger */
    if(POSTFILE_BASENAME(ent->filename)[0] == 'Z')
	{
		if(type==' ')
			type='d';
		else
			type='D';
		return type;
    }
    /* add end */
    if ((ent->accessed[0] & FILE_DIGEST)) {
        if (type == ' ')
            type = 'g';
        else
            type = 'G';
    }
    if (ent->accessed[0] & FILE_MARKED) {
        switch (type) {
        case ' ':
            type = 'm';
            break;
        case '*':
        case 'N':
            type = 'M';
            break;
        case 'g':
            type = 'b';
            break;
        case 'G':
            type = 'B';
            break;
        }
    }
    if (is_bm && (ent->accessed[1] & FILE_READ)) { 
        switch (type) {
        case 'g':
        case 'G':
            type = 'O';
            break;
        case 'm':
        case 'M':
            type = 'U';
            break;
        case 'b':
        case 'B':
            type = '8';
            break;
        case ' ':
        case '*':
        case 'N':
        default:
            type = ';';
            break;
        }
    } else if ((is_bm || HAS_PERM(user, PERM_OBOARDS)) && (ent->accessed[0] & FILE_SIGN)) {
        type = '#';
#ifdef FILTER
#ifdef SMTH
    } else if ((ent->accessed[1] & FILE_CENSOR) 
        && ((!strcmp(boardname, FILTER_BOARD)&&HAS_PERM(user, PERM_OBOARDS))
        ||(!strcmp(boardname,"NewsClub")&&(haspostperm(user, "NewsClub")||HAS_PERM(user, PERM_OBOARDS))))) {
        type = '@';
#else
    } else if (HAS_PERM(user, PERM_OBOARDS) && (ent->accessed[1] & FILE_CENSOR) && !strcmp(boardname, FILTER_BOARD)) {
        type = '@';
#endif
#endif
    }

    if (is_bm && (ent->accessed[1] & FILE_DEL)) {
        type = 'X';
    }

    return type;
}

int Origin2(text)
    char text[256];
{
    char tmp[STRLEN];

    sprintf(tmp, "�� ��Դ:��%s ", BBS_FULL_NAME);
    if (strstr(text, tmp))
        return 1;
    else
        return 0;
}

int add_edit_mark(char *fname, int mode, char *title)
{
    FILE *fp, *out;
    char buf[256];
    time_t now;
    char outname[STRLEN];
    int step = 0;
    int added = 0;
    int asize;

    if ((fp = fopen(fname, "rb")) == NULL)
        return 0;
    sprintf(outname, "tmp/%d.editpost", getpid());
    if ((out = fopen(outname, "w")) == NULL) {
        fclose(fp);
        return 0;
    }

    while ((asize=-attach_fgets(buf, 256, fp)) != 0) {
        if (mode & 2) {
            if (step != 3 && !strncmp(buf, "��  ��: ", 8)) {
                step = 3;
                fprintf(out, "��  ��: %s\n", title);
                continue;
            }
        }
        
        if (!strncmp(buf, "[36m�� �޸�:��", 17))
            continue;
        if (Origin2(buf)) {
            now = time(0);
            if(mode & 1)
                fprintf(out, "[36m�� �޸�:��%s �� %15.15s �޸ı��š�[FROM: %15.15s][m\n", currentuser->userid, ctime(&now) + 4, fromhost);
            else
                fprintf(out, "[36m�� �޸�:��%s �� %15.15s �޸ı��ġ�[FROM: %15.15s][m\n", currentuser->userid, ctime(&now) + 4, fromhost);
            step = 3;
            added = 1;
        }
        fputs(buf, out);
        put_attach(fp, out, asize);
    }
    if (!added)
    {
        now = time(0);
        if(mode & 1)
            fprintf(out, "[36m�� �޸�:��%s �� %15.15s �޸ı��š�[FROM: %15.15s][m\n", currentuser->userid, ctime(&now) + 4, fromhost);
        else
            fprintf(out, "[36m�� �޸�:��%s �� %15.15s �޸ı��ġ�[FROM: %15.15s][m\n", currentuser->userid, ctime(&now) + 4, fromhost);
    }
    fclose(fp);
    fclose(out);

    f_mv(outname, fname);

    return 1;
}

char* checkattach(char *buf, long size,long *len,char** attachptr)
{
    char *ptr;
    if (size<ATTACHMENT_SIZE+sizeof(long)+2)
        return NULL;
    if (memcmp(buf, ATTACHMENT_PAD,ATTACHMENT_SIZE))
        return NULL;
    buf+=ATTACHMENT_SIZE;
    size-=ATTACHMENT_SIZE;

    ptr=buf;
    for (;size>0&&*buf!=0;buf++,size--);
    if (size==0)
        return NULL;
    buf++;
    size--;
    if (size<sizeof(long))
        return NULL;
    *len = ntohl(*(unsigned long*)buf);
    if (*len>size) *len=size;
    *attachptr=buf+sizeof(long);
    return ptr;
}

/**
 * һ���ܼ��attach��fgets
 * ����attach����1
 * �ļ�β����-1
 */
int skip_attach_fgets(char* s,int size,FILE* stream)
{
  int matchpos=0;
  int ch;
  char* ptr;
  ptr=s;
  while ((ch=fgetc(stream))!=EOF) {
     if (ch==ATTACHMENT_PAD[matchpos]) {
        matchpos++;
        if (matchpos==ATTACHMENT_SIZE) {
            int size, d;
            while(ch=fgetc(stream));
            fread(&d, 1, 4, stream);
            size = htonl(d);
            fseek(stream,size,SEEK_CUR);
            ptr-=ATTACHMENT_SIZE;
            matchpos=0;
            continue;
        }
     }
     *ptr=ch;
     ptr++;
     *ptr=0;
     if ((ptr-s)==size-1) {
//	     *(ptr-1)=0;
	     return 1;
     }
     if (ch=='\n') {
//        *ptr=0;
        return 1;
     }
  }
  if(ptr!=s) return 1;
  else return 0;
}

int attach_fgets(char* s,int size,FILE* stream)
{
  int matchpos=0;
  int ch;
  char* ptr;
  ptr=s;
  while ((ch=fgetc(stream))!=EOF) {
     if (ch==ATTACHMENT_PAD[matchpos]) {
        matchpos++;
        if (matchpos==ATTACHMENT_SIZE) {
            int size, d, count=8+4+1;
            while(ch=fgetc(stream))count++;
            fread(&d, 1, 4, stream);
            size = htonl(d);
            fseek(stream,-count,SEEK_CUR);
            *ptr=0;
            return -(count+size);
        }
     }
     *ptr=ch;
     ptr++;
     if ((ptr-s)>=size-2) {
	     *ptr=0;
	     return 1;
     }
     if (ch=='\n') {
        *ptr=0;
        return 1;
     }
  }
  if(ptr!=s) return 1;
  else return 0;
}

int put_attach(FILE* in, FILE* out, int size)
{
    char buf[1024*16];
    int i,o;
    if(size<=0) return -1;
    while(o=fread(buf, 1, 1024*16, in)) {
        size-=o;
        fwrite(buf, 1, o, out);
    }
    return 0;
}

int get_effsize(char * ffn)
{
    char* p,*op, *attach;
    long attach_len;
    int j;
    size_t fsize;
    int k,abssize=0,entercount=0,ignoreline=0;
    j = safe_mmapfile(ffn, O_RDONLY, PROT_READ, MAP_SHARED, (void **) &p, &fsize, NULL);
    op = p;
    if(j) {
        k=fsize;
        while(k) {
            if(NULL!=(checkattach(p, k, &attach_len, &attach))) {
                k-=(attach-p)+attach_len;
                p=attach+attach_len;
                continue;
            }
            if(k>=3&&*p=='\n'&&*(p+1)=='-'&&*(p+2)=='-'&&*(p+3)=='\n') break;
            if(*p=='\n') {
                entercount++;
                ignoreline=0;
            }
            if(k>=5&&*p=='\n'&&*(p+1)=='\xa1'&&*(p+2)=='\xbe'&&*(p+3)==' '&&*(p+4)=='\xd4'&&*(p+5)=='\xda') ignoreline=1;
            if(k>=2&&*p=='\n'&&*(p+1)==':'&&*(p+2)==' ') ignoreline=2;
            if(k>=2&&*p==KEY_ESC&&*(p+1)=='['&&*(p+2)=='m') ignoreline=3;
                
            k--;
            p++;
            if(entercount>=4&&!ignoreline)
                abssize++;
        }
    }
    end_mmapfile((void*)op, fsize, -1);
    return abssize;
}

long calc_effsize(char *fname)
{
    int ch;
    FILE *fp;
    int matched;
    char* ptr;
    long size;
	long effsize = 0;
	int insign=0;
	long signsize=0;

    if ((fp = fopen(fname, "r+b")) == NULL) {
		return 0;
	}

    matched=0;

    BBS_TRY {
        if (safe_mmapfile_handle(fileno(fp),  PROT_READ, MAP_SHARED, (void **) &ptr, (size_t *) & size) == 1) {
            char* data;
            long not;
            data=ptr;

			not = 0;

			if(! strncmp(data, "������:", 7) ){
				for( ;not < size; not++, data ++){
					if( *data == '\r' || *data == '\n' ){
						not++;
						data ++;
						if( *data == '\r' || *data == '\n' ){
							not++;
							data ++;
						}
						break;
					}
				}
			}

			if(! strncmp(data, "��  ��:", 7) ){
				for( ;not < size; not++, data ++){
					if( *data == '\r' || *data == '\n' ){
						not++;
						data ++;
						if( *data == '\r' || *data == '\n' ){
							not++;
							data ++;
						}
						break;
					}
				}
			}

			if(! strncmp(data, "����վ:", 7) ){
				for( ;not < size; not++, data ++){
					if( *data == '\r' || *data == '\n' ){
						not++;
						data ++;
						if( *data == '\r' || *data == '\n' ){
							not++;
							data ++;
						}
						break;
					}
				}
			}

            for (;not<size;not++,data++) {
                if (*data==0) {
                    matched++;
                    if (matched==ATTACHMENT_SIZE) {
                        int d, size;
						char *sstart = data;
                        data++; not++;
                        while(*data){
							data++;
							not++;
						}
                        data++;
                        not++;
                        memcpy(&d, data, 4);
                        size = htonl(d);
                        data+=4+size-1;
                        not+=4+size-1;
                        matched = 0;
						effsize += size;
                    }
                    continue;
                }
				else{
					if( *data != '\r' && *data != '\n' ){
						if(insign == 0)
							effsize ++;
						else
							signsize ++;
					}
				}
            }
        }
	else
	{
		BBS_RETURN(-1);
	}
    }
    BBS_CATCH {
    }
    BBS_END end_mmapfile((void *) ptr, size, -1);

    fclose(fp);
    return effsize;
}

/*
  dirarg��Ҫ������dir�ṹ
  id1,id2, ��ʼ���
  ɾ��ģʽ [del_mode = 0]���ɾ�� [1]��ͨɾ�� [2]ǿ��ɾ��
  TODO: use mmap
*/
int delete_range(struct write_dir_arg* dirarg,int id1,int id2,int del_mode,int curmode)
{
#define DEL_RANGE_BUF 2048
    struct fileheader *savefhdr;
    struct fileheader *readfhdr;
    struct fileheader *delfhdr;
    int count, totalcount, delcount, remaincount, keepcount;
    int pos_read, pos_write, pos_end;
    int i;

#ifdef BBSMAIN
    int savedigestmode;

    /*
     * curmode=4, 5�����λ�����������ɾ��,���߲�����,�������
     * ���ú����л����κεط�����, ����Ĵ����ǰ��ղ�����ɾ��д��,
     * ����Ϊ�������κ�Ե����ɵ���ʱ�ļ�����(�����Զ�ɾ����), ����
     * ������һ�´򿪲���; tmpfile�Ƿ��ÿ��ģʽ����, �������ֵ��
     * ��ȶ��.  -- ylsdd 
     */
    if ((curmode != DIR_MODE_NORMAL)&& (curmode != DIR_MODE_MAIL)) {   /* KCN:�ݲ����� */
        return 0;
    }
#endif                          /* 
                                 */
    prepare_write_dir(dirarg, NULL, curmode);
    pos_end = lseek(dirarg->fd, 0, SEEK_END);
    delcount = 0;
    if (pos_end == -1) {
        return -2;
    }
    totalcount = pos_end / sizeof(struct fileheader);
    pos_end = totalcount * sizeof(struct fileheader);
    if (id2 != -1) {
        pos_read = sizeof(struct fileheader) * id2;
    } else
        pos_read = pos_end;
    if (id2 == -1)
        id2 = totalcount;
    if (id1 != 0) {
        pos_write = sizeof(struct fileheader) * (id1 - 1);
        count = id1;
        if (id1 > totalcount) {
            if (dirarg->needlock)
                flock(dirarg->fd,LOCK_UN);
#ifdef BBSMAIN
            prints("��ʼ���ºŴ�����������");
            pressanykey();
#endif                          /* 
                                 */
            return 0;
        }
    } else {
        pos_write = 0;
        count = 1;
        id2 = totalcount;
    }
    if (id2 > totalcount) {
#ifdef BBSMAIN
        char buf[3];

        if (dirarg->needlock)
            flock(dirarg->fd,LOCK_UN);
        getdata(6, 0, "���±�Ŵ�������������ȷ��ɾ�� (Y/N)? [N]: ", buf, 2, DOECHO, NULL, true);
        if (dirarg->needlock)
            flock(dirarg->fd,LOCK_EX);
        if (*buf != 'Y' && *buf != 'y') {
            return -3;
        }
#else
        if (dirarg->needlock)
            flock(dirarg->fd,LOCK_UN);
        return -3;
#endif
        pos_read = pos_end;
        id2 = totalcount;
    }
    savefhdr = (struct fileheader *) malloc(DEL_RANGE_BUF * sizeof(struct fileheader));
    readfhdr = (struct fileheader *) malloc(DEL_RANGE_BUF * sizeof(struct fileheader));
    delfhdr = (struct fileheader *) malloc(DEL_RANGE_BUF * sizeof(struct fileheader));
    if ((id1 != 0) && (del_mode == 0)) {        /*rangle mark del */
        while (count <= id2) {
            int i;
            int readcount;

            lseek(dirarg->fd, pos_write, SEEK_SET);
            readcount = read(dirarg->fd, savefhdr, DEL_RANGE_BUF * sizeof(struct fileheader)) / sizeof(struct fileheader);
            for (i = 0; i < readcount; i++, count++) {
                if (count > id2)
                    break;      /*del end */
                if (!(savefhdr[i].accessed[0] & FILE_MARKED))
                    savefhdr[i].accessed[1] |= FILE_DEL;
            }
            lseek(dirarg->fd, pos_write, SEEK_SET);
            write(dirarg->fd, savefhdr, i * sizeof(struct fileheader));
            pos_write += i * sizeof(struct fileheader);
        }
        if (dirarg->needlock)
            flock(dirarg->fd,LOCK_UN);
        free(savefhdr);
        free(readfhdr);
        free(delfhdr);
        return 0;
    }
    remaincount = count - 1;
    keepcount = 0;
    lseek(dirarg->fd, pos_write, SEEK_SET);
    while (count <= id2) {
        int readcount;
        lseek(dirarg->fd, (count - 1) * sizeof(struct fileheader), SEEK_SET);
        readcount = read(dirarg->fd, savefhdr, DEL_RANGE_BUF * sizeof(struct fileheader)) / sizeof(struct fileheader);
/*        if (readcount==0) break; */
        for (i = 0; i < readcount; i++, count++) {
            if (count > id2)
                break;          /*del end */
            if (((savefhdr[i].accessed[0] & FILE_MARKED) && del_mode != 2)
                || ((id1 == 0) && (!(savefhdr[i].accessed[1] & FILE_DEL)))) {
                memcpy(&readfhdr[keepcount], &savefhdr[i], sizeof(struct fileheader));
                readfhdr[keepcount].accessed[1] &= ~FILE_DEL;
                keepcount++;
                remaincount++;
                if (keepcount >= DEL_RANGE_BUF) {
                    lseek(dirarg->fd, pos_write, SEEK_SET);
                    write(dirarg->fd, readfhdr, DEL_RANGE_BUF * sizeof(struct fileheader));
                    pos_write += keepcount * sizeof(struct fileheader);
                    keepcount = 0;
                }
            }
#ifdef BBSMAIN
            else if (curmode != DIR_MODE_MAIL) {
                int j;
                memcpy(&delfhdr[delcount], &savefhdr[i], sizeof(struct fileheader));
                delcount++;
                if (delcount >= DEL_RANGE_BUF) {
                    for (j = 0; j < DEL_RANGE_BUF; j++)
                        cancelpost(currboard->filename, currentuser->userid, &delfhdr[j], !strcmp(delfhdr[j].owner, currentuser->userid), 0);
                    delcount = 0;
                    setbdir(DIR_MODE_DELETED, genbuf, currboard->filename);
                    append_record(genbuf, (char *) delfhdr, DEL_RANGE_BUF * sizeof(struct fileheader));
                }
                /*
                 * need clear delcount 
                 */
            }
            else if (!strstr(dirarg->filename, ".DELETED")) {
                int j;
                memcpy(&delfhdr[delcount], &savefhdr[i], sizeof(struct fileheader));
                delcount++;
                if (delcount >= DEL_RANGE_BUF) {
                    delcount = 0;
                    setmailfile(genbuf, currentuser->userid, ".DELETED");
                    append_record(genbuf, (char *) delfhdr, DEL_RANGE_BUF * sizeof(struct fileheader));
                }
            }
            else {
               int j;
	        struct stat st;
               memcpy(&delfhdr[delcount], &savefhdr[i], sizeof(struct fileheader));
               delcount++;
               if (delcount >= DEL_RANGE_BUF) {
               	delcount = 0;
	        	for (j = 0; j < DEL_RANGE_BUF; j++){
	            		setmailfile(genbuf, currentuser->userid, delfhdr[j].filename);
	            		if (stat(genbuf, &st) !=-1) currentuser->usedspace-=st.st_size;
	        	}
               }
            }
#endif                          /* 
                                 */
        }                       /*for readcount */
    }
    if (keepcount) {
        lseek(dirarg->fd, pos_write, SEEK_SET);
        write(dirarg->fd, readfhdr, keepcount * sizeof(struct fileheader));
    }
    while (1) {
        int readcount;

        lseek(dirarg->fd, pos_read, SEEK_SET);
        readcount = read(dirarg->fd, savefhdr, DEL_RANGE_BUF * sizeof(struct fileheader)) / sizeof(struct fileheader);
        if (readcount == 0)
            break;
        lseek(dirarg->fd, remaincount * sizeof(struct fileheader), SEEK_SET);
        write(dirarg->fd, savefhdr, readcount * sizeof(struct fileheader));
        pos_read += readcount * sizeof(struct fileheader);
        remaincount += readcount;
    }
#ifdef DEBUG
#ifdef BBSMAIN
            newbbslog(BBSLOG_DEBUG,"%s ftruncate %d",
                dirarg->filename?dirarg->filename:currboard->filename,
                remaincount * sizeof(struct fileheader));
#endif
#endif      
    ftruncate(dirarg->fd, remaincount * sizeof(struct fileheader));
#ifdef BBSMAIN
    if ((curmode != DIR_MODE_MAIL) && delcount) {
        int j;

        for (j = 0; j < delcount; j++)
            cancelpost(currboard->filename, currentuser->userid, &delfhdr[j], !strcmp(delfhdr[j].owner, currentuser->userid), 0);
        setbdir(DIR_MODE_DELETED, genbuf, currboard->filename);
        append_record(genbuf, (char *) delfhdr, delcount * sizeof(struct fileheader));
    }
    else if (curmode==DIR_MODE_MAIL&&!strstr(dirarg->filename, ".DELETED")) {
        setmailfile(genbuf, currentuser->userid, ".DELETED");
        append_record(genbuf, (char *) delfhdr, delcount * sizeof(struct fileheader));
    }
    else if (curmode==DIR_MODE_MAIL) {
        struct stat st;
        int j;
        for (j = 0; j < delcount; j++){
            setmailfile(genbuf, currentuser->userid, delfhdr[j].filename);
            if (stat(genbuf, &st) !=-1) currentuser->usedspace-=st.st_size;
        }
    }
#endif
    if (dirarg->needlock)
        flock(dirarg->fd,LOCK_UN);
    free(savefhdr);
    free(readfhdr);
    free(delfhdr);
    return 0;
}

/* �����ö�����*/
int add_top(struct fileheader* fileinfo,char* boardname,int flag)
{
    struct fileheader top;
    char path[MAXPATH],newpath[MAXPATH],dirpath[MAXPATH];

    if (POSTFILE_BASENAME(fileinfo->filename)[0] == 'Z')
        return 3;
    memcpy(&top, fileinfo, sizeof(top));
    POSTFILE_BASENAME(top.filename)[0] = 'Z';
    setbfile(path, boardname, top.filename);
    top.accessed[0] = flag;
    setbfile(newpath, boardname, fileinfo->filename);
    setbdir(DIR_MODE_ZHIDING,dirpath,boardname);
    if (get_num_records(dirpath, sizeof(top)) > MAX_DIGEST) {
        return 4;
    }
    link(newpath, path);
    append_record(dirpath, &top, sizeof(top));
    board_update_toptitle(getbcache(boardname),1);
    return 0;
}

/*������ժ*/
int add_digest(struct fileheader* fileinfo,char* boardname)
{
    struct fileheader digest;
    char path[MAXPATH],newpath[MAXPATH],dirpath[MAXPATH];

    memcpy(&digest, fileinfo, sizeof(digest));
    digest.accessed[0] &= ~FILE_DIGEST;
    POSTFILE_BASENAME(digest.filename)[0] = 'G';
    setbfile(path, boardname, digest.filename);
    digest.accessed[0] = 0;
    setbfile(newpath, boardname, fileinfo->filename);
    setbdir(DIR_MODE_DIGEST,dirpath,boardname);
    if (get_num_records(dirpath, sizeof(digest)) > MAX_DIGEST) {
        return 4;
    }
    link(newpath, path);
    append_record(dirpath, &digest, sizeof(digest));    /* ��ժĿ¼����� .DIR */
    fileinfo->accessed[0] = fileinfo->accessed[0] | FILE_DIGEST;
}

/*ɾ����ժ*/
int dele_digest(char *dname, const char *boardname)
{                               /* ɾ����ժ��һƪPOST, dname=post�ļ���,boardname��������*/
    char digest_name[STRLEN];
    char new_dir[STRLEN];
    char buf[STRLEN];
    char *ptr;
    struct fileheader fh;
    int pos;

    strcpy(digest_name, dname);
    setbdir(DIR_MODE_DIGEST, new_dir, boardname);

    POSTFILE_BASENAME(digest_name)[0] = 'G';
    //TODO: ����Ҳ�и�ͬ������
    pos = search_record(new_dir, &fh, sizeof(fh), (RECORD_FUNC_ARG) cmpname, digest_name);      /* ��ժĿ¼�� .DIR�� ���� ��POST */
    if (pos <= 0) {
        return 2;
    }
    delete_record(new_dir, sizeof(struct fileheader), pos, (RECORD_FUNC_ARG) cmpname, digest_name);
    setbfile(buf, boardname, digest_name);
    my_unlink(buf);
    return 0;
}

#ifdef FILTER
int pass_filter(struct fileheader* fileinfo,struct boardheader* board)
{
#ifdef SMTH
        if ((!strcmp(board->filename, FILTER_BOARD)) ||(!strcmp(board->filename, "NewsClub")))
#else
        if (!strcmp(board->filename, FILTER_BOARD)) 
#endif
        {
            if (fileinfo->accessed[1] & FILE_CENSOR || fileinfo->o_board[0] == 0) {
                return 3;
            } else {
                char oldpath[MAXPATH],newpath[MAXPATH],dirpath[MAXPATH];
                struct fileheader newfh;
                int nowid;
                int filedes;
                
                fileinfo->accessed[1] |= FILE_CENSOR;
                setbfile(oldpath, board->filename, fileinfo->filename);
                setbfile(newpath, fileinfo->o_board, fileinfo->filename);
                f_cp(oldpath, newpath, 0);

                setbfile(dirpath, fileinfo->o_board, DOT_DIR);
                if ((filedes = open(dirpath, O_WRONLY | O_CREAT, 0664)) == -1) {
                    return -1;
                }
                newfh=*fileinfo;
                flock(filedes, LOCK_EX);
                nowid = get_nextid(fileinfo->o_board);
                newfh.id = nowid;
                if (fileinfo->o_id == fileinfo->o_groupid)
                    newfh.groupid = newfh.reid = newfh.id;
                else {
                    newfh.groupid = fileinfo->o_groupid;
                    newfh.reid = fileinfo->o_reid;
                }
                lseek(filedes, 0, SEEK_END);
                if (safewrite(filedes, &newfh, sizeof(fileheader)) == -1) {
                    bbslog("3user", "apprec write err! %s",newfh.filename);
                }
                flock(filedes, LOCK_UN);
                close(filedes);
                
                updatelastpost(fileinfo->o_board);
#ifdef HAVE_BRC_CONTROL
                brc_add_read(newfh.id);
#endif
                if (newfh.id == newfh.groupid)
                    setboardorigin(fileinfo->o_board, 1);
                setboardtitle(fileinfo->o_board, 1);
                if (newfh.accessed[0] & FILE_MARKED)
                    setboardmark(fileinfo->o_board, 1);
            }
        }
    return 0;
}
#endif                          /* FILTER */

/**
  ����fileheader������
  @param dirarg ��Ҫ������.DIR
  @param currmode ��ǰdir��ģʽ
  @param board ��ǰ����
  @param fileinfo ���½ṹ
  @param flag Ҫ�����ı�־
  @param data ���еĲ������ݡ�
  @param bmlog �Ƿ�������������¼
  TODO: ��������������ĵط����붼������Ȩ��
  @return 0 �ɹ�
              1 ����������
              2 �Ҳ���ԭ��
              3 ���������
              4 ��ժ��(�ö���)��
              -1 �ļ��򿪴���
  */
int change_post_flag(struct write_dir_arg* dirarg,int currmode, struct boardheader*board, 
        struct fileheader *fileinfo, int flag,struct fileheader * data,bool dobmlog)
{
    char buf[MAXPATH];
    struct fileheader* originFh;
    int ret=0;
    if ( fileinfo&&POSTFILE_BASENAME(fileinfo->filename)[0]=='Z' )
        /*�ö������²���������*/
        return 1;
    
    if ((flag == FILE_DIGEST_FLAG) && (currmode != DIR_MODE_NORMAL))
        /*�ڳ�����ͨ����������ժ����*/
        return 1;
    
    if (currmode == DIR_MODE_DELETED || currmode == DIR_MODE_JUNK)
        /*��ɾ��������ɾ������������*/
        return 1;
    
    if ((flag == FILE_MARK_FLAG || flag == FILE_DELETE_FLAG) && (!strcmp(board->filename, "syssecurity")
                                                                 || !strcmp(board->filename, FILTER_BOARD)))
    /*�ڹ��˰壬ϵͳ��¼���治����mark,���ɾ������*/
        return 1;       /* Leeward 98.03.29 */

    if (flag == FILE_TITLE_FLAG && currmode != DIR_MODE_NORMAL)
        /*����ͨģʽ�²����޸�����*/
        return 1;

    if (prepare_write_dir(dirarg, fileinfo, currmode)!=0)
        return 2;
    
    originFh=dirarg->fileptr+(dirarg->ent-1); /*�µ�fileheader*/
    setbfile(buf, board->filename, originFh->filename);

    /* mark ����*/
    if (flag&FILE_MARK_FLAG) {
        if (data->accessed[0] & FILE_MARKED) {
            originFh->accessed[0] |= FILE_MARKED;
            if (dobmlog)
                bmlog(currentuser->userid, board->filename, 7, 1);
        } else {
            originFh->accessed[0] &= ~FILE_MARKED;
            if (dobmlog)
                bmlog(currentuser->userid, board->filename, 6, 1);
        } 
        setboardmark(board->filename, 1);
    }
    
    /* ���ɻظ� ����*/
    if (flag&FILE_NOREPLY_FLAG) {
        if (!strcmp(board->filename, SYSMAIL_BOARD)) {
            char ans[STRLEN];
            sprintf(ans, "��%s�� ����: %s", currentuser->userid, fileinfo->title);
            strncpy(fileinfo->title, ans, STRLEN);
            originFh->title[STRLEN - 1] = 0;
        }
        if (data->accessed[1] & FILE_READ) {
            originFh->accessed[1] |= FILE_READ;
        } else {
            originFh->accessed[1] &= ~FILE_READ;
        } 
    }
    
    /* ��� ����*/
    if (flag&FILE_SIGN_FLAG) {
        if (data->accessed[0] & FILE_SIGN)
            originFh->accessed[0] |= FILE_SIGN;
        else
            originFh->accessed[0] &= ~FILE_SIGN;
    }

    /* ���ɾ�� ����*/
    if (flag&FILE_DELETE_FLAG) {
        if (data->accessed[1] & FILE_DEL)
            originFh->accessed[1] |= FILE_DEL;
        else
            originFh->accessed[1] &= FILE_DEL;
    }

    /* ������ժ����*/
    if (flag&FILE_DIGEST_FLAG) {
        if (data->accessed[0] & FILE_DIGEST)  {     /*����DIGEST*/ 
            if (dobmlog)
                bmlog(currentuser->userid, board->filename, 3, 1);
            ret=add_digest(originFh,board->filename);
        } else {/* ����Ѿ�����ժ�Ļ��������ժ��ɾ����post */
            originFh->accessed[0] = (originFh->accessed[0] & ~FILE_DIGEST);
            if (dobmlog)
                bmlog(currentuser->userid, board->filename, 4, 1);
            ret=dele_digest(originFh->filename, board->filename);
        }
    }
    if (ret==0) {
        if (flag&FILE_TITLE_FLAG) {
            originFh->groupid = originFh->id;
            originFh->reid = originFh->id;
            if (!strncmp(originFh->title, "Re: ", 4)) {
                strcpy(buf, originFh->title + 4);
                if (*buf!=0)
                    strcpy(originFh->title, buf);
            }
        }
        if (flag&FILE_IMPORT_FLAG) {
            if (data->accessed[0] & FILE_IMPORTED)
                originFh->accessed[0] |= FILE_IMPORTED;
            else
                originFh->accessed[0] &= ~FILE_IMPORTED;
        }
#ifdef FILTER
        if (flag&FILE_CENSOR_FLAG) {
            ret=pass_filter(originFh,board);
        }
#endif
        if (flag&FILE_ATTACHPOS_FLAG) {
            originFh->attachment=data->attachment;
        }
        if (flag&FILE_DING_FLAG) {
            ret=add_top(originFh,board->filename,data->accessed[0]);
        }
        if (flag&FILE_EFFSIZE_FLAG) {
            originFh->eff_size=data->eff_size;
        }
    }
    if ((currmode != DIR_MODE_NORMAL) && (currmode != DIR_MODE_DIGEST)) {
        /*��Ҫ����.DIR�ļ�*/
        char dirpath[MAXPATH];
        struct write_dir_arg dotdirarg;
        malloc_write_dir_arg(&dotdirarg);
        setbdir(DIR_MODE_NORMAL, dirpath, board->filename);
        dotdirarg.filename=dirpath;
        change_post_flag(&dotdirarg, DIR_MODE_NORMAL, board, originFh, flag,data,false);
        free_write_dir_arg(&dotdirarg);
    }
    if (dirarg->needlock)
        flock(dirarg->fd,LOCK_UN);
    return ret;
}


