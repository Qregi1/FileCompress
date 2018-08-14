#pragma once 

#include <string>
#include <fstream>
#include "HuffmanTree.h"

typedef long long LongType;

/*
** �ļ�ѹ��
** ���ļ���ȡ
** Ȼ������ļ�����ͬ�ַ�������
** �ٰ���Щ���������������
** ���������У���Ϊ0����Ϊ1��ͨ�����ַ�ʽ������ַ��ڹ��������е�λ��
*/
struct CharInfo {
	//����ṹ����ǣ�ÿ���ַ��Ľṹ��
	//�ַ����ļ����г��ֵĴ���
	//_code��ʾ�ַ��ڹ��������е�λ�ã���һ��01�ַ���ʾ
	char ch;
	LongType _count;
	string _code;
	
	CharInfo operator+(const CharInfo& ch) {
		CharInfo ret;
		ret._count = _count + ch._count;
		return ret;
	}
	//Ϊ��֧�ֱȽϣ�����Ҫ���رȽϵķ���
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
	//���캯��
	FileCompress() {
		_hashinfo = new CharInfo[256];
		for (size_t i = 0; i < 256; ++i) {
			_hashinfo[i].ch = i;
			_hashinfo[i]._count = 0;
		}
	}

	//�ļ�ѹ��
	void Compress(const char* file) {
		//ͳ���ļ��е��ַ����ֵĴ���
		//out��ʾ���ļ���binary��ʾ��������,file���ļ���
		ifstream ifs(file, ifstream::out | ifstream::binary);
		char ch;
		//ÿ��ȥ��һ���ַ�
		while (ifs.get((ch))) {
			//Ȼ���_hashinfo���ַ���Ӧ��λ�õ�count���м�1���Դ���ͳ���ַ��ĸ���
			_hashinfo[(unsigned char)ch]._count++;
		}
		//�ر�ifs
		//ifs.close();

		//���ݶ�ȡ���ַ�������һ����������
		CharInfo invalid;
		invalid._count = 0;
		HuffmanTree<CharInfo> htree(_hashinfo, 256, invalid);
		//�������������_hashinfo�еĳ��ֵ��ַ���_code���и���
		GenerateHuffmanCode(htree.GetRoot());

		//ѹ���ļ�
		string CompressFile = file;
		CompressFile += ".huffman";
		ofstream ofs(CompressFile.c_str(), ofstream::out | ofstream::binary);

		//д���ַ����ֵĴ����������ѹ��ʱ�ؽ���������
		for (size_t i = 0; i < 256; ++i) {
			if (_hashinfo[i]._count > 0) {
				ofs << (unsigned char)_hashinfo[i].ch << endl;
				ofs << _hashinfo[i]._count << endl;
			}
		}
		//д���ַ�
		//��Ϊĳ���ַ������ܳ������
		ofs << (unsigned char)'\0' << endl;
		ofs << (LongType)0;

		ifs.clear();
		ifs.seekg(0);
		char value = 0;
		size_t pos = 0;
		//��ȡ�ļ����ַ�
	
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

	//��ѹ��
	void UnCompress(const char* file) {
		//ȥ���ļ����ĺ�׺
		string unfile = file;
		size_t pos = unfile.find('.');
		if (pos != string::npos) {
			unfile.erase(pos);
		}
		//��Ȼ���ȡѹ���ļ�

		ifstream ifs(file, ifstream::out | ifstream::binary);
		ofstream ofs(unfile.c_str(), ofstream::out | ofstream::binary);

		unsigned char ch;

		LongType count;
		do {
			//����ȡ�����ļ�������
			//��Ϊ�����ʱ����һ���ַ���һ������
			//���������ȡ��ʱ��Ҳ�������ַ�ʽ��ȡ
		
			ifs >> ch;
			ifs >> count;
			if (count > 0) {
				//��count����0��ʱ�򣬾�������ַ����ڣ�Ȼ���д��count��
				_hashinfo[(unsigned char)ch]._count = count;
			}
			//ֱ������'\0'����������'\0'�Ĵ�����0
		} while (count > 0);

		//������������
		CharInfo invalid;
		invalid._count = 0;
		HuffmanTree<CharInfo> htree(_hashinfo, 256, invalid);
		Node* root = htree.GetRoot();
		LongType file_size = root->_w._count;

		char value;
		Node* cur = root;
		//ÿ�ζ�ȡ�˸��ֽ�
		while (ifs.get(value)) {
			for (size_t i = 0; i < 8; ++i) {
				if (value & (1 << i)) {
					cur = cur->_right;
				}
				else {
					cur = cur->_left;
				}
				//����Ҷ�ڵ㣬�Ͱ��ַ�ȡ��д�뵽����ļ���
				if (cur->_left == NULL && cur->_right == NULL) {
					ofs.put((unsigned char)cur->_w.ch);
					cur = root;

					if (--file_size == 0) {
						break;
					}
				}//����Ҷ�ӽ��
			}//forѭ����ѭ����ȡ�˸��ֽڵ�����
		}//whileѭ������
		ifs.close();
		ofs.close();
	}//��ѹ������

	//ͨ��������������ÿ���ַ��γ�HuffmanCode���ݹ鴦��
	void GenerateHuffmanCode(Node* root) {
		if (root == NULL) {
			return;
		}
		if (root->_left == NULL && root->_right == NULL) {
			//��ǰ�ڵ���Ҷ�ӽڵ�
			//�ڹ��������У�Ҷ�ӽڵ�������ַ�
			//���ǰ��ҵ���Ҷ�ӽ���_code����_hashinfo�е��ַ���
			_hashinfo[root->_w.ch]._code = root->_w._code;
			
			//��һ�в���Ҫ
			//_hashinfo[root->_w.ch]._count = root->_w._count;
		}
		if (root->_left != NULL) {
			//�����������Ϊ�գ��Ͱѵ�ǰ�ڵ��_code��0Ȼ��ֵ����ǰ�ڵ�root��_left
			root->_left->_w._code = root->_w._code + "0";
			GenerateHuffmanCode(root->_left);
		}
		if (root->_right != NULL) {
			//�ҽڵ�ͬ��
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