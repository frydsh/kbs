#include "bbs.h"
#define BBS_PAGESIZE    (20) /* 19->20, modified by dong, 1999.1.26 */
#define refreshtime     (30)
extern time_t   login_start_time;
extern char     BoardName[];

int   (*func_list_show)();
time_t update_time=0;
int showexplain=0,freshmode=0;
int mailmode,numf;
int friendmode=0;
int usercounter,real_user_names=0;
int range,page,readplan,num;

struct user_info *user_record[USHM_SIZE];
struct userec *user_data;
extern char MsgDesUid[14]; /* ��������msg��Ŀ��uid 1998.7.5 by dong */

int myfriend(int uid,char* fexp)
{
    extern int  nf;
    int i,found=NA;
    int cmp;
    /*char buf[IDLEN+3];*/

    if(nf<=0)
    {
        return NA;
    }
    for (i=0;i<nf;i++) {
    	if (topfriend[i].uid==uid) {
    		found=YEA;
    		break;
    	}
    }
    if((found)&&fexp)
        strcpy(fexp,topfriend[i].exp);
    return found;
}

print_title()
{

    docmdtitle((friendmode)?"[�������б�]":"[ʹ�����б�]",
               " ����[t] ����[m] ��ѶϢ[s] ��,������[o,d] ��˵����[��,r] �л�ģʽ [f] ���[h]");
    update_endline();
}

print_title2()
{

    docmdtitle((friendmode)?"[�������б�]":"[ʹ�����б�]",
               "          ����[m] ��,������[o,d] ��˵����[��,r] ѡ��[��,��] ���[h]");
    update_endline();
}

void
update_data()
{
    if(readplan==YEA)
        return;
    if(time(0)>=update_time+refreshtime-1)
    {
        freshmode=1;
        /*Take out by SmallPig*/
        /*������ִ��һЩ�ӳ�ʽʱ������Ҳ�����*/
        /*                (*func_list_show)();
                        update_endline();
                        move( 3+num-page,0 ); prints( ">");
                        refresh();*/
    }
    signal(SIGALRM, update_data);
    alarm(refreshtime);
    return;
}


int
print_user_info_title()
{
    char title_str[ 512 ];
    char *field_2 ;

    move(2,0);
    clrtoeol();
    field_2 = "ʹ�����ǳ�";
    if (real_user_names) field_2 = "��ʵ����  ";
    sprintf( title_str,
             /*---	modified by period	2000-10-21	�����û������Դ���1000��
                     "[44m%s%-12.12s %-16.16s %-16.16s %c %c %-16.16s %5s[m\n",
             ---*/
             "[44m %s%-12.12s %-16.16s %-16.16s %c %c %-15.15s %5s[m\n",
             "���  ","ʹ���ߴ���", (showexplain==1)?"����˵�������":field_2, "����", 'P',
             /*(HAS_PERM(PERM_SYSOP) ? 'C' : ' ')*/'M', "��̬",
#ifdef SHOW_IDLE_TIME
             "ʱ:��" );
#else
"" );
#endif
    prints( "%s", title_str );
    return 0;
}

show_message(msg)
char msg[];
{

    move(BBS_PAGESIZE+3,0);
    clrtoeol();
    if(msg!=NULL)
        prints("[1m%s[m",msg);
    refresh();
}

void swap_user_record(a,b)
int a,b;
{
    struct user_info *c;

    c=user_record[a];
    user_record[a]=user_record[b];
    user_record[b]=c;
}

void
sort_user_record(left,right)
{
    int i,last;

    if(left>=right)
        return;
    swap_user_record(left,(left+right)/2);
    last=left;
    for(i=left+1;i<=right;i++)
        if(strcasecmp(user_record[i]->userid,user_record[left]->userid)<0)
            swap_user_record(++last,i);
    swap_user_record(left,last);
    sort_user_record(left,last-1);
    sort_user_record(last+1,right);
}

int full_utmp(struct user_info* uentp,int* count)
{
    if( !uentp->active || !uentp->pid )
    {
        return 0;
    }
    if(!HAS_PERM(PERM_SEECLOAK) && uentp->invisible && strcmp(uentp->userid,currentuser->userid))/*Haohmaru.99.4.24.���������ܿ����Լ�*/
    {
        return 0;
    }
    if(friendmode&&!myfriend(uentp->uid,NULL))
    {
        return 0;
    }
    user_record[*count]=uentp;
    (*count)++;
    return COUNT;
}

