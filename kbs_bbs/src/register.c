/*
    Pirate Bulletin Board System
    Copyright (C) 1990, Edward Luke, lush@Athena.EE.MsState.EDU
    Eagles Bulletin Board System
    Copyright (C) 1992, Raymond Rocker, rocker@rock.b11.ingr.com
                        Guy Vega, gtvega@seabass.st.usm.edu
                        Dominic Tynes, dbtynes@seabass.st.usm.edu

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 1, or (at your option)
    any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
*/

#include "bbs.h"

#define  EMAIL          0x0001 
#define  NICK           0x0002 
#define  REALNAME       0x0004 
#define  ADDR           0x0008
#define  REALEMAIL      0x0010
#define  BADEMAIL       0x0020
#define  NEWREG         0x0040

char    *sysconf_str();
char    *Ctime();

extern time_t   login_start_time;
time_t          system_time;
extern int convcode;  /* KCN,99.09.05 */
extern int switch_code(); /* KCN,99.09.05 */

void
new_register()
{
    struct userec       newuser;
    int    allocid, do_try,flag,lockfd;
    char   buf[STRLEN];
/* temp !!!!!*/
/*    prints("Sorry, we don't accept newusers due to system problem, we'll fixit ASAP\n");
    oflush();
    sleep(2);
    exit(-1);
*/
    memset( &newuser, 0, sizeof(newuser) );
    getdata(0, 0, "ʹ��GB�����Ķ�?(\xa8\xcf\xa5\xce BIG5\xbd\x58\xbe\x5c\xc5\xaa\xbd\xd0\xbf\xefN)(Y/N)? [Y]: ", buf, 4, DOECHO, NULL, YEA);
    if (*buf == 'n' || *buf == 'N')
        if (!convcode)
            switch_code();

    ansimore("etc/register", NA);
    do_try = 0;
    while( 1 ) {
        if( ++do_try >= 10 ) {
            prints("\n��������̫����  <Enter> ��...\n");
            refresh();
            longjmp( byebye, -1 );
        }
        getdata(0,0,"���������: ",newuser.userid,IDLEN+1,DOECHO,NULL,YEA);
        flag = 1;
        if(id_invalid(newuser.userid)==1)
        {
            prints("�ʺű�����Ӣ����ĸ��������ɣ����ҵ�һ���ַ�������Ӣ����ĸ!\n");
            /*                prints("�ʺű�����Ӣ����ĸ�����֣������ʺŵ�һ������Ӣ����ĸ!\n");*/
            flag=0;
        }
        if (flag) {
	    if(strlen(newuser.userid) < 2) {
                prints("����������������Ӣ����ĸ!\n");
            } else if ( (*newuser.userid == '\0') || bad_user_id( newuser.userid )){
                prints( "ϵͳ���ֻ��ǲ��ŵĴ��š�\n" );
            } else if((usernum = searchuser( newuser.userid )) != 0) /*( dosearchuser( newuser.userid ) ) midified by dong , 1998.12.2, change getuser -> searchuser , 1999.10.26*/
	    {
                prints("���ʺ��Ѿ�����ʹ��\n") ;
            } else {
		/*---	---*/
	      	struct stat lst;
		time_t lnow;
		lnow = time(NULL);
		sethomepath( genbuf, newuser.userid );
		if(!stat(genbuf, &lst) && S_ISDIR(lst.st_mode)
			&& (lnow - lst.st_ctime < SEC_DELETED_OLDHOME /* 3600*24*30 */) ) {
			prints("Ŀǰ�޷�ע���ʺ�%s������ϵͳ������Ա��ϵ��\n", newuser.userid);
			sprintf(genbuf, "IP %s new id %s failed[home changed in past 30 days]",
					fromhost, newuser.userid);
			report(genbuf);
		} else
		/*---	---*/
		break;
            }
	}
    }

    newuser.firstlogin = newuser.lastlogin = time(NULL) - 13 * 60 * 24 ;
    while( 1 ) {
        char  passbuf[ STRLEN ], passbuf2[ STRLEN ];
        getdata(0,0,"���趨��������: ",passbuf,39,NOECHO,NULL,YEA) ;
        if( strlen( passbuf ) < 4 || !strcmp( passbuf, newuser.userid ) ) {
            prints("����̫�̻���ʹ���ߴ�����ͬ, ����������\n") ;
            continue;
        }
        getdata(0,0,"��������һ���������: ",passbuf2,39,NOECHO,NULL,YEA);
        if( strcmp( passbuf, passbuf2) != 0 ) {
            prints("�����������, ��������������.\n") ;
            continue;
        }
        
        setpasswd(passbuf,&newuser);
        break;
    }
    newuser.userlevel = PERM_BASIC;
    newuser.userdefine=-1;
/*   newuser.userdefine&=~DEF_MAILMSG;
    newuser.userdefine&=~DEF_EDITMSG; */
    newuser.userdefine &= ~DEF_NOTMSGFRIEND;
    if (convcode)
        newuser.userdefine&=~DEF_USEGB;

    newuser.notemode=-1;
    newuser.unuse1=-1;
    newuser.unuse2=-1;
    newuser.flags[0] = CURSOR_FLAG;
    newuser.flags[0] |= PAGER_FLAG;
    newuser.flags[1] = 0;
    newuser.firstlogin = newuser.lastlogin = time(NULL) ;

    allocid = getnewuserid2(newuser.userid)  ;
    if(allocid > MAXUSERS || allocid <= 0) {
        prints("No space for new users on the system!\n\r") ;
        oflush();
	    exit(1) ;
    }

	update_user(&newuser,allocid,1);

    if( !dosearchuser(newuser.userid) ) {
        /* change by KCN 1999.09.08
                fprintf(stderr,"User failed to create\n") ;
        */
        prints("User failed to create %d-%s\n",allocid,newuser.userid) ;
        oflush();
        exit(1) ;
    }
    report( "new account" );
}

