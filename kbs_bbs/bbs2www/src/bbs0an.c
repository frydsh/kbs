/*
 * $Id$
 */
#include "bbslib.h"

int ret=0;
int ann_is_bm = 0;

void ann_show_item(MENU * pm, ITEM * it)
{
    char title[STRLEN];
    char *id;
    char buf[256];
    char pathbuf[256];
	char title_en[256];
    char *ptr;

    strncpy(title, it->title, sizeof(title) - 1);
    title[sizeof(title) - 1] = '\0';
    if (strlen(title) <= 39) {
        id = "";
    } else {
        if ((ptr = strchr(title + 38, '(')) != NULL) {
            *ptr = '\0';
            id = ptr + 1;
            if (strncmp(id, "BM: ", 4) == 0)
                id += 4;
            if ((ptr = strchr(id, ')')) != NULL)
                *ptr = '\0';
        } else if ((ptr = strchr(title + 38, ' ')) != NULL) {
            *ptr = '\0';
            id = ptr + 1;
            trim(id);
        } else
            id = "";
        rtrim(title);
    }
    printf("<tr><td>%d</td>", pm->now + 1);
    sprintf(buf, "%s/%s", pm->path, it->fname);
    ptr = strchr(pm->path, '/');
	encode_html(title_en, title, sizeof(title_en));
	if (!file_exist(buf)) {
        printf("<td>[����] </td><td>%s</td>", title_en);
    } else if (file_isdir(buf)) {
        snprintf(pathbuf, sizeof(pathbuf), "%s/%s", ptr == NULL ? "" : ptr, it->fname);
        printf("<td>[Ŀ¼] </td><td><a href=\"bbs0an?path=%s\">%s</a></td>", 
				http_encode_string(pathbuf, sizeof(pathbuf)), 
				title_en);
    } else {
        snprintf(pathbuf, sizeof(pathbuf), "%s/%s", ptr == NULL ? "" : ptr, it->fname);
        printf("<td>[%s] </td><td><a href=\"/bbsanc.php?path=%s\">%s</a></td>", 
				it->attachpos?"����":"�ļ�",
				http_encode_string(pathbuf, sizeof(pathbuf)), 
				title_en);
    }
    if (id[0]) {
        printf("<td>%s</td>", userid_str(id));
    } else {
        printf("<td>&nbsp;</td>");
    }
    printf("<td>%6.6s %s</td>\n", wwwCTime(file_time(buf)) + 4, wwwCTime(file_time(buf)) + 20);

	if(ann_is_bm){

	    snprintf(pathbuf, sizeof(pathbuf), "%s", ptr == NULL ? "" : ptr);
		printf("<td><a href=\"#\" onclick=\"if(confirm('ȷ��Ҫɾ����Ŀ¼��?(���ָܻ�)')){window.location.href='bbs0an?path=%s&action=del&dpath=%s';return true;}return false;\">ɾ��</a></td>",
				http_encode_string(pathbuf, sizeof(pathbuf)),it->fname);
	}

	printf("</tr>");
}

void ann_show_toolbar(char * path, char * board)
{
    char buf[STRLEN];
	char *c;

		printf("<br>[<a href=\"javascript:history.go(-1)\">���ٷ���</a>] ");
		strncpy(buf,path,STRLEN);
		buf[STRLEN-1]='\0';
		if((c=strrchr(buf,'/'))!=NULL) *c='\0';
		if (buf[0]) printf("[<a href=\"bbs0an?path=%s\">������һ��Ŀ¼</a>]",buf);
        if (board[0]){
            printf("[<a href=\"/bbsdoc.php?board=%s\">��������</a>]\n", encode_url(buf, board, sizeof(buf)));
    			if(ann_is_bm){
					printf("[<a href=\"/bbsmpath.php?action=add&path=%s\">��Ϊ˿·</a>]<br>",encode_url(buf,path,sizeof(buf)));
					printf("<form action=\"bbs0an\" method=\"post\">");
					printf("<input type=\"hidden\" name=\"action\" value=\"add\">");
					printf("<input type=\"hidden\" name=\"path\" value=\"%s\">",path);
					printf("Ŀ¼��(ֻ��Ӣ�ĺ�����)<input type=\"text\" name=\"dpath\">");
					printf("����<input type=\"text\" name=\"fname\">");
					printf("<input type=\"submit\" name=\"submit\" value=\"���Ŀ¼\">");
					printf("</form>");
				}
		}
}

void ann_show_directory(char *path,char * pathbuf)
{
    MENU me;
    ITEM *its;
    char board[STRLEN];
    char buf[STRLEN];
    int i;

    if ((its = ann_alloc_items(MAXITEMS)) == NULL)
        http_fatal("�����ڴ�ʧ��");
    ann_set_items(&me, its, MAXITEMS);
    me.path = pathbuf;
    if (ann_load_directory(&me, getSession()) == 0) {
        board[0] = '\0';
        ann_get_board(pathbuf, board, sizeof(board));
        buf[0] = '\0';
        if (board[0] != '\0'){
            sprintf(buf, "%s��", board);
			if(ret > 0) bmlog(getCurrentUser()->userid,board,13,1);
		}

		else
			buf[0]='\0';
        printf("<center>\n");
        printf("%s -- %s������<hr class=\"default\">\n", BBSNAME, buf);
        if (me.num <= 0) {
            printf("<br>&lt;&lt; Ŀǰû������ &gt;&gt;\n");
			ann_show_toolbar(path,board);
            http_quit();
        }
        printf("<table class=\"default\" border=\"1\" width=\"610\">\n");
        printf("<tr><td>���</td><td>���</td><td>����</td><td>����</td><td>����</td>%s</tr>\n",
						ann_is_bm?"<td>ɾ��</td>":"");
        me.now = 0;
        for (i = 0; i < me.num; i++) {
            trim(me.item[i]->title);
            ann_show_item(&me, me.item[i]);
            me.now++;
        }
        printf("</table>");
		ann_show_toolbar(path,board);
        printf("</center>\n");
    }
    ann_free_items(its, MAXITEMS);
}

