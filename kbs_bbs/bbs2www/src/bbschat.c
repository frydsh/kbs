/*
 * $Id$
 */
#include "bbslib.h"
#include "netinet/in.h"
#include <netdb.h>
#include <netinet/in.h>
#include <sys/types.h>
#ifdef FREEBSD
#include <sys/socket.h>         /* need for FreeBSD */
#endif
#ifdef AIX
#include <sys/socket.h>
#include <sys/socketvar.h>
#endif

int chat_fd;
int test = 0;
char genbuf[1024];

struct action {
    char *verb;                 /* ���� */
    char *part1_msg;            /* ��� */
    char *part2_msg;            /* ���� */
};


struct action party_data[] = {
    {"?", "���ɻ�Ŀ���", ""},
    {"admire", "��", "�ľ���֮���������Ͻ�ˮ���಻��"},
    {"agree", "��ȫͬ��", "�Ŀ���"},
    {"bearhug", "�����ӵ��", ""},
    {"bless", "ף��", "�����³�"},
    {"bow", "�Ϲ��Ͼ�����", "�Ϲ�"},
    {"bye", "����", "�ı�Ӱ����Ȼ���¡����������������������ĸ���:\\n\"\033[31m���վ�����.....\033[m\""},
    {"caress", "����ĸ���", ""},
    {"cat", "��ֻСè���������", "�Ļ���������"},
    {"cringe", "��", "������ϥ��ҡβ����"},
    {"cry", "��", "�������"},
    {"comfort", "���԰�ο", ""},
    {"clap", "��", "���ҹ���"},
    {"dance", "����", "������������"},
    {"dogleg", "��", "����"},
    {"drivel", "����", "����ˮ"},
    {"dunno", "�ɴ��۾���������ʣ�", "����˵ʲ���Ҳ���Ү... :("},
    {"faint", "�ε���", "�Ļ���"},
    {"fear", "��", "¶�����µı���"},
    {"fool", "����ע��", "�����׳�....\\n������������....�˼����Ļ....\\n����̫��ϧ�ˣ�"},
    {"forgive", "��ȵĶ�", "˵�����ˣ�ԭ������"},
    {"giggle", "����", "ɵɵ�Ĵ�Ц"},
    {"grin", "��", "¶��а���Ц��"},
    {"growl", "��", "��������"},
    {"hand", "��", "����"},
    {"hammer", "����ô�ô�����������ۣ���",
     "ͷ������һ�ã�\\n***************\\n*  5000000 Pt *\\n***************\\n      | |      %1��%2��%3��%4��%5��%6��%0\\n      | |         �ö������Ӵ\\n      |_|"},
    {"heng", "��������", "һ�ۣ� ����һ�����߸ߵİ�ͷ��������,��мһ�˵�����..."},
    {"hug", "�����ӵ��", ""},
    {"idiot", "����س�Ц", "�ĳմ���"},
    {"kick", "��", "�ߵ���ȥ����"},
    {"kiss", "����", "������"},
    {"laugh", "������Ц", ""},
    {"lovelook", "����", "���֣������ĬĬ���ӡ�Ŀ�����к���ǧ�����飬�������"},
    {"nod", "��", "��ͷ����"},
    {"nudge", "�����ⶥ", "�ķʶ���"},
    {"oh", "��", "˵����Ŷ�����Ӱ�����"},
    {"pad", "����", "�ļ��"},
    {"papaya", "������", "��ľ���Դ�"},
    {"pat", "��������", "��ͷ"},
    {"pinch", "�����İ�", "š�ĺ���"},
    {"puke", "����", "�°��°�����˵�¶༸�ξ�ϰ����"},
    {"punch", "�ݺ�����", "һ��"},
    {"pure", "��", "¶�������Ц��"},
    {"qmarry", "��", "�¸ҵĹ�������:\\n\"��Ը��޸�����\"\\n---���������ɼΰ�"},
    {"report", "͵͵�ض�", "˵���������Һ��𣿡�"},
    {"shrug", "���ε���", "�����ʼ��"},
    {"sigh", "��", "̾��һ����"},
    {"slap", "žž�İ���", "һ�ٶ���"},
    {"smooch", "ӵ����", ""},
    {"snicker", "�ٺٺ�..�Ķ�", "��Ц"},
    {"sniff", "��", "��֮�Ա�"},
    {"sorry", "ʹ�����������", "ԭ��"},
    {"spank", "�ð��ƴ�", "���β�"},
    {"squeeze", "������ӵ����", ""},
    {"thank", "��", "��л"},
    {"tickle", "��ߴ!��ߴ!ɦ", "����"},
    {"waiting", "����ض�", "˵��ÿ��ÿ�µ�ÿһ�죬ÿ��ÿ���Ҷ������������"},
    {"wake", "Ŭ����ҡҡ", "��������ߴ�У��������ѣ��������ģ���"},
    {"wave", "����", "ƴ����ҡ��"},
    {"welcome", "���һ�ӭ", "�ĵ���"},
    {"wink", "��", "���ص�գգ�۾�"},
    {"xixi", "�����ض�", "Ц�˼���"},
    {"zap", "��", "���Ĺ���"},
    {"inn", "˫�۱�������ˮ���޹�������", ""},
    {"mm", "ɫ���еĶ�", "�ʺã�����ü�á�������������ɫ�ǰ�������"},
    {"disapp", "����û��ͷ����Ϊʲô", "����������������ȫû��Ӧ��û�취��"},
    {"miss", "��ϵ�����", "�����������������������̫--��������!���಻����?"},
    {"buypig", "ָ��", "���������ͷ������һ�룬лл����"},
    {"rascal", "��", "��У������������å����������������������������������"},
    {"qifu", "С��һ�⣬��", "�޵��������۸��ң����۸��ң�������"},
    {"wa", "��", "���һ�����������������ۿ����Ү������������������������������"},
    {"feibang", "ร���������죬", "�������ҽ�������һ�����Ը���ٰ�������"},
    {NULL, NULL, NULL}
};

