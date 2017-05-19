#pragma once
//This file contains transient objects
#include "fs_struct.h"
#include <vector>
#include <string>
using std::vector;
using std::string;

class path_t {
protected:
	vector<string> n;
public:
	path_t() = default;
	path_t(const string &s);
	size_t size() const;
	string get(const size_t c) const;
	string format() const;
	void push(const string &s);
	void pop();
	void fromString(const string &s);
};

/*
文件对象只能有fs创建
写入前需要预先分配空间
时间戳由fs维护
*/
class file_t {
public:
	block_dev *base;
	inode_t *inode;
	vector<data_block_t*> data_addr;
	vector<sub_inode_rec_t> sub_inode_rec;
public:
	file_t() = default;
	file_t(block_dev *base, inode_t *inode);
	bool init(block_dev *base, inode_t *inode);
	addr_t size() const;
	bool isDir() const;
	bool isFile() const;

	//valid for file
	addr_t read(char* buffer, const addr_t offset, const addr_t length) const;
	addr_t write(const char* buffer, const addr_t offset, const addr_t length);

};

//Fill a file with random string
void genfile(file_t &f);

//Fill a file with another file
void copyfile(file_t &dst, const file_t &src);
