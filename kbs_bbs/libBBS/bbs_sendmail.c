#include "bbs.h"
extern char *gb2big(char *, int *, int);
extern char *sysconf_str();

#ifdef HAVE_LIBESMTP
#include <libesmtp.h>
#endif

int getmailnum(char *recmaildir)
{                               /*Haohmaru.99.4.5.��Է��ż��� */
    struct fileheader fh;
    struct stat st;
    int fd;
    register int numfiles;

    if ((fd = open(recmaildir, O_RDONLY)) < 0)
        return 0;
    fstat(fd, &st);
    numfiles = st.st_size;
    numfiles = numfiles / sizeof(fh);
    close(fd);
    return numfiles;
}

/*
 * ��������Ƿ���
 */
int chkusermail(struct userec *user)
{
    char recmaildir[STRLEN], buf[STRLEN];
    int num, sum, sumlimit, numlimit, i;
    struct _mail_list usermail;

/*Arbitrator's mailbox has no limit, stephen 2001.11.1 */
    get_mail_limit(user, &sumlimit, &numlimit);
    /*
     * peregrine
     */
    if (sumlimit != 9999) {
        setmailfile(recmaildir, user->userid, DOT_DIR);
        num = getmailnum(recmaildir);
        setmailfile(recmaildir, user->userid, ".SENT");
        num += getmailnum(recmaildir);
        setmailfile(recmaildir, user->userid, ".DELETED");
        num += getmailnum(recmaildir);
        load_mail_list(user,&usermail);
        for (i = 0; i < usermail.mail_list_t; i++) {
            sprintf(buf, ".%s", usermail.mail_list[i] + 30);
            setmailfile(recmaildir, user->userid, buf);
            num += getmailnum(recmaildir);
        }
        sum = get_mailusedspace(user, 0) / 1024;
        if (num > numlimit || sum > sumlimit)
            return 1;
    }
    return 0;
}

/**
 * ���ɷ��Ÿ� receiver.
 * 
 * @return 0 ���Է���
 *         1 ���ܷ��ţ��������Ѿ���ɱ�򱻷�� mail Ȩ��
 *         2 ���ܷ��ţ����������䳬��
 *         3 ���ܷ��ţ����������䳬��
 */
int chkreceiver(struct userec *fromuser, struct userec *touser)

/*Haohmaru.99.4.4.��������������Ƿ���,�Ķ����������ʱ��ͬʱ�Ķ�do_send do_gsend doforward doforward����*/
{
    /*
     * Bigman 2000.9.8 : ����û���û��Ļ�,����0 
     */
    /*
     * ����PERM_SYSOP����ɱ�û����ź�Ĵ��� 
     */
    if (fromuser)
        if ((HAS_PERM(fromuser, PERM_SYSOP)) || (!strcmp(fromuser->userid, "Arbitrator")))
            /*
             * Leeward 99.07.28 , Bigman 2002.6.5: Arbitrator can send any mail to user 
             */
            return 0;
    if (touser->userlevel & PERM_SUICIDE)
        return 1;
    if (!(touser->userlevel & PERM_READMAIL))
        return 1;
    if (fromuser)
        if (chkusermail(fromuser))
            return 2;
    if (chkusermail(touser))
        return 3;
    return 0;
}
int check_query_mail(char qry_mail_dir[STRLEN])
{
    struct fileheader fh;
    struct stat st;
    int fd;
    register int offset;
    register long numfiles;
    unsigned char ch;

    offset = (int) ((char *) &(fh.accessed[0]) - (char *) &(fh));
    if ((fd = open(qry_mail_dir, O_RDONLY)) < 0)
        return 0;
    fstat(fd, &st);
    numfiles = st.st_size;
    numfiles = numfiles / sizeof(fh);
    if (numfiles <= 0) {
        close(fd);
        return 0;
    }
    lseek(fd, (st.st_size - (sizeof(fh) - offset)), SEEK_SET);
    /*
     * for(i = 0 ; i < numfiles ; i++) {
     * read(fd,&ch,1) ;
     * if(!(ch & FILE_READ)) {
     * close(fd) ;
     * return true ;
     * }
     * lseek(fd,-sizeof(fh)-1,SEEK_CUR);
     * } 
     */
    /*
     * ���߲�ѯ����ֻҪ��ѯ���һ���Ƿ�Ϊ���ţ�����������Ҫ 
     */
    /*
     * Modify by SmallPig 
     */
    read(fd, &ch, 1);
    if (!(ch & FILE_READ)) {
        close(fd);
        return true;
    }
    close(fd);
    return false;
}