struct action speak_data[] = {
    {"ask", "ѯ��", NULL},
    {"chant", "����", NULL},
    {"cheer", "�Ȳ�", NULL},
    {"chuckle", "��Ц", NULL},
    {"curse", "����", NULL},
    {"demand", "Ҫ��", NULL},
    {"frown", "��ü", NULL},
    {"groan", "����", NULL},
    {"grumble", "����ɧ", NULL},
    {"hum", "������", NULL},
    {"moan", "��̾", NULL},
    {"notice", "ע��", NULL},
    {"order", "����", NULL},
    {"ponder", "��˼", NULL},
    {"pout", "������˵", NULL},
    {"pray", "��", NULL},
    {"request", "����", NULL},
    {"shout", "���", NULL},
    {"sing", "����", NULL},
    {"smile", "΢Ц", NULL},
    {"smirk", "��Ц", NULL},
    {"swear", "����", NULL},
    {"tease", "��Ц", NULL},
    {"whimper", "���ʵ�˵", NULL},
    {"yawn", "��Ƿ����", NULL},
    {"yell", "��", NULL},
    {NULL, NULL, NULL}
};

struct action condition_data[] = {
    {":D", "�ֵĺϲ�£��", NULL},
    {":)", "�ֵĺϲ�£��", NULL},
    {":P", "�ֵĺϲ�£��", NULL},
    {":(", "�ֵĺϲ�£��", NULL},
    {"applaud", "žžžžžžž....", NULL},
    {"blush", "��������", NULL},
    {"cough", "���˼���", NULL},
    {"faint", "�۵�һ�����ε��ڵ�", NULL},
    {"happy", "������¶�����Ҹ��ı��飬��ѧ�Ա��˵���ߺ�������", NULL},
    {"lonely", "һ�������ڷ��������������ϣ��˭�����㡣������", NULL},
    {"luck", "�ۣ���������", NULL},
    {"puke", "����ģ������˶�����", NULL},
    {"shake", "ҡ��ҡͷ", NULL},
    {"sleep", "Zzzzzzzzzz�������ģ�����˯����", NULL},
    {"so", "�ͽ���!!", NULL},
    {"strut", "��ҡ��ڵ���", NULL},
    {"tongue", "��������ͷ", NULL},
    {"think", "����ͷ����һ��", NULL},
    {"wawl", "���춯�صĿ�", NULL},
    {NULL, NULL, NULL}
};

int main()
{
    char t[80];
    int pid;

    initwww_all();
    if (!loginok) {
        http_fatal("�������ȵ�¼");
        exit(0);
    }
    if (!can_enter_chatroom())
        http_fatal("�����ܽ���" CHAT_SERVER);
    strsncpy(t, getparm("t"), 80);
    pid = atoi(getparm("pid"));
    if (pid == 0)
        reg();
    if (pid > 0) {
        if (kill(pid, 0) != 0) {
            printf("<script>top.location='about:���Ѿ����߻�����1�����ڽ�����������.'</script>");
            exit(0);
        }
    }
    if (!strcmp(t, "frm"))
        show_frm(pid);
    if (!strcmp(t, "chatsnd"))
        chatsnd(pid);
    if (!strcmp(t, "frame_input"))
        frame_input(pid);
    if (!strcmp(t, "chatrefresh")) {
        test = 1;
        chatrefresh(pid);
    }
    printf("err cmd");
}

