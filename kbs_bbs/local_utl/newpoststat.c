/* ʹ��mysql��ʮ��ͳ�ƹ��ܣ� 2004.5.25 stiger */

/*
   select bname,threadid,count(distinct userid) as count from postlog WHERE time<NOW() GROUP BY bname,threadid order by count desc;
*/
#include <stdio.h>
#include <time.h>
#include "bbs.h"
#include "urlencode.c"

#ifdef NEWPOSTSTAT

#define	DELETE

char *myfile[] = { "day", "week", "month", "year", "bless" };
int mytop[] = { 10, 50, 100, 100, 10 };
char *mytitle[] = { "��ʮ�����Ż���",
    "����ʮ�����Ż���",
    "�°ٴ����Ż���",
    "��Ȱٴ����Ż���",
    "��ʮ������ף��"
};

#define TOPCOUNT 100
#define SECTOPCOUNT 10
#define MAXCMP 10000
#ifdef BLESS_BOARD
const char *surfix_bless[23] = {
	" ",
    "  \x1b[1;34m��  ",
    "\x1b[1;32m��\x1b[0;32m��\x1b[1m��",
    "\x1b[0;32m��\x1b[1m��\x1b[0;32m��",
    "\x1b[0;32m��\x1b[1m��\x1b[0;32m��",
    "\x1b[1;32m��\x1b[0;32m��\x1b[1m��",
    "  \x1b[0;32m��  ",
    "\x1b[1;32m��\x1b[0;32m��\x1b[1m��",
    "\x1b[0;32m��\x1b[1mף\x1b[0;32m��",
    "\x1b[0;32m��\x1b[1m��\x1b[0;32m��",
    "\x1b[1;32m��\x1b[0;32m��\x1b[1m��",
    "  \x1b[1;34m��  ",
    "  \x1b[1;34m��  ",
    "\x1b[1;32m��\x1b[0;32m��\x1b[1m��",
    "\x1b[0;32m��\x1b[1mУ\x1b[0;32m��",
    "\x1b[0;32m��\x1b[1m��\x1b[0;32m��",
    "\x1b[1;32m��\x1b[0;32m��\x1b[1m��",
    "  \x1b[32m��  ",
    "\x1b[1;32m��\x1b[0;32m��\x1b[1m��",
    "\x1b[0;32m��\x1b[1m��\x1b[0;32m��",
    "\x1b[0;32m��\x1b[1m��\x1b[0;32m��",
    "\x1b[1;32m��\x1b[0;32m��\x1b[1m��",
    "  \x1b[1;34m��  "
};
#endif

struct postrec {
	char userid[IDLEN+1];
    char board[BOARDNAMELEN];      /* board name */
	unsigned int groupid;
    time_t date;                /* last post's date */
    int number;                 /* post number */
	char title[81];
};

struct postrec top[TOPCOUNT];
struct postrec sectop[SECNUM][SECTOPCOUNT];

int sectopnum[SECNUM];
int sectopnumtotal=0;

int topnum=0;
#ifdef SMTH
int topnum1=0;
#endif

#define INTERVAL 200

static char * get_file_title(char *boardname, int threadid, char *title, char *userid)
{

	char dirfile[256];
	int fd;
	struct fileheader fh;

            sprintf(dirfile, "boards/%s/.DIR", boardname);
			if ((fd = open(dirfile, O_RDWR, 0644)) < 0)
				return NULL;

    		if( get_records_from_id(fd, threadid, &fh, 1, NULL) == 0 ){
				close(fd);
				return NULL;
			}
			close(fd);

			strncpy(title, fh.title, 80);
			title[80]=0;

			strncpy(userid, fh.owner, IDLEN);
			userid[IDLEN]='\0';

			return title;
}

extern const char seccode[SECNUM][5];
static int get_seccode_index(char prefix)
{
    int i;

    for (i = 0; i < SECNUM; i++) {
        if (strchr(seccode[i], prefix) != NULL)
            return i;
    }
    return -1;
}

