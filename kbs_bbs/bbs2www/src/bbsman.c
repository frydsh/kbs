/*
 * $Id$
 */
#include "bbslib.h"

char genbuf[1024];
char currfile[STRLEN];
char im_path[MAXPATH];

int change_flag(struct fileheader*f,char* board,char*dirdir,int ent, int flag)
{
    int ret;
        struct write_dir_arg dirarg;
        struct boardheader* bh;
        struct fileheader data;
        data.accessed[0] = ~(f->accessed[0]);
        data.accessed[1] = ~(f->accessed[1]);
        bh=getbcache(board);
        init_write_dir_arg(&dirarg);
        dirarg.filename=dirdir;
        dirarg.ent = ent;
        if(change_post_flag(&dirarg, 
            DIR_MODE_NORMAL, 
            bh, f, flag, &data,true)!=0)
        ret = 1;
    else
        ret = 0;
        free_write_dir_arg(&dirarg);
     return ret;
}
int main()
{
    int i, total = 0, mode,num;
    char board[80], *ptr;
    char buf[STRLEN];
    boardheader_t *brd;

    init_all();
    if (!loginok)
        http_fatal("���ȵ�¼");
    strsncpy(board, getparm("board"), 60);
    mode = atoi(getparm("mode"));
    brd = getbcache(board);
    if (brd == 0)
        http_fatal("�����������");
    strcpy(board, brd->filename);
    if (!has_BM_perm(currentuser, board))
        http_fatal("����Ȩ���ʱ�ҳ");
    if (mode <= 0 || mode > 6)
        http_fatal("����Ĳ���");
	if( mode == 6 ){
                char *i_path[ANNPATH_NUM];
                char *i_title[ANNPATH_NUM];
                time_t i_time = 0;
                int i_select = 0;
		num = atoi(getparm("num"));
		if( num < 0 || num >= ANNPATH_NUM )
			http_fatal("˿·��������");
		load_import_path(i_path,i_title,&i_time,&i_select);
		strcpy(im_path,i_path[num]);
		free_import_path(i_path,i_title,&i_time);
		if(strncmp(im_path,"0Announce/groups/",17))
			http_fatal("˿·����");
	}
    printf("<table>");
    for (i = 0; i < parm_num && i < 40; i++) {
        if (!strncmp(parm_name[i], "box", 3)) {
            total++;
            if (mode == 1)
                do_del(board, atoi(parm_name[i] + 3));
            if (mode == 2)
                do_set(board, atoi(parm_name[i] + 3), FILE_MARK_FLAG);
            if (mode == 3)
                do_set(board, atoi(parm_name[i] + 3), FILE_DIGEST_FLAG);
            if (mode==4)
                do_set(board, atoi(parm_name[i] + 3), FILE_NOREPLY_FLAG);
            if (mode == 5)
                do_set(board, atoi(parm_name[i] + 3), FILE_DING_FLAG);
			if (mode == 6)
				do_import(board,atoi(parm_name[i]+3));
        }
       	else if (!strncmp(parm_name[i], "boz", 3)) {
            total++;
            if (mode == 1)
                do_del_zd(board, atoi(parm_name[i] + 3));
            if (mode == 2)
                do_set_zd(board, atoi(parm_name[i] + 3), FILE_MARK_FLAG);
            if (mode == 3)
                do_set_zd(board, atoi(parm_name[i] + 3), FILE_DIGEST_FLAG);
            if (mode==4)
                do_set_zd(board, atoi(parm_name[i] + 3), FILE_NOREPLY_FLAG);
            if (mode == 5)
                do_del_zd(board, atoi(parm_name[i] + 3));
			if (mode == 6)
				do_import(board,atoi(parm_name[i]+3));
        }
    }
    printf("</table>");
    if (total <= 0)
        printf("����ѡ������<br>\n");
    printf("<br><a href=\"/bbsmdoc.php?board=%s\">���ع���ģʽ</a>", encode_url(buf, board, sizeof(buf)));
    http_quit();
}

/* modified by stiger,20030414 */
int do_del(char *board, int id)
{
    int fd;
    int ent;
    char dir[256];
    struct fileheader f;

    sprintf(dir, "boards/%s/.DIR", board);
    fd = open(dir, O_RDWR, 0644);
    if (fd < 0)
        http_fatal("����Ĳ���");
    if( get_records_from_id( fd, id, &f, 1, &ent) ){
	close(fd);
        switch (del_post(ent, &f, dir, board)) {
        case 0:
            printf("<tr><td>%s  </td><td>����:%s </td><td>ɾ���ɹ�.</td></tr>\n", f.owner, nohtml(f.title));
            break;
        default:
            http_fatal("����Ȩɾ������");
	}
	return;
    }
	close(fd);
    printf("<tr><td></td><td></td><td>�ļ�������.</td></tr>\n");
}

