#include "bbslib.h"

int is_bad_id(char *s)
{
	FILE *fp;
	char buf[256], buf2[256];
	fp=fopen(".badname", "r");
	if(fp==0) return 0;
	while(1) {
		if(fgets(buf, 250, fp)==0) break;
		if(sscanf(buf, "%s", buf2)!=1) continue;
		if(strcasestr(s, buf2)) {
			fclose(fp);
			return 1;
		}
	}
	fclose(fp);
	return 0;
}

int badymd(int y, int m, int d)
{
	int max[]={0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
	if((y%4==0 && y%100!=0) || y%400==0) max[2]=29;
	if(y<10 || y>100 || m<1 || m>12) return 1;
	if(d<0 || d>max[m]) return 1;
	return 0;
}

int main()
{
   	FILE *fp;
	struct userec x;
	int i, gender, xz;
	char buf[256], filename[80], pass1[80], pass2[80], dept[80], phone[80], salt[80], words[1024];
	int allocid;

   	init_all();
 	bzero(&x, sizeof(x));
	xz = atoi(getparm("xz"));
  	strsncpy(x.userid, getparm("userid"), 13);
   	strsncpy(pass1, getparm("pass1"), 13);
   	strsncpy(pass2, getparm("pass2"), 13);
   	strsncpy(x.username, getparm("username"), 32);
   	strsncpy(x.realname, getparm("realname"), 32);
   	strsncpy(dept, getparm("dept"), 32);
   	strsncpy(x.address, getparm("address"), 32);
   	strsncpy(x.email, getparm("email"), 32);
   	strsncpy(phone, getparm("phone"), 32);
	strsncpy(words, getparm("words"), 1000);
	if (id_invalid(x.userid))
		http_fatal("�ʺű�����Ӣ����ĸ��������ɣ����ҵ�һ���ַ�������Ӣ����ĸ!");
   	if(strlen(x.userid)<2)
	   	http_fatal("�ʺų���̫��(2-12�ַ�)");
   	if(strlen(pass1) < 4 || !strcmp( pass1, x.userid ) )
	   	http_fatal("����̫�̻���ʹ���ߴ�����ͬ, ����������");
   	if(strcmp(pass1, pass2))
	   	http_fatal("������������벻һ��, ��ȷ������");
	setpasswd(pass1, &x);
   	if(strlen(x.username)<2)
	   	http_fatal("�������ǳ�(�ǳƳ�������2���ַ�)");
   	if(strlen(x.realname)<4)
	   	http_fatal("��������ʵ����(��������, ����2����)");
   	if(strlen(dept)<6)
	   	http_fatal("������λ�����Ƴ�������Ҫ6���ַ�(��3������)");
   	if(strlen(x.address)<6)
	   	http_fatal("ͨѶ��ַ��������Ҫ6���ַ�(��3������)");
   	if(badstr(x.passwd)||badstr(x.username)||badstr(x.realname))
	   	http_fatal("����ע�ᵥ�к��зǷ��ַ�");
	if(badstr(dept)||badstr(x.address)||badstr(x.email)||badstr(phone))
	   	http_fatal("����ע�ᵥ�к��зǷ��ַ�");
 	if(bad_user_id(x.userid))
	   	http_fatal("�����ʺŻ��ֹע���id, ������ѡ��");
   	if(searchuser(x.userid))
	   	http_fatal("���ʺ��Ѿ�����ʹ��,������ѡ��");
   	strcpy(x.lasthost, fromhost);
   	x.userlevel = PERM_BASIC;
   	x.firstlogin = x.lastlogin = time(0);
   	x.userdefine = -1;
	x.userdefine &= ~DEF_NOTMSGFRIEND;
	x.notemode = -1;
	x.unuse1 = -1;
	x.unuse2 = -1;
   	x.flags[0] = CURSOR_FLAG | PAGER_FLAG;
	x.flags[1] = 0;
	allocid = getnewuserid2(x.userid);
    if(allocid > MAXUSERS || allocid <= 0)
		http_fatal("No space for new users on the system!") ;
	update_user(&x, allocid, 1);
	if( !searchuser(x.userid) )
		http_fatal("�����û�ʧ��");
	report( "new account" );
   	sprintf(filename, "home/%c/%s", toupper(x.userid[0]), x.userid);
	sprintf(buf, "/bin/mv -f %s "BBSHOME"/homeback/%s", filename, x.userid);
	system(buf);
	sprintf(filename, "mail/%c/%s", toupper(x.userid[0]), x.userid);
	sprintf(buf, "/bin/mv -f %s "BBSHOME"/mailback/%s", filename, x.userid);
	system(buf);

   	sprintf(filename, "home/%c/%s", toupper(x.userid[0]), x.userid);
   	mkdir(filename, 0755);
#ifndef SMTH
   	printf("<center><table><td><td><pre>\n");
	printf("�װ�����ʹ���ߣ����ã�\n\n");
        printf("��ӭ���� ��վ, �������ʺ��Ѿ��ɹ����Ǽ��ˡ�\n");
        printf("��Ŀǰӵ�б�վ������Ȩ��, �����Ķ����¡������ķ�������˽��\n");
	printf("�ż����������˵���Ϣ�����������ҵȵȡ�����ͨ����վ�����ȷ\n");
	printf("������֮���������ø����Ȩ�ޡ�Ŀǰ����ע�ᵥ�Ѿ����ύ\n");
	printf("�ȴ����ġ�һ�����24Сʱ���ھͻ��д𸴣������ĵȴ���ͬʱ��\n");
	printf("��������վ�����䡣\n");
	printf("��������κ����ʣ�����ȥsysop(վ���Ĺ�����)�淢��������\n\n</pre></table>");
   	printf("<hr color=green><br>���Ļ�����������:<br>\n");
   	printf("<table border=1 width=400>");
   	printf("<tr><td>�ʺ�λ��: <td>%d\n", getusernum(x.userid));
   	printf("<tr><td>ʹ���ߴ���: <td>%s (%s)\n", x.userid, x.username);
   	printf("<tr><td>��  ��: <td>%s<br>\n", x.realname);
	printf("<tr><td>��  ��: <td>%s<br>\n", x.username);
   	printf("<tr><td>��վλ��: <td>%s<br>\n", x.lasthost);
   	printf("<tr><td>�����ʼ�: <td>%s<br></table><br>\n", x.email);
   	newcomer(&x, words);
#endif /* not SMTH */
	printf("<center>���� ID �ɹ���</center><br>");
   	printf("<center><input type=\"button\" onclick=\"window.close()\" value=\"�رձ�����\"></center>\n");
   	sprintf(buf, "%s %-12s %d\n", wwwCTime(time(0))+4, x.userid, getusernum(x.userid));
   	f_append("wwwreg.log", buf);
	http_quit();
}

int badstr(unsigned char *s) {
  	int i;
	for(i=0; s[i]; i++)
    		if(s[i]!=9 &&(s[i]<32 || s[i]==255)) return 1;
  	return 0;
}

#ifndef SMTH
int newcomer(struct userec *x, char *words) {
  	FILE *fp;
  	char filename[80];
	sprintf(filename, "tmp/%d.tmp", getpid());
	fp=fopen(filename, "w");
	fprintf(fp, "��Һ�, \n\n");
	fprintf(fp, "���� %s(%s), ���� %s\n", x->userid, x->username, fromhost);
	fprintf(fp, "��������˵ر���, ���Ҷ��ָ��.\n\n");
	fprintf(fp, "���ҽ���:\n\n");
	fprintf(fp, "%s", words);
	fclose(fp);
	unlink(filename);
}

int adduser(struct userec *x)
{
	int i;
	FILE *fp;
	fp=fopen(".PASSWDS", "r+");
	flock(fileno(fp), LOCK_EX);
	for(i=0; i<MAXUSERS; i++) {
		if(shm_ucache->userid[i][0]==0) {
			strncpy(shm_ucache->userid[i], x->userid, 13);
			save_user_data(x);
			break;
		}
	}
	flock(fileno(fp), LOCK_UN);
	fclose(fp);
	system("touch .PASSFLUSH");
}
#endif /* not SMTH */
