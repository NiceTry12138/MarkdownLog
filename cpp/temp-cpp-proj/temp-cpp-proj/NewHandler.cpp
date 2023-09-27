#include <iostream>
#include <new>

using namespace std;

void newErr(void)
{
	cout << "new error" << endl;
	system("pause");
	exit(-1);
}

void foo_new_handler() {

	double* ptr[50];
	set_new_handler(newErr);
	for (int i = 0; i < 50; i++)
	{
		ptr[i] = new double[99999999]; //不断申请空间
		cout << "ptr[" << i << "]" << "->success" << endl;
	}

	system("pause");
}

int main()
{
	foo_new_handler();
	return 0;
}
