#include "bbslib.h"

int main()
{
	int n, type;
	struct stat st;
	int num;
	char buf[STRLEN];

  	init_all();
	if(!loginok)
		http_fatal("����δ��¼");
	type = atoi(getparm("type"));
	printf("%s -- �û���������<hr color=\"green\">\n", BBSNAME);
	if(type!=0)
	{
		check_info();
		http_quit();
	}
    setmailfile(buf, currentuser->userid, DOT_DIR);
	if( stat( buf, &st ) >= 0 )
		num = st.st_size / (sizeof( struct fileheader ));
	else
		num = 0;
 	printf("<form action=\"bbsinfo?type=1\" method=\"post\">");
  	printf("�����ʺ�: %s<br>\n", currentuser->userid);
  	printf("�����ǳ�: <input type=\"text\" name=\"nick\" value=\"%s\" size=\"24\" maxlength=\"%d\"><br>\n",
		currentuser->username, NAMELEN-1);
  	printf("�������: %d ƪ<br>\n", currentuser->numposts);
  	printf("�ż�����: %d ��<br>\n", num);
  	printf("��վ����: %d ��<br>\n", currentuser->numlogins);
  	printf("��վʱ��: %d ����<br>\n", currentuser->stay/60);
  	printf("��ʵ����: <input type=\"text\" name=\"realname\" value=\"%s\" size=\"16\" maxlength=\"%d\"><br>\n",
	 	currentuser->realname, NAMELEN-1);
  	printf("��ס��ַ: <input type=\"text\" name=\"address\" value=\"%s\" size=\"40\" maxlength=\"%d\"><br>\n",
 		currentuser->address, STRLEN-1);
  	printf("�ʺŽ���: %s<br>", wwwCTime(currentuser->firstlogin));
  	printf("�������: %s<br>", wwwCTime(currentuser->lastlogin));
  	printf("��Դ��ַ: %s<br>", currentuser->lasthost);
  	printf("�����ʼ�: <input type=\"text\" name=\"email\" value=\"%s\" size=\"32\" maxlength=\"%d\"><br>\n", 
		currentuser->email, STRLEN-1);
  	printf("<input type=\"submit\" value=\"ȷ��\"> <input type=\"reset\" value=\"��ԭ\">\n");
  	printf("</form>");
  	printf("<hr>");
	http_quit();
}

int check_info()
{
  	int m, n;
  	char buf[256];

	/* ��������еı����˵�ANSI���Ʒ� */
	strsncpy(buf, getparm("nick"), NAMELEN);
	for(m=0; m<strlen(buf); m++)
	{
		if(buf[m]<32 && buf[m]>0 || buf[m]==-1)
			buf[m]=' ';
	}
	if(strlen(buf)>1)
		strcpy(currentuser->username, buf);
	else
		printf("����: �ǳ�̫��!<br>\n");
	strsncpy(buf, getparm("realname"), NAMELEN);
	if(strlen(buf)>1)
		strcpy(currentuser->realname, buf); 
	else
		printf("����: ��ʵ����̫��!<br>\n");
	strsncpy(buf, getparm("address"), STRLEN);
	if(strlen(buf)>8)
		strcpy(currentuser->address, buf);
	else
		printf("����: ��ס��ַ̫��!<br>\n");
	strsncpy(buf, getparm("email"), STRLEN);
   	if(strlen(buf)>8 && strchr(buf, '@'))
		strcpy(currentuser->email, buf);
	else
		printf("����: email��ַ���Ϸ�!<br>\n");
	printf("[%s] ���������޸ĳɹ�.", currentuser->userid);
}
