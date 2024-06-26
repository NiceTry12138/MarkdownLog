#include <complex>

using namespace std;

void foo() {

	void* p1 = malloc(521);
	free(p1);

	complex<int>* p2 = new complex<int>;
	delete p2;

	void* p3 = ::operator new(512);
	::operator delete(p3);

#ifdef _MSC_VER
	// 以下两个函数都是non-static，一定要通过object调用，以下分配3个int空间
	int* p4 = allocator<int>().allocate(3, (int*)0);
	// 上面的 (int*)0 暂时没有用
	// 通过 allocator<int>() 创建要给临时对象，执行临死对象的allocate方法
	allocator<int>().deallocate(p4, 3);
	// 通过 allocator<int>() 创建要给临时对象，执行临死对象的deallocate方法
#endif // _MSC_VER
#ifdef __BORLANDC__
	// 以下两个函数都是non-static，一定要通过object调用，以下分配5个int空间
	int* p4 = allocator<int>().allocator(5);
	allocator<int>().deallocate(p4, 5);
#endif // __BORLANDC__
#ifdef __GNUC__
	// 以下两个函数都是static，可通过全名调用，一下分配512bytes
	void* p4 = alloc::allocate(512);
	alloc::deallocate(p4, 512);
#endif // __GNUC__


}

//int main() {
//	foo();
//	return 0;
//}