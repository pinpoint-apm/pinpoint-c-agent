CPPFLAGS=-fPIC -Wall $(PINPOINT_CXXFLAG) 
CFLAGS=-fPIC -Wall $(PINPOINT_CFLAG) 
# -I changto -isystem
INC=#/usr/local/include
LIBS=#-L/usr/local/lib

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

ifeq ($(WITH_BOOST_PATH), $(wildcard $(WITH_BOOST_PATH)))
	LIBS+=-L$(WITH_BOOST_PATH)/lib 
	INC+=-isystem $(WITH_BOOST_PATH)/include 
endif

ifeq ($(WITH_THRIFT_PATH), $(wildcard $(WITH_THRIFT_PATH)))
	LIBS+=-L$(WITH_THRIFT_PATH)/lib
	INC+=-isystem $(WITH_THRIFT_PATH)/include 
else	
	THRIFT_INC=$(shell pkg-config --cflags thrift) 
    THRIFT_LIB=$(shell pkg-config --libs thrift) 
    INC+=$(subst -I,-isystem ,$(THRIFT_INC)) 
    LIBS+=$(THRIFT_LIB)
endif


#LIBS+=-lboost_system -lboost_thread

# for thrift
THRIFT_FILE_DIR = ./thrift/
THRIFT_TARGET_DIR = ./thrift/gen-cpp/
THRIFT_FILES = Trace.thrift \
               Command.thrift \
               Pinpoint.thrift
#THRIFT_SRCS = $(THRIFT_FILES:.thrift=_types.cpp)
#THRIFT_OBJS = $(THRIFT_FILES:.thrift=_types.o)
#THRIFT_HEADERS = $(THRIFT_FILES:.thrift=_types.h)

THRIFT_SRCS_TEMP_1 = $(foreach n, $(THRIFT_FILES), $(THRIFT_TARGET_DIR)$(n))
THRIFT_SRCS_TEMP_2 = $(THRIFT_SRCS_TEMP_1:.thrift=_types.cpp)
THRIFT_SRCS_TEMP_3 = $(THRIFT_SRCS_TEMP_1:.thrift=_constants.cpp)
#THRIFT_SRCS = $(shell echo $(THRIFT_SRCS_TEMP_2) | tr A-Z a-z)
THRIFT_SRCS = $(THRIFT_SRCS_TEMP_2) $(THRIFT_SRCS_TEMP_3)
#THRIFT_SRCS = $(shell echo $(THRIFT_SRCS_TEMP_4) | tr A-Z a-z)
THRIFT_HEADERS = $(THRIFT_SRCS:.cpp=.h)
THRIFT_OBJS = $(THRIFT_SRCS:.cpp=.o)
THRIFT_FILES_DIST = $(foreach n, $(THRIFT_FILES), $(THRIFT_FILE_DIR)$(n))

.PHONY: all clean thrift

headers = pinpoint_agent.h \
          pinpoint_error.h \
          pinpoint_agent_context.h \
          pinpoint_def.h \
          log_utility.h \
          trace.h \
          utility.h \
          serializer.h \
          executor.h \
          data_sender.h \
          buffer.h \
          agent_data_sender.h \
          agent_monitor_sender.h \
          hand_shake_sender.h \
          trace_data_sender.h \
          api_data_sender.h \
          string_data_sender.h \
          pinpoint_api.h \
          pinpoint_plugin.h \
          pinpoint_type.h \
          pinpoint_client.h \
          pinpoint_ping.h

c_src = log.c

cpp_src = pinpoint_agent.cpp \
          pinpoint_agent_context.cpp \
          log_utility.cpp \
          trace.cpp \
          utility.cpp \
          serializer.cpp \
          executor.cpp \
          data_sender.cpp \
          buffer.cpp \
          agent_data_sender.cpp \
          agent_monitor_sender.cpp \
          hand_shake_sender.cpp \
          trace_data_sender.cpp \
          api_data_sender.cpp \
          string_data_sender.cpp \
          pinpoint_api.cpp \
          pinpoint_plugin.cpp \
          pinpoint_type.cpp \
          pinpoint_client.cpp \
          pinpoint_ping.cpp


c_src_obj= $(c_src:.c=.o)
cpp_src_obj= $(cpp_src:.cpp=.o)

all:$(THRIFT_OBJS) $(headers) $(cpp_src_obj) $(c_src_obj)
	ar rcs $(COMMON_LIB_FILE_NAME) $(cpp_src_obj) $(c_src_obj) $(THRIFT_OBJS)

thrift:$(THRIFT_FILES_DIST)
	@for i in $(THRIFT_FILES_DIST); \
	    do $(WITH_THRIFT_PATH)/bin/thrift -o $(THRIFT_FILE_DIR) -r -gen cpp $$i; \
	done

$(THRIFT_OBJS): %.o: %.cpp
	$(CXX) -c $(CPPFLAGS) $(INC) $(LIBS) -o $@ $<

$(cpp_src_obj): %.o: %.cpp
	$(CXX) -c $(CPPFLAGS) $(INC) $(LIBS) -o $@ $<

$(c_src_obj): %.o: %.c
	$(CC) -c $(CFLAGS) $(INC) $(LIBS) -o $@ $<

clean:
	rm -fr $(c_src_obj)
	rm -fr $(cpp_src_obj)
	rm -fr $(COMMON_LIB_FILE_NAME)
	-rm -f $(THRIFT_OBJS)
	find . -name "*.gcda" | xargs rm -f
	find . -name "*.gcno" | xargs rm -f
