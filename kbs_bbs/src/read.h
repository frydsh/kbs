#ifndef __READ_H__
#define __READ_H__

typedef int (*READ_KEY_FUNC)(struct _select_def*,void*,void*);
typedef char *(*READ_ENT_FUNC) (void *, int, void *,void*,struct _select_def*);
int fileheader_thread_read(struct _select_def* conf, struct fileheader* fh,int ent, void* extraarg);
int find_nextnew(struct _select_def* conf,int begin);

struct key_command {                /* Used to pass commands to the readmenu */
    int key;
    READ_KEY_FUNC fptr;
    void* arg;
};

enum {
    READ_NORMAL,
    READ_THREAD,
    READ_NEW,
    READ_AUTHOR
};

struct read_arg {
    /* save argument */
    enum BBS_DIR_MODE mode;
    char* direct;
    char* dingdirect; /*�ö���.DIR����������*/
    void (*dotitle) ();
    READ_ENT_FUNC doentry;
    struct key_command *rcmdlist;
    int ssize;
    int oldpos; /*��ͬ�����Ķ���ʱ�򣬱���ԭʼλ��*/

    /*����ȷ����ǰ���Ķ�ģʽ�������
    READ_NORMAL  ������˳���Ķ�
    READ_THREAD  ���������Ķ�
    */
    int readmode; 

    void* data; //readed data
    int fd; //filehandle,open always

    void* readdata;

    int filecount; //the item count of file
};


/* ��õ�ǰ��pos�������ļ���,��Ҫ��Ϊ�������ö�����ͨ��.DIR*/
char* read_getcurrdirect(struct _select_def* conf);

enum {
        APPLY_CONTINUE,
        APPLY_QUIT,
        APPLY_REAPPLY
};

/* Ӧ����apply_thread�ĺ���
  conf��select�ṹ
  fh�ǵ�ǰ��fileheader
  ent�����
  arg�Ǵ���apply_thread�Ķ������
*/
typedef int (*APPLY_THREAD_FUNC)(struct _select_def* conf,struct fileheader* fh,int ent,void* arg);

/*��ͬ����Ӧ��ͬһ��func����
  @param conf ��ǰ��select�ṹ
  @param fh ��ǰ��fileheaderָ��
  @param func Ӧ��������ṹ�ĺ�����
        ����APPLY_CONTINUE����Ӧ����һ������ṹ
        ����APPLY_QUIT��ִֹͣ�С�
        ����APPLY_REAPPLY���ظ�Ӧ����һ��λ�õ�fileheader
  @param applycurrent �Ƿ�Ե�ǰλ�õ�fileheaderӦ��func
  @param down �������ⷽ�������1,���²��ң���������
  @param arg ���ݸ�func�Ĳ���
  @return Ӧ�õ��������
*/

int apply_thread(struct _select_def* conf, struct fileheader* fh,APPLY_THREAD_FUNC func,bool applycurrent, bool down,void* arg);

/*@param buf ��ʾ���ַ���
   @param num ���
   @param data ����
   @param readdata �ϴ��Ķ�������,�����ж�ͬ����֮��
   @param conf _select_conf�ṹָ��
*/

int new_i_read(enum BBS_DIR_MODE cmdmode, char *direct, void (*dotitle) (struct _select_def*), READ_ENT_FUNC doentry, struct key_command *rcmdlist, int ssize);

/* some function for fileheader */
int auth_search(struct _select_def* conf, struct fileheader *fileinfo,void* extraarg);
int title_search(struct _select_def* conf, struct fileheader* fh, void* extraarg);
int thread_search(struct _select_def* conf, struct fileheader* fh, void* extraarg);
int post_search(struct _select_def* conf, struct fileheader* fh, void* extraarg);

#define SR_FIRSTNEW     0
#define SR_FIRST            1
#define SR_LAST             2
#define SR_NEXT             3
#define SR_PREV             4
#define SR_READ            5    /*ͬ�����Ķ�*/
#define SR_READX           6   /*ͬ�����Ķ�������ԭʼλ��*/


#define SR_FIRSTNEWDOWNSEARCH 100

int thread_read(struct _select_def* conf, struct fileheader* fh, void* extraarg);
int author_read(struct _select_def* conf, struct fileheader* fh, void* extraarg);

int read_sendmsgtoauthor(struct _select_def* conf, struct fileheader* fh, void* extraarg);
int read_showauthor(struct _select_def* conf, struct fileheader* fh, void* extraarg);
int read_showauthorinfo(struct _select_def* conf, struct fileheader* fh, void* extraarg);
int read_cross(struct _select_def* conf, struct fileheader* fh, void* extraarg);
int read_zsend(struct _select_def* conf, struct fileheader* fh, void* extraarg);


int read_addauthorfriend(struct _select_def* conf, struct fileheader* fh, void* extraarg);

/*
    ���õ�ǰ�Ķ���fileheader����ͬ�����ж�
*/
void setreadpost(struct _select_def* conf,struct fileheader* fh);

int read_showauthorBM(struct _select_def* conf, struct fileheader* fh, void* extraarg);

#ifdef PERSONAL_CORP
int read_importpc(struct _select_def* conf, struct fileheader* fh, void* extraarg);
#endif
#endif  //__READ_H__
