/******************************************************
令狐冲搜索2003, 作者: bad@smth.org  Qian Wenjie
在水木清华bbs系统上运行

本游戏是自由软件，请随意复制移植
请在修改后的文件头部保留版权信息
******************************************************/

#define BBSMAIN
#include "bbs.h"

#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdio.h>

#define MAX_KEEP 100

char res_title[MAX_KEEP][80],res_filename[MAX_KEEP][200];
int res_total=0,toomany=0,wh=0;

char qn[60];

int get_word()
{
    clear();
    prints("令狐冲搜索");
    move(6, 0);
    prints("    我想查                    关键字");
    move(7, 0);
    prints("-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-");
    move(8, 0);
    prints("    电影影评                  movie 无间道");
    move(9, 0);
    prints("    流行乐歌词                popmusic 我怀念有一年的夏天");
    move(10, 0);
    prints("    新闻要点                  news 科索沃");
    move(11, 0);
    prints("    武侠小说                  emprise 大唐行镖");
    move(12, 0);
    prints("    食物食谱                  food 糖醋排骨");
    move(13, 0);
    prints("    娱乐要闻                  estar 蔡卓妍 钟欣桐");
    move(14, 0);
    prints("    安全技术                  使用softice");
    move(15, 0);
    prints("    电脑硬件                  液晶显示器");
    move(16, 0);
    prints("    游戏功略                  palsword 支线");
    move(17, 0);
    prints("    编程算法                  五子棋 算法");
    move(18, 0);
    prints("    幽默笑话                  joke 偶和偶mm");
    move(19, 0);
    prints("    包含bad不包含good文章     bad -good");
    move(20, 0);
    prints("-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-");
    getdata(2, 0, "查询关键字: ", qn, 60, 1, 0, 0);
    move(3, 0);
    prints("开始查询....");
    refresh();
    return qn[0];
}

void do_query_all(int w, char * s)
{
    struct sockaddr_in addr;
    FILE* sockfp;
    int sockfd, i;
    char buf[256];
    char ip[20];
    
    if(rand()%2==0)strcpy(ip,"166.111.3.125");
    else strcpy(ip,"166.111.8.235");
    
    res_total = -2;
    
    if((sockfd=socket(AF_INET, SOCK_STREAM, 0))==-1) return;
    memset(&addr, 0, sizeof(addr));
    addr.sin_family=AF_INET;    
    addr.sin_addr.s_addr=inet_addr(ip);
    addr.sin_port=htons(4875);
    if(connect(sockfd, (struct sockaddr*)&addr, sizeof(addr))<0) return;
    sockfp=fdopen(sockfd, "r+");
    fprintf(sockfp, "%d\n%s\n", w, s);
    fflush(sockfp);
    fscanf(sockfp, "%d %d %d\n", &toomany, &i, &res_total);
    for(i=0;i<res_total;i++) {
        fgets(buf, 256, sockfp);
        if(buf[0]&&buf[strlen(buf)-1]=='\n') buf[strlen(buf)-1]=0;
        strncpy(res_title[i], buf, 80);
        res_title[i][79] = 0;

        fgets(buf, 256, sockfp);
        if(buf[0]&&buf[strlen(buf)-1]=='\n') buf[strlen(buf)-1]=0;
        strncpy(res_filename[i], buf, 200);
        res_filename[i][199] = 0;
    }
    fclose(sockfp);
    close(sockfd);
}

static int choose_file_refresh(struct _select_def *conf)
{
    clear();
    docmdtitle("[令狐冲搜索]",
              "  退出[\x1b[1;32m←\x1b[0;37m,\x1b[1;32me\x1b[0;37m] 察看[\x1b[1;32mEnter\x1b[0;37m] 选择[\x1b[1;32m↑\x1b[0;37m,\x1b[1;32m↓\x1b[0;37m]                        作者: \x1b[31;1mbad@smth.org\x1b[m");
    move(2, 0);
    prints("[0;1;37;44m    %4s %-30s %s        %d-%d 共%d  关键字:%s", "编号", "标题", "路径", wh*MAX_KEEP+1, wh*MAX_KEEP+res_total, toomany, qn);
    clrtoeol();
    resetcolor();
    update_endline();
    return SHOW_CONTINUE;
}

