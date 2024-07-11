# 程序员的自我修养

## 静态链接

源码执行的过程可以被分成四部分

1. 预处理(Prepressing)：主要处理那些源码中以 `#` 开始的预编译指令，例如 `#include`、`#define`
2. 编译(Compilation)：把预处理完的文件进行一系列**词法分析**、**语法分析**、**语义分析**以及**优化**后生产相应的**汇编代码文件**
3. 汇编(Assembly)：汇编器是将汇编代码转变成机器可以执行的指令，每一个汇编语句几乎都对应一条机器指令。汇编器根据汇编指令和机器指令的对照表一一翻译就可以了
4. 链接(Linking)

以 `hello.c`、`hello1.c`、`hello2.c` 为例

```cpp
// hello2.c
int mul(int a, int b){
 return a * b;
}

// hello1.c
#include "hello2.c"
int add(int a, int b) {
 return a + b;
}

// hello.c
#include "hello1.c"
int main() {
 add(1, 2);
 return 0;
}
```

使用命令 `gcc -E hello.c -o hello.i`，对 `hello.c` 进行 **预处理**

```cpp
# 0 "hello.c"
# 0 "<built-in>"
# 0 "<command-line>"
# 1 "/usr/include/stdc-predef.h" 1 3 4
# 0 "<command-line>" 2
# 2 "hello.c"
# 1 "hello1.c" 1
# 1 "hello2.c" 1
int mul(int a, int b){
 return a * b;
}

# 2 "hello1.c" 2
int add(int a, int b) {
 return a + b;
}
# 2 "hello.c" 2
int main() {
 add(1, 2);
 return 0;
}
```

> `#include` 会递归执行

- 在预处理阶段删除 `#define` 并展开所有的宏定义
- 处理的条件预编译指令 `#if`、`#ifdef`、`#elif`、`#else`、`#endif`
- 处理 `#include` 将包含的文件插入到该预编译指令的位置
- 删除所有的注释
- 添加行号和文件名标识
- 保留 `#pragma` 编译器指令，编译器要用到

使用 `gcc -S hello.i -o hello.s` 将预处理后的文件编译得到汇编代代码

> 汇编文件内容较多，不贴代码

使用 `gcc -c hello.s -o hello.o` 得到机器指令的文件，但是这个文件还不可以执行

在最后执行了**链接**之后，才能够执行

