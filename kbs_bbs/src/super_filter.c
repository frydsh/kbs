#include "bbs.h"
#include <math.h>

#define MAX_FVAR 100
#define LIBLEN 1000

int ferr=0;

struct fvar_struct {
    bool num;
    char name[8];
    int s;
    char * p;
};

struct fvar_struct fvars[MAX_FVAR];
int fvart = 0;
char * libs, * libptr;

#define fmakesure(o,p) if(ferr) return;\
    if(!(o)) {ferr=p; return;}

extern struct boardheader* currboard;
extern char currdirect[255];
extern int digestmode;

int fget_var(char * name)
{
    int i;
    if(ferr) return 0;
    if(!name[0]||strlen(name)>7) {
        ferr=14;
        return 0;
    }
    for(i=0;i<fvart;i++)
        if(!strncasecmp(fvars[i].name, name, 8)) {
            return i;
        }
    if(fvart>=MAX_FVAR) {
        ferr=15;
        return 0;
    }
    strncpy(fvars[fvart].name, name, 8);
    fvars[fvart].p = 0;
    fvars[fvart].num = true;
    fvart++;
    return (fvart-1);
}

void set_vard(struct fvar_struct * a, int f)
{
    a->num = true;
    a->s = f;
}

void set_vars(struct fvar_struct * a, char * f)
{
    a->num = false;
    a->p = f;
}

int fcheck_var_name(char * s, int l)
{
    int i,p=1;
    for(i=0;i<l;i++)
        p=p&&(isalpha(s[i]));
    return p;
}

int check_var_int(char * s, int l)
{
    int i,p=1;
    for(i=0;i<l;i++)
        p=p&&(isdigit(s[i]));
    return p;
}

int get_rl(char * s, int r, int l)
{
    int n=r,i=0;
    do{
        if(s[n]==')') i++;
        if(s[n]=='(') i--;
        n--;
    }while(i!=0&&n>=l);
    if(i==0) return n+1;
    else return -1;
}

int get_rl2(char * s, int r, int l)
{
    int n=r,i=0;
    do{
        if(s[n]=='\'') i++;
        if(s[n]=='"') i++;
        if(i==2) return n;
        n--;
    }while(n>=l);
    return -1;
}