static int choose_file_show(struct _select_def *conf, int i)
{
    struct room_struct * r;
    char f1[160],f2[160];
    strcpy(f1, res_title[i-1]);
    strcpy(f2, res_filename[i-1]);
    prints("  %3d  %-30s %s", i+wh*MAX_KEEP, f1, f2);
    return SHOW_CONTINUE;
}

static int choose_file_select(struct _select_def *conf)
{
    char * ss;
    int ch;
again:
    ss = res_filename[conf->pos-1];
    ch = ansimore_withzmodem(ss, 0, res_title[conf->pos-1]);
    move(t_lines-1, 0);
    prints("[0;1;31;44m[第%d/%d篇]  [33m结束 Q,← │上一封 ↑│下一封 <Space>,↓│搜索关键字:%s", conf->pos+wh*MAX_KEEP, toomany, qn);
    clrtoeol();
    if(ch==0) ch=igetkey();
    switch(ch){
        case KEY_UP:
            conf->pos--;
            if(conf->pos<=0) conf->pos = res_total;
            goto again;
        case KEY_DOWN:
        case ' ':
            conf->pos++;
            if(conf->pos>res_total) conf->pos = 1;
            goto again;
    }
    return SHOW_REFRESH;
}

static int choose_file_getdata(struct _select_def *conf, int pos, int len)
{
    conf->item_count = res_total;
    return SHOW_CONTINUE;
}

static int choose_file_prekey(struct _select_def *conf, int *key)
{
    switch (*key) {
    case 'e':
    case 'q':
        *key = KEY_LEFT;
        break;
    case 'p':
    case 'k':
        *key = KEY_UP;
        break;
    case ' ':
    case 'N':
        *key = KEY_PGDN;
        break;
    case 'n':
    case 'j':
        *key = KEY_DOWN;
        break;
    }
    return SHOW_CONTINUE;
}

static int choose_file_key(struct _select_def *conf, int key)
{
    switch (key) {
    case ']':
        if((wh+1)*MAX_KEEP+1<=toomany) {
            move(t_lines-1, 0);
            prints("查找数据....");
            clrtoeol();
            refresh();
            wh++;
            do_query_all(wh, qn);
            return SHOW_DIRCHANGE;
        }
    case '[':
        if(wh>0) {
            move(t_lines-1, 0);
            prints("查找数据....");
            clrtoeol();
            refresh();
            wh--;
            do_query_all(wh, qn);
            return SHOW_DIRCHANGE;
        }
    }
    return SHOW_CONTINUE;
}

int choose_file()
{
    struct _select_def grouplist_conf;
    int i;
    POINT *pts;

    bzero(&grouplist_conf, sizeof(struct _select_def));
    grouplist_conf.item_count = res_total;
    pts = (POINT *) malloc(sizeof(POINT) * BBS_PAGESIZE);
    for (i = 0; i < BBS_PAGESIZE; i++) {
        pts[i].x = 2;
        pts[i].y = i + 3;
    }
    grouplist_conf.item_per_page = BBS_PAGESIZE;
    grouplist_conf.flag = LF_VSCROLL | LF_BELL | LF_LOOP | LF_MULTIPAGE;
    grouplist_conf.prompt = "◆";
    grouplist_conf.item_pos = pts;
    grouplist_conf.arg = NULL;
    grouplist_conf.title_pos.x = 0;
    grouplist_conf.title_pos.y = 0;
    grouplist_conf.pos = 1;     /* initialize cursor on the first mailgroup */
    grouplist_conf.page_pos = 1;        /* initialize page to the first one */

    grouplist_conf.on_select = choose_file_select;
    grouplist_conf.show_data = choose_file_show;
    grouplist_conf.pre_key_command = choose_file_prekey;
    grouplist_conf.key_command = choose_file_key;
    grouplist_conf.show_title = choose_file_refresh;
    grouplist_conf.get_data = choose_file_getdata;
    list_select_loop(&grouplist_conf);
    free(pts);
}

int show_res()
{
    int i;
    if(res_total<=0) {
        move(4,0);
        if(res_total==-2) prints("负荷太重，请稍后再使用");
        else prints("什么都没搜到！");
        igetkey();
    }
    else {
        choose_file();
    }
}

int iquery_main()
{
    qn[0] = 0;
    while(get_word()) {
        wh = 0;
        do_query_all(wh, qn);
        show_res();
    }
}