int
invalid_realmail( userid, email, msize )
char    *userid, *email;
int     msize;
{
    FILE        *fn;
    char        *emailfile, ans[4],fname[STRLEN];

    if( (emailfile = sysconf_str( "EMAILFILE" )) == NULL )
        return 0;

    if( strchr( email, '@' ) && valid_ident( email ) )
        return 0;
    /*
        ansimore( emailfile, NA );
        getdata(t_lines-1,0,"��Ҫ���� email-post ��? (Y/N) [Y]: ",
            ans,2,DOECHO,NULL,YEA);
        while( *ans != 'n' && *ans != 'N' ) {
    */
    sprintf( fname, "tmp/email/%s", userid );
    if( (fn = fopen( fname, "r" )) != NULL ) {
        fgets( genbuf, STRLEN, fn );
        fclose( fn );
        strtok( genbuf, "\n" );
        if (!valid_ident( genbuf )) {
        } else if( strchr( genbuf, '@' ) != NULL ) {
            unlink(fname);
            strncpy( email, genbuf, msize );
            move( 10, 0 );
            prints( "������!! ����ͨ�������֤, ��Ϊ��վ����. \n" );
            prints( "         ��վΪ�����ṩ�Ķ������, \n" );
            prints( "         ����Mail,Post,Message,Talk ��. \n" );
            prints( "  \n" );
            prints( "������,  ���Ĵ����һ��, \n" );
            prints( "         �����ĵط�, ���� sysop ������, \n" );
            prints( "         ��վ����ר��Ϊ�����. \n" );
            getdata( 18 ,0, "�밴 <Enter>  <<  ", ans,2,DOECHO,NULL ,YEA);
            return 0;
        }
    }
    return 1;
}

