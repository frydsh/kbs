/* ������ Server */
#undef BBSMAIN
#include "bbs.h"
#include "chat.h"
#include <sys/ioctl.h>
#ifdef lint
#include <sys/uio.h>
#endif  
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>

#ifdef LINUX
#include <unistd.h>
#endif

#ifdef AIX
#include <sys/select.h>
#endif

#if !RELIABLE_SELECT_FOR_WRITE
#include <fcntl.h>
#endif

#if USES_SYS_SELECT_H
#include <sys/select.h>
#endif
#define perror(x) perror(x)

#if NO_SETPGID
#define setpgid setpgrp
#endif


#define RESTRICTED(u)   (users[(u)].flags == 0) /* guest */
#define SYSOP(u)        (users[(u)].flags & PERM_SYSOP)
#define CHATOP(u)       (users[(u)].flags & PERM_CHATOP)
#define ROOMOP(u)       (users[(u)].flags & PERM_CHATROOM)

#define ROOM_LOCKED     0x1
#define ROOM_SECRET     0x2
#define ROOM_NOEMOTE    0x4  
#define ROOM_SYSOP      0x8
#define LOCKED(r)       (rooms[(r)].flags & ROOM_LOCKED)
#define SECRET(r)       (rooms[(r)].flags & ROOM_SECRET)
#define NOEMOTE(r)      (rooms[(r)].flags & ROOM_NOEMOTE)
#define SYSOPED(r)      (rooms[(r)].flags & ROOM_SYSOP)


#define ROOM_ALL        (-2)
#define PERM_CHATROOM PERM_CHAT
#define CHATMAXACTIVE  200
/*---	period modify IDLEN+1 ==> IDLEN+2	2000-10-18	---*/
struct chatuser
{
    int sockfd;                   /* socket to bbs server */
    int utent;                    /* utable entry for this user */
    int room;                     /* room: -1 means none, 0 means main */
    int flags;
    char userid[IDLEN + 2/*1*/];       /* real userid */
    char chatid[9];               /* chat id */
    char ibuf[128];               /* buffer for sending/receiving */
    int ibufsize;                 /* current size of ibuf */
    char lpIgnoreID[MAX_IGNORE][IDLEN + 2/*1*/]; /* Ignored-ID list,added by Luzi 97.11 */
    char lpEmote[MAX_EMOTES][128]; /* emotes list, added by Luzi 97.12.13 */
}        users[CHATMAXACTIVE];


struct chatroom
{
    char name[IDLEN];             /* name of room; room 0 is "main" */
    short occupants;              /* number of users in room */
    short flags;                  /* ROOM_LOCKED, ROOM_SECRET */
    char invites[CHATMAXACTIVE];      /* Keep track of invites to rooms */
    char topic[52];               /* Let the room op to define room topic */
}        rooms[MAXROOM];

struct chatcmd
{
    char *cmdstr;
    void (*cmdfunc) ();
    int exact;
    int bUsed;     /* Can this act be used in main-room :added by Luzi 98.1.3*/
};


int sock = -1;                  /* the socket for listening */
int nfds;                       /* number of sockets to select on */
int num_conns;                  /* current number of connections */
fd_set allfds;                  /* fd set for selecting */
struct timeval zerotv;          /* timeval for selecting */
char chatbuf[256];              /* general purpose buffer */

/* name of the main room (always exists) */

char mainroom[] = "zixia";
char maintopic[] = "��˿����������ˮ�������";
#define ENABLEMAIN 1

char *msg_not_op = "*** ��˿����Ҫ�Ҵ��� ***";
char *msg_no_such_id = "*** [1m%s[m ���ڱ��� ***";
char *msg_not_here = "*** [1m%s[m ��û��ǰ����˿�� ***";

/* Added by ming, 96.10.12 */
/* although there is strcasecmp() and strncasecmp()
   in solaris c runtime library, it has bugs when it compare Chinese
   code like "����" and "����". So we use our own code.
*/

#ifdef NO_STRCASECMP
ci_strcmp(s1,s2)
const char *s1, *s2;
{
    for(;;s1++,s2++) {
        if(*s1=='\0' || *s2 == '\0')
            return (*s1=='\0' && *s2=='\0' ? 0 : 1);
        if((isalpha(*s1)?*s1|0x20:*s1) != (isalpha(*s2)?*s2|0x20:*s2))
            return 1;
    }
    /*NOTREACHED*/
    return 1;
}

ci_strncmp(s1,s2,n)
const char * s1, *s2;
size_t n;
{
    for(;n;s1++,s2++,n--) {
        if(*s1=='\0' && *s2 == '\0')
            break ;
        /*if((isalpha(*s1)?*s1|0x20:*s1) != (isalpha(*s2)?*s2|0x20:*s2))
         */
        if (*s1 != *s2)
            return 1;
    }
    return 0;
}
#endif

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
            c1 += 'A' - 'a';
        if( c2 >= 'a' && c2 <= 'z' )
            c2 += 'A' - 'a';
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
            c1 += 'A' - 'a';
        if( c2 >= 'a' && c2 <= 'z' )
            c2 += 'A' - 'a';
        if( c1 != c2 )
            return (c1 - c2);
        if( c1 == 0 )
            return 0;
    }
}

int
flock(fd, op)
int fd, op;
{
    switch (op) {
    case LOCK_EX:
        return lockf( fd, F_LOCK, 0 );
    case LOCK_UN:
        return lockf( fd, F_ULOCK, 0 );
    default:
        return -1;
    }
}

#define  report  vdfdsa#fds



is_valid_chatid(id)
char *id;
{
    int i;
    if (*id == '\0')
        return 0;
    if (strlen(id)>8) id[8]=0;
    for (i = 0; i < 8 && *id; i++, id++) {
        if ( /* !isprint(*id) || */ *id==0x1b || *id == '*' || *id == ':'||*id=='/' ||*id=='%'||strstr(id,"\\n")||*id==']')
            return 0;
    }
    if(!strncmp(id, "��", 2))
        return 0;
    return 1;
}


int
Isspace(ch)
char ch;
{
    /*  return (int) strchr(" \t\n\r", ch); */
    if (ch == ' ' || ch == '\t' || ch == '\r' || ch == '\n')
        return 1;
    else
        return 0;
}


char *
getnextword(str)
char **str;
{
    char *p;
    while (Isspace(**str))
        (*str)++;
    p = *str;
    while (**str && !Isspace(**str))
        (*str)++;
    if (**str)
    {
        **str = '\0';
        (*str)++;
    }
    return p;
}


int
chatid_to_indx(chatid)
char *chatid;
{
    register int i;
    for (i = 0; i < CHATMAXACTIVE; i++)
    {
        if (users[i].sockfd == -1)
            continue;
        if (!strcasecmp(chatid, users[i].chatid))
            return i;
    }
    return -1;
}

int
userid_to_indx(userid) /* add by Luzi 1997.11.18 */
char *userid;
{
    register int i;
    for (i = 0; i < CHATMAXACTIVE; i++)
    {
        if (users[i].sockfd == -1)
            continue;
        if (!strcasecmp(userid, users[i].userid))
            return i;
    }
    return -1;
}

void
chat_query(myunum, msg)   /* add by Luzi 1997.11.18 */
int myunum;
char *msg;
{
    char modestr[30];
    char *userid;
    int unum;
    userid=getnextword(&msg);
    unum = userid_to_indx(userid);
    if (unum >= 0 && users[unum].room>=0)
        if ((rooms[users[unum].room].flags & ROOM_SECRET) ==0
                || rooms[users[unum].room].invites[myunum]
                || SYSOP(myunum) || CHATOP(myunum) )
        {
            sprintf(modestr,"1%s",
                    rooms[users[unum].room].name);
            send(users[myunum].sockfd,modestr,strlen(modestr),0);
            return;
        }
    send(users[myunum].sockfd,"0",1,0);
}

void
chat_query_ByChatid(myunum, msg)   /* add by dong , 1998.9.12 */
int myunum;
char *msg;
{
    char userstr[40];
    char *chatid;
    int unum;
    chatid=getnextword(&msg);
    unum = chatid_to_indx(chatid);
    if (unum >= 0 && users[unum].room>=0)
    {
        sprintf(userstr,"1%s",users[unum].userid);
        send(users[myunum].sockfd,userstr,strlen(userstr),0);
        return;
    }
    send(users[myunum].sockfd,"0",1,0);
}
int
fuzzy_chatid_to_indx(chatid) /* Modified by ming, 96.10.10 */
char *chatid;              /* Fixed a bug by Leeward 99.10.08 */
{
    register int i, indx = -1;
    int len = strlen(chatid);

    /* search chatid first */
    for (i = 0; i < CHATMAXACTIVE; i++)
    {
        if (users[i].sockfd == -1)
            continue;
        if (!strncasecmp(chatid, users[i].chatid, len))
        {
            if (len == strlen(users[i].chatid))
                return i;
            if (indx == -1)
                indx = i;
            else
                /*return -2;*/
                indx = -2; /* Leeward 99.10.08 */
        }
    }

    /* Alex said need not compare to userid
      if (indx != -1)
        return indx;

      for (i = 0; i < CHATMAXACTIVE; i++)
      {
        if (users[i].sockfd == -1)
          continue;
        if (!strncasecmp(chatid, users[i].userid, len))
        {
          if (len == strlen(users[i].userid))
            return i;
          if (indx == -1)
            indx = i;
          else
            return -2;
        }
      }
    */
    return indx;
}

