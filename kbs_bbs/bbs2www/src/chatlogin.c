#include "bbslib.h"

int main() {
	int pid, n, t;
	char buf[256], id[20], pw[20];
	struct userec *x = NULL;
	FILE *fp;
	init_all();
	strsncpy(id, getparm("id"), 13);
        strsncpy(pw, getparm("pw"), 13);
	if(loginok && strcasecmp(id, currentuser->userid)) {
		http_fatal("ϵͳ��⵽Ŀǰ��ļ�������Ѿ���¼��һ���ʺ� %s�������˳�.(%s)", 
			currentuser->userid, "ѡ������logout, ���߹ر��������������");
	}
	getuser(id,&x);
	if(x==0) http_fatal("�����ʹ�����ʺ�");
	if(strcasecmp(id, "guest")) {
		/*if(!checkpasswd(x->passwd, pw)) {*/
		if (!checkpasswd2(pw, getcurrusr()))
		{
			if(pw[0]!=0) sleep(2);
			sprintf(buf, "%s %s %s\n", wwwCTime(time(0))+4, id, fromhost);
			f_append("badlogin.www", buf);
			http_fatal("�������");
		}
		if(!user_perm(x, PERM_BASIC))
			http_fatal("���ʺ��ѱ�ͣ��, ��������, ���������ʺ���sysop��ѯ��.");
		if(file_has_word(".bansite", fromhost)) {
			http_fatal("�Բ���, ��վ����ӭ���� [%s] �ĵ�¼. <br>��������, ����SYSOP��ϵ.", fromhost);
		}
		t=x->lastlogin;
		x->lastlogin=time(0);
		save_user_data(x);
		if(abs(t-time(0))<5) http_fatal("���ε�¼�������!");
		x->numlogins++;
		strsncpy(x->lasthost, fromhost, 17);
		save_user_data(x);
		currentuser=x;
	}
	sprintf(buf, "%s %s %s\n", wwwCTime(time(0)), x->userid, fromhost);
	f_append("www.log", buf);
	sprintf(buf, "%s ENTER %-12s @%s [www]\n", wwwCTime(time(0))+4, x->userid, fromhost);
	f_append("usies", buf);
	n=0;
	if(!loginok && strcasecmp(id, "guest"))	wwwlogin(x);
	redirect("bbschat");
}

int wwwlogin(struct userec *user) {
	FILE *fp;
	char buf[80];
	int pid, n, tmp;
	struct user_info *u;
	fp=fopen(".UTMP.lock", "r");
	flock(fileno(fp), LOCK_EX);
	for(n=0; n<MAXACTIVE; n++) {
		if(shm_utmp->uinfo[n].active == 0) {
			u=&(shm_utmp->uinfo[n]);
			u_info=u;
			pid=fork();
 			if(pid<0) http_fatal("can't fork");
 			if(pid==0) {
				wwwagent();
				exit(0);
			}
			bzero(u, sizeof(struct user_info));
			u->active=1;
			u->uid=getusernum(user->userid)+1;
			u->pid=pid;
			u->mode=10001;
        		if(user_perm(currentuser, PERM_LOGINCLOAK) &&
			(currentuser->flags[0] & CLOAK_FLAG))
                		u->invisible = YEA;
        		u->pager = 0;
        		if(currentuser->userdefine & DEF_FRIENDCALL)
				u->pager|=FRIEND_PAGER;
        		if(currentuser->flags[0] & PAGER_FLAG) {
                		u->pager|=ALL_PAGER;
                		u->pager|=FRIEND_PAGER;
			}
        		if(currentuser->userdefine & DEF_FRIENDMSG)
				u->pager|=FRIENDMSG_PAGER;
        		if(currentuser->userdefine & DEF_ALLMSG) {
                		u->pager|=ALLMSG_PAGER;
                		u->pager|=FRIENDMSG_PAGER;
        		}
			strsncpy(u->from, fromhost, IPLEN);
			*(int*)(u->from+32)=time(0);
			set_idle_time(u, time(0));
			strsncpy(u->username, user->username, 20);
			strsncpy(u->userid, user->userid, 13);
			tmp=rand()%100000000;
			u->utmpkey=tmp;
			sprintf(buf, "%d", n);
			setcookie("utmpnum", buf);
			sprintf(buf, "%d", tmp);
			setcookie("utmpkey", buf);
			setcookie("utmpuserid", currentuser->userid);
			set_my_cookie();
			flock(fileno(fp), LOCK_UN);
			fclose(fp);
			return 0;
		}
	}
	flock(fileno(fp), LOCK_UN);
	fclose(fp);
	http_fatal("��Ǹ��Ŀǰ�����û����Ѵ����ޣ��޷���¼�����Ժ�������");
}

void add_msg() {
        int i;
        FILE *fp;
        char buf[129], file[256], *id=currentuser->userid;
        sprintf(file, "touch home/%c/%s/wwwmsg.flush", toupper(id[0]), id);
        system(file);
        /*sprintf(file, "home/%c/%s/msgfile", toupper(id[0]), id);*/
		setmsgfile(file, id);
        i=file_size(file)/129;
        if(get_record(file, buf, 129, i-1)<=0) return;
        sprintf(file, "home/%c/%s/wwwmsg", toupper(id[0]), id);
        fp=fopen(file, "a");
        fwrite(buf, 129, 1, fp);
        fclose(fp);
}

void abort_program() {
	int stay;
	struct userec *x = NULL;
	f_append("err", "ok");
	if(strcmp(u_info->userid, currentuser->userid))
		bzero(u_info, sizeof(struct user_info));
	stay=abs(time(0) - *(int*)(u_info->from+32));
        if(stay>7200) stay=7200;
	getuser(getcurruserid(), &x);
 	if(x) {
		x->stay+=stay;
		save_user_data(x);
	}
        exit(0);
}

int wwwagent() {
	int i;
	for(i=0; i<1024; i++) close(i);
	for(i=0; i<NSIG; i++) signal(i, SIG_IGN);
	signal(SIGUSR2, add_msg);
	signal(SIGHUP, abort_program);
	while(1) {
		sleep(60);
		if(abs(time(0) - get_idle_time(u_info))>600) {
			f_append("err", "idle timeout");
			abort_program();
		}
	}
	exit(0);
}