void
check_register_info()
{
    char        *newregfile;
    int         perm;
    time_t      code;
    FILE        *fin, *fout,*dp;
    char        ans[4],buf[STRLEN];
    extern int showansi;

    clear();
    sprintf(buf,"%s",email_domain());
    if( !(currentuser->userlevel & PERM_BASIC) ) {
        currentuser->userlevel = 0;
        return;
    }
    /*urec->userlevel |= PERM_DEFAULT;*/
    perm = PERM_DEFAULT & sysconf_eval( "AUTOSET_PERM" );

    /*    if( sysconf_str( "IDENTFILE" ) != NULL ) {  commented out by netty to save time */
    while ( strlen( currentuser->username ) < 2 ) {
        getdata( 2, 0, "�����������ǳ�:(����," DEFAULT_NICK ") << ", buf, NAMELEN,DOECHO,NULL ,YEA);
        strcpy(currentuser->username,buf);
        strcpy(uinfo.username,buf);
        UPDATE_UTMP_STR(username,uinfo);
    }
    if ( strlen( currentuser->realname ) < 2 ) {
        move( 3, 0 );
        prints( "������������ʵ����: (վ����������ܵ� !)\n" );
        getdata( 4, 0, "> ", buf, NAMELEN,DOECHO,NULL,YEA);
        strcpy(currentuser->realname,buf);
    }
    if ( strlen( currentuser->address ) < 6 ) {
        move( 5, 0 );
        prints( "��Ŀǰ��д�ĵ�ַ�ǡ�%s��������С�� [1m[37m6[m��ϵͳ��Ϊ����ڼ�̡�\n", currentuser->address[0] ? currentuser->address : "�յ�ַ" ); /* Leeward 98.04.26 */
        getdata( 6, 0, "����ϸ��д����סַ��", buf, NAMELEN,DOECHO,NULL,YEA);
        strcpy(currentuser->address,buf);
    }
    if ( strchr( currentuser->email, '@' ) == NULL ) {
        move( 3, 0 );
        prints( "ֻ�б�վ�ĺϷ�������ܹ���ȫ���и��ֹ��ܣ� \n" );
        /* alex           prints( "��Ϊ��վ�Ϸ����������ְ취��\n\n" );
                    prints( "1. ������кϷ���email����(��BBS), \n");
                                prints( "       ������û���֤�ŵķ�ʽ��ͨ����֤�� \n\n" );
                    prints( "2. �����û��email����(��BBS)��������ڽ��뱾վ�Ժ�\n" );
                    prints( "       ��'���˹�����'�� ��ϸע����ʵ��ݣ�\n" );
                    prints( "       SYSOPs �ᾡ�� ��鲢ȷ�����ע�ᵥ��\n" );
                    move( 17, 0 );
                    prints( "���������ʽΪ: xxx@xxx.xxx.edu.cn \n" );
                    getdata( 18, 0, "�������������: (�����ṩ�߰� <Enter>) << "
                                    , urec->email, STRLEN,DOECHO,NULL,YEA);
                    if ((strchr( urec->email, '@' ) == NULL )) { 
                        sprintf( genbuf, "%s.bbs@%s", urec->userid,buf );
                        strncpy( urec->email, genbuf, STRLEN);
                    }
        alex, ��Ϊȡ����email���� , 97.7 */
        prints( "��Ϊ"NAME_BBS_NICK"�Ϸ�"NAME_USER_SHORT"�ķ������£�\n\n" );
        prints( "����ʺŵĵ�һ�ε�¼��� "REGISTER_WAIT_TIME_NAME"�ڣ�[1m[33m����ָ�� BBS "REGISTER_WAIT_TIME_NAME"[m����\n");
        prints( "    �㴦��������·�ڼ�, ���Ĵ��ι�ѧϰ����ʹ�÷����͸������ǡ�\n");
        prints( "    �ڴ��ڼ䣬����ע���Ϊ�Ϸ�"NAME_USER_SHORT"��\n\n");
        prints( "�����⿪ʼ�� "REGISTER_WAIT_TIME_NAME", ��Ϳ����ڽ���"NAME_BBS_NICK"�Ժ�\n" );
        prints( "    �ڡ����˹����䡯����ϸע����ʵ��ݣ�\n" );
        prints( "    "NAME_SYSOP_GROUP"�ᾡ���鲢ȷ�����ע�ᵥ��\n\n" );
        /* Leeward adds below 98.04.26 */
        prints( "[1m[33m������Ѿ�ͨ����ע�ᣬ��Ϊ�˺Ϸ�"NAME_USER_SHORT"��ȴ��Ȼ���ǿ����˱���Ϣ��\n������Ϊ��û���ڡ����˹����䡯���趨�������ʼ����䡯��[m\n��ӡ���ѡ�������롮���˹����䡯�ڣ��ٽ��롮�趨�������ϡ�һ������趨��\n�����ʵ��û���κο��õġ������ʼ����䡯�����趨���ֲ�Ը�⿴������Ϣ��\n����ʹ�� [1m[33m%s.bbs@smth.org[m �����趨��\nע�����������ĵ����ʼ����䲻�ܽ��յ����ʼ�������������ʹϵͳ������ʾ����Ϣ��", currentuser->userid);
        pressreturn();
    }
    if(!strcmp(currentuser->userid,"SYSOP"))
    {
        currentuser->userlevel=~0;
        currentuser->userlevel&=~PERM_SUICIDE; /* Leeward 98.10.13 */
        currentuser->userlevel&=~PERM_DENYMAIL; /* Bigman 2000.9.22 */
    }
    if(!(currentuser->userlevel&PERM_LOGINOK))
    {
        if( HAS_PERM(currentuser, PERM_SYSOP ))
            return;
        if(!invalid_realmail( currentuser->userid, currentuser->realemail, STRLEN-16 ))
        {
            currentuser->userlevel |= PERM_DEFAULT;
            if( HAS_PERM(currentuser, PERM_DENYPOST ) && !HAS_PERM(currentuser, PERM_SYSOP ) )
                currentuser->userlevel &= ~PERM_POST;
        }else {
            /* added by netty to automatically send a mail to new user. */
            /* begin of check if local email-addr  */
            /*       if (
                       (!strstr( urec->email, "@bbs.") ) &&
                       (!strstr( urec->email, ".bbs@") )&&
                       (!invalidaddr(urec->email))&&
                       sysconf_str( "EMAILFILE" )!=NULL) 
                   {
                       move( 15, 0 );
                       prints( "���ĵ�������  ����ͨ��������֤...  \n" );
                       prints( "      SYSOP ����һ����֤�Ÿ���,\n" );
                       prints( "      ��ֻҪ����, �Ϳ��Գ�Ϊ��վ�ϸ���.\n" );
                       getdata( 19 ,0, "��Ҫ SYSOP ����һ������?(Y/N) [Y] << ", ans,2,DOECHO,NULL,YEA);
                       if ( *ans != 'n' && *ans != 'N' ) {
                       code=(time(0)/2)+(rand()/10);
                       sethomefile(genbuf,urec->userid,"mailcheck");
                       if((dp=fopen(genbuf,"w"))==NULL)
                       {
                            fclose(dp);
                            return;
                       }
                       fprintf(dp,"%9.9d\n",code);
                       fclose(dp);
                       sprintf( genbuf, "/usr/lib/sendmail -f SYSOP.bbs@%s %s ", 
                             email_domain(), urec->email );
                       fout = popen( genbuf, "w" );
                       fin  = fopen( sysconf_str( "EMAILFILE" ), "r" );
                       if ((fin != NULL) && (fout != NULL)) {
                           fprintf( fout, "Reply-To: SYSOP.bbs@%s\n", email_domain());
                           fprintf( fout, "From: SYSOP.bbs@%s\n",  email_domain() ); 
                           fprintf( fout, "To: %s\n", urec->email);
                           fprintf( fout, "Subject: @%s@[-%9.9d-]firebird mail check.\n", urec->userid ,code);
                           fprintf( fout, "X-Forwarded-By: SYSOP \n" );
                           fprintf( fout, "X-Disclaimer: None\n");
                           fprintf( fout, "\n");
                           fprintf(fout,"���Ļ����������£�\n",urec->userid);
                           fprintf(fout,"ʹ���ߴ��ţ�%s (%s)\n",urec->userid,urec->username);
                           fprintf(fout,"��      ����%s\n",urec->realname);
                           fprintf(fout,"��վλ��  ��%s\n",urec->lasthost);
                           fprintf(fout,"�����ʼ�  ��%s\n\n",urec->email);
                           fprintf(fout,"�װ��� %s(%s):\n",urec->userid,urec->username);
                           while (fgets( genbuf, 255, fin ) != NULL ) {
                               if (genbuf[0] == '.' && genbuf[ 1 ] == '\n')
                                    fputs( ". \n", fout );
                               else fputs( genbuf, fout );
                           }
                           fprintf(fout, ".\n");                                    
                           fclose( fin );
                           fclose( fout );                                     
                       }
                       getdata( 20 ,0, "���Ѽĳ�, SYSOP ����������Ŷ!! �밴 <Enter> << ", ans,2,DOECHO,NULL ,YEA);
                       }
                   }else
                   {
                    showansi=1;
                    if(sysconf_str( "EMAILFILE" )!=NULL)
                    {
                      prints("\n��ĵ����ʼ���ַ ��[33m%s[m��\n",urec->email);
                      prints("���� Unix �ʺţ�ϵͳ����Ͷ�����ȷ���ţ��뵽[32m������[m���޸�..\n");
                      pressanykey();
                    }
                   }
            deleted by alex, remove email certify */

            clear(); /* Leeward 98.05.14 */
            move( 12, 0 );
            prints( "�㻹û��ͨ�������֤������û��talk,mail,message,postȨ...  \n" );
            prints( "�����Ҫ��Ϊ"NAME_BBS_NICK"��ע��"NAME_USER_SHORT"��\n\n");
            prints( "����[31m���˹�����[0m��[31m��ϸע�����[0m\n" );
            prints( "��ֻҪȷʵ��ϸ��д�������ʵ�������, \n");
            prints( "��"NAME_SYSOP_GROUP"�����ֹ���֤���Ժ󣬾Ϳ��Գ�Ϊ��վ�ϸ�"NAME_USER_SHORT".\n" );
            pressreturn();
        }
        /* end of check if local email-addr */
        /*  above lines added by netty...  */
    }
    newregfile = sysconf_str( "NEWREGFILE" );
    if( currentuser->lastlogin - currentuser->firstlogin < REGISTER_WAIT_TIME &&
            !HAS_PERM(currentuser, PERM_SYSOP) && newregfile != NULL ) {
        currentuser->userlevel &= ~(perm);
        ansimore( newregfile, YEA );
    }
    if( HAS_PERM(currentuser, PERM_DENYPOST ) && !HAS_PERM(currentuser, PERM_SYSOP ) )
    {
        currentuser->userlevel &= ~PERM_POST;
    }
}

