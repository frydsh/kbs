#include "bbs.h"

static int ann_can_access(char *title, char *board, struct userec *user)
{
    struct boardheader *bp;
    char BM[STRLEN];

    if (strstr(title, "(BM: BMS)")) {
        if (board[0] == '\0')
            return 0;
        bp = getbcache(board);
        if (bp == NULL)
            return 0;
        strncpy(BM, bp->BM, sizeof(BM) - 1);
        BM[sizeof(BM) - 1] = '\0';
        if (chk_currBM(BM, user) == 0)
            return 0;
        return 2;
    }
    if (strstr(title, "(BM: SYSOPS)")) {
        if (!HAS_PERM(user, PERM_SYSOP))
            return 0;
        return 3;
    }
    return 1;
}

void ann_add_item(MENU * pm, ITEM * it)
{
    ITEM *newitem;

    if (pm->num < MAXITEMS) {
        newitem = pm->item[pm->num];
        strncpy(newitem->title, it->title, sizeof(newitem->title) - 1);
        if (it->host != NULL) {
            newitem->host = (char *) malloc(sizeof(char) * (strlen(it->host) + 1));
            strcpy(newitem->host, it->host);
        } else
            newitem->host = it->host;
        newitem->port = it->port;
        newitem->attachpos= it->attachpos;
        strncpy(newitem->fname, it->fname, sizeof(newitem->fname) - 1);
        (pm->num)++;
    }
}

int ann_load_directory(MENU * pm, session_t* session)
{
    FILE *fn;
    ITEM litem;
    char buf[PATHLEN];
    char *ptr;
    char hostname[STRLEN];
    char board[STRLEN];

    pm->num = 0;
    if ((ptr = strstr(pm->path, "groups/")) != NULL)
        ann_get_board(ptr, board, sizeof(board));
    else
        board[0] = '\0';
    snprintf(buf, sizeof(buf), "%s/.Names", pm->path);  /*.Names��¼�˵���Ϣ */
    if ((fn = fopen(buf, "r")) == NULL)
        return -1;
    bzero(&litem, sizeof(litem));
    hostname[0] = '\0';
    while (fgets(buf, sizeof(buf), fn) != NULL) {
        if ((ptr = strchr(buf, '\n')) != NULL)
            *ptr = '\0';
        if (strncmp(buf, "Name=", 5) == 0) {
            strncpy(litem.title, buf + 5, sizeof(litem.title) - 1);
            litem.title[sizeof(litem.title) - 1] = '\0';
            litem.attachpos = 0;
        } else if (strncmp(buf, "Path=", 5) == 0) {
            if (strncmp(buf, "Path=~/", 7) == 0)
                strncpy(litem.fname, buf + 7, sizeof(litem.fname) - 1);
            else
                strncpy(litem.fname, buf + 5, sizeof(litem.fname) - 1);
            if (strstr(litem.fname,"..")) continue;
            litem.fname[sizeof(litem.fname) - 1] = '\0';
            /*if ((!strstr(litem.title,"(BM: BMS)")||HAS_PERM(session->currentuser,PERM_BOARDS))&&
               (!strstr(litem.title,"(BM: SYSOPS)")||HAS_PERM(session->currentuser,PERM_SYSOP))) */
            if (ann_can_access(litem.title, board, session->currentuser)) {
                if (strstr(litem.fname, "!@#$%")) {     /*ȡ host & port */
                    char *ptr1, *ptr2, gtmp[STRLEN];

                    strncpy(gtmp, litem.fname, STRLEN - 1);
                    /* Warning: NOT thread safe */
                    ptr1 = strtok(gtmp, "!#$%@");
                    strcpy(hostname, ptr1);
                    ptr2 = strtok(NULL, "@");
                    strncpy(litem.fname, ptr2, sizeof(litem.fname) - 1);
                    litem.fname[sizeof(litem.fname) - 1] = '\0';
                    litem.port = atoi(strtok(NULL, "@"));
                }
                ann_add_item(pm, &litem);
            }
            hostname[0] = '\0';
        } else if (strncmp(buf, "# Title=", 8) == 0) {
            if (pm->mtitle[0] == '\0') {
                strncpy(pm->mtitle, buf + 8, sizeof(pm->mtitle) - 1);
                pm->mtitle[sizeof(pm->mtitle) - 1] = '\0';
            }
        } else if (strncmp(buf, "Host=", 5) == 0) {
            strncpy(hostname, buf + 5, sizeof(hostname) - 1);
            hostname[sizeof(hostname) - 1] = '\0';
        } else if (strncmp(buf, "Port=", 5) == 0) {
            litem.port = atoi(buf + 5);
        } else if (strncmp(buf, "Attach=", 7) == 0) {
            litem.attachpos= atol(buf + 7);
        }
    }
    fclose(fn);
    return 0;
}