int update_user_usedspace(int delta, struct userec *user)
{
    user->usedspace += delta;
    return 0;
}

int mail_file_sent(char *fromid, char *tmpfile, char *userid, char *title, int unlink)
{
    struct fileheader newmessage;
    struct stat st;
    char fname[STRLEN], filepath[STRLEN];
    char buf[255];

    memset(&newmessage, 0, sizeof(newmessage));
    strcpy(buf, fromid);        /* Leeward 98.04.14 */
    strncpy(newmessage.owner, buf, OWNER_LEN);
    newmessage.owner[OWNER_LEN-1] = 0;
    strncpy(newmessage.title, title, ARTICLE_TITLE_LEN - 1);
    newmessage.title[ARTICLE_TITLE_LEN-1] = 0;
    setmailpath(filepath, userid);
    if (stat(filepath, &st) == -1) {
        if (mkdir(filepath, 0755) == -1)
            return -1;
    } else {
        if (!(st.st_mode & S_IFDIR))
            return -1;
    }
    /*
     * setmailpath(filepath, userid, fname); 
     */
    setmailpath(filepath, userid);
    GET_MAILFILENAME(fname, filepath);
    strcpy(newmessage.filename, fname);
    setmailfile(filepath, userid, fname);
    /*
     * sprintf(genbuf, "cp %s %s",tmpfile, filepath) ;
     */
    if (unlink)
        f_mv(tmpfile, filepath);
    else
        f_cp(tmpfile, filepath, 0);
    if (stat(filepath, &st) != -1)
        currentuser->usedspace += st.st_size;
    setmailfile(buf, userid, ".SENT");
    newmessage.accessed[0] |= FILE_READ;
    if (append_record(buf, &newmessage, sizeof(newmessage)) == -1)
        return -1;
    newbbslog(BBSLOG_USER, "mailed %s ", userid);
    if (!strcasecmp(userid, "SYSOP"))
        updatelastpost(SYSMAIL_BOARD);
    return 0;

}

int mail_buf(struct userec*fromuser, char *mail_buf, char *userid, char *title)
{
    struct fileheader newmessage;
    struct stat st;
    char fname[STRLEN], filepath[STRLEN];
    char buf[255];
    struct userec *touser;      /*peregrine for updating used space */
    int unum;
    FILE* fp;

    unum = getuser(userid, &touser);
    if (touser == NULL)         /* flyriver, 2002.9.8 */
        return -1;
    memset(&newmessage, 0, sizeof(newmessage));
    strcpy(buf, fromuser->userid);        /* Leeward 98.04.14 */
    strncpy(newmessage.owner, buf, OWNER_LEN);
    newmessage.owner[OWNER_LEN - 1] = 0;
    strncpy(newmessage.title, title, ARTICLE_TITLE_LEN);
	newmessage.title[ARTICLE_TITLE_LEN - 1] = '\0';
    setmailpath(filepath, userid);
    if (stat(filepath, &st) == -1) {
        if (mkdir(filepath, 0755) == -1)
            return -1;
    } else {
        if (!(st.st_mode & S_IFDIR))
            return -1;
    }
    if (GET_MAILFILENAME(fname, filepath) < 0)
        return -1;
    strcpy(newmessage.filename, fname);
    setmailfile(filepath, userid, fname);

	fp = fopen(filepath, "w");
	if (fp != NULL) {
		write_header(fp, fromuser,1,NULL,title,0,0);
		fprintf(fp, "%s\n", mail_buf);
		fclose(fp);
	} else
		return -1;
    /*
     * peregrine update used space
     */
    if (stat(filepath, &st) != -1)
        touser->usedspace += st.st_size;

    setmailfile(buf, userid, DOT_DIR);

    if (append_record(buf, &newmessage, sizeof(newmessage)) == -1)
        return -1;
	setmailcheck( userid );

    newbbslog(BBSLOG_USER, "%s mailed %s ", fromuser->userid,userid);
    if (!strcasecmp(userid, "SYSOP"))
        updatelastpost(SYSMAIL_BOARD);
    return 0;
}

