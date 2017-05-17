#include<iostream>
#include<string>
#include<cstdlib>
#include"block_fs.h"
using namespace std;


int main(int argc, char **argv) {
	cout << sizeof(inode_t) << endl;
	cout << sizeof(sub_inode_rec_t) << endl;
	cout << sizeof(dir_block_t) << endl;
	cout << sizeof(indirect_block_t) << endl;
	cout << sizeof(data_block_t) << endl;
	cout << sizeof(inode_tuple_t) << endl;
	cout << sizeof(superblock_t) << endl;
	cout << sizeof(block_dev) << endl;
	return 0;
}
