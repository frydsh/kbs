#include <time.h>
#include <stdio.h>
#include "bbs.h"
#include "config.h"

FILE *fp;
FILE *fp1;

#define BONLINE_LOGDIR "/home/bbs/bonlinelog"

int do_userlist(struct user_info *uentp, char *arg, int t)
{
    int i;
    int fd, len;
    char user_info_str[256 /*STRLEN*2 */ ], pagec;
    int override;
	char modebuf[80],idlebuf[10];

    t++;
    if (!uentp->active || !uentp->pid) {
        printf(" %4d ��,�Ҹ���\n", t);
        return 0;
    }
    pagec = ' ';
    sprintf(user_info_str,
             /*---	modified by period	2000-10-21	�����û������Դ���1000��
                     " %3d%2s%s%-12.12s%s%s %-16.16s%s %-16.16s %c %c %s%-17.17s\033[m%5.5s\n",
             ---*/
            " %4d%2s%-12.12s %-16.16s %-16.16s %c %c %s%-12.12s\033[m%5.5s %d\n", t, uentp->invisible ? "��" : "��", uentp->userid, uentp->username, uentp->from, pagec, ' ', (uentp->invisible == true)
            ? "\033[34m" : "", modestring(modebuf,uentp->mode, uentp->destuid, 0,  /* 1->0 ����ʾ�������� modified by dong 1996.10.26 */
                                       (uentp->in_chat ? uentp->chatid : NULL)), idle_str(idlebuf,uentp), uentp->pid);
    fprintf(fp, "%s", user_info_str);
    return COUNT;
}
#if HAVE_WWW ==1
int show_wwwguest()
{
	int i;

    for (i = 0; i < MAX_WWW_GUEST; i++) {
        if (!(wwwguest_shm->use_map[i / 32] & (1 << (i % 32))) )
            continue;
	    fprintf(fp1, "%s\n", inet_ntoa(wwwguest_shm->guest_entry[i].fromip));
    }

}
#endif
main()
{
	char path[256];
	struct stat st;
	time_t now;
	struct tm t;

    if (init_all()) {
        printf("init data fail\n");
        return -1;
    }

	now = time(0);
	localtime_r( &now, &t);

	if( stat( BONLINE_LOGDIR, &st) < 0 ){
		if(mkdir(BONLINE_LOGDIR, 0755) < 0)
			exit(0);
	}
	sprintf(path, "%s/%d", BONLINE_LOGDIR, t.tm_year+1900);
	if( stat(path, &st) < 0){
		if(mkdir(path, 0755) < 0)
			exit(0);
	}
	sprintf(path, "%s/%d/%d", BONLINE_LOGDIR, t.tm_year+1900, t.tm_mon+1);
	if( stat(path, &st) < 0){
		if(mkdir(path, 0755) < 0)
			exit(0);
	}

	sprintf(path, "%s/%d/%d/%d_%d.ulog", BONLINE_LOGDIR, t.tm_year+1900, t.tm_mon+1, t.tm_mday, t.tm_hour);
	if((fp=fopen(path, "w"))==NULL){
		printf("cannot open log file\n");
		exit(0);
	}
	sprintf(path, "%s/%d/%d/%d_%d.wwwguest", BONLINE_LOGDIR, t.tm_year+1900, t.tm_mon+1, t.tm_mday, t.tm_hour);
	if((fp1=fopen(path, "w"))==NULL){
		fclose(fp);
		printf("cannot open log file1\n");
		exit(0);
	}

	fprintf(fp, "%d\n", get_utmp_number());
#if HAVE_WWW == 1
	fprintf(fp1, "%d\n", getwwwguestcount());
#endif
    //fprintf(fp," ���  �û�ID       �ǳ�             ��Դ                 ״̬     ����ʱ�� ���̺�\n");
    apply_ulist_addr((APPLY_UTMP_FUNC)do_userlist, NULL);
#if HAVE_WWW == 1
	show_wwwguest();
#endif
    fclose(fp);
    fclose(fp1);
}