/*peregrine*/
int mail_file(char *fromid, char *tmpfile, char *userid, char *title, int unlinkmode, struct fileheader* fh)
{
    struct fileheader newmessage;
    struct stat st;
    char fname[STRLEN], filepath[STRLEN];
    char buf[255];
    struct userec *touser;      /*peregrine for updating used space */
    int unum;

    unum = getuser(userid, &touser);
    if (touser == NULL)         /* flyriver, 2002.9.8 */
        return -1;
    memset(&newmessage, 0, sizeof(newmessage));
    if (fh) {
        newmessage.attachment=fh->attachment;
    }
    strcpy(buf, fromid);        /* Leeward 98.04.14 */
    strncpy(newmessage.owner, buf, OWNER_LEN);
    newmessage.owner[OWNER_LEN - 1] = 0;
    strncpy(newmessage.title, title, ARTICLE_TITLE_LEN - 1);
	newmessage.title[ARTICLE_TITLE_LEN - 1] = '\0';
    setmailpath(filepath, userid);
    if (stat(filepath, &st) == -1) {
        if (mkdir(filepath, 0755) == -1)
            return -1;
    } else {
        if (!(st.st_mode & S_IFDIR))
            return -1;
    }
    if (GET_MAILFILENAME(fname, filepath) < 0)
        return -1;
    strcpy(newmessage.filename, fname);
    setmailfile(filepath, userid, fname);

    switch (unlinkmode) {
    case BBSPOST_LINK:
        unlink(filepath);
        sprintf(buf,"%s/%s",BBSHOME,tmpfile);
        if (symlink(buf,filepath)==-1)
		bbslog("3bbs","symlink %s to %s:%s",tmpfile,filepath,strerror(errno));
        break;
    case BBSPOST_MOVE:
        f_mv(tmpfile, filepath);
        break;
    case  BBSPOST_COPY:
        f_cp(tmpfile, filepath, 0);
        break;
    }
    /*
     * peregrine update used space
     */
    if (unlinkmode!=BBSPOST_LINK&&stat(filepath, &st) != -1)
        touser->usedspace += st.st_size;

    setmailfile(buf, userid, DOT_DIR);

    if (append_record(buf, &newmessage, sizeof(newmessage)) == -1)
        return -1;

	setmailcheck( userid );

    newbbslog(BBSLOG_USER, "%s mailed %s ", fromid,userid);
    if (!strcasecmp(userid, "SYSOP"))
        updatelastpost(SYSMAIL_BOARD);
    return 0;
}