int
fill_userlist()
{
    static int i,i2;
    /*    struct      user_info *not_good; */

    i2=0;
    if(!friendmode)
    {
        /*�����ѷ���һ��

        int n;
        numf=0;
		for(n=0;n<i2;n++)
        {
            if(myfriend(searchuser(user_record[n]->userid,NULL)))
            {
                swap_user_record(numf++,n);
            }
        }
        sort_user_record(0,numf-1);
        sort_user_record(numf,i2-1);*/
	    apply_ulist_addr((APPLY_UTMP_FUNC)full_utmp,(char*)&i2);
    }else {
    	for (i=0;i<nf;i++) {
/*        sort_user_record(0,i2-1);*/
		if (topfriend[i].uid)
			apply_utmpuid((APPLY_UTMP_FUNC)full_utmp,topfriend[i].uid,(char*)&i2);
    	}
    }
    range=i2;
    return i2==0?-1:1;
}

int
do_userlist()
{
    int i;
    int fd,len;
    char  user_info_str[256/*STRLEN*2*/],pagec;
    int   override;
    char fexp[30];
    struct user_info *uentp;
    /*  _SHOW_ONLINE_USER */
    /* to print on line user to a file */
    /* char online_users[STRLEN+10];

     if(!strcmp(currentuser->userid,"guest")){
     fd=open("/home/system/bbs/onlineulist",O_RDWR|O_TRUNC, 0600);
     if(fd!=-1)
     {
    flock(fd,LOCK_EX);
     	for(i=0; i<range ; i++)
    	 {
        uentp=user_record[i];
        len = sprintf(online_users, " %3d %-12.12s %-24.24s %-20.20s %-17.17s %5.5s\n", i+1,uentp->userid,uentp->username,uentp->from,modestring(uentp->mode, uentp->destuid, 0, (uentp->in_chat ? uentp->chatid : NULL)),uentp->invisible? "#":" ");
         write(fd,online_users,len);
     }
         flock(fd,LOCK_UN);
       close(fd);
     }
}
    */
    /* end of this insertion */

    /*  end of this insertion */
    move(3,0);
    print_user_info_title();

    for(i=0;i<BBS_PAGESIZE&&i+page<range;i++)
    {
        uentp=user_record[i+page];
        if (!uentp->active||!uentp->pid)
		{
			prints(" %4d ��,�Ҹ���\n",i+1+page);
			continue;
        }	
        if(!showexplain)
            override=(i+page<numf)||friendmode;
        else
        {
            if((i+page<numf)||friendmode)
                override=myfriend(uentp->uid,fexp);
            else
                override=NA;
        }
        if(readplan==YEA)
        {
            return 0;
        }
        pagec=pagerchar( can_override(uentp->userid,currentuser->userid), uentp->pager);
        sprintf( user_info_str,
                 /*---	modified by period	2000-10-21	�����û������Դ���1000��
                         " %3d%2s%s%-12.12s%s%s %-16.16s%s %-16.16s %c %c %s%-17.17s[m%5.5s\n",
                 ---*/
                 " %4d%2s%s%-12.12s%s%s %-16.16s%s %-16.16s %c %c %s%-16.16s[m%5.5s\n",
                 i+1+page,(override)?(uentp->invisible?"��":"��"):(uentp->invisible?"��":""),
                         (override)? "[1;32m":"",uentp->userid
                         ,(override)? "[m":"",(override&&showexplain)?"[1;31m":"",
                         (real_user_names) ? uentp->realname:
                         (showexplain&&override)? fexp:uentp->username,(override&&showexplain)?"[m":"",
                         ((/* !DEFINE(DEF_HIDEIP) &&*/ (pagec==' ' || pagec=='O') ) || HAS_PERM(PERM_SYSOP)) ? uentp->from : "*",/*Haohmaru.99.12.18*/
                         pagec,
                         /*(uentp->invisible ? '#' : ' ')*/msgchar(uentp),(uentp->invisible==YEA)
                         ?"[34m":"",
                         modestring(uentp->mode, uentp->destuid, 0,/* 1->0 ����ʾ�������� modified by dong 1996.10.26 */
                                    (uentp->in_chat ? uentp->chatid : NULL)),
#ifdef SHOW_IDLE_TIME
                         idle_str( uentp ) );
#else
                         "" );
#endif
        clrtoeol();
        prints( "%s", user_info_str );
    }
    return 0 ;
}

