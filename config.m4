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
  PHP_ADD_INCLUDE(PHP_EXT_SRCDIR()/common/jsoncpp/include)
  PHP_ADD_INCLUDE(PHP_EXT_SRCDIR()/common/jsoncpp/include)
  PHP_ADD_INCLUDE(PHP_EXT_SRCDIR()/common/src)
  PHP_ADD_INCLUDE(PHP_EXT_SRCDIR()/src/PHP)
  AC_DEFINE(HAVE_PINPOINT_PHP, 1, [Whether you have pinpoint])

  # echo "------Build common libraries------------"
  # mkdir -p PHP_EXT_SRCDIR()/build
  # cd build 
  # cmake PHP_EXT_SRCDIR()/common -DCMAKE_BUILD_TYPE=Release
  # make
  # cd PHP_EXT_SRCDIR()
  # PINPOINT_COMMON_LIB=PHP_EXT_SRCDIR()/build/lib
  # echo "----------------------------------------"

  EXTRA_PHP_SRCS="src/PHP/pinpoint_php.cpp"

  PINPOINT_SRCS="$EXTRA_PHP_SRCS \
    common/src/common.cpp \
    common/src/Logging.cpp \
    common/src/NodePool/TraceNode.cpp \
    common/src/NodePool/PoolManager.cpp \
    common/src/ConnectionPool/TransLayer.cpp \
    common/src/Cache/State.cpp \
    common/src/Cache/Chunk.cpp \
    "
  PINPOINT_SRCS="$PINPOINT_SRCS \
    common/jsoncpp/lib_json/json_writer.cpp \
    common/jsoncpp/lib_json/json_value.cpp \
    common/jsoncpp/lib_json/json_reader.cpp \
  "


  PINPOINT_PHP_SHARED_LIBADD="$PINPOINT_PHP_SHARED_LIBADD -lrt"
  
  PHP_NEW_EXTENSION(pinpoint_php,$PINPOINT_SRCS, $ext_shared)
  PHP_ADD_BUILD_DIR($ext_builddir/src/PHP)
fi