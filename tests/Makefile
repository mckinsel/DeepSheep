LDLIBS+=-lgtest_main -lgtest -lpthread -lprotobuf
CXXFLAGS=-g -I../src -std=c++11 -Wall -Wextra

LIBSHEEPSHEAD=../build/libsheepshead.a

.PHONY: all run proto interface clean

all: run

valgrind: interface proto
	@(for test_exe in $(ALL_TESTS); do valgrind --leak-check=full --log-file=./$$test_exe.valgrind ./$$test_exe; done)

ALL_TESTS = $(INTERFACE_TESTS) $(PROTO_TESTS)

run: interface proto
	@(for test_exe in $(ALL_TESTS); do ./$$test_exe; done)
	
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
	find . -name "*.valgrind" | xargs rm -f