int
show_userlist()
{
    char genbuf[5];


    /*    num_alcounter();*/
    /*    if(!friendmode)
                range=count_users;
        else
                range=count_friends;*/
    if(update_time+refreshtime<time(0))
    {
        fill_userlist();
        update_time=time(0);
    }
    if( range==0/*||fill_userlist() == 0 */) {
        move(2,0);
        prints( "û��ʹ���ߣ����ѣ����б���...\n" );
        clrtobot();
        if(friendmode){
            getdata(BBS_PAGESIZE+3,0,"�Ƿ�ת����ʹ����ģʽ (Y/N)[Y]: ",genbuf,4,DOECHO,NULL,YEA);
            move(BBS_PAGESIZE+3,0);
            clrtobot();
            if(genbuf[0] != 'N' && genbuf[0] != 'n')
            {
                range=num_visible_users();
                page=-1;
                friendmode=NA;
                return 1;
            }
        }else
            pressanykey();
        return -1;
    }
    do_userlist();
    clrtobot();
    return 1;
}

void
t_rusers()
{
    real_user_names = 1;
    t_users();
    real_user_names = 0;
}

int
deal_key(ch,allnum,pagenum)
char ch;
int allnum,pagenum;
{
    char    buf[STRLEN],genbuf[5];
    static  int   msgflag;

    if(msgflag==YEA)
    {
        show_message(NULL);
        msgflag=NA;
    }
    switch(ch)
    {
case 'k': case'K':
        if(!HAS_PERM(PERM_SYSOP)&&strcmp(currentuser->userid,
                                         user_record[allnum]->userid))
            return 1;
        if (!strcmp(currentuser->userid, "guest"))
            return 1; /* Leeward 98.04.13 */
        sprintf(buf,"��Ҫ�� %s �߳�վ���� (Yes/No) [N]: ",
                user_record[allnum]->userid);
        move(BBS_PAGESIZE+3,0);
        clrtoeol();
        getdata(BBS_PAGESIZE+3,0,buf,genbuf,4,DOECHO,NULL,YEA);
        if(genbuf[0] != 'Y' && genbuf[0] != 'y')
        {
            return 1;
        }
        if(kick_user(user_record[allnum])==1)
        {
            sprintf(buf,"%s �ѱ��߳�վ��",
                    user_record[allnum]->userid);
        }
        else
        {
            sprintf(buf,"%s �޷��߳�վ��",
                    user_record[allnum]->userid);
        }
        msgflag=YEA;
        break;
case 'h':case 'H':
        show_help( "help/userlisthelp" );
        break;
case 'W':case 'w':
        if(showexplain==1)
            showexplain=0;
        else
            showexplain=1;
        break;
case 't': case'T':
        if(!HAS_PERM(PERM_PAGE))
            return 1;
        if(strcmp(currentuser->userid,
                  user_record[allnum]->userid))
            ttt_talk(user_record[allnum]);
        else
            return 1;
        break;
case 'm': case'M':
        if(!HAS_PERM(PERM_POST))
            return 1;
        m_send(user_record[allnum]->userid);
        break;
case 'f': case 'F':
        if(friendmode)
            friendmode=NA;
        else
            friendmode=YEA;
        update_time=0;
        break;
case 's': case 'S':
        if( strcmp(user_record[allnum]->userid,"guest") && 
		!HAS_PERM(PERM_PAGE))
            return 1;
        if(!canmsg(user_record[allnum]))
        {
            sprintf(buf,"%s �Ѿ��ر�ѶϢ������",
                    user_record[allnum]->userid);
            msgflag=YEA;
            break;
        }
        /* ��������msg��Ŀ��uid 1998.7.5 by dong*/
        strcpy(MsgDesUid, user_record[allnum]->userid);
        do_sendmsg(user_record[allnum],NULL,0);
        break;
case 'o': case 'O':
        if(!strcmp("guest",currentuser->userid))
            return 0;
        if(addtooverride(user_record[allnum]->userid)
                ==-1)
        {
            sprintf(buf,"%s ������������",
                    user_record[allnum]->userid);
        }
        else
        {
            sprintf(buf,"%s ������������",
                    user_record[allnum]->userid);
        }
        msgflag=YEA;
        break;
case 'd': case'D':
        if(!strcmp("guest",currentuser->userid))
            return 0;
        /* Leeward: 97.12.19: confirm removing operation */
        sprintf(buf,"��Ҫ�� %s �����������Ƴ��� (Y/N) [N]: ",
                user_record[allnum]->userid);
        move(BBS_PAGESIZE+3,0);
        clrtoeol();
        getdata(BBS_PAGESIZE+3,0,buf,genbuf,4,DOECHO,NULL,YEA);
        move(BBS_PAGESIZE+3,0);
        clrtoeol();
        if(genbuf[0] != 'Y' && genbuf[0] != 'y')
            return 0;
        if(deleteoverride(user_record[allnum]->userid)
                ==-1)
        {
            sprintf(buf,"%s �����Ͳ�������������",
                    user_record[allnum]->userid);
        }
        else
        {
            sprintf(buf,"%s �Ѵ����������Ƴ�",
                    user_record[allnum]->userid);
        }
        msgflag=YEA;
        break;
    default:
        return 0;
    }
    if(friendmode)
        modify_user_mode(FRIEND);
    else
        modify_user_mode(LUSERS);
    if(readplan==NA)
    {
        print_title();
        clrtobot();
        if(show_userlist()==-1)
            return -1;
        if(msgflag){
            show_message(buf);
        }
        update_endline();
    }
    return 1;
}