int
roomid_to_indx(roomid)    /* modify by Luzi 1998.1.16 */
char *roomid;
{
    register int i;
    char ch='\0';
    if (strlen(roomid)>=IDLEN)
    {
        ch=roomid[IDLEN-1];
        roomid[IDLEN-1]='\0';
    }
    for (i = 0; i < MAXROOM; i++)
    {
        if (i && rooms[i].occupants == 0)
            continue;
        /*    report(roomid);
            report(rooms[i].name);*/
        if (!strcasecmp(roomid, rooms[i].name))
            break;
    }
    if (ch) roomid[IDLEN-1]=ch;
    if (i< MAXROOM) return i;
    return -1;
}


void
do_send(writefds, str)
fd_set *writefds;
char *str;
{
    register int i;
    int len = strlen(str);

    if (select(nfds, NULL, writefds, NULL, &zerotv) > 0)
    {
        for (i = 0; i < nfds; i++)
            if (FD_ISSET(i, writefds))
                send(i, str, len + 1, 0);
    }
}


void
send_to_room(room, str, unum)
int room;
char *str;
int unum;
{
    int i;
    fd_set writefds;

    FD_ZERO(&writefds);
    for (i = 0; i < CHATMAXACTIVE; i++)
    {
        if (users[i].sockfd == -1)
            continue;
        /*    if (room == ROOM_ALL || room == users[i].room)*/
        if (room == ROOM_ALL || (room == users[i].room && can_send(unum,i)) )
            /* write(users[i].sockfd, str, strlen(str) + 1); */
            FD_SET(users[i].sockfd, &writefds);
    }
    do_send(&writefds, str);
}

/* add by KCN for support multi-line emote */
void
send_to_room2(roomid,chatbuf,unum)
int roomid;
char* chatbuf;
int unum;
{
    int i;
    char *tmpbuf,*tmpbuf2,*party2;

    i=0;
    tmpbuf=chatbuf;
    party2=strchr(chatbuf,' ');
    while ((i<6)&&(tmpbuf2=strstr(party2,"\\n"))) {
        *tmpbuf2=0;
        *(tmpbuf2+1)=' ';
        send_to_room(roomid, tmpbuf, unum);
        *tmpbuf2=' ';
        *(tmpbuf2-1)=' ';
        *(tmpbuf2-2)=' ';
        tmpbuf=tmpbuf2-2;
        party2=tmpbuf2+2;
        i++;
    }
    if (*party2)
        send_to_room(roomid, tmpbuf, unum);
}


void
send_to_unum(unum, str)
int unum;
char *str;
{
    fd_set writefds;
    FD_ZERO(&writefds);
    FD_SET(users[unum].sockfd, &writefds);
    do_send(&writefds, str);
}


void
send_to_unum2(unum, chatbuf)
int unum;
char *chatbuf;
{
    int i;
    char *tmpbuf,*tmpbuf2,*party2;

    i=0;
    tmpbuf=chatbuf;
    party2=strchr(chatbuf,' ');
    while ((i<6)&&(tmpbuf2=strstr(party2,"\\n"))) {
        *tmpbuf2=0;
        *(tmpbuf2+1)=' ';
        send_to_unum(unum,tmpbuf);
        *tmpbuf2=' ';
        *(tmpbuf2-1)=' ';
        *(tmpbuf2-2)=' ';
        tmpbuf=tmpbuf2-2;
        party2=tmpbuf2+2;
        i++;
    }
    if (*party2)
        send_to_unum(unum,tmpbuf);
}

void
exit_room(unum, disp, msg)
int unum;
int disp;
char *msg;
{
    int oldrnum = users[unum].room;

    if (oldrnum != -1)
    {
        if (--rooms[oldrnum].occupants)
        {
            switch (disp)
            {
            case EXIT_LOGOUT:
                sprintf(chatbuf, "*** [1m%s[m (%s) �����뿪�� ***", users[unum].chatid, users[unum].userid); /* dong 1998.9.13 */
                if (msg && *msg)
                {
                    strcat(chatbuf, ": ");
                    strncat(chatbuf, msg, 80);
                }
                break;

            case EXIT_LOSTCONN:
	        sprintf(chatbuf, "*** [1m%s[m (%s) �ѻص������ǰ ***", users[unum].chatid, users[unum].userid); /* dong 1998.9.13 */
                break;

            case EXIT_KICK:
                /*sprintf(chatbuf, "*** [1m%s[m ��������op�߳�ȥ�� ***", users[unum].chatid);*/
	        sprintf(chatbuf, "*** [1m%s[m (%s) ������(%s)���ȥ�� ***", users[unum].chatid, users[unum].userid,msg); /* Leeward 98.03.02 */  /* dong 1998.9.13 */
                break;
            }
            send_to_room(oldrnum, chatbuf, unum);
        }
    }
    users[unum].flags &= ~PERM_CHATROOM;
    users[unum].room = -1;
}


void
chat_topic(unum, msg)
int unum;
char *msg;
{
    int rnum;
    rnum = users[unum].room;

    if (!ROOMOP(unum) && !SYSOP(unum) && !CHATOP(unum))
    {
        send_to_unum(unum, msg_not_op);
        return;
    }
    if (rnum==0 && !SYSOP(unum))
    {
	send_to_unum(unum, "�� �����Ҹ�zixia�Ļ��� ��");
        return;
    }
    if (*msg == '\0')
    {
        send_to_unum(unum, "�� ��ָ������ ��");
        return;
    }

    strncpy(rooms[rnum].topic, msg, 52);
    rooms[rnum].topic[51] = '\0';
    sprintf(chatbuf, "/t%.51s", msg);
    send_to_room(rnum, chatbuf, unum);
    sprintf(chatbuf, "*** [1m%s[m �������Ϊ [1m%s[m ***", users[unum].chatid, msg);
    send_to_room(rnum, chatbuf, unum);
}

void
chat_name_room(unum, msg)
int unum;
char *msg;
{
    int rnum;
    char *roomid=getnextword(&msg);
    rnum = users[unum].room;

    if (!ROOMOP(unum) && !SYSOP(unum) && !CHATOP(unum))
    {
        send_to_unum(unum, msg_not_op);
        return;
    }
    if (*roomid== '\0')
    {
	send_to_unum(unum, "�� ��ָ���µĶ�Ѩ���� ��");
        return;
    }
    if (rnum==0)
    {
        send_to_unum(unum,"�� ���ܸ��� zixia �������� ��");
        return;
    }
    if (roomid_to_indx(roomid)>=0)
    {
	send_to_unum(unum, "�� �Ѿ���ͬ���Ķ�Ѩ�� ��");
        return;
    }
    strncpy(rooms[rnum].name, roomid, IDLEN-1);
    rooms[rnum].name[IDLEN-1]='\0';
    sprintf(chatbuf, "/z%s", rooms[rnum].name);
    send_to_room(rnum, chatbuf, unum);
    sprintf(chatbuf, "*** \033[1m%s\033[m ���������Ƹ�Ϊ \033[1m%s\033[m ***", users[unum].chatid, rooms[rnum].name);
    send_to_room(rnum, chatbuf, unum);
}

enter_room(unum, room, msg)
int unum;
char *room;
char *msg;
{
    int rnum;
    int op = 0;
    register int i;

    rnum = roomid_to_indx(room);
    if (rnum == -1)
    {
        /* new room */
        for (i = 1; i < MAXROOM; i++)
        {
            if (rooms[i].occupants == 0)
            {
                /*        report("new room");*/
                rnum = i;
                memset(rooms[rnum].invites, 0, CHATMAXACTIVE);
                strcpy(rooms[rnum].topic, maintopic);
                strncpy(rooms[rnum].name, room, IDLEN - 1);
                rooms[rnum].name[IDLEN - 1] = '\0';
                rooms[rnum].flags = 0;
                op++;
                break;
            }
        }
        if (rnum == -1)
        {
	    send_to_unum(unum, "�� ���ǵ���˿������� ��");
            return 0;
        }
    }
    if (!SYSOP(unum) && !CHATOP(unum))
        if (LOCKED(rnum) && rooms[rnum].invites[unum] == 0)
        {
	    send_to_unum(unum, "�� �������ֻ����У��������� ��");
            return 0;
        }
    if(!SYSOP(unum))
        if (SYSOPED(rnum) && rooms[rnum].invites[unum]==0)
        {
	    send_to_unum(unum, "�� �������ܣ��������� ��");
            return 0;
        }

    exit_room(unum, EXIT_LOGOUT, msg);
    users[unum].room = rnum;
    if (op)
        users[unum].flags |= PERM_CHATROOM;
    rooms[rnum].occupants++;
    rooms[rnum].invites[unum] = 0;
    sprintf(chatbuf, "*** [1m%s[m (%s) ���� [1m%s[m �� ***",
            users[unum].chatid, users[unum].userid, rooms[rnum].name);
    send_to_room(rnum, chatbuf, unum);
    sprintf(chatbuf, "/r%s", room);
    send_to_unum(unum, chatbuf);
    sprintf(chatbuf, "/t%s", rooms[rnum].topic);
    send_to_unum(unum, chatbuf);
    return 0;
}