/*********��¼ʮ����Ϣ��toplog��*********/
/*
create table `toplog` (
 `id` int unsigned NOT NULL auto_increment,
 `userid` char(15) NOT NULL default '',
 `bname` char(31) NOT NULL default '',
 `title` char(81) NOT NULL default '',
 `time` timestamp NOT NULL,
 `date` date NOT NULL,
 `topth` int NOT NULL default '1',
 `count` int NOT NULL default '0',
 `threadid` int unsigned NOT NULL default '0',
 PRIMARY KEY (`id`),
 KEY userid (`userid`),
 KEY bname(`bname`, `threadid`),
 KEY date(`date`),
 UNIQUE top (`date`,`topth`)
) TYPE=MyISAM COMMENT='toplog';
*/
int log_top()
{
	MYSQL s;
	char sqlbuf[500];
	char newtitle[161];
	int i;
	char newts[20];

	mysql_init (&s);

	if (! my_connect_mysql(&s) ){
		return 0;;
	}

	for(i=0;i<topnum;i++){
		
		mysql_escape_string(newtitle, top[i].title, strlen(top[i].title));

		sprintf(sqlbuf,"UPDATE toplog SET userid='%s',bname='%s',title='%s',count='%d',time='%s',threadid='%d' WHERE date=CURDATE() AND topth='%d';", top[i].userid, top[i].board, newtitle, top[i].number, tt2timestamp(top[i].date,newts), top[i].groupid, i+1);

		if( mysql_real_query( &s, sqlbuf, strlen(sqlbuf) )){
			printf("%s\n", mysql_error(&s));
			continue;
		}
		if( (int)mysql_affected_rows(&s) <= 0 ){
			sprintf(sqlbuf, "INSERT INTO toplog VALUES (NULL,'%s','%s','%s','%s',CURDATE(),'%d','%d','%d');",top[i].userid, top[i].board, newtitle, tt2timestamp(top[i].date,newts), i+1, top[i].number, top[i].groupid);
			if( mysql_real_query( &s, sqlbuf, strlen(sqlbuf) )){
				printf("%s\n", mysql_error(&s));
				continue;
			}
		}
	}

	mysql_close(&s);

	return 1;
}

/***********
  ����type�õ�ʮ���б�,�Ѿ����������һϵ�м��,����ֱ�����
  type!=4��ʱ�򻹵õ�����ʮ��
  *******/
