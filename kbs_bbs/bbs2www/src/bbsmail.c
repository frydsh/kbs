#include "bbslib.h"

int main() {
	FILE *fp;
	int filetime, i, start, total, type;
	char *ptr, buf[512], path[80], dir[80];
	struct fileheader *data;
   	init_all();
	if(!loginok) http_fatal("����δ��¼, ���ȵ�¼");
	strsncpy(buf, getparm("start"), 10);
	start=atoi(buf);
	if(buf[0]==0) start=999999;
   	printf("<center>\n");
   	printf("%s -- �ż��б� [ʹ����: %s]<hr color=green>\n", BBSNAME, currentuser->userid);
   	sprintf(dir, "mail/%c/%s/.DIR", toupper(currentuser->userid[0]), currentuser->userid);
   	total=file_size(dir)/sizeof(struct fileheader);
	if(total<0 || total>30000) http_fatal("too many mails");
   	data=(struct fileheader *)calloc(total, sizeof(struct fileheader));
   	if(data==0) http_fatal("memory overflow");
	fp=fopen(dir, "r");
	if(fp==0) http_fatal("dir error");
	total=fread(data, sizeof(struct fileheader), total, fp);
	fclose(fp);
	if(start>total-19) start=total-19;
	if(start<0) start=0;
      	printf("<table width=610>\n");
      	printf("<tr><td>���<td>״̬<td>������<td>����<td>�ż�����\n");
      	for(i=start; i<start+19 && i<total; i++) {
	 	int type='N';
	 	printf("<tr><td>%d", i+1);
         	if(data[i].accessed[0] & FILE_READ) type=' ';
         	if(data[i].accessed[0] & FILE_MARKED) type= (type=='N') ? 'M' : 'm';
		printf("<td>%c", type);
 		ptr=strtok(data[i].owner, " (");
		if(ptr==0) ptr=" ";
		ptr=nohtml(ptr);
		printf("<td><a href=bbsqry?userid=%s>%13.13s</a>", ptr, ptr);
	 	filetime=atoi(data[i].filename+2);
         	printf("<td>%12.12s", wwwCTime(filetime)+4);
         	printf("<td><a href=bbsmailcon?file=%s&num=%d>", data[i].filename, i);
	 	if(strncmp("Re: ", data[i].title, 4)) printf("�� ");
         	hprintf("%42.42s", void1(data[i].title));
	 	printf("</a>\n");
      	}
      	free(data);
      	printf("</table><hr>\n");
	printf("[�ż�����: %d]", total);
	printf("[<a href=bbspstmail>�����ż�</a>]");
	if(start>0) {
		i=start-19;
		if(i<0) i=0;
		printf("[<a href=bbsmail?start=0>��һҳ</a>] ");
		printf("[<a href=bbsmail?start=%d>��һҳ</a>] ", i);
	}
	if(start<total-19) {
		i=start+19;
		if(i>total-1) i=total-1;
		printf("[<a href=bbsmail?start=%d>��һҳ</a>] ", i);
		printf("[<a href=bbsmail>���һҳ</a>]");
	}
        printf("<form><input type=submit value=��ת��> �� <input style='height:20px' type=text name=start size=3> ��</form>");
	http_quit();
}