ITEM *ann_alloc_items(size_t num)
{
    ITEM *it;

    it = (ITEM *) calloc(num, sizeof(ITEM));
    return it;
}

void ann_free_items(ITEM * it, size_t num)
{
    size_t i;

    if (it != NULL) {
        for (i = 0; i < num; i++) {
            if (it[i].host != NULL) {
                free(it[i].host);
                it[i].host = NULL;
            }
        }
        free(it);
    }
}

void ann_set_items(MENU * pm, ITEM * it, size_t num)
{
    size_t i;

    for (i = 0; i < num; i++)
        pm->item[i] = it + i;
}

struct _tmp_findboard {
    char *path;
    char *board;
    size_t len;
};

static int findboard(struct boardheader* bh,void* data)
{
    struct _tmp_findboard *arg = (struct _tmp_findboard*)data;

	if(! bh->ann_path || ! bh->ann_path[0] ) return 0;

    if (!strncmp(bh->ann_path,arg->path,strlen(bh->ann_path))) {
        strncpy(arg->board,bh->filename,arg->len);
        arg->board[arg->len-1]=0;
        return QUIT;
    }
    return 0;
}

/*
 * Get a board name from an announce path.
*/
int ann_get_board(char *path, char *board, size_t len)
{
    char *ptr;
    struct _tmp_findboard arg;

    ptr = path;
    if (ptr[0] == '\0')
        return -1;
    if (ptr[0] == '/')
        ptr++;
    if (! strncmp(path,"0Announce/groups/",strlen("0Announce/groups/"))){
	    arg.path=path+strlen("0Announce/groups/");
	    arg.board=board;
	    arg.len=len;
	    if (apply_boards(findboard,&arg)==QUIT)
	        return 0;
	    return -1;
	}else if(! strncmp(path,"groups/",7)){
	    arg.path=path+7;
	    arg.board=board;
	    arg.len=len;
	    if (apply_boards(findboard,&arg)==QUIT)
	        return 0;
	    return -1;
	}
	return -1;
}

/*
 * Get an announce path from a board name.
*/
int ann_get_path(char *board, char *path, size_t len)
{
    struct boardheader* bh;
    if ((bh=getbcache(board))!=NULL) {
        if (strlen(bh->ann_path)+strlen("/groups")>len)
            return -1;
        sprintf(path,"/groups/%s",bh->ann_path);
        return 0;
    }
    return -1;
}

int ann_traverse_check(char *path, struct userec *user)
{
    char *ptr;
    char *ptr2;
    size_t i = 0;
    char filename[256];
    char buf[256];
    char pathbuf[256];
    char title[STRLEN];
    char currpath[256];
    FILE *fp;
    char board[STRLEN];
    int ret = 0;

    /* path parameter can not have leading '/' character */
    if (path[0] == '/')
        return -1;
    if ((ptr = strstr(path, "groups/")) != NULL)
        ann_get_board(ptr, board, sizeof(board));
    else
        board[0] = '\0';
    bzero(pathbuf, sizeof(pathbuf));
    ptr = path;
    while (*ptr != '\0') {
        if (*ptr == '/')
            snprintf(filename, sizeof(filename), "%s/.Names", pathbuf);
        else {
            if (i < sizeof(pathbuf))
                pathbuf[i] = *ptr;
            ptr++;
            i++;
            continue;
        }
        if ((fp = fopen(filename, "r")) == NULL)
            return -1;
        while (fgets(buf, sizeof(buf), fp) != NULL) {
            int t;

            if ((ptr2 = strrchr(buf, '\n')) != NULL)
                *ptr2 = '\0';
            if (strncmp(buf, "Name=", 5) == 0) {
                strncpy(title, buf + 5, sizeof(title) - 1);
                title[sizeof(title) - 1] = '\0';
                continue;
            }
            if (strncmp(buf, "Path=~/", 7) == 0)
                snprintf(currpath, sizeof(currpath), "%s/%s", pathbuf, buf + 7);
            else if (strncmp(buf, "Path=", 5) == 0)
                snprintf(currpath, sizeof(currpath), "%s/%s", pathbuf, buf + 5);
            else
                continue;
            if (strncmp(currpath, path, strlen(currpath)) != 0)
                continue;
            /*if ((!strstr(title,"(BM: BMS)")||HAS_PERM(session->currentuser,PERM_BOARDS))&&
               (!strstr(title,"(BM: SYSOPS)")||HAS_PERM(session->currentuser,PERM_SYSOP))&&
               (!strstr(title,"(BM: ZIXIAs)")||HAS_PERM(session->currentuser,PERM_SECANC))) */
            if ((t = ann_can_access(title, board, user)) != 0) {
                if (ret < t)
                    ret = t;    /* directory can be accessed but it should be access with some
                                   permission */
                break;
            } else {
                /* diretory cannot be accessed */
                fclose(fp);
                return -1;
            }
        }
        if (feof(fp)) {
            fclose(fp);
            return -1;
        }
        fclose(fp);
        if (i < sizeof(pathbuf))
            pathbuf[i] = *ptr;
        ptr++;
        i++;
    }
    return ret;
}

