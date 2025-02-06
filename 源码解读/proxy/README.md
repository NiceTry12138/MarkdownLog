# proxy

## 前置内容

### std::variant 和 std::optional

一些时候需要特殊标记，比如 `A GetA()` 这函数，期望返回一个对象 A，但是 `GetA()` 函数没有办法返回一个有效的 A 对象，但是又必须返回一个 A 对象时，就会非常难受

这个时候可以修改为 `bool GetA(A& InA)` 通过引用的方式来获取对象 A，通过函数返回值判断是否成功获取，但是无论如何都需要外界构建一个对象 A 传入到函数中

或者使用 C++17 引入的 `std::optional<T>` 来处理这个问题

创建 `optional` 可以使用

```cpp
//初始化为空
std::optional<int> emptyInt;
std::optional<double> emptyDouble = std::nullopt;
//直接用有效值初始化
std::optional<int> intOpt{10};
std::optional intOptDeduced{10.0}; // auto deduced
//使用make_optional
auto doubleOpt = std::make_optional(10.0);
auto complexOpt = std::make_optional<std::complex<double>>(3.0, 4.0);
//使用in_place
std::optional<std::complex<double>> complexOpt{std::in_place, 3.0, 4.0};
std::optional<std::vector<itn>> vectorOpt{std::in_place, {1, 2, 3}};
//使用其它optional对象构造
auto optCopied = vectorOpt;
```

不同的地方在于，对于一个类 `class A` 测试下面代码

```cpp
#include <iostream>
#include <optional>

class A
{
public:
    A()
    {
        std::cout << "A()" << std::endl;
    }
    
    ~A()
    {
        std::cout << "~A()" << std::endl;
    }
    
    A(A&& In)
    {
        std::cout << "A(A&& In)" << std::endl;
    }
};

int main() {
    std::optional<A> T1{A()};
    std::cout << "-------------" << std::endl;
    std::optional<A> T2{std::in_place};
    std::cout << "-------------" << std::endl;

    return 0;
}
```

输出内容如下

```
A()
A(A&& In)
~A()
-------------
A()
-------------
~A()
~A()
```

可以发现 `std::optional<A> T1{A()}` 创建了两个 A 对象，而 `std::optional<A> T2{std::in_place}` 只创建了一次 A 对象

`std::in_place` 有三种辅助类型：

- `std::in_place_t` 类型和全局值 `std::in_place`，用于 `std::optional`
- `std::in_place_type_t` 类型和全局值 `std::in_place_type`，用于 `std::variant` 和 `std::any`
- `std::in_place_index_t` 类型和全局值 `std::in_place_index`，用于 `std::variant`

这些辅助类型用于有效地原位初始化对象，而无需额外的临时拷贝或移动操作

如果想要判断 `std::optional` 是否存在有效值，可以使用 `has_value()` 进行判断，该函数会返回一个 `bool` 值，可以使用 `value()` 函数来获取

其实 `std::optional` 具体实现可以参考下面这段代码，本质上就是一个是否存在有效值的 `_Has_value` 和一个存储原始数据的 `_Value`，然后维护这个结构体

```cpp
template <class _Ty, bool = is_trivially_destructible_v<_Ty>>
struct _Optional_destruct_base { 
    union {
        _Nontrivial_dummy_type _Dummy;
        remove_const_t<_Ty> _Value;
    };
    bool _Has_value;

    constexpr _Optional_destruct_base() noexcept : _Dummy{}, _Has_value{false} {} // initialize an empty optional
    // 其他方法
};
```

`std::variant` 是 C++17 引入的一个新的模板类，提供了一种存储不同类型的值的方式，类似于之前的 `union`，能够在运行时进行类型检查和转换，具有更多的功能和更高的类型安全性

```cpp
#include <iostream>
#include <variant>
#include <string>

int main()
{
    std::variant<int, double, std::string> value;

    value = 110;
    std::cout << "The value is an integer: " << std::get<int>(value) << std::endl;

    value = 0.618;
    std::cout << "The value is a double: " << std::get<double>(value) << std::endl;

    value = "hello world";
    std::cout << "The value is a string: " << std::get<std::string>(value) << std::endl;

    // value = true;        // Compilation error: cannot convert type bool to any of the alternative types
    // std::get<int>(value) // std::bad_variant_access exception: value holds a different type

    return 0;
}
```

为了方便初始化，`std::variant` 提供了很方便的初始化操作

```cpp
std::variant<int, float, std::string> T {1.0f};
```

但是这也出现了一个问题，如果出现了歧义该怎么办，比如 `double` 转换成 `int` 还是 `float`，编译器会直接报错

```cpp
std::variant<int, float> T {1.0};
```

遇到出现歧义的情况，需要手动指定目标类型或者目标序号

```cpp
std::variant<int, float> T1 { std::in_place_index<0>, 10.5 };
std::variant<int, float> T2 { std::in_place_type<int>, 10.5 };
```

### std::construct_at

是 C++20 引入的引入的标准库函数，用于在指定的内存地址构造对。它是为了在新的内存模型中提供一种安全的对象构造方法，而不需要使用传统的 `placement new`

```
template <class T, class... Args>
constexpr T* construct_at(T* location, Args&&... args);
```

- `location` 表示构造对象的地址
- `args` 表示构造对象的参数，可以是任意数量和类型

```cpp
#include <iostream>
#include <memory>

class A
{
public:
    A(int InX)
    {
        X = InX;
    }
    
    int GetX() { return X; }

private:    
    int X {0};
};

int main()
{
    alignas(A) std::byte ptr_[sizeof(A)];
    A& result = *std::construct_at(reinterpret_cast<A*>(ptr_), 10);
    
    std::cout << result.GetX() << std::endl;
    
    return 0;
}
```

`alignas(A)` 是在C++中用于指定内存对齐方式的一个语法。它的主要作用是确保变量在内存中的起始地址符合特定类型的对齐要求

`alignas(A)` 表示 `ptr_` 的起始地址应该满足 `A` 的对齐要求

> 如果 `A` 类型有特定的对齐要求，而你在未对齐的内存位置上构造 `A` 的对象，可能会导致未定义行为。这是因为很多平台在访问未对齐的数据时可能会抛出硬件异常

所以在任何场合都推荐使用 `alignas` 来定义正确对齐

