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
#include <sys/param.h>
#include <sys/ipc.h>
#include <sys/shm.h>

extern char *getenv();
extern char fromhost[];


#ifdef BBSMAIN
void
printdash( mesg )       
char    *mesg;          
{
    char        buf[ 80 ], *ptr;
    int         len;
    
    memset( buf, '=', 79 );
    buf[ 79 ] = '\0';
    if( mesg != NULL ) {
        len = strlen( mesg ); 
        if( len > 76 )  len = 76;
        ptr = &buf[ 40 - len / 2 ];
        ptr[ -1  ] = ' ';
        ptr[ len ] = ' ';
        strncpy( ptr, mesg, len );
    }
    prints( "%s\n", buf );
}

void
bell()
{
    /* change by KCN 1999.09.08    fprintf(stderr,"%c",Ctrl('G')) ;*/
    char sound;

    sound= Ctrl('G');
    output( &sound, 1);

}   

int
pressreturn()
           {
               extern int showansi;
               char buf[3] ;

               showansi=1;
               move(t_lines-1,0);
               clrtoeol();
               getdata(t_lines-1,0,"                              \x1b[33m�밴 ��\x1b[36mEnter\x1b[33m�� ����\x1b[m",buf,2,NOECHO,NULL,YEA);
               move(t_lines-1,0) ;
               clrtoeol() ;
               refresh() ;
               return 0 ;
           }

           askyn(str,defa)
           char str[STRLEN];
int defa;
{
    int x,y;
    char realstr[STRLEN*2];
    char ans[6];

    sprintf(realstr,"%s (Y/N)? [%c]: ",str,(defa)?'Y':'N');
    getyx(&x,&y);
    getdata( x, y, realstr, ans,3,DOECHO,NULL,YEA);
    if(ans[0]!='Y' && ans[0]!='y' &&
            ans[0]!='N' && ans[0]!='n')
    {
        return defa;
    }else if(ans[0]=='Y' || ans[0]=='y')
        return 1;
    else if(ans[0]=='N' || ans[0]=='n')
        return 0;
}

int
pressanykey()
{
    extern int showansi;

    showansi=1;
    move( t_lines-1,0);
    clrtoeol();
    prints( "\x1b[m                                \x1b[5;1;33m���κμ����� ..\x1b[m" );
    egetch();
    move( t_lines-1, 0 );
    clrtoeol();
    return 0;
}

#endif


#include "modetype.c"

char *idle_str( struct user_info *uent )
{
    static char hh_mm_ss[ 32 ];
    struct stat buf;
    time_t      now, diff;
    int         hh, mm;

    now = time(0);
    diff = now - uent->freshtime;
    if (diff==now) /* @#$#!@$#@! */
        diff=0;
#ifdef DOTIMEOUT
    /* the 60 * 60 * 24 * 5 is to prevent fault /dev mount from
       kicking out all users */

    if ((diff > IDLE_TIMEOUT) && (diff < 60 * 60 * 24 * 5 ))
        kill( uent->pid, SIGHUP );
#endif

    hh = diff / 3600;
    mm = (diff / 60) % 60;

    if ( hh > 0 )
        sprintf( hh_mm_ss, "%d:%02d", hh, mm );
    else if ( mm > 0 )
        sprintf( hh_mm_ss, "%d", mm );
    else sprintf ( hh_mm_ss, "   ");


    return hh_mm_ss;
}

char *modestring(int mode,int towho,int complete,char *chatid)
{
    static char modestr[STRLEN];
    struct userec urec;

    /* Leeward: 97.12.18: Below removing ' characters for more display width */
    if (chatid) {
        if (complete) sprintf(modestr, "%s %s", ModeType(mode), chatid);
        else return (ModeType(mode));
        return (modestr);
    }
    if (mode != TALK && mode != PAGE && mode != QUERY)
        return (ModeType(mode));
    /*---	modified by period	2000-10-20	---*
        if (get_record(PASSFILE, &urec, sizeof(urec), towho) == -1)
            return (ModeType(mode));
    ---*/
    if (complete) {
	    if(getuserid(urec.userid, towho) != towho) return ModeType(mode);
        sprintf(modestr, "%s %s", ModeType(mode), urec.userid);
    }
    else
        return (ModeType(mode));
    return (modestr);
}