void feval(struct fvar_struct * p, char * s, int l, int r)
{
    int i,j,n;
    char op[13][4]={"&&","||","==","!=",">=","<=",">","<","+","-","*","/","%"};
    struct fvar_struct * t,q;
    char buf[1000];
    while(s[l]==' '&&l<=r) l++;
    while(s[r]==' '&&l<=r) r--;
    fmakesure(l<=r,11);
    while(s[l]=='('&&s[r]==')'&&get_rl(s,r,l)==l) {
        l++; r--;
        while(s[l]==' '&&l<=r) l++;
        while(s[r]==' '&&l<=r) r--;
    }
    if(fcheck_var_name(s+l, r-l+1)) {
        strncpy(buf, s+l, 1000);
        buf[r-l+1]=0;
        i=fget_var(buf);
        p->num = fvars[i].num;
        p->s = fvars[i].s;
        p->p = fvars[i].p;
        return;
    }
    if(check_var_int(s+l, r-l+1)) {
        int f;
        strncpy(buf, s+l, 1000);
        buf[r-l+1]=0;
        f = atoi(buf);
        set_vard(p, f);
        return;
    }
    if((s[l]=='\''||s[l]=='"')&&(s[r]=='\''||s[r]=='"')&&get_rl2(s,r,l)==l) {
        fmakesure(r-l+libptr<libs+LIBLEN,4);
        strncpy(libptr, s+l+1, r-l-1);
        libptr[r-l-1]=0;
        p->num=false;
        p->p=libptr;
        libptr+=r-l;
        return;
    }
    i=l;
    while(isalpha(s[i])&&i<=r) i++;
    if(i>l&&s[i]=='('&&s[r]==')'&&get_rl(s,r,l)==i) {
        struct fvar_struct u,v,w;
        u.p=0; v.p=0; w.p=0;
        strncpy(buf, s+l, 1000);
        buf[i-l]=0;
        if(!strcmp("sub",buf)) {
            int j=strchr(s+i+1, ',')-s;
            char * res;
            fmakesure(strchr(s+i+1, ',')!=NULL, 2);
            fmakesure(strchr(s+i+1, ',')<=s+r, 2);
            feval(&u, s, i+1, j-1);
            fmakesure(!u.num&&u.p, 2);
            feval(&v, s, j+1, r-1);
            fmakesure(!v.num&&v.p, 2);
            p->num=true;
            res = bm_strcasestr(v.p, u.p);
            if(res==NULL) p->s=0;
            else p->s=res-v.p+1;
            return;
        }
        else if(!strcmp("len",buf)){
            feval(&u, s, i+1, r-1);
            fmakesure(!u.num&&u.p, 3);
            p->num=true;
            p->s = strlen(u.p);
            return;
        }
        else if(!strcmp("date",buf)){
            int j=strchr(s+i+1, ',')-s, k;
            struct tm t;
            char * res;
            fmakesure(strchr(s+i+1, ',')!=NULL, 4);
            fmakesure(strchr(s+i+1, ',')<=s+r, 4);
            fmakesure(strchr(s+j+1, ',')!=NULL, 4);
            fmakesure(strchr(s+j+1, ',')<=s+r, 4);
            k=strchr(s+j+1, ',')-s;
            feval(&u, s, i+1, j-1);
            fmakesure(u.num, 4);
            feval(&v, s, j+1, k-1);
            fmakesure(v.num, 4);
            feval(&w, s, k+1, r-1);
            fmakesure(w.num, 4);
            p->num=true;
            t.tm_sec=0;
            t.tm_min=0;
            t.tm_hour=0;
            t.tm_mday=w.s;
            t.tm_mon=v.s-1;
            t.tm_year=u.s-1900;
            p->s = mktime(&t);
            return;
        }
        else if(!strcmp("today",buf)){
            struct tm t;
            time_t tt;
            char * res;
            p->num=true;
            tt=time(0);
            gmtime_r(&tt, &t);
            t.tm_sec=0;
            t.tm_min=0;
            t.tm_hour=0;
            p->s = mktime(&t);
            return;
        }
        else if(!strcmp("time",buf)){
            int j=strchr(s+i+1, ',')-s, k;
            struct tm t;
            char * res;
            fmakesure(strchr(s+i+1, ',')!=NULL, 4);
            fmakesure(strchr(s+i+1, ',')<=s+r, 4);
            fmakesure(strchr(s+j+1, ',')!=NULL, 4);
            fmakesure(strchr(s+j+1, ',')<=s+r, 4);
            k=strchr(s+j+1, ',')-s;
            feval(&u, s, i+1, j-1);
            fmakesure(u.num, 4);
            feval(&v, s, j+1, k-1);
            fmakesure(v.num, 4);
            feval(&w, s, k+1, r-1);
            fmakesure(w.num, 4);
            p->num=true;
            p->s = (u.s*60+v.s)*60+w.s;
            return;
        }
        ferr=18;
        return ;
    }
    for(j=0;j<13;j++) {
        n=r;
        do{
            if(toupper(s[n])==op[j][0]&&(!op[j][1]||toupper(s[n+1])==op[j][1])) {
                struct fvar_struct m1,m2,m3;
                m1.p=0; m2.p=0; m3.p=0;
                feval(&m1,s,l,n-1);
                if(j==2||j==3) {fmakesure(m1.num||!m1.num&&m1.p,1);}
                else {fmakesure(m1.num,1);}
                if(op[j][1]==0) feval(&m2,s,n+1,r);
                else feval(&m2,s,n+2,r);
                if(j==2||j==3) {fmakesure(m1.num&&m2.num||!m1.num&&!m2.num&&m2.p,1);}
                else {fmakesure(m2.num,1);}
                p->num=true;
                switch(j) {
                    case 0:
                        p->s=m1.s&&m2.s;
                        break;
                    case 1:
                        p->s=m1.s||m2.s;
                        break;
                    case 2:
                        if(m1.num) p->s=m1.s==m2.s;
                        else p->s=!strcasecmp(m1.p,m2.p);
                        break;
                    case 3:
                        if(m1.num) p->s=m1.s!=m2.s;
                        else p->s=strcasecmp(m1.p,m2.p);
                        break;
                    case 4:
                        p->s=m1.s>=m2.s;
                        break;
                    case 5:
                        p->s=m1.s<=m2.s;
                        break;
                    case 6:
                        p->s=m1.s>m2.s;
                        break;
                    case 7:
                        p->s=m1.s<m2.s;
                        break;
                    case 8:
                        p->s=m1.s+m2.s;
                        break;
                    case 9:
                        p->s=m1.s-m2.s;
                        break;
                    case 10:
                        p->s=m1.s*m2.s;
                        break;
                    case 11:
                        fmakesure(m2.s!=0, 6);
                        p->s=m1.s/m2.s;
                        break;
                    case 12:
                        fmakesure(m2.s!=0, 6);
                        p->s=m1.s%m2.s;
                        break;
                }
                return;
            }
            if(s[n]==')') n=get_rl(s,n,l);
            n--;
        }while(n>=l);
    }
    if(s[l]=='!') {
        struct fvar_struct m;
        m.p = 0;
        feval(&m, s, l+1, r);
        fmakesure(m.num,1);
        p->num=true;
        p->s=!m.s;
        return;
    }
    fmakesure(r-l+2+libptr<libs+LIBLEN,4);
    strncpy(libptr, s+l+1, r-l+1);
    libptr[r-l+1]=0;
    p->num=false;
    p->p=libptr;
    libptr+=r-l+2;
}

