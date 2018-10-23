PHP_ARG_ENABLE(pinpoint, whether to enable pinpoint support,
[ --enable-pinpoint   Enable pinpoint support])


PINPOINT_COMMON_PATH="../pinpoint_common/"
PINPOINT_COMMON_LIB="pinpoint_common"

PINPOINT_SRC="aop_hook.cpp interceptor.cpp php_common.cpp php_plugin.cpp php_interfaces.cpp php_function_helper.cpp"

PHP_ARG_WITH(source_dir, for php source dir,
[  --with-source-dir[=DIR]    Include curl_plugin_dir support ?], no, no)

PHP_ARG_WITH(thrift_dir, for thrift_dir support,
[  --with-thrift-dir[=DIR]    Include thrift_dir support ?] no, no)

PHP_ARG_WITH(boost_dir, for boost_dir support,
[  --with-boost-dir[=DIR]    Include boost_dir support ?] no, no)

PHP_ARG_ENABLE(gcov, enable gcov support,
[  --enable-gcov        Use gcov?], no, no)

PHP_ARG_ENABLE(release, release,
[  --enable-release       use static link?], no, no)


if test "$PHP_RELEASE" = "yes"; then
    BOOST_LIB="-L$WITH_BOOST_PATH/lib  -l:libboost_system.a -l:libboost_thread.a -l:libboost_filesystem.a  -l:libboost_date_time.a"
    THRIFT_LIB="-L$WITH_THRIFT_PATH/lib  -l:libthrift.a"
    echo "PINPOINT_SHARED_LIBADD" $BOOST_LIB
else
    BOOST_LIB="-L$WITH_BOOST_PATH/lib  -lboost_system -lboost_thread -lboost_filesystem  -lboost_date_time"
    THRIFT_LIB="-L$WITH_THRIFT_PATH/lib  -lthrift"
fi


PINPOINT_PLUGINS="plugins/request_plugin.cpp plugins/internel_functions_plugin.cpp"

PINPOINT_SRC="$PINPOINT_SRC $PINPOINT_PLUGINS"

if test "$PHP_PINPOINT" = "yes"; then
    PHP_REQUIRE_CXX()
    PHP_SUBST(PINPOINT_SHARED_LIBADD)
    PHP_ADD_LIBRARY(stdc++, 1, PINPOINT_SHARED_LIBADD)
    AC_DEFINE(HAVE_PINPOINT, 1, [Whether you have pinpoint])
    
    PHP_EVAL_INCLINE("-I$PINPOINT_COMMON_PATH")   

    if test "$PHP_THRIT_DIR" != "no"; then
        PHP_ADD_INCLUDE("$PHP_THRIT_DIR")
        echo "PHP_THRIT_DIR="$PHP_THRIT_DIR
    fi

    if test "$PHP_BOOST_DIR" != "no"; then
        PHP_ADD_INCLUDE("$PHP_BOOST_DIR") 
        echo "PHP_BOOST_DIR:"$PHP_BOOST_DIR
    fi

    
    PINPOINT_SRC="$PINPOINT_SRC"

    PINPOINT_SHARED_LIBADD="$PINPOINT_SHARED_LIBADD -L$PINPOINT_COMMON_PATH -l$PINPOINT_COMMON_LIB $BOOST_LIB $THRIFT_LIB -lm"

    if test "$PHP_GCOV" = "yes"; then
        AC_DEFINE([HAVE_GCOV], [1], [Has cURL])
        PINPOINT_SHARED_LIBADD="$PINPOINT_SHARED_LIBADD -lgcov"
    fi 
    
    PHP_NEW_EXTENSION(pinpoint, pinpoint_ext.cpp $PINPOINT_SRC, $ext_shared)
fi