void
logout_user(unum)
{
    int i, sockfd = users[unum].sockfd;

    close(sockfd);
    FD_CLR(sockfd, &allfds);
    memset(&users[unum], 0, sizeof(users[unum]));
    users[unum].sockfd = users[unum].utent = users[unum].room = -1;
    for (i = 0; i < MAXROOM; i++)
    {
        if (rooms[i].invites != NULL)
            rooms[i].invites[unum] = 0;
    }
    num_conns--;
}

print_user_counts(unum)
{
    int i, c, userc = 0, suserc = 0, roomc = 0;
    for (i = 0; i < MAXROOM; i++)
    {
        c = rooms[i].occupants;
        if (i > 0 && c > 0)
        {
            if (!SECRET(i) || SYSOP(unum))
                roomc++;
        }
        c = users[i].room;
        if (users[i].sockfd != -1 && c != -1)
        {
            if (SECRET(c) && !SYSOP(unum))
                suserc++;
            /* else modified by dong, 1998.10.6*/
            userc++;
        }
    }
    sprintf(chatbuf,
	    "�� ��ӭ������˿����Ŀǰ�Ѿ��� %d �䶴���п��� �� ", 
            roomc + 1);
    send_to_unum(unum, chatbuf);
    sprintf(chatbuf, "�� ��˿���ڹ��� %d �� ", userc + 1);
    if (suserc)
        sprintf(chatbuf + strlen(chatbuf), "[���� %d �����ض�]", suserc);
    sprintf(chatbuf + strlen(chatbuf), "��" );
    send_to_unum(unum, chatbuf);
    if (!ENABLEMAIN)
        send_to_unum(unum,"[1m�� [33mmain[0m[1m �������ֹ���졢��С���������� [33m/j[0m[1m ָ����������ҽ�������[0m");
    send_to_unum(unum,"[1m�� [33mmain[0m[1m ��˿�����ֹ���졢��С���������� [33m/j[0m[1m ָ��������ڽ�������[0m");
    send_to_unum(unum,"[1m���� [33m/e[0m[1m �뿪��˿����[33m/w[0m[1m �鿴�����е��û�[0m");
    send_to_unum(unum,"[1m���� [33m/r[0m[1m �鿴��ǰ�Ķ��ڣ�[33m/j roomname[0m[1m ������Ӧ����[0m");
    return 0;
}

char *
sethomefile( buf, userid , filename) /* added by Luzi 1997.11.30 */
char    *buf, *userid, *filename;
{
    if (isalpha(userid[0]))
        sprintf( buf, "home/%c/%s/%s", toupper(userid[0]), userid ,filename);
    else sprintf( buf, "home/wrong/%s/%s", userid, filename);
    return buf;
}

void
get_ignore_list(unum)                /* added by Luzi 1997.11.30 */
int unum ;
{
    int fd ;
    int id = 1 ;

    char path[40];
    sethomefile( path, users[unum].userid , "/ignores");

    /*---	Added by period	2000-10-18	just guessing problems	---*/
    for(id=0;id<MAX_IGNORE;id++)
        users[unum].lpIgnoreID[id][0]='\0';
    /*---		---*/

    if((fd = open(path,O_RDONLY,0)) == -1)
    {
        users[unum].lpIgnoreID[0][0]='\0';
        return;
    }
    for(id=0;id<MAX_IGNORE;id++)
        if (read(fd,users[unum].lpIgnoreID[id],IDLEN+1)<=IDLEN)
        {
            users[unum].lpIgnoreID[id][0]='\0';
            break;
        }
    close(fd);
    return;
}

void
get_emote_list(unum)                /* added by Luzi 1998.1.25 */
int unum ;
{
    int fd ;
    int id = 1 ;

    char path[40];
    sethomefile( path, users[unum].userid , "/emotes");

    if((fd = open(path,O_RDONLY,0)) == -1)
    {
        users[unum].lpEmote[0][0]='\0';
        return;
    }
    for(id=0;id<MAX_EMOTES;id++)
        if (read(fd,users[unum].lpEmote[id],128)<=127)
        {
            users[unum].lpEmote[id][0]='\0';
            break;
        }
    close(fd);
    return;
}

int can_send(myunum,unum)            /* added by Luzi 1997.11.30 */
int myunum,unum;
{
    int i;
    for(i=0;i<MAX_IGNORE;i++)
        if (users[unum].lpIgnoreID[i][0]!='\0')
            if (!strcasecmp(users[unum].lpIgnoreID[i], users[myunum].userid)
                    && !SYSOP(myunum)) return 0;
    return 1;
}

void
chat_ignore(unum, msg)   /* add by Luzi 1997.11.28 */
int unum;
char *msg;
{
    char *userid;
    short i;
    userid=getnextword(&msg);
    for(i=0;i<MAX_IGNORE;i++)
        if (users[unum].lpIgnoreID[i][0]=='\0')
        {
            strcpy(users[unum].lpIgnoreID[i],userid);
            return;
        }
}

void
chat_alias_add(unum, msg)   /* add by Luzi 1998.1.25 */
int unum;
char *msg;
{
    short i;
    for(i=0;i<MAX_EMOTES;i++)
        if (users[unum].lpEmote[i][0]=='\0')
        {
            strcpy(users[unum].lpEmote[i],msg);
            return;
        }
}


void
chat_listen(unum, msg)   /* add by Luzi 1997.11.28 */
int unum;
char *msg;
{
    char *userid;
    short i;
    userid=getnextword(&msg);
    for(i=0;i<MAX_IGNORE;i++)
        if (users[unum].lpIgnoreID[i][0]!='\0')
            if (!strcasecmp(users[unum].lpIgnoreID[i], userid))
            {
                users[unum].lpIgnoreID[i][0]='\0';
                return;
            }
}

void
chat_alias_del(unum, msg)   /* add by Luzi 1998.1.25 */
int unum;
char *msg;
{
    char *emoteid;
    short i,len;
    emoteid=getnextword(&msg);
    len=strlen(emoteid);
    for(i=0;i<MAX_EMOTES;i++)
        if (users[unum].lpEmote[i][0]!='\0')
        {
            if (!strncasecmp(users[unum].lpEmote[i], emoteid, len))
                if (users[unum].lpEmote[i][len]==' ')
                {
                    users[unum].lpEmote[i][0]='\0';
                    return;
                }
        }
}


login_user(unum, msg)
int unum;
char *msg;
{
    int i, utent, fd = users[unum].sockfd;
    char *utentstr;
    char *level;
    char *userid;
    char *chatid;

    utentstr = getnextword(&msg);
    level = getnextword(&msg);
    userid = getnextword(&msg);
    chatid = getnextword(&msg);

    utent = atoi(utentstr);
    for (i = 0; i < CHATMAXACTIVE; i++)
    {
        if (users[i].sockfd != -1 && users[i].utent == utent)
        {
            send_to_unum(unum, CHAT_LOGIN_BOGUS);
            return -1;
        }
    }
    if (!is_valid_chatid(chatid))
    {
        send_to_unum(unum, CHAT_LOGIN_INVALID);
        return 0;
    }
    if (chatid_to_indx(chatid) != -1)
    {
        /* userid in use */
        send_to_unum(unum, CHAT_LOGIN_EXISTS);
        return 0;
    }

    /*  report(level);*/

    users[unum].utent = utent;
    users[unum].flags = atoi(level);
    strcpy(users[unum].userid, userid);
    strncpy(users[unum].chatid, chatid, 8);
    users[unum].chatid[8] = '\0';
    send_to_unum(unum, CHAT_LOGIN_OK);
    get_emote_list(unum);               /* added by Luzi 1998.01.25 */
    get_ignore_list(unum);              /* added by Luzi 1997.11.28 */
    print_user_counts(unum);
    sprintf(chatbuf,"User entry:%d",utent);
    send_to_room(mainroom,chatbuf , unum);
    enter_room(unum, mainroom, (char *) NULL);
    return 0;
}


void
chat_list_rooms(unum, msg)
int unum;
char *msg;
{
    int i,j,pause, occupants;
    char* str;

    j=0;
    str = getnextword(&msg);
    if (*str == '\0')
        pause = 1;
    else
        pause = 0;

    if (RESTRICTED(unum))
    {
	send_to_unum(unum, "�� ��Ǹ����ز����㿴����Щ��Ѩ�п��� ��");
        return;
    }
    send_to_unum(unum, "[33m[44m ��Ѩ����    ������������        [m");
    for (i = 0; i < MAXROOM; i++)
    {
        occupants = rooms[i].occupants;
        if (occupants > 0)
        {
            if (!SYSOP(unum) && !CHATOP(unum))
                if ((rooms[i].flags & ROOM_SECRET) && (users[unum].room != i))
                    continue;
            sprintf(chatbuf, " %-12s��%4d��%s\033[m", rooms[i].name, occupants, rooms[i].topic);
            /*---	period	2000-10-24	add background color to chatroom status	---*/
            if (rooms[i].flags & ROOM_LOCKED)
                strcat(chatbuf, " [\033[30;47m��ס\033[m]");
            if (rooms[i].flags & ROOM_SECRET)
                strcat(chatbuf, " [\033[30;47m����\033[m]");
            if (rooms[i].flags & ROOM_NOEMOTE)
                strcat(chatbuf, " [\033[30;47m��ֹ����\033[m]");
            if (rooms[i].flags & ROOM_SYSOP)
                strcat(chatbuf, " [\033[30;47m����\033[m]");

            send_to_unum(unum, chatbuf);
        }
    }
}