int super_filter(int ent, struct fileheader *fileinfo, char *direct)
{
    struct fileheader *ptr1;
    struct flock ldata, ldata2;
    int fd, fd2, size = sizeof(fileheader), total, i, count = 0;
    char olddirect[PATHLEN];
    char *ptr;
    struct stat buf;
    int mode=8, load_content=0, found=0, load_stat=0;
    extern int scr_cols;
    static char index[1024]="";

    clear();
    prints("                  ��ǿ����ѡ��\n\n");
    move(5,0);
    prints("����: no(���º�) m(m����) g(g����) b(m&&g) noreply(���ɻظ�) sign(���)\n"
           "      del(ɾ�����) attach(������) unread(δ��)\n"
           "      title(����) author(����)\n"
           "����: sub(s1,s2)��һ���ַ����ڵڶ����е�λ��,��������ڷ���0\n"
           "      len(s)�ַ�������\n"
           "����: ��Ҫ��ѯ����badд�ı����b������:\n"
           "              author=='bad'&&b\n"
           "      ��Ҫ��ѯ���в��ɻظ�����δ��������:\n"
           "              noreply&&unread\n"
           "      ��Ҫ��ѯ����1000-2000��Χ�ڴ�����������:\n"
           "              (no>=1000)&&(no<=2000)&&attach\n"
           "      ��Ҫ��ѯ���ⳤ����5-10֮�������:\n"
           "              len(title)>=5&&len(title)<=10\n"
           "      ��Ҫ��ѯ�����ﺬ��faint������:\n"
           "              sub('faint',title)\n"
           "      ��Ҫ��ѯ���������hehe����λ������������:\n"
           "              sub('hehe',title)==len(title)-3\n"
           "      ��Ҫ��ѯ......�Լ����ֲ��,hehe"
);
    multi_getdata(2, 0, scr_cols-1, "��������ʽ: ", index, 1020, 20, 0, 0);
    if(!index[0]) 
        return FULLUPDATE;
    load_content = (strstr(index, "content")!=NULL);
    load_stat = (strstr(index, "asize")!=NULL);
    if (digestmode==7||digestmode==8 ) {
        if (digestmode == 7 || digestmode == 8)
            unlink(currdirect);
        digestmode = 0;
        setbdir(digestmode, currdirect, currboard->filename);
    }
    setbdir(digestmode, olddirect, currboard->filename);
    digestmode = mode;
    setbdir(digestmode, currdirect, currboard->filename);
    if ((fd = open(currdirect, O_WRONLY | O_CREAT, 0664)) == -1) {
        bbslog("user", "%s", "recopen err");
        return FULLUPDATE;      /* �����ļ���������*/
    }
    ldata.l_type = F_WRLCK;
    ldata.l_whence = 0;
    ldata.l_len = 0;
    ldata.l_start = 0;
    if (fcntl(fd, F_SETLKW, &ldata) == -1) {
        bbslog("user", "%s", "reclock err");
        close(fd);
        return FULLUPDATE;      /* lock error*/
    }
    /* ��ʼ�������*/

    if ((fd2 = open(olddirect, O_RDONLY, 0664)) == -1) {
        bbslog("user", "%s", "recopen err");
        ldata.l_type = F_UNLCK;
        fcntl(fd, F_SETLKW, &ldata);
        close(fd);
        return FULLUPDATE;
    }
    fstat(fd2, &buf);
    ldata2.l_type = F_RDLCK;
    ldata2.l_whence = 0;
    ldata2.l_len = 0;
    ldata2.l_start = 0;
    fcntl(fd2, F_SETLKW, &ldata2);
    total = buf.st_size / size;

    if ((i = safe_mmapfile_handle(fd2, PROT_READ, MAP_SHARED, (void **) &ptr, (size_t*)&buf.st_size)) != 1) {
        if (i == 2)
            end_mmapfile((void *) ptr, buf.st_size, -1);
        ldata2.l_type = F_UNLCK;
        fcntl(fd2, F_SETLKW, &ldata2);
        close(fd2);
        ldata.l_type = F_UNLCK;
        fcntl(fd, F_SETLKW, &ldata);
        close(fd);
        return FULLUPDATE;
    }
    ptr1 = (struct fileheader *) ptr;
    libs = (char*)malloc(LIBLEN);
    for (i = 0; i < total; i++) {
        struct stat st;
        char* p;
        char ffn[80];
        int j;
        size_t fsize;
        libptr = libs;
        ferr = 0;
        set_vard(fvars+fget_var("no"), i+1);
        set_vard(fvars+fget_var("id"), ptr1->id);
        set_vard(fvars+fget_var("reid"), ptr1->reid);
        set_vard(fvars+fget_var("groupid"), ptr1->groupid);
        set_vard(fvars+fget_var("origin"), ptr1->id==ptr1->groupid);
        set_vard(fvars+fget_var("m"), ptr1->accessed[0]&FILE_MARKED);
        set_vard(fvars+fget_var("g"), ptr1->accessed[0]&FILE_DIGEST);
        set_vard(fvars+fget_var("b"), (ptr1->accessed[0]&FILE_MARKED)&&(ptr1->accessed[0]&FILE_DIGEST));
        if (chk_currBM(currBM, currentuser)) {
            set_vard(fvars+fget_var("noreply"), ptr1->accessed[1]&FILE_READ);
            set_vard(fvars+fget_var("sign"), ptr1->accessed[0]&FILE_SIGN);
#ifdef FILTER
            set_vard(fvars+fget_var("censor"), ptr1->accessed[1]&FILE_CENSOR);
#endif
            set_vard(fvars+fget_var("del"), ptr1->accessed[1]&FILE_DEL);
            set_vard(fvars+fget_var("import"), ptr1->accessed[0]&FILE_IMPORTED);
        }
        set_vard(fvars+fget_var("attach"), ptr1->attachment);
        set_vars(fvars+fget_var("title"), ptr1->title);
        set_vars(fvars+fget_var("author"), ptr1->owner);
        set_vars(fvars+fget_var("fname"), ptr1->filename);
        set_vard(fvars+fget_var("my"), !strcmp(ptr1->owner,currentuser->userid));
#ifdef HAVE_BRC_CONTROL
        set_vard(fvars+fget_var("unread"), brc_unread(ptr1->id));
#endif
        setbfile(ffn, currboard, ptr1->filename);
        set_vard(fvars+fget_var("ftime"), get_posttime(ptr1));
        set_vard(fvars+fget_var("effsize"), ptr1->eff_size);
        if(load_stat) {
            if(stat(ffn, &st)!=-1)
                set_vard(fvars+fget_var("asize"), st.st_size);
            else
                set_vard(fvars+fget_var("asize"), 0);
        }
        if(load_content) {
            int k,abssize=0,entercount=0,ignoreline=0;
            set_vars(fvars+fget_var("content"), ptr1->filename);
            j = safe_mmapfile(ffn, O_RDONLY, PROT_READ, MAP_SHARED, (void **) &p, &fsize, NULL);
            if(j)
                set_vars(fvars+fget_var("content"), p);
        }
        ferr=0;
        feval(fvars+fget_var("res"), index, 0, strlen(index)-1);
        if(ferr) break;
        if(fvars[fget_var("res")].s) {
            write(fd, ptr1, size);
            count++;
            found++;
        }
        if(load_content) {
            if(j)
                end_mmapfile((void*)p, fsize, -1);
        }
        ptr1++;
    }
    free(libs);
    end_mmapfile((void *) ptr, buf.st_size, -1);
    ldata2.l_type = F_UNLCK;
    fcntl(fd2, F_SETLKW, &ldata2);
    close(fd2);
    ftruncate(fd, count * size);

    ldata.l_type = F_UNLCK;
    fcntl(fd, F_SETLKW, &ldata);        /* �˳���������*/
    close(fd);
    if(ferr) {
        move(3, 0);
        clrtoeol();
        prints("���ʽ����");
        refresh();
        sleep(1);
    }
    else if(count==0) {
        move(3, 0);
        clrtoeol();
        prints("һ����û���ҵ�....");
        refresh();
        sleep(1);
    }
/*    else if (chk_currBM(currBM, currentuser)) {
        char ans[4];
        int i,j,k;
        int fflag;
        int y,x;
        move(3, 0);
        clrtoeol();
        prints("�ҵ� %d ƪ����(0-�˳�, 1-�������m, 2-ɾ�����t, 3-���ɻظ����;) [0]", count);
        getyx(&y, &x);
        getdata(y, x, 0, ans, 3, 1, 0, 1);
        if(ans[0]>='1'&&ans[0]<='3') {
            struct fileheader f;
            k=ans[0]-'0';
            if(ans[0]=='1') fflag=FILE_MARK_FLAG;
            else if(ans[0]=='2') fflag=FILE_DELETE_FLAG;
            else if(ans[0]=='3') fflag=FILE_NOREPLY_FLAG;
            for(i=0;i<count;i++)
                change_post_flag(currBM, currentuser, digestmode, currboard, i+1, &f, currdirect, fflag, 0);
        }
    }*/
    return NEWDIRECT;
}

