CXXFLAGS        +=-Isrc -std=c++11 -Wall -Wextra

PROTOS    =$(wildcard src/sheepshead/proto/*.proto)
PROTO_CCS =$(patsubst %.proto,%.pb.cc,$(PROTOS))
PROTO_HS  =$(patsubst %.proto,%.pb.h,$(PROTOS))
PROTO_OBJS=$(patsubst %.proto,%.pb.o,$(PROTOS))
PROTO_LIB =$(dir $(word 1,$(PROTOS)))libproto.a

INTERFACE_SOURCES=$(wildcard src/sheepshead/interface/*.cc)
INTERFACE_OBJS   =$(patsubst %.cc,%.o,$(INTERFACE_SOURCES))
INTERFACE_LIB    =$(dir $(word 1,$(INTERFACE_OBJS)))libinterface.a


.PHONY: doc clean all test

all: proto interface

proto: $(PROTO_LIB)
$(PROTO_LIB): $(PROTO_OBJS)
	ar rcs $@ $^
	ranlib $@
$(PROTO_OBJS): $(PROTO_CCS) $(PROTO_HS)

interface: $(INTERFACE_LIB)
$(INTERFACE_LIB): $(INTERFACE_OBJS)
	ar rcs $@ $^
$(INTERFACE_OBJS): $(PROTO_LIB)


%.pb.cc: %.proto
	protoc --cpp_out=$(dir $<) --proto_path=$(dir $<) $<

doc:
#	protoc --proto_path=src/sheepshead/proto/ src/sheepshead/proto/*.proto \
		--plugin=doc/protoc-gen-doc --doc_out=html,protoc_index.html:doc/html
	doxygen doc/doxygen/doxyfile.cfg
	cp doc/doxygen/doxy-boot.js doc/html

clean:
	rm -rf $(INTERFACE_OBJS)
	rm -rf $(PROTO_OBJS)
	rm -rf $(PROTO_HS) $(PROTO_CCS)
	rm -rf $(PROTO_LIB) $(INTERFACE_LIB)
	rm -rf doc/html