char *email_domain()
{
    char *domain;

    /* �� MAIL_BBSDOMAIN �� BBSDOMAIN �ֿ� czz 03.03.08 */
    if (!(domain = sysconf_str("MAIL_BBSDOMAIN")))
	domain = sysconf_str("BBSDOMAIN");
    if (domain == NULL)
        domain = "unknown.BBSDOMAIN";
    /*
     * domain = MAIL_BBSDOMAIN; 
     */
    return domain;
}
struct mail_option {
    FILE *fin;
    int isbig5;
    int noansi;
    int bfirst;
    char* from;
    char* to;
};
#ifdef HAVE_LIBESMTP
#endif
char *bbs_readmailfile(char **buf, int *len, void *arg)
{
#define MAILBUFLEN	8192
    struct mail_option *pmo = (struct mail_option *) arg;
    char *retbuf;
    char *p, *pout;
    int i;
    char getbuf[MAILBUFLEN / 2];

    if (*buf == NULL)
        *buf = malloc(MAILBUFLEN);
    if (len == NULL) {
        rewind(pmo->fin);
        pmo->bfirst = 1;
        return NULL;
    }
    *len = fread(getbuf, 1, MAILBUFLEN / 2, pmo->fin);
    if (pmo->isbig5) {
        retbuf = gb2big(getbuf, len, 1);
    } else {
        retbuf = getbuf;
    }
    p = retbuf;
    pout = *buf;
    if (pmo->bfirst) {

        if (pmo->isbig5)
            sprintf(pout, "MIME-Version: 1.0\r\nContent-Type: text/plain; charset=big5\r\nContent-Transfer-Encoding: 8bit\r\nFrom: %s\r\nTo: %s\r\n\r\n",pmo->from,pmo->to);
        else
            sprintf(pout, "MIME-Version: 1.0\r\nContent-Type: text/plain; charset=gb2312\r\nContent-Transfer-Encoding: 8bit\r\nFrom: %s\r\nTo: %s\r\n\r\n",pmo->from,pmo->to);
        pout = *buf + strlen(*buf);
        pmo->bfirst = 0;
    }
    for (i = 0; i < *len; i++) {
        if ((*p == '\n') && ((i == 0) || (*(p - 1) != '\r'))) {
            *pout = '\r';
            pout++;
        }
        *pout = *p;
        pout++;
        p++;
    }
    *len = pout - (*buf);
    retbuf = *buf;
    if (pmo->noansi) {
        char *p1, *p2;
        int esc;

        p1 = retbuf;
        p2 = retbuf;
        esc = 0;
        for (i = 0; i < *len; i++, p1++) {
            if (esc) {
                if (*p1 == '\033') {
                    esc = 0;
                    *p2 = *p1;
                    p2++;
                } else if (isalpha(*p1))
                    esc = 0;
            } else {
                if (*p1 == '\033') {
                    esc = 1;
                } else {
                    *p2 = *p1;
                    p2++;
                }
            }
        }
        *p2 = 0;
        *len = p2 - retbuf;
    };
    return retbuf;
#undef MAILBUFLEN
}

#ifdef HAVE_LIBESMTP
void monitor_cb(const char *buf, int buflen, int writing, void *arg)
{
    FILE *fp = arg;

    if (writing == SMTP_CB_HEADERS) {
        fputs("H: ", fp);
        fwrite(buf, 1, buflen, fp);
        return;
    }
    fputs(writing ? "C >>>>\n" : "S <<<<\n", fp);
    fwrite(buf, 1, buflen, fp);
    if (buf[buflen - 1] != '\n')
        putc('\n', fp);
}

/* Callback to prnt the recipient status */
void print_recipient_status(smtp_recipient_t recipient, const char *mailbox, void *arg)
{
    const smtp_status_t *status;

    status = smtp_recipient_status(recipient);
#ifdef BBSMAIN
    prints("mail to %s: %d %s\n", mailbox, status->code, status->text);
#endif
}

static char* encodestring(const char* string,char* encode)
{
    char* encodestr;
	int len;
    len = strlen(string);
	encodestr=malloc((len+1)*2+8+strlen(encode)); //for gb2big5 +1,for base64 *2,for padding "=?GBK?B?" "=?BIG5?B?" "?=" +12
	sprintf(encodestr,"=?%s?B?",encode);
	to64frombits(encodestr+5+strlen(encode),string,len);
	strcat(encodestr,"?=");
	return encodestr;
}

