#include "bbslib.h"

int main()
{
	FILE *fp;
	char file[200], board[200];
	int rv;
	struct boardheader x;

	init_all();
	strsncpy(board, getparm("board"), 32);
	if(!loginok)
		http_fatal("��ʱ��δ��¼��������login");
	if (getboardnum(board, &x) == 0)
		http_fatal("��������������");
	load_favboard(0);
	if(!has_read_perm(currentuser, x.filename))
		http_fatal("��������������");
	rv = add_favboard(x.filename);
	switch (rv)
	{
	case -2:
		http_fatal("��Ԥ����������Ŀ�Ѵ����ޣ���������Ԥ��");
		break;
	case -1:
		http_fatal("���Ѿ�Ԥ�������������߸�������������");
		break;
	case -3:
		http_fatal("��������������");
		break;
	default:
		;/* do nothing*/
	}
	save_favboard();
	printf("<script>top.f2.location='bbsleft'</script>\n");
	printf("Ԥ�� %s �������ɹ�<br><a href=\"javascript:history.go(-1)\">"
			"���ٷ���</a>", x.filename);
	http_quit();
}
