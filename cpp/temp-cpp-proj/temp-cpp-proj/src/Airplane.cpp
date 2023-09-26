#include <cstddef>
#include <iostream>

class Airplane {
private:
	struct AirplaneRep {
		unsigned long miles;
		char type;
	};

private:
	union {
		AirplaneRep rep;	// 指针对使用中的 objects
		Airplane* next;		// 指针对 fire list 中的object
	};

public:
	unsigned long getMiles() { return rep.miles; }
	char getType() { return rep.type; }

	void set(unsigned long m, char t) {
		rep.miles = m; rep.type = t;
	}

public:
	static void* operator new (size_t size);
	static void operator delete (void* deadObject, size_t size);

private:
	static const int BLOCK_SIZE;
	static Airplane* headOfFreeList;
};

Airplane* Airplane::headOfFreeList = nullptr;
const int Airplane::BLOCK_SIZE = 512;

void* Airplane::operator new(size_t size) {
	// 如果大小有误 转交 ::operator new
	if(size!=sizeof(Airplane)) {
		return ::operator new(size);
	}

	Airplane* p = headOfFreeList;
	if (p) {
		headOfFreeList = p->next;
	}
	else {
		// free list 为空 需要申请初始内存
		Airplane* newBlock = static_cast<Airplane*>(::operator new(BLOCK_SIZE * sizeof(Airplane)));
		
		// 将小块串成 free list
		// 但跳过 #0 因为它将作为本次返回结果
		for (int i = 1; i < BLOCK_SIZE - 1; i++)
		{
			newBlock[i].next = &newBlock[i + 1];
		}
		newBlock[BLOCK_SIZE - 1].next = nullptr;
		p = newBlock;
		headOfFreeList = &newBlock[1];
	}
	return p;
}

void Airplane::operator delete(void* p, size_t size)
{
	if (p == nullptr) {
		return;
	}

	if (size != sizeof(Airplane)) {
		::operator delete(p);
		return;
	}

	Airplane* carcass = static_cast<Airplane*>(p);
	carcass->next = headOfFreeList;
	headOfFreeList = carcass;
}

void foo_Ariplane() {
	std::cout << sizeof(Airplane) << std::endl;

	size_t const N = 100;
	Airplane* p[N];

	for (int i = 0; i < N; ++i) {
		p[i] = new Airplane();
	}

	for (int i = 0; i < 10; ++i) {
		std::cout << p[i] << std::endl;
	}

	for (int i = 0; i < N; ++i) {
		delete p[i];
	}
}

//int main() {
//	foo_Ariplane();
//	return 0;
//}