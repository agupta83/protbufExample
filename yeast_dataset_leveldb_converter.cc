#include <assert.h>
#include <iostream>
#include <fstream>
#include <cstdlib>

#include <leveldb/db.h>
#include <glog/logging.h>

#include "protobufEx.pb.h"

void convert_dataset(const char* data_filename, const char * db_filename) {
	
	// Open files
	std::ifstream data_file(data_filename, std::ios::in);
	
	CHECK(data_file) << "Unable to open file "  << data_filename;
	LOG(INFO) << "Found the data file";
	
	// Open leveldb
	leveldb::DB* db;
	leveldb::Options options;
	options.create_if_missing = true;
	options.error_if_exists = true;
	leveldb::Status status = leveldb::DB::Open(options,db_filename, &db);
	CHECK(status.ok())<< "Failed to open leveldb " << db_filename;
	
	leveldbGlog::Datum data;
	std::string line;
	char ch;
	int begin_idx=0;
	int feild_num = 0;
	int linenum = 0;
	
	std::string seq_name;
	float mcg = 0.f;
	float gvh = 0.f;
	float alm = 0.f;
	float mit = 0.f;
	float erl = 0.f;
	float pox = 0.f;
	float vac = 0.f;
	float nuc = 0.f;
	std::string label;
	
	const int maxKeyLength = 10;
	char key[maxKeyLength];
	std::string value;
	
	while(!data_file.eof()){
		std::getline(data_file, line);
		//std::cout << line << std::endl;
		begin_idx = 0;
		int i = 0;
		int nchar = 0;
		for(int i = 0; i < line.length(); i++){
			if(line[i]==' ' || i == line.length()){
				switch(feild_num){
					case 0:
					seq_name = line.substr(begin_idx, nchar);
					//std::cout<<seq_name<<std::endl;
					break;
					case 1:
					mcg = atof(line.substr(begin_idx, nchar).c_str());
					//std::cout<<mcg<<std::endl;
					break;
					case 2:
					gvh = atof(line.substr(begin_idx, nchar).c_str());
					//std::cout<<gvh<<std::endl;
					break;
					case 3:
					alm = atof(line.substr(begin_idx, nchar).c_str());
					break;
					case 4:
					mit = atof(line.substr(begin_idx, nchar).c_str());
					break;
					case 5:
					erl = atof(line.substr(begin_idx, nchar).c_str());
					break;
					case 6:
					pox = atof(line.substr(begin_idx, nchar).c_str());
					break;
					case 7:
					vac = atof(line.substr(begin_idx, nchar).c_str());
					break;
					case 8:
					nuc = atof(line.substr(begin_idx, nchar).c_str());
					break;
					case 9:
					label = line.substr(begin_idx, nchar);
					break;
				}
				begin_idx = i+1;
				feild_num++;
				nchar = 0;
			}else{
				nchar++;
			}
		}
		feild_num = 0;
		data.set_seq_name(seq_name);
		data.set_mcg(mcg);
		data.set_gvh(gvh);
		data.set_alm(alm);
		data.set_mit(mit);
		data.set_erl(erl);
		data.set_pox(pox);
		data.set_vac(vac);
		data.set_nuc(nuc);
		data.set_label(label);
		data.SerializeToString(&value);
		
		snprintf(key, maxKeyLength, "%08d",linenum);
		
		db->Put(leveldb::WriteOptions(), std::string(key), value);
		linenum++;
	}
	delete db;
	LOG(INFO) << "A total of "<< (linenum - 1) << " records.";
	data_file.close();
	
}

void print_db(const char * db_filename){
	// Open leveldb
	leveldb::DB* db;
	leveldb::Options options;
	leveldb::Status status = leveldb::DB::Open(options,db_filename, &db);
	CHECK(status.ok())<< "Failed to open leveldb " << db_filename;
	
	leveldbGlog::Datum data;
	leveldb::Iterator* it = db->NewIterator(leveldb::ReadOptions());
	std::string value;
	for (it->SeekToFirst(); it->Valid(); it->Next()) {
		data.ParseFromString(it->value().ToString());
		std::cout << it->key().ToString() << ": "  << data.seq_name() <<std::endl;
	}
	assert(it->status().ok());  // Check for any errors found during the scan
	delete it;
}
int main(int argc, char** argv){
	
	
	google::InitGoogleLogging(argv[0]);
	google::SetLogDestination(google::INFO, "./"); 
	convert_dataset(argv[1], argv[2]);
	print_db(argv[2]);
	return 0;
}
