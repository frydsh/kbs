#include "bbslib.h"

int main()
{
   	FILE *fp;
   	int i;
	char userid[80], buf[512], path[512], file[512], board[512], title[80]="";
   	struct boardheader  brdhdr;
   	struct fileheader  DirInfo;

   	init_all();
	if(!loginok)
		http_fatal("�Ҵҹ��Ͳ��ܷ������£����ȵ�¼");
	strsncpy(board, getparm("board"), 20);
	strsncpy(file, getparm("file"), 20);
	strsncpy(title, getparm("title"), 50);
	if(title[0] && strncmp(title, "Re: ", 4))
		sprintf(title, "Re: %s", getparm("title"));
	strsncpy(userid, getparm("userid"), 40);
	if(file[0]!='M' && file[0])
		http_fatal("������ļ���");
	if(!haspostperm(currentuser, board))
		http_fatal("�������������������Ȩ�ڴ���������������");
   	printf("<center>\n");
	printf("%s -- �������� [ʹ����: %s]<hr color=\"green\">\n", BBSNAME, currentuser->userid);
   	printf("<form method=\"post\" action=\"bbssnd?board=%s\">\n<table border=\"1\">\n",board);
	printf("<tr><td>");
	printf("<font color=\"green\">����ע������: <br>\n");
	printf("����ʱӦ���ؿ������������Ƿ��ʺϹ������Ϸ������������ˮ��лл���ĺ�����<br></font></td></tr>\n");
	printf("<tr><td>\n");
   	printf("����: %s<br>\n", currentuser->userid);
   	printf("ʹ�ñ���: <input type=\"text\" name=\"title\" size=\"40\" maxlength=\"100\" value=\"%s\">\n", 
		void1(title));
	printf("������: [%s]<br>\n",board);
   	printf("ʹ��ǩ���� <select name=\"signature\">\n");
	if (currentuser->signature == 0)
		printf("<option value=\"0\" selected>��ʹ��ǩ����</option>\n");
	else
		printf("<option value=\"0\">��ʹ��ǩ����</option>\n");
	for (i = 1; i < 6; i++)
	{
		if (currentuser->signature == i)
   			printf("<option value=\"%d\" selected>�� %d ��</option>\n", i, i);
		else
   			printf("<option value=\"%d\">�� %d ��</option>\n", i, i);
	}
	printf("</select>\n");
   	printf(" [<a target=\"_balnk\" href=\"bbssig\">�鿴ǩ����</a>] \n");
	/* ����ת��֧��*/
	printf("<input type=\"checkbox\" name=\"outgo\" value=\"1\">ת��\n");
	/* ��������������֧��*/
	if (seek_in_file("etc/anonymous",board))
		printf("<input type=\"checkbox\" name=\"anony\" value=\"1\">ʹ������\n");
   	printf("<br>\n<textarea name=\"text\" rows=\"20\" cols=\"80\" wrap=\"physicle\">\n\n");
	if(file[0])
	{
		int lines=0;
		printf("�� �� %s �Ĵ������ᵽ: ��\n", userid);
		sprintf(path, "boards/%s/%s", board, file);
		fp=fopen(path, "r");
		if(fp)
		{
			for(i=0; i<3; i++)
			{
				if(fgets(buf, 500, fp)==0)
					break;
			}
			while(1)
			{
				if(fgets(buf, 500, fp)==0) break;
				if(!strncmp(buf, ": ��", 4)) continue;
				if(!strncmp(buf, ": : ", 4)) continue;
				if(!strncmp(buf, "--\n", 3)) break;
				if(buf[0]=='\n') continue;
				if(++lines>10)
				{
					printf(": ...................\n");
					break;
				}
				/* faint, why check </textarea> */
				if(!strcasestr(buf, "</textarea>"))
					printf(": %s", buf);
			}
			fclose(fp);
		}
	}
   	printf("</textarea></td></tr>\n");
   	printf("<tr><td class=\"post\" align=\"center\">\n");
	printf("<input type=\"submit\" value=\"����\"> \n");
   	printf("<input type=\"reset\" value=\"���\"></td></tr>\n");
	printf("</table></form>\n");
	http_quit();
}
