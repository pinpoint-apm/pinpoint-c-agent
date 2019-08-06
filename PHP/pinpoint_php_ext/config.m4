dnl $Id$
dnl config.m4 for extension pinpoint_php

dnl Comments in this file start with the string 'dnl'.
dnl Remove where necessary. This file will not work
dnl without editing.

dnl If your extension references something external, use with:

PHP_ARG_WITH(pinpoint_php, for pinpoint_php support,
[  --with-pinpoint_php             Include pinpoint_php support])



PHP_ARG_ENABLE(pinpoint_php, whether to enable pinpoint_php support,
[  --enable-pinpoint_php           Enable pinpoint_php support])


PHP_ARG_ENABLE(ext_test, support test,
[  --enable-ext-test              Enable test support],no,no)

if test "$PHP_PINPOINT_PHP" != "no"; then
  
  PHP_REQUIRE_CXX()
  PHP_SUBST(PINPOINT_PHP_SHARED_LIBADD)
  PHP_ADD_LIBRARY(stdc++, 1, PINPOINT_PHP_SHARED_LIBADD)
  PHP_EVAL_INCLINE("-Idepends/json/include")
  AC_DEFINE(HAVE_PINPOINT_PHP, 1, [Whether you have pinpoint])
  
  JSON_SRC="depends/json/src/json_reader.cpp depends/json/src/json_value.cpp depends/json/src/json_writer.cpp "
  PINPOINT_SRC=" $JSON_SRC "
 
 
  if test "$PHP_EXT_TEST" = "yes"; then
    AC_DEFINE([HAVE_EXT_TEST],[1],[has ext test])
  fi
  
  PHP_NEW_EXTENSION(pinpoint_php, pinpoint_php.cpp pinpoint_helper.cpp $PINPOINT_SRC, $ext_shared)
fi
