#include "bbs.h"

#include <sys/wait.h>
#include <sys/socket.h>
#include <netdb.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <arpa/telnet.h>
#include <sys/resource.h>
#ifdef AIX
#include <rpcsvc/rstat.h>
#endif
#include <varargs.h>

#define	QLEN		5
#define	PID_FILE	"reclog/bbs.pid"
#define	LOG_FILE	"reclog/bbs.log"

#define MAXPORTS        4
#ifdef D_TEST
static int myports[MAXPORTS] = {49457/* , 3456, 3001, 3002, 3003 */ };
#else
static int myports[MAXPORTS] = {23,8000,8001,8002/* , 3456, 3001, 3002, 3003 */ };
#endif
static int mport;
int csock;                      /* socket for Master and Child */

int max_load = 79; /* ԭֵ39 , modified by KCN,1999.09.07 */
/* 59->79 , modified by dong , 1999.9.7 */

/* ԭֵ29, modified by dong, 1998.11.23 */
#undef LOAD_LIMIT	
int heavy_load;
int server_pid;

char code[3];
void
cat(filename,msg)
char *filename,*msg;
{
    FILE*fp;

    if( (fp = fopen( filename, "a" )) != NULL ) {
        fputs( msg, fp );
        fclose( fp );
    }
}

int local_Net_Sleep(time)
{
    struct timeval tv ;
    int     sr;
    fd_set fd,efd;

    tv.tv_sec = time;
    tv.tv_usec = 0;
    FD_ZERO(&fd);
    FD_ZERO(&efd);
    FD_SET(csock,&fd);
    FD_SET(csock,&efd);

    while((sr=select(csock+1,&fd,NULL,&efd,&tv))>0) {
        char buf[256];
        if (FD_ISSET(csock,&efd))
            break;
        if (recv(csock,buf,256,0)<=0) break;
        tv.tv_sec = time;
        tv.tv_usec = 0;
        FD_SET(csock,&fd);
        FD_SET(csock,&efd);
    };
}


void
local_prints(va_alist)
va_dcl
{
    va_list args;
    unsigned char buf[512], *fmt;

    va_start(args);
    fmt = va_arg(args, char *);
    vsprintf(buf, fmt, args);
    va_end(args);
    write(0,buf,strlen(buf));
}

void
get_load( load )
/*ȡϵͳ����*/
double load[];
{
#ifdef LOAD_LIMIT
#ifdef LINUX
    FILE *fp;
    fp = fopen ("/proc/loadavg", "r");
    if (!fp) load[0] = load[1] = load[2] = 0;
    else {
        float av[3];
        fscanf (fp, "%g %g %g", av, av + 1, av + 2);
        fclose (fp);
        load[0] = av[0]; load[1] = av[1]; load[2] = av[2];
    }
#else
#ifdef BSD44
getloadavg( load, 3 );
#else
    struct statstime rs;
    rstat( "localhost", &rs );
    load[ 0 ] = rs.avenrun[ 0 ] / (double) (1 << 8);
    load[ 1 ] = rs.avenrun[ 1 ] / (double) (1 << 8);
    load[ 2 ] = rs.avenrun[ 2 ] / (double) (1 << 8);
#endif
#endif
#endif
}

int local_check_ban_IP(ip, buf)
char *ip;
char *buf;
{ /* Leeward 98.07.31
      RETURN:         
                     - 1: No any banned IP is defined now 
                       0: The checked IP is not banned
      other value over 0: The checked IP is banned, the reason is put in buf
      */
    FILE *Ban = fopen(".badIP", "r");
    char IPBan[64];
    int  IPX = - 1;
    char *ptr;

    if (!Ban)
        return IPX;
    else
        IPX ++;

    while (fgets(IPBan, 64, Ban))
    {
        if (ptr = strchr(IPBan, '\n'))
            *ptr = 0;
        if (ptr = strchr(IPBan, ' '))
        {
            *ptr ++ = 0;
            strcpy(buf, ptr);
        }
        IPX = strlen(IPBan);
        if (!strncmp(ip, IPBan, IPX))
            break;
        IPX = 0;
    }

    fclose(Ban);
    return IPX;
}