void a_freenames(MENU * pm)
{
    int i;

    for (i = 0; i < pm->num; i++)
        free(pm->item[i]);
}

void a_additem(MENU* pm,const char* title,const char* fname,char* host,int port,long attachpos)    /* ����ITEM object,����ʼ�� */
{
    ITEM *newitem;

    if (pm->num < MAXITEMS) {
        newitem = (ITEM *) malloc(sizeof(ITEM));
        strncpy(newitem->title, title, sizeof(newitem->title) - 1);
        if (host != NULL) {
            newitem->host = (char *) malloc(sizeof(char) * (strlen(host) + 1));
            strcpy(newitem->host, host);
        } else
            newitem->host = host;
        newitem->port = port;
        newitem->attachpos = attachpos;
        strncpy(newitem->fname, fname, sizeof(newitem->fname) - 1);
        pm->item[(pm->num)++] = newitem;
    }
}

int a_loadnames(MENU* pm, session_t* session)             /* װ�� .Names */
{
    FILE *fn;
    ITEM litem;
    char buf[PATHLEN], *ptr;
    char hostname[STRLEN];
    struct stat st;

    a_freenames(pm);
    pm->num = 0;
    sprintf(buf, "%s/.Names", pm->path);        /*.Names��¼�˵���Ϣ */
    if ((fn = fopen(buf, "r")) == NULL)
        return 0;
    if (fstat(fileno(fn), &st) != -1)
        pm->modified_time = st.st_mtime;
    hostname[0] = '\0';
    while (fgets(buf, sizeof(buf), fn) != NULL) {
        if ((ptr = strchr(buf, '\n')) != NULL)
            *ptr = '\0';
        if (strncmp(buf, "Name=", 5) == 0) {
            strncpy(litem.title, buf + 5, sizeof(litem.title));
            litem.attachpos = 0;
        } else if (strncmp(buf, "Path=", 5) == 0) {
            if (strncmp(buf, "Path=~/", 7) == 0)
                strncpy(litem.fname, buf + 7, sizeof(litem.fname));
            else
                strncpy(litem.fname, buf + 5, sizeof(litem.fname));
            if (strstr(litem.fname,"..")) continue;
            if (HAS_PERM(session->currentuser, PERM_SYSOP)
                ||((!strstr(litem.title, "(BM: BMS)") || HAS_PERM(session->currentuser, PERM_BOARDS))
                 &&(!strstr(litem.title, "(BM: SYSOPS)") || HAS_PERM(session->currentuser, PERM_SYSOP)) 
                 &&(!strstr(litem.title, "(BM: ZIXIAs)") || HAS_PERM(session->currentuser, PERM_SECANC)))){
                if (strstr(litem.fname, "!@#$%")) {     /*ȡ host & port */
                    char *ptr1, *ptr2, gtmp[STRLEN];

                    strncpy(gtmp, litem.fname, STRLEN - 1);
                    ptr1 = strtok(gtmp, "!#$%@");
                    strcpy(hostname, ptr1);
                    ptr2 = strtok(NULL, "@");
                    strncpy(litem.fname, ptr2, sizeof(litem.fname) - 1);
                    litem.port = atoi(strtok(NULL, "@"));
                }
                a_additem(pm, litem.title, litem.fname, (strlen(hostname) == 0) ?       /*����ITEM */
                          NULL : hostname, litem.port, litem.attachpos);
            }
            hostname[0] = '\0';
        } else if (strncmp(buf, "# Title=", 8) == 0) {
            if (pm->mtitle[0] == '\0')
                strncpy(pm->mtitle, buf + 8, STRLEN);
        } else if (strncmp(buf, "Host=", 5) == 0) {
            strcpy(hostname, buf + 5);
        } else if (strncmp(buf, "Port=", 5) == 0) {
            litem.port = atoi(buf + 5);
        } else if (strncmp(buf, "Attach=", 7) == 0) {
            litem.attachpos= atoi(buf + 7);
        }
    }
    fclose(fn);
    return 1;
}

