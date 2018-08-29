CPPFLAGS=-fPIC -Wall -g3 -DRUN_TEST -DDEBUG
CFLAGS=-fPIC -Wall -g3  -DRUN_TEST -DDEBUG
INC="-I/usr/local/include"
LIBS="-L/usr/local/lib"
WORKDIR=$(shell pwd)

INC+=-I$(WORKDIR)/../pinpoint_common
LIBS+=-lboost_system -lboost_thread -lboost_date_time 

ifneq ($(strip $(WITH_BOOST_PATH)),)
	LIBS+=-L$(WITH_BOOST_PATH)/lib
	INC+=-I$(WITH_BOOST_PATH)
endif

ifneq ($(strip $(WITH_THRIFT_PATH)),)
	LIBS+=-L$(WITH_THRIFT_PATH)/lib -lthrift
	INC+=-I$(WITH_THRIFT_PATH)/include
else	
	THRIFT_INC=$(shell pkg-config --cflags thrift)
    THRIFT_LIB=$(shell pkg-config --libs thrift)
    INC+=$(THRIFT_INC)
    LIBS+=$(THRIFT_LIB)
endif

## open unit test flag
export RUN_TEST=1
export RUN_TEST_FLAGS=-DUNITTEST 

export PINPOINT_CXXFLAG=-fprofile-arcs -ftest-coverage -DTEST_SIMULATE  
export PINPOINT_CFLAG=-fprofile-arcs -ftest-coverage -DTEST_SIMULATE 


TEST_FLAGS = $(RUN_TEST_FLAGS)

## linked library 
CURDIR = $(shell pwd)
COMMON_DIR=$(CURDIR)/../pinpoint_common/
COMMON_LIB_NAME=pinpoint_common

MY_LIBS = -L$(COMMON_DIR) -l$(COMMON_LIB_NAME) -lstdc++ -lgcov -lpthread -lrt
TEST_ROOT = .
USER_DIR  = $(TEST_ROOT)

# Where to find user code.
TEST_UNIT_DIR = $(TEST_ROOT)

#all objs
# .c indicates C source files, and others C++ ones.
SRCEXTS = .c .C .cc .cpp .CPP .c++ .cxx .cp
SOURCES += $(foreach d,$(USER_DIR),$(wildcard $(addprefix $(d)/*,$(SRCEXTS))))
TEST_OBJS    = $(addsuffix .o, $(basename $(SOURCES)))

GTEST_DIR = ./gmock/gtest
GMOCK_DIR = ./gmock

# All Google Test headers.  Usually you shouldn't change this
# definition.
GTEST_HEADERS = $(GTEST_DIR)/include/gtest/*.h \
                $(GTEST_DIR)/include/gtest/internal/*.h

# All Google Mock headers. Note that all Google Test headers are
# included here too, as they are #included by Google Mock headers.
# Usually you shouldn't change this definition.
GMOCK_HEADERS = $(GMOCK_DIR)/include/gmock/*.h \
                $(GMOCK_DIR)/include/gmock/internal/*.h \
                $(GTEST_HEADERS)

# Usually you shouldn't tweak such internal variables, indicated by a
# trailing _.
GTEST_SRCS_ = $(GTEST_DIR)/src/*.cc $(GTEST_DIR)/src/*.h $(GTEST_HEADERS)
GMOCK_SRCS_ = $(GMOCK_DIR)/src/*.cc $(GMOCK_HEADERS)
TEST_FLAG += gmock_main.a

# Flags passed to the preprocessor.
CPPFLAGS += -I$(GTEST_DIR)/include -I$(GMOCK_DIR)/include $(TEST_FLAGS)



# All tests produced by this Makefile.  Remember to add new tests you
# created to the list.
PROGRAM = pinpoint_unittest

#all test case for each module
TESTS  = pinpoint_unittest

TEST_UNIT_OBJS = $(addsuffix .o, $(basename $(foreach d,$(TEST_UNIT_DIR),$(wildcard $(addprefix $(d)/*,$(SRCEXTS))))))


COMPILE.cxx = $(CXX) $(TEST_CPPFLAGS) $(CPPFLAGS) $(INC) -c
LINK.cxx    = $(CXX) $(TEST_CPPFLAGS) $(CPPFLAGS) $(LDFLAGS)

$(PROGRAM): pinpoint_common $(TEST_OBJS)  gmock_main.a
	$(CXX) $(INC)  $(TEST_OBJS)  $(TEST_FLAG) $(MY_LIBS)  $(LIBS) -o $@

pinpoint_common:
	$(MAKE) -C $(COMMON_DIR)


# Rules for generating object files (.o).
#----------------------------------------
objs:$(OBJS)
%.o:%.c
	$(COMPILE.c) $< -o $@

%.o:%.C
	$(COMPILE.cxx) $< -o $@

%.o:%.cc
	$(COMPILE.cxx) $< -o $@

%.o:%.cpp
	$(COMPILE.cxx) $< -o $@

%.o:%.CPP
	$(COMPILE.cxx) $< -o $@

%.o:%.c++
	$(COMPILE.cxx) $< -o $@

%.o:%.cp
	$(COMPILE.cxx) $< -o $@

%.o:%.cxx
	$(COMPILE.cxx) $< -o $@

# For simplicity and to avoid depending on implementation details of
# Google Mock and Google Test, the dependencies specified below are
# conservative and not optimized.  This is fine as Google Mock and
# Google Test compile fast and for ordinary users their source rarely
# changes.
gtest-all.o : $(GTEST_SRCS_)
	$(CXX) $(CPPFLAGS) $(INC) $(LIBS) -I$(GTEST_DIR) -I$(GMOCK_DIR) $(CXXFLAGS) \
            -c $(GTEST_DIR)/src/gtest-all.cc

gmock-all.o : $(GMOCK_SRCS_)
	$(CXX) $(CPPFLAGS) $(INC) $(LIBS) -I$(GTEST_DIR) -I$(GMOCK_DIR) $(CXXFLAGS) \
            -c $(GMOCK_DIR)/src/gmock-all.cc

gmock_main.o : $(GMOCK_SRCS_)
	$(CXX) $(CPPFLAGS) $(INC) $(LIBS) -I$(GTEST_DIR) -I$(GMOCK_DIR) $(CXXFLAGS) \
            -c $(GMOCK_DIR)/src/gmock_main.cc

gmock.a : gmock-all.o gtest-all.o
	$(AR) $(ARFLAGS) $@ $^

gmock_main.a : gmock-all.o gtest-all.o gmock_main.o
	$(AR) $(ARFLAGS) $@ $^

#install:
#	cp -f $(AP_MODULE) $(HTTPD_DIR)/modules/

clean:
	$(RM) $(TEST_OBJS) $(PROGRAM) $(TESTS) gmock.a gmock_main.a *.gcno *.gcda *.o ../$(PROGRAM)
	cd $(COMMON_DIR) && make clean
