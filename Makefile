CPPFLAGS        +=-MMD
CXXFLAGS        +=-Isrc -std=c++11 -Wall -Wextra -g

STATIC_LIB_TARGET=build/libsheepshead.a

.PHONY: doc clean all test


all: $(STATIC_LIB_TARGET)

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
INTERFACE_CCS=$(wildcard src/sheepshead/interface/*.cc)
INTERFACE_HS=$(wildcard src/sheepshead/interface/*.h)
INTERFACE_OBJS   =$(patsubst %.cc,%.o,$(INTERFACE_CCS))

interface: $(INTERFACE_OBJS) proto

build:
	@mkdir -p build

OBJS =$(INTERFACE_OBJS) $(PROTO_OBJS)
DEPENDS = $(OBJS:.o=.d)

$(STATIC_LIB_TARGET): CXXFLAGS += -fPIC
$(STATIC_LIB_TARGET): build proto interface
	ar rcs $@ $(OBJS)
	ranlib $@

-include $(DEPENDS)

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
	rm -rf $(PROTO_OBJS)
	rm -rf $(PROTO_HS) $(PROTO_CCS)
	rm -rf $(DEPENDS)
	rm -rf build/
	rm -rf doc/html