int
deal_key2(ch,allnum,pagenum)
char ch;
int allnum,pagenum;
{
    char    buf[STRLEN];
    static  int   msgflag;

    if(msgflag==YEA)
    {
        show_message(NULL);
        msgflag=NA;
    }
    switch(ch)
    {
case 'h':case 'H':
        show_help( "help/usershelp" );
        break;
case 'm': case'M':
        if(!HAS_PERM(PERM_POST))
            return 1;
        m_send(user_data[allnum-pagenum].userid);
        break;
case 'o': case 'O':
        if(!strcmp("guest",currentuser->userid))
            return 0;
        if(addtooverride(user_data[allnum-pagenum].userid)
                ==-1)
        {
            sprintf(buf,"%s ������������",
                    user_data[allnum-pagenum].userid);
            show_message(buf);
        }
        else
        {
            sprintf(buf,"%s ������������",
                    user_data[allnum-pagenum].userid);
            show_message(buf);
        }
        msgflag=YEA;
        if(!friendmode)
            return 1;
        break;
case 'W':case 'w':
        if(showexplain==1)
            showexplain=0;
        else
            showexplain=1;
        break;
case 'd': case'D':
        if(!strcmp("guest",currentuser->userid))
            return 0;
        /* Leeward: 97.12.19: confirm removing operation */
        sprintf(buf,"��Ҫ�� %s �����������Ƴ��� (Y/N) [N]: ",
                user_data[allnum-pagenum].userid);
        move(BBS_PAGESIZE+3,0);
        clrtoeol();
        getdata(BBS_PAGESIZE+3,0,buf,genbuf,4,DOECHO,NULL,YEA);
        move(BBS_PAGESIZE+3,0);
        clrtoeol();
        if(genbuf[0] != 'Y' && genbuf[0] != 'y')
            return 0;
        if(deleteoverride(user_data[allnum-pagenum].userid)
                ==-1)
        {
            sprintf(buf,"%s �����Ͳ�������������",
                    user_data[allnum-pagenum].userid);
            show_message(buf);
        }
        else
        {
            sprintf(buf,"%s �Ѵ����������Ƴ�",
                    user_data[allnum-pagenum].userid);
            show_message(buf);
        }
        msgflag=YEA;
        if(!friendmode)
            return 1;
        break;
    default:
        return 0;
    }
    modify_user_mode(LAUSERS);
    if(readplan==NA)
    {
        print_title2();
        move(3,0);
        clrtobot();
        if(Show_Users()==-1)
            return -1;
    }
    return 1;
}