chat_do_user_list(unum, msg, whichroom)
int unum;
char *msg;
int whichroom;
{
    char buf[14]="    [����]    ";/*Haohmaru.99.09.18*/
    char buf1[14]="[����]  [��ס]";
    char buf2[14]="    [����]    ";
    char buf3[14]="    [��ס]    ";
    int start, stop, curr = 0;
    int i,j, rnum, myroom = users[unum].room;
    j=0;
    while (*msg && Isspace(*msg))
        msg++;
    start = atoi(msg);
    while (*msg && isdigit(*msg))
        msg++;
    while (*msg && !isdigit(*msg))
        msg++;
    stop = atoi(msg);
    /*Haohmaru.99.09.18*/
    if(1/*users[unum].room != 0*/){
        if ((rooms[whichroom].flags & ROOM_SECRET) && (rooms[whichroom].flags & ROOM_LOCKED))
            sprintf(chatbuf, "[33m[44m ������ũ����ڴ���    ��������%-14s[m",buf1);
        else if (rooms[whichroom].flags & ROOM_SECRET)
            sprintf(chatbuf, "[33m[44m ������ũ����ڴ���    ��������%-14s[m",buf2);
        else if (rooms[whichroom].flags & ROOM_LOCKED)
            sprintf(chatbuf, "[33m[44m ������ũ����ڴ���    ��������%-14s[m",buf3);
        else
            sprintf(chatbuf, "[33m[44m ������ũ����ڴ���    ��������%-14s[m",buf);
        send_to_unum(unum,chatbuf);}
    else
        send_to_unum(unum,"[33m[44m ������ũ����ڴ���    �������� [m");
    for (i = 0; i < CHATMAXACTIVE; i++)
    {
        rnum = users[i].room;
        if (users[i].sockfd != -1 && rnum != -1)
        {
            if (whichroom != -1 && whichroom != rnum)
                continue;
            if (myroom != rnum)
            {
                if (RESTRICTED(unum))
                    continue;
                if ((rooms[rnum].flags & ROOM_SECRET) &&
                        (rooms[rnum].invites[unum]==0) && !SYSOP(unum) && !CHATOP(unum))
                    continue;
            }
            curr++;
            if (curr < start)
                continue;
            else if (stop && (curr > stop))
                break;
            sprintf(chatbuf, " %-8s��%-12s��%s", users[i].chatid,
                    users[i].userid, rooms[rnum].name);
            if (ROOMOP(i))
                strcat(chatbuf, " [Op]");
            send_to_unum(unum, chatbuf);
        }
    }
    return 0;
}


void
chat_list_by_room(unum, msg)
int unum;
char *msg;
{
    int whichroom;
    char *roomstr;

    roomstr = getnextword(&msg);
    if (*roomstr == '\0')
        whichroom = users[unum].room;
    else
    {
        if ((whichroom = roomid_to_indx(roomstr)) == -1)
        {
	    sprintf(chatbuf, "�� û�������� ��", roomstr);
            send_to_unum(unum, chatbuf);
            return;
        }
        if ((rooms[whichroom].flags & ROOM_SECRET) && !SYSOP(unum) && !CHATOP(unum))
        {
            send_to_unum(unum, "�� ��˿�������춴Ѩ�Թ����ģ�û������ ��");
            return;
        }
    }
    chat_do_user_list(unum, msg, whichroom);
}


void
chat_list_users(unum, msg)
int unum;
char *msg;
{
    chat_do_user_list(unum, msg, -1);
}

void
chat_map_chatids(unum, whichroom)
int unum;
int whichroom;
{
    int i, c, myroom, rnum;
    myroom = users[unum].room;
    send_to_unum(unum,
                 "[33m[44m ������� ���ڴ���    �� ������� ���ڴ���    �� ������� ���ڴ���   [m");
    for (i = 0, c = 0; i < CHATMAXACTIVE; i++)
    {
        rnum = users[i].room;
        if (users[i].sockfd != -1 && rnum != -1)
        {
            if (whichroom != -1 && whichroom != rnum)
                continue;
            if (myroom != rnum)
            {
                if (RESTRICTED(unum))
                    continue;
                if ((rooms[rnum].flags & ROOM_SECRET) && !SYSOP(unum) && !CHATOP(unum))
                    continue;
            }
            sprintf(chatbuf + (c * 24), " %-8s%c%-12s%s",
                    users[i].chatid, (ROOMOP(i)) ? '*' : ' ', users[i].userid,
                    (c <2 ? "��" : "  "));
            if (++c == 3)
            {
                send_to_unum(unum, chatbuf);
                c = 0;
            }
        }
    }
    if (c > 0)
        send_to_unum(unum, chatbuf);
}


void
chat_map_chatids_thisroom(unum, msg)
int unum;
char *msg;
{
    chat_map_chatids(unum, users[unum].room);
}


void
chat_setroom(unum, msg)
int unum;
char *msg;
{
    char *modestr;
    int rnum = users[unum].room;
    int sign = 1;
    int flag;
    char *fstr;

    modestr = getnextword(&msg);
    if (!ROOMOP(unum))
    {
        send_to_unum(unum, msg_not_op);
        return;
    }
    if (*modestr == '+')
        modestr++;
    else if (*modestr == '-')
    {
        modestr++;
        sign = 0;
    }
    if (*modestr == '\0')
    {
        send_to_unum(unum,
                     "�� ����������Ҫ�Ķ�Ѩ��: {[[1m+[m(�趨)][[1m-[m(ȡ��)]}{[[1ml[m(��ס)][[1ms[m(����)]}");
        return;
    }
    while (*modestr)
    {
        flag = 0;
        switch (*modestr)
        {
        case 'l':
        case 'L':
            if (!rnum&&!SYSOP(unum))/*added by Haohmaru,98.9.6*/
            {
                send_to_unum(unum, "�� zixia �����Ҳ��ܱ���ס ��");
                break;
            }
            flag = ROOM_LOCKED;
            fstr = "��ס";
            break;
        case 's':
        case 'S':
            if (!rnum&&!SYSOP(unum))/*added by Haohmaru,98.9.6*/
            {
                send_to_unum(unum, "�� zixia �����Ҳ��ܱ���Ϊ������ʽ ��");
                break;
            }
            flag = ROOM_SECRET;
            fstr = "����";
            break;
        case 'e':
        case 'E':
            flag = ROOM_NOEMOTE;
            fstr = "'��ֹ����'";
            break;
        case 'x':
        case 'X':
            if (SYSOP(unum))
            {
                flag = ROOM_SYSOP;
                fstr = "'����'";
            }
            break;
        default:
            sprintf(chatbuf, "�� ��ز����������˼��[[1m%c[m] ��", *modestr);
            send_to_unum(unum, chatbuf);
        }
        if (flag && ((rooms[rnum].flags & flag) != sign * flag))
        {
            rooms[rnum].flags ^= flag;
            sprintf(chatbuf, "*** �ⶴѨ�� [1m%s[m %s%s����ʽ ***",
                    users[unum].chatid, sign ? "�趨Ϊ" : "ȡ��", fstr);
            send_to_room(rnum, chatbuf, unum);
        }
        modestr++;
    }
}


void
chat_nick(unum, msg)
int unum;
char *msg;
{
    char *chatid;
    int othernum;

    chatid = getnextword(&msg);
    if (!is_valid_chatid(chatid))
    {
        send_to_unum(unum, "�� �Ƿ����֣����������� ��");
        return;
    }
    chatid[8]='\0';
    othernum = chatid_to_indx(chatid);
    if (othernum != -1 && othernum != unum)
    {
        /*    send_to_unum(unum, "�� ��Ǹ�����˸���ͬ���������㲻�ܽ��� ��");*/
        send_to_unum(unum, "�� ��Ǹ�������Ѿ�ʹ�ø�chatid,������������ʹ�� ��");
        return;
    }
    sprintf(chatbuf, "�� [1m%s[m ��������Ÿ�Ϊ [1m%s[m ��",
            users[unum].chatid, chatid);
    send_to_room(users[unum].room, chatbuf, unum);
    

    strcpy(users[unum].chatid, chatid);
    sprintf(chatbuf, "/n%s", users[unum].chatid);
    send_to_unum(unum, chatbuf);
}


void
chat_private(unum, msg)
int unum;
char *msg;
{
    char *recipient;
    int recunum;

    recipient = getnextword(&msg);
    recunum = fuzzy_chatid_to_indx(recipient);
    if (recunum < 0)
    {
        /* no such user, or ambiguous */
        if (recunum == -1)
            sprintf(chatbuf, msg_no_such_id, recipient);
        else
            sprintf(chatbuf, " �� �Է���ʲ������? ��", recipient);
        send_to_unum(unum, chatbuf);
        return;
    }
    if (*msg)
    {
        if (can_send(unum,recunum))
        {
            sprintf(chatbuf, "[1m*%s*[m ", users[unum].chatid);
            strncat(chatbuf, msg, 80);
            send_to_unum(recunum, chatbuf);
            sprintf(chatbuf, "[1m%s>[m ", users[recunum].chatid);
            strncat(chatbuf, msg, 80);
            send_to_unum(unum, chatbuf);
        }
    }
}


