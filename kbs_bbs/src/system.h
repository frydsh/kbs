#ifndef BBS_SYSTEM_H_5423523
#define BBS_SYSTEM_H_5423523
#include "bbs.h"
int f_rm(char *fpath);
int f_mv(char *src, char *dst);
int f_cp(char *src, char *dst, int mode);
void f_cat(char *fpath, char *msg);
void f_catfile(char* srcpath,char* dstpath);
int f_touch(char *filename);
int f_ln(char *src, char *dst);
typedef int(*OUTC_FUNC)(char);
void uuencode(char* inbuf, int size, char *filename, OUTC_FUNC fn);


/* �ַ���ƥ�亯��*/
char* bm_strstr(const char* string,const char* pattern);
/* �ַ������ƥ�亯��*/
char* bm_strstr_rp(const char* string,const char* pattern,
	size_t* shift,bool* init);
/* �ַ�����Сд�����е�ƥ�亯��*/
char* bm_strcasestr(const char* string,const char* pattern);
/* �ַ�����δ�Сд������ƥ�亯��*/
char* bm_strcasestr_rp(const char* string,const char* pattern,
	size_t* shift,bool* init);
void *memfind(const void *in_block,     /* ���ݿ� */
              const size_t block_size,  /* ���ݿ鳤�� */
              const void *in_pattern,   /* ��Ҫ���ҵ����� */
              const size_t pattern_size,        /* �������ݵĳ��� */
              size_t * shift,   /* ��λ��Ӧ����256*size_t������ */
              bool * init); /* �Ƿ���Ҫ��ʼ����λ�� */

void *txtfind(const void *in_block,     /* ���ݿ� */
              const size_t block_size,  /* ���ݿ鳤�� */
              const void *in_pattern,   /* ��Ҫ���ҵ����� */
              const size_t pattern_size,        /* �������ݵĳ��� */
              size_t * shift,   /* ��λ��Ӧ����256*size_t������ */
              bool * init); /* �Ƿ���Ҫ��ʼ����λ�� */

int lock_reg(int fd,int cmd,int type,off_t offset,int whence,off_t len);
pid_t lock_test(int fd,int cmd,int type,off_t offset,int whence,off_t len);

/* some marco from APUE*/
#define read_lock(fd,offset,whence,len)\
	lock_reg(fd,F_SETLK,F_RDLCK,offset,whence,len)

#define readw_lock(fd,offset,whence,len)\
	lock_reg(fd,F_SETLKW,F_RDLCK,offset,whence,len)

#define write_lock(fd,offset,whence,len)\
	lock_reg(fd,F_SETLK,F_WRLCK,offset,whence,len)

#define writew_lock(fd,offset,whence,len)\
	lock_reg(fd,F_SETLKW,F_WRLCK,offset,whence,len)

#define un_lock(fd,offset,whence,len)\
	lock_reg(fd,F_SETLKW,F_UNLCK,offset,whence,len)

#define is_read_lockable(fd,offset,whence,len)\
	!lock_test(fd,F_GETLK,F_RDLCK,offset,whence,len)

#define is_write_lockable(fd,offset,whence,len)\
	!lock_test(fd,F_GETLK,F_WRLCK,offset,whence,len)

/* base64.c */
int from64tobits(char *out, const char *in); /* base 64 to raw bytes in quasi-big-endian order, returning count of bytes */
void to64frombits(unsigned char *out, const unsigned char *in, int inlen);

#endif
