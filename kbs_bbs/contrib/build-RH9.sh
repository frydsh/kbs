#!/bin/sh
# $Id$

# �޸�����Ĳ�����������վ��
# BBSHOME ��ʾ���� BBS ���ڵ�Ŀ¼
BBSHOME=/usr/local/bbs

# BBSSITE ��ʾ�����õ�վ�㶨���ļ�
BBSSITE=fb2k-v2

# WWWROOT ��ʾ���� WWW ���ڵ�Ŀ¼
# ע�⣺���Ŀ¼���� httpd.conf ����� DocumentRoot ������
WWWROOT=/var/www

# ���µĴ����벻Ҫ�޸ģ���������ȷ���Լ��ڸ�ʲô
if [ -f Makefile ]; then
  make distclean
fi

if [ -d CVS ]; then
  cvs up -d
  aclocal; autoheader; automake -a; autoconf
fi

if [ -d bbs2www ]; then
  if [ -d CVS ]; then
    cd bbs2www
    aclocal; autoheader; automake -a; autoconf
    cd ..
  fi
  WWWCONFIG="--with-www=$WWWROOT --with-php=/usr/include/php"
else
  WWWCONFIG=--disable-www
fi

if [ -d sshbbsd ]; then
  if [ -d CVS ]; then
    cd sshbbsd
    aclocal; autoheader; automake -a; autoconf
    cd ..
  fi
  SSHCONFIG=--enable-ssh
else
  SSHCONFIG=--disable-ssh
fi

if [ -d innbbsd ]; then
  if [ -d CVS ]; then
    cd innbbsd
    aclocal; autoheader; automake -a; autoconf
    cd ..
  fi
  INNCONFIG=--enable-innbbsd
else
  INNCONFIG=--disable-innbbsd
fi

./configure --prefix=$BBSHOME --enable-site=$BBSSITE \
         $WWWCONFIG \
         $SSHCONFIG \
         $INNCONFIG \
         --with-mysql

make
