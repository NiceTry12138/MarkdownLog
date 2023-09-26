#include <cstddef>
#include <iostream>

class MyAllocator {
private:
	struct obj
	{
		struct obj* next;
	};

public:
	void* allocate(size_t);
	void deallocate(void*, size_t);

private:
	obj* freeStore = nullptr;
	const int CHUNK = 5;	// 标准库是20
	const int min_size = 8;
};

void* MyAllocator::allocate(size_t size)
{
	obj* p = nullptr;
	size =  size > min_size ? size : min_size;
	if (!freeStore) {
		size_t chunk = CHUNK * size;
		freeStore = p = (obj*)malloc(chunk);

		if (p == nullptr) {
			return nullptr;
		}

		// 连接内存块
		for (int i = 0; i < CHUNK - 1; ++i) {
			p->next = (obj*)((char*)p + size);
			p = p->next;
		}

		p->next = nullptr; // last
	}
	p = freeStore;
	freeStore = freeStore->next;
	return p;
}

void MyAllocator::deallocate(void* p, size_t)
{
	// 将 p 收回插入 free list 前端
	((obj*)p)->next = freeStore;
	freeStore = (obj*)p;
}

class Goo {
public:
	long L;
	static MyAllocator myAlloc;

public:
	Goo(long l) : L(l) {}
	static void* operator new(size_t size) {
		return myAlloc.allocate(size);
	}

	static void operator delete(void* pDead, size_t size) {
		myAlloc.deallocate(pDead, size);
	}
};

MyAllocator Goo::myAlloc;

void foo_myALlocator() {
	std::cout << sizeof(Goo) << std::endl;

	size_t const N = 4;
	Goo* p[N];

	for (int i = 0; i < N; ++i) {
		p[i] = new Goo(i);
	}

	for (int i = 0; i < 3; ++i) {
		std::cout << p[i] << std::endl;
	}

	for (int i = 0; i < N; ++i) {
		delete p[i];
	}
}

int main() {
	foo_myALlocator();
	return 0;
}