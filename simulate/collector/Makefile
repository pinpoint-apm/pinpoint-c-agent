CPPFLAGS=-g3 -DTEST_SIMULATE -DBOOST_NO_CXX11_SMART_PTR
CFLAGS=-g3 -DTEST_SIMULATE -DBOOST_NO_CXX11_SMART_PTR

CURDIR = $(shell pwd)
COMMON_DIR=$(CURDIR)/../../pinpoint_common/
COMMON_LIB_NAME=pinpoint_common

TEST_COLLECTOR=test_collector
TEST_CHECKER=test_checker

ifneq ($(strip $(WITH_BOOST_PATH)),)
	LIBS+=-L$(WITH_BOOST_PATH)/lib
	INC+=-isystem $(WITH_BOOST_PATH)/include
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

LIBS += -lboost_system -lboost_thread -lboost_filesystem -lboost_date_time -lthrift -lgcov -lrt -lpthread
INC += -I$(COMMON_DIR)

export RUN_SIMULATE_TEST=1
export RUN_SIMULATE_TEST_FLAGS=-g3 -DTEST_SIMULATE

headers = server.h \
          server_handler.h \
          collector_context.h \
          simulate_def.h \
          tbase_op.h \
          checker_handler.h \
          test_case.h

cpp_src = server.cpp \
          server_handler.cpp \
          collector_context.cpp \
          tbase_op.cpp \
          checker_handler.cpp \
          test_case.cpp

cpp_src_obj= $(cpp_src:.cpp=.o)

export DEBUG_FLAG=YES

all: collector checker

common:
	make -C $(COMMON_DIR) $(JENKINS_DEFINE_CONFIG)  -j$(CPUNUM)

collector: collector.cpp $(cpp_src_obj) $(cpp_src) $(headers) common
	# make -C $(COMMON_DIR)  with-boost-path=$(WITH_BOOST_PATH) with-thrift-path=$(WITH_THRIFT_PATH) -j4
	$(CXX) $(CPPFLAGS) collector.cpp $(cpp_src_obj) $(INC)  -l$(COMMON_LIB_NAME)   $(LIBS) -L$(COMMON_DIR) -o $(TEST_COLLECTOR)

checker: checker.cpp common
	$(CXX) $(CPPFLAGS) checker.cpp $(cpp_src_obj) $(INC) -L$(COMMON_DIR)  -l$(COMMON_LIB_NAME) $(LIBS)  -o $(TEST_CHECKER)

$(cpp_src_obj): %.o: %.cpp
	$(CXX) -c $(CPPFLAGS) $(INC) $(LIBS) -o $@ $<


.PHONY: all clean install uninstall collector checker common

clean:
	make -C $(COMMON_DIR) clean
	-rm -f $(TEST_COLLECTOR)
	-rm -f *.o
	-rm -f *.gcno
	-rm -f *.gcda