int a_savenames(MENU* pm)             /*���浱ǰMENU�� .Names */
{
    FILE *fn;
    ITEM *item;
    char fpath[PATHLEN];
    int n;
    struct stat st;

    sprintf(fpath, "%s/.Names", pm->path);
    if (stat(fpath, &st) != -1) {
        if (st.st_mtime != pm->modified_time)
            return -3;
    }
    if ((fn = fopen(fpath, "w")) == NULL)
        return -1;
    fprintf(fn, "#\n");
    if (!strncmp(pm->mtitle, "[Ŀ¼] ", 7) || !strncmp(pm->mtitle, "[�ļ�] ", 7)
        || !strncmp(pm->mtitle, "[����] ", 7)) {
        fprintf(fn, "# Title=%s\n", pm->mtitle + 7);
    } else {
        fprintf(fn, "# Title=%s\n", pm->mtitle);
    }
    fprintf(fn, "#\n");
    for (n = 0; n < pm->num; n++) {
        item = pm->item[n];
        if (!strncmp(item->title, "[Ŀ¼] ", 7) || !strncmp(item->title, "[�ļ�] ", 7)
            || !strncmp(item->title, "[����] ", 7)) {
            fprintf(fn, "Name=%s\n", item->title + 7);
        } else
            fprintf(fn, "Name=%s\n", item->title);
        fprintf(fn, "Attach=%ld\n", item->attachpos);
        if (item->host != NULL) {
            fprintf(fn, "Host=%s\n", item->host);
            fprintf(fn, "Port=%d\n", item->port);
            fprintf(fn, "Type=1\n");
            fprintf(fn, "Path=%s\n", item->fname);
        } else
            fprintf(fn, "Path=~/%s\n", item->fname);
        fprintf(fn, "Numb=%d\n", n + 1);
        fprintf(fn, "#\n");
    }
    fclose(fn);
    if (stat(fpath, &st) != -1)
        pm->modified_time = st.st_mtime;
    chmod(fpath, 0644);
    return 0;
}

int save_import_path(char **i_path,char **i_title,time_t* i_path_time, session_t* session)
{
    FILE *fn;
    int i;
    char buf[MAXPATH];

    sethomefile(buf, session->currentuser->userid, "BMpath");
    fn = fopen(buf, "wt");
    if (fn) {
        struct stat st;

        for (i = 0; i < ANNPATH_NUM; i++) {
            fputs(i_path[i], fn);
            fputs("\n", fn);
            fputs(i_title[i], fn);
            fputs("\n", fn);
        }
        fstat(fileno(fn), &st);
        fclose(fn);
        * i_path_time = st.st_mtime;
        return 0;
    }
    return -1;
}

void load_import_path(char ** i_path,char ** i_title, time_t* i_path_time,int * i_path_select, session_t* session)
{
    FILE *fn;
    char buf[MAXPATH];
    int i;
    struct stat st;

    sethomefile(buf, session->currentuser->userid, "BMpath");
    if (stat(buf, &st) != -1)
        if (st.st_mtime == * i_path_time)
            return;
    if (* i_path_select != 0)
        free_import_path(i_path,i_title,i_path_time);
    fn = fopen(buf, "rt");
    if (fn) {
        * i_path_time = st.st_mtime;
        for (i = 0; i < ANNPATH_NUM; i++) {
            if (!feof(fn)) {
                fgets(buf, MAXPATH - 1, fn);
                if (buf[strlen(buf) - 1] == '\n')
                    buf[strlen(buf) - 1] = 0;
            } else
                buf[0] = 0;
            /*
             * TODO: access check need complete!
             * if (buf[0]!=0&&(ann_traverse_check(buf, session->currentuser)!=0))
             * buf[0]=0;  can't access 
             */

            i_path[i] = (char *) malloc(strlen(buf) + 1);
            strcpy(i_path[i], buf);
            if (!feof(fn)) {
                fgets(buf, MAXPATH - 1, fn);
                if (buf[strlen(buf) - 1] == '\n')
                    buf[strlen(buf) - 1] = 0;
            } else {            //get the title of pm
                buf[0] = 0;
                if (i_path[i][0]) {
                    MENU pm;

                    bzero(&pm, sizeof(pm));
                    pm.path = i_path[i];
                    a_loadnames(&pm, session);
                    strncpy(buf, pm.mtitle, MAXPATH - 1);
                    buf[MAXPATH - 1] = 0;
                    a_freenames(&pm);
                }
            }
            if (i_path[i][0] == 0) /* if invalid path,then let the title empty */
                buf[0] = 0;
            i_title[i] = (char *) malloc(strlen(buf) + 1);
            strcpy(i_title[i], buf);
        }
        fclose(fn);
    } else {
        for (i = 0; i < ANNPATH_NUM; i++) {
            i_path[i] = (char *) malloc(1);
            i_path[i][0] = 0;
            i_title[i] = (char *) malloc(1);
            i_title[i][0] = 0;
        }
        save_import_path(i_path,i_title,i_path_time, session);
    }
    * i_path_select = 1;
}