put_chatid(unum, str)
int unum;
char *str;
{
    int i;
    char *chatid = users[unum].chatid;
    memset(str, ' ', 10);
    for (i = 0; chatid[i]; i++)
        str[i] = chatid[i];
    str[i] = ':';
    str[10] = '\0';
}


chat_allmsg(unum, msg)
int unum;
char *msg;
{
    if (*msg)
    {
        put_chatid(unum, chatbuf);
        strcat(chatbuf, msg);
        send_to_room(users[unum].room, chatbuf, unum);
    }
    return 0;
}


void
chat_act(unum, msg)
int unum;
char *msg;
{
    if (*msg)
    {
        sprintf(chatbuf, "[1m%s[m %s", users[unum].chatid, msg);
        send_to_room2(users[unum].room, chatbuf, unum);
    }
}


void
chat_join(unum, msg)
int unum;
char *msg;
{
    char *roomid;

    roomid = getnextword(&msg);
    if (RESTRICTED(unum))
    {
        send_to_unum(unum, "�� ��ֻ������������ ��");
        return;
    }
    if (*roomid == '\0')
    {
        send_to_unum(unum, "�� �����ĸ����� ��");
        return;
    }
    enter_room(unum, roomid, msg);
    return;
}

void
chat_kick(unum, msg)
int unum;
char *msg;
{
    char *twit;
    int rnum = users[unum].room, rnum2;
    int recunum;
    char buf[80];

    twit = getnextword(&msg);
    if (!ROOMOP(unum) && !SYSOP(unum) && !CHATOP(unum))
    {
        send_to_unum(unum, msg_not_op);
        return;
    }
    if ((recunum = chatid_to_indx(twit)) == -1)
    {
        sprintf(chatbuf, msg_no_such_id, twit);
        send_to_unum(unum, chatbuf);
        return;
    }
    if (rnum != users[recunum].room && !SYSOP(unum))
    {
        sprintf(chatbuf, msg_not_here, users[recunum].chatid);
        send_to_unum(unum, chatbuf);
        return;
    }
    if (SYSOP(recunum) && !SYSOP(unum))
    {
        send_to_unum(unum, msg_not_op);
        return;
    }

    /* Leeward 98.04.30 */
    sprintf(chatbuf, "*** �㱻����([1m[33m%s[m)�߳����� ***", users[unum].userid);
    send_to_unum(recunum, chatbuf);

    rnum2=users[recunum].room;

    /*exit_room(recunum, EXIT_KICK, (char *) NULL);*/
    exit_room(recunum, EXIT_KICK, users[unum].userid); /* Leeward 98.03.02 */

    if (rnum == 0 || rnum != rnum2)
        logout_user(recunum);
    else
        enter_room(recunum, mainroom, (char *) NULL);
}

void
chat_kickid(unum, msg)/*Haohmaru.99.6.9.����ID����*/
int unum;
char *msg;
{
    char *twit;
    int rnum = users[unum].room, rnum2;
    int recunum;

    twit = getnextword(&msg);
    if (!ROOMOP(unum) && !SYSOP(unum) && !CHATOP(unum))
    {
        send_to_unum(unum, msg_not_op);
        return;
    }
    if ((recunum = userid_to_indx(twit)) == -1)
    {
        sprintf(chatbuf, msg_no_such_id, twit);
        send_to_unum(unum, chatbuf);
        return;
    }
    if (rnum != users[recunum].room && !SYSOP(unum))
    {
        sprintf(chatbuf, msg_not_here, users[recunum].userid);
        send_to_unum(unum, chatbuf);
        return;
    }
    if (SYSOP(recunum) && !SYSOP(unum))
    {
        send_to_unum(unum, msg_not_op);
        return;
    }

    /* Leeward 98.04.30 */
    sprintf(chatbuf, "*** �㱻����([1m[33m%s[m)�߳����� ***", users[unum].userid);
    send_to_unum(recunum, chatbuf);

    rnum2=users[recunum].room;

    /*exit_room(recunum, EXIT_KICK, (char *) NULL);*/
    exit_room(recunum, EXIT_KICK, users[unum].userid); /* Leeward 98.03.02 */

    if (rnum == 0 || rnum != rnum2)
        logout_user(recunum);
    else
        enter_room(recunum, mainroom, (char *) NULL);
}

void
chat_makeop(unum, msg)
int unum;
char *msg;
{
    char *newop = getnextword(&msg);
    int rnum = users[unum].room;
    int recunum;

    if (!ROOMOP(unum))
    {
        send_to_unum(unum, msg_not_op);
        return;
    }
    if ((recunum = chatid_to_indx(newop)) == -1)
    {
        /* no such user */
        sprintf(chatbuf, msg_no_such_id, newop);
        send_to_unum(unum, chatbuf);
        return;
    }
    if (unum == recunum)
    {
        sprintf(chatbuf, "�� �������㱾�����Ƕ���� ��");
        send_to_unum(unum, chatbuf);
        return;
    }
    if (rnum != users[recunum].room)
    {
        sprintf(chatbuf, msg_not_here, users[recunum].chatid);
        send_to_unum(unum, chatbuf);
        return;
    }
    users[unum].flags &= ~PERM_CHATROOM;
    users[recunum].flags |= PERM_CHATROOM;
    sprintf(chatbuf, "*** [1m%s[m ������ [1m%s[m �������Ķ��� ***", users[unum].chatid,
            users[recunum].chatid);
    send_to_room(rnum, chatbuf, unum);
}

void
chat_toggle(unum, msg)
int unum;
char *msg;
{
    char *togglee = getnextword(&msg);
    int rnum = users[unum].room;
    int recunum;

    if (!SYSOP(unum) && !CHATOP(unum))
    {
        send_to_unum(unum, msg_not_op);
        return;
    }
    if ((recunum = chatid_to_indx(togglee)) == -1)
    {
        /* no such user */
        sprintf(chatbuf, msg_no_such_id, togglee);
        send_to_unum(unum, chatbuf);
        return;
    }
    if (rnum != users[recunum].room)
    {
        sprintf(chatbuf, msg_not_here, users[recunum].chatid);
        send_to_unum(unum, chatbuf);
        return;
    }
    if (ROOMOP(recunum))
    {
        users[recunum].flags &= ~PERM_CHATROOM;
        sprintf(chatbuf, "*** [1m%s[m ȡ���� [1m%s[m ��op ***", users[unum].chatid,
                users[recunum].chatid);
    }
    else
    {
        users[recunum].flags |= PERM_CHATROOM;
        sprintf(chatbuf, "*** [1m%s[m ���� [1m%s[m Ϊ�����Ķ��� ***", users[unum].chatid,
                users[recunum].chatid);
    }
    send_to_room(rnum, chatbuf, unum);
}

void
chat_invite(unum, msg)
int unum;
char *msg;
{
    char *invitee = getnextword(&msg);
    int rnum = users[unum].room;
    int recunum;

    if (!ROOMOP(unum))
    {
        send_to_unum(unum, msg_not_op);
        return;
    }
    if ((recunum = chatid_to_indx(invitee)) == -1)
    {
        sprintf(chatbuf, msg_not_here, invitee);
        send_to_unum(unum, chatbuf);
        return;
    }
    if (rooms[rnum].invites[recunum] == 1)
    {
        sprintf(chatbuf, "*** [1m%s[m ��һ�¾��� ***", users[recunum].chatid);
        send_to_unum(unum, chatbuf);
        return;
    }
    rooms[rnum].invites[recunum] = 1;
    sprintf(chatbuf, "*** [1m%s[m �������� [1m%s[m ������ ***",
            users[unum].chatid, rooms[rnum].name);
    send_to_unum(recunum, chatbuf);
    sprintf(chatbuf, "*** [1m%s[m ��һ�¾��� ***", users[recunum].chatid);
    send_to_unum(unum, chatbuf);
}

/*---	Added by period		2000-09-15	---*/
/*---	NOTICE : send_to_room in SMTH codes need 3 parameters !!!	---*/
void chat_knock_room(int unum, char * msg)
{
    char *roomid;
    const char * outd = "����";
    const char * ind = "����";
    int recunum, rnum;

    roomid = getnextword(&msg);
    if (RESTRICTED(unum)) {
        send_to_unum(unum, "\033[1;31m�� \033[37m��ֻ������������ \033[31m��\033[m");
        return;
    }
    if (*roomid == '\0') {
        send_to_unum(unum, "\033[1;31m�� \033[37m�����ĸ��� \033[31m��\033[m");
        return;
    }
    rnum = roomid_to_indx(roomid);
    if (rnum == -1) {
        send_to_unum(unum, "\033[1;31m�� \033[37m û������� \033[31m��\033[m");
        return;
    }
    if ((SECRET(rnum)||NOEMOTE(rnum)) && !SYSOP(unum) && !CHATOP(unum)) {
        send_to_unum(unum, "\033[1;31m�� \033[37m ������ţ�лл������\033[31m��\033[m");
        return;
    }
    sprintf(chatbuf, "\033[1;37m�� \033[31m������... \033[33m%s [%s] \033[37m��%s���� : \033[32m%s \033[37m��\033[m",
            users[unum].chatid, users[unum].userid,
            (rnum == users[unum].room)?(outd):(ind), (msg));
    send_to_room(rnum, chatbuf, unum);
    if(rnum != users[unum].room) send_to_unum(unum, chatbuf);
    return;
}
/*---	End of Addition	---*/
void
chat_broadcast(unum, msg)
int unum;
char *msg;
{
    if (!SYSOP(unum) && !CHATOP(unum))
    {
        send_to_unum(unum, "�� �㲻��������˿���ڴ������� ��");
        return;
    }
    if (*msg == '\0')
    {
        send_to_unum(unum, "�� �㲥������ʲ�� ��");
        return;
    }
    sprintf(chatbuf, "�� ��˿������Ա [1m%s[m �л��԰���������",

            users[unum].chatid);
    send_to_room(ROOM_ALL, chatbuf, unum);
    sprintf(chatbuf, "��%s��", msg);
    send_to_room(ROOM_ALL, chatbuf, unum);
}