static void
telnet_init()
{
    static char svr[] = {
        IAC, DO, TELOPT_TTYPE,
        IAC, SB, TELOPT_TTYPE, TELQUAL_SEND, IAC, SE,
        IAC, WILL, TELOPT_ECHO,
        IAC, WILL, TELOPT_SGA
    };

    int n, len;
    char *cmd;
    int rset;
    struct timeval to;
    char buf[64];

    /* --------------------------------------------------- */
    /* init telnet protocol				 */
    /* --------------------------------------------------- */

#ifndef	LINUX
    to.tv_sec = 1;
    to.tv_usec = 1;
#endif
    cmd = svr;

    for (n = 0; n < 4; n++)
    {
        len = (n == 1 ? 6 : 3);
        send(0, cmd, len, 0);
        cmd += len;
        /* KCN !!!!!!!!!! wrong telnet
        #ifdef	LINUX
            to.tv_sec = 1;
            to.tv_usec = 1;
        #endif

            rset = 1;
            if (select(1, (fd_set *) & rset, NULL, NULL, &to) > 0)
              recv(0, buf, sizeof(buf), 0);
        */
    }
}

/* ----------------------------------------------------- */
/* stand-alone daemon					 */
/* ----------------------------------------------------- */


static void
start_daemon(inetd, port)
int inetd;
int port; /* Thor.981206: ȡ 0 ���� *û�в���* */
{
    int n;
    struct linger ld;
    struct sockaddr_in sin;
    struct rlimit rl;
    char buf[80], data[80];
    time_t val;
    int lock_pid;

    /*
     * More idiot speed-hacking --- the first time conversion makes the C
     * library open the files containing the locale definition and time zone.
     * If this hasn't happened in the parent process, it happens in the
     * children, once per connection --- and it does add up.
     */

    time(&val);
    strftime(buf, 80, "%d/%b/%Y %H:%M:%S", localtime(&val));

    /* --------------------------------------------------- */
    /* detach daemon process				 */
    /* --------------------------------------------------- */

    close(1);
    close(2);
    chdir(BBSHOME);
    umask(07);

    if(inetd) /* Thor.981206: inetd -i */
    {
        /* Give up root privileges: no way back from here	 */
        server_pid=0;
        setgid(BBSGID);
        setuid(BBSUID);
        setreuid(BBSUID,BBSUID);
        setregid(BBSGID,BBSGID);
        mport = port;
        if (port) strcpy(code,"e");
        else strcpy(code,"d");
        /*    sprintf(data, "%d\tinetd -i\n", getpid() );
            cat(PID_FILE, data);
        */
        return;
    }

    sprintf(buf,"bbsd start at %s",ctime(&val));
    cat(PID_FILE, buf);

    close(0);

    if (fork())
        exit(0);

    setsid();

    if (fork())
        exit(0);

    sin.sin_family = AF_INET;
    sin.sin_addr.s_addr = INADDR_ANY;

    /*  if (port <= 0)  Thor.981206: port 0 ����û�в���
      {
        n = MAXPORTS - 1;
        while (n)
        {
          if (fork() == 0)
    	break;

          sleep(1);
          n--;
        }
        port = myports[n];
      }
    */

    n = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

    val = 1;
    setsockopt(n, SOL_SOCKET, SO_REUSEADDR, (char *) &val, sizeof(val));

#if 0
    setsockopt(n, SOL_SOCKET, SO_KEEPALIVE, (char *) &val, sizeof(val));

    setsockopt(n, IPPROTO_TCP, TCP_NODELAY, (char *) &val, sizeof(val));
#endif

    ld.l_onoff = ld.l_linger = 0;
    setsockopt(n, SOL_SOCKET, SO_LINGER, (char *) &ld, sizeof(ld));
    sprintf(buf,"reclog/bbsd.pid.%d",port);
    if ((lock_pid=open(buf,O_CREAT|O_TRUNC),0755)==-1) {
        cat(PID_FILE,strerror(errno));
        exit(0);
    }
    /*
      flock(lock_pid,LOCK_EX);
    */
    sprintf(buf,"%d\n",getpid());
    write(lock_pid,buf,strlen(buf));
    close(lock_pid);

    mport = port;
    if (port==6001) strcpy(code,"e");
    else strcpy(code,"d");
    sin.sin_port = htons(port);
    if ((bind(n, (struct sockaddr *)&sin, sizeof(sin)) < 0) || (listen(n, QLEN) < 0)) {
        cat(PID_FILE,strerror(errno));
        exit(1);
    }

    /* --------------------------------------------------- */
    /* Give up root privileges: no way back from here	 */
    /* --------------------------------------------------- */

    setgid(BBSGID);
    setuid(BBSUID);
}


