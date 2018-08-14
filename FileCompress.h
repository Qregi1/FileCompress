#pragma once 

#include <string>
#include <fstream>
#include "HuffmanTree.h"

typedef long long LongType;

/*
** 文件压缩
** 把文件读取
** 然后计算文件中相同字符的数量
** 再把这些东西存入哈夫曼树
** 哈夫曼树中，左为0，右为1，通过这种方式来标记字符在哈夫曼树中的位置
*/
struct CharInfo {
	//这个结构体就是，每个字符的结构体
	//字符在文件找中出现的次数
	//_code表示字符在哈夫曼树中的位置，用一串01字符表示
	char ch;
	LongType _count;
	string _code;
	
	CharInfo operator+(const CharInfo& ch) {
		CharInfo ret;
		ret._count = _count + ch._count;
		return ret;
	}
	//为了支持比较，所以要重载比较的符号
	bool operator>(const CharInfo& ch) {
		return _count > ch._count;
	}
	bool operator<(const CharInfo& ch) {
		return _count < ch._count;
	}
	bool operator!=(const CharInfo& ch) {
		return _count != ch._count;
	}
};

class FileCompress {
	typedef HuffmanTreeNode<CharInfo> Node;
public:
	//构造函数
	FileCompress() {
		_hashinfo = new CharInfo[256];
		for (size_t i = 0; i < 256; ++i) {
			_hashinfo[i].ch = i;
			_hashinfo[i]._count = 0;
		}
	}

	//文件压缩
	void Compress(const char* file) {
		//统计文件中的字符出现的次数
		//out表示读文件，binary表示二进制流,file是文件名
		ifstream ifs(file, ifstream::out | ifstream::binary);
		char ch;
		//每次去读一个字符
		while (ifs.get((ch))) {
			//然后对_hashinfo中字符对应的位置的count进行加1，以此来统计字符的个数
			_hashinfo[(unsigned char)ch]._count++;
		}
		//关闭ifs
		//ifs.close();

		//根据读取的字符，构建一个哈弗曼树
		CharInfo invalid;
		invalid._count = 0;
		HuffmanTree<CharInfo> htree(_hashinfo, 256, invalid);
		//根据这棵树，把_hashinfo中的出现的字符的_code进行更新
		GenerateHuffmanCode(htree.GetRoot());

		//压缩文件
		string CompressFile = file;
		CompressFile += ".huffman";
		ofstream ofs(CompressFile.c_str(), ofstream::out | ofstream::binary);

		//写入字符出现的次数，方便解压缩时重建哈弗曼树
		for (size_t i = 0; i < 256; ++i) {
			if (_hashinfo[i]._count > 0) {
				ofs << (unsigned char)_hashinfo[i].ch << endl;
				ofs << _hashinfo[i]._count << endl;
			}
		}
		//写入字符
		//因为某个字符不可能出现零次
		ofs << (unsigned char)'\0' << endl;
		ofs << (LongType)0;

		ifs.clear();
		ifs.seekg(0);
		char value = 0;
		size_t pos = 0;
		//读取文件的字符
	
		while (ifs.get(ch)) {
			string& code = _hashinfo[(unsigned char)ch]._code;
			for (size_t i = 0; i < 256; ++i) {
				if (code[i] == '0') {
					value &= ~(1 << pos);
				}
				else if (code[i] == '1') {
					value |= (1 << pos);
				}
				else if (code[i] == '\0') {
					break;
				}
				else {
					throw exception("invalid huffman code");
				}
				++pos;
				if (pos == 8) {
					ofs.put(value);
					printf("%x\n", value);
					value = 0;
					pos = 0;
				}
			}
		}
		if (pos > 0) {
			ofs.put(value);
		}
		ifs.close();
		ofs.close();
	}

	//解压缩
	void UnCompress(const char* file) {
		//去掉文件名的后缀
		string unfile = file;
		size_t pos = unfile.find('.');
		if (pos != string::npos) {
			unfile.erase(pos);
		}
		//打开然后读取压缩文件

		ifstream ifs(file, ifstream::out | ifstream::binary);
		ofstream ofs(unfile.c_str(), ofstream::out | ofstream::binary);

		unsigned char ch;

		LongType count;
		do {
			//用析取器从文件中输入
			//因为输入的时候，是一个字符，一个次数
			//所以这里读取的时候，也按照这种方式读取
		
			ifs >> ch;
			ifs >> count;
			if (count > 0) {
				//当count大于0的时候，就是这个字符存在，然后就写入count中
				_hashinfo[(unsigned char)ch]._count = count;
			}
			//直到读到'\0'，我们设置'\0'的次数是0
		} while (count > 0);

		//创建哈夫曼树
		CharInfo invalid;
		invalid._count = 0;
		HuffmanTree<CharInfo> htree(_hashinfo, 256, invalid);
		Node* root = htree.GetRoot();
		LongType file_size = root->_w._count;

		char value;
		Node* cur = root;
		//每次读取八个字节
		while (ifs.get(value)) {
			for (size_t i = 0; i < 8; ++i) {
				if (value & (1 << i)) {
					cur = cur->_right;
				}
				else {
					cur = cur->_left;
				}
				//遇到叶节点，就把字符取出写入到输出文件中
				if (cur->_left == NULL && cur->_right == NULL) {
					ofs.put((unsigned char)cur->_w.ch);
					cur = root;

					if (--file_size == 0) {
						break;
					}
				}//遇到叶子结点
			}//for循环，循环读取八个字节的数据
		}//while循环结束
		ifs.close();
		ofs.close();
	}//解压缩结束

	//通过哈弗曼树，对每个字符形成HuffmanCode，递归处理
	void GenerateHuffmanCode(Node* root) {
		if (root == NULL) {
			return;
		}
		if (root->_left == NULL && root->_right == NULL) {
			//当前节点是叶子节点
			//在哈弗曼树中，叶子节点绝对是字符
			//我们把找到的叶子结点的_code存入_hashinfo中的字符上
			_hashinfo[root->_w.ch]._code = root->_w._code;
			
			//这一行不需要
			//_hashinfo[root->_w.ch]._count = root->_w._count;
		}
		if (root->_left != NULL) {
			//如果左子树不为空，就把当前节点的_code加0然后赋值给当前节点root的_left
			root->_left->_w._code = root->_w._code + "0";
			GenerateHuffmanCode(root->_left);
		}
		if (root->_right != NULL) {
			//右节点同上
			root->_right->_w._code = root->_w._code + "1";
			GenerateHuffmanCode(root->_right);
		}
	}
	~FileCompress() {
		delete[] _hashinfo;
	}
protected:
	CharInfo* _hashinfo;
};

void TestFileCompress() {
	//CharInfo ch1, ch2, ch3, ch4;
	//ch1.ch = 'a';
	//ch1._count = 4;

	//ch2.ch = 'b';
	//ch2._count = 3;

	//ch3.ch = 'c';
	//ch3._count = 2;
	//
	//ch4.ch = 'd';
	//ch4._count = 1;

	//CharInfo ch_end;
	//
	//CharInfo arr[5] = { ch1, ch2, ch3, ch4, ch_end };
	//HuffmanTree<CharInfo> h(arr, size_t(5), ch_end);
	//FileCompress f;
	//f.GenerateHuffmanCode(h.GetRoot());

	FileCompress fc;
	fc.Compress("Compress.txt");
}

void TestFileUnCompress() {
	FileCompress fc;
	fc.Compress("Compress.txt");
	fc.UnCompress("Compress.txt.huffman");
}