void
chat_goodbye(unum, msg)
int unum;
char *msg;
{
    exit_room(unum, EXIT_LOGOUT, msg);
}


/* -------------------------------------------- */
/* MUD-like social commands : action             */
/* -------------------------------------------- */


struct action
{
    char *verb;                   /* ���� */
    char *part1_msg;              /* ��� */
    char *part2_msg;              /* ���� */
};


struct action party_data[] =
    {
        {"?", "���ɻ�Ŀ���", ""},
        {"admire", "��", "�ľ���֮���������Ͻ�ˮ���಻��"},
        {"agree","��ȫͬ��","�Ŀ���"},
        {"bearhug", "�����ӵ��", ""},
        {"bless", "ף��", "�����³�"},
        {"bow", "�Ϲ��Ͼ�����", "�Ϲ�"},
        {"bye", "����","�ı�Ӱ����Ȼ���¡����������������������ĸ���:\\n\"[31m���վ�����.....[m\""},
        {"caress", "����ĸ���", ""},
        {"cat", "��ֻСè���������", "�Ļ���������"},
        {"cringe", "��", "������ϥ��ҡβ����"},
        {"cry", "��", "�������"},
        {"comfort", "���԰�ο", ""},
        {"clap", "��", "���ҹ���"},
        {"dance", "����", "������������"},
        {"dogleg", "��", "����"},
        {"drivel", "����", "����ˮ"},
        {"dunno","�ɴ��۾���������ʣ�","����˵ʲ���Ҳ���Ү... :("},
        {"faint", "�ε���", "�Ļ���"},
        {"fear", "��", "¶�����µı���"},
        {"fool", "�����ע��","�����׳�....\\n������������....�˼����Ļ....\\n����̫��ϧ�ˣ�"},
        {"forgive","��ȵĶ�","˵�����ˣ�ԭ������"},
        {"giggle", "����", "ɵɵ�Ĵ�Ц"},
        {"grin", "��", "¶��а���Ц��"},
        {"growl", "��", "��������"},
        {"hand", "��", "����"},
        {"hammer","����ô�ô�����������ۣ���","ͷ������һ�ã�\\n***************\\n*  5000000 Pt *\\n***************\\n      | |      %1��%2��%3��%4��%5��%6��%0\\n      | |         �ö������Ӵ\\n      |_|"},
        {"heng","��������","һ�ۣ� ����һ�����߸ߵİ�ͷ��������,��мһ�˵�����..."},
        {"hug", "�����ӵ��", ""},
        {"idiot", "����س�Ц", "�ĳմ���"},
        {"kick", "��", "�ߵ���ȥ����"},
        {"kiss", "����", "������"},
        {"laugh", "������Ц", ""},
        {"lovelook", "����","���֣������ĬĬ���ӡ�Ŀ�����к���ǧ�����飬�������"},
        {"nod", "��", "��ͷ����"},
        {"nudge", "�����ⶥ", "�ķʶ���"},
        {"oh","��","˵����Ŷ�����Ӱ�����"},
        {"pad", "����", "�ļ��"},
        {"papaya", "������", "��ľ���Դ�"},
        {"pat", "��������", "��ͷ"},
        {"pinch", "�����İ�", "š�ĺ���"},
        {"puke", "����", "�°��°�����˵�¶༸�ξ�ϰ����"},
        {"punch", "�ݺ�����", "һ��"},
        {"pure", "��", "¶�������Ц��"},
        {"qmarry", "��","�¸ҵĹ�������:\\n\"��Ը��޸�����\"\\n---���������ɼΰ�"},
        {"report","͵͵�ض�","˵���������Һ��𣿡�"},
        {"shrug", "���ε���", "�����ʼ��"},
        {"sigh", "��", "̾��һ����"},
        {"slap", "žž�İ���", "һ�ٶ���"},
        {"smooch", "ӵ����", ""},
        {"snicker", "�ٺٺ�..�Ķ�", "��Ц"},
        {"sniff", "��", "��֮�Ա�"},
        {"sorry", "ʹ�����������","ԭ��"},
        {"spank", "�ð��ƴ�", "���β�"},
        {"squeeze", "������ӵ����", ""},
        {"thank", "��", "��л"},
        {"tickle", "��ߴ!��ߴ!ɦ", "����"},
        {"waiting", "����ض�", "˵��ÿ��ÿ�µ�ÿһ�죬ÿ��ÿ���Ҷ������������"},
        {"wake","Ŭ����ҡҡ","��������ߴ�У��������ѣ��������ģ���"},
        {"wave", "����", "ƴ����ҡ��"},
        {"welcome", "���һ�ӭ", "�ĵ���"},
        {"wink", "��", "���ص�գգ�۾�"},
        {"xixi","�����ض�","Ц�˼���"},
        {"zap", "��", "���Ĺ���"},
        {"inn", "˫�۱�������ˮ���޹�������", ""},
        {"mm", "ɫ���еĶ�", "�ʺã�����ü�á�������������ɫ�ǰ�������"},
        {"disapp","����û��ͷ����Ϊʲô","����������������ȫû��Ӧ��û�취��"},
        {"miss","��ϵ�����","�����������������������̫--��������!���಻����?"},
        {"buypig","ָ��","���������ͷ������һ�룬лл����"},
        {"rascal","��","��У������������å����������������������������������"},
        {"qifu","С��һ�⣬��","�޵��������۸��ң����۸��ң�������"},
        {"wa","��","���һ�����������������ۿ����Ү������������������������������"},
        {"feibang","ร���������죬","�������ҽ�������һ�����Ը���ٰ�������"},
        {NULL, NULL, NULL}
  };
  
int
alias_action(unum, cmd, party)
int unum;
char *cmd;
char *party;
{
    int i, len, chatlen,recunum;
    char ch,*tmpbuf,*tmpbuf2,*party2,*party3;
    len=strlen(cmd);
    for (i = 0; i<MAX_EMOTES; i++)
    {
        if (users[unum].lpEmote[i][0]=='\0') continue;
        if (!strncasecmp(cmd, users[unum].lpEmote[i], len)
                && users[unum].lpEmote[i][len]==' ')
        {
            if (*party == '\0')
            {
                party = "����";
                party2=NULL;
            }
            else
            { party2=party;
                party=getnextword(&party2);
                recunum = fuzzy_chatid_to_indx(party);
                if (recunum >= 0)
                    party = users[recunum].chatid;
            }
            sprintf(chatbuf, "\033[1m%s\033[m ",users[unum].chatid);
            tmpbuf=(users[unum].lpEmote[i]+len+1);
            recunum=-1;
            while(1)
            {
                tmpbuf2=strstr(tmpbuf,"%s");
                chatlen=strlen(chatbuf);
                if (tmpbuf2==NULL)
                {
                    if (chatlen+strlen(tmpbuf)+5<256)
                        sprintf(chatbuf+strlen(chatbuf),"%s\033[0m",tmpbuf);
                    break;
                }
                if (recunum>=0)
                    if (party2!=NULL)
                    {
                        party3=getnextword(&party2);
                        recunum = fuzzy_chatid_to_indx(party3);
                        if (recunum>=0)
                            party3=users[recunum].chatid;
                        else if (*party3!=' '&& *party3!='\0') recunum=0;
                    }
                    else recunum=-1;
                ch=*tmpbuf2;
                *tmpbuf2='\0';
                if (strlen(tmpbuf)+strlen(recunum<0?party:party3)+chatlen+9>255)
                {
                    *tmpbuf2=ch; break;
                }
                sprintf(chatbuf+strlen(chatbuf),"%s\033[1m%s\033[m",tmpbuf,recunum<0?party:party3);
                *tmpbuf2=ch;
                tmpbuf=tmpbuf2+2;
                recunum=0;
            }
            /* chang to send_to_room2 call */
            send_to_room2(users[unum].room, chatbuf, unum);
            return 0;
        }
    }
    return 1;
}

