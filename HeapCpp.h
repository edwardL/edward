#include <vector>
#include <iostream>
#include <functional>

#define MAX_VALUE 999999

const int startIndex = 1; //容器中堆元素开始索引

using namespace std;

template<class ElemType, class Compare = less<ElemType> >
class MyHeap {
	private:
		vector<ElemType> heapDataVec;
		int numCounts;
		Compare comp; //比较规则

	public:
		MyHeap();
		vector<ElemType> getVec();
		void initHeap(ElemType *data, const int n);
		void printfHeap();
		void makeHeap(); //建堆
		void sortHeap();
		void pushHeap(ElemType elem);
		void popHeap();
		void adjustHeap(int childTree, ElemType adjustValue); //调整子树
		void percolateUp(int holeIndex, ElemType adjustValue); //上溯操作
};

template<class ElemType, class Compare>
MyHeap<ElemType, Compare>::MyHeap()
:numCounts(0)
{
	heapDataVec.push_back(MAX_VALUE);
}

template<class ElemType,class Compare>
vector<ElemType> MyHeap<ElemType,Compare>::getVec()
{
	return heapDataVec;
}

template<class ElemType, class Compare>
void MyHeap<ElemType,Compare>::initHeap(ElemType *data, const int n)
{
	//拷贝元素数据到vector
	for(int i = 0; i < n; ++i)
	{
		heapDataVec.push_back(*(data + i));
		++numCounts;
	}
}

template<class ElemType, class Compare>
void MyHeap<ElemType,Compare>::printfHeap()
{
	cout << "Heap : ";
	for(int i = 0; i < numCounts; ++i)
	{
		cout <<  heapDataVec[i] << " ";
	}
	cout << endl;
}

template<class ElemType, class Compare>
void MyHeap<ElemType,Compare>::makeHeap()
{
	//建堆的过程就是一个不断调整堆的过程， 循环调用函数adjustHeap 依次调整子树	
	if(numCounts < 2)
		return;
	// 第一个需要调整的子树的根节点
	int parent = numCounts / 2;
	while(1)
	{
		adjustHeap(parent,heapDataVec[parent]);
		if(startIndex == parent) // 到达根节点
			return;
		--parent;
	}
}

template<class ElemType, class Compare>
void MyHeap<ElemType,Compare>::sortHeap()
{
	//堆排序
	while(numCounts > 0)
		popHeap();
}

template<class ElemType, class Compare>
void MyHeap<ElemType, Compare>::pushHeap(ElemType elem)
{
	heapDataVec.push_back(elem);
	++numCounts;
	// 执行一次上溯操作，调整堆
	percolateUp(numCounts,heapDataVec[numCounts]);
}

template<class ElemType, class Compare>
void MyHeap<ElemType,Compare>::popHeap()
{
	ElemType adjustValue = heapDataVec[numCounts];
	heapDataVec[numCounts] = heapDataVec[startIndex];
	--numCounts;
	adjustHeap(startIndex,adjustValue);
}

///// 调整childTree 为根的子树为堆
template<class ElemType, class Compare>
void MyHeap<ElemType,Compare>::adjustHeap(int childTree, ElemType adjustValue)
{
	// 洞节点索引
	int holeIndex = childTree;
	int secondChild = 2 * holeIndex + 1; // 右节点
	while(secondChild <= numCounts)
	{
		if(comp(heapDataVec[secondChild],heapDataVec[secondChild - 1]))
		{
			--secondChild;
		}
		// 上溯
		heapDataVec[holeIndex] = heapDataVec[secondChild];
		holeIndex = secondChild;
		secondChild = 2 * secondChild + 1;
	}
	// 如果洞节点只有左节点
	if(secondChild == numCounts + 1)
	{
		heapDataVec[holeIndex] = heapDataVec[secondChild + 1];
	}
	// 将调整至赋予洞节点
	heapDataVec[holeIndex] = adjustValue;
	
	percolateUp(holeIndex,adjustValue);
}

//上溯操作
template<class ElemType,class Compare>
void MyHeap<ElemType,Compare>::percolateUp(int holeIndex, ElemType adjustValue)
{
	int parentIndex = holeIndex / 2;
	while(holeIndex > startIndex && comp(heapDataVec[parentIndex],adjustValue))
	{
		heapDataVec[holeIndex] = heapDataVec[parentIndex];
		holeIndex = parentIndex;
		parentIndex /= 2;
	}
	heapDataVec[holeIndex] = adjustValue; // 将新值放置在正确的位置
}
