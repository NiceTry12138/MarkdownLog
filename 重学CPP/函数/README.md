# 函数

- 函数可以让代码模块化，便于阅读维护
- 代码模块化之后，能够实现分工合作
- 减少重复代码，降低工作量

```cpp
// 返回类型 函数名称(参数1, 参数2, 参数3...) {
//     函数功能区
//     renturn 返回值;
// }

int add(int a, int b) {
    int result = a + b;
    return result;
}
```

> 一般函数定义如上

## 函数参数：指针参数

```cpp
int add(int a, int b) {
    a *= 100;
    b *= 10;
    return a + b;
}

int a = 2, b = 1;
int c = add(a, b);
```

上面函数运行结束之后，全局定义的 a、b 的值并不会被修改为 200 和 10，因为函数参数 a、b 是函数作用域的临时变量，相当于全局作用域的 a 给函数作用域的 a 赋值，只是变量名相同

```cpp
int add(int* a, int* b) {
    a *= 100;
    b *= 10;
    return a + b;
}

int x = 2, y = 1;
int z = add(&x, &y);
```

如果 `add` 函数参数接受的是变量地址，那么通过地址可以直接修改 x、y 的值

```cpp
int add(int &a, int &b) {
    a *= 100;
    b *= 10;
    return a + b;
}

int x = 2, y = 1;
int z = add(x, y);
```

如果 `add` 函数参数接受的是引用，那么可以直接通过引用的方式来修改原来的值

## 默认参数

默认参数智能放在最后

```cpp
int add(int a = 100, int b = 100);  // 正确
int add(int a = 100, int b);    // 错误
int add(int a, int b, int c = 100); // 正确
int add(int a, int b = 100, int c); // 错误
int add(int a, int& b = 100);   // 错误 int& 无法使用默认参数
```

## 不定量参数

如果遇到想要求多个数的和，但是不确定数的个数，也就不能确定函数参数个数，这种情况需要使用不定量参数

再比如一些控制台命令，根据你参数的个数会启动不同的功能

