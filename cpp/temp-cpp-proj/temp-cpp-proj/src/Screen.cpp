#include <cstddef>
#include <iostream>

class Screen {
public:
	Screen(int x): i(x) {};
	int get() { return i; }

	static void* operator new(size_t);
	static void operator delete(void*, size_t);

private:
	Screen* next;
	static Screen* freeStore;
	static const int screenChunk;

private:
	int i;
};

Screen* Screen::freeStore = nullptr;
const int Screen::screenChunk = 24;

void* Screen::operator new(size_t size)
{
	Screen* p = nullptr;
	if (!freeStore) {
		// linked list 为空 需要申请一大块内存
		size_t chunk = screenChunk * size;
		freeStore = p = reinterpret_cast<Screen*>(new char[chunk]);
		// &freeStore[screenChunk - 1] 表示 freeStore 数组中最后一个 Screen 的内存地址 p 指向最后一个 Screen 时表示遍历结束
		for (; p != &freeStore[screenChunk - 1]; ++p) {
			p->next = p + 1;
		}
		p->next = nullptr;
	}

	p = freeStore;
	freeStore = freeStore->next;
	return p;
}

void Screen::operator delete(void* p, size_t)
{
	(static_cast<Screen*>(p))->next = freeStore;
	freeStore = static_cast<Screen*>(p);
}

void foo_Screen() {
	std::cout << sizeof(Screen) << std::endl;

	size_t const N = 100;
	Screen* p[N];

	for (int i = 0; i < N; ++i) {
		p[i] = new Screen(i);
	}

	for (int i = 0; i < 10; ++i) {
		std::cout << p[i] << std::endl;
	}

	for (int i = 0; i < N; ++i) {
		delete p[i];
	}
}

//int main() {
//	foo_Screen(); 
//	return 0;
//}