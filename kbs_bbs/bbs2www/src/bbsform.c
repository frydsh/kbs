#include "bbslib.h"

int main()
{
  	FILE *fp;
	int type;

	init_all();
	type = atoi(getparm("type"));
  	if(!loginok)
		http_fatal("����δ��¼, �����µ�¼��");
	printf("%s -- ��дע�ᵥ<hr color=\"green\">\n", BBSNAME);
	check_if_ok();
	if(type==1)
	{
		check_submit_form();
		http_quit();
	}
  	printf("����, %s, ע�ᵥͨ���󼴿ɻ��ע���û���Ȩ��, ������������������д<br><hr>\n", currentuser->userid);
  	printf("<form method=\"post\" action=\"bbsform?type=1\">\n");
  	printf("��ʵ����: <input name=\"realname\" type=\"text\" maxlength=\"8\" size=\"8\" value=\"%s\">(��������,��������ĺ�������ƴ��)<br>\n", 
		nohtml(currentuser->realname));
  	printf("����λ: <input name=\"career\" type=\"text\" maxlength=\"32\" size=\"32\">(ѧУϵ����λȫ��)<br>\n");
  	printf("Ŀǰסַ: <input name=\"address\" type=\"text\" maxlength=\"32\" size=\"32\" value=\"%s\">(����嵽���һ����ƺ���)<br>\n", 
		nohtml(currentuser->address));
  	printf("����绰: <input name=\"phone\" type=\"text\" maxlength=\"32\" size=\"32\">(����������ʱ��,���޿��ú�����Email��ַ����)<br>\n");
  	printf("��������: <input name=\"birth\" type=\"text\" maxlength=\"32\" size=\"32\">(��.��.��(��Ԫ))<br><hr><br>\n");
  	printf("<input type=\"submit\"> <input type=\"reset\">");
	http_quit();
}

int check_if_ok()
{
  	if(user_perm(currentuser, PERM_LOGINOK))
		http_fatal("�������ȷ���Ѿ��ɹ�, ��ӭ���뱾վ������.");
    if ((time(0)-currentuser->firstlogin) < REGISTER_WAIT_TIME)
	{
		hprintf("���״ε��뱾վδ��"REGISTER_WAIT_TIME_NAME"...");
		hprintf( "�����Ĵ���Ϥһ�£�����"REGISTER_WAIT_TIME_NAME"�Ժ�����дע�ᵥ��");
		exit(-1);
	}

  	if(has_fill_form())
		http_fatal("Ŀǰվ����δ��������ע�ᵥ�������ĵȴ�.");
}

int check_submit_form()
{
	FILE *fp;

  	fp=fopen("new_register", "a");
	if(fp==0)
		http_fatal("ע���ļ�������֪ͨSYSOP");
  	fprintf(fp, "usernum: %d, %s\n",
			getusernum(currentuser->userid), wwwCTime(time(0)));
  	fprintf(fp, "userid: %s\n", currentuser->userid);
	/* ����ĸ�����ϢӦ���˵��Ƿ��ַ�֮����д���ļ�.*/
  	fprintf(fp, "realname: %s\n", getparm("realname"));
	fprintf( fp, "career: %s\n", getparm("career") );
  	fprintf(fp, "addr: %s\n", getparm("address"));
  	fprintf(fp, "phone: %s\n", getparm("phone"));
	fprintf(fp, "birth: %s\n", getparm("birth"));
  	fprintf(fp, "----\n" );
  	fclose(fp);
  	printf("����ע�ᵥ�ѳɹ��ύ. վ�����������������, ��������������.");
}