int bbs_sendmail(char *fname, char *title, char *receiver, int isuu, int isbig5, int noansi)
{                               /* Modified by ming, 96.10.9  KCN,99.12.16 */
    struct mail_option mo;
    FILE *fin;
    char uname[STRLEN];
    char from[STRLEN];
    int len;
    smtp_session_t session;
    smtp_message_t message;
    smtp_recipient_t recipient;
    const smtp_status_t *status;
    enum notify_flags notify = Notify_NOTSET;
    char *server;
    char newbuf[257];
	char* encodestr;

    if (isuu) {
        char buf[256];

		gettmpfilename( uname, "uu" );
        //sprintf(uname, "tmp/uu%05d", getpid());
        sprintf(buf, "uuencode %s thbbs.%05d > %s", fname, getpid(), uname);
        system(buf);
    }
    if ((fin = fopen(isuu ? uname : fname, "r")) == NULL) {
#ifdef BBSMAIN
        prints("can't open %s: %s\n", isuu ? uname : fname, strerror(errno));
#endif
        return -1;
    }
    session = smtp_create_session();
    message = smtp_add_message(session);

/*
    if ((fout = fopen ("tmp/maillog", "w+")) == NULL)
    {
      prints("can't open %s: %s\n", "tmp/maillog", strerror (errno));
      return -1;
    }
    smtp_set_monitorcb (session, monitor_cb, fout, 1);
*/
    server = sysconf_str("MAILSERVER");
    /*
     * server = MAIL_MAILSERVER; 
     */
    if ((server == NULL) || !strcmp(server, "(null ptr)"))
        server = "127.0.0.1:25";
    smtp_set_server(session, server);
	smtp_set_hostname(session, email_domain());
    snprintf(newbuf, STRLEN, "%s(%s)",currentuser->userid, currentuser->username);
    newbuf[STRLEN-1]=0;
    if (isbig5) {
       len=strlen(newbuf);
       encodestr=gb2big(newbuf,&len,1);
       encodestr=encodestring(encodestr,"BIG5");
    } else {
       encodestr=encodestring(newbuf,"GBK");
    }
	strncpy(newbuf,encodestr,STRLEN-1);
    newbuf[STRLEN-1]=0;
    snprintf(from, STRLEN, "\"%s\" <%s@%s>",newbuf, currentuser->userid, email_domain());
    from[STRLEN-1]=0;
	free(encodestr);
    sprintf(newbuf, "%s@%s", currentuser->userid, email_domain());
    smtp_set_reverse_path(message, newbuf);
    smtp_set_header(message, "Message-Id", NULL);
    if (isbig5) {
        len=strlen(title);
		encodestr=gb2big(title,&len,1);
		encodestr=encodestring(encodestr,"BIG5");
    } else {
		encodestr=encodestring(title,"GBK");
    }
    smtp_set_header(message, "Subject", encodestr);
	free(encodestr);
    smtp_set_header_option(message, "Subject", Hdr_OVERRIDE, 3);
    /*
     * smtp_8bitmime_set_body(message, E8bitmime_8BITMIME); 
     */
    mo.isbig5 = isbig5;
    mo.noansi = noansi;
    mo.fin = fin;
    mo.bfirst = 1;
    mo.from = from;
    mo.to = receiver;
    smtp_set_messagecb(message, (smtp_messagecb_t) bbs_readmailfile, (void *) &mo);
    recipient = smtp_add_recipient(message, receiver);
    if (notify != Notify_NOTSET)
        smtp_dsn_set_notify(recipient, notify);
    /*
     * Initiate a connection to the SMTP server and transfer the
     * message. 
     */
    smtp_start_session(session);
    status = smtp_message_transfer_status(message);
#ifdef BBSMAIN
    prints("return code:%d(%s)\n", status->code, status->text);
#endif                          /* 
                                 */
    smtp_enumerate_recipients(message, print_recipient_status, NULL);
    /*
     * Free resources consumed by the program.
     */
    smtp_destroy_session(session);
    fclose(fin);
    if (isuu)
        unlink(uname);
    return (status->code != 250);
}

#else