int get_top(int type)
{

	MYSQL s;
	MYSQL_RES *res;
	MYSQL_ROW row;
	char sqlbuf[500];
	char cmptime[100];
	int start=0;
	int i,secid;
	int threadid;
	char title[81];
	char userid[IDLEN+1];
	int m,n;
#ifdef BLESS_BOARD
	struct boardheader *bh;
#endif

	topnum = 0;
#ifdef SMTH
	topnum1 = 0;
#endif

	if(type < 0 || type > 4)
		return 0;

	mysql_init (&s);

	if (! my_connect_mysql(&s) ){
		return 0;;
	}

	if(type==0 || type==4){
		sprintf(cmptime,"YEAR(time)=YEAR(CURDATE()) AND MONTH(time)=MONTH(CURDATE()) AND DAYOFMONTH(time)=DAYOFMONTH(CURDATE())");
	}else if(type==1){
		sprintf(cmptime,"YEAR(date)=YEAR(CURDATE()) AND WEEK(date)=WEEK(CURDATE())");
	}else if(type==2){
		sprintf(cmptime,"YEAR(date)=YEAR(CURDATE()) AND MONTH(date)=MONTH(CURDATE())");
	}else if(type==3){
		sprintf(cmptime,"YEAR(date)=YEAR(CURDATE())");
	}

	bzero(top, TOPCOUNT * sizeof(struct postrec));
	bzero(sectop, TOPCOUNT * sizeof(struct postrec));

	for(i=0;i<SECNUM;i++) sectopnum[i]=0;
	sectopnumtotal=0;

	while(1){
		if(type==4){
#ifdef SMTH
			if(topnum>=5 && topnum1>=5)
				break;
#else
			if(topnum>=mytop[type])
				break;
#endif
		}else if(type==0){
			if(topnum>=mytop[type] && sectopnumtotal>=SECNUM*SECTOPCOUNT)
				break;
		}else{
			if(topnum >= mytop[type])
				break;
		}

		if(start > MAXCMP)
			break;

		if(type==0 || type==4)
			sprintf(sqlbuf,"SELECT bname,threadid,MAX(time) AS maxtime,count(DISTINCT userid) AS count FROM postlog WHERE %s GROUP BY bname,threadid ORDER BY count desc LIMIT %d,%d;", cmptime, start, INTERVAL);
		else
			sprintf(sqlbuf,"SELECT bname,threadid,time,count,title,userid FROM toplog WHERE %s ORDER BY count desc LIMIT %d,%d",cmptime,start, INTERVAL);
		
		if( mysql_real_query( &s, sqlbuf, strlen(sqlbuf) )){
			mysql_close(&s);
			
			return topnum;
		}

		res = mysql_store_result(&s);

		while(1){
			row = mysql_fetch_row(res);
			if(row==NULL)
				break;

			/***����Ƿ�ü���ʮ��***/
#ifdef BLESS_BOARD
			bh = getbcache(row[0]);
			if(bh==NULL || bh->flag & BOARD_POSTSTAT){
				continue;
			}
			if(type==0){
				if ( ! strcasecmp(row[0], BLESS_BOARD) 
#ifdef SMTH
						|| bh->group==503 || bh->group==552
#endif
								){
					continue;
				}
			}else if(type==4){
				if ( strcasecmp(row[0], BLESS_BOARD) 
#ifdef SMTH
						&& bh->group!=503 && bh->group!=552
#endif
								){
					continue;
				}
			}
#endif

			secid= get_seccode_index(bh->title[0]);

			if(topnum >= mytop[type] && ( secid==-1 || sectopnum[secid] >= SECTOPCOUNT))
				continue;

			threadid = atoi(row[1]);
			if(type==0 || type==4){
				if(get_file_title(row[0], threadid, title, userid) == NULL){
					continue;
				}
			}else{
				strncpy(title, row[4], 80);
				title[80]=0;
				strncpy(userid, row[5], IDLEN);
				userid[IDLEN]=0;
			}
/**һ���������3��ʮ��**/
#ifndef NINE_BUILD
			if(type==0){
                m = 0;
                for (n = 0; n < topnum; n++) {
                    if (!strcmp(row[0], top[n].board))
                        m++;
                }
				if(m>2)
					continue;

				/***����ʮ����һ������Ҳ���3��***/
				if(secid!=-1){
                	m = 0;
                	for (n = 0; n < sectopnum[i]; n++) {
                    	if (!strcmp(row[0], sectop[secid][n].board))
                        	m++;
                	}
					if(m>2)
						continue;
				}
			}else if(type==4){
#ifdef SMTH
				if(strcasecmp(row[0], BLESS_BOARD)){
                	m = 0;
                	for (n = 0; n < topnum1; n++) {
                    	if (!strcmp(row[0], top[5+n].board))
                        	m++;
                	}
					if(m>1)
						continue;
				}
#endif
			}
#endif

			/***�ȼ�¼����ʮ���ֵ***/
#ifdef SMTH
			if(type==4 && strcasecmp(row[0], BLESS_BOARD) ){
				if(topnum1>=5)
					continue;
			strncpy(top[5+topnum1].board, row[0], BOARDNAMELEN);
			top[5+topnum1].board[BOARDNAMELEN-1]='\0';
			top[5+topnum1].groupid = threadid;
			strncpy(top[5+topnum1].title, title, 80);
			top[5+topnum1].title[80]='\0';
			strncpy(top[5+topnum1].userid, userid, IDLEN);
			top[5+topnum1].userid[IDLEN]='\0';
			top[5+topnum1].date = timestamp2tt(row[2]);
			top[5+topnum1].number = atoi(row[3]);

			topnum1++;
			}else{
				if(type==4 && topnum>=5)
					continue;
#endif
			if(topnum < mytop[type]){

			strncpy(top[topnum].board, row[0], BOARDNAMELEN);
			top[topnum].board[BOARDNAMELEN-1]='\0';
			top[topnum].groupid = threadid;
			strncpy(top[topnum].title, title, 80);
			top[topnum].title[80]='\0';
			strncpy(top[topnum].userid, userid, IDLEN);
			top[topnum].userid[IDLEN]='\0';
			top[topnum].date = timestamp2tt(row[2]);
			top[topnum].number = atoi(row[3]);

			topnum++;

			}
#ifdef SMTH
			}
#endif

			/***�������ʮ��***/
			if(type==0){

			i=secid;
			if(i!=-1){
				if( sectopnum[i] < SECTOPCOUNT){

			strncpy(sectop[i][sectopnum[i]].board, row[0], BOARDNAMELEN);
			sectop[i][sectopnum[i]].board[BOARDNAMELEN-1]='\0';
			sectop[i][sectopnum[i]].groupid = threadid;
			strncpy(sectop[i][sectopnum[i]].title, title, 80);
			sectop[i][sectopnum[i]].title[80]='\0';
			strncpy(sectop[i][sectopnum[i]].userid, userid, IDLEN);
			sectop[i][sectopnum[i]].userid[IDLEN]='\0';
			sectop[i][sectopnum[i]].date = timestamp2tt(row[2]);
			sectop[i][sectopnum[i]].number = atoi(row[3]);

					sectopnum[i]++;
					sectopnumtotal++;
				}
			}

			}//type==0

			if(type==4){
#ifdef SMTH
				if(topnum>=5 && topnum1>=5)
					break;
#else
				if(topnum>=mytop[type])
					break;
#endif
			}else if(type==0){
				if(topnum >= mytop[type] && sectopnumtotal >= SECNUM*SECTOPCOUNT)
					break;
			}else{
				if(topnum >= mytop[type])
					break;
			}

		}

		mysql_free_result(res);

		start += INTERVAL;

	}
	
	mysql_close(&s);

	return topnum;

}

