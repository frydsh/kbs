#include "bbslib.h"

char genbuf[1024];
char quote_title[120],quote_board[120];
char quote_file[120], quote_user[120];
char save_title[STRLEN];
char save_filename[4096];

/* Add by SmallPig */
/* ��quote_file���Ƶ�filepath (ת�����Զ�����)*/
void getcross2(char *filepath, char *board, struct userec *user)
{
    FILE        *inf, *of;
    char        buf[256];
    char        owner[256];
    int                     count;
    time_t      now;
	int normal_file;         
	int header_count;        

    now=time(0);
    inf=fopen(quote_file,"r");
    of = fopen( filepath, "w" );
    if(inf==NULL || of ==NULL)
    {
        if(NULL != inf) fclose(inf);
        if(NULL != of ) fclose(of) ;

        report("Cross Post error");
        return ;
    }
	normal_file=1;           

	write_header2(of, board, save_title, user->userid, user->username, 0);
	if(fgets( buf, 256, inf ) != NULL)
	{
		for(count=8;buf[count]!=' ';count++)
			owner[count-8]=buf[count];
	}
	owner[count-8]='\0';
	fprintf( of, "�� ��������ת���� %s ������ ��\n",quote_board);
	if (id_invalid(owner))
		normal_file=0;
	if (normal_file)
	{
		for (header_count=0;header_count<3;header_count++)
		{
			if ( fgets( buf, 256, inf ) == NULL)
				break;/*Clear Post header*/ 
		}
		if ((header_count!=2)||(buf[0]!='\n'))
			normal_file=0;
	}                                                          
	if (normal_file)                                           
		fprintf( of, "�� ԭ���� %s ������ ��\n",owner);        
	else                                                       
		fseek(inf,0,SEEK_SET);                                 

    while( fgets( buf, 256, inf ) != NULL)
    {
        if((strstr(buf,"�� ��������ת���� ")&&strstr(buf,"������ ��"))
			||(strstr(buf,"�� ԭ����")&&strstr(buf,"������ ��")))
            continue; /* ���������ظ� */
        else
            fprintf( of, "%s", buf );
    }
    fclose( inf );
    fclose( of);
    *quote_file = '\0';
}

/* Add by SmallPig */
int post_cross2(char islocal, char *board)
{
    struct fileheader postfile ;
    char        filepath[STRLEN], fname[STRLEN];
    char        buf[256],buf4[STRLEN],whopost[IDLEN];
    int         fp,i;
    time_t          now;
	int local_article;

    if (!haspostperm(currentuser,board))
    {
		printf("no post perm.<br>\n");
        return -1;
    }

    memset(&postfile,0,sizeof(postfile));
	// ����Ƚ����, fname[] �����ݲ���ȷ��
    strncpy(save_filename,fname,4096);

    now=time(0);
    sprintf(fname,"M.%d.A",now) ;
	if(!strstr(quote_title,"(ת��)"))
		sprintf(buf4,"%s (ת��)",quote_title);
	else
		strcpy(buf4,quote_title);
	// ����һ���� save_title ������
    strncpy(save_title,buf4,STRLEN) ;

    setbfile( filepath, board, fname ); /* �õ� Ŀ��POST�ļ��� */

    i = 0;
    while((fp = open(filepath,O_CREAT|O_EXCL|O_WRONLY,0644)) == -1)
	{
        now++;
        sprintf(fname,"M.%d.A",now) ; /*great idea! */
        setbfile(filepath, board, fname);
        if(i > 10)
            break;
        i++;
    }
    close(fp) ;

    strcpy(postfile.filename,fname) ;

    strcpy(whopost, getcurruserid());

    strncpy(postfile.owner,whopost,STRLEN) ;
    setbfile( filepath, board, postfile.filename );

    local_article = 0;
    if ( !strcmp( postfile.title, buf ) && quote_file[0] != '\0' )
        if(islocal=='l'||islocal=='L')
            local_article=YEA;

    //modify_user_mode( POSTING );
    getcross2(filepath, board, getcurrusr()); /*����fname��� �ļ����� */

    strncpy( postfile.title, save_title, STRLEN );
    if ( local_article == 1 ) /* local save */
    {
        postfile.filename[ STRLEN - 1 ] = 'L';
        postfile.filename[ STRLEN - 2 ] = 'L';
    }
	else
    {
		struct userec *user = getcurrusr();
        postfile.filename[ STRLEN - 1 ] = 'S';
        postfile.filename[ STRLEN - 2 ] = 'S';
        outgo_post2(&postfile, board, user->userid,
				user->username, save_title);
    }
    sprintf( buf, "boards/%s/%s", board, DOT_DIR);
    if (!strcmp(board, "syssecurity")
            && strstr(quote_title, "�޸� ")
            && strstr(quote_title, " ��Ȩ��"))
        postfile.accessed[0] |= FILE_MARKED; /* Leeward 98.03.29 */
    if (append_record( buf, &postfile, sizeof(postfile)) == -1)
	{ /* ���POST��Ϣ����ǰ��.DIR */
		sprintf(buf, "cross_posting '%s' on '%s': append_record failed!",
				postfile.title, quote_board);
        report(buf);
        return 1 ;
    }
    /* brc_add_read( postfile.filename ) ;*/
    //if(!mode)       /* �û�post�����Զ�����*/
	sprintf(buf,"cross_posted '%s' on '%s'", postfile.title, board) ;
    report(buf) ;
    return 1;
}