int
party_action(unum, cmd, party)
int unum;
char *cmd;
char *party;
{
    int i;
    for (i = 0; party_data[i].verb; i++)
    {
        if (!strcasecmp(cmd, party_data[i].verb))
        {
            if (*party == '\0')
            {
                party = "����";
            }
            else
            {
                int recunum = fuzzy_chatid_to_indx(party);
                if (recunum < 0)
                {
                    /* no such user, or ambiguous */
                    if (recunum == -1)
                        sprintf(chatbuf, msg_no_such_id, party);
                    else
                        sprintf(chatbuf, "�� �����ļ䶴 ��", party);
                    send_to_unum(unum, chatbuf);
                    return 0;
                }
                party = users[recunum].chatid;
            }
            if (strcasecmp(cmd, "faint")==0 &&
                    (strcasecmp(party, users[unum].chatid)==0 ||
                     strcmp(party, "����")==0))
                sprintf(chatbuf, "\033[1m%s\033[m %sһ�����ε��ڵ�...\033[0m",users[unum].chatid, (strcasecmp(users[unum].userid,"roberto")==0||strcasecmp(users[unum].chatid,"roberto")==0)?"��ͨ":"�۵�");
            else {
                if (strcasecmp(party, users[unum].chatid)==0)
                    party="�Լ�";
                sprintf(chatbuf, "[1m%s[m %s [1m%s[m %s[0m",
                        users[unum].chatid,
                        party_data[i].part1_msg, party, party_data[i].part2_msg);
            }
            send_to_room2(users[unum].room, chatbuf, unum);
            return 0;
        }
    }
    return 1;
}


/* -------------------------------------------- */
/* MUD-like social commands : speak              */
/* -------------------------------------------- */


