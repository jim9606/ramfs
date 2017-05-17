#include"misc.h"
#include"utils.h"
#include<algorithm>

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


bool file_t::init(char *base, inode_t *inode) {
	this->base = base;
	this->inode = inode;
	if (size() == 0)
		return true;
	data_addr.push_back((data_block_t*)base + inode->d_ref);
	if (size() <= BLOCK_SIZE)
		return true;
	indirect_block_t *i_block = (indirect_block_t*)(base + inode->i_ref * BLOCK_SIZE);
	for (size_t i = 0; i < i_block->rec_count; ++i) {
		data_addr.push_back((data_block_t*)base + i_block->block_no[i]);
	}

	if (isDir()) {
		for (auto it : data_addr) {
			//TODO:
		}
	}
	return true;
}

size_t file_t::size() const {
	return (size_t)inode->size;
}

bool file_t::isDir() const {
	return inode->flags & inode_t::f_dir;
}

bool file_t::isFile() const {
	return inode->flags & inode_t::f_file;
}

size_t file_t::read(char* buffer, const addr_t offset, const addr_t length) const {
	addr_t roff = offset, rlen = length, fsize = size(), r = 0;
	if (offset > fsize) {
		logerr("file_t.read", "Too large offset");
		return 0;
	}
	if (length == 0) return 0;
	auto it = data_addr.cbegin(),cend = data_addr.cend();
	it += roff / BLOCK_SIZE;
	roff %= BLOCK_SIZE;
	if (roff + rlen <= BLOCK_SIZE) {
		memcpy(buffer,(char*)(*it) + roff , rlen);
		rlen = 0;
	}
	for (; rlen > 0 && it != cend; ++it) {
		size_t cr = min(length, BLOCK_SIZE);
		memcpy(buffer + r, (char*)(*it), cr);
		rlen -= cr; r += cr;
	}
	return r;
}

size_t file_t::write(char* buffer, const addr_t offset, const addr_t length) {
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
	if (roff + rlen <= BLOCK_SIZE) {
		memcpy((char*)(*it) + roff, buffer, rlen);
		rlen = 0;
	}
	for (; rlen > 0 && it != iend; ++it) {
		size_t cr = min(length, BLOCK_SIZE);
		memcpy((char*)(*it), buffer + r, cr);
		rlen -= cr; r += cr;
	}
	return r;
}