int reg()
{
    int n, pid;

    if (pid = fork()) {
        sprintf(genbuf, "bbschat?pid=%d&t=frm", pid);
        redirect(genbuf);
        http_quit();
    }
    for (n = 0; n < 1024; n++)
        close(n);
    pid = getpid();
    agent(pid);
    exit(0);
}

int show_frm(int pid)
{
    printf("<frameset rows=\"0,0,*,48,16\" frameborder=\"0\">\n" "	<frame name=\"hide\" src=\"\">\n" "	<frame name=\"hide2\" src=\"\">\n"
           /*"  <frame name=\"hide3\" src=\"/chataid.html\">\n" */
           "	<frame name=\"main\" src=\"\">\n"
           "	<frame scrolling=\"auto\" marginheight=\"1\" framespacing=\"1\" name=\"input\" src=\"bbschat?t=frame_input&pid=%d\">\n"
           "	<frame scrolling=\"no\" marginwidth=\"4\" marginheight=\"1\" framespacing=\"1\" name=\"foot\" src=\"bbsfoot\">\n" "	</frameset>\n" "	</html>\n", pid);
    http_quit();
}

int frame_input(int pid)
{
    printf(" <script>\n"
           "		function r1() {\n"
           "			top.hide2.location='bbschat?t=chatrefresh&pid=%d';\n"
           "			setTimeout('r1()', 10000);\n"
           "		}\n"
           "		setTimeout('r1()', 500);\n"
           "	</script>\n"
           "	<body onload=\"document.form1.in1.focus()\">\n"
           "	<nobr>\n"
           "	<form onsubmit=\"add_cc()\" name=\"form1\" action=\"bbschat?pid=%d&t=chatsnd\" method=\"post\" target=\"hide\">\n"
           "	����: <input name=\"in1\" maxlength=\"60\" size=\"56\">\n"
           "	<input type=\"submit\" value=\"����\">\n"
           "	<script>\n"
           "		var cc, cc2;\n"
           "		cc='';\n"
           "		function add_cc0(x1, x) {\n"
           "			cc2=x1;\n"
           "			cc=x;\n"
           "		}\n"
           "		function do_quit() {\n"
           "			if(confirm('�����Ҫ�˳�����')) {\n"
           "				form1.in1.value='/b';\n"
           "				form1.submit();\n"
           "			}\n"
           "		}\n"
           "		function do_help() {\n"
           "			open('/chathelp.html', '_blank', \n"
           "			'toolbar=yes,location=no,status=no,menubar=no,scrollbar=auto,resizable=yes,width=620,height=400');\n"
           "		}\n"
           "		function do_alias(x) {\n"
           "			form1.in1.value=x;\n"
           "			form1.submit();\n"
           "		}\n"
           "		function do_room() {\n"
           "			xx=prompt('�������������','');\n"
           "			if(xx=='' || xx==null) return;\n"
           "			form1.in1.value='/j '+ xx;\n"
           "			form1.submit();\n"
           "		}\n"
           "		function do_user() {\n"
           "			form1.in1.value='/l';\n"
           "			form1.submit();\n"
           "		}\n"
           "		function do_r() {\n"
           "			form1.in1.value='/r';\n"
           "			form1.submit();\n"
           "		}\n"
           "		function do_w() {\n"
           "			form1.in1.value='/w';\n"
           "			form1.submit();\n"
           "		}\n"
           "		function do_msg() {\n"
           "			xx=prompt('��˭��Сֽ��','');\n"
           "			if(xx=='' || xx==null) return;\n"
           "			yy=prompt('ʲô����','');\n"
           "			if(yy=='' || xx==null) return;\n"
           "			form1.in1.value='/m '+xx+' '+yy;\n"
           "			form1.submit();\n"
           "		}\n"
           "		function do_n() {\n"
           "			xx=prompt('����ĳ�ʲô����?','');\n"
           "			if(xx=='' || xx==null) return;\n"
           "			form1.in1.value='/n '+xx;\n"
           "			form1.submit();\n"
           "		}\n"
           "		function do_pic() {\n"
           "			xx=prompt('������ͼƬ��URL��ַ:','http://');\n"
           "			if(xx=='http://' || xx=='' || xx==null) return;\n"
           "			form1.in1.value='<img src=\"'+xx+'\">';\n"
           "			form1.submit();\n"
           "		}\n"
           "	</script>\n" "	<select onChange=\"do_alias(this.options[this.selectedIndex].value);this.selectedIndex=0;\">\n" "        <option value=\" \" selected>���춯��</option>\n"
           /*"        <option value=\"//hehe\">�Ǻǵ�ɵЦ</option>\n" */
           "	<option value=\"//faint\">Ҫ�ε���</option>\n"
           "	<option value=\"//sleep\">Ҫ˯����</option>\n"
           "	<option value=\"//:D\">�����̵�</option>\n"
           "	<option value=\"//nod\">��ͷ</option>\n"
           "	<option value=\"//so\">������</option>\n"
           "	<option value=\"//shake\">ҡҡͷ</option>\n"
           "	<option value=\"//luck\">�����˰�</option>\n"
           "	<option value=\"//tongue\">������ͷ</option>\n"
           "	<option value=\"//blush\">������</option>\n"
           "	<option value=\"//applaud\">���ҹ���</option>\n"
           "	<option value=\"//cough\">����һ��</option>\n"
           "	<option value=\"//happy\">�Ҹ�</option>\n"
           "	<option value=\"//hungry\">���Ӷ���</option>\n"
           "	<option value=\"//strut\">��ҡ���</option>\n"
           "	<option value=\"//miss\">����</option>\n"
           "	<option value=\"//think\">��һ��</option>\n"
           "	<option value=\"//?\">�ɻ���</option>\n"
           "	<option value=\"//bearbug\">����ӵ��</option>\n"
           "	<option value=\"//bless\">ף��</option>\n"
           "	<option value=\"//bow\">�Ϲ�</option>\n"
           "        <option value=\"//caress\">����</option>\n"
           "        <option value=\"//cringe\">�����ˡ</option>\n"
           "        <option value=\"//cry\">�������</option>\n"
           "        <option value=\"//comfort\">��οһ��</option>\n"
           "	<option value=\"//clap\">���ҹ���</option>\n"
           "        <option value=\"//dance\">��������</option>\n"
           "    	<option value=\"//drivel\">����ˮ</option>\n"
           "    	<option value=\"//wave\">ҡ��</option>\n"
           "    	<option value=\"//bye\">�ټ�</option>\n"
           "  	<option value=\"//giggle\">��Ц</option>\n"
           "    	<option value=\"//grin\">а���Ц</option>\n"
           "      	<option value=\"//growl\">��������</option>\n"
           "        </select>\n"
           "	<select name=\"ccc\" onChange=\"add_cc0(this, this.options[this.selectedIndex].value)\">\n"
           "	<option value=\"\" selected>��ɫ</option>\n"
           "	<option value=\"%s\"><font color=\"green\">��ɫ</font></option>\n"
           "	<option value=\"%s\"><font color=\"red\">��ɫ</font></option>\n"
           "        <option value=\"%s\"><font color=\"blue\">��ɫ</font></option>\n"
           "        <option value=\"%s\"><font color=\"blue\">����</font></option>\n"
           "        <option value=\"%s\"><font color=\"yellow\">��ɫ</font></option>\n"
           "        <option value=\"%s\"><font color=\"red\">Ʒ��</font></option>\n"
           "	<option value=\"%s\">����</option>\n"
           "	</select>\n"
           "	<select onChange=\"do_func(this.selectedIndex);this.selectedIndex=0;\">\n"
           " 	<option selected>�����ҹ���</option>\n"
           "	<option>����" CHAT_ROOM_NAME "</option>\n"
           "	<option>�鿴" CHAT_ROOM_NAME "����</option>\n"
           "	<option>��" CHAT_ROOM_NAME "��˭</option>\n"
           "	<option>����˭����</option>\n"
           "	<option>��Сֽ��</option>\n"
           "	<option>���������</option>\n"
           "	<option>��ͼƬ</option>\n"
           "	<option>�����Ļ</option>\n"
           "	<option>������ɫ</option>\n"
           "	<option>�뿪������</option>\n"
           "        </select>\n"
           "	<br>\n"
           "	[<a href=\"javascript:do_quit()\">�뿪"
           CHAT_SERVER
           "</a>] \n"
           "	[<a href=\"/chathelp.html\" target=\"_blank\">�����Ұ���</a>]\n"
           "	<script>\n"
           "	function do_func(n) {\n"
           "		if(n==0) return;\n"
           "		if(n==1) return do_room();\n"
           "		if(n==2) return do_r();\n"
           "		if(n==3) return do_w();\n"
           "		if(n==4) return do_user();\n"
           "		if(n==5) return do_msg();\n"
           "		if(n==6) return do_n();\n"
           "		if(n==7) return do_pic();\n"
           "		if(n==8) return do_c();\n"
           "		if(n==9) return do_css2();\n"
           "		if(n==10) return do_quit();\n"
           "	}\n"
           "	var css1;\n"
           "	css1='/bbschat.css';\n"
           "	function do_c() {\n"
           "		top.main.document.close();\n"
           "                top.main.document.writeln('<link rel=\"stylesheet\" type=\"text/css\" href=\"'+css1+'\"><body><pre><font class=\"c37\">');\n"
           "	}\n"
           "	function do_css2() {\n"
           "		if(css1=='bbschat.css')\n"
           "			css1='/bbschat2.css';\n"
           "		else\n"
           "			css1='/bbschat.css';\n"
           "		top.main.document.writeln('<link rel=\"stylesheet\" type=\"text/css\" href=\"'+css1+'\"><body><pre><font class=\"c37\">');\n"
           "	}\n"
           "	function add_cc() {\n"
           "	 	xxx=form1.in1.value;\n"
           "		if(xxx=='/h' || xxx=='//') {\n"
           "			do_help();\n"
           "			form1.in1.value='';\n"
           "			return; \n"
           "		}\n"
           "		if(xxx=='/c') {\n"
           "			do_c();\n"
           "			form1.in1.value='';\n"
           "			return;\n"
           "		}\n"
           "		if(xxx=='') return;\n"
           " 		if(xxx.indexOf('/')<0) {\n"
           " 			form1.in1.value=cc+xxx;\n"
           " 		}\n"
           " 		if(cc=='/I') {\n"
           " 			cc='';\n"
           " 			cc2.selectedIndex=0;\n" " 		}\n" " 	}\n" "	</script>\n" "	</form></body>\n", pid, pid, "%R", "%G", "%B", "%C", "%Y", "%M", "%I");
    http_quit();
}

