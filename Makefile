CPPFLAGS        +=-MMD
CXXFLAGS        +=-Isrc -std=c++11 -Wall -Wextra -O3

STATIC_LIB_TARGET=build/libsheepshead.a
LEARNING_LIB_TARGET=build/liblearning.a

.PHONY: doc clean all test


all: $(STATIC_LIB_TARGET) $(LEARNING_LIB_TARGET) actors bindings

#################################################################
## Build the protocol buffer sources
#################################################################
PROTOS    =$(wildcard src/sheepshead/proto/*.proto)
PROTO_CCS =$(patsubst %.proto,%.pb.cc,$(PROTOS))
PROTO_HS  =$(patsubst %.proto,%.pb.h,$(PROTOS))
PROTO_OBJS=$(patsubst %.proto,%.pb.o,$(PROTOS))

$(PROTO_OBJS): $(PROTO_CCS) $(PROTO_HS)

%.pb.cc: %.proto
	protoc --cpp_out=$(dir $<) --proto_path=$(dir $<) $<

proto: $(PROTO_OBJS)

#################################################################
## Build the sheepshead interface sources
#################################################################
INTERFACE_CCS  =$(wildcard src/sheepshead/interface/*.cc)
INTERFACE_HS   =$(wildcard src/sheepshead/interface/*.h)
INTERFACE_OBJS =$(patsubst %.cc,%.o,$(INTERFACE_CCS))

interface: $(INTERFACE_OBJS) proto

build:
	@mkdir -p build

OBJS =$(PROTO_OBJS) $(INTERFACE_OBJS)
DEPENDS = $(OBJS:.o=.d)

$(STATIC_LIB_TARGET): CXXFLAGS += -fPIC
$(STATIC_LIB_TARGET): build $(OBJS)
	ar rcs $@ $(OBJS)
	ranlib $@

-include $(DEPENDS)

#################################################################
## Build the learning sources
#################################################################
LEARNING_CCS  =$(wildcard src/learning/*.cc)
LEARNING_HS   =$(wildcard src/learning/*.h)
LEARNING_OBJS =$(patsubst %.cc,%.o,$(LEARNING_CCS))

LEARNING_DEPENDS=$(LEARNING_OBJS:.o=.d)

learning: LDLIBS += -Lbuild -lsheepshead -lprotobuf
learning: $(STATIC_LIB_TARGET) $(LEARNING_OBJS)

$(LEARNING_LIB_TARGET): CXXFLAGS += -fPIC
$(LEARNING_LIB_TARGET): build $(LEARNING_OBJS)
	ar rcs $@ $(LEARNING_OBJS)
	ranlib $@

-include $(LEARNING_DEPENDS)
#################################################################
## Build the actors
#################################################################
ACTOR_CCS  =$(wildcard src/actors/*.cc)
ACTOR_EXES =$(patsubst %.cc,%,$(ACTOR_CCS))

actors: LDLIBS += -Lbuild -lsheepshead -lprotobuf -llearning
actors: $(STATIC_LIB_TARGET) $(ACTOR_EXES)

#################################################################
## Build the python bindings
#################################################################
BINDINGS_CC =bindings/sheepshead.cc
BINDINGS_SO =bindings/sheepshead.so

bindings: LDLIBS += -Lbuild -lsheepshead -lprotobuf
bindings: CXXFLAGS = -shared -fPIC -std=c++11 -Os -Wall -Isrc  $$(python3-config --cflags --ldflags --libs)

bindings: $(BINDINGS_SO)

$(BINDINGS_SO): $(BINDINGS_CC)
	$(CXX) $(CXXFLAGS) $< -o $@ $(LDLIBS)

#################################################################
## Build documentation
#################################################################
doc:
#	protoc --proto_path=src/sheepshead/proto/ src/sheepshead/proto/*.proto \
		--plugin=doc/protoc-gen-doc --doc_out=html,protoc_index.html:doc/html
	doxygen doc/doxygen/doxyfile.cfg
	cp doc/doxygen/doxy-boot.js doc/html

#################################################################
## Build and run the tests
#################################################################
test:
	$(MAKE) -C tests/

testclean:
	$(MAKE) clean -C tests/

testvalgrind:
	$(MAKE) valgrind -C tests/

clean:
	rm -rf $(INTERFACE_OBJS)
	rm -rf $(LEARNING_OBJS)
	rm -rf $(ACTOR_EXES)
	rm -rf $(PROTO_OBJS)
	rm -rf $(PROTO_HS) $(PROTO_CCS)
	rm -rf $(DEPENDS)
	rm -rf $(BINDINGS_SO)
	rm -rf build/
	rm -rf doc/html
