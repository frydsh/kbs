#!/bin/sh

aclocal; libtoolize -c --force; autoheader; automake -a; autoconf
