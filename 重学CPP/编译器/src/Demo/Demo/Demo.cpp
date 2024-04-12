
#include <iostream>

//extern void testX();

//int ppp = 30;   // 与 T.cpp 全局数据区中定义的 ppp 变量冲突

void Test();
void Test();
void Test();
void Test();
void Test();

void PPInline_t();
 
extern const int pt;

inline int ii = 20;

void Text() {
    std::cout << "Test" << std::endl;
}

void PPInline() {
    std::cout << ii << std::endl;
}

//void Text() {
//    std::cout << "Test" << std::endl;
//}

int main()
{
    Text();
    PPInline();
    PPInline_t();
    
    std::cout << pt << std::endl;   // 输出 10
}