int chatsnd(int pid)
{
    char in1[255], filename[256];
    FILE *fp;

    strsncpy(in1, getparm("in1"), 60);
    sprintf(filename, "tmp/%d.in", pid);
    fp = fopen(filename, "a");
    fprintf(fp, "%s\n\n", in1);
    fclose(fp);
    chatrefresh(pid);
}

char *cco(char *s)
{
    static char buf[512];
    char *p = buf, co[20];

    bzero(buf, 512);
    while (s[0]) {
        if (s[0] != '%') {
            p[0] = s[0];
            p++;
            s++;
            continue;
        }
        bzero(co, 20);
        if (!strncmp(s, "%R", 2))
            strcpy(co, "\033[31m");
        if (!strncmp(s, "%G", 2))
            strcpy(co, "\033[32m");
        if (!strncmp(s, "%B", 2))
            strcpy(co, "\033[34m");
        if (!strncmp(s, "%C", 2))
            strcpy(co, "\033[36m");
        if (!strncmp(s, "%Y", 2))
            strcpy(co, "\033[33m");
        if (!strncmp(s, "%M", 2))
            strcpy(co, "\033[35m");
        if (!strncmp(s, "%N", 2))
            strcpy(co, "\033[m");
        if (!strncmp(s, "%W", 2))
            strcpy(co, "\033[37m");
        if (!strncmp(s, "%I", 2))
            strcpy(co, "\033[99m");
        if (co[0]) {
            strncpy(p, co, strlen(co));
            p += strlen(co);
            s += 2;
            continue;
        }
        p[0] = s[0];
        p++;
        s++;
    }
    return buf;
}

