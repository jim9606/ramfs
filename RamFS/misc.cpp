#include "misc.h"
#include "utils.h"
#include <algorithm>

using std::min;

size_t path_t::size() const {
	return n.size();
}

string path_t::get(const size_t c) const {
	return n.at(c);
}

string path_t::format() const {
	string r;
	for (const string &it : n)
		r.append("/").append(it);
	return r;
}

void path_t::push(const string &s) {
	n.push_back(s);
}

void path_t::pop() {
	n.pop_back();
}

file_t::file_t(block_dev *base, inode_t *inode) {
	init(base, inode);
}

bool file_t::init(block_dev *base, inode_t *inode) {
	this->base = base;
	this->inode = inode;
	if (size() == 0)
		return true;
	data_addr.push_back((data_block_t*)base + inode->d_ref);
	if (size() <= BLOCK_SIZE)
		return true;
	indirect_block_t *i_block = (indirect_block_t*)base + inode->i_ref;
	for (size_t i = 1; i < inode->rec_count; ++i) {
		data_addr.push_back((data_block_t*)base + i_block->block_no[i]);
	}

	if (isDir()) {
		auto it = data_addr.cbegin();
		for (size_t i = 0; i < inode->rec_count; ++it) {
			dir_block_t *db = (dir_block_t*)(*it);
			for (size_t j = 0; j < INODE_REC_PER_DIRBLOCK && i < inode->rec_count; ++j, ++i)
				sub_inode_rec.push_back(db->rec[j]);
		}
	}
	return true;
}

addr_t file_t::size() const {
	return (size_t)inode->size;
}

bool file_t::isDir() const {
	return inode->flags & inode_t::f_dir;
}

bool file_t::isFile() const {
	return inode->flags & inode_t::f_file;
}

addr_t file_t::read(char* buffer, const addr_t offset, const addr_t length) const {
	if (!isFile()) {
		logerr("file_t.read", "Try to read a directory");
		return 0;
	}
	addr_t roff = offset, rlen = length, fsize = size(), r = 0;
	if (offset > fsize) {
		logerr("file_t.read", "Too large offset");
		return 0;
	}
	if (length == 0) return 0;
	auto it = data_addr.cbegin(),cend = data_addr.cend();
	it += roff / BLOCK_SIZE;
	roff %= BLOCK_SIZE;
	//Read first target block
	if (roff + rlen <= BLOCK_SIZE) {
		memcpy(buffer,(char*)(*it) + roff , rlen);
		rlen = 0;
	}
	//Read more block
	for (; rlen > 0 && it != cend; ++it) {
		addr_t cr = min(length,(addr_t)BLOCK_SIZE);
		memcpy(buffer + r, (char*)(*it), cr);
		rlen -= cr; r += cr;
	}
	return r;
}

addr_t file_t::write(const char* buffer, const addr_t offset, const addr_t length) {
	if (!isFile()) {
		logerr("file_t.write", "Try to write a directory");
		return 0;
	}
	addr_t roff = offset, rlen = length, fsize = size(), r = 0;
	if (offset > fsize) {
		logerr("file_t.write", "Too large offset");
		return 0;
	}
	else if (offset + length > fsize){
		logerr("file_t.write", "Too large length");
		return 0;
	}
	if (length == 0) return 0;
	auto it = data_addr.begin(), iend = data_addr.end();
	it += roff / BLOCK_SIZE;
	roff %= BLOCK_SIZE;
	//Write the first block
	if (roff + rlen <= BLOCK_SIZE) {
		memcpy((char*)(*it) + roff, buffer, rlen);
		rlen = 0;
	}
	//Write more block
	for (; rlen > 0 && it != iend; ++it) {
		addr_t cr = min(length, (addr_t)BLOCK_SIZE);
		memcpy((char*)(*it), buffer + r, cr);
		rlen -= cr; r += cr;
	}
	return r;
}

//Fill a file with random string
void genfile(file_t &f) {
	char buffer[BLOCK_SIZE];
	addr_t len = f.size(), offset = 0;
	while (len > BLOCK_SIZE) {
		fillbuffer(buffer, BLOCK_SIZE);
		f.write(buffer, offset, BLOCK_SIZE);
		offset += BLOCK_SIZE; len -= BLOCK_SIZE;
	}
	if (len > 0) {
		fillbuffer(buffer, len);
		f.write(buffer, offset, len);
	}
}

//Fill a file with another file
void copyfile(file_t &dst, const file_t &src) {
	char buffer[BLOCK_SIZE];
	addr_t len = src.size(), offset = 0;
	while (len > BLOCK_SIZE) {
		src.read(buffer, offset, BLOCK_SIZE);
		dst.write(buffer, offset, BLOCK_SIZE);
		offset += BLOCK_SIZE; len -= BLOCK_SIZE;
	}
	if (len > 0) {
		src.read(buffer, offset, len);
		dst.write(buffer, offset, len);
	}
}