int
dashf( fname )
char *fname;
{
    struct stat st;

    return ( stat( fname, &st ) == 0 && S_ISREG( st.st_mode ) );
}

int
dashd( fname )
char    *fname;
{
    struct stat st;

    return ( stat( fname, &st ) == 0 && S_ISDIR( st.st_mode ) );
}

/* rrr - Snagged from pbbs 1.8 */

#define LOOKFIRST  (0)
#define LOOKLAST   (1)
#define QUOTEMODE  (2)
#define MAXCOMSZ (1024)
#define MAXARGS (40)
#define MAXENVS (20)
#define BINDIR "/bin/"

char *bbsenv[MAXENVS] ;
int numbbsenvs = 0 ;

/* Case Independent strncmp */

int
ci_strncmp(s1,s2,n)
register char *s1,*s2 ;
register int n ;
{
    char        c1, c2;

    while( n-- > 0 ) {
        c1 = *s1++;
        c2 = *s2++;
        if( c1 >= 'a' && c1 <= 'z' )
            c1 &= 0xdf;
        if( c2 >= 'a' && c2 <= 'z' )
            c2 &= 0xdf;
        if( c1 != c2 )
            return (c1 - c2);
        if( c1 == 0 )
            return 0;
    }
    return 0;
}

int
ci_strcmp( s1, s2 )
register char   *s1, *s2;
{
    char        c1, c2;

    while( 1 ) {
        c1 = *s1++;
        c2 = *s2++;
        if( c1 >= 'a' && c1 <= 'z' )
            c1 &= 0xdf;
        if( c2 >= 'a' && c2 <= 'z' )
            c2 &= 0xdf;
        if( c1 != c2 )
            return (c1 - c2);
        if( c1 == 0 )
            return 0;
    }
}

int
bbssetenv(env,val)
char *env, *val ;
{
    register int i,len ;

    if(numbbsenvs == 0)
        bbsenv[0] = NULL ;
    len = strlen(env) ;
    for(i=0;bbsenv[i];i++)
        if(!strncasecmp(env,bbsenv[i],len))
            break ;
    if(i>=MAXENVS)
        return -1 ;
    if(bbsenv[i])
        free(bbsenv[i]) ;
    else
        bbsenv[++numbbsenvs] = NULL ;
    bbsenv[i] = malloc(strlen(env)+strlen(val)+2) ;
    strcpy(bbsenv[i],env) ;
    strcat(bbsenv[i],"=") ;
    strcat(bbsenv[i],val) ;
    return 0;
}