int chatrefresh(int pid)
{
    char filename[256], tmp[256];
    int t1;
    FILE *fp;

    kill(pid, SIGINT);
    usleep(150000);
    if (kill(pid, 0) != 0) {
        printf("<script>top.location='javascript:close()';</script>");
        exit(0);
    }
    sprintf(filename, "tmp/%d.out", pid);
    t1 = time(0);
    while (abs(t1 - time(0)) < 8 && !file_exist(filename)) {
        sleep(1);
        continue;
    }
    fp = fopen(filename, "r");
    if (fp) {
        while (1) {
            int i;
            char buf2[512];

            if (fgets(buf2, 255, fp) == NULL)
                break;
            sprintf(genbuf, "%s", cco(buf2));
            for (i = 0; genbuf[i]; i++) {
                if (genbuf[i] == 10 || genbuf[i] == 13)
                    genbuf[i] = 0;
            }
            if (!strncmp(genbuf, "/init", 5)) {
                printf("<script>\n");
                printf("top.main.document.write('");
                printf("<link rel=\"stylesheet\" type=\"text/css\" href=\"/bbschat.css\"><body id=\"body1\" bgColor=\"black\"><pre><img src=\"/pic001.gif\">");
                printf("');");
                printf("\n</script>\n");
                continue;
            }
            if (!strncmp(genbuf, "/t", 2)) {
                int i;

                printf("<script>top.document.title='%s%s--����: ", BBS_FULL_NAME, CHAT_SERVER);
                hprintf("%s", genbuf + 2);
                printf("'</script>");
                sprintf(buf2, "��%s�Ļ�����: [\033[1;33m%s\033[37m]", CHAT_ROOM_NAME, genbuf + 2);
                strcpy(genbuf, buf2);
            }
            if (!strncmp(genbuf, "/r", 2)) {
                sprintf(buf2, "��%s��������: [\033[1;33m%s\033[37m]", CHAT_ROOM_NAME, genbuf + 2);
                strcpy(genbuf, buf2);
            }
            if (!strncmp(genbuf, "/", 1)) {
                genbuf[0] = '>';
                genbuf[1] = '>';
            }
            for (i = 0; i < strlen(genbuf); i++) {
                if (genbuf[i] == 10 || genbuf[i] == 13)
                    genbuf[i] = 0;
                if (genbuf[i] == 34)
                    genbuf[i] = 39;
            }
            printf("<script>\n");
            printf("top.main.document.writeln('");
            hhprintf("%s", genbuf);
            printf(" <font class=\"c37\">");
            printf("');");
            printf("top.main.scrollBy(0, 99999);\n");
            if (test == 0)
                printf("top.input.form1.in1.value='';\n");
            printf("</script>\n");
        }
    }
    unlink(filename);
    printf("<br>");
    http_quit();
}

