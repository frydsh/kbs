#include "bbslib.h"

int main() {
	char path[80];

	init_all();
	if(!loginok) http_fatal("�Ҵҹ��Ͳ��ܴ���ѶϢ, ���ȵ�¼");
	/*sethomefile(path, currentuser->userid, "msgfile.me");*/
	setmsgfile(path, currentuser->userid);
	unlink(path);
	sethomefile(path, currentuser->userid,"msgcount");
	unlink(path);
	printf("��ɾ������ѶϢ����");
}