/*
 * mytype 0 ����
 *        1 ����
 *        2 ����
 *        3 ����
 *        4 ף����
 */
void writestat(int mytype)
{
	int i;
	char buf[256];
	char *p;
	char curfile[256];
	FILE *fp;

    sprintf(curfile, "etc/posts/%s", myfile[mytype]);
    if ((fp = fopen(curfile, "w")) != NULL) {
#ifdef SMTH
	if( mytype == 4) {
        fprintf(fp, "              \x1b[1;33m���� \x1b[31m��\x1b[33m��\x1b[32m�� \x1b[41;32m  \x1b[33m�����������ף��  \x1b[m\x1b[1;32m ��\x1b[31m��\x1b[33m�� ����\x1b[m               %s\x1b[m\n", surfix_bless[0]);

        for (i = 0; i < topnum; i++) {

            strcpy(buf, ctime(&top[i].date));
            buf[20] = NULL;
            p = buf + 4;

            fprintf(fp,
                        "                                            \x1b[33m%s \x1b[1;31m%4d\x1b[0;37m��      %s\x1b[m\n"
                        "\x1b[1m��\x1b[31m%2d \x1b[37m�� \x1b[4%dm %-51.51s\x1b[m \x1b[1;33m%-12s%s\x1b[m\n",
                        p, top[i].number, surfix_bless[(i) * 2 + 1], i+1, (i) / 2 + 1, top[i].title, top[i].userid, surfix_bless[(i+1) * 2]);

		}
		for( ; i < 5; i++){
            fprintf(fp,
                        "                                                                         %s\x1b[m\n"
                        "                                                                         %s\x1b[m\n",
                        surfix_bless[i * 2 +1], surfix_bless[(i+1) * 2 ] );
		}
        fprintf(fp, "                                                                         %s\x1b[m\n", surfix_bless[11]);

        fprintf(fp, "              \x1b[1;33m���� \x1b[31m��\x1b[33m��\x1b[32m�� \x1b[41;32m  \x1b[33m�������У�ڻ���  \x1b[m\x1b[1;32m ��\x1b[31m��\x1b[33m�� ����\x1b[m               %s\x1b[m\n", surfix_bless[12]);

        for (i = 0; i < topnum1; i++) {

            strcpy(buf, ctime(&top[5+i].date));
            buf[20] = NULL;
            p = buf + 4;

            fprintf(fp,
                        "        \x1b[33m%-20.20s                %s \x1b[1;31m%4d\x1b[0;37m��      %s\x1b[m\n"
                        "\x1b[1m��\x1b[31m%2d \x1b[37m�� \x1b[4%dm %-51.51s\x1b[m \x1b[1;33m%-12s%s\x1b[m\n",
                        top[5+i].board, p, top[5+i].number, surfix_bless[(i + 6) * 2 + 1], i+1, (i) / 2 + 1, top[5+i].title, top[5+i].userid, surfix_bless[(i+7) * 2 ]);

		}
		for( ; i < 5; i++){
            fprintf(fp,
                        "                                                                         %s\x1b[m\n"
                        "                                                                         %s\x1b[m\n",
                        surfix_bless[(i+6) * 2 + 1], surfix_bless[(i+7) * 2 ] );
		}

		fclose(fp);
		return;
	}
#endif

#ifdef BLESS_BOARD
        if (mytype == 4)
            fprintf(fp, "              \x1b[1;33m���� \x1b[31m��\x1b[33m��\x1b[32m�� \x1b[41;32m  \x1b[33m����ʮ������ף��  \x1b[m\x1b[1;32m ��\x1b[31m��\x1b[33m�� ����\x1b[m\n\n");
        else
#endif
            fprintf(fp, "                \033[34m-----\033[37m=====\033[41m ��%s \033[m=====\033[34m-----\033[m\n\n", mytitle[mytype]);

        for (i = 0; i < topnum; i++) {

            strcpy(buf, ctime(&top[i].date));
            buf[20] = NULL;
            p = buf + 4;


#ifdef BLESS_BOARD
            if (mytype == 4)
                fprintf(fp,
                        "                                            %s \x1b[1;31m%4d\x1b[0;37m��      %s\x1b[m\n"
                        "\x1b[1m��\x1b[31m%2d \x1b[37m�� \x1b[4%dm %-51.51s\x1b[m \x1b[1;33m%-12s%s\x1b[m\n",
                        p, top[i].number, surfix_bless[(i) * 2], i+1, (i) / 2 + 1, top[i].title, top[i].userid, surfix_bless[(i) * 2 + 1]);
            else
#endif
                fprintf(fp,
                        "\033[37m��\033[31m%3d\033[37m �� \033[37m���� : \033[33m%-16s\033[37m��\033[32m%s\033[37m��\033[36m%4d \033[37m��\033[35m%16s\n"
                        "     \033[37m���� : \033[44m\033[37m%-60.60s\033[m\n", i + 1, top[i].board, p, top[i].number, top[i].userid, top[i].title);
        }

#ifdef BLESS_BOARD
        if (mytype == 4)
            fprintf(fp, "                                                                         %s\x1b[m", surfix_bless[20]);
#endif
        fclose(fp);
    }
}