void foo()
{
    FILE *fp;
    char filename[80], buf[256];

    sprintf(filename, "tmp/%d.in", getpid());
    fp = fopen(filename, "r");
    if (fp) {
        while (1) {
            if (fgets(buf, 250, fp) == NULL)
                break;
            write(chat_fd, buf, strlen(buf));
        }
        fclose(fp);
    }
    unlink(filename);
    alarm(60);
}

void abort_chat()
{
    int pid = getpid();
    char filename[200];

    sprintf(filename, "tmp/%d.out", pid);
    unlink(filename);
    sprintf(filename, "tmp/%d.in", pid);
    unlink(filename);
    exit(0);
}

int agent(int pid)
{
    int i, num;
    FILE *fp;
    char filename[80];
    struct sockaddr_in blah;

    sprintf(filename, "tmp/%d.out", pid);
    bzero((char *) &blah, sizeof(blah));
    blah.sin_family = AF_INET;
    blah.sin_addr.s_addr = inet_addr("127.0.0.1");
    blah.sin_port = htons(CHATPORT3);
    chat_fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (connect(chat_fd, (struct sockaddr *) &blah, 16) < 0)
        return;
    signal(SIGINT, foo);
    signal(SIGALRM, abort_chat);
    alarm(60);
    sprintf(genbuf, "/! %d %d %s %s %d\n", u_info->uid, getCurrentUser()->userlevel, getCurrentUser()->userid, getCurrentUser()->userid, 0);
    write(chat_fd, genbuf, strlen(genbuf));
    read(chat_fd, genbuf, 2);
    if (!strcasecmp(genbuf, "OK"))
        exit(0);
    fp = fopen(filename, "w");
    fprintf(fp, "/init\n");
    fclose(fp);
    while (1) {
        num = read(chat_fd, genbuf, 2048);
        if (num <= 0)
            break;
        for (i = 0; i < num; i++)
            if (genbuf[i] == 0)
                genbuf[i] = 10;
        genbuf[num] = 0;
        fp = fopen(filename, "a");
        fprintf(fp, "%s", genbuf);
        fclose(fp);
    }
    abort_chat();
}