int encode_imail_file(char *fromid, char *fromhost, char *fromip, char *to, char *oldfile, char *newfile, char *title){

	FILE *fp2;
	time_t now;
	char boundary[256];

	int fd;
	char *ptr;
	long size;
	int matched=0;

	snprintf(newfile, STRLEN, "%s.ib", oldfile);
	if((fp2=fopen(newfile, "w"))==NULL){
		return -1;
	}
	now = time(0);
	sprintf(boundary,"----=_%ld_%d.attach", now, rand());
	fprintf(fp2,"Return-Path: <%s@%s>\n", fromid, fromhost);
	fprintf(fp2,"Received: from %s by %s\n", fromip, fromhost);
	fprintf(fp2,"From: <%s@%s>\n", fromid, fromhost);
	fprintf(fp2,"To: %s\n", to);
	fprintf(fp2,"Date: %s", ctime(&now) );
	fprintf(fp2,"Reply-To: <%s@%s>\n", fromid, fromhost);
	fprintf(fp2,"Subject: %s\n", title);
	fprintf(fp2,"MIME-Version: 1.0\n");
	fprintf(fp2,"Content-Type: multipart/mixed;\n\tboundary=\"%s\"\n\n", boundary);

	fd=open(oldfile, O_RDONLY);
	if(fd<0)
		goto endencode;

    BBS_TRY { 
        if (safe_mmapfile_handle(fd, PROT_READ, MAP_SHARED, (void **) &ptr, (off_t *) & size
) == 1) {
            char *start,*end;
            long not;
            
            start = ptr;
            for (not = 0; not < size; ) {
				if (*start != 0) {
					int length;
					for(length=0,end=start;not<size && *end!=0;not++,end++,length++);
					fprintf(fp2, "--%s\nContent-Type: text/plain\n", boundary);
					fwrite(start, length, 1, fp2);
					fprintf(fp2, "\n\n");
					if(not >= size)
						break;
					start=end;
					matched=0;
				}
                if (*start == 0) {
                    matched++;
                    if (matched == ATTACHMENT_SIZE) {
                        int d;
                        long attsize;
						char *attfilename;
						char *base64old;
						char base64new[73];
						int i;

                        start++;
                        not++;
						attfilename = start;
                        while (*start) {
                            start++;
                            not++;
                        }

                        start++;
                        not++;
                        memcpy(&d, start, 4);
                        attsize = htonl(d);
                        start += 4;
                        not += 4;
						/* deal with attach */
						fprintf(fp2, "--%s\nContent-Type: text/plain;\tname=\"%s\"\nContent-Transfer-Encoding: base64\nContent-Disposition: attachment;\n\tfilename=\"%s\"\n\n", boundary, attfilename, attfilename);

						base64old = start;
						for(i=0; i<attsize; i+=54, base64old += 54){
							to64frombits (base64new, base64old, attsize-i>54?54:(attsize-i) );
							base64new[72]=0;
							fprintf(fp2,"%s\n",base64new);
						}

						fprintf(fp2, "\n\n");
						/* deal end */
						start += attsize;
						not += attsize;
                        matched = 0;
                    }else{
						start++;
						not++;
					}
                }
			}
		}else{
			BBS_RETURN(-1);
		}
	}
	BBS_CATCH{
	}
	BBS_END end_mmapfile((void*)ptr, size, -1);

	close(fd);
endencode:

	fprintf(fp2, "--%s--\n", boundary);
	fclose(fp2);

	return 0;
}

void my_ansi_filter(char *source)
{
    char result[500];
    int i, flag = 0, loc = 0;
    int len;

    len = strlen(source);
    len = len >= sizeof(result) ? sizeof(result) - 1 : len;
    for (i = 0; i < len; i++) {
        if (source[i] == '\x1B') {
            flag = 1;
            continue;
        } else if (flag == 1 && isalpha(source[i])) {
            flag = 0;
            continue;
        } else if (flag == 1) {
            continue;
        } else {
            result[loc++] = source[i];
        }
    }
    result[loc] = '\0';
    strncpy(source, result, loc + 1);
}

int bbs_sendmail(char *fname, char *title, char *receiver, int isuu, int isbig5, int noansi)
{                               /* Modified by ming, 96.10.9  KCN,99.12.16 */
    FILE *fin;
    char newbuf[257];
	FILE *fout;
	char gbuf[256];

	sprintf(gbuf, "%s -f %s@%s %s", OWNSENDMAIL, currentuser->userid, MAIL_BBSDOMAIN, receiver);

	if( encode_imail_file(currentuser->userid, MAIL_BBSDOMAIN, currentuser->lasthost, receiver, fname, newbuf, title) != 0 )
	    return -1;

	fout = popen(gbuf, "w");
	if(fout == NULL)
		return -1;
	fin = fopen(newbuf, "r");
	if(fin==NULL){
		pclose(fout);
		return -1;
	}

	while (fgets(gbuf, 255, fin) != NULL) {
		if(noansi)  my_ansi_filter(gbuf);
		if (gbuf[0] == '.' && gbuf[1] == '\n')
			fputs(". \n", fout);
		else
			fputs(gbuf, fout);
	}

	fprintf(fout, ".\n");
	fclose(fin);
    pclose(fout);

	unlink(newbuf);
	return 0;
}
#endif