void free_import_path(char ** i_path,char ** i_title,time_t * i_path_time)
{
    int i;

    for (i = 0; i < ANNPATH_NUM; i++) {
        if (i_path[i] != NULL) {
            free(i_path[i]);
            i_path[i] = NULL;
        }
        if (i_title[i] != NULL) {
            free(i_title[i]);
            i_title[i] = NULL;
        }
    }
    * i_path_time = 0;
}

int valid_fname(str)
char *str;
{
    char ch;

	if(strstr(str,"..")) return 0;

    while ((ch = *str++) != '\0') {
        if ((ch >= 'A' && ch <= 'Z') || (ch >= 'a' && ch <= 'z') || strchr("0123456789@[]-._", ch) != NULL) {
            ;
        } else {
            return 0;
        }
    }
    return 1;
}

int linkto(char *path, const char *fname, const char *title, session_t* session)
{
    MENU pm;

    bzero(&pm,sizeof(pm));
    pm.path = path;

    strcpy(pm.mtitle, title);
    a_loadnames(&pm, session);
    a_additem(&pm, title, fname, NULL, 0, 0);
    if (a_savenames(&pm) != 0) {
#ifdef BBSMAIN
        char buf[80], ans[40];

        sprintf(buf, "��������ʧ�ܣ����������������ڴ���ͬһĿ¼���� Enter ���� ");
        a_prompt(-1, buf, ans);
#endif
    }
    a_freenames(&pm);
    return 0;
}

/*
 * ����˵��:
 *     group    groups[]�е�ĳ��
 *     bname    ���Ӣ����
 *     title    �澫������������
 *     gname    �� group ��Ӧ��������
 */
int add_grp(const char group[STRLEN],const char bname[STRLEN],const char title[STRLEN],const char gname[STRLEN], session_t* session)
        /*
         * ������ �� Ŀ¼ 
         */
{
    FILE *fn;
    char buf[PATHLEN];
    char searchname[STRLEN];
    char gpath[STRLEN * 2];
    char bpath[STRLEN * 2];

    sprintf(searchname, "%s: groups/%s/%s", bname, group, bname);
    sprintf(gpath, "0Announce/groups/%s", group);
    sprintf(bpath, "%s/%s", gpath, bname);
    if (!dashd("0Announce")) {
        mkdir("0Announce", 0755);
        chmod("0Announce", 0755);
        if ((fn = fopen("0Announce/.Names", "w")) == NULL)
            return -1;
        fprintf(fn, "#\n");
        fprintf(fn, "# Title=%s ������������\n", BBS_FULL_NAME);
        fprintf(fn, "#\n");
        fclose(fn);
    }
    if (!dashd("0Announce/groups")) {
        mkdir("0Announce/groups", 0755);
        chmod("0Announce/groups", 0755);

        linkto("0Announce", "groups", "����������", session);
    }
    if (!dashd(gpath)) {
        mkdir(gpath, 0755);
        chmod(gpath, 0755);
        linkto("0Announce/groups", group, gname, session);
    }
    if (!dashd(bpath)) {
        mkdir(bpath, 0755);
        chmod(bpath, 0755);
        linkto(gpath, bname, title, session);
        sprintf(buf, "%s/.Names", bpath);
        if ((fn = fopen(buf, "w")) == NULL) {
            return -1;
        }
        fprintf(fn, "#\n");
        fprintf(fn, "# Title=%s\n", title);
        fprintf(fn, "#\n");
        fclose(fn);
    }
    return 0;
}

