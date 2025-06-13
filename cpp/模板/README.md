# C++ 模板

## 函数模板 基本认知

**类型推断中的类型转换**

在类型推断的时候自动的类型转换是受限制的：

- 如果调用参数是按引用传递的，任何类型转换都不被允许。通过模板类型参数T定义的两个参数，它们实参的类型必须完全一样
- 如果调用参数是按值传递的，那么只有退化（decay）这一类简单转换是被允许的：`const` 和 `volatile` 限制符会被忽略，引用被转换成被引用的类型，`raw array` 和 函数 被转换为相应的指针类型。通过模板类型参数T定义的两个参数，它们实参的类型在退化（ `decay` ）后必须一样

```cpp
template<typename T>
T m_max(T& a, T& b)
{
    return a > b ? a : b;
}

template<typename T>
T mmmax(T a, T b)
{
    return a > b ? a : b;
}

int const c = 42;
int i = 1;
int& ri = i;

mmmax(c, c);      // ok
mmmax(c, i);      // ok
mmmax(ri, c);     // ok

m_max(c, c);      // ok
m_max(c, i);      // error 如果模板函数参数是 T& 引用，则任何转换都不允许
m_max(ri, c);     // error 同理 由于参数是 T& 无法转化 
```

**默认调用参数的类型推断**

类型推断并不适用于默认调用参数,比如下面的 `foo_1` 函数,虽然设置了 x 的默认值是 `"12"`,但是编译器并不会认为 `x` 的默认类型是 `std::string`, 需要像 `foo_1_o` 中设置 `T = std::string`,编译器才知道 T 的默认类型是 `std::string`

```cpp
template<typename T>
void foo_1(T x = "12")
{
    std::cout << "x = " << x << std::endl;
}

template<typename T = std::string>
void foo_1_o(T x = "123")
{
    std::cout << "x = " << x << std::endl;
}

foo_1(1);               // 正确的
// foo_1();             // 错误的 因为未明确参数类型 无法给形参 x 定义
foo_1<std::string>();   // 正确的
// foo_1<int>();        // 错误的 定义形参 x 为 int 无法使用默认值 "123"

foo_1_o();              // 正确的 指定 T 的默认类型为 std::string
```

**多个模板参数**

如果想要让函数接受传入两个不同类型的参数,可以通过 `typename` 定义多个类型

```cpp
template<typename T1, typename T2>
T1 mTest(T1 a, T2 b)
{
    std::cout << typeid(a).name() << "  " << typeid(b).name() << std::endl;
    return a < b ? a : b;
}

int main() {
    std::cout << mTest(2, 1.1) << std::endl;
    return 0;
}
```

不过有一个问题,那就是 `mTest` 函数的返回值类型是 T1,以 `mTest(2, 1.1)` 来说返回值类型是 `int`, 但是返回的值却是 `1.1` 导致数据类型转换,最终得到的是 `1`

为了解决这个问题

1. 引入第三个模板参数作为返回类型
2. 让编译器找出返回类型
3. 将返回类型定义为两个参数类型的 **公共类型**

引入第三个模板参数的解决方法如下

```cpp
// 解决方法 1
template<typename RT, typename T1, typename T2>
RT mTest(T1 a, T2 b)
{
    std::cout << typeid(a).name() << "  " << typeid(b).name() << std::endl;
    return a < b ? a : b;
}
// mTest<int, double, double>(2, 1.1);
// mTest<double>(2, 1.1);   // T1 和 T2 的类型能够通过编译器推理出来,只用显式指定 RT 类型为 double 即可
```

让编译器找出返回类型的解决方法如下,使用 C++ 提供的类型推导 `decltype`

| 组成部分 | 作用 | |
| --- | --- | --- |
| b < a | **类型检查**：确保这两个类型可以比较 | 必要 |
| ? a : b | **类型推导**：确定条件表达式的公共类型 | 核心 |

无论 `mTest(1, 1.1)` 还是 `mTest(2, 1.1)` 返回类型都是 `double`, 因为通过 `? a : b` 类型推导得到公共类型是 `double`,与传入参数的 a,b 的具体值没有关系

```cpp
// 解决方法 2
template<typename T1, typename T2>
auto mTest (T1 a, T2 b)-> decltype(b<a?a:b)
{
    return b < a ? a : b;
}
```

> 不要以为 `mTest(1, 1.1)` 返回值类型是 `int`

如果传入参数是 `int` 和 `std::string`, 就算类型比较能够通过,但是 `int` 和 `std::string` 是没有公共类型的 `decltype` 会在编译器报错
