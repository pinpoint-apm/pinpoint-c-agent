MAKE=make
CPPFLAGS=-fPIC -Wall $(PINPOINT_CXXFLAG)
CFLAGS=-fPIC -Wall $(PINPOINT_CFLAG)

DEBUG_FLAG=YES
WITH_BOOST_PATH=$(HOME)/bin
WITH_THRIFT_PATH=$(HOME)/bin
HTTPD_DIR = $(HTTPDDIR)
APR_HEADER=$(HOME)/bin/include/apr-1

# -I changto -isystem
INC=-isystem /usr/local/include
LIBS=-L/usr/local/lib

COMMON_LIB_NAME=pinpoint_common
COMMON_LIB_FILE_NAME=lib$(COMMON_LIB_NAME).a

# todo: debug flag
DEBUG=$(DEBUG_FLAG)

ifeq ($(RUN_TEST),1)
    # run gtest
	CFLAGS+=-g3 -DRUN_TEST -DDEBUG $(RUN_TEST_FLAGS)
	CPPFLAGS+=-g3 -DRUN_TEST -DDEBUG $(RUN_TEST_FLAGS)
else ifeq ($(DEBUG), YES)
    # debug
    CFLAGS+=-g3 -DDEBUG -Werror
	CPPFLAGS+=-g3 -DDEBUG -Werror
else
    # release
    CFLAGS+=-O3 -DNDEBUG -Werror
	CPPFLAGS+=-O3 -DNDEBUG -Werror
endif

ifneq ($(strip $(WITH_BOOST_PATH)),)
	LIBS+=-L$(WITH_BOOST_PATH)/lib
	INC+=-isystem $(WITH_BOOST_PATH)
endif

ifneq ($(strip $(WITH_THRIFT_PATH)),)
	LIBS+=-L$(WITH_THRIFT_PATH)/lib
	INC+=-isystem $(WITH_THRIFT_PATH)/include
else
	THRIFT_INC=$(shell pkg-config --cflags thrift)
    THRIFT_LIB=$(shell pkg-config --libs thrift)
    INC+=$(subst -I,-isystem ,$(THRIFT_INC))
    LIBS+=$(THRIFT_LIB)
endif

# gcc option
INC+=-I$(HTTPD_DIR)/include
INC+=-I$(APR_HEADER)
LIBS+=-lthrift -lboost_system -lboost_thread -lboost_filesystem -lboost_date_time

# target
AP_MODULE_NAME = mod_pinpoint

AP_MODULE = $(AP_MODULE_NAME).so
AP_MODULE_OBJ = $(AP_MODULE_NAME).o
AP_MODULE_FILE = $(AP_MODULE_NAME).cpp
SRCS = $(AP_MODULE_FILE)
HEADERS =
OBJS = $(SRCS:.cpp=.o)

# common lib
PINPOINT_COMMON_PATH="../pinpoint_common/"
PINPOINT_COMMON_LIB="pinpoint_common"

INC += -I$(PINPOINT_COMMON_PATH)

all: $(AP_MODULE)

%o: %cpp
	$(CXX) -c $(CPPFLAGS) $(INC) $(LIBS) -o $@ $<

$(AP_MODULE): common $(OBJS) $(HEADERS)
	$(CXX) -shared $(CPPFLAGS) $(INC) $(LIBS) -o $(AP_MODULE) $(OBJS) -L$(PINPOINT_COMMON_PATH) -l$(PINPOINT_COMMON_LIB)


common:
	$(MAKE) -j4 -C $(PINPOINT_COMMON_PATH)

.PHONY: all clean install uninstall common

install: all
	cp -f $(AP_MODULE) $(HTTPD_DIR)/modules/

uninstall:
	-rm -f $(HTTPD_DIR)/modules/$(AP_MODULE)

clean:
	$(MAKE) clean -C $(PINPOINT_COMMON_PATH)
	-rm -f $(AP_MODULE)
	-rm -f $(OBJS)