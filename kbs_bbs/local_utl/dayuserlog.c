/*******************************************

stiger: ���ӵ� BONLINE_LOGDIR/year/mon/day_useronline �ļ�
�� gnuplot �õ�
һ��1Сʱ����10�ΰɣ���ϵ����̫��
Ȼ��ÿ������ʱ����ר�ŵĳ������������dayonline.png
mkonlinepng.sh

*******************************************/

#include <time.h>
#include <stdio.h>
#include "bbs.h"
#include "config.h"

FILE *fp;
int totalonline=0;
int wwwguestonline=0;
int wwwnotguestonline=0;
int telnetonline=0;
int telnetschool=0;
int wwwnotguestschool=0;
int wwwguestschool=0;

static int is_school(char *ip)
{
	if(! strncmp(ip, "219.224", 7) )
		return 1;
	if( !strncmp(ip, "166.111", 7))
		return 1;
	return 0;
}

int do_userlist(struct user_info *uentp, char *arg, int t)
{

	if( uentp->mode == WWW ){
		/* www */
		wwwnotguestonline++;
		if( is_school(uentp->from) )
			wwwnotguestschool++;
	}else{
		/* telnet */
		telnetonline ++;
		if( is_school(uentp->from) )
			telnetschool++;
	}

    return COUNT;
}
#if HAVE_WWW == 1
int show_wwwguest()
{
	int i;

    for (i = 0; i < MAX_WWW_GUEST; i++) {
        if (!(wwwguest_shm->use_map[i / 32] & (1 << (i % 32))) )
            continue;
	    if( is_school( inet_ntoa(wwwguest_shm->guest_entry[i].fromip)) )
			wwwguestschool++;
    }

}
#endif 
main()
{
	char path[256];
	char path1[256];
	struct stat st;
	time_t now;
	struct tm t;

	now = time(0);
	localtime_r( &now, &t);

    chdir(BBSHOME);
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

	sprintf(path, "%s/%d/%d/%d_useronline", BONLINE_LOGDIR, t.tm_year+1900, t.tm_mon+1, t.tm_mday, t.tm_hour);

	if((fp=fopen(path, "a"))==NULL){
		printf("cannot open log file\n");
		exit(0);
	}

    resolve_utmp();
	get_publicshm();
#if HAVE_WWW == 1
	resolve_guest_table();

	wwwguestonline = getwwwguestcount();
	totalonline =  get_utmp_number() + wwwguestonline;
#else
    totalonline =  get_utmp_number() ;
    wwwguestonline = wwwnotguestonline = wwwguestschool = wwwnotguestschool = 0;
#endif 
    apply_ulist_addr((APPLY_UTMP_FUNC)do_userlist, NULL);
#if HAVE_WWW == 1
	show_wwwguest();
#endif

	/*��ʽ: ʱ�� totalonline wwwguestonline wwwnotguestonline telnetonline wwwguestschool wwwnotguestschool telnetschool */
	fprintf(fp, "%d.%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\n", t.tm_hour, t.tm_min/6, totalonline, wwwguestonline, wwwnotguestonline, telnetonline, wwwguestschool, wwwnotguestschool, telnetschool);
    fclose(fp);
}