/* ----------------------------------------------------- */
/* reaper - clean up zombie children			 */
/* ----------------------------------------------------- */


static void
siguser1()
{
    heavy_load=1;
}
static void
siguser2()
{
    heavy_load=0;
}

static void
reaper()
{
    while (waitpid(-1, NULL, WNOHANG | WUNTRACED) > 0);
}

static void
main_term()
{
    exit(0);
}

static void
main_signals()
{
    struct sigaction act;

    /* act.sa_mask = 0; */ /* Thor.981105: ��׼�÷� */
    sigemptyset(&act.sa_mask);
    act.sa_flags = 0;

    act.sa_flags = SA_RESTART;
    act.sa_handler = main_term;
    sigaction(SIGTERM, &act, NULL);

    act.sa_handler = siguser1;
    sigaction(SIGUSR1, &act, NULL);

    act.sa_handler = siguser2;
    sigaction(SIGUSR2, &act, NULL);

    act.sa_handler=SIG_IGN;
    sigaction(SIGPIPE, &act, NULL);

    act.sa_handler = SIG_IGN;
    sigaction(SIGTTOU, &act, NULL);

    act.sa_handler = SIG_IGN;
    sigaction(SIGHUP, &act, NULL);

    /*  act.sa_handler = reaper; */
#ifdef AIX
    act.sa_handler = NULL;
    act.sa_flags = SA_RESTART|SA_NOCLDWAIT;
    sigaction(SIGCHLD, &act, NULL);
#else
    act.sa_handler = reaper;
    act.sa_flags = SA_RESTART;
    sigaction(SIGCHLD, &act, NULL);
#endif
    /* COMMAN: Ignore other signals used in bbs, to avoid group killing(pid=0)
    */

}

int
bbs_main(hid,argv)
char* hid;
char* argv;
{
    int         uid;
    char        buf[256]; /* Leeward 98.07.31 */
    char bbs_prog_path[ 256 ];
    FILE        *fp;

    /*    modified by period      2000-11-13      allow localhost anyway  */
    /*    if((fp = fopen("NOLOGIN","r")) != NULL) */
    if(strcmp(hid, "127.0.0.1") && (fp = fopen("NOLOGIN","r")) != NULL)
    {
        while(fgets(buf,256,fp) != NULL)
            local_prints(buf);
        fclose(fp);
        local_Net_Sleep(20);
        shutdown(csock,2);
        close(csock);
        exit(-1);
    }

#ifdef LOAD_LIMIT

    if ((fp = fopen("NO_LOAD_LIMIT", "r")) == NULL){  /* add by dong , 1999, 9, 10 */
        /* load control for BBS */
        {
            double      cpu_load[ 3 ];
            double         load ;

            get_load( cpu_load );
            load = cpu_load[ 0 ];
            local_prints("CPU ��� (1,10,15) ���ӵ�ƽ�����ɷֱ�Ϊ %.2f, %.2f, %.2f (Ŀǰ���� = %d)\r\n", cpu_load[ 0 ], cpu_load[ 1 ], cpu_load[ 2 ], max_load );
            if ( load < 0 || load > max_load ) {
                local_prints("�ܱ�Ǹ,Ŀǰ CPU ���ɹ���, ���Ժ�����\r\n");
                local_prints("\r\n��Ϊ�ظ����ӶԱ�վ���̫��������ϣ���Ҫ�ظ��������");
                local_prints("\r\n��������Ϣ10���ӣ�Ȼ�������ӱ�վ���ǳ���л��\r\n");
                /*sleep( load ); */
                /*sleep(1);  modified by dong, 1999.9.10 */
                local_Net_Sleep( load );
                shutdown(csock,2);
                close(csock);

                exit( -1 );
            }
#ifdef AIX
            { /* Leeward 98.06.11 For AIX only. Please man psdanger */
                int free = psdanger(-1);
                int safe = psdanger(SIGDANGER);
                int danger = 125000;
                local_prints("RAM ��ǰ����ҳ���߳�������ֵ %d (������ֵ = %d)\r\n\r\n",
                             safe, free - safe);

                if ( safe < danger/*Haohmaru.99.06.16.rem free - safe*/ ) {
                    if ((server_pid!=-1)&&(!heavy_load))
                        kill(server_pid,SIGUSR1);
                    local_prints("�ܱ�Ǹ,Ŀǰ RAM ������ʹ��, ���Ժ�����\r\n");
                    local_prints("\r\n��Ϊ�ظ����ӶԱ�վ���̫��������ϣ���Ҫ�ظ��������");
                    local_prints("\r\n��������Ϣ10���ӣ�Ȼ�������ӱ�վ���ǳ���л��\r\n");

                    /*sleep( 60 ); */
                    local_Net_Sleep( 60 );
                    shutdown(csock,2);
                    close(csock);

                    exit( -1 );
                }
                if ((server_pid!=-1)&&(heavy_load))
                    kill(server_pid,SIGUSR2);
            }
#endif
        }
    }
    else
        fclose(fp);
#endif	/* LOAD_LIMIT */

#ifdef BBSRF_CHROOT
    sprintf( bbs_prog_path, "/bin/bbs", BBSHOME );
    if (chroot(BBSHOME) != 0) {
        local_prints("Cannot chroot, exit!\r\n");
        exit( -1 );
    }
#else
    sprintf( bbs_prog_path, "%s/bin/bbs", BBSHOME );
#endif

    if (local_check_ban_IP(hid, buf) > 0) /* Leeward 98.07.31 */
    {
        local_prints("��վĿǰ����ӭ���� %s ����!\r\nԭ��%s��\r\n\r\n", hid, buf);
        local_Net_Sleep( 60 );
        shutdown(csock,2);
        close(csock);

        exit(-1);
    }


    hid[16] = '\0' ;
#if 0
#ifdef D_TEST
    strcat(bbs_prog_path, "test");
    execl( bbs_prog_path,"bbstest",code, hid,  NULL) ; /*����BBS*/
#else
    strcat(bbs_prog_path, "new");
    execl( bbs_prog_path,"bbsnew",code, hid,  NULL) ; /*����BBS*/
#endif
#endif
    main_bbs(hid,0,argv);
    exit(-1);
    write(0,"execl failed\r\n",12);
    exit( -1 );
}

