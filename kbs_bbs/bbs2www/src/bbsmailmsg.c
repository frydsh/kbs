#include "bbslib.h"

int main()
{
	char filename[80];

	init_all();
	if(!loginok) http_fatal("�Ҵҹ��Ͳ��ܴ���ѶϢ�����ȵ�¼");
	/*sprintf(filename, "home/%c/%s/msgfile", toupper(currentuser->userid[0]), currentuser->userid);*/
	setmsgfile(filename, currentuser->userid);
	post_mail(currentuser->userid, "����ѶϢ����", filename, currentuser->userid, currentuser->username, fromhost, -1);
	unlink(filename);
	sethomefile(filename, currentuser->userid,"msgcount");
	unlink(filename);
	printf("ѶϢ�����Ѿ��Ļ���������");
	printf("<a href='javascript:history.go(-2)'>����</a>");
	http_quit();
}
