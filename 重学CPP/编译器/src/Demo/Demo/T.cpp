#include <iostream>

extern const int pt = 10;
//int ppp = 10;	// 与 Demo.,cpp 全局数据区的 ppp 变量冲突

inline int ii = 40;

void PPInline_t() {
	std::cout << ii << std::endl;
}