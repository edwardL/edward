#include<cstdio>
#include<cstring>
#include<iostream>
#include<algorithm>
#define maxn 100010
using namespace std;

// https://www.cnblogs.com/JVxie/p/4859889.html

struct Heap
{
	int size, queue[maxn];  ///shuzu
	Heap()
	{
		size = 0;
		for(int i = 0; i < maxn; i++)
			queue[i] = 0;
	}
	void shift_up(int i) // shangfu
	{
		while(i > 1)
		{
			if(queue[i] < queue[i >> 1])
			{
				int tmp = queue[i];
				queue[i] = queue[i >> 1];
				queue[i>>1] = tmp;
			}
			i>>=1;
		}
	}
	void shift_down(int i) // xiachen
	{
		while((i<<1) <= size)
		{
			int next = i << 1;
			if(next < size && queue[next + 1] < queue[next])
				next++;
			if(queue[i] > queue[next])
			{
				int tmp = queue[i];
				queue[i] = queue[next];
				queue[next] = tmp;
				i = next;
			}
			else return;
		}	
	}
	void push(int x)
	{
		queue[++size] = x;
		shift_up(size);
	}

	void pop()
	{
		int tmp = queue[1];
		queue[1] = queue[size];
		queue[size] = tmp;
		size--;
		shift_down(1);
	}
	int top(){return queue[1];}
	bool empty(){return size;}
	void heap_sort()
	{
		int m = size;
		for(int i = 1; i <= size; i ++)
		{
			int tmp = queue[m];
			queue[m] = queue[i];
			queue[i] = tmp;
			m--;
			shift_down(i);
		}
	}
};

int main()
{
	Heap Q;
	int n,a,i,j,k;
	cin >> n;
	for(i = 1; i <= n; i++)
	{
		cin >> a;
		Q.push(a);
	}

	for(i = 1; i <= n; i++)
	{
		cout << Q.top() <<" ";
		Q.pop();
	}
	return 0;
}