int ann_check_dir(char * path,char *pathbuf)
{
	int len;

    if (strstr(path, "..") || strstr(path, "SYSHome"))  /* SYSHome? */
		return 0;
    if (path[0] != '\0') {
        len = strlen(path);
        if (path[len - 1] == '/')
            path[len - 1] = '\0';
        if (path[0] == '/')
            snprintf(pathbuf, 255, "0Announce%s", path);
        else
            snprintf(pathbuf, 255, "0Announce/%s", path);
        if (ann_traverse_check(pathbuf, getCurrentUser()) < 0)
			return 0;
    } else
        strcpy(pathbuf, "0Announce");

	return 1;
}

int ann_check_bm(char *buf)
{

	char board[STRLEN];
	char *c;

	board[0]='\0';
    ann_get_board(buf, board, sizeof(board));
	if(board[0]=='\0') return 0;

	if(!has_BM_perm(getCurrentUser(), board))
		return 0;

	ann_is_bm = 1;
	return 1;
}

int ann_del_dir(char *path,char *pathbuf,char *fname)
{
	char fpath[PATHLEN];
	char fpath2[PATHLEN];
	char buf[STRLEN];
	MENU pm;
	FILE *fp;
	int n,m;

	if(! ann_is_bm )
		return -1;

	sprintf(fpath, "%s/%s", pathbuf, fname);

	if(dashf(fpath)){
		unlink(fpath);
	}else if(dashd(fpath)){
		f_rm(fpath);
	}else{
		return -3;
	}

	bzero(&pm,sizeof(pm));
	pm.path=pathbuf;
	a_loadnames(&pm,getSession());

	for(n=0; n<pm.num; n++){
		if(! strcmp(pm.item[n]->fname,fname) )
			break;
	}
	if(n < pm.num){
		free(pm.item[n]);
		pm.num--;
		for(m=n; m<pm.num; m++)
			pm.item[m] = pm.item[m+1];

		if(a_savenames(&pm)){
			a_freenames(&pm);
			return -4;
		}

		a_freenames(&pm);
		return 1;
	}
	else{
		a_freenames(&pm);
		return -5;
	}

}
	
int ann_add_dir(char *path,char *pathbuf,char *fname,char *title)
{
	char fpath[PATHLEN];
	char fpath2[PATHLEN];
	char buf[STRLEN];
	MENU pm;
	FILE *fp;

	if(! ann_is_bm)
		return -1;

	if(!valid_fname(fname))
		return -2;

	sprintf(fpath, "%s/%s", pathbuf, fname);

	if(dashf(fpath) || dashd(fpath))
		return -3;

	mkdir(fpath, 0755);
	chmod(fpath, 0755);

	bzero(&pm,sizeof(pm));
	pm.path=pathbuf;
	a_loadnames(&pm, getSession());

	sprintf(buf,"%-38.38s",title);

	a_additem(&pm,buf,fname,NULL,0,0);

	if(a_savenames(&pm)){
		a_freenames(&pm);
		return -4;
	}

	a_freenames(&pm);

	sprintf(fpath, "%s/%s/.Names", pathbuf, fname);
	if((fp=fopen(fpath,"w"))==NULL)
		return -5;

	fprintf(fp, "#\n");
	fprintf(fp, "# Title=%s\n", buf);
	fprintf(fp, "#\n");
	fclose(fp);

	return 1;

}

int main()
{
    char path[512];
	char *c;
	char action[10];
	char dpath[PATHLEN];
	char title[STRLEN];
    char pathbuf[256];

    initwww_all();
    strsncpy(path, getparm("path"), 511);

	if(! ann_check_dir(path,pathbuf))
        http_fatal("��Ŀ¼������");

	ann_check_bm(pathbuf);

	c = getparm("action");

	if(c && *c){
		strncpy(action,c,9);
		action[9]='\0';
		if(! strcmp(action,"add") ){
			c = getparm("dpath");
			if(!c || ! *c)
				http_fatal("Ŀ¼·������");
			strncpy(dpath,c,PATHLEN-1);
			dpath[PATHLEN-1]='\0';

			c = getparm("fname");
			if(!c || ! *c)
				http_fatal("Ŀ¼��Ŀ����");
			strncpy(title,c,STRLEN-1);
			title[STRLEN-1]='\0';

			ret = ann_add_dir(path,pathbuf,dpath,title);
			if(ret < 0){
				switch(ret){
				case -1:
					http_fatal("��û�����Ȩ��");
					break;
				case -2:
					http_fatal("�ļ������Ϸ�");
					break;
				case -3:
					http_fatal("Ŀ¼�Ѿ�����");
					break;
				default:
					http_fatal("ϵͳ����");
					break;
				}
			}
		}
		else if(! strcmp(action,"del") ){
			c = getparm("dpath");
			if(!c || ! *c)
				http_fatal("Ŀ¼·������");
			strncpy(dpath,c,PATHLEN-1);
			dpath[PATHLEN-1]='\0';

			ret = ann_del_dir(path,pathbuf,dpath);
			if(ret < 0){
				switch(ret){
				case -1:
					http_fatal("��û�����Ȩ��");
					break;
				case -5:
					http_fatal("û������ļ�����Ŀ¼");
					break;
				case -3:
					http_fatal("Ŀ¼������");
					break;
				default:
					http_fatal("ϵͳ����");
					break;
				}
			}
		}
	}

    ann_show_directory(path,pathbuf);
    http_quit();
}
