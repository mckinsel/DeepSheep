CXXFLAGS  +=-Isrc -std=c++0x -Wall -Wextra
PROTOS    =$(wildcard src/sheepshead/proto/*.proto)
PROTO_CCS =$(patsubst %.proto,%.pb.cc,$(PROTOS))
PROTO_HS  =$(patsubst %.proto,%.pb.h,$(PROTOS))
PROTO_OBJS=$(patsubst %.proto,%.pb.o,$(PROTOS))

INTERFACE_SOURCES=$(wildcard src/sheepshead/interface/*.cc)
INTERFACE_OBJS   =$(patsubst %.cc,%.o,$(INTERFACE_SOURCES))

all: $(INTERFACE_OBJS)
$(INTERFACE_OBJS): $(PROTO_OBJS)
$(PROTO_OBJS): $(PROTO_CCS)

%.pb.cc: %.proto
	LD_LIBRARY_PATH=/usr/local/lib protoc --cpp_out=$(dir $<) --proto_path=$(dir $<) $<

clean:
	rm -rf $(INTERFACE_OBJS)
	rm -rf $(PROTO_OBJS)
	rm -rf $(PROTO_HS) $(PROTO_CCS)