void gen_sec_hot_subjects_xml(int mytype, int secid)
{
	FILE *fp;
	char curfile[256];
	char xml_buf[256];
	char url_buf[256];
	int i;

    sprintf(curfile, "xml/%s_sec%d.xml", myfile[mytype], secid);
    if ((fp = fopen(curfile, "w")) != NULL) 
	{
		fprintf(fp, "<?xml version=\"1.0\" encoding=\"GBK\"?>\n");
		fprintf(fp, "<hotsubjects>\n");

        for (i = 0; i < sectopnum[secid]; i++) 
		{

			fprintf(fp, "<hotsubject>\n");
			fprintf(fp, "<title>%s</title>\n", encode_url(url_buf,encode_xml(xml_buf, sectop[secid][i].title, 
						sizeof(xml_buf)),sizeof(url_buf)));
			fprintf(fp, "<author>%s</author>\n", encode_url(url_buf,sectop[secid][i].userid,sizeof(url_buf)));
			fprintf(fp, "<board>%s</board>\n", encode_url(url_buf,sectop[secid][i].board,sizeof(url_buf)));
			fprintf(fp, "<time>%d</time>\n", sectop[secid][i].date);
			fprintf(fp, "<number>%d</number>\n", sectop[secid][i].number);
			fprintf(fp, "<groupid>%d</groupid>\n", sectop[secid][i].groupid);
			fprintf(fp, "</hotsubject>\n");
        }
		fprintf(fp, "</hotsubjects>\n");

        fclose(fp);
    }
}

void gen_secs_hot_subjects_xml(int mytype)
{
	int i;

	for(i=0; i<SECNUM; i++){
		gen_sec_hot_subjects_xml(mytype, i);
	}
}