struct action speak_data[] =
    {
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


int
speak_action(unum, cmd, msg)
int unum;
char *cmd;
char *msg;
{
    int i;

    for (i = 0; speak_data[i].verb; i++)
    {
        if (!strcasecmp(cmd, speak_data[i].verb))
        {
            sprintf(chatbuf, "[1m%s[m %s��%s[0m",
                    users[unum].chatid, speak_data[i].part1_msg, msg);
            send_to_room(users[unum].room, chatbuf, unum);
            return 0;
        }
    }
    return 1;
}


/* -------------------------------------------- */
/* MUD-like social commands : condition          */
/* -------------------------------------------- */


struct action condition_data[] =
    {
        {":D", "�ֵĺϲ�£��", NULL},
        {":)", "�ֵĺϲ�£��", NULL},
        {":P", "�ֵĺϲ�£��", NULL},
        {":(", "�ֵĺϲ�£��", NULL},
        {"applaud", "žžžžžžž....", NULL},
        {"blush", "��������", NULL},
        {"cough", "���˼���", NULL},
        {"faint", "�۵�һ�����ε��ڵ�", NULL},
        {"happy", "������¶�����Ҹ��ı��飬��ѧ�Ա��˵���ߺ�������", NULL},
        {"lonely", "һ�������ڷ��������������ϣ��˭�����㡣������",NULL},
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


int
condition_action(unum, cmd)
int unum;
char *cmd;
{
    int i;

    for (i = 0; condition_data[i].verb; i++)
    {
        if (!strcasecmp(cmd, condition_data[i].verb))
        {
            sprintf(chatbuf, "[1m%s[m %s[0m",
                    users[unum].chatid, condition_data[i].part1_msg);
            send_to_room(users[unum].room, chatbuf, unum);
            return 1;
        }
    }
    return 0;
}


/* -------------------------------------------- */
/* MUD-like social commands : help               */
/* -------------------------------------------- */

char *dscrb[] = {
    "�� Verb + Nick��   ���� + �Է����� ��   ����//kick piggy",
    "�� Verb + Message������ + Ҫ˵�Ļ� ��   ����//sing ��������",
    "�� Verb������ ��    �������ɻ�����", NULL};
struct action *verbs[] = {party_data, speak_data, condition_data, NULL};


#define SCREEN_WIDTH    80
#define MAX_VERB_LEN    10
#define VERB_NO         8

void
view_action_verb(unum,verb)
int unum;
char* verb;
{
    int i, j, all, count;
    char *p,*tmp;

    /* add by KCN for list all emote */
    count=1;
    all=0;
    if (verb) {
        if (!strcmp(verb,"all"))
            all=1;
    }
    if ((!verb)||all)
        send_to_unum(unum, "/c");

    for (i = 0; dscrb[i]; i++)
    {
        if (!verb)
            send_to_unum(unum, dscrb[i]);
        chatbuf[0] = '\0';
        j = 0;
        while (p = verbs[i][j++].verb)
        {
            if (!verb)
                strcat(chatbuf, p);
            else
                /* add by KCN */
                if (all) {
                    if (i==0)
                        sprintf(chatbuf, "[1m%-8s: [m %s [1m����[m %s[0m", verbs[i][j-1].verb,
                                party_data[j-1].part1_msg, party_data[j-1].part2_msg);
                    else if (i==1)
                        sprintf(chatbuf, "[1m%-8s: [m %s��%s[0m", verbs[i][j-1].verb,
                                speak_data[j-1].part1_msg, "�Ұ������㰮��!");
                    else
                        sprintf(chatbuf, "[1m%-8s: [m %s[0m",verbs[i][j-1].verb,
                                condition_data[j-1].part1_msg);

                    tmp=chatbuf;
                    while (tmp=strstr(tmp,"\\n")){
                        count++;
                        tmp++;
                    };
                    count++;

                    send_to_unum2(unum,chatbuf);
                    continue;
                }
            if (verb)
                if (!strcmp(p,verb))
                    break;

            if ((j % VERB_NO) == 0)
            {
                if (!verb)
                    send_to_unum(unum, chatbuf);
                chatbuf[0] = '\0';
            }
            else
            {
                strncat(chatbuf, "        ", MAX_VERB_LEN - strlen(p));
            }
        }
        if (!verb) {
            if (j % VERB_NO)
                send_to_unum(unum, chatbuf);
        } else
            if ((!all)&&p) break;
    }
    /* add by KCN */
    if (!verb)
        send_to_unum(unum, "��//help ����,���Կ���������˵��,//help all�г����ж���ʾ��");
    else if (!all)
        if (!p) {
            send_to_unum(unum,"û�����������");
        } else {
            sprintf(chatbuf, "����%s��ʾ��:",verb);
            send_to_unum(unum,chatbuf);
            j--;
            if (i==0)
                sprintf(chatbuf, "[1m%s[m %s [1m����[m %s[0m", users[unum].chatid,
                        party_data[j].part1_msg, party_data[j].part2_msg);
            else if (i==1)
                sprintf(chatbuf, "[1m%s[m %s��%s[0m",
                        users[unum].chatid, speak_data[j].part1_msg, "�Ұ������㰮��!");
            else
                sprintf(chatbuf, "[1m%s[m %s[0m",
                        users[unum].chatid, condition_data[j].part1_msg);
            send_to_unum2(unum,chatbuf);
        }
}


struct chatcmd chatcmdlist[] =
    {
        "act", chat_act, 0, 0,
        "bye", chat_goodbye, 0, 1,
        "exit", chat_goodbye, 0, 1,
        "flags", chat_setroom, 0, 1,
        "invite", chat_invite, 0, 0,
        "join", chat_join, 0, 1,
        "kick", chat_kick, 0,  1,
        "kid", chat_kickid, 0, 1,/*Haohmaru.99.4.6*/
        "msg", chat_private, 0, 1,
        "nick", chat_nick, 0, 0,
        "operator", chat_makeop, 0, 1,
        "rooms", chat_list_rooms, 0, 1,
        "whoin", chat_list_by_room, 1, 1,
        "wall", chat_broadcast, 1, 1,
        "rname", chat_name_room, 1, 1,       /* added by Luzi 1998.1.16 */
        "who", chat_map_chatids_thisroom, 0, 1,
        "list", chat_list_users, 0, 1,
        "topic", chat_topic, 0, 1,
        "toggle", chat_toggle, 0, 1,
        "me", chat_act, 0, 0,
        "q", chat_query, 1, 1,               /* exect flag from 0 to 1, modified by dong, 1998.9.12 */
        "qc", chat_query_ByChatid,1,1,       /* added by dong 1998.9.12 */
        "ignore",chat_ignore,1,1,            /* added by Luzi 1997.11.30 */
        "listen",chat_listen,1,1,            /* added by Luzi 1997.11.30 */
        "alias_add",chat_alias_add,1,1,      /* added by Luzi 1998.01.25 */
        "alias_del",chat_alias_del,1,1,      /* added by Luzi 1998.01.25 */
        "knock", chat_knock_room, 0, 1,      /* added by period 2000-09-15 */
        NULL, NULL, 0,0
    };


int
command_execute(unum)
int unum;
{
    char *msg = users[unum].ibuf;
    char *cmd;
    struct chatcmd *cmdrec;
    int match = 0;

    /* Validation routine */
    if (users[unum].room == -1)
    {
        /* MUST give special /! command if not in the room yet */
        if (msg[0] != '/' || msg[1] != '!')
            return -1;
        else
            return (login_user(unum, msg + 2));
    }


    /* If not a /-command, it goes to the room. */
    if (msg[0] != '/')
    {
        if (users[unum].room==0 && !ENABLEMAIN) /* added bye Luzi 1998.1.3 */
            send_to_unum(unum, "�� �������ֹ���� ��");
        else chat_allmsg(unum, msg);
        return 0;
    }

    msg++;
    cmd = getnextword(&msg);
    if (cmd[0] == '/')
    {

        if (!strcasecmp(cmd + 1, "help") || (cmd[1] == '\0'))
        {
            /* add by KCN */
            if (cmd[1]&&cmd[6]) {
                /*
                      char* verb;
                      verb=getnextword(cmd+1);
                */
                view_action_verb(unum,cmd+6);
            } else
                view_action_verb(unum,0);
            match = 1;
        }
        else if (users[unum].room==0 && !ENABLEMAIN)  /* added by Luzi 1998.1.3*/
        {
            send_to_unum(unum, "�� �����ﱣ���ྲ ��");
            match=1;
        }  else if (NOEMOTE(users[unum].room))
        {
            send_to_unum(unum, "�� ������ֹ��С���� ��");
            match=1;
        }
        else if (alias_action(unum, cmd + 1, msg) ==0 )
            match = 1;
        else if (party_action(unum, cmd + 1, msg) == 0)
            match = 1;
        else if (speak_action(unum, cmd + 1, msg) == 0)
            match = 1;
        else
            match = condition_action(unum, cmd + 1);
    }
    else
    {
        if (!strncasecmp(cmd, "shutdown",8))
        {
            if (SYSOP(unum)) {
                match=-1; /* SYSOP����ִ��shutdown���� */
            }
        }
        else
        {
            for (cmdrec = chatcmdlist; !match && cmdrec->cmdstr; cmdrec++)
            {
                if (cmdrec->exact)
                    match = !strcasecmp(cmd, cmdrec->cmdstr);
                else
                    match = !strncasecmp(cmd, cmdrec->cmdstr, strlen(cmd));
                if (match)
                    if (ENABLEMAIN || users[unum].room || cmdrec->bUsed)/* added by Luzi 98.1.3 */
                        if (NOEMOTE(users[unum].room) &&
                                (cmd[0]=='a' || cmd[0]=='A' ||
                                 ((cmd[0]=='m' || cmd[0]=='M') && (cmd[1]=='e' || cmd[1]=='E'))))
                            send_to_unum(unum, "�� ������ֹ��С���� ��");
                        else cmdrec->cmdfunc(unum, msg);
                    else send_to_unum(unum, "�� �������ֹʹ�ø�ָ�� ��");
            }
        }
    }

    if (match==0)
    {
        sprintf(chatbuf, "�� ϵͳ�����������˼��[1m%s[m ��", cmd);
        send_to_unum(unum, chatbuf);
    }
    memset(users[unum].ibuf, 0, sizeof users[unum].ibuf);
    if (match==-1) return 1;
    else return 0;
}


int
process_chat_command(unum)
int unum;
{
    register int i;
    int rc, ibufsize;
    short nFlag;
    if ((rc = recv(users[unum].sockfd, chatbuf, sizeof chatbuf, 0)) <= 0)
    {
        /* disconnected */
        exit_room(unum, EXIT_LOSTCONN, (char *) NULL);
        return -1;
    }
    ibufsize = users[unum].ibufsize;
    for (i = 0; i < rc; i++)
    {
        /* if newline is two characters, throw out the first */
        if (chatbuf[i] == '\r')
            continue;

        /* carriage return signals end of line */
        else if (chatbuf[i] == '\n')
        {
            users[unum].ibuf[ibufsize] = '\0';
            nFlag=command_execute(unum);
            if (nFlag) return nFlag;
            ibufsize = 0;
        }


        /* add other chars to input buffer unless size limit exceeded */
        else
        {
            if (ibufsize < 126)
                users[unum].ibuf[ibufsize++] = chatbuf[i];
            else {
                users[unum].ibuf[ibufsize] = '\0';
                nFlag=command_execute(unum);
                if (nFlag) return nFlag;
                ibufsize = 0;
                /* Luzi���ӣ�����Խ�����ݣ��Զ����� 1999/1/8 */
            }
        }
    }
    users[unum].ibufsize = ibufsize;

    return 0;
}

int
main(argc,argv)
int argc;
char* argv[];
{
    struct sockaddr_in sin;
    register int i;
    int pid, sr, newsock,nFlag=0;
    long  sinsize;
    fd_set readfds;
    struct timeval *tvptr = NULL;
    /* ����ΪLuzi���ӣ���ֹ���ⲿbbs���� disable by KCN
      char inbuf[80];
      struct hostent *h;
      gethostname(inbuf, STRLEN);
      if (!(h = gethostbyname(inbuf)))
      {
        perror("gethostbyname");
        return -1;
      }
      memset(inbuf, 0, 80);
      memcpy(inbuf, h->h_addr, h->h_length);
    ����ΪLuzi���Ӵ��� 1998.3.20 */
    /* ----------------------------- */
    /* init variable : rooms & users */
    /* ----------------------------- */
    strcpy(rooms[0].name, mainroom);
    /*strcpy(rooms[0].topic, maintopic);*/

    /* Leeward: 98.01.04: �趨 main ��ȱʡ����������ʾ(No chatting...);
                          ͬʱע��: Ϊ��� chat.c, ���ɼ� [m �ڱ���ĩβ */
    if (ENABLEMAIN)
        strcpy(rooms[0].topic, "\033[1m��˿����������ˮ������...");
    else
        strcpy(rooms[0].topic, "[1mNo chatting in main room, type /h for help");

    for (i = 0; i < CHATMAXACTIVE; i++)
    {
        users[i].chatid[0] = '\0';
        users[i].sockfd = users[i].utent = -1;
    }

    /* ------------------------------ */
    /* bind chat server to port       */
    /* ------------------------------ */

    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        return -1;
    }
    nFlag = 1;
    setsockopt(sock,SOL_SOCKET,SO_REUSEADDR,(char*)&nFlag,sizeof(nFlag));

    sin.sin_family = AF_INET;
    if (argc>1)
        sin.sin_port = htons(CHATPORT2);
    else
        sin.sin_port = htons(CHATPORT3);
    /* change by KCN 1999.10.22
      sin.sin_addr.s_addr = INADDR_ANY;
    */
    sin.sin_addr.s_addr = htonl(INADDR_LOOPBACK);

    if (bind(sock, (struct sockaddr *) & sin, sizeof(sin)) < 0)
    {
        return -1;
    }

    sinsize = sizeof sin;
    if (getsockname(sock, (struct sockaddr *)&sin, &sinsize) == -1)
    {
        perror("getsockname");
        exit(1);
    }

    if (listen(sock, 5) == -1)
    {
        perror("listen");
        exit(1);
    }


    if (fork())
    {
        return (0);
    }
    setpgid(0, 0);

    /* ------------------------------ */
    /* trap signals                   */
    /* ------------------------------ */

    signal(SIGHUP, SIG_IGN);
    signal(SIGINT, SIG_IGN);
    signal(SIGQUIT, SIG_IGN);
    signal(SIGPIPE, SIG_IGN);
    signal(SIGALRM, SIG_IGN);
    signal(SIGTERM, SIG_IGN);
    signal(SIGURG, SIG_IGN);
    signal(SIGTSTP, SIG_IGN);
    signal(SIGTTIN, SIG_IGN);
    signal(SIGTTOU, SIG_IGN);


    FD_ZERO(&allfds);
    FD_SET(sock, &allfds);
    nfds = sock + 1;

    while (1)
    {
        memcpy(&readfds, &allfds, sizeof readfds);

        if ((sr = select(nfds, &readfds, NULL, NULL, tvptr)) < 0)
        {
            if (errno == EINTR)
                continue;
            exit(-1);
        }

        if (sr == 0)
        {
            goto SHUTDOWN;                  /* normal chat server shutdown */
        }

        if (tvptr)
            tvptr = NULL;

        if (FD_ISSET(sock, &readfds))
        {
            sinsize = sizeof sin;
            newsock = accept(sock, (struct sockaddr *)&sin, &sinsize);
            if (newsock == -1)
            {
                continue;
            }
            /* Luzi 1998.3.20 �ж������Ƿ����Ա��� */
            /* disable by KCN 1999.10.22
                  if (strncmp((char*)&sin.sin_addr, inbuf, h->h_length)!=0)
                  {                   
                   close(newsock);
                   continue;
                  }
            */
            for (i = 0; i < CHATMAXACTIVE; i++)
            {
                if (users[i].sockfd == -1)
                {
                    users[i].sockfd = newsock;
                    users[i].room = -1;
                    break;
                }
            }

            if (i >= CHATMAXACTIVE)
            {
                /* full -- no more chat users */
                close(newsock);
            }
            else
            {

#if !RELIABLE_SELECT_FOR_WRITE
                int flags = fcntl(newsock, F_GETFL, 0);
                flags |= O_NDELAY;
                fcntl(newsock, F_SETFL, flags);
#endif

                FD_SET(newsock, &allfds);
                if (newsock >= nfds)
                    nfds = newsock + 1;
                num_conns++;
            }
        }

        for (i = 0; i < CHATMAXACTIVE; i++)
        {
            /* we are done with newsock, so re-use the variable */
            newsock = users[i].sockfd;
            if (newsock != -1 && FD_ISSET(newsock, &readfds))
            {
                nFlag=process_chat_command(i);
                if (nFlag == -1)
                {
                    logout_user(i);
                }
                if (nFlag == 1) goto SHUTDOWN;
            }
        }
        /*    if (num_conns <= 0)
            {
              tvptr = &zerotv;
            } */
    }
    /* NOTREACHED */

SHUTDOWN:
    close(sock);
    for (i = 0; i < CHATMAXACTIVE; i++)
    {
        /* we are done with newsock, so re-use the variable */
        newsock = users[i].sockfd;
        if (newsock != -1)
            shutdown(newsock,3);
        close(newsock);
    }
    exit(0);
}