int
main(argc, argv)
int argc;
char *argv[];
{
    int csock;			/* socket for Master and Child */
    int *totaluser;
    int value;
    struct sockaddr_in sin;
    struct hostent * whee;
    char hid[17];

    /* --------------------------------------------------- */
    /* setup standalone daemon				 */
    /* --------------------------------------------------- */

    /* start_daemon(argc > 1 ? atoi(argv[1]) : 0); */
    /* Thor.981206: ȡ 0 ���� *û�в���* */
    /* start_daemon(argc > 1 ? atoi(argv[1]) : -1); */

    /* Thor.981207: usage,  bbsd, or bbsd 1234, or bbsd -i 1234 */
    /*  start_daemon(argc > 2, atoi(argv[argc-1]));
     KCN change it for not port parm */
    start_daemon(argc > 2, atoi(argv[argc-1]));
    main_signals();

    /* --------------------------------------------------- */
    /* attach shared memory & semaphore			 */
    /* --------------------------------------------------- */

    server_pid=getpid();
    if (argc<=2)
        for (;;)
        {
            value = 1;
            if (select(1, (fd_set *) & value, NULL, NULL, NULL) < 0)
                continue;

            value = sizeof(sin);
            csock = accept(0, (struct sockaddr *)&sin, &value);
            if (csock < 0)
            {
                reaper();
                continue;
            }

            if (fork())
            {
                close(csock);
                if (heavy_load)
                    sleep(5);
                continue;
            }

            setsid();

            dup2(csock, 0);
            /* COMMAN: ����˵������1��2���ļ�������stderr and stdout����ļ�Ū��PASSWD֮��
            ����ͦ�е����˵������Ϊʲô��ǰ˰Ŀû�����Ϲ���....
            ����COMMAN:�����Ȼ����Ϊstderr,stdout����ȫ������������д�룬Ψһ�Ŀ���
            �������������system���á�*/
            dup2(0,2);
            dup2(0,1);
            /* COMMAN end */
            close(csock);
            break;
        }
    else {
        int sinlen = sizeof (struct sockaddr_in);
        getpeername (0,(struct sockaddr *) &sin,(void *) &sinlen);
    }

    /*
      whee = gethostbyaddr((char*)&sin.sin_addr.s_addr,sizeof(struct in_addr),AF_INET);
        if ((whee)&&(whee->h_name[0]))
          strncpy(hid, whee->h_name, 17) ;
        else
     KCN temp change it for trace IP!! don't remove. 2000.8.19*/
    {
        char*host = (char*)inet_ntoa(sin.sin_addr);
        strncpy(hid,host,17);
    }
    telnet_init();
    bbs_main(hid,argv[0]);
}
