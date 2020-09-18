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
  PHP_ADD_INCLUDE(PHP_EXT_SRCDIR()/common/include)
  PHP_ADD_INCLUDE(PHP_EXT_SRCDIR()/src/PHP)
  AC_DEFINE(HAVE_PINPOINT_PHP, 1, [Whether you have pinpoint])

  echo "------Build common libraries------------"
  mkdir -p PHP_EXT_SRCDIR()/build
  cd build 
  cmake PHP_EXT_SRCDIR()/common -DCMAKE_BUILD_TYPE=Release
  make
  cd PHP_EXT_SRCDIR()
  PINPOINT_COMMON_LIB=PHP_EXT_SRCDIR()/common/build/lib
  echo "----------------------------------------"

  PINPOINT_PHP_SHARED_LIBADD="$PINPOINT_PHP_SHARED_LIBADD -L$PINPOINT_COMMON_LIB -l:libpinpoint_common.a -l:libjsoncpp.a -lrt"
  
  PHP_NEW_EXTENSION(pinpoint_php,src/PHP/pinpoint_php.cpp, $ext_shared)
  PHP_ADD_BUILD_DIR($ext_builddir/src/PHP)
fi