int bbsman_import(int ent,char *board,struct fileheader *f,char *dirdir)
{
	MENU pm;
	char buf[PATHLEN],bname[PATHLEN];
	char fname[STRLEN];
	int ret;
	int i;

	bzero(&pm, sizeof(pm));
	pm.path = im_path;
	a_loadnames(&pm);

	ann_get_postfilename(fname,f,&pm);
	sprintf(bname,"%s/%s",pm.path,fname);
	sprintf(buf,"%-38.38s %s",f->title,currentuser->userid);
	a_additem(&pm,buf,fname,NULL,0,f->attachment);
	if(a_savenames(&pm) == 0){
		sprintf(buf,"boards/%s/%s",board,f->filename);
		f_cp(buf,bname,0);
		/* change flag */
		ret = 1;
	}else 
		ret = 0;

	for(i=0;i<pm.num;i++)
		free(pm.item[i]);

	if(ret){
        ret=change_flag(f,board,dirdir,ent,FILE_IMPORT_FLAG);
	}

	return ret;
}


int do_import(char *board, int id)
{
	struct fileheader f;
	int ent;
	int fd;
	char dir[256];

	sprintf(dir,"boards/%s/.DIR",board);
	fd = open(dir,O_RDWR,0644);
	if(fd < 0)
		http_fatal("���ļ�����");
	if(get_records_from_id(fd,id,&f,1,&ent)){
		close(fd);
		if(bbsman_import(ent,board,&f,dir))
            printf("<tr><td>%s  </td><td>����:%s </td><td>��¼�ɹ�.</td></tr>\n", f.owner, nohtml(f.title));
		else
            printf("<tr><td>%s  </td><td>����:%s </td><td>��¼���ɹ�.</td></tr>\n", f.owner, nohtml(f.title));
	}
	else{
		close(fd);
    	printf("<tr><td></td><td></td><td>�ļ�������.</td></tr>\n");
	}

}

int do_del_zd(char *board, int id)
{
    FILE *fp;
	int ffind=0;
    int ent=1;
    char dir[256];
    struct fileheader f;

    sprintf(dir, "boards/%s/" DING_DIR, board);

	fp = fopen(dir, "r+");
    if (fp == 0) 
        http_fatal("����Ĳ���"); 
    while (1) {
	    if (fread(&f, sizeof(struct fileheader), 1, fp) <= 0)
		    break;               
	    if (f.id==id) {
		    ffind=1;
		    break;
	    }
	    ent++;
    }
    fclose(fp);

    if( ffind ){
        switch (del_post(ent, &f, dir, board)) {
        case 0:
            printf("<tr><td>%s  </td><td>����:%s </td><td>ɾ���ɹ�.</td></tr>\n", f.owner, nohtml(f.title));
            break;
        default:
            http_fatal("����Ȩɾ������");
	}
	return;
    }

    printf("<tr><td></td><td></td><td>�ļ�������.</td></tr>\n");
}

/* �� G ʱ��û�� post ����ժ�� */
/* modified by stiger,20030414 */
int do_set(char *board, int id, int flag)
{
    int fd;
    char dir[256];
    struct fileheader f;
    int ent;

    setbdir(DIR_MODE_NORMAL, dir, board);
    fd = open(dir, O_RDWR, 0644);
    if( fd >= 0 && get_records_from_id( fd, id, &f, 1, &ent) )
	{
        close(fd);
		if(change_flag(&f,board,dir,ent,flag)==0)
			printf("<tr><td>%s</td><td>����:%s</td><td>��ǳɹ�.</td></tr>\n", f.owner, nohtml(f.title));
		else
			printf("<tr><td>%s</td><td>����:%s</td><td>��ǲ��ɹ�.</td></tr>\n", f.owner, nohtml(f.title));
    }else{
        close(fd);
        printf("<tr><td></td><td></td><td></td><td>�ļ�������.</td></tr>\n");
    }
    
}


int do_set_zd(char *board, int id, int flag)
{
    FILE *fp;
    char dir[256];
    struct fileheader f;
    int ent=1;
    int ffind=0;

    setbdir(DIR_MODE_NORMAL,dir,board);
    
	fp = fopen(dir, "r+");
    if (fp == 0) 
        http_fatal("����Ĳ���"); 
    while (1) {
	    if (fread(&f, sizeof(struct fileheader), 1, fp) <= 0)
		    break;               
	    if (f.id==id) {
		    ffind=1;
		    break;
	    }
	    ent++;
    }
    fclose(fp);

	if(ffind){
		if(change_flag(&f,board,dir,ent,flag)==0)
			printf("<tr><td>%s</td><td>����:%s</td><td>��ǳɹ�.</td></tr>\n", f.owner, nohtml(f.title));
		else
			printf("<tr><td>%s</td><td>����:%s</td><td>��ǲ��ɹ�.</td></tr>\n", f.owner, nohtml(f.title));
    }else{
        printf("<tr><td></td><td></td><td></td><td>�ļ�������.</td></tr>\n");
    }
}
