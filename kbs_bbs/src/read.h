#ifndef __READ_H__
#define __READ_H__

typedef int (*READ_KEY_FUNC)(struct _select_def*,void*,void*);

struct key_command {                /* Used to pass commands to the readmenu */
    int key;
    READ_KEY_FUNC fptr;
    void* arg;
};

struct read_arg {
  /* save argument */
  enum BBS_DIR_MODE mode;
  char* direct;
  void (*dotitle) ();
  READ_FUNC doentry;
  struct key_command *rcmdlist;
  int ssize;

  void* data; //readed data
  int fd; //filehandle,open always

  int filecount; //the item count of file
};

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
  @param down �������ⷽ�������1,���²��ң���������
  @param arg ���ݸ�func�Ĳ���
  @return Ӧ�õ��������
*/

int apply_thread(struct _select_def* conf, struct fileheader* fh,APPLY_THREAD_FUNC* func, bool down,void* arg);

int new_i_read(enum BBS_DIR_MODE cmdmode, char *direct, void (*dotitle) (), READ_FUNC doentry, struct key_command *rcmdlist, int ssize);

/* some function for fileheader */
int auth_search(struct _select_def* conf, struct fileheader *fileinfo,void* extraarg);
int title_search(struct _select_def* conf, struct fileheader* fh, void* extraarg);
int thread_search(struct _select_def* conf, struct fileheader* fh, void* extraarg);

#define SR_FIRSTNEW     0
#define SR_FIRST            1
#define SR_LAST             2
#define SR_FIRSTNEWDOWNSEARCH 100

int thread_read(struct _select_def* conf, struct fileheader* fh, void* extraarg);
#endif  //__READ_H__
