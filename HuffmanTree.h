#pragma once 

#include <iostream>
#include <vector>
#include <queue>

using namespace std;

//Huffman树的节点
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
	//构造函数
	//构造一个哈夫曼树
	HuffmanTree(W* a, size_t n, const W& invalid) {
		//priority_queue是优先级队列，底层是个大堆，maxheap
		//class Compare = less<typename Container::value_type> 
		//Compare函数本来是less，是大堆，要是改为小堆，compare仿函数要改为“大”
		priority_queue<Node*, vector<Node*>, PrCompare> _minheap;
		//把字符都入堆
		for (size_t i = 0; i < n; ++i) {
			if (a[i] != invalid) {
				//这里虽然入堆得是Node，但是比较的时候，比的是_w的值
				_minheap.push(new Node(a[i]));
			}
		}
		//从堆中取出每个元素然后建树
		while (_minheap.size() > 1) {
			//每次取出堆中最小的两个节点
			Node* left = _minheap.top();
			_minheap.pop();
			Node* right = _minheap.top();
			_minheap.pop();

			//然后new一个父节点，父节点的_w值是子节点和父节点的_w
			Node* parent = new Node(left->_w + right->_w);
			parent->_left = left;
			parent->_right = right;
			
			//然后把这个父节点重新入堆
			_minheap.push(parent);
		}
		//哈弗曼树已经构建造好了
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