printuent(struct userec *uentp ,char* arg)
{
    static int i ;
    char permstr[10];
    int override;
    char fexp[30];
    char buf[20] = "           ";


    if(uentp == NULL) {
        move(3,0);
        printutitle();
        i = 0 ;
        return 0;
    }
    if( uentp->numlogins == 0 ||
            uleveltochar( permstr, uentp->userlevel ) == 0 )
        return 0;
    if(i<page||i>=page+BBS_PAGESIZE||i>=range)
    {
        i++;
        if(i>=page+BBS_PAGESIZE||i>=range)
            return QUIT;
        else
            return 0;
    }
    uleveltochar(&permstr,uentp->userlevel);
    user_data[i-page]=*uentp;
    override=myfriend(searchuser(uentp->userid),fexp);
    /*---	modified by period	2000-11-02	hide posts/logins	---*/
#ifdef _DETAIL_UINFO_
    prints(" %5d%2s%s%-14s%s %s%-19s%s  %5d %5d %4s   %-16s\n",i+1,
#else
    if(HAS_PERM(PERM_ADMINMENU))
        sprintf(buf, "%5d %5d", uentp->numlogins, uentp->numposts);
    prints(" %5d%2s%s%-14s%s %s%-19s%s  %11s %4s   %-16s\n", i+1,
#endif
           (override)?"��":"",
           (override)?"[32m":"",uentp->userid,(override)?"[m":"",
           (override&&showexplain)?"[31m":"",
#if defined(ACTS_REALNAMES)
           uentp->realname,
#else
           (override&&showexplain)?fexp:uentp->username,
#endif
           (override&&showexplain)?"[m":"",
#ifdef _DETAIL_UINFO_
           uentp->numlogins, uentp->numposts,
#else
           buf,
#endif
           permstr,
           Ctime(&uentp->lastlogin) );
    i++ ;
    usercounter++;
    return 0 ;
}

int
countusers(struct userec *uentp ,char* arg)
{
    char permstr[10];

    if(uentp->numlogins != 0&&uleveltochar( permstr, uentp->userlevel ) != 0)
		return COUNT;
    return 0;
}

int
allusers()
{
	int count;
    if((count=apply_users(countusers,0)) <= 0) {
        return 0;
    }
    return count;
}

int
mailto(struct userec *uentp ,char* arg)
{
    char filename[STRLEN];

    sprintf(filename,"etc/%s.mailtoall",currentuser->userid);
    if((uentp->userlevel==PERM_BASIC&&mailmode==1)||
            (uentp->userlevel&PERM_POST&&mailmode==2)||
            (uentp->userlevel&PERM_BOARDS&&mailmode==3)||
            (uentp->userlevel&PERM_CHATCLOAK&&mailmode==4))
    {
        mail_file(filename,uentp->userid,save_title);
    }
    return 1;
}

int mailtoall(mode)
int mode;
{

    mailmode=mode;
    return apply_users(mailto,0);
}

Show_Users()
{

    usercounter = 0;
    modify_user_mode(LAUSERS );
    printuent((struct userec *)NULL,0) ;

    apply_users(printuent,0);
    clrtobot();
    return 0;
}

setlistrange(i)
int i;
{range=i;}


do_query(star,curr)
int star,curr;
{

    clear();
    if( user_record[curr]->active && user_record[curr]->pid ){
    	t_query(user_record[curr]->userid);
    	move(t_lines-1,0);
    	prints( "[44m����[t] ����[m] ��ѶϢ[s] ��,������[o,d] ѡ��ʹ����[��,��] �л�ģʽ [f] ���[h][m");
    }else{
	prints( "\n\n    �治�ɣ���λ�����Ѿ���ʧ����Ⱥ���ˡ���" );
    }
}

do_query2(star,curr)
int star,curr;
{
    t_query(user_data[curr-star].userid);
    move(t_lines-1,0);
    prints( "[44m           ����[m] ��,������[o,d] ��˵����[��,r] ѡ��[��,��] ���[h]           [m");

}

Users()
{
    range=allusers();
    modify_user_mode(LAUSERS );
    clear();
    user_data=(struct userec *)calloc(sizeof(struct userec),BBS_PAGESIZE);
    choose(NA,0,print_title2,deal_key2,Show_Users,do_query2);
    clear();
    free(user_data);
    return;
}

int
t_friends()
{
    FILE        *fp;
    char        genbuf[STRLEN];

    modify_user_mode(FRIEND);
    friendmode=YEA;
    setuserfile( genbuf, "friends" );
    if ((fp = fopen(genbuf, "r")) == NULL) {
        move( 1, 0 );
        clrtobot();
        prints("����δ���� Talk -> Override �趨��������������...\n");
        pressanykey();
        return 0;
    }
    fclose(fp);
    num_alcounter();
    range=count_friends;
    if( range == 0 ) {
        move( 2, 0 );
        clrtobot();
        prints( "Ŀǰ�޺�������\n");
        getdata(BBS_PAGESIZE+3,0,"�Ƿ�ת����ʹ����ģʽ (Y/N)[Y]: ",genbuf,4,DOECHO,NULL,YEA);
        move(BBS_PAGESIZE+3,0);
        clrtobot();
        if(genbuf[0] != 'N' && genbuf[0] != 'n')
        {
            range=num_visible_users();
            page=-1;
            friendmode=NA;
            update_time=0;
            choose(YEA,0,print_title,deal_key,show_userlist,do_query);
            clear();
            return;
        }
    }else
    {
        update_time=0;
        choose(YEA,0,print_title,deal_key,show_userlist,do_query);
    }
    clear();
    friendmode=NA;
    return;
}

int
t_users()
{
    friendmode=NA;
    modify_user_mode(LUSERS);
    range=num_visible_users();
    if( range == 0 ) {
        move( 3, 0 );
        clrtobot();
        prints( "Ŀǰ��ʹ��������\n");
    }
    update_time=0;
    choose(YEA,0,print_title,deal_key,show_userlist,do_query);
    clear();
    return 0;
}

int
choose(update,defaultn,title_show,key_deal,list_show,read)
int update;
int defaultn;
int (*title_show)();
int (*key_deal)();
int (*list_show)();
int (*read)();
{
    int         ch, number,deal;
    readplan=NA;
    (*title_show)();
    func_list_show=list_show;
    signal(SIGALRM, SIG_IGN);
    if(update==1)
        update_data();
    page=-1;
    number=0;
    num=defaultn;
    while( 1 ) {
        if( num <= 0 )  num = 0;
        if( num >= range )  num = range - 1;
        if( page < 0||freshmode==1) {
            freshmode=0;
            page = (num / BBS_PAGESIZE) * BBS_PAGESIZE;
            move(3,0);clrtobot();
            if((*list_show)()==-1)
                return -1;
            update_endline();
        }
        if( num < page || num >= page + BBS_PAGESIZE ) {
            page = (num / BBS_PAGESIZE) * BBS_PAGESIZE;
            if((*list_show)()==-1)
                return -1;
            update_endline();
            continue;
        }
        if(readplan==YEA)
        {
            if((*read)(page,num)==-1)
                return num;
        }
        else{
            move( 3+num-page,0 ); prints( ">", number );}
        ch = egetch();
        if(readplan==NA)
            move( 3+num-page,0 ); prints( " " );
        if( ch == 'q' || ch == 'e' || ch == KEY_LEFT || ch == EOF )
        {
            if(readplan==YEA)
            {
                readplan=NA;
                move(1,0);
                clrtobot();
                if((*list_show)()==-1)
                    return -1;
                (*title_show)();
                continue;
            }
            break;
        }

        deal=(*key_deal)(ch,num,page);
        if(range==0)
            break;
        if(deal==1)
            continue;
        else if(deal==-1)
            break;
        switch( ch ) {
        case Ctrl('Z'): r_lastmsg(); /* Leeward 98.07.30 support msgX */
            break;
case 'P': case 'b': case Ctrl('B'): case KEY_PGUP:
            if( num == 0 )  num = range - 1;
            else  num -= BBS_PAGESIZE;
            break;
        case ' ':
            if(readplan==YEA){
                if( ++num >= range )  num = 0;
                break;
            }
case 'N': case Ctrl('F'): case KEY_PGDN:
            if( num == range - 1 )  num = 0;
            else  num += BBS_PAGESIZE;
            break;
case 'p': case 'l': case KEY_UP:
            if( num-- <= 0 )  num = range - 1;
            break;
case 'n': case 'j': case KEY_DOWN:
            if( ++num >= range )  num = 0;
            break;
case '$':case KEY_END:
            num = range - 1;       break;
        case KEY_HOME:
            num = 0;       break;
    case '\n': case '\r':
            if( number > 0 ) {
                num = number - 1;
                break;
            }
            /* fall through */
    case 'r': case KEY_RIGHT:
            {
                if(readplan==YEA)
                {
                    if( ++num >= range )  num = 0;
                }
                else
                    readplan=YEA;
                break;
            }
        default:
            ;
        }
        if( ch >= '0' && ch <= '9' ) {
            number = number * 10 + (ch - '0');
            ch = '\0';
        } else {
            number = 0;
        }
    }
    signal(SIGALRM, SIG_IGN);
    return -1 ;
}