void gen_blessing_list_xml()
{
	FILE *fp;
	char curfile[256];
	char xml_buf[256];
	char url_buf[256];
	int i;

    sprintf(curfile, "xml/%s.xml", myfile[4]);
    if ((fp = fopen(curfile, "w")) != NULL) 
	{
		fprintf(fp, "<?xml version=\"1.0\" encoding=\"GBK\"?>\n");
		fprintf(fp, "<hotsubjects>\n");

        for (i = 0; i < topnum; i++) 
		{
			fprintf(fp, "<hotsubject>\n");
			fprintf(fp, "<title>%s</title>\n", encode_url(url_buf,encode_xml(xml_buf, top[i].title, 
						sizeof(xml_buf)),sizeof(url_buf)));
			fprintf(fp, "<author>%s</author>\n", encode_url(url_buf,top[i].userid,sizeof(url_buf)));
			fprintf(fp, "<board>%s</board>\n", encode_url(url_buf,top[i].board,sizeof(url_buf)));
			fprintf(fp, "<time>%d</time>\n", top[i].date);
			fprintf(fp, "<number>%d</number>\n", top[i].number);
			fprintf(fp, "<groupid>%d</groupid>\n", top[i].groupid);
			fprintf(fp, "</hotsubject>\n");
        }
#ifdef SMTH
        for (i = 5; i < topnum1+5; i++) 
		{
			fprintf(fp, "<hotsubject>\n");
			fprintf(fp, "<title>%s</title>\n", encode_url(url_buf,encode_xml(xml_buf, top[i].title, 
						sizeof(xml_buf)),sizeof(url_buf)));
			fprintf(fp, "<author>%s</author>\n", encode_url(url_buf,top[i].userid,sizeof(url_buf)));
			fprintf(fp, "<board>%s</board>\n", encode_url(url_buf,top[i].board,sizeof(url_buf)));
			fprintf(fp, "<time>%d</time>\n", top[i].date);
			fprintf(fp, "<number>%d</number>\n", top[i].number);
			fprintf(fp, "<groupid>%d</groupid>\n", top[i].groupid);
			fprintf(fp, "</hotsubject>\n");
        }
#endif
		fprintf(fp, "</hotsubjects>\n");

        fclose(fp);
    }
}

void gen_hot_subjects_xml(int mytype)
{
	FILE *fp;
	char curfile[256];
	char xml_buf[256];
	char url_buf[256];
	int i;

	if(mytype==4)
		return gen_blessing_list_xml();

    sprintf(curfile, "xml/%s.xml", myfile[mytype]);
    if ((fp = fopen(curfile, "w")) != NULL) 
	{
		fprintf(fp, "<?xml version=\"1.0\" encoding=\"GBK\"?>\n");
		fprintf(fp, "<hotsubjects>\n");

        for (i = 0; i < topnum; i++) 
		{
			fprintf(fp, "<hotsubject>\n");
			fprintf(fp, "<title>%s</title>\n", encode_url(url_buf,encode_xml(xml_buf, top[i].title, 
						sizeof(xml_buf)),sizeof(url_buf)));
			fprintf(fp, "<author>%s</author>\n", encode_url(url_buf,top[i].userid,sizeof(url_buf)));
			fprintf(fp, "<board>%s</board>\n", encode_url(url_buf,top[i].board,sizeof(url_buf)));
			fprintf(fp, "<time>%d</time>\n", top[i].date);
			fprintf(fp, "<number>%d</number>\n", top[i].number);
			fprintf(fp, "<groupid>%d</groupid>\n", top[i].groupid);
			fprintf(fp, "</hotsubject>\n");
        }
		fprintf(fp, "</hotsubjects>\n");

        fclose(fp);
    }
}

void poststat(int mytype)
{
	get_top(mytype);
	writestat(mytype);
	gen_hot_subjects_xml(mytype);
	if(mytype==0)
		gen_secs_hot_subjects_xml(mytype);

	if(mytype==0)
		log_top();
}


int main(int argc, char **argv)
{
    time_t now;
    struct tm ptime;
    int i;
    char buf[80], curfile[80] ;

	if (init_all()) {
		printf("init data fail\n");
		return -1;
	}
    time(&now);
    ptime = *localtime(&now);
    if (argc == 2) {
        i = atoi(argv[1]);
        if (i != 0 || argv[1][0]=='0') {
            poststat(i);
            return 0;
        }
    }

    if (ptime.tm_hour == 0) {
		if (ptime.tm_yday == 1)
			poststat(3);
        if (ptime.tm_mday == 1)
            poststat(2);
        if (ptime.tm_wday == 0)
            poststat(1);
    }

    poststat(0);
    poststat(4);

    if (ptime.tm_hour == 23) {
        char fname[STRLEN];

        sprintf(fname, "%d��%2d��%2d��ʮ�����Ż���", ptime.tm_year + 1900, ptime.tm_mon + 1, ptime.tm_mday);
        post_file(NULL, "", "etc/posts/day", "BBSLists", fname, 0, 1, getSession());
        if (ptime.tm_wday == 6) {
            sprintf(fname, "%d��%2d��%2d�ձ�����ʮ�����Ż���", ptime.tm_year + 1900, ptime.tm_mon + 1, ptime.tm_mday);
            post_file(NULL, "", "etc/posts/week", "BBSLists", fname, 0, 1, getSession());
        }
    }
    return 0;
}

#else
int main(int argc, char** argv)
{
    return 0;
}
#endif