// ent		1-based
// board    source board
// board2   dest board
int do_cross(int ent, struct fileheader *fileinfo, char *direct,
			char *board, char *board2, int local_save)
{
    char bname[STRLEN];
    char dbname[STRLEN];
	char local;

    if (!HAS_PERM(currentuser,PERM_POST)) /* �ж��Ƿ���POSTȨ */
    {
		return DONOTHING;
	}

    if ((fileinfo->accessed[0] & FILE_FORWARDED) && !HAS_PERM(currentuser,PERM_SYSOP))
    {
        http_fatal("�������Ѿ�ת����һ�Σ��޷��ٴ�ת��");
    }

	sprintf(genbuf,"boards/%s/%s",board,fileinfo->filename) ;
    strcpy( quote_file, genbuf );
    strcpy(quote_title,fileinfo->title);

    if( !strcmp(board2,board))
    {
        http_fatal("\n\n                          ��������²���Ҫת��������!");
    }

	if(deny_me(currentuser->userid,board2)&&!HAS_PERM(currentuser,PERM_SYSOP))     /* ������ֹPOST ��� */
	{
		http_fatal("\n\n                �ܱ�Ǹ�����ڸð汻�����ֹͣ�� POST ��Ȩ��...\n");
	}
	else if (YEA == checkreadonly(board2)) /* Leeward 98.03.28 */
	{
		return FULLUPDATE;
	}

	local = local_save ? 'l' : 's';
	strcpy(quote_board,board);
	if(post_cross2(local, board2)==-1) /* ת�� */
	{
		http_fatal("ת������ʧ��.\n");
	}
	fileinfo->accessed[0] |= FILE_FORWARDED;  /*added by alex, 96.10.3 */
	substitute_record(direct, fileinfo, sizeof(*fileinfo),ent) ;
    return FULLUPDATE;
}


int do_ccc(int ent, struct fileheader *x, char *dir,
		char *board, char *board2, int local)
{
	FILE *fp;//, *fp2;
	//char title[512], buf[512], path[200], path2[200], i;
	char path[200];

	sprintf(path, "boards/%s/%s", board, x->filename);
	fp=fopen(path, "r");
	if(fp==0)
		http_fatal("�ļ������Ѷ�ʧ, �޷�ת��");
	fclose(fp);
	do_cross(ent, x, dir, board, board2, local);
	printf("'%s' ��ת���� %s ��.<br>\n", nohtml(x->title), board2);
	printf("[<a href=\"javascript:history.go(-2)\">����</a>]");
}

int main()
{
	struct fileheader f;
	char board[80], dir[80], file[80], target[80];
	FILE *fp;
	int found = 0;
	int num = 0;
	int local;

	init_all();
	strsncpy(board, getparm("board"), 30);
	strsncpy(file, getparm("file"), 30);
	strsncpy(target, getparm("target"), 30);
	local = atoi(getparm("outgo")) ? 0 : 1;
	if(!loginok)
		http_fatal("�Ҵҹ��Ͳ��ܽ��б������");
	if(!has_read_perm(currentuser, board))
		http_fatal("�����������");
	sprintf(dir, "boards/%s/.DIR", board);
	fp=fopen(dir, "r");
	if(fp==0)
		http_fatal("����Ĳ���");
	while(1)
	{
		if(fread(&f, sizeof(struct fileheader), 1, fp)<=0)
			break;
		if(!strcmp(f.filename, file))
		{
			found = 1;
			break;
		}
		num++;
	}
	fclose(fp);
	if (found == 0)
		http_fatal("����Ĳ���");
	printf("<center>%s -- ת������ [ʹ����: %s]<hr color=\"green\">\n",
			BBSNAME, currentuser->userid);
	if(target[0])
	{
		if(!haspostperm(currentuser, target))
			http_fatal("��������������ƻ���û���ڸð淢�ĵ�Ȩ��");
		return do_ccc(num+1, &f, dir, board, target, local);
	}
	printf("<table><tr><td>\n");
	printf("<font color=\"red\">ת������ע������:<br>\n");
	printf("��վ�涨ͬ�����ݵ������Ͻ��� 5 ���� 5 ���������������ظ�����<br>\n");
	printf("Υ�߽�������ڱ�վ���ĵ�Ȩ��.<br><br></font>\n");
	printf("���±���: %s<br>\n", nohtml(f.title));
	printf("��������: %s<br>\n", f.owner);
	printf("ԭ������: %s<br>\n", board);
	printf("<form action=\"bbsccc\" method=\"post\">\n");
	printf("<input type=\"hidden\" name=\"board\" value=\"%s\">", board);
	printf("<input type=\"hidden\" name=\"file\" value=\"%s\">", file);
	printf("ת�ص� <input name=\"target\" size=\"30\" maxlength=\"30\"> ������.<br>\n ");
	printf("<input type=\"checkbox\" name=\"outgo\" value=\"1\">ת��<br>\n");
	printf("<input type=\"submit\" value=\"ȷ��\"></form></td></tr></table>");
	http_quit();
	return 0;
}

