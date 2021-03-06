LDLIBS+=-lgtest_main -lgtest -lpthread -lprotobuf
CXXFLAGS=-g -I../src -std=c++11 -Wall -Wextra

LIBSHEEPSHEAD=../build/libsheepshead.a

.PHONY: all run proto interface clean

all: run

valgrind: interface proto
	VALGRIND="valgrind --leak-check=full --log-file=valgrind-%p.log" $(MAKE) run

ALL_TESTS = $(INTERFACE_TESTS) $(PROTO_TESTS)

run: interface proto
	bash ./runtests.sh
	
# Build tests of the protocol buffer models
PROTO_TEST_CCS=$(wildcard proto/*.cc)
PROTO_TEST_OBJS=$(patsubst %.cc,%.o,$(PROTO_TEST_CCS))
PROTO_TESTS=$(patsubst %.o,%,$(PROTO_TEST_OBJS))

proto: $(PROTO_TESTS) $(PROTO_TEST_OBJS)

$(PROTO_TESTS): $(PROTO_TEST_OBJS)

# Build tests of the sheepshead interface
INTERFACE_TEST_CCS =$(wildcard interface/*.cc)
INTERFACE_TEST_OBJS=$(patsubst %.cc,%.o,$(INTERFACE_TEST_CCS))
INTERFACE_TESTS=$(patsubst %.o,%,$(INTERFACE_TEST_OBJS))

interface: $(INTERFACE_TESTS) $(INTERFACE_TEST_OBJS)

$(INTERFACE_TESTS): $(INTERFACE_TEST_OBJS)

%.o: %.cc
	$(CXX) $(CXXFLAGS) -c $< -o $@

%: %.o $(LIBSHEEPSHEAD)
	$(CXX) $< $(LIBSHEEPSHEAD) $(LDLIBS) -o $@

clean:
	rm -rf $(PROTO_TESTS) $(PROTO_TEST_OBJS)
	rm -rf $(INTERFACE_TESTS) $(INTERFACE_TEST_OBJS)
	rm -f *.log
