#include <iostream>
#include <string>
#include "fsimpl.h"
#include "utils.h"
#include "time.h"
using namespace std;

void PrintStructSize() {
	cout << sizeof(inode_t) << endl;
	cout << sizeof(sub_inode_rec_t) << endl;
	cout << sizeof(dir_block_t) << endl;
	cout << sizeof(indirect_block_t) << endl;
	cout << sizeof(data_block_t) << endl;
	cout << sizeof(inode_tuple_t) << endl;
	cout << sizeof(superblock_t) << endl;
	cout << sizeof(block_dev) << endl;
}

fsimpl fs;

int main(int argc, char **argv) {
	//PrintStructSize();
	//TODO:CLI Parsing
	cout << time(0) << endl;
	return 0;
}
