LIBRARIES := leveldb glog protobuf
LDFLAGS += $(foreach library,$(LIBRARIES), -l $(library))
INCLUDE_DIR := /usr/local/include
CXXFLAGS := -stdlib=libstdc++
CXX := /usr/bin/clang++

build: converter protoc proto
	$(CXX) $(CXXFLAGS) -I$(INCLUDE_DIR) $(LDFLAGS) yeast_dataset_leveldb_converter.o protobufEx.pb.o -o converter.bin 

converter: yeast_dataset_leveldb_converter.cc
	$(CXX) $(CXXFLAGS) -c yeast_dataset_leveldb_converter.cc
proto: protoc protobufEx.pb.cc
	$(CXX) $(CXXFLAGS) -c protobufEx.pb.cc
protoc: protobufEx.proto
	protoc -I=./ --cpp_out=./ protobufEx.proto
clean:
	rm *.o 2014* *.bin
	