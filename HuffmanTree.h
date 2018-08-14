#pragma once 

#include <iostream>
#include <vector>
#include <queue>

using namespace std;

//Huffman���Ľڵ�
template<typename W>
struct  HuffmanTreeNode {
	W _w;
	HuffmanTreeNode<W>* _left;
	HuffmanTreeNode<W>* _right;

	HuffmanTreeNode(const W& w)
		:_w(w)
		, _left(NULL)
		, _right(NULL)
	{}

};

template<typename W>
class HuffmanTree {
	typedef HuffmanTreeNode<W> Node;
	struct PrCompare {
		bool operator()(Node* n1, Node* n2) {
			return n1->_w > n2->_w;
		}
	};
public:
	//���캯��
	//����һ����������
	HuffmanTree(W* a, size_t n, const W& invalid) {
		//priority_queue�����ȼ����У��ײ��Ǹ���ѣ�maxheap
		//class Compare = less<typename Container::value_type> 
		//Compare����������less���Ǵ�ѣ�Ҫ�Ǹ�ΪС�ѣ�compare�º���Ҫ��Ϊ����
		priority_queue<Node*, vector<Node*>, PrCompare> _minheap;
		//���ַ������
		for (size_t i = 0; i < n; ++i) {
			if (a[i] != invalid) {
				//������Ȼ��ѵ���Node�����ǱȽϵ�ʱ�򣬱ȵ���_w��ֵ
				_minheap.push(new Node(a[i]));
			}
		}
		//�Ӷ���ȡ��ÿ��Ԫ��Ȼ����
		while (_minheap.size() > 1) {
			//ÿ��ȡ��������С�������ڵ�
			Node* left = _minheap.top();
			_minheap.pop();
			Node* right = _minheap.top();
			_minheap.pop();

			//Ȼ��newһ�����ڵ㣬���ڵ��_wֵ���ӽڵ�͸��ڵ��_w
			Node* parent = new Node(left->_w + right->_w);
			parent->_left = left;
			parent->_right = right;
			
			//Ȼ���������ڵ��������
			_minheap.push(parent);
		}
		//���������Ѿ����������
		_root = _minheap.top();
	}

	~HuffmanTree() {
		clear(_root);
		_root = NULL;
	}
	void clear(Node* root) {
		if (root == NULL) return;

		clear(root->_left);
		clear(root->_right);

		delete root;
	}
	Node* GetRoot() {
		return _root;
	}

private:
	HuffmanTree(const HuffmanTree<W>& h);
	HuffmanTree<W>& operator=(const HuffmanTree<W>& h);
protected:
	Node* _root;
};


#define null -1

void TestHuffManTree() {

	/*int a[5] = { 4, 3, 2, 1, null };
	size_t sz = 5;
	HuffmanTree<int> h(a, sz, -1);*/
}