#ifdef BBSMAIN
int
do_exec(com,wd)
char *com, *wd ;
{
    char path[MAXPATHLEN] ;
    char pcom[MAXCOMSZ] ;
    char *arglist[MAXARGS] ;
    char *tz;
    register int i,len ;
    register int argptr ;
    int status, pid, w ;
    int pmode ;
    void (*isig)(), (*qsig)() ;

    strncpy(path,BINDIR,MAXPATHLEN) ;
    strncpy(pcom,com,MAXCOMSZ) ;
    len = Min(strlen(com)+1,MAXCOMSZ) ;
    pmode = LOOKFIRST ;
    for(i=0,argptr=0;i<len;i++) {
        if(pcom[i] == '\0')
            break ;
        if(pmode == QUOTEMODE) {
            if(pcom[i] == '\001') {
                pmode = LOOKFIRST ;
                pcom[i] = '\0' ;
                continue ;
            }
            continue ;
        }
        if(pcom[i] == '\001') {
            pmode = QUOTEMODE ;
            arglist[argptr++] = &pcom[i+1] ;
            if(argptr+1 == MAXARGS)
                break ;
            continue ;
        }
        if(pmode == LOOKFIRST)
            if(pcom[i] != ' ') {
                arglist[argptr++] = &pcom[i] ;
                if(argptr+1 == MAXARGS)
                    break ;
                pmode = LOOKLAST ;
            } else continue ;
        if(pcom[i] == ' ') {
            pmode = LOOKFIRST ;
            pcom[i] = '\0' ;
        }
    }
    arglist[argptr] = NULL ;
    if(argptr == 0)
        return -1 ;
    if(*arglist[0] == '/')
        strncpy(path,arglist[0],MAXPATHLEN) ;
    else
        strncat(path,arglist[0],MAXPATHLEN) ;
    reset_tty() ;
    alarm(0) ;
#ifdef IRIX
    if((pid = fork()) == 0) {
#else
if((pid = vfork()) == 0) {
#endif
        if(wd)
            if(chdir(wd)) {
                /* change by KCN 1999.09.08
                              fprintf(stderr,"Unable to chdir to '%s'\n",wd) ;
                */
                prints("Unable to chdir to '%s'\n",wd) ;
                oflush();
                exit(-1) ;
            }
        bbssetenv("PATH", "/bin:.");
        bbssetenv("TERM", "vt100");
        bbssetenv("USER", currentuser->userid);
        bbssetenv("USERNAME", currentuser->username);
        /* added for tin's reply to */
        bbssetenv("REPLYTO", currentuser->email);
        bbssetenv("FROMHOST", fromhost);
        /* end of insertion */
        if ((tz = getenv("TZ")) != NULL)
            bbssetenv("TZ", tz);
        if(numbbsenvs == 0)
            bbsenv[0] = NULL ;
        execve(path,arglist,bbsenv) ;
        /* change by KCN 1999.09.08
                fprintf(stderr,"EXECV FAILED... path = '%s'\n",path) ;
        	*/
        prints("EXECV FAILED... path = '%s'\n",path) ;
        oflush();
        exit(-1) ;
    }
    isig = signal(SIGINT, SIG_IGN) ;
    qsig = signal(SIGQUIT, SIG_IGN) ;
    while((w = wait(&status)) != pid && w != 1)
        /* NULL STATEMENT */ ;
    signal(SIGINT, isig) ;
    signal(SIGQUIT, qsig) ;
    restore_tty() ;
#ifdef DOTIMEOUT
    alarm(IDLE_TIMEOUT) ;
#endif
    return((w == -1)? w: status) ;
}
#endif

#ifdef kill
#undef kill
#endif

int safe_kill(int x, int y)
{  if(x<=0) return -1;
   return kill(x,y);
}



char * Cdate(time_t * clock) /* ʱ�� --> Ӣ�� */
{
    /* Leeward 2000.01.01 Adjust year display for 20** */
    static char foo[24/*22*/];
    struct tm *mytm = localtime(clock);

    strftime(foo, 24/*22*/, "%Y-%m-%d %T %a"/*"%D %T %a"*/, mytm);
    return (foo);
}

char * Ctime(time_t * clock) /* ʱ�� ת�� �� Ӣ�� */
{
    char *foo;
    char *ptr = ctime(clock);

    if (foo = strchr(ptr, '\n')) *foo = '\0';
    return (ptr);
}

int Isspace(char ch)
{
    return (ch == ' ' || ch =='\t' || ch == 10 || ch == 13);
}


char * nextword(const char * * str, char *buf, int sz)
{
    const char *p;
    
    while (Isspace(**str))(*str)++;
    
    while (**str && !Isspace(**str)){
        if(sz>0){
            if(sz>1)  *buf++=**str;
            sz--;
        }
        (*str)++;
    }
    *buf=0;
    while (Isspace(**str))(*str)++;
    return buf;
}

#ifndef BBSMAIN
void
attach_err( shmkey, name )
int     shmkey;
char    *name;
{
    log( "3system", "Attach:Error! %s error! key = %x.\n", name, shmkey );
    exit( 1 );
}

void *attach_shm( char    *shmstr,int     defaultkey, int shmsize,int* iscreate)
{
    void        *shmptr;
    int         shmkey, shmid;

    shmkey = defaultkey;
    shmid = shmget( shmkey, shmsize, 0 );
    if( shmid < 0 ) {
        shmid = shmget( shmkey, shmsize, IPC_CREAT | 0600 );
        if( shmid < 0 )
            attach_err( shmkey, "shmget" );
        shmptr = (void *) shmat( shmid, NULL, 0 );
        if( shmptr == (void *)-1 )
            attach_err( shmkey, "shmat" );
        memset( shmptr, 0, shmsize );
        *iscreate=1;
    } else {
        shmptr = (void *) shmat( shmid, NULL, 0 );
        if( shmptr == (void *)-1 )
            attach_err( shmkey, "shmat" );
        *iscreate=0;
    }
    return shmptr;
}
#else
void attach_err( int     shmkey,char    *name)
{
    prints( "Error! %s error! key = %x.\n", name, shmkey );
    oflush();
    exit( 1 );
}

void *attach_shm( char    *shmstr,int     defaultkey, int shmsize,int* iscreate)
{
    void        *shmptr;
    int         shmkey, shmid;

    shmkey = sysconf_eval( shmstr );
    if( shmkey < 1024 )
        shmkey = defaultkey;
    shmid = shmget( shmkey, shmsize, 0 );
    if( shmid < 0 ) {
        shmid = shmget( shmkey, shmsize, IPC_CREAT | 0660 ); /* modified by dong , for web application , 1998.12.1 */
        *iscreate=YEA;
        if( shmid < 0 ) {
            attach_err( shmkey, "shmget" );
            exit(0);
        }
        shmptr = (void *) shmat( shmid, NULL, 0 );
        if( shmptr == (void *)-1 ) {
            attach_err( shmkey, "shmat" );
            exit(0);
    	}
        memset( shmptr, 0, shmsize );
    } else {
    	*iscreate=0;
        shmptr = (void *) shmat( shmid, NULL, 0 );
        if( shmptr == (void *)-1 ) {
            attach_err( shmkey, "shmat" );
            exit(0);
        }
    }
    return shmptr;
}
#endif

/*
char *
cexp(exp)
int exp;
{
        int expbase=0;

        if(exp==-9999)
                return "û�ȼ�";
        if(exp<=100+expbase)
                return "������·";
        if(exp>100+expbase&&exp<=450+expbase)
                return "һ��վ��";
        if(exp>450+expbase&&exp<=850+expbase)
                return "�м�վ��";
        if(exp>850+expbase&&exp<=1500+expbase)
                return "�߼�վ��";
        if(exp>1500+expbase&&exp<=2500+expbase)
                return "��վ��";
        if(exp>2500+expbase&&exp<=3000+expbase)
                return "���ϼ�";
        if(exp>3000+expbase&&exp<=5000+expbase)
                return "��վԪ��";
        if(exp>5000+expbase)
                return "��������";
        
}

char *
cperf(perf)
int perf;
{        
        
        if(perf==-9999)
                return "û�ȼ�";
        if(perf<=5)
                return "�Ͽ����";
        if(perf>5&&perf<=12)
                return "Ŭ����";
        if(perf>12&&perf<=35)
                return "������";
        if(perf>35&&perf<=50)
                return "�ܺ�";
        if(perf>50&&perf<=90)
                return "�ŵ���";
        if(perf>90&&perf<=140)
                return "̫������";
        if(perf>140&&perf<=200)
                return "��վ֧��";
        if(perf>200)
                return "�񡫡�";

}
*/
int countexp(struct userec *udata)
{
    int exp;

    if(!strcmp(udata->userid,"guest"))
        return -9999;
    exp=udata->numposts +/*post_in_tin( udata->userid )*/+udata->numlogins/5+(time(0)-udata->firstlogin)/86400+udata->stay/3600;
    return exp>0?exp:0;
}

int countperf(struct userec *udata)
{
    int perf;
    int reg_days;

    if(!strcmp(udata->userid,"guest"))
        return -9999;
    reg_days=(time(0)-udata->firstlogin)/86400+1;
    perf=((float)(udata->numposts/*+post_in_tin( udata->userid )*/)/(float)udata->numlogins+
          (float)udata->numlogins/(float)reg_days)*10;
    return perf>0?perf:0;
}


int compute_user_value( struct userec *urec)
{
    int         value;

    /* if (urec) has CHATCLOAK permission, don't kick it */
    if( (urec->userlevel & PERM_CHATCLOAK ) && (!(urec->userlevel & PERM_SUICIDE)))
        return 999;


    value = (time(0) - urec->lastlogin) / 60;    /* min */
    if (0 == value) value = 1; /* Leeward 98.03.30 */

    /* �޸�: �������ʺ�תΪ�����ʺ�, Bigman 2000.8.11 */
    if ((urec->userlevel & PERM_XEMPT) && (!(urec->userlevel & PERM_SUICIDE)) )
    {	if (urec->lastlogin < 988610030)
        return 666; /* ���û�е�¼���� */
        else
            return (667 * 24 * 60 - value)/(60*24);
    }
    /* new user should register in 30 mins */
    if( strcmp( urec->userid, "new" ) == 0 ) {
        return (30 - value) / 60; /* *->/ modified by dong, 1998.12.3 */
    }

    /* ��ɱ����,Luzi 1998.10.10 */
    if (urec->userlevel & PERM_SUICIDE)
        return (15 * 24 * 60 - value)/(60*24);
    /**********************/
    if(urec->numlogins <= 3)
        return (15 * 24 * 60 - value)/(60*24);
    if( !(urec->userlevel & PERM_LOGINOK) )
        return (30 * 24 * 60 - value)/(60*24);
    /* if (urec->userlevel & PERM_LONGID)
         return (667 * 24 * 60 - value)/(60*24); */
    return (366 * 24 * 60 - value)/(60*24);
}


char *
sethomefile( buf, userid, filename )  /*ȡĳ�û��ļ� ·��*/
char    *buf, *userid, *filename;
{
    if (isalpha(userid[0]))  /* ��������ж�,����ݴ���, alex 1997.1.6*/
        sprintf( buf, "home/%c/%s/%s", toupper(userid[0]), userid, filename );
    else
        sprintf( buf, "home/wrong/%s/%s", userid, filename);
    return buf;
}

char *
setuserfile( buf, filename )    /* ȡ��ǰ�û��ļ� ·��*/
char    *buf, *filename;
{
    if (isalpha(currentuser->userid[0]))  /* ��������ж�,����ݴ���, alex 1997.1.6*/
        sprintf( buf, "home/%c/%s/%s", toupper(currentuser->userid[0]), currentuser->userid, filename );
    else
        sprintf( buf, "home/wrong/%s/%s", currentuser->userid, filename);
    return buf;
}

char *
setmailfile( buf, userid, filename )    /* ȡĳ�û�mail�ļ� ·��*/
char    *buf, *userid, *filename;
{
    if (isalpha(userid[0]))  /* ��������ж�,����ݴ���, alex 1997.1.6*/
        sprintf( buf, "mail/%c/%s/%s", toupper(userid[0]), userid, filename );
    else
        sprintf( buf, "mail/wrong/%s/%s", userid, filename);
    return buf;
}

char *
setbpath( buf, boardname )   /* ȡĳ�� ·�� */
char *buf, *boardname;
{
    strcpy( buf, "boards/" );
    strcat( buf, boardname );
    return buf;
}

char *
setbfile( buf, boardname, filename )  /* ȡĳ�����ļ� */
char *buf, *boardname, *filename;
{
    sprintf( buf, "boards/%s/%s", boardname, filename );
    return buf;
}

void RemoveMsgCountFile2(userID)
char *userID;
{
  char fname[STRLEN];
  sethomefile(fname,userID,"msgcount");
  unlink(fname);
 }


